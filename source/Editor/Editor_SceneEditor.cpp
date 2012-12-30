
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/CollisionGroups.hpp>
#include <GTEngine/Logging.hpp>

#undef min
#undef max

namespace GTEngine
{
    Editor_SceneEditor::Editor_SceneEditor(Editor &editorIn)
        : editor(editorIn), currentState(nullptr), loadedStates()
    {
    }

    Editor_SceneEditor::~Editor_SceneEditor()
    {
        for (size_t i = 0; i < this->loadedStates.count; ++i)
        {
            delete this->loadedStates.buffer[i]->value;
        }
    }


    void Editor_SceneEditor::Startup()
    {
        this->InitialiseScripting();
    }


    bool Editor_SceneEditor::LoadScene(const char* absolutePath)
    {
        auto &gui    = this->editor.GetGame().GetGUI();
        auto &script = this->editor.GetGame().GetScript();

        auto iState = this->loadedStates.Find(absolutePath);
        if (iState != nullptr)
        {
            // We can return early in this case.
            if (this->currentState == iState->value)
            {
                return true;
            }

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
                        auto panelElement = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').Panel:GetID();", mainElement->id).c_str()));
                        if (panelElement != nullptr)
                        {
                            // Now we just need to create a new state object and make it the current one.
                            if (this->currentState != nullptr)
                            {
                                this->currentState->GUI.Main->Hide();
                            }

                            this->currentState = new State(*this);
                            this->currentState->GUI.Main     = mainElement;
                            this->currentState->GUI.Viewport = viewportElement;
                            this->currentState->GUI.Panel    = panelElement;
                            this->loadedStates.Add(absolutePath, this->currentState);

                            
                            viewportElement->AttachEventHandler(this->currentState->viewportEventHandler);
                            viewportElement->OnSize();

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
                return false;
            }
        }


        // At this point we will have a valid current state. We need to let the scripting environment know of a few things - mainly the new active scene.
        this->SetCurrentSceneInScript(&this->currentState->scene, this->currentState->GUI.Main->id);


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
                // We need to let the scripting environment know about a few things.
                this->SetCurrentSceneInScript(nullptr, nullptr);
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



    bool Editor_SceneEditor::TryGizmoMouseSelect()
    {
        if (this->currentState != nullptr && this->currentState->GUI.Main->IsVisible())
        {
            int clickPosX;
            int clickPosY;
            this->currentState->viewportEventHandler.GetMousePosition(clickPosX, clickPosY);

            glm::vec3 rayStart;
            glm::vec3 rayEnd;
            this->currentState->viewport.CalculatePickingRay(clickPosX, clickPosY, rayStart, rayEnd);


            CollisionWorld::ClosestRayTestCallback rayTestCallback(rayStart, rayEnd);
            rayTestCallback.m_collisionFilterGroup = CollisionGroups::EditorSelectionRay;
            rayTestCallback.m_collisionFilterMask  = CollisionGroups::EditorGizmo;
            this->currentState->pickingWorld.RayTest(rayStart, rayEnd, rayTestCallback);
            if (rayTestCallback.hasHit())
            {
                auto metadata = static_cast<EditorMetadataComponent*>(rayTestCallback.m_collisionObject->getUserPointer());
                assert(metadata != nullptr);
                {
                    auto &selectedNode = metadata->GetNode();

                    if (&selectedNode == &this->currentState->positionGizmo.GetXArrowSceneNode())
                    {
                        this->currentState->isDraggingGizmoX = true;
                        this->currentState->gizmoDragAxis    = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    else if (&selectedNode == &this->currentState->positionGizmo.GetYArrowSceneNode())
                    {
                        this->currentState->isDraggingGizmoY = true;
                        this->currentState->gizmoDragAxis    = glm::vec3(0.0f, 1.0f, 0.0f);
                    }
                    else if (&selectedNode == &this->currentState->positionGizmo.GetZArrowSceneNode())
                    {
                        this->currentState->isDraggingGizmoZ = true;
                        this->currentState->gizmoDragAxis    = glm::vec3(0.0f, 0.0f, 1.0f);
                    }
                    else
                    {
                        return false;
                    }


                    // The mouse dragging has a different level of influence depending on the direction of the axis. We need to calculate that now. We project two points - the
                    // center of the gizmo and the end point of the selected axis. From that we can get a normalised direction vector and use that as the influence.
                    glm::vec3 gizmoWorldPos        = this->currentState->positionGizmo.GetPosition();
                    glm::vec3 gizmoCenterWindowPos = this->currentState->viewport.Project(gizmoWorldPos);
                    glm::vec3 axisTipWindowPos     = this->currentState->viewport.Project(gizmoWorldPos + (selectedNode.GetForwardVector() * selectedNode.GetWorldScale()));

                    this->currentState->gizmoDragFactor = glm::vec2(axisTipWindowPos - gizmoCenterWindowPos);
                    if (glm::dot(this->currentState->gizmoDragFactor, this->currentState->gizmoDragFactor) > 0.0f)
                    {
                        this->currentState->gizmoDragFactor = glm::normalize(this->currentState->gizmoDragFactor);
                    }
                    else
                    {
                        this->currentState->gizmoDragFactor.x = 1.0f;
                        this->currentState->gizmoDragFactor.y = 0.0f;
                    }


                    this->currentState->positionGizmo.ChangeAxisColour(selectedNode, 1.0f, 1.0f, 1.0f);
                }

                return true;
            }
        }

        return false;
    }

    void Editor_SceneEditor::DoMouseSelection()
    {
        if (this->currentState != nullptr && this->currentState->GUI.Main->IsVisible())
        {
            // We want to do a few ray tests here. Some stuff will have priority over other stuff. For example, any gizmo will have a higher priority over everything else.

            int clickPosX;
            int clickPosY;
            this->currentState->viewportEventHandler.GetMousePosition(clickPosX, clickPosY);

            glm::vec3 rayStart;
            glm::vec3 rayEnd;
            this->currentState->viewport.CalculatePickingRay(clickPosX, clickPosY, rayStart, rayEnd);


            CollisionWorld::ClosestRayTestCallback rayTestCallback(rayStart, rayEnd);
            rayTestCallback.m_collisionFilterGroup = CollisionGroups::EditorSelectionRay;
            rayTestCallback.m_collisionFilterMask  = CollisionGroups::EditorSelectionVolume;
            this->currentState->pickingWorld.RayTest(rayStart, rayEnd, rayTestCallback);
            if (rayTestCallback.hasHit())
            {
                // The use data of the object will be a pointer to the EditorMetadataComponent object. From this, we can grab the actual scene node.
                auto metadata = static_cast<EditorMetadataComponent*>(rayTestCallback.m_collisionObject->getUserPointer());
                assert(metadata != nullptr);
                {
                    auto &selectedNode = metadata->GetNode();

                    // The way we do the selection depends on what we're doing. If shift is being held down, we don't want to deselect anything and instead just add
                    // or remove the node to the selection. If the selected node is already selected, it needs to be deselected. Otherwise it needs to be selected.
                    if (this->editor.GetGame().IsKeyDown(GTCore::Keys::Shift))
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
                        this->DeselectAll();
                        this->SelectSceneNode(selectedNode);
                    }
                }
            }
            else
            {
                this->DeselectAll();
            }
        }
    }

    void Editor_SceneEditor::DeselectAll()
    {
        if (this->currentState != nullptr)
        {
            while (this->currentState->selectedNodes.count > 0)
            {
                auto node = this->currentState->selectedNodes[0];
                assert(node != nullptr);

                this->DeselectSceneNode(*node);
            }


            // The scripting environment needs to know about this.
            this->GetScript().Execute("Editor.SceneEditor.MarkAllNodesAsDeselected();");
        }
    }


    bool Editor_SceneEditor::IsSceneNodeSelected(const SceneNode &node) const
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            return metadata->IsSelected();
        }

