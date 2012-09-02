
#include <GTEngine/Editor/EditorMode_ModelEditor.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Math.hpp>
#include <GTEngine/Logging.hpp>
#include <GTCore/Path.hpp>

namespace GTEngine
{
    EditorMode_ModelEditor::EditorMode_ModelEditor(Editor &editor)
        : EditorMode(editor),
          editor(editor), GUI(), scene(), viewport(0, 0), renderer(),
          cameraNode(), modelNode(),
          convexHullParentNode(), convexHullNodes(),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          viewportEventHandler(editor.GetGame(), viewport),
          random()
    {
        cameraNode.Add3DCameraComponent(90.0f, static_cast<float>(16.0f) / static_cast<float>(9.0f), 0.1f, 1000.0f);
        cameraNode.AddDirectionalLightComponent(0.5f, 0.5f, 0.5f);
        cameraNode.AddAmbientLightComponent(0.25f, 0.25f, 0.25f);

        // The model node will need a model component attached to it.
        this->modelNode.AddComponent<GTEngine::ModelComponent>();
        
        // Here we position the camera in it's default position.
        this->ResetCamera();


        // Here we setup the viewport.
        this->renderer.SetClearColour(0.5f, 0.5f, 0.5f);
        this->renderer.EnableColourClears();

        this->viewport.SetRenderer(&this->renderer);
        this->viewport.SetCameraNode(&this->cameraNode);

        this->scene.AddViewport(this->viewport);

        this->scene.AddSceneNode(this->cameraNode);
        this->scene.AddSceneNode(this->modelNode);


        this->convexHullParentNode.Hide();
        this->scene.AddSceneNode(this->convexHullParentNode);
    }

    EditorMode_ModelEditor::~EditorMode_ModelEditor()
    {
        GTEngine::ModelLibrary::Delete(this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel());
    }

    bool EditorMode_ModelEditor::Startup(GTGUI::Server &guiServer)
    {
        this->GUI.Main    = guiServer.GetElementByID("ModelEditor");
        this->GUI.MenuBar = guiServer.GetElementByID("MenuBar_ModelEditor");

        this->GUI.ModelViewport = guiServer.GetElementByID("ModelViewport");
        if (this->GUI.ModelViewport != nullptr)
        {
            this->GUI.ModelViewport->AttachEventHandler(this->viewportEventHandler);

            this->LoadModel("engine/models/default.dae");
        }
        else
        {
            return false;
        }

        return true;
    }

    bool EditorMode_ModelEditor::LoadModel(const char* fileName)
    {
        // We first try to load the new model. Only if it succeeds do we want to unload the previous model.
        GTCore::Timer timer;
        auto newModel = GTEngine::ModelLibrary::LoadFromFile(fileName);

        printf("Load Time: %f\n", timer.GetTimeSinceLastUpdate());

        if (newModel != nullptr)
        {
            // The previous model needs to be unloaded.
            auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
            if (model != nullptr)
            {
                this->DeleteConvexHulls();
                GTEngine::ModelLibrary::Delete(model);
            }

            this->modelNode.GetComponent<GTEngine::ModelComponent>()->SetModel(newModel);
            this->modelNode.GetScene()->RefreshObject(this->modelNode);

            // Here we need to update the build settings GUI.
            this->UpdateConvexDecompositionBuildSettings(newModel->definition.convexHullBuildSettings);

            return true;
        }

        return false;
    }

    bool EditorMode_ModelEditor::SaveModel(const char* fileNameIn)
    {
        auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
        if (model != nullptr)
        {
            return ModelLibrary::WriteToFile(*model, fileNameIn);
        }

        return false;
    }


    bool EditorMode_ModelEditor::SetMaterial(int index, const char* fileName)
    {
        auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
        if (model != nullptr)
        {
            if (index < static_cast<int>(model->meshes.count))
            {
                model->meshes[index]->SetMaterial(fileName);
            }
        }

        return false;
    }

    
    void EditorMode_ModelEditor::PlayAnimation()
    {
        auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
        if (model != nullptr)
        {
            AnimationSequence sequence;
            sequence.AddFrame(0, model->animation.GetKeyFrameCount() - 1, 0.0, true);

            model->PlayAnimation(sequence);
        }
    }

    void EditorMode_ModelEditor::StopAnimation()
    {
        auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
        if (model != nullptr)
        {
            model->PauseAnimation();
        }
    }


