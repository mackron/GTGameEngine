// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/SceneNodeClassLibrary.hpp>
#include <GTEngine/IO.hpp>
#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>
#include <GTCore/Path.hpp>


#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    SceneEditor::SceneEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          viewport(), camera(), cameraXRotation(0.0f), cameraYRotation(0.0f),
          updateManager(camera), physicsManager(), cullingManager(),
          scene(updateManager, physicsManager, cullingManager), sceneEventHandler(*this),
          viewportEventHandler(ownerEditor.GetGame(), viewport),
          selectedNodes(), selectedNodesBeforePhysicsSimulation(),
          pickingWorld(),
          transformGizmo(), gizmoDragAxis(1.0f, 0.0f, 0.0f), gizmoDragFactor(1.0f, 0.0f),
          gizmoDragMode(GizmoDragMode_None), gizmoTransformMode(GizmoTransformMode_Translate), gizmoTransformSpace(GizmoTransformSpace_Global),
          snapTranslation(), snapAngle(0.0f), snapScale(), isSnapping(false),
          translateSnapSize(0.25f), rotateSnapSize(5.625f), scaleSnapSize(0.25f),
          transformedObjectWithGizmo(false),
          isDeserializing(false),
          GUI()
    {
        this->scene.AttachEventHandler(this->sceneEventHandler);

        // We will add the system scene nodes to the scene before setting the minimum automatic scene node IDs. This will save
        // us from having to manually set IDs on the system nodes.
        this->CreateAndAddSystemNodes();


        // We want to have a minimum ID for automatically generated IDs so that we can have enough room for system nodes such
        // as the camera and gizmos.
        this->scene.SetMinAutoSceneNodeID(16384);

        


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


            // The scene will be done loading by this pointer, so we can close the file.
            GTCore::IO::Close(file);
        }
    }

    SceneEditor::~SceneEditor()
    {
        // GUI elements need to be deleted. We will delete the toolbar first, via the scripting interface.
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("DeleteToolBar");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);

        // We need to delete the main GUI element, along with it's children.
        this->GetGUI().DeleteElement(this->GUI.Main);
    }



    void SceneEditor::ResetCamera()
    {
        this->camera.SetPosition(0.0f, 0.0f, 10.0f);
        this->SetCameraRotation(0.0f, 0.0f);
    }

    void SceneEditor::SetCameraRotation(float xRotation, float yRotation)
    {
        this->cameraXRotation = xRotation;
        this->cameraYRotation = yRotation;
        this->ApplyCameraRotation();
    }


    void SceneEditor::EnablePhysicsSimulation()
    {
        if (!this->IsPhysicsSimulationEnabled())
        {
            this->selectedNodesBeforePhysicsSimulation = this->selectedNodes;
            this->physicsManager.EnableSimulation();
            this->physicsManager.ActivateAllRigidBodies();

            this->UpdatePhysicsButtonIcon();
        }
    }

    void SceneEditor::DisablePhysicsSimulation()
    {
        if (this->IsPhysicsSimulationEnabled())
        {
            this->physicsManager.DisableSimulation();


            // To restore, all we need to do is revert the staging area.
            this->scene.RevertStateStackStagingArea();

            // We want to revert the selections, too.
            this->DeselectAll();
            this->SelectSceneNodes(this->selectedNodesBeforePhysicsSimulation);

            this->UpdatePhysicsButtonIcon();
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
            this->viewport.CalculatePickingRay(clickPosX, clickPosY, rayStart, rayEnd);


            CollisionWorld::ClosestRayTestCallback rayTestCallback(rayStart, rayEnd);
            rayTestCallback.m_collisionFilterGroup = CollisionGroups::EditorSelectionRay;
            rayTestCallback.m_collisionFilterMask  = CollisionGroups::EditorGizmo;
            this->pickingWorld.RayTest(rayStart, rayEnd, rayTestCallback);
            if (rayTestCallback.hasHit())
            {
                auto metadata = static_cast<EditorMetadataComponent*>(rayTestCallback.m_collisionObject->getUserPointer());
                assert(metadata != nullptr);
                {
                    auto &selectedNode = metadata->GetNode();

                    // Arrows
                    if (&selectedNode == &this->transformGizmo.GetXArrowSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(1.0f, 0.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Translate;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetYArrowSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 1.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Translate;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetZArrowSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 0.0f, 1.0f);
                        this->gizmoDragMode    = GizmoDragMode_Translate;
                    }
                    // Circles
                    else if (&selectedNode == &this->transformGizmo.GetXCircleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(1.0f, 0.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Rotate;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetYCircleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 1.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Rotate;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetZCircleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 0.0f, 1.0f);
                        this->gizmoDragMode    = GizmoDragMode_Rotate;
                    }
                    // Scale handles.
                    else if (&selectedNode == &this->transformGizmo.GetXScaleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(1.0f, 0.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Scale;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetYScaleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 1.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Scale;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetZScaleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 0.0f, 1.0f);
                        this->gizmoDragMode    = GizmoDragMode_Scale;
                    }
                    else
                    {
                        return false;
                    }


                    // The mouse dragging has a different level of influence depending on the direction of the axis. We need to calculate that now. We project two points - the
                    // center of the gizmo and the end point of the selected axis. From that we can get a normalised direction vector and use that as the influence.
                    glm::vec3 gizmoWorldPos        = this->transformGizmo.GetPosition();
                    glm::vec3 gizmoCenterWindowPos = this->viewport.Project(gizmoWorldPos);
                    glm::vec3 axisTipWindowPos     = this->viewport.Project(gizmoWorldPos + (selectedNode.GetWorldForwardVector() * selectedNode.GetWorldScale()));

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


                    this->transformGizmo.ChangeAxisColour(selectedNode, 1.0f, 1.0f, 1.0f);
                }

                return true;
            }
            else
            {
                this->gizmoDragMode = GizmoDragMode_None;
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
            this->viewport.CalculatePickingRay(clickPosX, clickPosY, rayStart, rayEnd);


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
                            this->DeselectSceneNode(selectedNode);
                        }
                        else
                        {
                            this->SelectSceneNode(selectedNode);
                        }
                    }
                    else
                    {
                        // If the node is already the selected one, we don't do anything.
                        if (!(this->selectedNodes.count == 1 && this->selectedNodes[0] == selectedNode.GetID()))
                        {
                            this->DeselectAll();
                            this->SelectSceneNode(selectedNode);
                        }
                    }
                }
            }
            else
            {
                this->DeselectAll();
            }
        }
    }

    void SceneEditor::DeselectAll(bool dontPostBackNotification)
    {
        while (this->selectedNodes.count > 0)
        {
            auto node = this->GetSceneNodeByID(this->selectedNodes[0]);
            assert(node != nullptr);
            {
                this->DeselectSceneNode(*node, false, true);
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
        if (!dontPostBackNotification)
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

    void SceneEditor::SelectSceneNode(SceneNode &node, bool force, bool dontPostBackNotification)
    {
        if (force || !this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Select();

                if (!this->selectedNodes.Exists(node.GetID()))
                {
                    this->selectedNodes.PushBack(node.GetID());
                }

                // The scripting environment needs to be aware of this change.
                if (!dontPostBackNotification)
                {
                    this->PostOnSelectionChangedEventToScript();
                }


                // With a change in selection, we will need to update the position of the gizmos.
                this->ShowTransformGizmo();
            }
        }
    }

    void SceneEditor::SelectSceneNodes(const GTCore::Vector<uint64_t> &selectedNodeIDs)
    {
        for (size_t i = 0; i < selectedNodeIDs.count; ++i)
        {
            auto node = this->GetSceneNodeByID(selectedNodeIDs[i]);
            assert(node != nullptr);
            {
                this->SelectSceneNode(*node, false, true);
            }
        }

        this->PostOnSelectionChangedEventToScript();
    }

    void SceneEditor::DeselectSceneNode(SceneNode &node, bool force, bool dontPostBackNotification)
    {
        if (force || this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Deselect();


                this->selectedNodes.RemoveFirstOccuranceOf(node.GetID());

                // The scripting environment needs to be aware of this change.
                if (!dontPostBackNotification)
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
                    this->RepositionGizmo();
                }
            }
        }
    }

    void SceneEditor::DeselectSceneNodeAndChildren(SceneNode &node, bool dontPostBackNotification)
    {
        this->DeselectSceneNode(node, false, false);

        for (auto childNode = node.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
        {
            this->DeselectSceneNodeAndChildren(*childNode, false);
        }


        if (!dontPostBackNotification)
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
            this->DeselectAll(true);



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
        this->DeselectAll(true);        // <-- True means to block notifications to the scripting environment.

        auto &newNode = this->CopySceneNodeAndChildren(sceneNodeToDuplicate, sceneNodeToDuplicate.GetParent());
        this->SelectSceneNode(newNode, true);

        return newNode;
    }


    void SceneEditor::Undo()
    {
        // Don't bother doing anything if we're already at the start of the current branch.
        if (this->scene.GetStateStackCurrentFrameIndex() > 0)
        {
            // If the physics simulation is running, it needs to be stopped first.
            this->DisablePhysicsSimulation();


            // We deselect everything because we're going to be reselecting the appropriate nodes after the state change.
            this->DeselectAll();

            this->scene.SeekStateStack(-1);
            this->MarkAsModified();

            // All nodes need to be reselected.
            this->ReselectSceneNodes();
        }
    }

    void SceneEditor::Redo()
    {
        if (this->scene.GetStateStackCurrentFrameIndex() < this->scene.GetStateStackMaxFrameIndex())
        {
            // If the physics simulation is running, it needs to be stopped first.
            this->DisablePhysicsSimulation();


            // We deselect everything because we're going to be reselecting the appropriate nodes after the state change.
            this->DeselectAll();

            this->scene.SeekStateStack(+1);
            this->MarkAsModified();

            // All nodes need to be reselected.
            this->ReselectSceneNodes();
        }
    }

    void SceneEditor::CommitStateStackFrame()
    {
        // We will never do this if a simulation is running.
        if (!this->IsPhysicsSimulationEnabled())
        {
            this->scene.CommitStateStackFrame();
            this->MarkAsModified();
        }
    }

    SceneNode* SceneEditor::InstantiatePrefab(const char* relativePath)
    {
        auto prefab = SceneNodeClassLibrary::Acquire(relativePath);
        if (prefab != nullptr)
        {
            auto rootSceneNode = this->scene.CreateNewSceneNode(*prefab);
            if (rootSceneNode != nullptr)
            {
                // We need to recursively map every scene node to the prefab.
                size_t rootSceneNodeIndex = 0;
                this->MapSceneNodeToPrefab(*rootSceneNode, *prefab, rootSceneNodeIndex);




                // We now need to map our scene node to the component. This is recursive.
                //uint64_t prefabSceneNodeID = prefab->GetRootID();
                //this->MapSceneNodeToPrefab(*rootSceneNode, relativePath, prefabSceneNodeID);

                // We also want to recursively deselect the scene nodes. We don't post notifications to the editor about this. The reason we do this is
                // because the metadata component may have left it marked as selected, which we don't want.
                this->DeselectSceneNodeAndChildren(*rootSceneNode, true);


                // We don't need the prefab anymore, so we can unacquire.
                SceneNodeClassLibrary::Unacquire(prefab);


                return rootSceneNode;
            }
        }

        return nullptr;
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
    }

    void SceneEditor::SwitchGizmoToGlobalSpace()
    {
        this->gizmoTransformSpace = GizmoTransformSpace_Global;
        this->UpdateGizmo();
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


    ///////////////////////////////////////////////////
    // Scene Events
    //
    // These method should only be called for the active scene.

    void SceneEditor::OnObjectAdded(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            // We need a metadata component if we don't already have one.
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata == nullptr)
            {
                metadata = node.AddComponent<EditorMetadataComponent>();
                this->OnSceneNodeComponentChanged(node, *metadata);
            }


            // Picking volume must be set here.
            if (metadata->UseModelForPickingShape() && node.HasComponent<ModelComponent>())
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
                        pickingCollisionObject.getCollisionShape()->setLocalScaling(ToBulletVector3(node.GetWorldScale()));
                    }
                }


                if (node.IsVisible())
                {
                    if (metadata->GetPickingCollisionShape() != nullptr)
                    {
                        btTransform transform;
                        node.GetWorldTransform(transform);
                        pickingCollisionObject.setWorldTransform(transform);

                        this->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                    }


                    // If we have a sprite, we'll want to add it's picking object to the picking world.
                    if (metadata->IsUsingSprite() && metadata->GetSpritePickingCollisionObject() != nullptr)
                    {
                        this->pickingWorld.AddCollisionObject(*metadata->GetSpritePickingCollisionObject(), metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                    }
                }


                // We need to let the editor know about this. It will need to do things like add it to the hierarchy explorer.
                this->PostOnSceneNodeAddedToScript(node);


                // Select the scene node if it's marked as such.
                if (metadata->IsSelected())
                {
                    this->SelectSceneNode(node, true, true);      // <-- 'true' means to force the selection so that the scripting environment is aware of it.
                }
            }
        }
    }

    void SceneEditor::OnObjectRemoved(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            auto metadata = node.GetComponent<EditorMetadataComponent>();
            assert(metadata != nullptr);
            {
                // Collision shapes need to be removed.
                this->pickingWorld.RemoveCollisionObject(metadata->GetPickingCollisionObject());

                if (metadata->IsUsingSprite() && metadata->GetSpritePickingCollisionObject() != nullptr)
                {
                    this->pickingWorld.RemoveCollisionObject(*metadata->GetSpritePickingCollisionObject());
                }


                // We need to make sure scene nodes are deseleted when they are removed from the scene.
                this->DeselectSceneNode(node);


                // We need to let the editor know about this. It will need to do things like remove it from the hierarchy explorer.
                this->PostOnSceneNodeRemovedToScript(node);
            }
        }
    }

    void SceneEditor::OnSceneNodeNameChanged(SceneNode &node)
    {
        // We need to let the scripting environment know about this change.
        this->PostOnSceneNodeNameChangedToScript(node);
    }

    void SceneEditor::OnSceneNodeParentChanged(SceneNode &node, SceneNode* previousParent)
    {
        this->PostOnSceneNodeParentChangedToScript(node, previousParent);
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
                this->RepositionGizmo();
            }


            // If the node that was transformed is the main camera we'll need to scale the gizmos so that they look a constant size.
            if (&node == &this->camera || metadata->IsSelected())
            {
                this->RescaleGizmo();
            }

            if (this->selectedNodes.count == 1 && node.GetID() == this->selectedNodes[0])
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
        }
    }

    void SceneEditor::OnSceneNodeComponentAdded(SceneNode &node, Component &component)
    {
        (void)node;
        (void)component;
    }

    void SceneEditor::OnSceneNodeComponentRemoved(SceneNode &node, Component &component)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            if (GTCore::Strings::Equal(component.GetName(), ModelComponent::Name))
            {
                if (metadata->UseModelForPickingShape())
                {
                    metadata->ClearPickingCollisionShape();
                }
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



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void SceneEditor::Show()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("ShowToolBar");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);


        this->GUI.Main->Show();
    }

    void SceneEditor::Hide()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("HideToolBar");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);


        this->GUI.Main->Hide();
    }

    bool SceneEditor::Save()
    {
        FILE* file = GTCore::IO::Open(this->GetAbsolutePath(), GTCore::IO::OpenMode::Write);
        if (file != nullptr)
        {
            // The physics simulation must be disable first!
            this->DisablePhysicsSimulation();


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

                float mouseOffsetX;
                float mouseOffsetY;
                game.GetSmoothedMouseOffset(mouseOffsetX, mouseOffsetY);

                if (this->IsDraggingGizmo())
                {
                    // TODO: If the parent of a node being processed here is selected, we don't actually want to do anything with it, unless it is ignoring
                    //       the parents transformation.

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


                        glm::vec3 startPosition = this->GetSelectionCenterPoint();
                        float     startAngle    = glm::length(glm::eulerAngles(this->GetGizmoRotation()) * dragAxis);



                        bool wasSnapping = this->isSnapping;
                        this->isSnapping = this->GetOwnerEditor().GetGame().IsKeyDown(GTCore::Keys::Ctrl);

                        if (!wasSnapping && this->isSnapping)
                        {
                            // If we get here, we've just started snapping.
                            this->snapTranslation = startPosition;
                            this->snapAngle       = startAngle;
                        }


                        // We need to drag the selected objects.
                        if (this->gizmoDragMode == GizmoDragMode_Translate)
                        {
                            glm::vec3 translation = dragAxis * (dragDirection * dragDistance * moveSpeed);

                            // What we do is simulate us moving the gizmo and then move the selected nodes by the offset.
                            glm::vec3 endPosition = startPosition;

                            if (this->gizmoTransformSpace == GizmoTransformSpace_Global || this->selectedNodes.count > 1)
                            {
                                endPosition           = startPosition         + translation;
                                this->snapTranslation = this->snapTranslation + translation;
                            }
                            else
                            {
                                endPosition           = startPosition         + (this->GetGizmoRotation() * translation);
                                this->snapTranslation = this->snapTranslation + (this->GetGizmoRotation() * translation);
                            }

                            // If we're snapping, we need to offset the endPosition in such a way that it causes it to snap to the "grid".
                            if (this->isSnapping)
                            {
                                glm::vec3 gridPosition = this->translateSnapSize * glm::floor(this->snapTranslation / glm::vec3(this->translateSnapSize));

                                // At this point we have an end position without snapping, and that same position by snapped to the grid ('gridPosition'). We need
                                // to get the difference, multiply it by the drag axis, and then add that different to the endPosition.
                                endPosition += (gridPosition - endPosition) * dragAxis;
                            }

                            // We we need to grab the difference between the start and end positions and add to the positions of the selected nodes.
                            translation = endPosition - startPosition;



                            for (size_t i = 0; i < this->selectedNodes.count; ++i)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                assert(node != nullptr);
                                {
                                    // We change the world position here. If the node has a parent who is also selected, and position inheritance is enabled, we ignore it.
                                    if (!this->IsAncestorSelected(*node) || !node->IsPositionInheritanceEnabled())
                                    {
                                        node->SetWorldPosition(node->GetWorldPosition() + translation);
                                    }
                                }
                            }
                        }
                        else if (this->gizmoDragMode == GizmoDragMode_Rotate)
                        {
                            float dragAngle = dragDirection * dragDistance * rotateSpeed;

                            // If we have multiple selections, we only ever do a world rotation. Otherwise, we'll do a local rotation.
                            if (this->selectedNodes.count == 1)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[0]);
                                assert(node != nullptr);
                                {
                                    if (this->gizmoTransformSpace == GizmoTransformSpace_Global)
                                    {
                                        node->RotateAroundWorldAxis(dragAngle, dragAxis);
                                    }
                                    else
                                    {
                                        node->Rotate(dragAngle, dragAxis);
                                    }
                                }
                            }
                            else
                            {
                                glm::vec3 pivot = this->GetSelectionCenterPoint();

                                for (size_t i = 0; i < this->selectedNodes.count; ++i)
                                {
                                    auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                    assert(node != nullptr);
                                    {
                                        if (!this->IsAncestorSelected(*node) || !node->IsOrientationInheritanceEnabled())
                                        {
                                            node->RotateAtPivotAroundWorldAxis(dragAngle, dragAxis, pivot);
                                        }
                                    }
                                }
                            }
                        }
                        else if (this->gizmoDragMode == GizmoDragMode_Scale)
                        {
                            // Scaling is always done in local space. Global space scaling is sheering, which will never be supported.

                            glm::vec3 dragAxis      = this->gizmoDragAxis;
                            float     dragDistance  = glm::length(glm::vec2(mouseOffsetX, -mouseOffsetY));
                            float     dragDirection = glm::dot(glm::normalize(glm::vec2(mouseOffsetX, -mouseOffsetY)), this->gizmoDragFactor);
                            float     dragSpeed     = 0.05f;

                            glm::vec3 scaleOffset = dragAxis * (dragDirection * dragDistance * dragSpeed);

                            for (size_t i = 0; i < this->selectedNodes.count; ++i)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                assert(node != nullptr);
                                {
                                    if (!this->IsAncestorSelected(*node) || !node->IsScaleInheritanceEnabled())
                                    {
                                        glm::vec3 newScale = node->GetWorldScale() + scaleOffset;

                                        // Negative scaling not yet supported.
                                        newScale = glm::max(newScale, glm::vec3(0.0f, 0.0f, 0.0f));

                                        node->SetWorldScale(newScale);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
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

                    if (this->transformGizmo.GetSceneNode().IsVisible())
                    {
                        this->transformGizmo.SetRotation(this->GetGizmoRotation(), this->camera);
                    }
                }
            }


            this->scene.Update(deltaTimeInSeconds);
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

            if (this->transformedObjectWithGizmo)
            {
                this->CommitStateStackFrame();
                this->transformedObjectWithGizmo = false;
            }
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
                            if (model->GetDefinition().absolutePath == item.info.absolutePath || (model->GetDefinition().absolutePath + ".gtmodel") == item.info.absolutePath)
                            {
                                modelComponent->OnChanged();

                                // If this model is the only one selected, we will also update the panel. To do this, we'll just let the editor
                                // think that the selection has changed.
                                if (this->GetSelectedSceneNodeCount() == 1 && this->selectedNodes[0] == sceneNode->GetID())
                                {
                                    this->PostOnSelectionChangedEventToScript();
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (GTEngine::IO::IsSupportedPrefabExtension(item.relativePath.c_str()))
        {
            this->UpdateAllSceneNodesLinkedToPrefab(item.relativePath.c_str());
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
            this->camera.AddComponent<GTEngine::CameraComponent>();
            this->camera.AddComponent<GTEngine::AmbientLightComponent>()->SetColour(0.0f, 0.0f, 0.0f);
        
            auto metadata = this->camera.AddComponent<GTEngine::EditorMetadataComponent>();
            {
                metadata->IsSystemNode(true);
            }
        
            this->camera.DisableSerialization();
            this->camera.DisableStateStackStaging();


            this->viewport.SetCameraNode(this->camera);
            this->scene.AddViewport(this->viewport);
            this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);

            this->scene.AddSceneNode(this->camera);

            this->transformGizmo.Initialise();
            this->transformGizmo.Hide();
            this->scene.AddSceneNode(this->transformGizmo.GetSceneNode());
        }
    }

    void SceneEditor::ApplyCameraRotation()
    {
        this->camera.SetOrientation(glm::quat());
        this->camera.RotateY(this->cameraYRotation);
        this->camera.RotateX(this->cameraXRotation);
    }

    void SceneEditor::SerializeScene(GTCore::Serializer &serializer, bool serializeMetadata) const
    {
        this->scene.Serialize(serializer);

        // We now want to save our own chunk. This will contain metadata such as the camera position and whatnot.
        if (serializeMetadata)
        {
            GTCore::BasicSerializer metadataSerializer;

            this->camera.Serialize(metadataSerializer);
            metadataSerializer.Write(this->cameraXRotation);
            metadataSerializer.Write(this->cameraYRotation);

            // We're going to serialize the state stack, too.
            this->scene.SerializeStateStack(metadataSerializer);


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
        this->isDeserializing = true;
        {
            this->transformGizmo.Hide();


            // With pre-deserialization done, we can now do a full deserialization of the scene.
            this->scene.Deserialize(deserializer);

            // We now want to load the metadata chunk. We'll peek at the next chunk and see if that's it. We should probably do an iteration type
            // system later on.
            Serialization::ChunkHeader header;
            deserializer.Peek(&header, sizeof(Serialization::ChunkHeader));

            if (header.id == Serialization::ChunkID_Scene_EditorMetadata)
            {
                // Since we only peeked at the header, we'll need to now seek past it.
                deserializer.Seek(sizeof(Serialization::ChunkHeader));

                // The camera node needs to be deserialized.
                this->camera.Deserialize(deserializer);
                deserializer.Read(this->cameraXRotation);
                deserializer.Read(this->cameraYRotation);

                this->camera.DisableSerialization();
                this->camera.DisableStateStackStaging();


                // We need to peek at the next bytes. If it's a state stack, we need to deserialize it.
                if (deserializer.Peek(&header, sizeof(header)) == sizeof(header) && header.id == Serialization::ChunkID_SceneStateStack)
                {
                    this->scene.DeserializeStateStack(deserializer);
                }
                else
                {
                    // We need an initial commit.
                    this->scene.CommitStateStackFrame();
                }
            }
            else
            {
                // We need an initial commit.
                this->scene.CommitStateStackFrame();
            }


            // The deserializer will clear the scene, so we'll need to re-add the camera and gizmo.
            this->scene.AddSceneNode(this->camera);
            this->scene.AddSceneNode(this->transformGizmo.GetSceneNode());

            this->UpdateGizmo();



            // We need to make sure all scene nodes are brought up-to-date with their prefabs.
            this->UpdateAllSceneNodesLinkedToPrefabs();
        }
        this->isDeserializing = false;
        this->scene.EnableStateStackStaging();


        // We'll let the editor do it's thing with selections.
        this->PostOnSelectionChangedEventToScript();
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


    void SceneEditor::ShowTransformGizmo()
    {
        this->transformGizmo.Show();
        this->UpdateGizmo();
    }

    void SceneEditor::HideGizmo()
    {
        this->transformGizmo.Hide();
    }

    void SceneEditor::RepositionGizmo()
    {
        this->transformGizmo.SetPosition(this->GetSelectionCenterPoint());
        this->transformGizmo.SetRotation(this->GetGizmoRotation(), this->camera);
    }

    void SceneEditor::RescaleGizmo()
    {
        // We're going to determine the new scale of the gizmos by using a project/unproject system. We first project the actual position of the gizmo
        // into window coordinates. We then add a value to the y result that will represent the size of the object on the screen. Then, we unproject
        // that position back into world space. The length between the gizmo position and unprojected position will be the new scale.
        glm::vec3 gizmoPosition = this->GetSelectionCenterPoint();

        glm::vec3 windowPos = this->viewport.Project(gizmoPosition);
        windowPos.y += 64.0f;

        glm::vec3 gizmoScale(glm::distance(this->viewport.Unproject(windowPos), gizmoPosition));
        this->transformGizmo.SetScale(gizmoScale);
    }

    void SceneEditor::ShowGizmoHandles()
    {
        switch (this->gizmoTransformMode)
        {
        case GizmoTransformMode_Translate:
            {
                this->transformGizmo.GetXArrowSceneNode().Show();
                this->transformGizmo.GetYArrowSceneNode().Show();
                this->transformGizmo.GetZArrowSceneNode().Show();

                this->transformGizmo.GetCameraFacingCircleSceneNode().Hide();
                this->transformGizmo.GetXCircleSceneNode().Hide();
                this->transformGizmo.GetYCircleSceneNode().Hide();
                this->transformGizmo.GetZCircleSceneNode().Hide();

                this->transformGizmo.GetXScaleSceneNode().Hide();
                this->transformGizmo.GetYScaleSceneNode().Hide();
                this->transformGizmo.GetZScaleSceneNode().Hide();


                break;
            }

        case GizmoTransformMode_Rotate:
            {
                this->transformGizmo.GetXArrowSceneNode().Hide();
                this->transformGizmo.GetYArrowSceneNode().Hide();
                this->transformGizmo.GetZArrowSceneNode().Hide();

                this->transformGizmo.GetCameraFacingCircleSceneNode().Show();
                this->transformGizmo.GetXCircleSceneNode().Show();
                this->transformGizmo.GetYCircleSceneNode().Show();
                this->transformGizmo.GetZCircleSceneNode().Show();

                this->transformGizmo.GetXScaleSceneNode().Hide();
                this->transformGizmo.GetYScaleSceneNode().Hide();
                this->transformGizmo.GetZScaleSceneNode().Hide();

                break;
            }

        case GizmoTransformMode_Scale:
            {
                this->transformGizmo.GetXArrowSceneNode().Hide();
                this->transformGizmo.GetYArrowSceneNode().Hide();
                this->transformGizmo.GetZArrowSceneNode().Hide();

                this->transformGizmo.GetCameraFacingCircleSceneNode().Hide();
                this->transformGizmo.GetXCircleSceneNode().Hide();
                this->transformGizmo.GetYCircleSceneNode().Hide();
                this->transformGizmo.GetZCircleSceneNode().Hide();

                this->transformGizmo.GetXScaleSceneNode().Show();
                this->transformGizmo.GetYScaleSceneNode().Show();
                this->transformGizmo.GetZScaleSceneNode().Show();

                break;
            }

        default:
            {
                break;
            }
        }
    }

    void SceneEditor::UpdateGizmo()
    {
        this->RepositionGizmo();
        this->RescaleGizmo();
        this->ShowGizmoHandles();
    }

    bool SceneEditor::IsDraggingGizmo() const
    {
        return this->gizmoDragMode != GizmoDragMode_None;
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
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            if (!metadata->IsSystemNode())
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
        }
    }

    void SceneEditor::PostOnSceneNodeRemovedToScript(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            if (!metadata->IsSystemNode())
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
        }
    }

    void SceneEditor::PostOnSceneNodeNameChangedToScript(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            if (!metadata->IsSystemNode())
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
        }
    }

    void SceneEditor::PostOnSceneNodeParentChangedToScript(SceneNode &node, SceneNode* previousParent)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            if (!metadata->IsSystemNode())
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
        }
    }


    void SceneEditor::ReselectSceneNodes()
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
                    this->DeselectSceneNode(*sceneNode);
                }
            }
        }


        // Reselect.
        for (size_t i = 0; i < nodesForReselection.count; ++i)
        {
            auto sceneNode = nodesForReselection[i];
            assert(sceneNode != nullptr);
            {
                this->SelectSceneNode(*sceneNode, true);
            }
        }
    }


    SceneNode & SceneEditor::CopySceneNodeAndChildren(SceneNode &nodeToCopy, SceneNode* parentNode)
    {
        GTCore::BasicSerializer serializer;
        nodeToCopy.Serialize(serializer);


        GTCore::BasicDeserializer deserializer(serializer.GetBuffer(), serializer.GetBufferSizeInBytes());
        auto newNode = this->scene.CreateNewSceneNode(deserializer, true);      // <-- 'true' means to generate a new ID if a node of the same ID already exists (spoiler: it does already exist).



        // We now want to link the new node to the parent, if we have one.
        if (parentNode != nullptr)
        {
            parentNode->AttachChild(*newNode);     // <-- This does an implicit Scene::AddSceneNode().
        }

        // Node needs to be selected, but we don't want to notify the editor yet (we'll do it in one go at a higher level).
        this->SelectSceneNode(*newNode, false, true);



        // And now we need to make a copy of the children. The parent will be the new node.
        for (auto childNode = nodeToCopy.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
        {
            this->CopySceneNodeAndChildren(*childNode, newNode);
        }


        return *newNode;
    }


    void SceneEditor::UpdatePhysicsButtonIcon()
    {
        if (this->GUI.Main != nullptr)
        {
            auto &script = this->GetScript();

            script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("UpdatePhysicsButtonIcon");
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


    void SceneEditor::MapSceneNodeToPrefab(SceneNode &sceneNode, SceneNodeClass &prefab, size_t &prefabSceneNodeIndex)
    {
        auto metadata = sceneNode.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            metadata->SetPrefabRelativePath(prefab.GetRelativePath());
            metadata->SetPrefabID(prefab.GetSerializers().buffer[prefabSceneNodeIndex]->key);

            // Now we just iterate over and do the children, making sure we increment the scene node index.
            for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
            {
                this->MapSceneNodeToPrefab(*childNode, prefab, ++prefabSceneNodeIndex);
            }
        }
    }

    void SceneEditor::CreateMissingChildPrefabSceneNodes(SceneNodeClass &prefab, const SceneNode &sceneNode, uint64_t sceneNodePrefabID, GTCore::Vector<SceneNode*> &newSceneNodes)
    {
        GTCore::Vector<uint64_t> childIDs;
        prefab.GetChildIDs(sceneNodePrefabID, childIDs);

        // Now we need to loop over each child and check if it's in childIDs. If it is, we ignore it. If it's not, we create it.
        for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
        {
            auto metadata = childNode->GetComponent<EditorMetadataComponent>();
            assert(metadata != nullptr);
            {
                if (metadata->IsLinkedToPrefab() && childIDs.Exists(metadata->GetPrefabID()))
                {
                    childIDs.RemoveFirstOccuranceOf(metadata->GetPrefabID());
                }
            }
        }


        GTCore::Vector<SceneNode*> newChildNodes;

        // At this point, childIDs contains only the IDs that are not present. We need to create new scene nodes for these ones.
        for (size_t i = 0; i < childIDs.count; ++i)
        {
            auto serializer = prefab.GetSerializerByID(childIDs[i]);
            assert(serializer != nullptr);
            {
                auto newSceneNode = new SceneNode;
                
                GTCore::BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());
                newSceneNode->Deserialize(deserializer);

                // Important that we set the ID to 0 here.
                newSceneNode->SetID(0);


                // Something else we want to do here is make sure it's not marked as selected in the editor metadata component.
                auto metadata = newSceneNode->GetComponent<EditorMetadataComponent>();
                if (metadata == nullptr)
                {
                    metadata = newSceneNode->AddComponent<EditorMetadataComponent>();
                }
                else
                {
                    metadata->Deselect();
                }


                // We need to link the new node to the prefab.
                metadata->LinkToPrefab(prefab.GetRelativePath(), childIDs[i]);


                newChildNodes.PushBack(newSceneNode);
            }
        }



        // We want to call this recursively on all children, including the new ones.
        for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
        {
            auto metadata = childNode->GetComponent<EditorMetadataComponent>();
            assert(metadata != nullptr);
            {
                this->CreateMissingChildPrefabSceneNodes(prefab, *childNode, metadata->GetPrefabID(), newSceneNodes);
            }
        }

        for (size_t i = 0; i < newChildNodes.count; ++i)
        {
            auto childNode = newChildNodes[i];
            assert(childNode != nullptr);
            {
                newSceneNodes.PushBack(childNode);

                auto metadata = childNode->GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    this->CreateMissingChildPrefabSceneNodes(prefab, *childNode, metadata->GetPrefabID(), newSceneNodes);
                }
            }
        }
    }

    bool SceneEditor::IsDescendantLinkedToDifferentPrefab(SceneNode &sceneNode) const
    {
        auto metadata = sceneNode.GetComponent<EditorMetadataComponent>();
        assert(metadata != nullptr);
        {
            for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
            {
                auto childMetadata = childNode->GetComponent<EditorMetadataComponent>();
                assert(childMetadata != nullptr);
                {
                    if (!GTCore::Strings::Equal<false>(metadata->GetPrefabRelativePath(), childMetadata->GetPrefabRelativePath()))
                    {
                        return true;
                    }
                    else
                    {
                        if (this->IsDescendantLinkedToDifferentPrefab(*childNode))
                        {
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    void SceneEditor::UpdateAllSceneNodesLinkedToPrefab(const char* prefabRelativePath)
    {
        // It's a prefab. We need to update any scene node that uses this prefab.
        auto prefab = SceneNodeClassLibrary::Acquire(prefabRelativePath);
        if (prefab != nullptr)
        {
            // The new scene nodes to be added after the main iteration. The key is the node in question and the value is the parent.
            GTCore::Map<SceneNode*, SceneNode*> newSceneNodes;
                
            // The scene nodes to remove after the main iteration.
            GTCore::Vector<SceneNode*> sceneNodesToRemove;


            size_t sceneNodeCount = this->scene.GetSceneNodeCount();

            for (size_t i = 0; i < sceneNodeCount; ++i)
            {
                auto sceneNode = this->scene.GetSceneNodeByIndex(i);
                assert(sceneNode != nullptr);
                {
                    auto metadata = sceneNode->GetComponent<EditorMetadataComponent>();
                    assert(metadata != nullptr);
                    {
                        if (metadata->IsLinkedToPrefab() && GTCore::Strings::Equal<false>(prefabRelativePath, metadata->GetPrefabRelativePath()))
                        {
                            uint64_t prefabID = metadata->GetPrefabID();

                            auto serializer = prefab->GetSerializerByID(prefabID);
                            if (serializer != nullptr)
                            {
                                bool wasSelected = this->IsSceneNodeSelected(*sceneNode);
                                this->DeselectSceneNode(*sceneNode, true, true);

                                // It's the same node, but it needs to be updated. We maintain the transformation. Not quite sure yet how I want to allow
                                // other attributes to be maintained.
                                glm::vec3 worldPosition    = sceneNode->GetWorldPosition();
                                glm::quat worldOrientation = sceneNode->GetWorldOrientation();
                                glm::vec3 worldScale       = sceneNode->GetWorldScale();

                                GTCore::BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());
                                sceneNode->Deserialize(deserializer, SceneNode::NoID);      // <-- Super important! SceneNode::NoID will cause the deserializer to keep the original ID.

                                sceneNode->SetWorldPosition(worldPosition);
                                sceneNode->SetWorldOrientation(worldOrientation);
                                sceneNode->SetWorldScale(worldScale);

                                // Deserializing will break the link to the prefab, so we'll want to re-link.
                                metadata->LinkToPrefab(prefab->GetRelativePath(), prefabID);
                                metadata->Deselect();

                                // We will want to reselect if we were previously selected.
                                if (wasSelected)
                                {
                                    this->SelectSceneNode(*sceneNode, true, true);
                                }


                                // We now need to create the missing child scene nodes. This needs to be done recursively, of course. This doesn't add them to the scene straight away
                                // because we don't want to break the current iteration.
                                GTCore::Vector<SceneNode*> newChildNodes;
                                this->CreateMissingChildPrefabSceneNodes(*prefab, *sceneNode, metadata->GetPrefabID(), newChildNodes);

                                for (size_t iChildNode = 0; iChildNode < newChildNodes.count; ++iChildNode)
                                {
                                    newSceneNodes.Add(newChildNodes[iChildNode], sceneNode);
                                }
                            }
                            else
                            {
                                // The scene node is no longer referenced in the prefab. If the node has children that are not part of the prefab, we just
                                // want to unlink it from the prefab. Otherwise, we want to delete it.
                                if (this->IsDescendantLinkedToDifferentPrefab(*sceneNode))
                                {
                                    metadata->UnlinkFromPrefab();
                                }
                                else
                                {
                                    sceneNodesToRemove.PushBack(sceneNode);
                                }
                            }
                        }
                    }
                }
            }



            // Now we can delete the old nodes and add the new ones. We'll do the delete's first.
            for (size_t i = 0; i < sceneNodesToRemove.count; ++i)
            {
                this->scene.RemoveSceneNode(*sceneNodesToRemove[i]);
            }

            // Now we need to go ahead and link the scene nodes together. Just linking will cause the child to be added to the scene like normal.
            for (size_t i = 0; i < newSceneNodes.count; ++i)
            {
                auto childNode  = newSceneNodes.buffer[i]->key;
                auto parentNode = newSceneNodes.buffer[i]->value;

                assert(childNode  != nullptr);
                assert(parentNode != nullptr);
                {
                    parentNode->AttachChild(*childNode);
                }
            }
        }
    }

    void SceneEditor::UpdateAllSceneNodesLinkedToPrefabs()
    {
        // TODO: Optimize this. This is badly inefficient because it does one iteration of every scene node for every prefab, and another when first
        //       retrieving the list of referenced prefabs.

        // We'll first grab every referenced prefab.
        GTCore::Vector<GTCore::String> prefabRelativePaths;
        this->GetAllReferencedPrefabs(prefabRelativePaths);

        // Now we want to iterate over each prefab and update all scene nodes that reference it.
        for (size_t i = 0; i < prefabRelativePaths.count; ++i)
        {
            this->UpdateAllSceneNodesLinkedToPrefab(prefabRelativePaths[i].c_str());
        }
    }


    void SceneEditor::GetAllReferencedPrefabs(GTCore::Vector<GTCore::String> &prefabRelativePaths)
    {
        size_t sceneNodeCount = this->scene.GetSceneNodeCount();

        for (size_t i = 0; i < sceneNodeCount; ++i)
        {
            auto sceneNode = this->scene.GetSceneNodeByIndex(i);
            assert(sceneNode != nullptr);
            {
                auto metadata = sceneNode->GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    if (metadata->IsLinkedToPrefab() && !prefabRelativePaths.Exists(metadata->GetPrefabRelativePath()))
                    {
                        prefabRelativePaths.PushBack(metadata->GetPrefabRelativePath());
                    }
                }
            }
        }
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
