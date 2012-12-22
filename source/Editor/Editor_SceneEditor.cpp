
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>

namespace GTEngine
{
    Editor_SceneEditor::Editor_SceneEditor(Editor &editorIn)
        : editor(editorIn), currentState(nullptr), loadedStates()
    {
    }

    Editor_SceneEditor::~Editor_SceneEditor()
    {
    }


    void Editor_SceneEditor::Startup()
    {
    }


    bool Editor_SceneEditor::LoadScene(const char* absolutePath)
    {
        auto &gui = this->editor.GetGame().GetGUI();

        auto iState = this->loadedStates.Find(absolutePath);
        if (iState != nullptr)
        {
            if (this->currentState != nullptr)
            {
                this->currentState->GUI.Main->Hide();
            }

            this->currentState = iState->value;
            this->currentState->GUI.Main->Show();
        }
        else
        {
            // We need to load the scene.
            auto file = GTCore::IO::Open(absolutePath, GTCore::IO::OpenMode::Write);
            if (file != nullptr)
            {
                auto &script = this->editor.GetGame().GetScript();

                // We need to now create the GUI elements for this particular file. We start with the main element.
                auto mainElement = gui.CreateElement("<div parentid='Editor_SceneEditor' styleclass='scene-editor-main' />");
                if (mainElement != nullptr)
                {
                    // The main element has been created, but we need to run a script to have it turn it into a proper SceneEditor object.
                    script.Execute(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s'):SceneEditor();", mainElement->id).c_str());

                    // We need to grab the viewport so we can attach an event handler to it.
                    auto viewportElement = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').Viewport:GetID();", mainElement->id).c_str()));
                    if (viewportElement != nullptr)
                    {
                        // Now we just need to create a new state object and make it the current one.
                        if (this->currentState != nullptr)
                        {
                            this->currentState->GUI.Main->Hide();
                        }

                        this->currentState = new State(*this);
                        this->currentState->GUI.Main     = mainElement;
                        this->currentState->GUI.Viewport = viewportElement;
                        this->loadedStates.Add(absolutePath, this->currentState);

                        viewportElement->AttachEventHandler(this->currentState->viewportEventHandler);

                        this->ResetCamera();
                    }
                    else
                    {
                        gui.DeleteElement(mainElement);
                        GTCore::IO::Close(file);
                        return false;
                    }
                }
                else
                {
                    GTCore::IO::Close(file);
                    return false;
                }
            }
            else
            {
                GTCore::IO::Close(file);
                return false;
            }
        }

        return true;
    }

    bool Editor_SceneEditor::SaveScene(const char* absolutePath)
    {
        auto iState = this->loadedStates.Find(absolutePath);
        if (iState != nullptr)
        {
            return true;
        }

        return false;
    }

    void Editor_SceneEditor::CloseScene(const char* absolutePath)
    {
        auto iState = this->loadedStates.Find(absolutePath);
        if (iState != nullptr)
        {
            this->editor.GetGame().GetGUI().DeleteElement(iState->value->GUI.Main);
            

            if (this->currentState == iState->value)
            {
                this->currentState = nullptr;
            }


            delete iState->value;
            this->loadedStates.Remove(absolutePath);
        }
    }


    void Editor_SceneEditor::ResetCamera()
    {
        if (this->currentState != nullptr)
        {
            this->currentState->camera.SetPosition(0.0f, 0.0f, 10.0f);
            this->SetCameraRotation(0.0f, 0.0f);
        }
    }

    void Editor_SceneEditor::SetCameraRotation(float xRotation, float yRotation)
    {
        if (this->currentState != nullptr)
        {
            this->currentState->cameraXRotation = xRotation;
            this->currentState->cameraYRotation = yRotation;
            this->ApplyCameraRotation();
        }
    }



    ////////////////////////////////////////////////
    // Events

    void Editor_SceneEditor::Update(double deltaTimeInSeconds)
    {
        if (this->currentState != nullptr)
        {
            if (this->currentState->GUI.Main->IsVisible())
            {
                auto &game = this->editor.GetGame();

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
                            this->currentState->camera.MoveUp(-mouseOffsetY * moveSpeed);
                            this->currentState->camera.MoveRight(mouseOffsetX * moveSpeed);
                        }
                        else
                        {
                            this->currentState->camera.MoveForward(-mouseOffsetY * moveSpeed);
                            this->currentState->cameraYRotation += -mouseOffsetX * rotateSpeed;
                        }
                    }
                    else
                    {
                        if (game.IsMouseButtonDown(GTCore::MouseButton_Right))
                        {
                            this->currentState->cameraXRotation += -mouseOffsetY * rotateSpeed;
                            this->currentState->cameraYRotation += -mouseOffsetX * rotateSpeed;
                        }
                    }

                    this->ApplyCameraRotation();
                }

                this->currentState->scene.Update(deltaTimeInSeconds);
            }
        }
    }




    ////////////////////////////////////////////////
    // Private

    void Editor_SceneEditor::ApplyCameraRotation()
    {
        if (this->currentState != nullptr)
        {
            this->currentState->camera.SetOrientation(glm::quat());
            this->currentState->camera.RotateY(this->currentState->cameraYRotation);
            this->currentState->camera.RotateX(this->currentState->cameraXRotation);
        }
    }




    ////////////////////////////////////////////////
    // State

    Editor_SceneEditor::State::State(Editor_SceneEditor &sceneEditorIn)
        : sceneEditor(sceneEditorIn),
          scene(), viewport(), camera(),
          viewportEventHandler(sceneEditor.GetEditor().GetGame(), viewport),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          GUI()
    {
        this->camera.AddComponent<GTEngine::CameraComponent>();
        this->camera.AddComponent<GTEngine::AmbientLightComponent>()->SetColour(0.0f, 0.0f, 0.0f);

        this->viewport.SetCameraNode(this->camera);
        this->scene.AddViewport(this->viewport);
        this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);


        this->scene.AddSceneNode(this->camera);
    }

    Editor_SceneEditor::State::~State()
    {
    }
}
