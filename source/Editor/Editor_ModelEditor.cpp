
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/ModelLibrary.hpp>

namespace GTEngine
{
    Editor_ModelEditor::Editor_ModelEditor(Editor &editor)
        : editor(editor),
          scene(), viewport(), camera(), renderer(),
          modelNode(),
          GUI(), viewportEventHandler(editor.GetGame(), viewport),
          cameraXRotation(), cameraYRotation(),
          currentState(nullptr), loadedStates()
    {
        this->viewport.SetRenderer(this->renderer);
        this->viewport.SetCameraNode(this->camera);

        this->scene.AddViewport(this->viewport);


        // The viewport should clear the background.
        this->renderer.SetClearColour(0.5f, 0.5f, 0.5f);
        this->renderer.EnableColourClears();


        this->ResetCamera();
    }

    Editor_ModelEditor::~Editor_ModelEditor()
    {
        // All of our state objects need to be deleted.
        for (size_t i = 0; i < this->loadedStates.count; ++i)
        {
            delete this->loadedStates.buffer[i]->value;
        }
    }

    void Editor_ModelEditor::Startup()
    {
        auto &gui = this->editor.GetGame().GetGUI();

        this->GUI.Viewport = gui.GetElementByID("Editor_ModelEditor_Viewport");
        this->GUI.Viewport->AttachEventHandler(this->viewportEventHandler);


        // We use the camera for our lights.
        this->camera.AddComponent<GTEngine::CameraComponent>();
        this->camera.AddDirectionalLightComponent(0.5f, 0.5f, 0.5f);
        this->camera.AddAmbientLightComponent(0.25f, 0.25f, 0.25f);

        // We need to ensure the model node has a model component.
        this->modelNode.AddComponent<GTEngine::ModelComponent>();


        // Now we add the scene nodes to the scene.
        this->scene.AddSceneNode(this->camera);
        this->scene.AddSceneNode(this->modelNode);
    }

    bool Editor_ModelEditor::LoadModel(const char* fileName)
    {
        auto newModel = ModelLibrary::LoadFromFile(fileName);
        if (newModel != nullptr)
        {
            // The old model needs to be deleted.
            auto oldModel = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
            if (oldModel != nullptr)
            {
                ModelLibrary::Delete(oldModel);
            }


            // Now we need to set the new model, remembering to refresh the object.
            this->modelNode.GetComponent<GTEngine::ModelComponent>()->SetModel(newModel);
            this->modelNode.Refresh();


            // Here we need to make sure the state is saved.
            this->SaveState();



            // Now we need to setup some state for the new model. If the model has already had it's state recorded, we can just restore it from the map.
            auto iState = this->loadedStates.Find(fileName);
            if (iState != nullptr)
            {
                this->currentState = iState->value;
            }
            else
            {
                this->currentState = new State;
                this->loadedStates.Add(fileName, this->currentState);

                // TODO: Do some kind of automatic positioning based on the size of the bounding box.
                this->currentState->cameraPosition.z = 10.0f;
            }

            // Here we take 'currentState' and restore everything from that.
            this->RestoreState();

            return true;
        }

        return false;
    }



    void Editor_ModelEditor::ResetCamera()
    {
        this->camera.SetPosition(0.0f, 0.0f, 10.0f);
        this->SetCameraRotation(0.0f, 0.0f);
    }

    void Editor_ModelEditor::SetCameraRotation(float xRotation, float yRotation)
    {
        this->cameraXRotation = xRotation;
        this->cameraYRotation = yRotation;
        this->ApplyCameraRotation();
    }


    //////////////////////////////////
    // Events

