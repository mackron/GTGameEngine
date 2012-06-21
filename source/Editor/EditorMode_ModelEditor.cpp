
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
        cameraNode.MoveForward(-10.0f);

        auto model = modelNode.AddModelComponent(ModelLibrary::LoadFromFile("engine/models/default.dae"))->GetModel();
        model->meshes[0]->SetMaterial(GTEngine::MaterialLibrary::Create("engine/materials/default.material"));


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
        GTEngine::MaterialLibrary::Delete(model->meshes[0]->GetMaterial());
        GTEngine::ModelLibrary::Delete(model);
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

                // Doing the rotation this way allows us to keep the up axis constant.
                this->cameraNode.SetOrientation(glm::quat());
                this->cameraNode.RotateY(this->cameraYRotation);
                this->cameraNode.RotateX(this->cameraXRotation);
            }

            this->scene.Update(deltaTimeInSeconds);
        }
    }

    void EditorMode_ModelEditor::OnSwapRCQueues()
    {
        this->renderer.OnSwapRCQueues();
    }
}

