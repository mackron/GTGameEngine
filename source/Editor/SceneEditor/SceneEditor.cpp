// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/PrefabLibrary.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/String.hpp>

#undef min
#undef max

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wcast-align"
    #endif
#endif

namespace GTEngine
{
    static const bool DontMarkAsModified = false;   // <-- Passed to 'markAsModified' boolean parameters.


    //////////////////////////////////////////////
    // Scene Editor
    SceneEditor::SceneEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          camera(), cameraEventHandler(*this), cameraXRotation(0.0f), cameraYRotation(0.0f),
          updateManager(camera), physicsManager(), cullingManager(),
          scene(updateManager, physicsManager, cullingManager), sceneEventHandler(*this),
          selectedNodes(), selectedNodesBeforePlaying(), selectedNodesBeforePhysicsSimulation(),
          pickingWorld(),
          transformGizmo(), gizmoDragAxis(1.0f, 0.0f, 0.0f), gizmoDragFactor(1.0f, 0.0f),
          gizmoDragMode(GizmoDragMode_None), gizmoTransformMode(GizmoTransformMode_Translate), gizmoTransformSpace(GizmoTransformSpace_Global),
          pivotPoint(),
          transformedObjectWithGizmo(false),
          isDeserializing(false), isInstantiatingPrefab(false), isUpdatingFromStateStack(false),
          playbackState(PlaybackState_Stopped), wasPlayingBeforeHide(false), wasPlayingBeforeLosingFocus(false),
          isViewportMouseControlsEnabled(false),
          parentChangedLockCounter(0),
          GUI(), viewportEventHandler(*this, ownerEditor.GetGame(), scene.GetDefaultViewport()),
          grid(1.0f, 8, 32), isShowingGrid(false), wasShowingGridBeforePlaying(false),
          axisArrows(), isShowingAxisArrows(false), wasShowingAxisArrowsBeforePlaying(false),
          prefabLinker(scene, *this),
          pauseState()
    {
        this->scene.SetPrefabLinker(this->prefabLinker);

        this->scene.SetDefaultViewportCamera(this->camera);
        this->scene.AttachEventHandler(this->sceneEventHandler);

        // We're going to register the scene to the scripting environment right from the start.
        this->scene.RegisterToScript(this->GetScript());
        this->scene.BlockScriptEvents();                        // <-- Don't want any events posted to scripts while in edit mode.


        // We will add the system scene nodes to the scene before setting the minimum automatic scene node IDs. This will save
        // us from having to manually set IDs on the system nodes.
        this->CreateAndAddSystemNodes();



        auto &gui    = this->GetGUI();
        auto &script = this->GetScript();

        // We need to load the scene.
        auto file = GTCore::IO::Open(absolutePath, GTCore::IO::OpenMode::Read);
        if (file != nullptr)
        {
            // We need to now create the GUI elements for this particular file. We start with the main element.
            this->GUI.Main = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='scene-editor-main' />");
            assert(this->GUI.Main != nullptr);
            {
                // The main element has been created, but we need to run a script to have it turn it into a proper SceneEditor object.
                script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
                assert(script.IsTable(-1));
                {
                    script.Push("SceneEditor");
                    script.GetTableValue(-2);
                    assert(script.IsFunction(-1));
                    {
                        script.PushValue(-2);   // <-- 'self'.
                        script.Push(this);      // <-- '_internalPtr'
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
                                    this->GUI.Viewport = gui.GetElementByID(script.ToString(-1));

                                    this->GUI.Viewport->AttachEventHandler(this->viewportEventHandler);
                                    this->GUI.Viewport->OnSize();
                                }
                                script.Pop(1);          // <-- return value from GetID()
                            }
                        }
                        script.Pop(1);

                        script.Push("PropertiesPanel");
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
                                    this->GUI.PropertiesPanel = gui.GetElementByID(script.ToString(-1));
                                }
                                script.Pop(1);          // <-- return value from GetID()
                            }
                        }
                        script.Pop(1);
                    }
                }
                script.Pop(1);
            }



            // To place the camera correctly, we'll just reset it.
            this->ResetCamera();

            // Show the grid by default.
            this->ShowGrid();

            // Show the axis arrows by default.
            this->ShowAxisArrows();


            // HDR+Bloom should be disabled by default. The 'false' arguments means that the scene should NOT be marked as modified.
            this->DisableSceneHDR(false);
            this->DisableSceneBloom(false);




            // At this point we should actually load the scene file. If this is an empty file, we'll just load an empty scene.
            if (GTCore::IO::Size(file) > 0)
            {
                GTCore::FileDeserializer deserializer(file);
                this->DeserializeScene(deserializer);
            }
            else
            {
                // We want to do an initial commit.
                this->scene.CommitStateStackFrame();
            }


            // Now we want to update the "view" menu.
            this->UpdateViewportMenuGUI();


            // The scene will be done loading by this pointer, so we can close the file.
            GTCore::IO::Close(file);
        }
    }

    SceneEditor::~SceneEditor()
    {
        // If we're playing, we need to stop. If we don't do this there are a few things that won't get restored correctly.
        this->StopPlaying();

        // Hide the navigation meshes.
        this->HideAllNavigationMeshes();

        // We need to delete the main GUI element, along with it's children.
        this->GetGUI().DeleteElement(this->GUI.Main);
    }



    void SceneEditor::ResetCamera()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
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


    void SceneEditor::SetViewportCamera(SceneNode &sceneNode)
    {
        this->scene.GetDefaultViewport().SetCameraNode(sceneNode);
        this->viewportEventHandler.OnSize(*this->GUI.Viewport);         // <-- Ensure the projection is set correctly.
    }


    void SceneEditor::StartPlaying()
    {
        if (this->IsStopped() || this->IsPaused())
        {
            if (this->IsPaused())
            {
                this->RestorePauseState();

                this->updateManager.Enable();
                this->updateManager.EnableParticles();
                this->physicsManager.EnableSimulation();
            }
            else
            {
                //this->isPlaying                  = true;
                this->selectedNodesBeforePlaying = this->selectedNodes;

                // We want to deselect everything to begin with.
                this->DeselectAll(SelectionOption_NoStateStaging);

                // The grid needs to be hidden while playing, but we need to keep track of whether or not it was showing beforehand so we can restore
                // it when we stop playing.
                this->wasShowingGridBeforePlaying = this->isShowingGrid;
                this->HideGrid();

                // As with the grid, we'll hide the axis arrows.
                this->wasShowingAxisArrowsBeforePlaying = this->isShowingAxisArrows;
                this->HideAxisArrows();


                // We will hide the icons and directional arrows of every scene node. These will naturally be restored when the state stack is restored.
                // which will happen when playback is stopped.
                this->HideAllSpritesAndDirectionalArrows();


                // We need to make it so Game.GetGameWindowGUIElement() is our own implementation. We restore it later. Our version returns an element
                // that is contained within the viewport.
                auto &script = this->GetScript();

                script.Execute(GTCore::String::CreateFormatted("GTEngine.Editor.__CurrentSceneEditorViewportElement = GTGUI.Server.GetElementByID('%s')", this->GUI.Viewport->id).c_str());
                script.Execute
                (
                    "Game.GetGameWindowGUIElement = function()"
                    "    return GTEngine.Editor.__CurrentSceneEditorViewportElement;"
                    "end"
                );

                // Game.LoadScene needs to be overridden, also. What we will do is load the file, switch to it, and then run it.
                script.Execute
                (
                    "Game.LoadScene = function(sceneRelativePath)"
                    "    local currentEditor = Editor.GetCurrentlyShownEditor();"
                    "    if currentEditor and currentEditor.StopPlaying then"
                    "        currentEditor:StopPlaying();"
                    "    end;"
                    ""
                    "    local sceneEditorElement = Editor.OpenFile(sceneRelativePath);"
                    "    if sceneEditorElement then"
                    "        sceneEditorElement:StartPlaying();"
                    "    end;"
                    "end"
                );

                // The editor needs to intercept calls to Game.Pause and Game.Resume to simulate the functionality when playing from in-editor.
                script.Execute
                (
                    "Game.Pause = function()"
                    "    local currentEditor = Editor.GetCurrentlyShownEditor();"
                    "    if currentEditor and currentEditor.IsPlaying and currentEditor:IsPlaying() then"
                    "        currentEditor:PauseSceneUpdates();"
                    "    end;"
                    "end;"

                    "Game.Resume = function()"
                    "    local currentEditor = Editor.GetCurrentlyShownEditor();"
                    "    if currentEditor and currentEditor.IsPlaying and currentEditor:IsPlaying() then"
                    "        currentEditor:ResumeSceneUpdates();"
                    "    end;"
                    "end;"
                );


                // All particle system should be reset.
                this->ResetAllParticleSystems();


                // We want to unblock script events before posting OnStartup.
                this->scene.UnblockScriptEvents();

                // We'll call OnStartup on all scene nodes here.
                this->scene.PostSceneNodeScriptEvent_OnStartup();


                // If the camera is not he main camera, we want to disable mouse capture in the viewport.
                if (this->scene.GetViewportByIndex(0).GetCameraNode() != &this->camera)
                {
                    this->DisableViewportMouseControls();
                }


                this->updateManager.Enable();

                this->physicsManager.EnableSimulation();
                this->physicsManager.ActivateAllRigidBodies();
            }

            //this->isPaused = false;
            this->playbackState = PlaybackState_Playing;

            // This must be done after setting the playback state because otherwise the controls will think it's in the old state.
            this->UpdatePlaybackControls();
        }
    }

    void SceneEditor::PausePlaying()
    {
        if (this->IsPlaying())
        {
            //this->isPaused = true;

            this->playbackState = PlaybackState_Paused;
            this->CapturePauseState();

            this->GetOwnerEditor().GetGame().ReleaseMouse();
            this->EnableViewportMouseControls();

            this->SetViewportCameraToDefault();


            this->updateManager.Disable();
            this->updateManager.DisableParticles();
            this->physicsManager.DisableSimulation();

            this->UpdatePlaybackControls();
        }
    }

    void SceneEditor::StopPlaying()
    {
        if (this->IsPlaying() || this->IsPaused())
        {
            this->LockParentChangedEvents();
            this->isUpdatingFromStateStack = true;
            {
                this->ResumeSceneUpdates();


                // We'll call OnShutdown on all scene nodes here.
                this->scene.PostSceneNodeScriptEvent_OnShutdown();

                // Events need to be blocked from being posted to scripts since we're now in proper edit mode.
                this->scene.BlockScriptEvents();


                //this->isPlaying = false;
                //this->isPaused  = false;
                this->playbackState = PlaybackState_Stopped;


                // Some functions need to be restored.
                auto &script = this->GetScript();
                script.GetGlobal("Game");
                assert(script.IsTable(-1));
                {
                    script.SetTableFunction(-1, "GetGameWindowGUIElement", Scripting::GameFFI::GetGameWindowGUIElement);
                    script.SetTableFunction(-1, "LoadScene",               Scripting::GameFFI::LoadScene);
                    script.SetTableFunction(-1, "Pause",                   Scripting::GameFFI::Pause);
                    script.SetTableFunction(-1, "Resume",                  Scripting::GameFFI::Resume);
                }
                script.Pop(1);


                // The scripting environment might have changed the main camera. We need to revert back just in case.
                this->SetViewportCameraToDefault();

                // A game may have captured the mouse. We'll force a release just in case it doesn't handle it correctly.
                this->GetOwnerEditor().GetGame().ReleaseMouse();

                // We will also want to make sure mouse capture is re-enabled.
                this->EnableViewportMouseControls();


                // The grid might need to be shown.
                if (this->wasShowingGridBeforePlaying)
                {
                    this->ShowGrid();
                }

                // As with the grid, we might need to show the axis arrows.
                if (this->wasShowingAxisArrowsBeforePlaying)
                {
                    this->ShowAxisArrows();
                }


                this->updateManager.Disable();
                this->physicsManager.DisableSimulation();


                // Particles systems should be reset.
                this->ResetAllParticleSystems();



                // To restore, all we need to do is revert the staging area.
                this->scene.RevertStateStackStagingArea();

                // We want to revert the selections, but we don't want to stage these on the state stack.
                this->DeselectAll(SelectionOption_NoStateStaging);
                this->SelectSceneNodes(this->selectedNodesBeforePlaying, SelectionOption_NoStateStaging);


                this->UpdatePlaybackControls();
            }
            this->isUpdatingFromStateStack = false;
            this->UnlockParentChangedEvents();
        }
    }

    bool SceneEditor::IsPlaying() const
    {
        return this->playbackState == PlaybackState_Playing;
    }

    bool SceneEditor::IsPaused() const
    {
        return this->playbackState == PlaybackState_Paused;
    }

    bool SceneEditor::IsStopped() const
    {
        return this->playbackState == PlaybackState_Stopped;
    }


    void SceneEditor::EnablePhysicsSimulation()
    {
        if (!this->IsPhysicsSimulationEnabled())
        {
            this->selectedNodesBeforePhysicsSimulation = this->selectedNodes;
            this->physicsManager.EnableSimulation();
            this->physicsManager.ActivateAllRigidBodies();
        }
    }

    void SceneEditor::DisablePhysicsSimulation()
    {
        if (this->IsPhysicsSimulationEnabled())
        {
            this->LockParentChangedEvents();
            this->isUpdatingFromStateStack = true;
            {
                this->physicsManager.DisableSimulation();


                // To restore, all we need to do is revert the staging area.
                this->scene.RevertStateStackStagingArea();

                // We want to revert the selections, but we don't want to stage these on the state stack.
                this->DeselectAll(SelectionOption_NoStateStaging);
                this->SelectSceneNodes(this->selectedNodesBeforePhysicsSimulation, SelectionOption_NoStateStaging);
            }
            this->isUpdatingFromStateStack = false;
            this->UnlockParentChangedEvents();
        }
    }

    bool SceneEditor::IsPhysicsSimulationEnabled() const
    {
        return this->physicsManager.IsSimulationEnabled();
    }


    SceneNode* SceneEditor::GetSceneNodeByID(uint64_t id)
    {
        return this->scene.GetSceneNodeByID(id);
    }

    const SceneNode* SceneEditor::GetSceneNodeByID(uint64_t id) const
    {
        return this->scene.GetSceneNodeByID(id);
    }



    void SceneEditor::DisableViewportMouseControls()
    {
        // We need to disable on the scripting side, too.
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("DisableViewportControls");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);       // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);


        this->viewportEventHandler.DisableMouseControls();
        this->isViewportMouseControlsEnabled = false;
    }

    void SceneEditor::EnableViewportMouseControls()
    {
        // We need to enable on the scripting side, too.
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("EnableViewportControls");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);       // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);


        this->viewportEventHandler.EnableMouseControls();
        this->isViewportMouseControlsEnabled = true;
    }

    bool SceneEditor::IsViewportMouseControlsEnabled() const
    {
        return this->isViewportMouseControlsEnabled;
    }



    void SceneEditor::ShowGrid()
    {
        if (!this->isShowingGrid)
        {
            this->grid.Show(this->scene.GetRenderer());
            this->isShowingGrid = true;
        }
    }

    void SceneEditor::HideGrid()
    {
        if (this->isShowingGrid)
        {
            this->grid.Hide(this->scene.GetRenderer());
            this->isShowingGrid = false;
        }
    }

    bool SceneEditor::IsShowingGrid() const
    {
        return this->isShowingGrid;
    }


    void SceneEditor::ShowAxisArrows()
    {
        if (!this->isShowingAxisArrows)
        {
            this->axisArrows.Show(this->scene.GetRenderer());
            this->isShowingAxisArrows = true;
        }
    }

    void SceneEditor::HideAxisArrows()
    {
        if (this->isShowingAxisArrows)
        {
            this->axisArrows.Hide(this->scene.GetRenderer());
            this->isShowingAxisArrows = false;
        }
    }

    bool SceneEditor::IsShowingAxisArrows() const
    {
        return this->isShowingAxisArrows;
    }


    void SceneEditor::SetSceneName(const char* sceneName)
    {
        this->scene.SetName(sceneName);
        this->MarkAsModified();
    }

    const char* SceneEditor::GetSceneName() const
    {
        return this->scene.GetName();
    }


    void SceneEditor::EnableSceneBackgroundClearing(float r, float g, float b, bool markAsModified)
    {
        this->scene.EnableBackgroundClearing(r, g, b);

        if (markAsModified)
        {
            this->MarkAsModified();
        }
    }
        
    void SceneEditor::DisableSceneBackgroundClearing(bool markAsModified)
    {
        this->scene.DisableBackgroundClearing();

        if (markAsModified)
        {
            this->MarkAsModified();
        }
    }

    bool SceneEditor::IsSceneBackgroundClearingEnabled() const
    {
        return this->scene.IsBackgroundClearingEnabled();
    }

    const glm::vec3 SceneEditor::GetSceneBackgroundClearColour() const
    {
        return this->scene.GetBackgroundClearColour();
    }

        
    void SceneEditor::EnableSceneHDR(bool markAsModified)
    {
        this->scene.EnableHDR();
        
        if (markAsModified)
        {
            this->MarkAsModified();
        }
    }

    void SceneEditor::DisableSceneHDR(bool markAsModified)
    {
        this->scene.DisableHDR();

        if (markAsModified)
        {
            this->MarkAsModified();
        }
    }

    bool SceneEditor::IsSceneHDREnabled() const
    {
        return this->scene.IsHDREnabled();
    }


    void SceneEditor::EnableSceneBloom(bool markAsModified)
    {
        this->scene.EnableBloom();
        
        if (markAsModified)
        {
            this->MarkAsModified();
        }
    }

    void SceneEditor::DisableSceneBloom(bool markAsModified)
    {
        this->scene.DisableBloom();

        if (markAsModified)
        {
            this->MarkAsModified();
        }
    }

    bool SceneEditor::IsSceneBloomEnabled() const
    {
        return this->scene.IsBloomEnabled();
    }


    void SceneEditor::ShowNavigationMesh(size_t index)
    {
        assert(index == 0);     // <-- Temp assert until we implement support for multiple navigation meshes.

        
        // Get the mesh.
        SceneRendererMesh* mainMesh      = nullptr;
        SceneRendererMesh* innerEdgeMesh = nullptr;
        SceneRendererMesh* outerEdgeMesh = nullptr;

        auto iNavigationMesh = this->navigationMeshRendererMeshes.Find(index);
        if (iNavigationMesh == nullptr)
        {
            // The renderer mesh doesn't exists. Create it.
            mainMesh = new SceneRendererMesh;
            mainMesh->vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
            mainMesh->material    = MaterialLibrary::CreateNavigationMeshMaterial();
            
            innerEdgeMesh = new SceneRendererMesh;
            innerEdgeMesh->vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
            innerEdgeMesh->material    = MaterialLibrary::CreateNavigationMeshMaterial();
            innerEdgeMesh->material->SetParameter("Colour", glm::vec3(0.0f, 0.0f, 0.25f));
            innerEdgeMesh->material->SetParameter("Alpha",  0.05f);
            innerEdgeMesh->drawMode    = DrawMode_Lines;

            outerEdgeMesh = new SceneRendererMesh;
            outerEdgeMesh->vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
            outerEdgeMesh->material    = MaterialLibrary::CreateNavigationMeshMaterial();
            outerEdgeMesh->material->SetParameter("Colour", glm::vec3(0.0f, 0.0f, 0.25f));
            outerEdgeMesh->material->SetParameter("Alpha",  0.5f);
            outerEdgeMesh->drawMode    = DrawMode_Lines;


            MeshBuilderP3 mainMeshBuilder;
            MeshBuilderP3 innerEdgeMeshBuilder;
            MeshBuilderP3 outerEdgeMeshBuilder;
            this->scene.GetNavigationMesh(index).BuildMeshVisualization(mainMeshBuilder, innerEdgeMeshBuilder, outerEdgeMeshBuilder);

            mainMesh->vertexArray->SetVertexData(mainMeshBuilder.GetVertexData(), mainMeshBuilder.GetVertexCount());
            mainMesh->vertexArray->SetIndexData( mainMeshBuilder.GetIndexData(),  mainMeshBuilder.GetIndexCount());

            innerEdgeMesh->vertexArray->SetVertexData(innerEdgeMeshBuilder.GetVertexData(), innerEdgeMeshBuilder.GetVertexCount());
            innerEdgeMesh->vertexArray->SetIndexData( innerEdgeMeshBuilder.GetIndexData(),  innerEdgeMeshBuilder.GetIndexCount());

            outerEdgeMesh->vertexArray->SetVertexData(outerEdgeMeshBuilder.GetVertexData(), outerEdgeMeshBuilder.GetVertexCount());
            outerEdgeMesh->vertexArray->SetIndexData( outerEdgeMeshBuilder.GetIndexData(),  outerEdgeMeshBuilder.GetIndexCount());


            this->navigationMeshRendererMeshes.Add(index, NavigationMeshRendererMeshes(mainMesh, innerEdgeMesh, outerEdgeMesh));
        }
        else
        {
            mainMesh      = iNavigationMesh->value.m_mainMesh;
            innerEdgeMesh = iNavigationMesh->value.m_innerEdgeMesh;
            outerEdgeMesh = iNavigationMesh->value.m_outerEdgeMesh;
        }


        // Show the mesh.
        assert(mainMesh != nullptr);
        {
            this->scene.GetRenderer().AddExternalMesh(*mainMesh);
            this->scene.GetRenderer().AddExternalMesh(*innerEdgeMesh);
            this->scene.GetRenderer().AddExternalMesh(*outerEdgeMesh);
        }
    }

    void SceneEditor::HideNavigationMesh(size_t index)
    {
        assert(index == 0);     // <-- Temp assert until we implement support for multiple navigation meshes.

        auto iNavigationMesh = this->navigationMeshRendererMeshes.Find(index);
        if (iNavigationMesh != nullptr)
        {
            auto &rendererMesh = iNavigationMesh->value;

            assert(rendererMesh.m_mainMesh);
            assert(rendererMesh.m_innerEdgeMesh);
            assert(rendererMesh.m_outerEdgeMesh);
            {
                this->scene.GetRenderer().RemoveExternalMesh(*rendererMesh.m_mainMesh);
                this->scene.GetRenderer().RemoveExternalMesh(*rendererMesh.m_innerEdgeMesh);
                this->scene.GetRenderer().RemoveExternalMesh(*rendererMesh.m_outerEdgeMesh);

                // Delete the renderer mesh.
                MaterialLibrary::Delete(rendererMesh.m_mainMesh->material);
                MaterialLibrary::Delete(rendererMesh.m_innerEdgeMesh->material);
                MaterialLibrary::Delete(rendererMesh.m_outerEdgeMesh->material);

                VertexArrayLibrary::Delete(rendererMesh.m_mainMesh->vertexArray);
                VertexArrayLibrary::Delete(rendererMesh.m_innerEdgeMesh->vertexArray);
                VertexArrayLibrary::Delete(rendererMesh.m_outerEdgeMesh->vertexArray);

                delete rendererMesh.m_mainMesh;
                delete rendererMesh.m_innerEdgeMesh;
                delete rendererMesh.m_outerEdgeMesh;

                this->navigationMeshRendererMeshes.RemoveByIndex(iNavigationMesh->index);
            }
        }
    }

    void SceneEditor::HideAllNavigationMeshes()
    {
        while (this->GetNavigationMeshCount() > 0)
        {
            this->HideNavigationMesh(0);
        }
    }

    size_t SceneEditor::GetNavigationMeshCount() const
    {
        return this->navigationMeshRendererMeshes.count;
    }


    void SceneEditor::PauseSceneUpdates()
    {
        this->updateManager.PauseSceneUpdates();
    }

    void SceneEditor::ResumeSceneUpdates()
    {
        this->updateManager.ResumeSceneUpdates();
    }

    bool SceneEditor::IsSceneUpdatesPaused() const
    {
        return this->updateManager.IsSceneUpdatesPaused();
    }



    ///////////////////////////////////////////////////
    // Scene Events

    bool SceneEditor::TryGizmoMouseSelect()
    {
        if (this->GUI.Main->IsVisible())
        {
            int clickPosX;
            int clickPosY;
            this->viewportEventHandler.GetMousePosition(clickPosX, clickPosY);

            glm::vec3 rayStart;
            glm::vec3 rayEnd;
            this->scene.GetDefaultViewport().CalculatePickingRay(clickPosX, clickPosY, rayStart, rayEnd);


            CollisionWorld::ClosestRayTestCallback rayTestCallback(rayStart, rayEnd);
            rayTestCallback.m_collisionFilterGroup = CollisionGroups::EditorSelectionRay;
            rayTestCallback.m_collisionFilterMask  = CollisionGroups::EditorGizmo;
            this->pickingWorld.RayTest(rayStart, rayEnd, rayTestCallback);
            if (rayTestCallback.hasHit())
            {
                auto handle = static_cast<TransformGizmo::Handle*>(rayTestCallback.m_collisionObject->getUserPointer());
                assert(handle != nullptr);
                {
                    if ((handle->axis & TransformGizmo::HandleAxis_X))
                    {
                        this->gizmoDragAxis = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    else if ((handle->axis & TransformGizmo::HandleAxis_Y))
                    {
                        this->gizmoDragAxis = glm::vec3(0.0f, 1.0f, 0.0f);
                    }
                    else if ((handle->axis & TransformGizmo::HandleAxis_Z))
                    {
                        this->gizmoDragAxis = glm::vec3(0.0f, 0.0f, 1.0f);
                    }

                    if (handle->type == TransformGizmo::HandleType_Translate)
                    {
                        this->gizmoDragMode = GizmoDragMode_Translate;
                    }
                    else if (handle->type == TransformGizmo::HandleType_Rotate)
                    {
                        this->gizmoDragMode = GizmoDragMode_Rotate;
                    }
                    else if (handle->type == TransformGizmo::HandleType_Scale)
                    {
                        this->gizmoDragMode = GizmoDragMode_Scale;
                    }


                    // The mouse dragging has a different level of influence depending on the direction of the axis. We need to calculate that now. We project two points - the
                    // center of the gizmo and the end point of the selected axis. From that we can get a normalised direction vector and use that as the influence.
                    glm::vec3 gizmoWorldPos        = this->transformGizmo.GetPosition();
                    glm::vec3 gizmoCenterWindowPos = this->scene.GetDefaultViewport().Project(gizmoWorldPos);
                    glm::vec3 axisTipWindowPos     = this->scene.GetDefaultViewport().Project(gizmoWorldPos + (handle->GetForwardVector() * this->transformGizmo.GetScale()));

                    this->gizmoDragFactor = glm::vec2(axisTipWindowPos - gizmoCenterWindowPos);
                    if (glm::dot(this->gizmoDragFactor, this->gizmoDragFactor) > 0.0f)
                    {
                        this->gizmoDragFactor = glm::normalize(this->gizmoDragFactor);
                    }
                    else
                    {
                        this->gizmoDragFactor.x = 1.0f;
                        this->gizmoDragFactor.y = 0.0f;
                    }


                    handle->SetColour(glm::vec3(1.0f, 1.0f, 1.0f));


                    // We need to disable camera controls so that it doesn't move as we're transforming the gizmo.
                    this->DisableViewportCameraControls();
                }

                return true;
            }
            else
            {
                this->gizmoDragMode = GizmoDragMode_None;

                // We need to re-enable camera controls because we may have disabled them earlier.
                this->EnableViewportCameraControls();
            }
        }

        return false;
    }

    void SceneEditor::DoMouseSelection()
    {
        if (this->GUI.Main->IsVisible())
        {
            // We want to do a few ray tests here. Some stuff will have priority over other stuff. For example, any gizmo will have a higher priority over everything else.

            int clickPosX;
            int clickPosY;
            this->viewportEventHandler.GetMousePosition(clickPosX, clickPosY);

            glm::vec3 rayStart;
            glm::vec3 rayEnd;
            this->scene.GetDefaultViewport().CalculatePickingRay(clickPosX, clickPosY, rayStart, rayEnd);


            CollisionWorld::ClosestRayTestCallback rayTestCallback(rayStart, rayEnd);
            rayTestCallback.m_collisionFilterGroup = CollisionGroups::EditorSelectionRay;
            rayTestCallback.m_collisionFilterMask  = CollisionGroups::EditorSelectionVolume;
            this->pickingWorld.RayTest(rayStart, rayEnd, rayTestCallback);
            if (rayTestCallback.hasHit())
            {
                // The use data of the object will be a pointer to the EditorMetadataComponent object. From this, we can grab the actual scene node.
                auto metadata = static_cast<EditorMetadataComponent*>(rayTestCallback.m_collisionObject->getUserPointer());
                assert(metadata != nullptr);
                {
                    auto &selectedNode = metadata->GetNode();

                    // The way we do the selection depends on what we're doing. If shift is being held down, we don't want to deselect anything and instead just add
                    // or remove the node to the selection. If the selected node is already selected, it needs to be deselected. Otherwise it needs to be selected.
                    if (this->GetOwnerEditor().GetGame().IsKeyDown(GTCore::Keys::Shift))
                    {
                        if (this->IsSceneNodeSelected(selectedNode))
                        {
                            this->DeselectSceneNode(selectedNode, 0);
                        }
                        else
                        {
                            this->SelectSceneNode(selectedNode, 0);
                        }
                    }
                    else
                    {
                        // If the node is already the selected one, we don't do anything.
                        if (!(this->selectedNodes.count == 1 && this->selectedNodes[0] == selectedNode.GetID()))
                        {
                            this->DeselectAll(0);
                            this->SelectSceneNode(selectedNode, 0);
                        }
                    }
                }
            }
            else
            {
                this->DeselectAll(0);
            }
        }
    }

    void SceneEditor::DeselectAll(unsigned int options)
    {
        while (this->selectedNodes.count > 0)
        {
            auto node = this->GetSceneNodeByID(this->selectedNodes[0]);
            assert(node != nullptr);
            {
                this->DeselectSceneNode(*node, options | SelectionOption_NoScriptNotify);
            }
        }



        // TODO: This feels bad. See if we can remove this.
        size_t sceneNodeCount = this->scene.GetSceneNodeCount();

        for (size_t i = 0; i < sceneNodeCount; ++i)
        {
            auto node = this->scene.GetSceneNodeByIndex(i);
            assert(node != nullptr);
            {
                auto metadata = node->GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    metadata->Deselect();
                }
            }
        }



        // We need to let the scripting environment know about this change.
        if ((options & SelectionOption_NoScriptNotify) == 0)
        {
            this->PostOnSelectionChangedEventToScript();
        }
    }


    bool SceneEditor::IsSceneNodeSelected(const SceneNode &node) const
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            if (metadata->IsSelected())
            {
                return true;
            }

            // We need to also check the actual list of selected nodes. It's considered selected if it's here.
            return this->selectedNodes.Exists(node.GetID());
        }

        return false;
    }

    bool SceneEditor::IsAncestorSelected(const SceneNode &node) const
    {
        if (node.GetParent() != nullptr)
        {
            if (this->IsSceneNodeSelected(*node.GetParent()))
            {
                return true;
            }
            else
            {
                return this->IsAncestorSelected(*node.GetParent());
            }
        }

        return false;
    }

    bool SceneEditor::IsAnythingSelected() const
    {
        return this->selectedNodes.count > 0;
    }

    void SceneEditor::SelectSceneNode(SceneNode &node, unsigned int options)
    {
        if ((options & SelectionOption_Force) || !this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Select();

                if (node.HasComponent<DynamicsComponent>())
                {
                    metadata->ShowCollisionShapeMesh();
                    this->scene.GetRenderer().AddExternalMesh(metadata->GetCollisionShapeMesh());
                }

                if (node.HasComponent<ProximityComponent>())
                {
                    metadata->ShowProximityShapeMesh();
                    this->scene.GetRenderer().AddExternalMesh(metadata->GetProximityShapeMesh());
                }


                if (!this->selectedNodes.Exists(node.GetID()))
                {
                    this->selectedNodes.PushBack(node.GetID());
                }

                // The scripting environment needs to be aware of this change.
                if (!(options & SelectionOption_NoScriptNotify))
                {
                    this->PostOnSelectionChangedEventToScript();
                }


                // We will need to update the pivot point.
                this->UpdatePivotPoint();

                // With a change in selection, we will need to update the position of the gizmos.
                this->ShowGizmo();


                // We want to add this node to the staging area as an update command. We don't want to do this if we're in the middle of
                // updating the scene from the state stack.
                if (!(options & SelectionOption_NoStateStaging) && !this->isUpdatingFromStateStack)
                {
                    this->scene.StageUpdateOnStateStack(node);
                }
            }
        }
    }

    void SceneEditor::SelectSceneNodes(const GTCore::Vector<uint64_t> &selectedNodeIDs, unsigned int options)
    {
        for (size_t i = 0; i < selectedNodeIDs.count; ++i)
        {
            auto node = this->GetSceneNodeByID(selectedNodeIDs[i]);
            assert(node != nullptr);
            {
                this->SelectSceneNode(*node, options | SelectionOption_NoScriptNotify);
            }
        }


        if (!(options & SelectionOption_NoScriptNotify))
        {
            this->PostOnSelectionChangedEventToScript();
        }
    }

    void SceneEditor::DeselectSceneNode(SceneNode &node, unsigned int options)
    {
        if ((options & SelectionOption_Force) || this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Deselect();

                if (node.HasComponent<DynamicsComponent>())
                {
                    metadata->HideCollisionShapeMesh();

                    this->scene.GetRenderer().RemoveExternalMesh(metadata->GetCollisionShapeMesh());
                }

                if (node.HasComponent<ProximityComponent>())
                {
                    metadata->HideProximityShapeMesh();

                    this->scene.GetRenderer().RemoveExternalMesh(metadata->GetProximityShapeMesh());
                }


                this->selectedNodes.RemoveFirstOccuranceOf(node.GetID());

                // The scripting environment needs to be aware of this change.
                if (!(options & SelectionOption_NoScriptNotify))
                {
                    this->PostOnSelectionChangedEventToScript();
                }


                // With a change in selection, we will need to update the gizmos.
                if (this->selectedNodes.count == 0)
                {
                    this->HideGizmo();
                }
                else
                {
                    this->UpdateGizmoTransform();
                }


                // We want to add this node to the staging area as an update command.
                if (!(options & SelectionOption_NoStateStaging))
                {
                    this->scene.StageUpdateOnStateStack(node);
                }
            }
        }
    }

    void SceneEditor::DeselectSceneNodeAndChildren(SceneNode &node, unsigned int options)
    {
        this->DeselectSceneNode(node, options | SelectionOption_NoScriptNotify);

        for (auto childNode = node.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
        {
            this->DeselectSceneNodeAndChildren(*childNode, options | SelectionOption_NoScriptNotify);
        }


        if (!(options & SelectionOption_NoScriptNotify))
        {
            this->PostOnSelectionChangedEventToScript();
        }
    }


    glm::vec3 SceneEditor::GetSelectionCenterPoint() const
    {
        if (this->selectedNodes.count > 0)
        {
            glm::vec3 aabbMin( FLT_MAX,  FLT_MAX,  FLT_MAX);
            glm::vec3 aabbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);


            for (size_t i = 0; i < this->selectedNodes.count; ++i)
            {
                auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                assert(node != nullptr);
                {
                    glm::vec3 position = node->GetWorldPosition();

                    aabbMin = glm::min(aabbMin, position);
                    aabbMax = glm::max(aabbMax, position);
                }
            }


            return (aabbMin + aabbMax) * 0.5f;
        }

        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::quat SceneEditor::GetGizmoRotation() const
    {
        if (this->selectedNodes.count == 1 && (this->gizmoTransformSpace == GizmoTransformSpace_Local || this->gizmoTransformMode == GizmoTransformMode_Scale))
        {
            auto node = this->GetSceneNodeByID(this->selectedNodes[0]);
            assert(node != nullptr);
            {
                return node->GetWorldOrientation();
            }
        }

        return glm::quat();
    }

    size_t SceneEditor::GetSelectedSceneNodeCount() const
    {
        return this->selectedNodes.count;
    }

    SceneNode* SceneEditor::GetFirstSelectedSceneNode()
    {
        if (this->selectedNodes.count > 0)
        {
            return this->GetSceneNodeByID(this->selectedNodes[0]);
        }

        return nullptr;
    }

    void SceneEditor::GetSelectedSceneNodeIDs(GTCore::Vector<uint64_t> &sceneNodeIDsOut)
    {
        // TODO: When we migrate the scene node ID system over to the new one, we will want to optimize this to a simple copy. Might event change this to a simple getter.

        for (size_t i = 0; i < this->selectedNodes.count; ++i)
        {
            auto sceneNode = this->GetSceneNodeByID(this->selectedNodes[i]);
            assert(sceneNode != nullptr);
            {
                sceneNodeIDsOut.PushBack(sceneNode->GetID());
            }
        }
    }


    ///////////////////////////////////////////////////
    // Editting

    void SceneEditor::RemoveSelectedSceneNodes()
    {
        if (this->selectedNodes.count > 0)
        {
            auto nodesToDelete = this->selectedNodes;
            this->RemoveSceneNodes(nodesToDelete);

            this->CommitStateStackFrame();
        }
    }

    void SceneEditor::RemoveSceneNode(SceneNode &sceneNodeToRemove)
    {
        this->scene.RemoveSceneNode(sceneNodeToRemove);
    }

    void SceneEditor::RemoveSceneNodes(const GTCore::Vector<uint64_t> &sceneNodeIDs)
    {
        for (size_t i = 0; i < sceneNodeIDs.count; ++i)
        {
            this->scene.RemoveSceneNodeByID(sceneNodeIDs[i]);
        }
    }

    void SceneEditor::DuplicateSelectedSceneNodes()
    {
        if (this->selectedNodes.count > 0)
        {
            GTCore::Vector<uint64_t>   prevSelectedNodes(this->selectedNodes);
            GTCore::Vector<SceneNode*> newNodes(prevSelectedNodes.count);



            // What we're going to do is grab the IDs of all of the scene nodes we want to copy. We include all descendants.
            GTCore::Vector<SceneNode*> sceneNodesToCopy;
            
            for (size_t iNode = 0; iNode < prevSelectedNodes.count; ++iNode)
            {
                auto nodeToCopy = this->GetSceneNodeByID(prevSelectedNodes[iNode]);
                assert(nodeToCopy != nullptr);
                {
                    if (!this->IsAncestorSelected(*nodeToCopy))
                    {
                        sceneNodesToCopy.PushBack(nodeToCopy);
                    }
                }
            }



            // We want to select the new nodes, so we're going to deselect everything at this point.
            this->DeselectAll(SelectionOption_NoScriptNotify);



            // At this point sceneNodesToCopy will be filled with the IDs of the scene nodes that do not have an ancestor selected. The parent IDs for these
            // will be to existing nodes.
            //
            // We will also want to copy the children of each of these nodes, but we copy the top-level ones first.
            for (size_t iNode = 0; iNode < sceneNodesToCopy.count; ++iNode)
            {
                auto nodeToCopy = sceneNodesToCopy[iNode];
                assert(nodeToCopy != nullptr);
                {
                    this->CopySceneNodeAndChildren(*nodeToCopy, nodeToCopy->GetParent());
                }
            }


            // We will let the editor know about a change in selection.
            this->PostOnSelectionChangedEventToScript();


            // Undo/Redo point.
            this->CommitStateStackFrame();
        }
    }

    SceneNode & SceneEditor::DuplicateSceneNode(SceneNode &sceneNodeToDuplicate)
    {
        this->DeselectAll(SelectionOption_NoScriptNotify);

        auto &newNode = this->CopySceneNodeAndChildren(sceneNodeToDuplicate, sceneNodeToDuplicate.GetParent());
        this->SelectSceneNode(newNode, SelectionOption_Force);

        this->CommitStateStackFrame();

        return newNode;
    }


    void SceneEditor::Undo()
    {
        // Don't bother doing anything if we're already at the start of the current branch.
        if (this->scene.GetStateStackCurrentFrameIndex() > 0)
        {
            this->LockParentChangedEvents();
            this->isUpdatingFromStateStack = true;
            {
                // If the physics simulation is running or the game is playing, it needs to be stopped first.
                if (this->IsPlaying() || this->IsPaused())
                {
                    this->StopPlaying();
                }
                else if (this->IsPhysicsSimulationEnabled())
                {
                    this->DisablePhysicsSimulation();
                }


                // We deselect everything because we're going to be reselecting the appropriate nodes after the state change.
                this->DeselectAll(SelectionOption_NoScriptNotify | SelectionOption_NoStateStaging);

                this->scene.SeekStateStack(-1);
                this->MarkAsModified();

                // All nodes need to be reselected.
                this->ReselectSceneNodes(SelectionOption_NoStateStaging);
            }
            this->isUpdatingFromStateStack = false;
            this->UnlockParentChangedEvents();
        }
    }

    void SceneEditor::Redo()
    {
        if (this->scene.GetStateStackCurrentFrameIndex() < this->scene.GetStateStackMaxFrameIndex())
        {
            this->LockParentChangedEvents();
            this->isUpdatingFromStateStack = true;
            {
                // If the physics simulation is running or the game is playing, it needs to be stopped first.
                if (this->IsPlaying() || this->IsPaused())
                {
                    this->StopPlaying();
                }
                else if (this->IsPhysicsSimulationEnabled())
                {
                    this->DisablePhysicsSimulation();
                }


                // We deselect everything because we're going to be reselecting the appropriate nodes after the state change.
                this->DeselectAll(SelectionOption_NoScriptNotify | SelectionOption_NoStateStaging);

                this->scene.SeekStateStack(+1);
                this->MarkAsModified();

                // All nodes need to be reselected.
                this->ReselectSceneNodes(SelectionOption_NoStateStaging);
            }
            this->isUpdatingFromStateStack = false;
            this->UnlockParentChangedEvents();
        }
    }

    void SceneEditor::CommitStateStackFrame(bool markAsModified)
    {
        // We will never do this if a simulation is running.
        if (this->IsStopped())
        {
            this->scene.CommitStateStackFrame();

            if (markAsModified)
            {
                this->MarkAsModified();
            }
        }
    }

    void SceneEditor::ClearStateStackStagingArea()
    {
        this->scene.ClearStateStackStagingArea();
    }


    bool SceneEditor::CreatePrefab(const char* absolutePath, const char* makeRelativeTo, SceneNode &sceneNode)
    {
        bool successful = false;

        auto prefab = PrefabLibrary::Acquire(absolutePath, makeRelativeTo);
        if (prefab != nullptr)
        {
            prefab->SetFromSceneNode(sceneNode);

            // We want to link the scene node to the prefab.
            this->LinkSceneNodeToPrefab(sceneNode, GTCore::IO::ToRelativePath(absolutePath, makeRelativeTo).c_str(), true);
            this->CommitStateStackFrame();      // <-- Undo/Redo point.

            if (prefab->WriteToFile())
            {
                // With the prefab written to the file, we now need to force a check of the file system so that events are posted which in turn
                // will cause the scene editor to update anything linked to the prefab.
                this->GetOwnerEditor().GetGame().GetDataFilesWatcher().CheckForChangesAndDispatchEvents();

                // If this was created from via the scene editor, we want to make sure any scene nodes that have updated in accordance
                // with the new prefab definition are not part of the undo/redo stack. To do this, we just clear the undo/redo staging
                // area. If we don't do this, the next undo will actually revert the changed scene nodes to their previous state, which
                // is definately not what we want.
                this->ClearStateStackStagingArea();

                successful = true;
            }

            PrefabLibrary::Unacquire(prefab);
        }


        return successful;
    }

    SceneNode* SceneEditor::InstantiatePrefab(const char* relativePath)
    {
        SceneNode* rootSceneNode = nullptr;

        this->LockParentChangedEvents();
        this->isInstantiatingPrefab = true;
        {
            rootSceneNode = this->scene.CreateNewSceneNode();
            assert(rootSceneNode != nullptr);
            {
                if (this->LinkSceneNodeToPrefab(*rootSceneNode, relativePath))
                {
                    // We also want to recursively deselect the scene nodes. The reason we do this is because the metadata component may have
                    // left it marked as selected, which we don't want. We don't post notifications to the editor about this.
                    this->DeselectSceneNodeAndChildren(*rootSceneNode, SelectionOption_NoScriptNotify);
                }
                else
                {
                    this->scene.RemoveSceneNode(*rootSceneNode);
                    rootSceneNode = nullptr;
                }
            }
        }
        this->isInstantiatingPrefab = false;
        this->UnlockParentChangedEvents();

        return rootSceneNode;
    }


    bool SceneEditor::LinkSceneNodeToPrefab(SceneNode &sceneNode, const char* prefabRelativePath, bool isSourceSceneNode)
    {
        return this->prefabLinker.LinkSceneNodeToPrefab(sceneNode, prefabRelativePath, isSourceSceneNode);
    }

    void SceneEditor::UnlinkSceneNodeFromPrefab(SceneNode &sceneNode)
    {
        auto rootSceneNode = this->prefabLinker.FindRootSceneNode(sceneNode);
        if (rootSceneNode != nullptr)
        {
            this->prefabLinker.UnlinkSceneNodeFromPrefab(*rootSceneNode, false);
        }
        else
        {
            // If we get here it means the scene node is linked to a prefab, but is not longer a child to the root node of the prefab. In this case
            // we just unlink as if it was the base node.
            this->prefabLinker.UnlinkSceneNodeFromPrefab(sceneNode, false);
        }
    }


    ///////////////////////////////////////////////////
    // Gizmo Control

    void SceneEditor::SwitchGizmoToTranslateMode()
    {
        this->gizmoTransformMode = GizmoTransformMode_Translate;
        this->UpdateGizmo();
    }

    void SceneEditor::SwitchGizmoToRotateMode()
    {
        this->gizmoTransformMode = GizmoTransformMode_Rotate;
        this->UpdateGizmo();
    }

    void SceneEditor::SwitchGizmoToScaleMode()
    {
        this->gizmoTransformMode = GizmoTransformMode_Scale;
        this->UpdateGizmo();
    }


    void SceneEditor::SwitchGizmoToLocalSpace()
    {
        this->gizmoTransformSpace = GizmoTransformSpace_Local;
        this->UpdateGizmo();
        this->UpdatePivotPoint();
    }

    void SceneEditor::SwitchGizmoToGlobalSpace()
    {
        this->gizmoTransformSpace = GizmoTransformSpace_Global;
        this->UpdateGizmo();
        this->UpdatePivotPoint();
    }

    void SceneEditor::ToggleGizmoSpace()
    {
        if (this->IsGizmoInLocalSpace())
        {
            this->SwitchGizmoToGlobalSpace();
        }
        else
        {
            this->SwitchGizmoToLocalSpace();
        }
    }

    bool SceneEditor::IsGizmoInLocalSpace() const
    {
        return this->gizmoTransformSpace == GizmoTransformSpace_Local;
    }

    bool SceneEditor::IsGizmoInGlobalSpace() const
    {
        return this->gizmoTransformSpace == GizmoTransformSpace_Global;
    }

    void SceneEditor::UpdateGizmoTransform(bool onlyUpdateVisibleHandles)
    {
        // Position.
        glm::vec3 position = this->GetSelectionCenterPoint();
        
        // Orientation.
        glm::quat orientation = this->GetGizmoRotation();

        // Scale.
        glm::vec3 windowPos = this->scene.GetDefaultViewport().Project(position);
        windowPos.y += 64.0f;

        glm::vec3 scale(glm::distance(this->scene.GetDefaultViewport().Unproject(windowPos), position));


        // Now we just update in one go.
        this->transformGizmo.SetTransform(position, orientation, scale, *this->scene.GetViewportByIndex(0).GetCameraNode(), onlyUpdateVisibleHandles);
    }


    void SceneEditor::SetTranslationSnappingInterval(float interval)
    {
        this->pivotPoint.SetTranslationSnappingInterval(interval);
    }

    float SceneEditor::GetTranslationSnappingInterval() const
    {
        return this->pivotPoint.GetTranslationSnappingInterval();
    }


    void SceneEditor::EnableSnapToGrid()
    {
        this->pivotPoint.EnableSnapToGrid();
    }

    void SceneEditor::DisableSnapToGrid()
    {
        this->pivotPoint.DisableSnapToGrid();
    }

    bool SceneEditor::IsSnappingToGrid() const
    {
        return this->pivotPoint.IsSnappingToGrid();
    }


    ///////////////////////////////////////////////////
    // Scene Events
    //
    // These method should only be called for the active scene.

    void SceneEditor::OnSceneNodeAdded(SceneNode &sceneNode)
    {
        // We need a metadata component if we don't already have one.
        auto metadata = sceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata == nullptr)
        {
            metadata = sceneNode.AddComponent<EditorMetadataComponent>();
            this->OnSceneNodeComponentChanged(sceneNode, *metadata);
        }


        // Picking volume must be set here.
        if (metadata->UseModelForPickingShape() && sceneNode.HasComponent<ModelComponent>())
        {
            metadata->SetPickingCollisionShapeToModel();
        }


        assert(metadata != nullptr);
        {
            // Picking shapes need to be created.
            //
            // We always create the shapes regardless of whether or not they're visible. We only add to the picking world if it's visible, though.
            auto &pickingCollisionObject = metadata->GetPickingCollisionObject();

            if (metadata->UseModelForPickingShape())
            {
                metadata->SetPickingCollisionShapeToModel();

                if (metadata->GetPickingCollisionShape())
                {
                    pickingCollisionObject.getCollisionShape()->setLocalScaling(ToBulletVector3(sceneNode.GetWorldScale()));
                }
            }


            if (sceneNode.IsVisible())
            {
                if (metadata->GetPickingCollisionShape() != nullptr)
                {
                    btTransform transform;
                    sceneNode.GetWorldTransform(transform);
                    pickingCollisionObject.setWorldTransform(transform);

                    this->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                }


                // If we have a sprite, we'll want to add it's picking object to the picking world.
                if (metadata->IsUsingSprite() && metadata->GetSpritePickingCollisionObject() != nullptr)
                {
                    this->pickingWorld.AddCollisionObject(*metadata->GetSpritePickingCollisionObject(), metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                }


                if (metadata->IsUsingSprite())
                {
                    auto scene = sceneNode.GetScene();
                    assert(scene != nullptr);
                    {
                        scene->GetRenderer().AddExternalMesh(metadata->GetSpriteMesh());
                    }
                }

                if (metadata->IsShowingDirectionArrow())
                {
                    auto scene = sceneNode.GetScene();
                    assert(scene != nullptr);
                    {
                        scene->GetRenderer().AddExternalMesh(metadata->GetDirectionArrowMesh());
                    }
                }
            }

                


            // Select the scene node if it's marked as such, but only if we're running. Indeed, if we're running, we actually want to explicitly deselect.
            if (metadata->IsSelected())
            {
                if (this->IsPlaying() || this->IsPaused())
                {
                    metadata->Deselect();
                }
                else
                {
                    this->SelectSceneNode(sceneNode, SelectionOption_NoScriptNotify | SelectionOption_Force);      // <-- 'true' means to force the selection so that the scripting environment is aware of it.
                }
            }


            // We need to let the editor know about this. It will need to do things like add it to the hierarchy explorer.
            this->PostOnSceneNodeAddedToScript(sceneNode);
        }
    }

    void SceneEditor::OnSceneNodeRemoved(SceneNode &sceneNode)
    {
        auto metadata = sceneNode.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            // Collision shapes need to be removed.
            this->pickingWorld.RemoveCollisionObject(metadata->GetPickingCollisionObject());

            if (metadata->IsUsingSprite() && metadata->GetSpritePickingCollisionObject() != nullptr)
            {
                this->pickingWorld.RemoveCollisionObject(*metadata->GetSpritePickingCollisionObject());
            }


            auto scene = sceneNode.GetScene();
            assert(scene != nullptr);
            {
                scene->GetRenderer().RemoveExternalMesh(metadata->GetSpriteMesh());
                scene->GetRenderer().RemoveExternalMesh(metadata->GetDirectionArrowMesh());
            }


            // We need to make sure scene nodes are deseleted when they are removed from the scene.
            this->DeselectSceneNode(sceneNode, 0);


            // We need to let the editor know about this. It will need to do things like remove it from the hierarchy explorer.
            this->PostOnSceneNodeRemovedToScript(sceneNode);
        }
    }

    void SceneEditor::OnSceneNodeNameChanged(SceneNode &node)
    {
        // We need to let the scripting environment know about this change.
        this->PostOnSceneNodeNameChangedToScript(node);
    }

    void SceneEditor::OnSceneNodeParentChanged(SceneNode &node, SceneNode* previousParent)
    {
        this->PostOnSceneNodeParentChangedToScript(node, previousParent);       // <-- Should probably move this into the conditional below, but it will break the hierarchy tree-view updates when things like undo/redo, etc are performed.

        if (!this->IsParentChangedEventsLocked())
        {
            auto prefabComponent = node.GetComponent<PrefabComponent>();
            if (prefabComponent != nullptr)
            {
                if (prefabComponent->GetLocalHierarchyID() != 1)
                {
                    assert(previousParent != nullptr);
                    {
                        this->UnlinkSceneNodeFromPrefab(*previousParent);
                        this->UnlinkSceneNodeFromPrefab(node);
                    }
                }
            }
        }
    }

    void SceneEditor::OnSceneNodeTransform(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            // We need to remove and re-add the collision shape since it might have changed.
            auto &pickingCollisionObject = metadata->GetPickingCollisionObject();


            btTransform transform;
            node.GetWorldTransform(transform);

            pickingCollisionObject.setWorldTransform(transform);

            auto world = pickingCollisionObject.GetWorld();
            if (world != nullptr)
            {
                world->UpdateAABB(pickingCollisionObject);
            }


            // The picking collision shape of the sprite will also need to be updated.
            if (metadata->IsUsingSprite())
            {
                metadata->ApplyTransformToSprite();
            }



            // If the scene node was selected, we should reposition the gizmo.
            if (metadata->IsSelected())
            {
                this->UpdateGizmoTransform();
            }


            if (this->selectedNodes.count == 1 && node.GetID() == this->selectedNodes[0])
            {
                this->UpdatePropertiesTransformPanel();
            }
        }
    }

    void SceneEditor::OnSceneNodeScale(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            // We need to remove and re-add the collision shape since it might have changed.
            auto &pickingCollisionObject = metadata->GetPickingCollisionObject();

            auto world = pickingCollisionObject.GetWorld();
            if (world != nullptr)
            {
                world->RemoveCollisionObject(pickingCollisionObject);
            }

            if (metadata->GetPickingCollisionShape() != nullptr)
            {
                if (metadata->UseModelForPickingShape())
                {
                    metadata->GetPickingCollisionShape()->setLocalScaling(ToBulletVector3(node.GetWorldScale()));
                }

                if (node.IsVisible())
                {
                    this->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                }
            }

            if (this->selectedNodes.count == 1 && node.GetID() == this->selectedNodes[0])
            {
                this->UpdatePropertiesTransformPanel();
            }

            metadata->MarkCollisionShapeMeshAsDirty();
            metadata->MarkProximityShapeMeshAsDirty();
        }
    }

    void SceneEditor::OnSceneNodeHide(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            this->pickingWorld.RemoveCollisionObject(metadata->GetPickingCollisionObject());

            if (metadata->IsUsingSprite())
            {
                assert(metadata->GetSpritePickingCollisionObject() != nullptr);
                {
                    this->pickingWorld.RemoveCollisionObject(*metadata->GetSpritePickingCollisionObject());
                }
            }


            // External Meshes.
            auto scene = node.GetScene();
            assert(scene != nullptr);
            {
                // Sprite.
                if (metadata->IsUsingSprite())
                {
                    scene->GetRenderer().RemoveExternalMesh(metadata->GetSpriteMesh());
                }

                // Direction Arrow.
                if (metadata->IsShowingDirectionArrow())
                {
                    scene->GetRenderer().RemoveExternalMesh(metadata->GetDirectionArrowMesh());
                }
            }
        }
    }

    void SceneEditor::OnSceneNodeShow(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            // The picking objects need to be positioned when shown, so we'll grab the transform now.
            btTransform transform;
            node.GetWorldTransform(transform);


            if (metadata->GetPickingCollisionShape() != nullptr)
            {
                metadata->GetPickingCollisionObject().setWorldTransform(transform);
                this->pickingWorld.AddCollisionObject(metadata->GetPickingCollisionObject(), metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
            }

            if (metadata->IsUsingSprite())
            {
                assert(metadata->GetSpritePickingCollisionObject() != nullptr);
                {
                    metadata->GetSpritePickingCollisionObject()->setWorldTransform(transform);
                    this->pickingWorld.AddCollisionObject(*metadata->GetSpritePickingCollisionObject(), metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                }
            }


            // External Meshes.
            auto scene = node.GetScene();
            assert(scene != nullptr);
            {
                // Sprite.
                if (metadata->IsUsingSprite())
                {
                    scene->GetRenderer().AddExternalMesh(metadata->GetSpriteMesh());
                }

                // Direction Arrow.
                if (metadata->IsShowingDirectionArrow())
                {
                    scene->GetRenderer().AddExternalMesh(metadata->GetDirectionArrowMesh());
                }
            }
        }
    }

    void SceneEditor::OnSceneNodeComponentAdded(SceneNode &node, Component &component)
    {
        if (GTCore::Strings::Equal(component.GetName(), EditorMetadataComponent::Name))
        {
            auto &metadata = static_cast<EditorMetadataComponent &>(component);

            auto scene = node.GetScene();
            assert(scene != nullptr);
            {
                auto &renderer = scene->GetRenderer();

                if (metadata.IsUsingSprite())
                {
                    renderer.AddExternalMesh(metadata.GetSpriteMesh());
                }
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), DynamicsComponent::Name))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->MarkCollisionShapeMeshAsDirty();

                if (metadata->IsSelected())
                {
                    metadata->ShowCollisionShapeMesh();
                    this->scene.GetRenderer().AddExternalMesh(metadata->GetCollisionShapeMesh());
                }
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), ProximityComponent::Name))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->MarkProximityShapeMeshAsDirty();

                if (metadata->IsSelected())
                {
                    metadata->ShowProximityShapeMesh();
                    this->scene.GetRenderer().AddExternalMesh(metadata->GetProximityShapeMesh());
                }
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), PrefabComponent::Name))     // Prefab
        {
            this->PostOnSceneNodePrefabChanged(node);
        }
    }

    void SceneEditor::OnSceneNodeComponentRemoved(SceneNode &node, Component &component)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            if (GTCore::Strings::Equal(component.GetName(), EditorMetadataComponent::Name))
            {
                auto scene = node.GetScene();
                assert(scene != nullptr);
                {
                    scene->GetRenderer().RemoveExternalMesh(metadata->GetSpriteMesh());
                    scene->GetRenderer().RemoveExternalMesh(metadata->GetDirectionArrowMesh());
                }
            }
            else if (GTCore::Strings::Equal(component.GetName(), ModelComponent::Name))
            {
                if (metadata->UseModelForPickingShape())
                {
                    metadata->ClearPickingCollisionShape();
                }
            }
            else if (GTCore::Strings::Equal(component.GetName(), DynamicsComponent::Name))
            {
                if (metadata->IsSelected())
                {
                    metadata->HideCollisionShapeMesh();
                    this->scene.GetRenderer().RemoveExternalMesh(metadata->GetCollisionShapeMesh());
                }
            }
            else if (GTCore::Strings::Equal(component.GetName(), ProximityComponent::Name))
            {
                if (metadata->IsSelected())
                {
                    metadata->HideProximityShapeMesh();
                    this->scene.GetRenderer().RemoveExternalMesh(metadata->GetProximityShapeMesh());
                }
            }
            else if (GTCore::Strings::Equal(component.GetName(), PrefabComponent::Name))    // Prefab
            {
                this->PostOnSceneNodePrefabChanged(node);
            }
        }
    }

    void SceneEditor::OnSceneNodeComponentChanged(SceneNode &node, Component &component)
    {
        // If the component is editor metadata, we need to check the selection state.
        if (GTCore::Strings::Equal(component.GetName(), EditorMetadataComponent::Name))
        {
            auto &metadata = static_cast<EditorMetadataComponent &>(component);


            // We need to remove and re-add the collision object since it might have changed. We only re-add if it's visible.
            auto &pickingCollisionObject = metadata.GetPickingCollisionObject();

            auto world = pickingCollisionObject.GetWorld();
            if (world != nullptr)
            {
                world->RemoveCollisionObject(pickingCollisionObject);
            }


            // If the node is visible, we'll need to include the picking collision objects.
            if (node.IsVisible())
            {
                // If the picking shape is set to the model, we want to update it here just to make sure everything is valid.
                if (metadata.UseModelForPickingShape())
                {
                    metadata.SetPickingCollisionShapeToModel();
                }


                if (metadata.GetPickingCollisionShape() != nullptr)
                {
                    this->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata.GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                }


                // If we have a sprite, we'll want to add it's picking object to the picking world.
                if (metadata.IsUsingSprite() && metadata.GetSpritePickingCollisionObject() != nullptr)
                {
                    this->pickingWorld.AddCollisionObject(*metadata.GetSpritePickingCollisionObject(), metadata.GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                }
            }


            // External Meshes.
            auto scene = node.GetScene();
            assert(scene != nullptr);
            {
                auto &renderer = scene->GetRenderer();

                // Sprite.
                if (metadata.IsUsingSprite())
                {
                    renderer.AddExternalMesh(metadata.GetSpriteMesh());
                }
                else
                {
                    renderer.RemoveExternalMesh(metadata.GetSpriteMesh());
                }

                // Direction Arrow.
                if (metadata.IsShowingDirectionArrow())
                {
                    renderer.AddExternalMesh(metadata.GetDirectionArrowMesh());
                }
                else
                {
                    renderer.RemoveExternalMesh(metadata.GetDirectionArrowMesh());
                }
            }
        }
        else
        {
            if (GTCore::Strings::Equal(component.GetName(), ModelComponent::Name))
            {
                // The picking shape needs to be updated.
                auto metadata = node.GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    if (metadata->UseModelForPickingShape())
                    {
                        metadata->SetPickingCollisionShapeToModel();

                        if (metadata->GetPickingCollisionShape() != nullptr && metadata->GetPickingCollisionObject().GetWorld() == nullptr)
                        {
                            this->pickingWorld.AddCollisionObject(metadata->GetPickingCollisionObject(), metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                        }
                    }
                }
            }
            else if (GTCore::Strings::Equal(component.GetName(), DynamicsComponent::Name))
            {
                auto metadata = node.GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    metadata->MarkCollisionShapeMeshAsDirty();
                }
            }
            else if (GTCore::Strings::Equal(component.GetName(), ProximityComponent::Name))
            {
                auto metadata = node.GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    metadata->MarkProximityShapeMeshAsDirty();
                }
            }
            else if (GTCore::Strings::Equal(component.GetName(), PrefabComponent::Name))
            {
                this->PostOnSceneNodePrefabChanged(node);
            }
        }
    }

    void SceneEditor::OnStateStackFrameCommitted()
    {
        // We'll commit a new frame whenever something worth of an undo/redo operation has happened. And when that happens, we want the scene to be marked as modified.
        //
        // We only mark as modified if this is not the initial commit. We can determine this by looking at the number of frames. If there is only 1, it was the initial
        // commit and we don't want to mark as modified in that case.
        if (!this->isDeserializing && this->scene.GetStateStackFrameCount() > 1 && this->scene.IsStateStackStagingEnabled())
        {
            this->MarkAsModified();
        }
    }


    void SceneEditor::OnViewportSize()
    {
        this->UpdateGizmo();

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


    void SceneEditor::OnCameraTransformed()
    {
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void SceneEditor::Show()
    {
        if (this->IsPaused() && this->wasPlayingBeforeHide)
        {
            this->StartPlaying();
            this->wasPlayingBeforeHide = false;
        }

        this->GUI.Main->Show();
    }

    void SceneEditor::Hide()
    {
        // When the editor is hidden, we need to pause playback, but not stop entirely. When the editor is re-shown, it will be resumed.
        if (this->IsPlaying())
        {
            this->PausePlaying();
            this->wasPlayingBeforeHide = true;
        }

        this->GUI.Main->Hide();
    }

    bool SceneEditor::Save()
    {
        FILE* file = GTCore::IO::Open(this->GetAbsolutePath(), GTCore::IO::OpenMode::Write);
        if (file != nullptr)
        {
            // If the physics simulation is running or the game is playing, it needs to be stopped first.
            if (this->IsPlaying() || this->IsPaused())
            {
                this->StopPlaying();
            }
            else if (this->IsPhysicsSimulationEnabled())
            {
                this->DisablePhysicsSimulation();
            }


            GTCore::FileSerializer serializer(file);
            this->SerializeScene(serializer);

            GTCore::IO::Close(file);

            this->UnmarkAsModified();

            return true;
        }

        return false;
    }

    void SceneEditor::OnUpdate(double deltaTimeInSeconds)
    {
        if (this->GUI.Main->IsVisible())
        {
            auto &game = this->GetOwnerEditor().GetGame();

            // If the mouse is captured we may need to move the screen around.
            if (game.IsMouseCaptured())
            {
                const float moveSpeed   = 0.05f;
                const float rotateSpeed = 0.1f;
                const float scaleSpeed  = 0.05f;

                float mouseOffsetX;
                float mouseOffsetY;
                game.GetMouseOffset(mouseOffsetX, mouseOffsetY);

                if (this->IsDraggingGizmo())
                {
                    if (mouseOffsetX != 0.0f || mouseOffsetY != 0.0f)
                    {
                        this->transformedObjectWithGizmo = true;


                        glm::vec3 dragAxis      = this->gizmoDragAxis;
                        float     dragDistance  = glm::length(glm::vec2(mouseOffsetX, -mouseOffsetY));
                        float     dragDirection = glm::dot(glm::normalize(glm::vec2(mouseOffsetX, -mouseOffsetY)), this->gizmoDragFactor);

                        if (this->gizmoDragMode == GizmoDragMode_Rotate)
                        {
                            dragDirection = glm::dot(glm::normalize(glm::vec2(mouseOffsetY, -mouseOffsetX)), this->gizmoDragFactor);
                        }



                        // We need to drag the selected objects.
                        if (this->gizmoDragMode == GizmoDragMode_Translate)
                        {
                            glm::vec3 pivotStartPosition = this->pivotPoint.GetPosition();

                            if (this->gizmoTransformSpace == GizmoTransformSpace_Local && this->selectedNodes.count == 1)
                            {
                                dragAxis = this->pivotPoint.GetOrientation() * dragAxis;
                            }


                            // We just move the pivot point. After the pivot has been moved, we just re-position the scene nodes relative to it's new position.
                            this->pivotPoint.Translate(dragAxis * (dragDirection * dragDistance * moveSpeed));
                            
                            // The selected nodes need to be re-positioned based on the pivot point.
                            for (size_t i = 0; i < this->selectedNodes.count; ++i)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                assert(node != nullptr);
                                {
                                    // We change the world position here. If the node has a parent who is also selected, and position inheritance is enabled, we ignore it.
                                    if (!this->IsAncestorSelected(*node) || !node->IsPositionInheritanceEnabled())
                                    {
                                        const glm::vec3 offsetFromPivot = node->GetWorldPosition() - pivotStartPosition;
                                        node->SetWorldPosition(this->pivotPoint.GetPosition() + offsetFromPivot);
                                    }
                                }
                            }
                        }
                        else if (this->gizmoDragMode == GizmoDragMode_Rotate)
                        {
                            glm::quat pivotStartOrientation = this->pivotPoint.GetOrientation();

                            if (this->gizmoTransformSpace == GizmoTransformSpace_Global || this->selectedNodes.count > 1)
                            {
                                dragAxis = dragAxis * this->pivotPoint.GetOrientation();
                            }


                            float dragAngle = dragDirection * dragDistance * rotateSpeed;
                            this->pivotPoint.Rotate(dragAngle, dragAxis);

                            
                            // The selected nodes need to be re-orientated based on the pivot point.
                            for (size_t i = 0; i < this->selectedNodes.count; ++i)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                assert(node != nullptr);
                                {
                                    // We change the world position here. If the node has a parent who is also selected, and position inheritance is enabled, we ignore it.
                                    if (!this->IsAncestorSelected(*node) || !node->IsPositionInheritanceEnabled())
                                    {
                                        if (this->gizmoTransformSpace == GizmoTransformSpace_Global || this->selectedNodes.count > 1)
                                        {
                                            // Global axis.
                                            const glm::quat offsetOrientationFromPivot = glm::inverse(pivotStartOrientation) * node->GetWorldOrientation();

                                            const glm::quat oldOrientation = node->GetWorldOrientation();
                                            const glm::quat newOrientation = this->pivotPoint.GetOrientation() * offsetOrientationFromPivot;
                                            node->SetWorldOrientation(newOrientation);
                                            
                                            const glm::quat orientationDifference = newOrientation * glm::inverse(oldOrientation);
                                            node->SetWorldPosition(this->pivotPoint.GetPosition() + (orientationDifference * (node->GetWorldPosition() - this->pivotPoint.GetPosition())));
                                        }
                                        else
                                        {
                                            // Local axis.
                                            node->SetWorldOrientation(this->pivotPoint.GetOrientation());
                                        }
                                    }
                                }
                            }
                        }
                        else if (this->gizmoDragMode == GizmoDragMode_Scale)
                        {
                            // Scaling is always done in local space. Global space scaling is sheering, which will never be supported.
                            dragAxis = this->pivotPoint.GetOrientation() * dragAxis;

                            glm::vec3 pivotStartScale = this->pivotPoint.GetScale();


                            // We want to use an additive scale here.
                            this->pivotPoint.AdditiveScale(dragAxis * (dragDirection * dragDistance * scaleSpeed));
                            
                            // The selected nodes need to be re-positioned based on the pivot point.
                            for (size_t i = 0; i < this->selectedNodes.count; ++i)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                assert(node != nullptr);
                                {
                                    // We change the world position here. If the node has a parent who is also selected, and position inheritance is enabled, we ignore it.
                                    if (!this->IsAncestorSelected(*node) || !node->IsPositionInheritanceEnabled())
                                    {
                                        const glm::vec3 offsetFromPivot = node->GetWorldScale() - pivotStartScale;
                                        node->SetWorldScale(glm::max(this->pivotPoint.GetScale() + offsetFromPivot, glm::vec3(0.0f, 0.0f, 0.0f)));
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // We'll update the axis arrows every frame. This was previously event driven, but it's proving to be too much of a hassle.
            if (this->isShowingAxisArrows)
            {
                this->axisArrows.UpdateTransform(this->camera);
            }


            this->scene.Update(deltaTimeInSeconds);


            // If we're playing and have only a single selected node and it has a script component, we want to update the GUI to show those values.
            if (this->IsPlaying())
            {
                if (this->selectedNodes.count == 1)
                {
                    auto selectedNode = this->GetSceneNodeByID(this->selectedNodes[0]);
                    assert(selectedNode != nullptr);
                    {
                        if (selectedNode->HasComponent<ScriptComponent>())
                        {
                            this->UpdateSelecteSceneNodeScriptPropertiesGUI();
                        }
                    }
                }
            }
        }
    }


    void SceneEditor::OnKeyPressed(GTCore::Key key)
    {
        if (key == GTCore::Keys::Ctrl)
        {
            this->pivotPoint.EnableAllSnapping();
        }
    }

    void SceneEditor::OnKeyReleased(GTCore::Key key)
    {
        if (key == GTCore::Keys::Ctrl)
        {
            this->pivotPoint.DisableAllSnapping();
        }
    }


    void SceneEditor::OnMouseButtonDown(GTCore::MouseButton, int, int)
    {
    }

    void SceneEditor::OnMouseButtonUp(GTCore::MouseButton button, int, int)
    {
        if (button == GTCore::MouseButton_Left)
        {
            this->gizmoDragMode = GizmoDragMode_None;
            this->transformGizmo.RestoreColours();

            // If the gizmo was previously being dragged, we will have disabled the viewport camera controls. They need to be re-enabled.
            this->EnableViewportCameraControls();


            if (this->transformedObjectWithGizmo)
            {
                this->CommitStateStackFrame();
                this->transformedObjectWithGizmo = false;
            }
        }
    }

    void SceneEditor::OnMainWindowReceiveFocus()
    {
        if (this->wasPlayingBeforeLosingFocus)
        {
            this->StartPlaying();
            this->wasPlayingBeforeLosingFocus = false;
        }
    }

    void SceneEditor::OnMainWindowLoseFocus()
    {
        this->wasPlayingBeforeLosingFocus = this->IsPlaying();
        
        if (this->IsPlaying())
        {
            this->PausePlaying();
        }
    }


    void SceneEditor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
        // We want to go through and notify the editor of a change to the model component of any scene node referencing this file (if it's a model file).
        if (GTEngine::IO::IsSupportedModelExtension(item.info.path.c_str()))
        {
            size_t sceneNodeCount = this->scene.GetSceneNodeCount();

            for (size_t i = 0; i < sceneNodeCount; ++i)
            {
                auto sceneNode = this->scene.GetSceneNodeByIndex(i);
                assert(sceneNode != nullptr);
                {
                    auto modelComponent = sceneNode->GetComponent<ModelComponent>();
                    if (modelComponent != nullptr)
                    {
                        auto model = modelComponent->GetModel();
                        if (model != nullptr)
                        {
                            if (model->GetDefinition().absolutePath == item.info.absolutePath || GTCore::IO::RemoveExtension(item.info.absolutePath.c_str()) == model->GetDefinition().absolutePath)
                            {
                                modelComponent->OnChanged();

                                // If this model is the only one selected, we will also update the panel. To do this, we'll just let the editor
                                // think that the selection has changed.
                                if (this->GetSelectedSceneNodeCount() == 1 && this->selectedNodes[0] == sceneNode->GetID())
                                {
                                    this->PostOnSelectionChangedEventToScript();
                                }

                                // If this scene node also has a proximity or dynamics component, we'll need to refresh the model convex hulls for them.
                                auto proximityComponent = sceneNode->GetComponent<ProximityComponent>();
                                if (proximityComponent != nullptr)
                                {
                                    proximityComponent->RefreshModelConvexHullsShapes();
                                }

                                auto dynamicsComponent = sceneNode->GetComponent<DynamicsComponent>();
                                if (dynamicsComponent != nullptr)
                                {
                                    dynamicsComponent->RefreshModelConvexHullsShapes();
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (GTEngine::IO::IsSupportedPrefabExtension(item.relativePath.c_str()))
        {
            this->RelinkSceneNodesLinkedToPrefab(item.relativePath.c_str());
        }
        else
        {
            // It might be a script file.
            if (GTCore::Strings::Equal<false>(GTCore::Path::Extension(item.relativePath.c_str()), "lua")    ||
                GTCore::Strings::Equal<false>(GTCore::Path::Extension(item.relativePath.c_str()), "script") ||
                GTCore::Strings::Equal<false>(GTCore::Path::Extension(item.relativePath.c_str()), "gtscript"))
            {
                this->UpdateAllSceneNodesLinkedToScript(item.relativePath.c_str());
            }
        }
    }





    ///////////////////////////////////////////////////
    // Private Methods.

    void SceneEditor::CreateAndAddSystemNodes()
    {
        // There are a few pre-conditions that neeed to be met.
        assert(this->scene.GetSceneNodeCount()     == 0);
        assert(this->scene.GetMinAutoSceneNodeID() == 1);
        {
            this->camera.AttachEventHandler(this->cameraEventHandler);
            this->camera.AddComponent<GTEngine::CameraComponent>();
            this->camera.DisableSerialization();
            this->camera.DisableStateStackStaging();


            this->scene.GetDefaultViewport().SetCameraNode(this->camera);
            this->scene.AddViewport(this->scene.GetDefaultViewport());
            this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);
        }
    }

    void SceneEditor::ApplyCameraRotation()
    {
        this->UpdateGizmoTransform();
    }

    void SceneEditor::SerializeScene(GTCore::Serializer &serializer, bool serializeMetadata) const
    {
        this->scene.Serialize(serializer);

        // We now want to save our own chunk. This will contain metadata such as the camera position and whatnot.
        if (serializeMetadata)
        {
            GTCore::BasicSerializer metadataSerializer;

            this->camera.Serialize(metadataSerializer);

            float cameraXRotation;
            float cameraYRotation;
            this->GetViewportCameraRotation(cameraXRotation, cameraYRotation);

            metadataSerializer.Write(cameraXRotation);
            metadataSerializer.Write(cameraYRotation);

            // View settings.
            metadataSerializer.Write(this->IsShowingGrid());
            metadataSerializer.Write(this->IsShowingAxisArrows());



            Serialization::ChunkHeader header;
            header.id          = Serialization::ChunkID_Scene_EditorMetadata;
            header.version     = 1;
            header.sizeInBytes = metadataSerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(metadataSerializer.GetBuffer(), header.sizeInBytes);
        }
    }


    void SceneEditor::DeserializeScene(GTCore::Deserializer &deserializer)
    {
        this->scene.DisableStateStackStaging();
        this->LockParentChangedEvents();
        this->isDeserializing = true;
        {
            this->transformGizmo.Hide(this->scene.GetRenderer(), this->pickingWorld);


            // The scene deserialize callback for handling non-standard chunks.
            struct MySceneDeserializeCallback : public SceneDeserializeCallback
            {
                SceneEditor &m_sceneEditor;


                MySceneDeserializeCallback(SceneEditor &sceneEditor)
                    : m_sceneEditor(sceneEditor)
                {
                }

                bool IsChunkHandled(const Serialization::ChunkHeader &header) const
                {
                    return header.id == Serialization::ChunkID_Scene_EditorMetadata;
                }

                bool HandleChunk(const Serialization::ChunkHeader &header, GTCore::Deserializer &deserializer)
                {
                    if (header.id == Serialization::ChunkID_Scene_EditorMetadata)
                    {
                        auto &camera = m_sceneEditor.GetCameraSceneNode();

                        camera.Deserialize(deserializer);
                        camera.DisableSerialization();
                        camera.DisableStateStackStaging();

                        float cameraXRotation;
                        float cameraYRotation;
                        deserializer.Read(cameraXRotation);
                        deserializer.Read(cameraYRotation);
                        m_sceneEditor.SetViewportCameraRotation(cameraXRotation, cameraYRotation);

                        bool isShowingGrid;
                        bool isShowingAxisArrows;
                        deserializer.Read(isShowingGrid);
                        deserializer.Read(isShowingAxisArrows);

                        if (isShowingGrid)
                        {
                            m_sceneEditor.ShowGrid();
                        }
                        else
                        {
                            m_sceneEditor.HideGrid();
                        }

                        if (isShowingAxisArrows)
                        {
                            m_sceneEditor.ShowAxisArrows();
                        }
                        else
                        {
                            m_sceneEditor.HideAxisArrows();
                        }


                        // TODO: Delete this later. It only exists for backwards compatibility.
                        // We need to peek at the next bytes. If it's a state stack, we need to deserialize it.
                        Serialization::ChunkHeader stateStackHeader;
                        if (deserializer.Peek(&stateStackHeader, sizeof(stateStackHeader)) == sizeof(stateStackHeader) && stateStackHeader.id == Serialization::ChunkID_SceneStateStack)
                        {
                            deserializer.Seek(sizeof(stateStackHeader));
                            deserializer.Seek(stateStackHeader.sizeInBytes);
                        }

                        return true;
                    }

                    return false;
                }


            private:    // No copying. Warning silencers.
                MySceneDeserializeCallback(const MySceneDeserializeCallback &other) : m_sceneEditor(other.m_sceneEditor) {}
                MySceneDeserializeCallback & operator=(const MySceneDeserializeCallback &) { return *this; }
            }deserializeCallback(*this);


            // With pre-deserialization done, we can now do a full deserialization of the scene.
            this->scene.Deserialize(deserializer, deserializeCallback);

            // The projection and aspect ratios of the camera may not be correct for the viewport dimensions, so we'll simulate
            // a viewport resize by calling the OnSize event directly.
            this->viewportEventHandler.OnSize(*this->GUI.Viewport);

            // Need an initial commit.
            this->CommitStateStackFrame(DontMarkAsModified);


            // Gizmo should be updated now.
            this->UpdateGizmo();
        }
        this->isDeserializing = false;
        this->UnlockParentChangedEvents();
        this->scene.EnableStateStackStaging();


        // We'll let the editor do it's thing with selections.
        this->PostOnSelectionChangedEventToScript();

        // We'll also want to update the properties panel.
        this->RefreshScenePropertiesPanel();
    }

    void SceneEditor::SerializeSceneNodes(const GTCore::Vector<size_t> &sceneNodeIDs, GTCore::Serializer &serializer)
    {
        for (size_t i = 0; i < sceneNodeIDs.count; ++i)
        {
            auto node = this->GetSceneNodeByID(sceneNodeIDs[i]);
            assert(node != nullptr);
            {
                node->Serialize(serializer);
            }
        }
    }

    void SceneEditor::DeserializeSceneNodes(const GTCore::Vector<size_t> &sceneNodeIDs, GTCore::Deserializer &deserializer)
    {
        for (size_t i = 0; i < sceneNodeIDs.count; ++i)
        {
            auto node = this->GetSceneNodeByID(sceneNodeIDs[i]);
            if (node == nullptr)
            {
                node = new SceneNode;
            }

            node->Deserialize(deserializer);

            // If the node is not contained in the editor's scene, we need to add it.
            if (node->GetScene() != &this->scene)
            {
                this->scene.AddSceneNode(*node);
            }
        }
    }


    void SceneEditor::ShowGizmo()
    {
        this->transformGizmo.Show(this->scene.GetRenderer(), this->pickingWorld);
        this->UpdateGizmo();
    }

    void SceneEditor::HideGizmo()
    {
        this->transformGizmo.Hide(this->scene.GetRenderer(), this->pickingWorld);
    }



#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunreachable-code"
#endif

    void SceneEditor::ShowGizmoHandles()
    {
        switch (this->gizmoTransformMode)
        {
        case GizmoTransformMode_Translate:
            {
                this->transformGizmo.ShowTranslationHandles(this->scene.GetRenderer(), this->pickingWorld);
                break;
            }

        case GizmoTransformMode_Rotate:
            {
                this->transformGizmo.ShowRotationHandles(this->scene.GetRenderer(), this->pickingWorld);
                break;
            }

        case GizmoTransformMode_Scale:
            {
                this->transformGizmo.ShowScaleHandles(this->scene.GetRenderer(), this->pickingWorld);
                break;
            }

        default: break;
        }
    }
    
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif
    
    

    void SceneEditor::UpdateGizmo()
    {
        this->UpdateGizmoTransform(false);  // 'false' means to update all handles.

        if (this->IsAnythingSelected())
        {
            this->ShowGizmoHandles();
        }
        else
        {
            this->HideGizmo();
        }
    }

    bool SceneEditor::IsDraggingGizmo() const
    {
        return this->gizmoDragMode != GizmoDragMode_None;
    }


    void SceneEditor::UpdatePivotPoint()
    {
        // The position of the pivot point needs to be the center of the selected objects.
        this->pivotPoint.SetPosition(this->GetSelectionCenterPoint());

        // The orientation and scale will depend on what's selected. If we only have one thing selected, we'll set it to that. Otherwise we
        // set it to the respective identities.
        if (this->selectedNodes.count == 1)
        {
            const auto selectedNode = this->GetSceneNodeByID(this->selectedNodes[0]);
            assert(selectedNode != nullptr);
            {
                if (this->gizmoTransformSpace == GizmoTransformSpace_Local)
                {
                    this->pivotPoint.SetOrientation(selectedNode->GetWorldOrientation());
                }
                else
                {
                    this->pivotPoint.SetOrientation(glm::quat());
                }

                this->pivotPoint.SetScale(selectedNode->GetWorldScale());
            }
        }
        else
        {
            this->pivotPoint.SetOrientation(glm::quat());
            this->pivotPoint.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }


    void SceneEditor::PostOnSelectionChangedEventToScript()
    {
        if (this->GUI.Main != nullptr)
        {
            auto &script = this->GetScript();

            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("OnSelectionChanged");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // <-- 'self'.
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }
    }

    void SceneEditor::PostOnSceneNodeAddedToScript(SceneNode &node)
    {
        assert(this->GUI.Main != nullptr);
        {
            auto &script = this->GetScript();

            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("OnSceneNodeAdded");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // <-- 'self'.
                    script.Push(&node);
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }
    }

    void SceneEditor::PostOnSceneNodeRemovedToScript(SceneNode &node)
    {
        assert(this->GUI.Main != nullptr);
        {
            auto &script = this->GetScript();

            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("OnSceneNodeRemoved");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // <-- 'self'.
                    script.Push(&node);
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }
    }

    void SceneEditor::PostOnSceneNodeNameChangedToScript(SceneNode &node)
    {
        assert(this->GUI.Main != nullptr);
        {
            auto &script = this->GetScript();

            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("OnSceneNodeNameChanged");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // <-- 'self'.
                    script.Push(&node);
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }
    }

    void SceneEditor::PostOnSceneNodeParentChangedToScript(SceneNode &node, SceneNode* previousParent)
    {
        assert(this->GUI.Main != nullptr);
        {
            auto &script = this->GetScript();

            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("OnSceneNodeParentChanged");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // <-- 'self'.
                    script.Push(&node);
                    script.Push(previousParent);
                    script.Call(3, 0);
                }
            }
            script.Pop(1);
        }
    }

    void SceneEditor::PostOnSceneNodePrefabChanged(SceneNode &node)
    {
        assert(this->GUI.Main != nullptr);
        {
            auto &script = this->GetScript();

            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("OnSceneNodePrefabChanged");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // <-- 'self'.
                    script.Push(&node);
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }
    }

    void SceneEditor::UpdatePropertiesTransformPanel()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("UpdatePropertiesTransformPanel");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);       // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }

    void SceneEditor::UpdateSelecteSceneNodeScriptPropertiesGUI()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("UpdateScriptProperties");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }

    
    void SceneEditor::UpdateViewportMenuGUI()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("UpdateViewportMenu");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }


    void SceneEditor::RefreshScenePropertiesPanel()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("RefreshSceneProperties");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }


    void SceneEditor::EnableViewportCameraControls()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("EnableViewportCameraControls");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }

    void SceneEditor::DisableViewportCameraControls()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("DisableViewportCameraControls");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }


    void SceneEditor::SetViewportCameraRotation(float rotationX, float rotationY)
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("SetViewportCameraRotation");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Push(rotationX);
                script.Push(rotationY);
                script.Call(3, 0);
            }
        }
        script.Pop(1);
    }

    void SceneEditor::GetViewportCameraRotation(float &rotationX, float &rotationY) const
    {
        auto &script = const_cast<GTEngine::GameScript &>(this->GetScript());

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("GetViewportCameraRotation");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 2);

                rotationX = script.ToFloat(-2);
                rotationY = script.ToFloat(-1);

                script.Pop(2);  // The two return values.
            }
        }
        script.Pop(1);
    }


    void SceneEditor::ReselectSceneNodes(unsigned int options)
    {
        // Grab the nodes marked as selected.
        GTCore::Vector<SceneNode*> nodesForReselection;
        
        size_t sceneNodeCount = this->scene.GetSceneNodeCount();
        for (size_t i = 0; i < sceneNodeCount; ++i)
        {
            auto sceneNode = this->scene.GetSceneNodeByIndex(i);
            assert(sceneNode != nullptr);
            {
                auto metadata = sceneNode->GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    if (metadata->IsSelected())
                    {
                        nodesForReselection.PushBack(sceneNode);
                    }

                    // We always want to do this regardless of whether or not the scene node is marked as selected in the metadata.
                    this->DeselectSceneNode(*sceneNode, options | SelectionOption_NoScriptNotify | SelectionOption_Force);
                }
            }
        }


        // Reselect.
        for (size_t i = 0; i < nodesForReselection.count; ++i)
        {
            auto sceneNode = nodesForReselection[i];
            assert(sceneNode != nullptr);
            {
                this->SelectSceneNode(*sceneNode, options | SelectionOption_NoScriptNotify | SelectionOption_Force);
            }
        }


        // The scripting environment needs to know about a change in selection.
        if (!(options & SelectionOption_NoScriptNotify))
        {
            this->PostOnSelectionChangedEventToScript();
        }
    }


    SceneNode & SceneEditor::CopySceneNodeAndChildren(SceneNode &nodeToCopy, SceneNode* parentNode)
    {
        SceneNode* newNode = nullptr;


        this->LockParentChangedEvents();
        {
            GTCore::BasicSerializer serializer;
            nodeToCopy.Serialize(serializer);


            GTCore::BasicDeserializer deserializer(serializer.GetBuffer(), serializer.GetBufferSizeInBytes());
            newNode = this->scene.CreateNewSceneNode(deserializer, true);      // <-- 'true' means to generate a new ID if a node of the same ID already exists (spoiler: it does already exist).



            // We now want to link the new node to the parent, if we have one.
            if (parentNode != nullptr)
            {
                parentNode->AttachChild(*newNode);     // <-- This does an implicit Scene::AddSceneNode().
            }

            // Node needs to be selected, but we don't want to notify the editor yet (we'll do it in one go at a higher level).
            this->SelectSceneNode(*newNode, SelectionOption_NoScriptNotify);



            // And now we need to make a copy of the children. The parent will be the new node.
            for (auto childNode = nodeToCopy.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
            {
                this->CopySceneNodeAndChildren(*childNode, newNode);
            }
        }
        this->UnlockParentChangedEvents();


        assert(newNode != nullptr);
        return *newNode;
    }


    void SceneEditor::UpdatePlaybackControls()
    {
        if (this->GUI.Main != nullptr)
        {
            auto &script = this->GetScript();

            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("UpdatePlaybackControls");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // <-- 'self'.
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }
    }

    void SceneEditor::RelinkSceneNodesLinkedToPrefab(const char* prefabRelativePath)
    {
        // The first step is to extract the root nodes that are linked to the prefab.
        GTCore::Vector<SceneNode*> rootSceneNodes;
        for (size_t iSceneNode = 0; iSceneNode < this->scene.GetSceneNodeCount(); ++iSceneNode)
        {
            auto sceneNode = this->scene.GetSceneNodeByIndex(iSceneNode);
            assert(sceneNode != nullptr);
            {
                auto prefabComponent = sceneNode->GetComponent<PrefabComponent>();
                if (prefabComponent != nullptr)
                {
                    if (GTCore::Strings::Equal(prefabComponent->GetPrefabRelativePath(), prefabRelativePath))
                    {
                        if (prefabComponent->GetLocalHierarchyID() == 1)    // Is it the root node in the prefab?
                        {
                            rootSceneNodes.PushBack(sceneNode);
                        }
                    }
                }
            }
        }


        // Now we just iterate over the root nodes and re-link, deleting children that should no longer exist.
        for (size_t iSceneNode = 0; iSceneNode < rootSceneNodes.count; ++iSceneNode)
        {
            auto sceneNode = rootSceneNodes[iSceneNode];
            assert(sceneNode != nullptr);
            {
                this->LinkSceneNodeToPrefab(*sceneNode, prefabRelativePath);
            }
        }
    }


    void SceneEditor::UpdateAllSceneNodesLinkedToScript(const char* scriptRelativePath)
    {
        // If the scene is registered to the script we're going to reload the definition.
        if (this->scene.GetRegisteredScript() != nullptr)
        {
            if (ScriptLibrary::IsLoaded(scriptRelativePath))
            {
                auto definition = ScriptLibrary::Acquire(scriptRelativePath);
                assert(definition != nullptr);
                {
                    Scripting::LoadScriptDefinition(*this->scene.GetRegisteredScript(), scriptRelativePath, definition->GetScriptString());
                }
                ScriptLibrary::Unacquire(definition);
            }
        }



        // If the selected node is linked to the given script, we'll need to let the scripting environment know about it.
        bool notifyScriptingEnvironment = false;


        size_t sceneNodeCount = this->scene.GetSceneNodeCount();

        for (size_t i = 0; i < sceneNodeCount; ++i)
        {
            auto sceneNode = this->scene.GetSceneNodeByIndex(i);
            assert(sceneNode != nullptr);
            {
                auto scriptComponent = sceneNode->GetComponent<ScriptComponent>();
                if (scriptComponent != nullptr)
                {
                    for (size_t iScript = 0; iScript < scriptComponent->GetScriptCount(); ++iScript)
                    {
                        auto script = scriptComponent->GetScriptDefinitionByIndex(iScript);
                        assert(script != nullptr);
                        {
                            if (GTCore::Strings::Equal(script->GetRelativePath(), scriptRelativePath))
                            {
                                scriptComponent->ReloadScript(iScript);

                                // After the script component itself has been reloaded we want to update the Lua object as well if the scene is currently registered.
                                if (this->scene.GetRegisteredScript() != nullptr)
                                {
                                    Scripting::RegisterComponent(*this->scene.GetRegisteredScript(), *sceneNode, ScriptComponent::Name);
                                }



                                // The scripting environment might need to know about this.
                                if (this->selectedNodes.count == 1 && this->selectedNodes[0] == sceneNode->GetID())
                                {
                                    notifyScriptingEnvironment = true;
                                }
                                
                                // We can break from the iScript loop because we should not logically have a script of the same name.
                                break;
                            }
                        }
                    }
                }
            }
        }


        // The more correct way of doing this would be just to updating the Script panel, but we'll just post an OnSelectionChanged event for now.
        if (notifyScriptingEnvironment)
        {
            this->PostOnSelectionChangedEventToScript();
        }
    }


    void SceneEditor::ResetAllParticleSystems()
    {
        size_t sceneNodeCount = this->scene.GetSceneNodeCount();

        for (size_t iSceneNode = 0; iSceneNode < sceneNodeCount; ++iSceneNode)
        {
            auto sceneNode = this->scene.GetSceneNodeByIndex(iSceneNode);
            assert(sceneNode != nullptr);
            {
                auto particleSystemComponent = sceneNode->GetComponent<ParticleSystemComponent>();
                if (particleSystemComponent != nullptr)
                {
                    particleSystemComponent->Reset();
                }
            }
        }
    }


    void SceneEditor::LockParentChangedEvents()
    {
        this->parentChangedLockCounter += 1;
    }

    void SceneEditor::UnlockParentChangedEvents()
    {
        assert(this->parentChangedLockCounter > 0);

        this->parentChangedLockCounter -= 1;
    }

    bool SceneEditor::IsParentChangedEventsLocked() const
    {
        return this->parentChangedLockCounter > 0;
    }


    void SceneEditor::CapturePauseState()
    {
        this->pauseState.wasMouseCaptured = this->GetOwnerEditor().GetGame().IsMouseCaptured();
        this->pauseState.cameraNode       = this->scene.GetDefaultViewport().GetCameraNode();
    }

    void SceneEditor::RestorePauseState()
    {
        // Mouse capture
        if (this->pauseState.wasMouseCaptured)
        {
            this->GetOwnerEditor().GetGame().CaptureMouse();
        }
        else
        {
            this->GetOwnerEditor().GetGame().ReleaseMouse();
        }

        // Camera
        if (this->pauseState.cameraNode != nullptr)
        {
            this->SetViewportCamera(*this->pauseState.cameraNode);
        }



        // Defaults.
        this->pauseState.wasMouseCaptured = false;
        this->pauseState.cameraNode       = nullptr;
    }


    void SceneEditor::HideAllSpritesAndDirectionalArrows()
    {
        for (size_t iSceneNode = 0; iSceneNode < this->scene.GetSceneNodeCount(); ++iSceneNode)
        {
            auto sceneNode = this->scene.GetSceneNodeByIndex(iSceneNode);
            assert(sceneNode != nullptr);
            {
                auto metadata = sceneNode->GetComponent<EditorMetadataComponent>();
                if (metadata != nullptr)
                {
                    metadata->HideSprite();
                    metadata->HideDirectionArrow();
                }
            }
        }
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
