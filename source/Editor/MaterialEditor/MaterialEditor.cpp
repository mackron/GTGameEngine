// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/MaterialEditor/MaterialEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/IO.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    MaterialEditor::MaterialEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          scene(), viewport(), camera(), modelNode(),
          mainElement(nullptr), scriptTextBoxElement(nullptr), scriptTextBoxEventHandler(*this), viewportElement(nullptr), viewportEventHandler(ownerEditor.GetGame(), this->viewport),
          cameraXRotation(0.0f), cameraYRotation(0.0f), material(nullptr),
          isSaving(false), isReloading(false)
    {
        // We use the camera for our lights.
        this->camera.AddComponent<GTEngine::CameraComponent>();
        this->camera.AddComponent<GTEngine::DirectionalLightComponent>()->SetColour(0.4f, 0.4f, 0.4f);
        this->camera.AddComponent<GTEngine::AmbientLightComponent>()->SetColour(0.25f, 0.25f, 0.25f);

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
        this->material = MaterialLibrary::Create(absolutePath, basePath.c_str());

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


                    script.Push("ScriptTextBox");
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
                                this->scriptTextBoxElement = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').TextArea:GetID();", script.ToString(-1)).c_str()));
                                this->scriptTextBoxElement->SetText(this->material->GetDefinition().GetXMLString());

                                // We want to attach this after setting the initial text so that is isn't marked as modified.
                                this->scriptTextBoxElement->AttachEventHandler(this->scriptTextBoxEventHandler);
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
        this->camera.SetPosition(3.5f, 2.0f, 3.5f);
        this->camera.LookAt(0.0f, 0.0f, 0.0f);

        glm::vec3 cameraRotation = glm::eulerAngles(this->camera.GetWorldOrientation());
        this->cameraXRotation = cameraRotation.x;
        this->cameraYRotation = cameraRotation.y;
        this->ApplyCameraRotation();
    }



    ///////////////////////////////////////////////////
    // GUI Events.

    void MaterialEditor::OnScriptTextChanged()
    {
        if (!this->IsMarkedAsModified() && !this->isReloading)
        {
            this->MarkAsModified();
        }
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
        this->isSaving = true;
        {
            bool wasSaved = false;

            auto xmlString = this->scriptTextBoxElement->GetText();
            if (xmlString != nullptr)
            {
                wasSaved = GTCore::IO::OpenAndWriteTextFile(this->GetAbsolutePath(), xmlString);
            }
            else
            {
                wasSaved = GTCore::IO::OpenAndWriteTextFile(this->GetAbsolutePath(), "");
            }

            if (wasSaved)
            {
                this->UnmarkAsModified();

                // We want to immediatly force the game to check for changes so that the material is immediately reloaded.
                auto &dataFilesWatcher = this->GetOwnerEditor().GetGame().GetDataFilesWatcher();
                dataFilesWatcher.CheckForChanges(false);
                dataFilesWatcher.DispatchEvents();
            }
        }
        this->isSaving = false;

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
        if (GTCore::Strings::Equal(item.absolutePath.c_str(), this->GetAbsolutePath()))
        {
            if (!this->isSaving)
            {
                this->isReloading = true;
                {
                    // TODO: If our file is already marked as modified, we should ask if it should be reloaded. Don't want to lose changes.

                    this->scriptTextBoxElement->SetText(this->material->GetDefinition().GetXMLString());
                }
                this->isReloading = false;
            }
        }
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


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
