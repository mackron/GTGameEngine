// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/MaterialEditor/MaterialEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/IO.hpp>

namespace GTEngine
{
    MaterialEditor::MaterialEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          scene(), viewport(), camera(), modelNode(),
          mainElement(nullptr), viewportElement(nullptr), viewportEventHandler(ownerEditor.GetGame(), this->viewport)
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


        // We'll use the default cube by default.
        this->modelNode.AddComponent<ModelComponent>()->SetModel("engine/models/default.dae");


        // We'll load the material here. What we want to do is pass an absolute path, which will in turn require us to specify the base part of the path that would be used to make it relative.
        GTCore::String basePath = GTEngine::IO::GetBasePath(absolutePath, relativePath);
        this->material = MaterialLibrary::Create(absolutePath);
        
        // Now we apply the material to the model.
        auto model = this->modelNode.GetComponent<ModelComponent>()->GetModel();
        if (model != nullptr)
        {
            for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
            {
                model->meshes[iMesh]->SetMaterial(this->material);
            }
        }



        auto &gui    = this->GetGUI();
        auto &script = this->GetScript();

        this->mainElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='material-editor' />");
        assert(this->mainElement != nullptr);
        {
            // The main element is the ModelEditor element. We need to pass 'this' as the '_internalPtr' argument.
            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("MaterialEditor");
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


        // Here we'll add the appropriate scene nodes to the preview scene.
        this->scene.AddSceneNode(this->camera);
        this->scene.AddSceneNode(this->modelNode);
        
        // Now position the camera.
        this->ResetCamera();
    }

    MaterialEditor::~MaterialEditor()
    {
        // Material.
        MaterialLibrary::Delete(this->material);

        // GUI elements.
        this->GetGUI().DeleteElement(this->mainElement);
    }


    void MaterialEditor::ResetCamera()
    {
        this->camera.SetPosition(5.0f, 2.5f, 5.0f);
        this->camera.LookAt(0.0f, 0.0f, 0.0f);

        glm::vec3 cameraRotation = glm::eulerAngles(this->camera.GetWorldOrientation());
        this->cameraXRotation = cameraRotation.x;
        this->cameraYRotation = cameraRotation.y;
        this->ApplyCameraRotation();
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void MaterialEditor::Show()
    {
        this->mainElement->Show();
    }

    void MaterialEditor::Hide()
    {
        this->mainElement->Hide();
    }

    bool MaterialEditor::Save()
    {
        return true;
    }

    void MaterialEditor::OnUpdate(double deltaTimeInSeconds)
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

    void MaterialEditor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
    }





    ///////////////////////////////////////////////////
    // Private Methods.

    void MaterialEditor::ApplyCameraRotation()
    {
        this->camera.SetOrientation(glm::quat());
        this->camera.RotateY(this->cameraYRotation);
        this->camera.RotateX(this->cameraXRotation);
    }
}