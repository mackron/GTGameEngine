
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/CollisionGroups.hpp>

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
                GTCore::IO::Close(file);
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
        if (this->currentState != nullptr && !this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Select();

                assert(this->currentState->selectedNodes.Exists(&node) == false);
                {
                    this->currentState->selectedNodes.PushBack(&node);

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
                }
            }
        }
    }

    void Editor_SceneEditor::DeselectSceneNode(SceneNode &node)
    {
        if (this->currentState != nullptr && this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Deselect();

                assert(this->currentState->selectedNodes.Exists(&node) == true);
                {
                    this->currentState->selectedNodes.RemoveFirstOccuranceOf(&node);

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
                }
            }
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


    ///////////////////////////////////////////////////
    // Scene Events
    //
    // These method should only be called for the active scene.

    void Editor_SceneEditor::OnObjectAdded(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);
            
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata == nullptr)
            {
                metadata = node.AddComponent<EditorMetadataComponent>();
            }

            // We can cheat here and just act as if the object has been refreshed.
            this->OnObjectRefreshed(object);
        }
    }

    void Editor_SceneEditor::OnObjectRemoved(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            // We need to make sure scene nodes are deseleted when they are removed from the scene.
            this->DeselectSceneNode(static_cast<SceneNode &>(object));


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
                    metadata->SetPickingCollisionShapeToModel();
                }
                else
                {
                    // TODO: If the object is using a sprite for visual representation, we should setup a collision volume for that. We could use a sphere for that, I think... Nice and simple...
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
                    this->currentState->pickingWorld.AddCollisionObject(pickingCollisionObject, CollisionGroups::EditorSelectionVolume, CollisionGroups::EditorSelectionRay);
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
                metadata->GetPickingCollisionShape()->setLocalScaling(ToBulletVector3(node.GetWorldScale()));
                this->currentState->pickingWorld.AddCollisionObject(pickingCollisionObject, CollisionGroups::EditorSelectionVolume, CollisionGroups::EditorSelectionRay);
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
                script.SetTableFunction(-1, "DoMouseSelection",  SceneEditorFFI::DoMouseSelection);
                script.SetTableFunction(-1, "DeselectAll",       SceneEditorFFI::DeselectAll);
                script.SetTableFunction(-1, "SelectSceneNode",   SceneEditorFFI::SelectSceneNode);
                script.SetTableFunction(-1, "DeselectSceneNode", SceneEditorFFI::DeselectSceneNode);
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
          GUI()
    {
        this->scene.AttachEventHandler(this->sceneEventHandler);

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





    ////////////////////////////////////////////////
    // Scripting FFI

    Editor_SceneEditor & Editor_SceneEditor::SceneEditorFFI::GetSceneEditor(GTCore::Script &script)
    {
        return GameScript::FFI::GetGameObject(script).GetEditor().GetSceneEditor();
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
}
