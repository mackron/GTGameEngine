
#include <GTEngine/Editor/EditorMode_ModelEditor.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Math.hpp>

namespace GTEngine
{
    EditorMode_ModelEditor::EditorMode_ModelEditor(Editor &editor)
        : EditorMode(editor),
          editor(editor), GUI(), scene(), viewport(0, 0), renderer(),
          cameraNode(), modelNode(),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          viewportEventHandler(editor.GetGame(), viewport)
    {
        cameraNode.Add3DCameraComponent(90.0f, static_cast<float>(16.0f) / static_cast<float>(9.0f), 0.1f, 1000.0f);
        cameraNode.AddDirectionalLightComponent(0.5f, 0.5f, 0.5f);
        cameraNode.AddAmbientLightComponent(0.25f, 0.25f, 0.25f);

        // The model node will need a model component attached to it.
        this->modelNode.AddComponent<GTEngine::ModelComponent>();
        
        // Here we position the camera in it's default position.
        this->ResetCamera();


        // Here we setup the viewport.
        this->renderer.SetClearColour(0.1f, 0.1f, 0.1f);
        this->renderer.EnableColourClears();

        this->viewport.SetRenderer(&this->renderer);
        this->viewport.SetCameraNode(&this->cameraNode);

        this->scene.AddViewport(this->viewport);

        this->scene.AddSceneNode(this->cameraNode);
        this->scene.AddSceneNode(this->modelNode);
    }

    EditorMode_ModelEditor::~EditorMode_ModelEditor()
    {
        auto model = modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
        if (model != nullptr)
        {
            GTEngine::MaterialLibrary::Delete(model->meshes[0]->GetMaterial());
            GTEngine::ModelLibrary::Delete(model);
        }
    }

    bool EditorMode_ModelEditor::Startup(GTGUI::Server &guiServer)
    {
        this->GUI.Main    = guiServer.GetElementByID("ModelEditor");
        this->GUI.MenuBar = guiServer.GetElementByID("MenuBar_ModelEditor");

        this->GUI.ModelViewport = guiServer.GetElementByID("ModelViewport");
        if (this->GUI.ModelViewport != nullptr)
        {
            this->GUI.ModelViewport->AttachEventHandler(this->viewportEventHandler);
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
        auto newModel = GTEngine::ModelLibrary::LoadFromFile(fileName);
        if (newModel != nullptr)
        {
            // The previous model needs to be unloaded.
            auto model = this->modelNode.GetComponent<GTEngine::ModelComponent>()->GetModel();
            if (model != nullptr)
            {
                GTEngine::ModelLibrary::Delete(model);
            }


            // Now we assign default materials. This will need to change for .gtmodel files, which will use their own local copy of the materials.
            if (newModel != nullptr)
            {
                for (size_t i = 0; i < newModel->meshes.count; ++i)
                {
                    newModel->meshes[i]->SetMaterial(GTEngine::MaterialLibrary::Create("engine/materials/simple-diffuse.material"));
                }
            }

            this->modelNode.GetComponent<GTEngine::ModelComponent>()->SetModel(newModel);

            return true;
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
                // We need to try loading the material before setting it.
                auto material = GTEngine::MaterialLibrary::Create(fileName);
                if (material != nullptr)
                {
                    model->meshes[index]->SetMaterial(material);
                    return true;
                }
            }
        }

        return false;
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
        this->renderer.OnSwapRCQueues();
    }





    ////////////////////////////////////////////////////////////////////
    // Private.

    void EditorMode_ModelEditor::ApplyCameraRotation()
    {
        this->cameraNode.SetOrientation(glm::quat());
        this->cameraNode.RotateY(this->cameraYRotation);
        this->cameraNode.RotateX(this->cameraXRotation);
    }
}

