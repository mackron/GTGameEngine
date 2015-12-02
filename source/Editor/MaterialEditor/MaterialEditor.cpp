// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Editor/MaterialEditor/MaterialEditor.hpp>
#include <GTGE/Editor.hpp>
#include <GTGE/Game.hpp>
#include <GTGE/IO.hpp>
#include <GTGE/GTEngine.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GT
{
    MaterialEditor::MaterialEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          scene(), camera(), modelNode(),
          mainElement(nullptr), scriptTextBoxElement(nullptr), scriptTextBoxEventHandler(*this), viewportElement(nullptr), viewportEventHandler(ownerEditor.GetGame(), scene.GetDefaultViewport()),
          cameraXRotation(0.0f), cameraYRotation(0.0f), material(nullptr),
          isSaving(false), isReloading(false)
    {
        // We use the camera for our lights.
        this->camera.AddComponent<CameraComponent>()->Set3DProjection(90.0f, 16.0f / 9.0f, 0.1f, 100.0f);
        this->camera.AddComponent<DirectionalLightComponent>()->SetColour(0.4f, 0.4f, 0.4f);
        this->camera.AddComponent<AmbientLightComponent>()->SetColour(0.25f, 0.25f, 0.25f);

        // Viewport and Renderer.
        this->scene.SetDefaultViewportCamera(this->camera);
        this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);


        // HDR should be disabled for this.
        static_cast<DefaultSceneRenderer &>(this->scene.GetRenderer()).DisableHDR();


        // We'll use the default cube by default.
        this->modelNode.AddComponent<ModelComponent>()->SetModel("engine/models/default.dae");


        // We'll load the material here. What we want to do is pass an absolute path, which will in turn require us to specify the base part of the path that would be used to make it relative.
        String basePath = GT::GetBasePath(absolutePath, relativePath);
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
            script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
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
                                this->scriptTextBoxElement = gui.GetElementByID(script.GetString(String::CreateFormatted("GTGUI.Server.GetElementByID('%s').TextArea:GetID();", script.ToString(-1)).c_str()));
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
        auto &script = this->GetScript();

        script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("ResetCamera");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
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
                wasSaved = easyvfs_open_and_write_text_file(g_EngineContext->GetVFS(), this->GetAbsolutePath(), xmlString);
            }
            else
            {
                wasSaved = easyvfs_open_and_write_text_file(g_EngineContext->GetVFS(), this->GetAbsolutePath(), "");
            }

            if (wasSaved)
            {
                this->UnmarkAsModified();
            }
        }
        this->isSaving = false;

        return true;
    }

    void MaterialEditor::OnUpdate(double deltaTimeInSeconds)
    {
        if (this->viewportElement->IsVisible())
        {
            this->scene.Update(deltaTimeInSeconds);
        }
    }

    void MaterialEditor::OnFileUpdate(const char* absolutePath)
    {
        if (Strings::Equal(absolutePath, this->GetAbsolutePath()))
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
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