    void EditorMode_ModelEditor::ResetCamera()
    {
        this->cameraNode.SetPosition(0.0f, 0.0f, 10.0f);
        this->SetCameraRotation(0.0f, 0.0f);
    }

    void EditorMode_ModelEditor::SetCameraRotation(float xRotation, float yRotation)
    {
        this->cameraXRotation = xRotation;
        this->cameraYRotation = yRotation;
        this->ApplyCameraRotation();
    }



    void EditorMode_ModelEditor::ShowConvexDecomposition()
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
                        hullModel->meshes[0]->GetMaterial()->SetParameter("DiffuseColour", random.Next<float>(0.0f, 1.0f), random.Next<float>(0.0f, 1.0f), random.Next<float>(0.0f, 1.0f));

                        auto node = new SceneNode;
                        node->AddModelComponent(hullModel);

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

    void EditorMode_ModelEditor::HideConvexDecomposition()
    {
        this->convexHullParentNode.Hide();
        this->modelNode.Show();
    }

    void EditorMode_ModelEditor::BuildConvexDecomposition(ConvexHullBuildSettings &settings)
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



    void EditorMode_ModelEditor::OnActivate()
    {
        if (this->GUI.Main != nullptr)
        {
            this->GUI.Main->Show();
        }

        if (this->GUI.MenuBar != nullptr)
        {
            this->GUI.MenuBar->Show();
        }
    }

    void EditorMode_ModelEditor::OnDeactivate()
    {
        if (this->GUI.Main != nullptr)
        {
            this->GUI.Main->Hide();
        }

        if (this->GUI.MenuBar != nullptr)
        {
            this->GUI.MenuBar->Hide();
        }
    }

    void EditorMode_ModelEditor::OnUpdate(double deltaTimeInSeconds)
    {
        if (this->GUI.ModelViewport != nullptr && this->GUI.ModelViewport->IsVisible())
        {
            auto &game = this->GetGame();       // <-- For ease of use.

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
                        this->cameraNode.MoveUp(-mouseOffsetY * moveSpeed);
                        this->cameraNode.MoveRight(mouseOffsetX * moveSpeed);
                    }
                    else
                    {
                        this->cameraNode.MoveForward(-mouseOffsetY * moveSpeed);
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

    void EditorMode_ModelEditor::OnSwapRCQueues()
    {
    }





    ////////////////////////////////////////////////////////////////////
    // Private.

    void EditorMode_ModelEditor::ApplyCameraRotation()
    {
        this->cameraNode.SetOrientation(glm::quat());
        this->cameraNode.RotateY(this->cameraYRotation);
        this->cameraNode.RotateX(this->cameraXRotation);
    }

    void EditorMode_ModelEditor::DeleteConvexHulls()
    {
        // We need to delete any existing convex hull nodes so that they are re-built to account for changes.
        for (size_t i = 0; i < this->convexHullNodes.count; ++i)
        {
            // TODO: We need to delete the model we created for this node.
            delete this->convexHullNodes[i];
        }
        this->convexHullNodes.Clear();
    }

    void EditorMode_ModelEditor::UpdateConvexDecompositionBuildSettings(const ConvexHullBuildSettings &settings)
    {
        auto &game = this->GetGame();

        GTCore::String scriptString;
        scriptString.AssignFormatted
        (
            "spinModelEditor_CD_CompacityWeight:GetValue(%f);"
            "spinModelEditor_CD_VolumeWeight:SetValue(%f);"
            "spinModelEditor_CD_MinClusters:SetValue(%d);"
            "spinModelEditor_CD_VerticesPerCH:SetValue(%d);"
            "spinModelEditor_CD_Concavity:SetValue(%f);"
            "spinModelEditor_CD_SmallThreshold:SetValue(%f);"
            "spinModelEditor_CD_ConnectedDistance:SetValue(%f);"
            "spinModelEditor_CD_SimplifiedTriangleCount:SetValue(%d);"
            "checkModelEditor_CD_AddExtraDistPoints:SetChecked(%s);"
            "checkModelEditor_CD_AddFacePoints:SetChecked(%s);",

            settings.compacityWeight,
            settings.volumeWeight,
            settings.minClusters,
            settings.verticesPerCH,
            settings.concavity,
            settings.smallClusterThreshold,
            settings.connectedComponentsDist,
            settings.simplifiedTriangleCountTarget,
            settings.addExtraDistPoints ? "true" : "false",
            settings.addFacesPoints     ? "true" : "false"
        );

        game.ExecuteScript(scriptString.c_str());
    }
}

