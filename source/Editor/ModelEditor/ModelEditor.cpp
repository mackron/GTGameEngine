// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/ModelEditor/ModelEditor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/MaterialLibrary.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    ModelEditor::ModelEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          scene(), viewport(), camera(),
          modelNode(), convexHullParentNode(), convexHullNodes(),
          mainElement(nullptr), viewportElement(nullptr), viewportEventHandler(ownerEditor.GetGame(), viewport),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          random()
    {
        // We use the camera for our lights.
        this->camera.AddComponent<GTEngine::CameraComponent>();
        this->camera.AddComponent<GTEngine::DirectionalLightComponent>()->SetColour(0.25f, 0.25f, 0.25f);
        this->camera.AddComponent<GTEngine::AmbientLightComponent>()->SetColour(0.6f, 0.6f, 0.6f);

        // Viewport and Renderer.
        this->viewport.SetCameraNode(this->camera);
        this->scene.AddViewport(this->viewport);
        this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);


        // HDR should be disabled for this.
        static_cast<DefaultSceneRenderer &>(this->scene.GetRenderer()).DisableHDR();


        // We need to ensure the model node has a model component. What we want to do is pass an absolute path, which will in turn
        // require us to specify the base part of the path that would be used to make it relative.
        GTCore::String basePath = GTEngine::IO::GetBasePath(absolutePath, relativePath);
        this->modelNode.AddComponent<GTEngine::ModelComponent>()->SetModel(absolutePath, basePath.c_str());


        auto &gui    = this->GetGUI();
        auto &script = this->GetScript();

        this->mainElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='model-editor-main' />");
        assert(this->mainElement != nullptr);
        {
            // The main element is the ModelEditor element. We need to pass 'this' as the '_internalPtr' argument.
            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("ModelEditor");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);       // 'self'
                    script.Push(this);          // '_internalPtr'
                    script.Call(2, 0);

                    // At this point we will have the scene editor initialized in the scripting environment, and is sitting at the top of the stack. So now we
                    // need to retrieve pointers to some of it's objects.
                    script.Push("Viewport");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.Push("GetID");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);   // <-- 'self'
                            script.Call(1, 1);
                            assert(script.IsString(-1));
                            {
                                this->viewportElement = gui.GetElementByID(script.ToString(-1));

                                this->viewportElement->AttachEventHandler(this->viewportEventHandler);
                                this->viewportElement->OnSize();
                            }
                            script.Pop(1);          // <-- return value from GetID()
                        }
                    }
                    script.Pop(1);
                }
            }
            script.Pop(1);
        }






        // Convex hull.
        this->convexHullParentNode.Hide();
        this->scene.AddSceneNode(this->convexHullParentNode);


        // Now we add the scene nodes to the scene.
        this->scene.AddSceneNode(this->camera);
        this->scene.AddSceneNode(this->modelNode);


        // We'll position the camera properly by just resetting it.
        this->ResetCamera();
    }

    ModelEditor::~ModelEditor()
    {
        this->GetGUI().DeleteElement(this->mainElement);

        // Convex hulls need to be deleted.
        this->DeleteConvexHulls();
    }



    void ModelEditor::ResetCamera()
    {
        this->camera.SetPosition(0.0f, 0.0f, 10.0f);
        this->SetCameraRotation(0.0f, 0.0f);
    }

    void ModelEditor::SetCameraRotation(float xRotation, float yRotation)
    {
        this->cameraXRotation = xRotation;
        this->cameraYRotation = yRotation;
        this->ApplyCameraRotation();
    }

    void ModelEditor::GetMaterials(GTCore::Vector<GTCore::String> &materialsOut)
    {
        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                auto &definition = model->GetDefinition();

                for (size_t i = 0; i < definition.meshMaterials.count; ++i)
                {
                    materialsOut.PushBack(definition.meshMaterials[i]->GetDefinition().relativePath);
                }
            }
        }
    }

    void ModelEditor::GetConvexHullBuildSettings(ConvexHullBuildSettings &settingsOut)
    {
        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                settingsOut = model->GetDefinition().convexHullBuildSettings;
            }
        }
    }

    void ModelEditor::PlayAnimation()
    {
        auto modelComponent = this->modelNode.GetComponent<GTEngine::ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                AnimationSequence sequence;
                sequence.AddFrame(0, model->animation.GetKeyFrameCount(), 0.0f, true);      // '0.0f' is the transition time. 'true' says to loop.

                model->PlayAnimation(sequence);
            }
        }
    }

    void ModelEditor::StopAnimation()
    {
        auto modelComponent = this->modelNode.GetComponent<GTEngine::ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                model->StopAnimation();
            }
        }
    }

    bool ModelEditor::SetMaterial(size_t index, const char* relativePath)
    {
        auto modelComponent = this->modelNode.GetComponent<GTEngine::ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                if (model->meshes[index]->SetMaterial(relativePath))
                {
                    // This is painful, but we're going to cheat here and do a const_cast so we can modify the model's definition to hold the new base material.
                    auto &definition = const_cast<ModelDefinition &>(model->GetDefinition());

                    MaterialLibrary::Delete(definition.meshMaterials[index]);
                    definition.meshMaterials[index] = MaterialLibrary::CreateCopy(*model->meshes[index]->GetMaterial());

                    return true;
                }
            }
        }

        return false;
    }


    void ModelEditor::ShowConvexDecomposition()
    {
        // If we have content in our convex hull scene node list, it means we don't have to create anything.
        if (this->convexHullNodes.count == 0)
        {
            auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
            if (model != nullptr)
            {
                auto &convexHulls = model->GetConvexHulls();
                for (size_t i = 0; i < convexHulls.count; ++i)
                {
                    auto hull = convexHulls[i];
                    if (hull != nullptr)
                    {
                        auto hullModel = ModelLibrary::CreateFromConvexHull(*hull);
                        hullModel->meshes[0]->GetMaterial()->SetParameter("DiffuseColour", this->random.Next<float>(0.0f, 1.0f), this->random.Next<float>(0.0f, 1.0f), this->random.Next<float>(0.0f, 1.0f));

                        auto node = new SceneNode;
                        node->AddComponent<GTEngine::ModelComponent>()->SetModel(hullModel);

                        this->convexHullNodes.PushBack(node);

                        this->convexHullParentNode.AttachChild(*node);
                    }
                }
            }
        }

        // Now we just show the main scene node.
        this->convexHullParentNode.Show();
        this->modelNode.Hide();
    }

    void ModelEditor::HideConvexDecomposition()
    {
        this->convexHullParentNode.Hide();
        this->modelNode.Show();
    }

    void ModelEditor::BuildConvexDecomposition(ConvexHullBuildSettings &settings)
    {
        // The old convex hulls need to be deleted.
        this->DeleteConvexHulls();

        // We build the convex decomposition on the definition directly.
        auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
        if (model != nullptr)
        {
            // We're going to be naughty here and do a const_cast so we can build the convex hulls.
            auto &definition = const_cast<ModelDefinition &>(model->GetDefinition());
            definition.BuildConvexDecomposition(settings);
        }



        if (this->convexHullParentNode.IsVisible())
        {
            this->ShowConvexDecomposition();
        }
    }


    ///////////////////////////////////////////////////
    // Virtual Methods.

    void ModelEditor::Show()
    {
        this->mainElement->Show();
    }

    void ModelEditor::Hide()
    {
        this->mainElement->Hide();
    }

    bool ModelEditor::Save()
    {
        bool wasSaved = ModelLibrary::WriteToFile(this->GetAbsolutePath());

        if (wasSaved)
        {
            this->UnmarkAsModified();
        }

        return wasSaved;
    }

    void ModelEditor::OnUpdate(double deltaTimeInSeconds)
    {
        if (this->viewportElement->IsVisible())
        {
            auto &game = this->GetOwnerEditor().GetGame();       // <-- For ease of use.

            // If the mouse is captured we may need to move the screen around.
            if (game.IsMouseCaptured())
            {
                const float moveSpeed   = 0.05f;
                const float rotateSpeed = 0.1f;

                float mouseOffsetX;
                float mouseOffsetY;
                game.GetSmoothedMouseOffset(mouseOffsetX, mouseOffsetY);

                if (game.IsMouseButtonDown(GTCore::MouseButton_Left))
                {
                    if (game.IsMouseButtonDown(GTCore::MouseButton_Right))
                    {
                        this->camera.MoveUp(  -mouseOffsetY * moveSpeed);
                        this->camera.MoveRight(mouseOffsetX * moveSpeed);
                    }
                    else
                    {
                        this->camera.MoveForward(-mouseOffsetY * moveSpeed);
                        this->cameraYRotation += -mouseOffsetX * rotateSpeed;
                    }
                }
                else
                {
                    if (game.IsMouseButtonDown(GTCore::MouseButton_Right))
                    {
                        this->cameraXRotation += -mouseOffsetY * rotateSpeed;
                        this->cameraYRotation += -mouseOffsetX * rotateSpeed;
                    }
                }

                this->ApplyCameraRotation();
            }

            this->scene.Update(deltaTimeInSeconds);
        }
    }


    void ModelEditor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
        if (item.info.absolutePath == this->GetAbsolutePath() || (item.info.absolutePath + ".gtmodel") == this->GetAbsolutePath())
        {
            this->Refresh();
        }
    }





    ///////////////////////////////////////////////////
    // Private Methods.

    void ModelEditor::ApplyCameraRotation()
    {
        this->camera.SetOrientation(glm::quat());
        this->camera.RotateY(this->cameraYRotation);
        this->camera.RotateX(this->cameraXRotation);
    }

    void ModelEditor::DeleteConvexHulls()
    {
        // We need to delete any existing convex hull nodes so that they are re-built to account for changes.
        for (size_t i = 0; i < this->convexHullNodes.count; ++i)
        {
            auto node = this->convexHullNodes[i];
            assert(node != nullptr);
            {
                auto modelComponent = node->GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);
                {
                    auto model = modelComponent->GetModel();
                    assert(model != nullptr);
                    {
                        ModelLibrary::Delete(model);
                    }
                }

                delete node;
            }
        }
        this->convexHullNodes.Clear();
    }

    void ModelEditor::Refresh()
    {
        // We need to let the scene know that the model has changed. We just call OnChanged() on the component for this. If we don't do this, the
        // scene will not know that it needs to update the frustum culling volume.
        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        assert(modelComponent != nullptr);
        {
            modelComponent->OnChanged();
        }


        // GTGUI.Server.GetElementByID(this->mainElement->id):Refresh()
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("Refresh");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);       // 'self'
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
