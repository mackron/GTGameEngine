// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/ModelEditor/ModelEditor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTCore/Path.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    ModelEditor::ModelEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          modelDefinition(), model(modelDefinition),
          scene(), viewport(), camera(),
          modelNode(), convexHullParentNode(), convexHullNodes(),
          mainElement(nullptr), viewportElement(nullptr), timelineElement(nullptr),
          viewportEventHandler(ownerEditor.GetGame(), viewport),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          grid(0.25f, 8, 32),
          random(),
          isSaving(false), isReloading(false)
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

        // Show the grid.
        this->grid.Show(this->scene.GetRenderer());


        // Load the model. If it needs serialization, we'll also do that. This mimicks the behaviour of the model library.
        bool needsSerialize;
        if (this->modelDefinition.LoadFromFile(absolutePath, relativePath, needsSerialize))
        {
            if (needsSerialize)
            {
                ModelLibrary::WriteToFile(this->modelDefinition);
            }
        }

        // Refresh the model to have it show the current state of the model definition.
        this->model.OnDefinitionChanged();

        // Add a model component to the scene node.
        this->modelNode.AddComponent<GTEngine::ModelComponent>()->SetModel(this->model);


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

                    // Timeline Element
                    script.Push("Timeline");
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
                                this->timelineElement = gui.GetElementByID(script.ToString(-1));
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
        // The camera needs to be positioned based on the bounding volume. We will position the camera at the +Z/+X edge of a scaled bounding volume.
        glm::vec3 aabbMin;
        glm::vec3 aabbMax;

        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        assert(modelComponent != nullptr);
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                model->GetBaseAABB(aabbMin, aabbMax);

                // Center point (camera look-at target).
                glm::vec3 cameraLookAtTarget = (aabbMin + aabbMax) * 0.5f;


                // Camera position.
                glm::vec3 aabbSize = aabbMax - aabbMin;
                float maxDimension = glm::max(aabbSize.x, glm::max(aabbSize.y, aabbSize.z));

                glm::vec3 cameraPosition = cameraLookAtTarget;
                cameraPosition += glm::normalize(glm::vec3(1.0f, 0.5f, 1.0f)) * maxDimension * 2.0f;



                // Position and rotate the camera.
                this->camera.SetPosition(cameraPosition);
                this->camera.LookAt(cameraLookAtTarget);
            }
            else
            {
                this->camera.SetPosition(10.0f, 5.0f, 10.0f);
                this->camera.LookAt(0.0f, 0.0f, 0.0f);
            }
        }


        glm::vec3 cameraRotation = glm::eulerAngles(this->camera.GetWorldOrientation());
        this->cameraXRotation = cameraRotation.x;
        this->cameraYRotation = cameraRotation.y;
        this->ApplyCameraRotation();
    }


    ModelDefinition & ModelEditor::GetModelDefinition()
    {
        return this->modelDefinition;
    }

    void ModelEditor::RefreshViewport()
    {
        this->model.OnDefinitionChanged();
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


    void ModelEditor::AddNewAnimationSegment(const char* name, size_t startIndex, size_t endIndex)
    {
        this->modelDefinition.animation.AddNamedSegment(name, startIndex, endIndex);
        this->MarkAsModified();
    }

    void ModelEditor::RemoveAnimationSegmentByIndex(size_t index)
    {
        this->modelDefinition.animation.RemoveSegmentByIndex(index);
        this->MarkAsModified();
    }

    void ModelEditor::SetAnimationSegmentName(size_t index, const char* newName)
    {
        auto segment = this->modelDefinition.animation.GetNamedSegmentByIndex(index);
        if (segment != nullptr)
        {
            segment->name = newName;
            this->MarkAsModified();
        }
    }

    const char* ModelEditor::GetAnimationSegmentName(size_t index) const
    {
        auto segment = this->modelDefinition.animation.GetNamedSegmentByIndex(index);
        if (segment != nullptr)
        {
            return segment->name.c_str();
        }

        return nullptr;
    }

    void ModelEditor::SetAnimationSegmentFrameIndices(size_t segmentIndex, size_t startIndex, size_t endIndex)
    {
        auto segment = this->modelDefinition.animation.GetNamedSegmentByIndex(segmentIndex);
        if (segment != nullptr)
        {
            segment->startKeyFrame = startIndex;
            segment->endKeyFrame   = endIndex;

            this->MarkAsModified();
        }
    }

    void ModelEditor::GetAnimationSegmentFrameIndices(size_t segmentIndex, size_t &startIndex, size_t &endIndex) const
    {
        auto segment = this->modelDefinition.animation.GetNamedSegmentByIndex(segmentIndex);
        if (segment != nullptr)
        {
            startIndex = segment->startKeyFrame;
            endIndex   = segment->endKeyFrame;
        }
    }


    void ModelEditor::PlayAnimationSegmentByIndex(size_t segmentIndex)
    {
        auto segment = this->modelDefinition.animation.GetNamedSegmentByIndex(segmentIndex);
        if (segment != nullptr)
        {
            AnimationSequence sequence;
            sequence.AddFrame(segment->startKeyFrame, segment->endKeyFrame, 0.0, true);

            this->model.PlayAnimation(sequence);
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
        auto newMaterial = MaterialLibrary::Create(relativePath);
        if (newMaterial)
        {
            MaterialLibrary::Delete(this->modelDefinition.meshMaterials[index]);
            this->modelDefinition.meshMaterials[index] = newMaterial;

            this->RefreshViewport();

            return true;
        }

        return false;
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
        this->modelDefinition.BuildConvexDecomposition(settings);


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
        bool wasSaved = false;

        this->isSaving = true;
        {
            wasSaved = ModelLibrary::WriteToFile(this->modelDefinition);

            if (wasSaved)
            {
                this->UnmarkAsModified();
            }


            // We want to immediatly force the game to check for changes so that the model is immediately reloaded.
            auto &dataFilesWatcher = this->GetOwnerEditor().GetGame().GetDataFilesWatcher();
            dataFilesWatcher.CheckForChanges(false);
            dataFilesWatcher.DispatchEvents();
        }
        this->isSaving = false;

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
        if (!this->isSaving)
        {
            if (item.info.absolutePath == this->GetAbsolutePath())
            {
                this->Reload();
            }
            else
            {
                if (GTCore::Path::ExtensionEqual(item.info.absolutePath.c_str(), "gtmodel") && GTCore::IO::RemoveExtension(item.info.absolutePath.c_str()) == this->GetAbsolutePath())
                {
                    this->Reload();
                }
            }
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
        this->RefreshViewport();

        // We need to let the scene know that the model has changed. We just call OnChanged() on the component for this. If we don't do this, the
        // scene will not know that it needs to update the frustum culling volume.
        auto modelComponent = this->modelNode.GetComponent<ModelComponent>();
        assert(modelComponent != nullptr);
        {
            modelComponent->OnChanged();
        }



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

    void ModelEditor::Reload()
    {
        this->isReloading = true;
        {
            bool needsSerialize;
            if (this->modelDefinition.LoadFromFile(this->modelDefinition.absolutePath.c_str(), this->modelDefinition.relativePath.c_str(), needsSerialize))
            {
                if (needsSerialize && !this->IsMarkedAsModified())
                {
                    ModelLibrary::WriteToFile(this->modelDefinition);
                }
            }


            this->Refresh();
        }
        this->isReloading = false;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
