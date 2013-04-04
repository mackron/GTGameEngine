// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Scene.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadSceneLibrary(GTCore::Script &script)
        {
            bool successful = script.Execute
            (
                "GTEngine.Scene = {};"
                "GTEngine.Scene.__index = GTEngine.Scene;"

                "function GTEngine.Scene:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.Scene);"
                "        new._internalPtr                            = internalPtr;"
                "        new._sceneNodes                             = {};"
                "        new._sceneNodesWithOnMouseMove              = {};"
                "        new._sceneNodesWithOnMouseWheel             = {};"
                "        new._sceneNodesWithOnMouseButtonDown        = {};"
                "        new._sceneNodesWithOnMouseButtonUp          = {};"
                "        new._sceneNodesWithOnMouseButtonDoubleClick = {};"
                "        new._sceneNodesWithOnKeyPressed             = {};"
                "        new._sceneNodesWithOnKeyReleased            = {};"
                ""
                "        new:InstantiateSceneNodesWithScriptComponents();"
                "    return new;"
                "end;"


                "function GTEngine.Scene:AddSceneNode(sceneNode)"
                "    GTEngine.System.Scene.AddSceneNode(self._internalPtr, sceneNode._internalPtr);"
                "end;"

                "function GTEngine.Scene:RemoveSceneNode(sceneNode)"
                "    GTEngine.System.Scene.RemoveSceneNode(self._internalPtr, sceneNode._internalPtr);"
                "end;"

                "function GTEngine.Scene:GetSceneNodePtrs()"
                "    return GTEngine.System.Scene.GetSceneNodePtrs(self._internalPtr);"
                "end;"


                "function GTEngine.Scene:InstantiateSceneNodesWithScriptComponents()"
                "    local sceneNodes = self:GetSceneNodePtrs();"
                "    for sceneNodeID,sceneNodePtr in pairs(sceneNodes) do"
                "        local scriptComponent = GTEngine.System.SceneNode.GetComponent(sceneNodePtr, GTEngine.Components.Script);"
                "        if scriptComponent ~= nil then"
                "            self:InstantiateSceneNode(sceneNodeID, sceneNodePtr);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:InstantiateSceneNode(sceneNodeID, sceneNodePtr)"
                "    local sceneNode = self._sceneNodes[sceneNodeID];"
                "    if sceneNode == nil then"
                "        if sceneNodePtr == nil then sceneNodePtr = self:GetSceneNodePtrByID(sceneNodeID) end;"
                "        if sceneNodePtr ~= nil then"
                "            sceneNode = GTEngine.SceneNode:Create(sceneNodePtr);"
                "            sceneNode.Scene = self;"
                ""
                "            self._sceneNodes[sceneNodeID] = sceneNode;"
                ""
                "            self:OnSceneNodeEventsChanged(sceneNode);"
                "        end;"
                "    end;"
                ""
                "    return sceneNode;"
                "end;"

                "function GTEngine.Scene:UninstantiateSceneNode(sceneNodeID)"
                "    self._sceneNodes[sceneNodeID]                = nil;"
                "    self._sceneNodesWithOnMouseMove[sceneNodeID] = nil;"
                "end;"

                "function GTEngine.Scene:GetSceneNodeByID(sceneNodeID)"
                "    return self:InstantiateSceneNode(sceneNodeID);"
                "end;"

                "function GTEngine.Scene:GetSceneNodePtrByID(sceneNodeID)"
                "    return GTEngine.System.Scene.GetSceneNodePtrByID(self._internalPtr, sceneNodeID);"
                "end;"


                "function GTEngine.Scene:CreateNewSceneNode()"
                "    local sceneNodePtr = GTEngine.System.Scene.CreateNewSceneNode(self._internalPtr);"
                "    if sceneNodePtr ~= nil then"
                "        return GTEngine.SceneNode:Create(sceneNodePtr);"
                "    end;"
                ""
                "    return nil;"
                "end;"


                "function GTEngine.Scene:OnSceneNodeEventsChanged(sceneNode)"
                "    local sceneNodeID = sceneNode:GetID();"
                ""
                "    self._sceneNodesWithOnMouseMove[sceneNodeID]              = nil;"
                "    self._sceneNodesWithOnMouseWheel[sceneNodeID]             = nil;"
                "    self._sceneNodesWithOnMouseButtonDown[sceneNodeID]        = nil;"
                "    self._sceneNodesWithOnMouseButtonUp[sceneNodeID]          = nil;"
                "    self._sceneNodesWithOnMouseButtonDoubleClick[sceneNodeID] = nil;"
                "    self._sceneNodesWithOnKeyPressed[sceneNodeID]             = nil;"
                "    self._sceneNodesWithOnKeyReleased[sceneNodeID]            = nil;"
                ""
                "    if sceneNode.OnMouseMove ~= nil then"
                "        self._sceneNodesWithOnMouseMove[sceneNodeID] = sceneNode;"
                "    end;"
                "    if sceneNode.OnMouseWheel ~= nil then"
                "        self._sceneNodesWithOnMouseWheel[sceneNodeID] = sceneNode;"
                "    end;"
                "    if sceneNode.OnMouseButtonDown ~= nil then"
                "        self._sceneNodesWithOnMouseButtonDown[sceneNodeID] = sceneNode;"
                "    end;"
                "    if sceneNode.OnMouseButtonUp ~= nil then"
                "        self._sceneNodesWithOnMouseButtonUp[sceneNodeID] = sceneNode;"
                "    end;"
                "    if sceneNode.OnMouseButtonDoubleClick ~= nil then"
                "        self._sceneNodesWithOnMouseButtonDoubleClick[sceneNodeID] = sceneNode;"
                "    end;"
                "    if sceneNode.OnKeyPressed ~= nil then"
                "        self._sceneNodesWithOnKeyPressed[sceneNodeID] = sceneNode;"
                "    end;"
                "    if sceneNode.OnKeyReleased ~= nil then"
                "        self._sceneNodesWithOnKeyReleased[sceneNodeID] = sceneNode;"
                "    end;"
                "end;"


                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseMove(mousePosX, mousePosY)"
                "    if not self:IsPaused() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseMove) do"
                "            sceneNode:OnMouseMove(mousePosX, mousePosY);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseWheel(mousePosX, mousePosY, delta)"
                "    if not self:IsPaused() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseWheel) do"
                "            sceneNode:OnMouseWheel(mousePosX, mousePosY, delta);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseButtonDown(mousePosX, mousePosY, button)"
                "    if not self:IsPaused() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseButtonDown) do"
                "            sceneNode:OnMouseButtonDown(mousePosX, mousePosY, button);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseButtonUp(mousePosX, mousePosY, button)"
                "    if not self:IsPaused() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseButtonUp) do"
                "            sceneNode:OnMouseButtonUp(mousePosX, mousePosY, button);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseButtonDoubleClick(mousePosX, mousePosY, button)"
                "    if not self:IsPaused() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseButtonDoubleClick) do"
                "            sceneNode:OnMouseButtonDoubleClick(mousePosX, mousePosY, button);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnKeyPressed(key)"
                "    if not self:IsPaused() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnKeyPressed) do"
                "            sceneNode:OnKeyPressed(key);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnKeyReleased(key)"
                "    if not self:IsPaused() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnKeyReleased) do"
                "            sceneNode:OnKeyReleased(key);"
                "        end;"
                "    end;"
                "end;"


                "function GTEngine.Scene:IsPaused()"
                "    return GTEngine.System.Scene.IsPaused(self._internalPtr);"
                "end;"

                "function GTEngine.Scene:SetViewportCamera(cameraNode, viewportIndex)"
                "    return self:SetViewportCameraByPtr(cameraNode._internalPtr, viewportIndex);"
                "end;"

                "function GTEngine.Scene:SetViewportCameraByPtr(cameraNodePtr, viewportIndex)"
                "    return GTEngine.System.Scene.SetViewportCamera(self._internalPtr, cameraNodePtr, viewportIndex);"
                "end;"

                "function GTEngine.Scene:ApplyViewportCameraAspectRatio(viewportIndex)"
                "    GTEngine.System.Scene.ApplyViewportCameraAspectRatio(self._internalPtr, viewportIndex);"
                "end;"



                "GTEngine.RegisteredScenes = {};"
            );

            if (successful)
            {
                script.GetGlobal("GTEngine");
                assert(script.IsTable(-1));
                {
                    script.Push("System");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.Push("Scene");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.SetTableFunction(-1, "AddSceneNode",                   SceneFFI::AddSceneNode);
                            script.SetTableFunction(-1, "RemoveSceneNode",                SceneFFI::RemoveSceneNode);
                            script.SetTableFunction(-1, "CreateNewSceneNode",             SceneFFI::CreateNewSceneNode);
                            script.SetTableFunction(-1, "GetSceneNodePtrs",               SceneFFI::GetSceneNodePtrs);
                            script.SetTableFunction(-1, "GetSceneNodePtrByID",            SceneFFI::GetSceneNodePtrByID);
                            script.SetTableFunction(-1, "IsPaused",                       SceneFFI::IsPaused);
                            script.SetTableFunction(-1, "SetViewportCamera",              SceneFFI::SetViewportCamera);
                            script.SetTableFunction(-1, "ApplyViewportCameraAspectRatio", SceneFFI::ApplyViewportCameraAspectRatio);
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }


            return true;
        }


        bool RegisterScene(GTCore::Script &script, Scene &scene)
        {
            // We map Lua Scene objects to the C++ pointer counterparts. We store this in GTEngine.RegisteredScenes.

            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    // Key (C++ pointer).
                    script.Push(&scene);

                    // Value (Lua object).
                    script.Push("Scene");
                    script.GetTableValue(-4);       // <-- GTEngine
                    assert(script.IsTable(-1));
                    {
                        script.Push("Create");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);       // Argument 1: GTEngine.Scene.
                            script.Push(&scene);        // Argument 2: The C++ pointer.

                            script.Call(2, 1);          // 1 return value: The Lua Scene object.

                            // We need to move the return value so that it's in the correct position after all of the pops below.
                            script.InsertIntoStack(-2);
                        }
                    }
                    script.Pop(1);

                    assert(script.IsPointer(-2));
                    assert(script.IsTable(-1));
                    {
                        // The key and value are in place, so now we just want to set it.
                        script.SetTableValue(-3);
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);


            return true;
        }

        void UnregisterScene(GTCore::Script &script, Scene &scene)
        {
            // For now we'll just set the value in GTEngine.RegisteredScenes to nil, but this might need improving later on.

            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(&scene);        // Key   - C++ pointer.
                    script.PushNil();           // Value - Lua object, or in this case nil so that it's removed.
                    script.SetTableValue(-3);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }


        namespace SceneFFI
        {
            int AddSceneNode(GTCore::Script &script)
            {
                auto scene     = reinterpret_cast<Scene*    >(script.ToPointer(1));
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (scene != nullptr && sceneNode != nullptr)
                {
                    scene->AddSceneNode(*sceneNode);
                }

                return 0;
            }

            int RemoveSceneNode(GTCore::Script &script)
            {
                auto scene     = reinterpret_cast<Scene*    >(script.ToPointer(1));
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (scene != nullptr && sceneNode != nullptr)
                {
                    scene->RemoveSceneNode(*sceneNode);
                }

                return 0;
            }

            int CreateNewSceneNode(GTCore::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->CreateNewSceneNode());
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }


            int GetSceneNodePtrs(GTCore::Script &script)
            {
                script.PushNewTable();

                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    size_t sceneNodeCount = scene->GetSceneNodeCount();
                    for (size_t i = 0; i < sceneNodeCount; ++i)
                    {
                        auto sceneNode = scene->GetSceneNodeByIndex(i);
                        assert(sceneNode != nullptr);
                        {
                            script.SetTableValue(-1, static_cast<int>(sceneNode->GetID()), sceneNode);
                        }
                    }
                }

                return 1;
            }


            int GetSceneNodePtrByID(GTCore::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->GetSceneNodeByID(static_cast<uint64_t>(script.ToInteger(2))));
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }


            int IsPaused(GTCore::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->IsPaused());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int SetViewportCamera(GTCore::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                    if (sceneNode != nullptr)
                    {
                        int viewportIndex = script.ToInteger(3);
                        if (static_cast<size_t>(viewportIndex) < scene->GetViewportCount())
                        {
                            scene->GetViewportByIndex(viewportIndex).SetCameraNode(*sceneNode);
                        }
                    }
                }

                return 0;
            }

            int ApplyViewportCameraAspectRatio(GTCore::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    int viewportIndex = script.ToInteger(2);
                    if (static_cast<size_t>(viewportIndex) < scene->GetViewportCount())
                    {
                        auto &viewport = scene->GetViewportByIndex(viewportIndex);
                        {
                            auto sceneNode = viewport.GetCameraNode();
                            if (sceneNode != nullptr)
                            {
                                auto cameraComponent = sceneNode->GetComponent<CameraComponent>();
                                if (cameraComponent != nullptr)
                                {
                                    cameraComponent->Set3DProjection(cameraComponent->perspective.fov, static_cast<float>(viewport.GetWidth()) / static_cast<float>(viewport.GetHeight()), cameraComponent->zNear, cameraComponent->zFar);
                                }
                            }
                        }
                    }
                }

                return 0;
            }
        }
    }
}