    void Editor_ModelEditor::Update(double deltaTimeInSeconds)
    {
        if (this->GUI.Viewport != nullptr && this->GUI.Viewport->IsVisible())
        {
            auto &game = this->editor.GetGame();       // <-- For ease of use.

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
                        this->camera.MoveUp(-mouseOffsetY * moveSpeed);
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







    //////////////////////////////////////
    // Private

    void Editor_ModelEditor::ApplyCameraRotation()
    {
        this->camera.SetOrientation(glm::quat());
        this->camera.RotateY(this->cameraYRotation);
        this->camera.RotateX(this->cameraXRotation);
    }

    void Editor_ModelEditor::SaveState()
    {
        if (this->currentState != nullptr)
        {
            auto &script = this->editor.GetGame().GetScript();

            this->currentState->cameraPosition    = this->camera.GetPosition();
            this->currentState->cameraOrientation = this->camera.GetOrientation();
            this->currentState->cameraXRotation   = this->cameraXRotation;
            this->currentState->cameraYRotation   = this->cameraYRotation;

            this->currentState->showConvexDecomposition = script.GetBoolean("Editor_ModelEditor_ShowConvexDecomposition:IsChecked()");

            this->currentState->convexDecompositionSettings.compacityWeight               = script.GetFloat("Editor_ModelEditor_CD_CompacityWeight:GetValue()");
            this->currentState->convexDecompositionSettings.volumeWeight                  = script.GetFloat("Editor_ModelEditor_CD_VolumeWeight:GetValue()");
            this->currentState->convexDecompositionSettings.minClusters                   = static_cast<unsigned int>(script.GetInteger("Editor_ModelEditor_CD_MinClusters:GetValue()"));
            this->currentState->convexDecompositionSettings.verticesPerCH                 = static_cast<unsigned int>(script.GetInteger("Editor_ModelEditor_CD_VerticesPerCH:GetValue()"));
            this->currentState->convexDecompositionSettings.concavity                     = script.GetFloat("Editor_ModelEditor_CD_Concavity:GetValue()");
            this->currentState->convexDecompositionSettings.smallClusterThreshold         = script.GetFloat("Editor_ModelEditor_CD_SmallThreshold:GetValue()");
            this->currentState->convexDecompositionSettings.connectedComponentsDist       = script.GetFloat("Editor_ModelEditor_CD_ConnectedDistance:GetValue()");
            this->currentState->convexDecompositionSettings.simplifiedTriangleCountTarget = static_cast<unsigned int>(script.GetInteger("Editor_ModelEditor_CD_SimplifiedTriangleCount:GetValue()"));
            this->currentState->convexDecompositionSettings.addExtraDistPoints            = script.GetBoolean("Editor_ModelEditor_CD_AddExtraDistPoints:IsChecked()");
            this->currentState->convexDecompositionSettings.addFacesPoints                = script.GetBoolean("Editor_ModelEditor_CD_AddFacePoints:IsChecked()");
        }
    }

    void Editor_ModelEditor::RestoreState()
    {
        if (this->currentState != nullptr)
        {
            auto &script = this->editor.GetGame().GetScript();

            this->camera.SetPosition(this->currentState->cameraPosition);
            this->camera.SetOrientation(this->currentState->cameraOrientation);
            this->cameraXRotation = this->currentState->cameraXRotation;
            this->cameraYRotation = this->currentState->cameraYRotation;

            GTCore::String scriptString;
            scriptString.AssignFormatted("Editor_ModelEditor_ShowConvexDecomposition:SetChecked(%s);", this->currentState->showConvexDecomposition ? "true" : "false");
            script.Execute(scriptString.c_str());
            
            scriptString.AssignFormatted
            (
                "Editor_ModelEditor_CD_CompacityWeight:SetValue(%f);"
                "Editor_ModelEditor_CD_VolumeWeight:SetValue(%f);"
                "Editor_ModelEditor_CD_MinClusters:SetValue(%d);"
                "Editor_ModelEditor_CD_VerticesPerCH:SetValue(%d);"
                "Editor_ModelEditor_CD_Concavity:SetValue(%f);"
                "Editor_ModelEditor_CD_SmallThreshold:SetValue(%f);"
                "Editor_ModelEditor_CD_ConnectedDistance:SetValue(%f);"
                "Editor_ModelEditor_CD_SimplifiedTriangleCount:SetValue(%d);"
                "Editor_ModelEditor_CD_AddExtraDistPoints:SetChecked(%s);"
                "Editor_ModelEditor_CD_AddFacePoints:SetChecked(%s);",

                this->currentState->convexDecompositionSettings.compacityWeight,
                this->currentState->convexDecompositionSettings.volumeWeight,
                this->currentState->convexDecompositionSettings.minClusters,
                this->currentState->convexDecompositionSettings.verticesPerCH,
                this->currentState->convexDecompositionSettings.concavity,
                this->currentState->convexDecompositionSettings.smallClusterThreshold,
                this->currentState->convexDecompositionSettings.connectedComponentsDist,
                this->currentState->convexDecompositionSettings.simplifiedTriangleCountTarget,
                this->currentState->convexDecompositionSettings.addExtraDistPoints ? "true" : "false",
                this->currentState->convexDecompositionSettings.addFacesPoints     ? "true" : "false"
            );
            script.Execute(scriptString.c_str());
        }
    }
}