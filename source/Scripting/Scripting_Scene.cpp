// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Scripting/Scripting_Scene.hpp>
#include <GTEngine/Scripting/Scripting_SceneNode.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/PrefabLibrary.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        /// The ray test callback structure for performing a ray test from the scripting environment.
        ///
        /// This structure is not intended to be used externally.
        struct ScriptRayTestCallback : public RayTestCallback
        {
        public:
            
            /// Constructor.
            ///
            /// @remarks
            ///     The 'script' object should have the callback structure sitting at the top of the stack at index -1.
            ScriptRayTestCallback(GTLib::Script &script)
                : RayTestCallback(), m_script(script)
            {
                // The top item should be a table. We can assert this because this check should have been done at a higher level. In this table we will check
                // for the 'collisionGroup' and 'collidesWith' fields. If these are present, we do an optimized NeedsCollision() implementation.
                assert(m_script.IsTable(-1));
                {
                    m_script.Push("collisionGroup");
                    m_script.GetTableValue(-2);
                    if (m_script.IsTable(-1))
                    {
                        m_script.Push("bitfield");
                        m_script.GetTableValue(-2);
                        if (m_script.IsNumber(-1))
                        {
                            this->collisionGroup = static_cast<short>(m_script.ToInteger(-1));
                        }
                        m_script.Pop(1);
                    }
                    m_script.Pop(1);
                    
                    m_script.Push("collidesWith");
                    m_script.GetTableValue(-2);
                    if (m_script.IsTable(-1))
                    {
                        m_script.Push("bitfield");
                        m_script.GetTableValue(-2);
                        if (m_script.IsNumber(-1))
                        {
                            this->collisionMask = static_cast<short>(m_script.ToInteger(-1));
                        }
                        m_script.Pop(1);
                    }
                    m_script.Pop(1);
                }
            }
            
            /// Destructor.
            ~ScriptRayTestCallback()
            {
            }
            
            
            /// RayTestCallback::NeedsCollision()
            bool NeedsCollision(short collisionGroup, short collisionMask, SceneNode &otherSceneNode)
            {
                bool result = RayTestCallback::NeedsCollision(collisionGroup, collisionMask, otherSceneNode);
                if (result)
                {
                    // We now need to check if the table at the top of the stack on the script contains the method "NeedsCollision". If so, call it and look
                    // at the return value.
                    assert(m_script.IsTable(-1));
                    {
                        m_script.Push("NeedsCollision");
                        m_script.GetTableValue(-2);
                        if (m_script.IsFunction(-1))
                        {
                            // We want to pass a reference to the scripting representation of the scene node. To do this, we first need to grab the scene, and then
                            // from that grab the scene node by it's ID.
                            m_script.PushValue(-2);                                 // self
                            Scripting::PushSceneNode(m_script, otherSceneNode);     // otherSceneNode
                            m_script.Call(2, 1);
                            {
                                result = m_script.ToBoolean(-1);
                            }
                            m_script.Pop(1);    // The return value of NeedsCollision().
                        }
                        else
                        {
                            // NeedsCollision is not a function (probably nil), so it needs to be popped. We don't do this when it is a function, because the call
                            // will pop the function automatically.
                            m_script.Pop(1);
                        }
                    }
                }
                
                return result;
            }
            
            /// RayTestCallback::ProcessResult()
            void ProcessResult(SceneNode &sceneNode, const glm::vec3 &worldPosition, const glm::vec3 &worldNormal)
            {
                assert(m_script.IsTable(-1));
                {
                    m_script.Push("ProcessResult");
                    m_script.GetTableValue(-2);
                    if (m_script.IsFunction(-1))
                    {
                        m_script.PushValue(-2);                                 // self
                        Scripting::PushSceneNode(m_script, sceneNode);          // sceneNode
                        Scripting::PushNewVector3(m_script, worldPosition);     // worldPosition
                        Scripting::PushNewVector3(m_script, worldNormal);       // worldNormal
                        m_script.Call(4, 0);
                    }
                    else
                    {
                        m_script.Pop(1);
                    }
                }
            }
            
            
        private:
            
            /// A reference to the script object which will have functions called on it.
            GTLib::Script &m_script;
            
            
        private:    // No copyiong.
            ScriptRayTestCallback(const ScriptRayTestCallback &);
            ScriptRayTestCallback & operator=(const ScriptRayTestCallback &);
        };
        
        
        bool LoadSceneLibrary(GTLib::Script &script)
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
                "        new._sceneNodesWithOnGamePause              = {};"
                "        new._sceneNodesWithOnGameResume             = {};"
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
                "    self._sceneNodes[sceneNodeID]                             = nil;"
                "    self._sceneNodesWithOnMouseMove[sceneNodeID]              = nil;"
                "    self._sceneNodesWithOnMouseWheel[sceneNodeID]             = nil;"
                "    self._sceneNodesWithOnMouseButtonDown[sceneNodeID]        = nil;"
                "    self._sceneNodesWithOnMouseButtonUp[sceneNodeID]          = nil;"
                "    self._sceneNodesWithOnMouseButtonDoubleClick[sceneNodeID] = nil;"
                "    self._sceneNodesWithOnKeyPressed[sceneNodeID]             = nil;"
                "    self._sceneNodesWithOnKeyReleased[sceneNodeID]            = nil;"
                "    self._sceneNodesWithOnGamePause[sceneNodeID]              = nil;"
                "    self._sceneNodesWithOnGameResume[sceneNodeID]             = nil;"
                "end;"

                "function GTEngine.Scene:GetSceneNodeByID(sceneNodeID)"
                "    return self:InstantiateSceneNode(sceneNodeID);"
                "end;"
                
                "function GTEngine.Scene:GetSceneNodeByName(sceneNodeName)"
                "    return self:GetSceneNodeByID(self:GetSceneNodeIDByName(sceneNodeName));"
                "end;"

                "function GTEngine.Scene:GetSceneNodeByPtr(sceneNodePtr)"
                "    return self:GetSceneNodeByID(self:GetSceneNodeIDByPtr(sceneNodePtr));"
                "end;"


                "function GTEngine.Scene:GetSceneNodePtrByID(sceneNodeID)"
                "    return GTEngine.System.Scene.GetSceneNodePtrByID(self._internalPtr, sceneNodeID);"
                "end;"

                "function GTEngine.Scene:GetSceneNodePtrByName(sceneNodeName)"
                "    return GTEngine.System.Scene.GetSceneNodePtrByName(self._internalPtr, sceneNodeName);"
                "end;"


                "function GTEngine.Scene:GetSceneNodeIDByName(sceneNodeName)"
                "    return GTEngine.System.Scene.GetSceneNodeIDByName(self._internalPtr, sceneNodeName);"
                "end;"

                "function GTEngine.Scene:GetSceneNodeIDByPtr(sceneNodePtr)"
                "    return GTEngine.System.SceneNode.GetID(sceneNodePtr);"
                "end;"



                "function GTEngine.Scene:CreateNewSceneNode(prefabFileName)"
                "    local sceneNodePtr = GTEngine.System.Scene.CreateNewSceneNode(self._internalPtr, prefabFileName);"
                "    if sceneNodePtr ~= nil then"
                "        return self:InstantiateSceneNode(GTEngine.System.SceneNode.GetID(sceneNodePtr), sceneNodePtr);"
                "    end;"
                ""
                "    return nil;"
                "end;"

                "function GTEngine.Scene:InstantiatePrefab(prefabFileName)"
                "    if prefabFileName ~= nil then"
                "        return self:CreateNewSceneNode(prefabFileName);"
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
                "    self._sceneNodesWithOnGamePause[sceneNodeID]              = nil;"
                "    self._sceneNodesWithOnGameResume[sceneNodeID]             = nil;"
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
                "    if sceneNode.OnGamePause ~= nil then"
                "        self._sceneNodesWithOnGamePause[sceneNodeID] = sceneNode;"
                "    end;"
                "    if sceneNode.OnGameResume ~= nil then"
                "        self._sceneNodesWithOnGameResume[sceneNodeID] = sceneNode;"
                "    end;"
                "end;"


                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseMove(mousePosX, mousePosY)"
                "    if not self:IsPaused() and not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseMove) do"
                "            sceneNode:OnMouseMove(mousePosX, mousePosY);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseWheel(mousePosX, mousePosY, delta)"
                "    if not self:IsPaused() and not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseWheel) do"
                "            sceneNode:OnMouseWheel(mousePosX, mousePosY, delta);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseButtonDown(mousePosX, mousePosY, button)"
                "    if not self:IsPaused() and not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseButtonDown) do"
                "            sceneNode:OnMouseButtonDown(mousePosX, mousePosY, button);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseButtonUp(mousePosX, mousePosY, button)"
                "    if not self:IsPaused() and not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseButtonUp) do"
                "            sceneNode:OnMouseButtonUp(mousePosX, mousePosY, button);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnMouseButtonDoubleClick(mousePosX, mousePosY, button)"
                "    if not self:IsPaused() and not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnMouseButtonDoubleClick) do"
                "            sceneNode:OnMouseButtonDoubleClick(mousePosX, mousePosY, button);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnKeyPressed(key)"
                "    if not self:IsPaused() and not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnKeyPressed) do"
                "            sceneNode:OnKeyPressed(key);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnKeyReleased(key)"
                "    if not self:IsPaused() and not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnKeyReleased) do"
                "            sceneNode:OnKeyReleased(key);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnGamePause()"
                "    if not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnGamePause) do"
                "            sceneNode:OnGamePause();"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:PostSceneNodeEvent_OnGameResume()"
                "    if not self:IsScriptEventsBlocked() then"
                "        for sceneNodeID,sceneNode in pairs(self._sceneNodesWithOnGameResume) do"
                "            sceneNode:OnGameResume();"
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
                "    GTEngine.System.Scene.SetViewportCamera(self._internalPtr, cameraNodePtr, viewportIndex);"
                "    self:ApplyViewportCameraAspectRatio(viewportIndex);"
                "end;"

                "function GTEngine.Scene:ApplyViewportCameraAspectRatio(viewportIndex)"
                "    GTEngine.System.Scene.ApplyViewportCameraAspectRatio(self._internalPtr, viewportIndex);"
                "end;"


                "function GTEngine.Scene:IsScriptEventsBlocked()"
                "    return GTEngine.System.Scene.IsScriptEventsBlocked(self._internalPtr);"
                "end;"


                "function GTEngine.Scene:SetWalkableHeight(value)"
                "    return GTEngine.System.Scene.SetWalkableHeight(self._internalPtr, value);"
                "end;"

                "function GTEngine.Scene:SetWalkableRadius(value)"
                "    return GTEngine.System.Scene.SetWalkableRadius(self._internalPtr, value);"
                "end;"

                "function GTEngine.Scene:SetWalkableSlopeAngle(value)"
                "    return GTEngine.System.Scene.SetWalkableSlopeAngle(self._internalPtr, value);"
                "end;"

                "function GTEngine.Scene:SetWalkableClimbHeight(value)"
                "    return GTEngine.System.Scene.SetWalkableClimbHeight(self._internalPtr, value);"
                "end;"

                "function GTEngine.Scene:GetWalkableHeight()"
                "    return GTEngine.System.Scene.GetWalkableHeight(self._internalPtr);"
                "end;"

                "function GTEngine.Scene:GetWalkableRadius()"
                "    return GTEngine.System.Scene.GetWalkableRadius(self._internalPtr);"
                "end;"

                "function GTEngine.Scene:GetWalkableSlopeAngle()"
                "    return GTEngine.System.Scene.GetWalkableSlopeAngle(self._internalPtr);"
                "end;"

                "function GTEngine.Scene:GetWalkableClimbHeight()"
                "    return GTEngine.System.Scene.GetWalkableClimbHeight(self._internalPtr);"
                "end;"

                "function GTEngine.Scene:BuildNavigationMesh(index)"
                "    return GTEngine.System.Scene.BuildNavigationMesh(self._internalPtr, index);"
                "end;"


                "function GTEngine.Scene:CalculateViewportPickingRay(x, y, viewportIndex)"
                "    return GTEngine.System.Scene.CalculateViewportPickingRay(self._internalPtr, x, y, viewportIndex);"
                "end;"

                "function GTEngine.Scene:RayTest(rayNear, rayFar, callback)"
                "    local sceneNodePtr = GTEngine.System.Scene.RayTest(self._internalPtr, rayNear, rayFar, callback);"
                "    if sceneNodePtr ~= nil then"
                "        return self:GetSceneNodeByPtr(sceneNodePtr);"
                "    end;"
                ""
                "    return nil;"
                "end;"


                "function GTEngine.Scene:SetGravity(gravity)"
                "    return GTEngine.System.Scene.SetGravity(self._internalPtr, gravity);"
                "end;"

                "function GTEngine.Scene:GetGravity()"
                "    return GTEngine.System.Scene.GetGravity(self._internalPtr);"
                "end;"


                "GTEngine.RegisteredScenes = {};"
            );
            
            successful = successful & script.Execute
            (
                "GTEngine.__DefaultRayTestCallback = {};"
                "GTEngine.__DefaultRayTestCallback.__index = GTEngine.__DefaultRayTestCallback;"
                
                "function GTEngine.__DefaultRayTestCallback.ProcessResult(self, sceneNode, worldPosition, worldNormal)"
                "    self.sceneNode     = sceneNode;"
                "    self.worldPosition = worldPosition;"
                "    self.worldNormal   = worldNormal;"
                "end;"
                
                "function GTEngine.DefaultRayTestCallback()"
                "    local new = {};"
                "    setmetatable(new, GTEngine.__DefaultRayTestCallback);"
                "        new.collisionGroup = GTEngine.CollisionGroupMask();"
                "        new.collidesWith   = GTEngine.CollisionGroupMask();"
                "        new.sceneNode      = nil;"
                "        new.worldPosition  = math.vec3(0.0, 0.0,  0.0);"
                "        new.worldNormal    = math.vec3(0.0, 0.0, -1.0)"
                "    return new;"
                "end;"
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
                        script.PushNewTable();
                        {
                            script.SetTableFunction(-1, "AddSceneNode",                   SceneFFI::AddSceneNode);
                            script.SetTableFunction(-1, "RemoveSceneNode",                SceneFFI::RemoveSceneNode);
                            script.SetTableFunction(-1, "CreateNewSceneNode",             SceneFFI::CreateNewSceneNode);
                            script.SetTableFunction(-1, "GetSceneNodePtrs",               SceneFFI::GetSceneNodePtrs);
                            script.SetTableFunction(-1, "GetSceneNodePtrByID",            SceneFFI::GetSceneNodePtrByID);
                            script.SetTableFunction(-1, "GetSceneNodePtrByName",          SceneFFI::GetSceneNodePtrByName);
                            script.SetTableFunction(-1, "GetSceneNodeIDByName",           SceneFFI::GetSceneNodeIDByName);
                            script.SetTableFunction(-1, "IsPaused",                       SceneFFI::IsPaused);
                            script.SetTableFunction(-1, "SetViewportCamera",              SceneFFI::SetViewportCamera);
                            script.SetTableFunction(-1, "ApplyViewportCameraAspectRatio", SceneFFI::ApplyViewportCameraAspectRatio);
                            script.SetTableFunction(-1, "IsScriptEventsBlocked",          SceneFFI::IsScriptEventsBlocked);
                            script.SetTableFunction(-1, "SetWalkableHeight",              SceneFFI::SetWalkableHeight);
                            script.SetTableFunction(-1, "SetWalkableRadius",              SceneFFI::SetWalkableRadius);
                            script.SetTableFunction(-1, "SetWalkableSlopeAngle",          SceneFFI::SetWalkableSlopeAngle);
                            script.SetTableFunction(-1, "SetWalkableClimbHeight",         SceneFFI::SetWalkableClimbHeight);
                            script.SetTableFunction(-1, "GetWalkableHeight",              SceneFFI::GetWalkableHeight);
                            script.SetTableFunction(-1, "GetWalkableRadius",              SceneFFI::GetWalkableRadius);
                            script.SetTableFunction(-1, "GetWalkableSlopeAngle",          SceneFFI::GetWalkableSlopeAngle);
                            script.SetTableFunction(-1, "GetWalkableClimbHeight",         SceneFFI::GetWalkableClimbHeight);
                            script.SetTableFunction(-1, "BuildNavigationMesh",            SceneFFI::BuildNavigationMesh);
                            script.SetTableFunction(-1, "CalculateViewportPickingRay",    SceneFFI::CalculateViewportPickingRay);
                            script.SetTableFunction(-1, "RayTest",                        SceneFFI::RayTest);
                            script.SetTableFunction(-1, "SetGravity",                     SceneFFI::SetGravity);
                            script.SetTableFunction(-1, "GetGravity",                     SceneFFI::GetGravity);
                        }
                        script.SetTableValue(-3);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }


            return successful;
        }


        bool RegisterScene(GTLib::Script &script, Scene &scene)
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

        void UnregisterScene(GTLib::Script &script, Scene &scene)
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
            int AddSceneNode(GTLib::Script &script)
            {
                auto scene     = reinterpret_cast<Scene*    >(script.ToPointer(1));
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (scene != nullptr && sceneNode != nullptr)
                {
                    scene->AddSceneNode(*sceneNode);
                }

                return 0;
            }

            int RemoveSceneNode(GTLib::Script &script)
            {
                auto scene     = reinterpret_cast<Scene*    >(script.ToPointer(1));
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (scene != nullptr && sceneNode != nullptr)
                {
                    scene->RemoveSceneNode(*sceneNode);
                }

                return 0;
            }

            int CreateNewSceneNode(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    if (script.IsString(2))
                    {
                        auto prefab = PrefabLibrary::Acquire(script.ToString(2));
                        if (prefab != nullptr)
                        {
                            auto sceneNode = scene->CreateNewSceneNode(*prefab);
                            if (sceneNode != nullptr)
                            {
                                script.Push(sceneNode);
                            }
                            else
                            {
                                script.PushNil();
                            }
                        }
                        else
                        {
                            script.PushNil();
                        }
                    }
                    else
                    {
                        script.Push(scene->CreateNewSceneNode());
                    }
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }


            int GetSceneNodePtrs(GTLib::Script &script)
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


            int GetSceneNodePtrByID(GTLib::Script &script)
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

            int GetSceneNodePtrByName(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->GetSceneNodeByName(script.ToString(2)));
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }

            int GetSceneNodeIDByName(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(static_cast<int>(scene->GetSceneNodeIDByName(script.ToString(2))));
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }


            int IsPaused(GTLib::Script &script)
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


            int SetViewportCamera(GTLib::Script &script)
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

            int ApplyViewportCameraAspectRatio(GTLib::Script &script)
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


            int IsScriptEventsBlocked(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->IsScriptEventsBlocked());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int SetWalkableHeight(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    scene->SetWalkableHeight(script.ToFloat(2));
                }

                return 0;
            }

            int SetWalkableRadius(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    scene->SetWalkableRadius(script.ToFloat(2));
                }

                return 0;
            }

            int SetWalkableSlopeAngle(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    scene->SetWalkableSlopeAngle(script.ToFloat(2));
                }

                return 0;
            }

            int SetWalkableClimbHeight(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    scene->SetWalkableClimbHeight(script.ToFloat(2));
                }

                return 0;
            }

            int GetWalkableHeight(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->GetWalkableHeight());
                }
                else
                {
                    script.Push(0.0f);
                }

                return 1;
            }

            int GetWalkableRadius(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->GetWalkableRadius());
                }
                else
                {
                    script.Push(0.0f);
                }

                return 1;
            }

            int GetWalkableSlopeAngle(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->GetWalkableSlopeAngle());
                }
                else
                {
                    script.Push(0.0f);
                }

                return 1;
            }

            int GetWalkableClimbHeight(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    script.Push(scene->GetWalkableClimbHeight());
                }
                else
                {
                    script.Push(0.0f);
                }

                return 1;
            }

            int BuildNavigationMesh(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    scene->BuildNavigationMesh(static_cast<size_t>(script.ToInteger(2)));
                }

                return 0;
            }



            int CalculateViewportPickingRay(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    glm::vec3 rayStart;
                    glm::vec3 rayEnd;
                    scene->GetViewportByIndex(script.ToInteger(4)).CalculatePickingRay(script.ToInteger(2), script.ToInteger(3), rayStart, rayEnd);
                
                    Scripting::PushNewVector3(script, rayStart);
                    Scripting::PushNewVector3(script, rayEnd);

                    return 2;
                }

                return 0;
            }

            int RayTest(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    glm::vec3 rayNear = Scripting::ToVector3(script, 2);
                    glm::vec3 rayFar  = Scripting::ToVector3(script, 3);
                    
                    // We should usually have a table as the fourth parameter. If not we just do a test against all objects
                    // and return the closest scene node.
                    if (script.IsTable(4))
                    {
                        script.PushValue(4);    // We want the callback to be at -1.
                        Scripting::ScriptRayTestCallback callback(script);
                        
                        auto result = scene->RayTest(rayNear, rayFar, callback);
                        if (result != nullptr)
                        {
                            Scripting::PushSceneNode(script, *result);
                        }
                        else
                        {
                            script.PushNil();
                        }
                    }
                    else
                    {
                        ClosestRayExceptMeTestCallback callback;

                        auto result = scene->RayTest(rayNear, rayFar, callback);
                        if (result != nullptr)
                        {
                            Scripting::PushSceneNode(script, *result);
                        }
                        else
                        {
                            script.PushNil();
                        }
                    }
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }



            int SetGravity(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    scene->SetGravity(Scripting::ToVector3(script, 2));
                }

                return 0;
            }

            int GetGravity(GTLib::Script &script)
            {
                auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                if (scene != nullptr)
                {
                    Scripting::PushNewVector3(script, scene->GetGravity());
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, -9.81f, 0.0f);
                }

                return 1;
            }
        }
    }
}
