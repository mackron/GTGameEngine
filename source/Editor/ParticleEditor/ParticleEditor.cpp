// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/ParticleEditor/ParticleEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/GTEngine.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GT
{
    ParticleEditor::ParticleEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          particleSystemDefinition(), particleSystem(particleSystemDefinition),
          scene(), camera(), particleNode(),
          mainElement(nullptr), viewportElement(nullptr), viewportEventHandler(*this, ownerEditor.GetGame(), scene.GetDefaultViewport()),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          grid(0.25f, 8, 32), axisArrows(),
          isSaving(false), isReloading(false),
          isShowingGrid(false), isShowingAxisArrows(false)
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

        // Show the grid and axis arrows.
        this->ShowGrid();
        this->ShowAxisArrows();


        // Load the particle system.
        this->particleSystemDefinition.LoadFromFile(absolutePath, relativePath);
        this->particleSystem.Refresh();


        // Now we setup the particle node.
        this->particleNode.AddComponent<ParticleSystemComponent>()->SetParticleSystem(this->particleSystem);
        this->particleNode.GetComponent<ParticleSystemComponent>()->PlayOnStartup(true);
        this->particleNode.GetComponent<ParticleSystemComponent>()->Play();

        // We're going to have the particles travelling in the +Y direction by default.
        this->particleNode.RotateX(90.0f);




        auto &gui    = this->GetGUI();
        auto &script = this->GetScript();

        this->mainElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='particle-editor' />");
        assert(this->mainElement != nullptr);
        {
            // The main element is the ModelEditor element. We need to pass 'this' as the '_internalPtr' argument.
            script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("ParticleEditor");
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
        this->scene.AddSceneNode(this->particleNode);
        
        // Now position the camera.
        this->ResetCamera();
    }

    ParticleEditor::~ParticleEditor()
    {
        // GUI elements.
        this->GetGUI().DeleteElement(this->mainElement);
    }


    void ParticleEditor::ResetCamera()
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


    ParticleSystemDefinition & ParticleEditor::GetParticleSystemDefinition()
    {
        return this->particleSystemDefinition;
    }


    void ParticleEditor::RefreshViewport()
    {
        this->particleSystem.Refresh();
    }


    void ParticleEditor::SetOrientation(const glm::quat &orientation)
    {
        this->particleNode.SetWorldOrientation(orientation);
    }


    void ParticleEditor::ShowGrid()
    {
        if (!this->isShowingGrid)
        {
            this->grid.Show(this->scene.GetRenderer());
            this->isShowingGrid = true;
        }
    }

    void ParticleEditor::HideGrid()
    {
        if (this->isShowingGrid)
        {
            this->grid.Hide(this->scene.GetRenderer());
            this->isShowingGrid = false;
        }
    }

    bool ParticleEditor::IsShowingGrid() const
    {
        return this->isShowingGrid;
    }


    void ParticleEditor::ShowAxisArrows()
    {
        if (!this->isShowingAxisArrows)
        {
            this->axisArrows.Show(this->scene.GetRenderer());
            this->isShowingAxisArrows = true;
        }
    }

    void ParticleEditor::HideAxisArrows()
    {
        if (this->isShowingAxisArrows)
        {
            this->axisArrows.Hide(this->scene.GetRenderer());
            this->isShowingAxisArrows = false;
        }
    }

    bool ParticleEditor::IsShowingAxisArrows() const
    {
        return this->isShowingAxisArrows;
    }



    void ParticleEditor::OnViewportSize()
    {
        float screenPositionX = 48.0f;
        float screenPositionY = 48.0f;
        float screenSize      = 32.0f;
        


        // We need to update the position of the gizmo so that it's positioned in the appropriate corner.
        glm::vec3 cameraPosition      = this->camera.GetWorldPosition();
        glm::vec3 cameraForward       = this->camera.GetWorldForwardVector();
        glm::vec3 arrowsWorldPosition = cameraPosition + (cameraForward * 1.0f);

        glm::vec3 windowPos      = this->scene.GetDefaultViewport().Project(arrowsWorldPosition);
        glm::vec3 arrowsPosition = this->scene.GetDefaultViewport().Unproject(glm::vec3(screenPositionX, screenPositionY, windowPos.z));
        

        // We actually need to scale this a bit to keep it a constant size.
        glm::vec3 arrowsScale(glm::distance(this->scene.GetDefaultViewport().Unproject(glm::vec3(screenPositionX, screenPositionY + screenSize, windowPos.z)), arrowsPosition));


        // World Space -> Local Space.
        arrowsPosition -= cameraPosition;
        arrowsPosition  = glm::inverse(this->camera.GetWorldOrientation()) * arrowsPosition;
        

        this->axisArrows.SetLocalPosition(arrowsPosition);
        this->axisArrows.SetLocalScale(arrowsScale);
        this->axisArrows.UpdateTransform(this->camera);
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void ParticleEditor::Show()
    {
        this->mainElement->Show();
    }

    void ParticleEditor::Hide()
    {
        this->mainElement->Hide();
    }

    bool ParticleEditor::Save()
    {
        bool wasSaved = false;

        this->isSaving = true;
        {
            auto file = easyvfs_open(g_EngineContext->GetVFS(), this->GetAbsolutePath(), EASYVFS_WRITE, 0);
            if (file != nullptr)
            {
                FileSerializer serializer(file);
                this->particleSystemDefinition.Serialize(serializer);

                this->UnmarkAsModified();

                easyvfs_close(file);
                wasSaved = true;


                // We want to immediatly force the game to check for changes so that the particle system is immediately reloaded.
                auto &dataFilesWatcher = this->GetOwnerEditor().GetGame().GetDataFilesWatcher();
                dataFilesWatcher.CheckForChanges(false);
                dataFilesWatcher.DispatchEvents();
            }
        }
        this->isSaving = false;

        return wasSaved;
    }

    void ParticleEditor::OnUpdate(double deltaTimeInSeconds)
    {
        if (this->viewportElement->IsVisible())
        {
            if (this->isShowingAxisArrows)
            {
                this->axisArrows.UpdateTransform(this->camera);
            }

            this->scene.Update(deltaTimeInSeconds);
        }
    }

    void ParticleEditor::OnFileUpdate(const char* absolutePath)
    {
        (void)absolutePath;
    }




    ///////////////////////////////////////////////////
    // Private.
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