        return false;
    }

    void Editor_SceneEditor::SelectSceneNode(SceneNode &node)
    {
        auto state = node.GetDataPointer<State>(0);
        if (state != nullptr && !this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Select();

                assert(state->selectedNodes.Exists(&node) == false);
                {
                    state->selectedNodes.PushBack(&node);

                    // The scripting environment needs to be aware of this change.
                    auto &script = this->GetScript();

                    script.GetGlobal("Editor");
                    assert(script.IsTable(-1));
                    {
                        script.Push("SceneEditor");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("MarkNodeAsSelected");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.Push(&node);
                                script.Call(1, 0);
                            }
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);



                    // With a change in selection, we will need to update the position of the gizmos.
                    this->ShowPositionGizmo();
                }
            }
        }
    }

    void Editor_SceneEditor::DeselectSceneNode(SceneNode &node)
    {
        auto state = node.GetDataPointer<State>(0);
        if (state != nullptr && this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Deselect();

                assert(state->selectedNodes.Exists(&node) == true);
                {
                    state->selectedNodes.RemoveFirstOccuranceOf(&node);

                    // The scripting environment needs to be aware of this change.
                    auto &script = this->GetScript();

                    script.GetGlobal("Editor");
                    assert(script.IsTable(-1));
                    {
                        script.Push("SceneEditor");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("MarkNodeAsDeselected");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.Push(&node);
                                script.Call(1, 0);
                            }
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);


                    // With a change in selection, we will need to update the gizmos.
                    if (state->selectedNodes.count == 0)
                    {
                        this->HideGizmos();
                    }
                    else
                    {
                        this->RepositionGizmos();
                    }
                }
            }
        }
    }

    glm::vec3 Editor_SceneEditor::GetSelectionCenterPoint() const
    {
        if (this->currentState != nullptr && this->currentState->selectedNodes.count > 0)
        {
            glm::vec3 aabbMin( FLT_MAX,  FLT_MAX,  FLT_MAX);
            glm::vec3 aabbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);


            for (size_t i = 0; i < this->currentState->selectedNodes.count; ++i)
            {
                auto node = this->currentState->selectedNodes[i];
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



    ////////////////////////////////////////////////
    // Editting

    void Editor_SceneEditor::DeleteSelectedSceneNodes()
    {
        if (this->currentState != nullptr)
        {
            for (size_t i = 0; i < this->currentState->selectedNodes.count; ++i)
            {
                this->currentState->sceneNodesToDelete.RemoveFirstOccuranceOf(this->currentState->selectedNodes[i]);
                delete this->currentState->selectedNodes[i];
            }
            this->currentState->selectedNodes.Clear();
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

                    if (this->currentState->IsDraggingGizmo())
                    {
                        if (mouseOffsetX != 0.0f || mouseOffsetY != 0.0f)
                        {
                            // We need to drag the selected objects.
                            glm::vec3 dragAxis      = this->currentState->gizmoDragAxis;
                            float     dragDistance  = glm::length(glm::vec2(mouseOffsetX, -mouseOffsetY));
                            float     dragDirection = glm::dot(glm::normalize(glm::vec2(mouseOffsetX, -mouseOffsetY)), this->currentState->gizmoDragFactor);
                            float     dragSpeed     = 0.05f;

                            for (size_t i = 0; i < this->currentState->selectedNodes.count; ++i)
                            {
                                auto node = this->currentState->selectedNodes[i];
                                if (node != nullptr)
                                {
                                    node->SetWorldPosition(node->GetWorldPosition() + (dragAxis * (dragDirection * dragDistance * dragSpeed)));
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
                }

                this->currentState->scene.Update(deltaTimeInSeconds);
            }
        }
    }

    void Editor_SceneEditor::OnMouseButtonDown(GTCore::MouseButton button, int x, int y)
    {
        (void)button;
        (void)x;
        (void)y;
    }

    void Editor_SceneEditor::OnMouseButtonUp(GTCore::MouseButton button, int x, int y)
    {
        (void)x;
        (void)y;

        if (this->currentState != nullptr)
        {
            if (button == GTCore::MouseButton_Left)
            {
                this->currentState->isDraggingGizmoX = false;
                this->currentState->isDraggingGizmoY = false;
                this->currentState->isDraggingGizmoZ = false;

                this->currentState->positionGizmo.RestoreColours();
            }
        }
    }


    ///////////////////////////////////////////////////
    // Scene Events
    //
    // These method should only be called for the active scene.

    void Editor_SceneEditor::OnObjectAdded(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            // The default data pointer is going to be a pointer to the editor state that owns the scene node. If this has already been set, we leave it
            // alone. Otherwise, we set it to the current state.
            if (node.GetDataPointer<State>(0) == nullptr)
            {
                if (this->currentState != nullptr)
                {
                    node.SetDataPointer(0, this->currentState);
                }
                else
                {
                    GTEngine::Log("SceneEditor - Warning: Adding object to scene that is not the active scene. This is not stable.");
                }
            }


            
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata == nullptr)
            {
                metadata = node.AddComponent<EditorMetadataComponent>();

                // When a scene node is added without a metadata component (which is true if we've made it here), we know that it must be deleted when the
                // state is also deleted. We need to mark it as such.
                metadata->DeleteOnClose(true);

                auto state = node.GetDataPointer<State>(0);
                if (state != nullptr)
                {
                    state->sceneNodesToDelete.PushBack(&node);
                }
            }



            // We can cheat here and just act as if the object has been refreshed.
            this->OnObjectRefreshed(object);
        }
    }

    void Editor_SceneEditor::OnObjectRemoved(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);


            // We need to make sure scene nodes are deseleted when they are removed from the scene.
            this->DeselectSceneNode(node);

            // The data pointer at position 0 will be a pointer to the Editor_SceneEditor::State object that previously owned the scene node. This needs to be cleared.
            node.SetDataPointer(0, nullptr);
        }
    }

    void Editor_SceneEditor::OnObjectRefreshed(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                // If we have a model, we'll want to set the collision shape to that of the model.
                if (node.HasComponent<ModelComponent>())
                {
                    if (metadata->UseModelForPickingShape())
                    {
                        metadata->SetPickingCollisionShapeToModel();
                    }
                }
                

                // If got a light component attached, we will want to attach a sprite here.
                if (node.HasComponent<PointLightComponent>()       ||
                    node.HasComponent<SpotLightComponent>()        ||
                    node.HasComponent<DirectionalLightComponent>() ||
                    node.HasComponent<AmbientLightComponent>() &&
                    &node != &node.GetDataPointer<State>(0)->camera)
                {
                    metadata->SetModel("engine/models/default.dae");
                    metadata->GetModel()->meshes[0]->SetMaterial("engine/materials/simple-emissive.material");
                    metadata->GetModel()->meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 1.0f, 1.0f);
                }



                // We need to remove and re-add the collision shape since it might have changed.
                auto &pickingCollisionObject = metadata->GetPickingCollisionObject();
                
                auto world = pickingCollisionObject.GetWorld();
                if (world != nullptr)
                {
                    world->RemoveCollisionObject(pickingCollisionObject);
                }

                

                if (metadata->GetPickingCollisionShape() != nullptr)
                {
                    btTransform transform;
                    node.GetWorldTransform(transform);

                    pickingCollisionObject.setWorldTransform(transform);
                    pickingCollisionObject.getCollisionShape()->setLocalScaling(ToBulletVector3(node.GetWorldScale()));


                    // Here we need to find the state containing the collision world this object will be added to. If the current state is null, we check the
                    // data pointer at position 0 which, if set, will be a pointer to the State object the node belongs to.
                    auto state = node.GetDataPointer<State>(0);
                    if (state != nullptr)
                    {
                        state->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                    }
                    else
                    {
                        GTEngine::Log("Scene Editor - Warning: Attempting to modify an object that is not part of any loaded scene.");
                    }
                }
            }
        }
    }

    void Editor_SceneEditor::OnSceneNodeTransform(SceneNode &node)
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


            // If the scene node was selected, we should reposition the gizmo.
            this->RepositionGizmos();


            // If the node that was transformed is the main camera we'll need to scale the gizmos so that they look a constant size.
            if (&node == &this->currentState->camera || metadata->IsSelected())
            {
                this->RescaleGizmos();
            }

            if (this->currentState->selectedNodes.count == 1 && &node == this->currentState->selectedNodes[0])
            {
                this->GetScript().Execute("Editor.SceneEditor.UpdateTransformPanel();");
            }
        }
    }

    void Editor_SceneEditor::OnSceneNodeScale(SceneNode &node)
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
                    auto state = node.GetDataPointer<State>(0);
                    if (state != nullptr)
                    {
                        this->currentState->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                    }
                    else
                    {
                        GTEngine::Log("Scene Editor - Warning: Attempting to scale an object that is not part of any loaded scene.");
                    }
                }
            }
        }
    }

    void Editor_SceneEditor::OnSceneNodeHide(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            auto state = node.GetDataPointer<State>(0);
            if (state != nullptr)
            {
                state->pickingWorld.RemoveCollisionObject(metadata->GetPickingCollisionObject());
            }

            // We need to remove the collision object from the world so that we don't end up selecting it.
            //this->currentState->pickingWorld.RemoveCollisionObject(metadata->GetPickingCollisionObject());
        }
    }

    void Editor_SceneEditor::OnSceneNodeShow(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            auto state = node.GetDataPointer<State>(0);
            if (state != nullptr)
            {
                if (metadata->GetPickingCollisionShape() != nullptr)
                {
                    // We need to add the collision object to the world. We assert that the collision object is not already in the world.
                    auto &pickingCollisionObject = metadata->GetPickingCollisionObject();
                    assert(pickingCollisionObject.GetWorld() == nullptr);
                    {
                        state->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                    }
                }
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

    void Editor_SceneEditor::ShowPositionGizmo()
    {
        if (this->currentState != nullptr)
        {
            this->currentState->positionGizmo.Show();
            this->currentState->positionGizmo.SetPosition(this->GetSelectionCenterPoint());
            
            // We'll re-scale the gizmos just to make sure.
            this->RescaleGizmos();
        }
    }

    void Editor_SceneEditor::HideGizmos()
    {
        if (this->currentState != nullptr)
        {
            this->currentState->positionGizmo.Hide();
        }
    }

    void Editor_SceneEditor::RepositionGizmos()
    {
        if (this->currentState != nullptr)
        {
            this->currentState->positionGizmo.SetPosition(this->GetSelectionCenterPoint());
        }
    }

    void Editor_SceneEditor::RescaleGizmos()
    {
        // We're going to determine the new scale of the gizmos by using a project/unproject system. We first project the actual position of the gizmo
        // into window coordinates. We then add a value to the y result that will represent the size of the object on the screen. Then, we unproject
        // that position back into world space. The length between the gizmo position and unprojected position will be the new scale.
        if (this->currentState != nullptr)
        {
            glm::vec3 gizmoPosition = this->GetSelectionCenterPoint();

            glm::vec3 windowPos = this->currentState->viewport.Project(gizmoPosition);
            windowPos.y += 64.0f;

            glm::vec3 gizmoScale(glm::distance(this->currentState->viewport.Unproject(windowPos), gizmoPosition));
            this->currentState->positionGizmo.SetScale(gizmoScale);
        }
    }



    void Editor_SceneEditor::SetCurrentSceneInScript(Scene* scene, const char* elementID)
    {
        auto &script = this->editor.GetGame().GetScript();

        script.GetGlobal("Editor");
        assert(script.IsTable(-1));
        {
            script.Push("SceneEditor");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("SetCurrentScene");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(scene);
                    script.Push(elementID);
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }
        script.Pop(1);
    }

    GameScript & Editor_SceneEditor::GetScript()
    {
        return this->editor.GetGame().GetScript();
    }


    void Editor_SceneEditor::InitialiseScripting()
    {
        auto &script = this->editor.GetGame().GetScript();


        //////////////////////////////////////////
        // FFI

        script.GetGlobal("Editor");
        assert(script.IsTable(-1));
        {
            script.Push("SceneEditor");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.SetTableFunction(-1, "TryGizmoMouseSelect",      SceneEditorFFI::TryGizmoMouseSelect);
                script.SetTableFunction(-1, "DoMouseSelection",         SceneEditorFFI::DoMouseSelection);
                script.SetTableFunction(-1, "DeselectAll",              SceneEditorFFI::DeselectAll);
                script.SetTableFunction(-1, "SelectSceneNode",          SceneEditorFFI::SelectSceneNode);
                script.SetTableFunction(-1, "DeselectSceneNode",        SceneEditorFFI::DeselectSceneNode);
                script.SetTableFunction(-1, "DeleteSelectedSceneNodes", SceneEditorFFI::DeleteSelectedSceneNodes);
            }
            script.Pop(1);
        }
        script.Pop(1);
    }




    ////////////////////////////////////////////////
    // State

    Editor_SceneEditor::State::State(Editor_SceneEditor &sceneEditorIn)
        : sceneEditor(sceneEditorIn),
          scene(), sceneEventHandler(sceneEditor),
          viewport(), camera(),
          viewportEventHandler(sceneEditor.GetEditor().GetGame(), viewport),
          cameraXRotation(0.0f), cameraYRotation(0.0f),
          selectedNodes(),
          positionGizmo(),
          sceneNodesToDelete(),
          isDraggingGizmoX(false), isDraggingGizmoY(false), isDraggingGizmoZ(false), gizmoDragFactor(1.0f, 0.0f),
          GUI()
    {
        this->scene.AttachEventHandler(this->sceneEventHandler);


        this->camera.AddComponent<GTEngine::CameraComponent>();
        this->camera.AddComponent<GTEngine::AmbientLightComponent>()->SetColour(0.0f, 0.0f, 0.0f);
        this->camera.AddComponent<GTEngine::EditorMetadataComponent>();
        this->camera.SetDataPointer(0, this);


        this->viewport.SetCameraNode(this->camera);
        this->scene.AddViewport(this->viewport);
        this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);

        this->scene.AddSceneNode(this->camera);

        
        this->positionGizmo.Initialise();
        this->positionGizmo.GetSceneNode().SetDataPointer(0, this);
        this->positionGizmo.GetXArrowSceneNode().SetDataPointer(0, this);
        this->positionGizmo.GetYArrowSceneNode().SetDataPointer(0, this);
        this->positionGizmo.GetZArrowSceneNode().SetDataPointer(0, this);

        this->scene.AddSceneNode(this->positionGizmo.GetSceneNode());
        this->positionGizmo.Hide();
    }

    Editor_SceneEditor::State::~State()
    {
        for (size_t i = 0; i < this->sceneNodesToDelete.count; ++i)
        {
            delete this->sceneNodesToDelete[i];
        }

        this->sceneEditor.GetEditor().GetGame().GetGUI().DeleteElement(this->GUI.Main);
    }





    ////////////////////////////////////////////////
    // Scripting FFI

    Editor_SceneEditor & Editor_SceneEditor::SceneEditorFFI::GetSceneEditor(GTCore::Script &script)
    {
        return GameScript::FFI::GetGameObject(script).GetEditor().GetSceneEditor();
    }

    int Editor_SceneEditor::SceneEditorFFI::TryGizmoMouseSelect(GTCore::Script &script)
    {
        script.Push(GetSceneEditor(script).TryGizmoMouseSelect());
        return 1;
    }

    int Editor_SceneEditor::SceneEditorFFI::DoMouseSelection(GTCore::Script &script)
    {
        GetSceneEditor(script).DoMouseSelection();
        return 0;
    }

    int Editor_SceneEditor::SceneEditorFFI::DeselectAll(GTCore::Script &script)
    {
        GetSceneEditor(script).DeselectAll();
        return 0;
    }

    int Editor_SceneEditor::SceneEditorFFI::SelectSceneNode(GTCore::Script &script)
    {
        script.Push("_internalPtr");
        script.GetTableValue(1);
        assert(script.IsPointer(-1));
        {
            auto nodeToSelect = reinterpret_cast<SceneNode*>(script.ToPointer(-1));
            if (nodeToSelect != nullptr)
            {
                GetSceneEditor(script).SelectSceneNode(*nodeToSelect);
            }
        }
        script.Pop(1);

        return 0;
    }

    int Editor_SceneEditor::SceneEditorFFI::DeselectSceneNode(GTCore::Script &script)
    {
        script.Push("_internalPtr");
        script.GetTableValue(1);
        assert(script.IsPointer(-1));
        {
            auto nodeToSelect = reinterpret_cast<SceneNode*>(script.ToPointer(-1));
            if (nodeToSelect != nullptr)
            {
                GetSceneEditor(script).DeselectSceneNode(*nodeToSelect);
            }
        }
        script.Pop(1);

        return 0;
    }

    int Editor_SceneEditor::SceneEditorFFI::DeleteSelectedSceneNodes(GTCore::Script &script)
    {
        GetSceneEditor(script).DeleteSelectedSceneNodes();
        return 0;
    }
}
