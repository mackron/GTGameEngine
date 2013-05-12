// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_SceneNode.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadSceneNodeLibrary(GTCore::Script &script)
        {
            bool successful = script.Execute
            (
                "GTEngine.SceneNode = {};"
                "GTEngine.SceneNode.__index = GTEngine.SceneNode;"

                "function GTEngine.SceneNode:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.SceneNode);"
                "        if internalPtr == nil then"
                "            new._internalPtr       = GTEngine.System.SceneNode.Create();"
                "            new._deleteInternalPtr = true;"
                "        else"
                "            new._internalPtr       = internalPtr;"
                "            new._deleteInternalPtr = false;"
                "        end"
                ""
                "        new._eventHandlers   = {};"
                "        new._scriptVariables = {};"
                "        new.Scene            = nil;"
                ""
                "        new:RegisterScriptComponent();"
                "    return new;"
                "end;"

                "function GTEngine.SceneNode:Delete()"
                "    if self._deleteInternalPtr then"
                "        GTEngine.System.SceneNode.Delete(self._internalPtr);"
                "    end;"
                "end;"


                "function GTEngine.SceneNode:RegisterScriptComponent()"
                "    local prevScriptVariables = self._scriptVariables;"
                "    self._eventHandlers   = {};"
                "    self._scriptVariables = {};"
                ""
                "    local scriptComponent = self:GetComponent(GTEngine.Components.Script);"
                "    if scriptComponent ~= nil then"
                "        local scriptFilePaths = scriptComponent:GetScriptFilePaths();"
                "        for i,scriptPath in ipairs(scriptFilePaths) do"
                "            self:LinkToScript(GTEngine.ScriptDefinitions[scriptPath]);"
                "            self:UpdatePublicVariables();"
                "        end;"
                "    end;"
                ""
                "    self:RemoveUnusedScriptVariables(prevScriptVariables);"
                ""
                "    if self.Scene ~= nil then"
                "        self.Scene:OnSceneNodeEventsChanged(self);"
                "    end;"
                "end;"

                "function GTEngine.SceneNode:LinkToScript(script)"
                "    if script ~= nil then"
                "        for key,value in pairs(script) do"
                "            if not GTEngine.IsSceneNodeEventHandler(key) then"
                "                self[key] = value;"
                "                self._scriptVariables[#self._scriptVariables + 1] = key;"
                "            else"
                "                self:RegisterEventHandler(key, value);"
                "            end;"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.SceneNode:UpdatePublicVariables(scriptComponent)"
                "    if scriptComponent == nil then scriptComponent = self:GetComponent(GTEngine.Components.Script) end;"
                "    if scriptComponent ~= nil then"
                "        local variables = scriptComponent:GetPublicVariableNamesAndValues();"
                "        for name,value in pairs(variables) do"
                "            self[name] = value;"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.SceneNode:RegisterEventHandler(name, value)"
                "    if name ~= nil and value ~= nil then"
                "        if self._eventHandlers[name] == nil then"
                "            self._eventHandlers[name] = {};"
                ""
                "            self[name] = function(self, ...)"
                "                for i,eventHandler in pairs(self._eventHandlers[name]) do"
                "                    eventHandler(self, ...);"
                "                end;"
                "            end;"
                "        end;"
                ""
                "        self._eventHandlers[name][#self._eventHandlers[name] + 1] = value;" 
                "    end;"
                "end;"

                "function GTEngine.SceneNode:RemoveUnusedScriptVariables(prevScriptVariables)"
                "    for i,variableName in ipairs(prevScriptVariables) do"
                "        if table.indexof(self._scriptVariables, variableName) == nil then"
                "            self[variableName] = nil;"
                "        end;"
                "    end;"
                "end;"



                "function GTEngine.SceneNode:GetID()"
                "    return GTEngine.System.SceneNode.GetID(self._internalPtr);"
                "end;"


                "function GTEngine.SceneNode:GetName()"
                "    return GTEngine.System.SceneNode.GetName(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetName(name)"
                "    GTEngine.System.SceneNode.SetName(self._internalPtr, name);"
                "end;"


                "function GTEngine.SceneNode:AttachChild(childNode)"
                "    GTEngine.System.SceneNode.AttachChild(self._internalPtr, childNode._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:Orphan()"
                "    GTEngine.System.SceneNode.Orphan(self._internalPtr);"
                "end;"


                "function GTEngine.SceneNode:AddComponent(componentID)"
                "    return GTEngine.System.SceneNode.AddComponent(self._internalPtr, componentID);"
                "end;"

                "function GTEngine.SceneNode:RemoveComponent(componentID)"
                "    return GTEngine.System.SceneNode.RemoveComponent(self._internalPtr, componentID);"
                "end;"

                "function GTEngine.SceneNode:GetComponent(componentID)"
                "    return GTEngine.System.SceneNode.GetComponent(self._internalPtr, componentID);"
                "end;"

                "function GTEngine.SceneNode:GetAttachedComponentIDs()"
                "    return GTEngine.System.SceneNode.GetAttachedComponentIDs(self._internalPtr);"
                "end;"




                "function GTEngine.SceneNode:GetPosition()"
                "    return GTEngine.System.SceneNode.GetPosition(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:GetWorldPosition()"
                "    return GTEngine.System.SceneNode.GetWorldPosition(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetPosition(x, y, z)"
                "    return GTEngine.System.SceneNode.SetPosition(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:SetWorldPosition(x, y, z)"
                "    return GTEngine.System.SceneNode.SetWorldPosition(self._internalPtr, x, y, z);"
                "end;"


                "function GTEngine.SceneNode:GetOrientation()"
                "    return GTEngine.System.SceneNode.GetOrientation(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:GetWorldOrientation()"
                "    return GTEngine.System.SceneNode.GetWorldOrientation(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetOrientation(x, y, z, w)"
                "    return GTEngine.System.SceneNode.SetOrientation(self._internalPtr, x, y, z, w);"
                "end;"

                "function GTEngine.SceneNode:SetWorldOrientation(x, y, z, w)"
                "    return GTEngine.System.SceneNode.SetWorldOrientation(self._internalPtr, x, y, z, w);"
                "end;"


                "function GTEngine.SceneNode:GetEulerRotation()"
                "    return GTEngine.System.SceneNode.GetEulerRotation(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:GetWorldEulerRotation()"
                "    return GTEngine.System.SceneNode.GetWorldEulerRotation(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetEulerRotation(x, y, z)"
                "    return GTEngine.System.SceneNode.SetEulerRotation(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:SetWorldEulerRotation(x, y, z)"
                "    return GTEngine.System.SceneNode.SetWorldEulerRotation(self._internalPtr, x, y, z);"
                "end;"


                "function GTEngine.SceneNode:GetScale()"
                "    return GTEngine.System.SceneNode.GetScale(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:GetWorldScale()"
                "    return GTEngine.System.SceneNode.GetWorldScale(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetScale(x, y, z)"
                "    return GTEngine.System.SceneNode.SetScale(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:SetWorldScale(x, y, z)"
                "    return GTEngine.System.SceneNode.SetWorldScale(self._internalPtr, x, y, z);"
                "end;"


                "function GTEngine.SceneNode:Translate(x, y, z)"
                "    return GTEngine.System.SceneNode.Translate(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:Rotate(angle, x, y, z)"
                "    return GTEngine.System.SceneNode.Rotate(self._internalPtr, angle, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:Scale(x, y, z)"
                "    return GTEngine.System.SceneNode.Scale(self._internalPtr, x, y, z);"
                "end;"


                "function GTEngine.SceneNode:TranslateX(x)"
                "    return GTEngine.System.SceneNode.Translate(self._internalPtr, x, 0.0, 0.0);"
                "end;"

                "function GTEngine.SceneNode:TranslateY(y)"
                "    return GTEngine.System.SceneNode.Translate(self._internalPtr, 0.0, y, 0.0);"
                "end;"

                "function GTEngine.SceneNode:TranslateZ(z)"
                "    return GTEngine.System.SceneNode.Translate(self._internalPtr, 0.0, 0.0, z);"
                "end;"


                "function GTEngine.SceneNode.RotateX(angle)"
                "    return GTEngine.System.SceneNode.Rotate(self._internalPtr, angle, 1.0, 0.0, 0.0);"
                "end;"

                "function GTEngine.SceneNode.RotateY(angle)"
                "    return GTEngine.System.SceneNode.Rotate(self._internalPtr, angle, 0.0, 1.0, 0.0);"
                "end;"

                "function GTEngine.SceneNode.RotateZ(angle)"
                "    return GTEngine.System.SceneNode.Rotate(self._internalPtr, angle, 0.0, 0.0, 1.0);"
                "end;"


                "function GTEngine.SceneNode:ScaleX(x)"
                "    return GTEngine.System.SceneNode.Scale(self._internalPtr, x, 1.0, 1.0);"
                "end;"

                "function GTEngine.SceneNode:ScaleY(y)"
                "    return GTEngine.System.SceneNode.Scale(self._internalPtr, 1.0, y, 1.0);"
                "end;"

                "function GTEngine.SceneNode:ScaleZ(z)"
                "    return GTEngine.System.SceneNode.Scale(self._internalPtr, 1.0, 1.0, z);"
                "end;"


                "function GTEngine.SceneNode:InterpolatePosition(dest, a)"
                "    return GTEngine.System.SceneNode.InterpolatePosition(self._internalPtr, dest, a);"
                "end;"

                "function GTEngine.SceneNode:InterpolateOrientation(dest, a)"
                "    return GTEngine.System.SceneNode.InterpolateOrientation(self._internalPtr, dest, a);"
                "end;"

                "function GTEngine.SceneNode:InterpolateScale(dest, a)"
                "    return GTEngine.System.SceneNode.InterpolateScale(self._internalPtr, dest, a);"
                "end;"

                "function GTEngine.SceneNode:Slerp(dest, a)"
                "    return GTEngine.System.SceneNode.Slerp(self._internalPtr, dest, a);"
                "end;"


                "function GTEngine.SceneNode:LookAt(target, up)"
                "    return GTEngine.System.SceneNode.LookAt(self._internalPtr, target, up);"
                "end;"

                "function GTEngine.SceneNode:LookAtSceneNode(target, up)"
                "    return GTEngine.System.SceneNode.LookAtSceneNode(self._internalPtr, target._internalPtr, up);"
                "end;"

                "function GTEngine.SceneNode:LookInDirection(target, up)"
                "    return GTEngine.System.SceneNode.LookInDirection(self._internalPtr, target, up);"
                "end;"


                "function GTEngine.SceneNode:Forward()"
                "    return GTEngine.System.SceneNode.Forward(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:Up()"
                "    return GTEngine.System.SceneNode.Up(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:Right()"
                "    return GTEngine.System.SceneNode.Right(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:WorldForward()"
                "    return GTEngine.System.SceneNode.WorldForward(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:WorldUp()"
                "    return GTEngine.System.SceneNode.WorldUp(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:WorldRight()"
                "    return GTEngine.System.SceneNode.WorldRight(self._internalPtr);"
                "end;"




                "function GTEngine.SceneNode:Show()"
                "    GTEngine.System.SceneNode.Show(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:Hide()"
                "    GTEngine.System.SceneNode.Hide(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:IsVisible()"
                "    return GTEngine.System.SceneNode.IsVisible(self._internalPtr);"
                "end;"



                "function GTEngine.SceneNode:GetScenePtr()"
                "    return GTEngine.System.SceneNode.GetScenePtr(self._internalPtr);"
                "end;"
            );

            // A helper for checking if a string is the name of an event handler.
            successful = successful && script.Execute
            (
                "function GTEngine.IsSceneNodeEventHandler(name)"
                "    return table.indexof(GTEngine.SceneNodeEventHandlers, name) ~= nil;"
                "end;"
            );


            if (successful)
            {
                script.GetGlobal("GTEngine");
                assert(script.IsTable(-1));
                {
                    script.Push("SceneNodeEventHandlers");
                    script.PushNewTable();
                    {
                        script.SetTableValue(-1, 1,  "OnUpdate");
                        script.SetTableValue(-1, 2,  "OnStartup");
                        script.SetTableValue(-1, 3,  "OnShutdown");
                        script.SetTableValue(-1, 4,  "OnShow");
                        script.SetTableValue(-1, 5,  "OnHide");
                        script.SetTableValue(-1, 6,  "OnObjectEnter");
                        script.SetTableValue(-1, 7,  "OnEnterObject");
                        script.SetTableValue(-1, 8,  "OnObjectLeave");
                        script.SetTableValue(-1, 9,  "OnLeaveObject");
                        script.SetTableValue(-1, 10, "OnMouseMove");
                        script.SetTableValue(-1, 11, "OnMouseWheel");
                        script.SetTableValue(-1, 12, "OnMouseButtonDown");
                        script.SetTableValue(-1, 13, "OnMouseButtonUp");
                        script.SetTableValue(-1, 14, "OnMouseButtonDoubleClick");
                        script.SetTableValue(-1, 15, "OnKeyPressed");
                        script.SetTableValue(-1, 16, "OnKeyReleased");
                    }
                    script.SetTableValue(-3);



                    script.Push("System");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.Push("SceneNode");
                        script.PushNewTable();
                        {
                            script.SetTableFunction(-1, "Create",                  SceneNodeFFI::Create);
                            script.SetTableFunction(-1, "Delete",                  SceneNodeFFI::Delete);

                            // Identification.
                            script.SetTableFunction(-1, "GetID",                   SceneNodeFFI::GetID);
                            script.SetTableFunction(-1, "GetName",                 SceneNodeFFI::GetName);
                            script.SetTableFunction(-1, "SetName",                 SceneNodeFFI::SetName);

                            // Hierarchy.
                            script.SetTableFunction(-1, "GetParentPtr",            SceneNodeFFI::GetParentPtr);
                            script.SetTableFunction(-1, "AttachChild",             SceneNodeFFI::AttachChild);
                            script.SetTableFunction(-1, "Orphan",                  SceneNodeFFI::Orphan);
                            script.SetTableFunction(-1, "IsAncestor",              SceneNodeFFI::IsAncestor);
                            script.SetTableFunction(-1, "IsDescendant",            SceneNodeFFI::IsDescendant);
                            script.SetTableFunction(-1, "IsRelative",              SceneNodeFFI::IsRelative);
                            script.SetTableFunction(-1, "GetChildrenIDs",          SceneNodeFFI::GetChildrenIDs);

                            // Components.
                            script.SetTableFunction(-1, "AddComponent",            SceneNodeFFI::AddComponent);
                            script.SetTableFunction(-1, "RemoveComponent",         SceneNodeFFI::RemoveComponent);
                            script.SetTableFunction(-1, "GetComponent",            SceneNodeFFI::GetComponent);
                            script.SetTableFunction(-1, "GetAttachedComponentIDs", SceneNodeFFI::GetAttachedComponentIDs);

                            // Transformations.
                            script.SetTableFunction(-1, "GetPosition",             SceneNodeFFI::GetPosition);
                            script.SetTableFunction(-1, "GetWorldPosition",        SceneNodeFFI::GetWorldPosition);
                            script.SetTableFunction(-1, "SetPosition",             SceneNodeFFI::SetPosition);
                            script.SetTableFunction(-1, "SetWorldPosition",        SceneNodeFFI::SetWorldPosition);
                            script.SetTableFunction(-1, "GetOrientation",          SceneNodeFFI::GetOrientation);
                            script.SetTableFunction(-1, "GetWorldOrientation",     SceneNodeFFI::GetWorldOrientation);
                            script.SetTableFunction(-1, "SetOrientation",          SceneNodeFFI::SetOrientation);
                            script.SetTableFunction(-1, "SetWorldOrientation",     SceneNodeFFI::SetWorldOrientation);
                            script.SetTableFunction(-1, "GetEulerRotation",        SceneNodeFFI::GetEulerRotation);
                            script.SetTableFunction(-1, "GetWorldEulerRotation",   SceneNodeFFI::GetWorldEulerRotation);
                            script.SetTableFunction(-1, "SetEulerRotation",        SceneNodeFFI::SetEulerRotation);
                            script.SetTableFunction(-1, "SetWorldEulerRotation",   SceneNodeFFI::SetWorldEulerRotation);
                            script.SetTableFunction(-1, "GetScale",                SceneNodeFFI::GetScale);
                            script.SetTableFunction(-1, "GetWorldScale",           SceneNodeFFI::GetWorldScale);
                            script.SetTableFunction(-1, "SetScale",                SceneNodeFFI::SetScale);
                            script.SetTableFunction(-1, "SetWorldScale",           SceneNodeFFI::SetWorldScale);
                            script.SetTableFunction(-1, "Translate",               SceneNodeFFI::Translate);
                            script.SetTableFunction(-1, "Rotate",                  SceneNodeFFI::Rotate);
                            script.SetTableFunction(-1, "Scale",                   SceneNodeFFI::Scale);
                            script.SetTableFunction(-1, "InterpolatePosition",     SceneNodeFFI::InterpolatePosition);
                            script.SetTableFunction(-1, "InterpolateOrientation",  SceneNodeFFI::InterpolateOrientation);
                            script.SetTableFunction(-1, "InterpolateScale",        SceneNodeFFI::InterpolateScale);
                            script.SetTableFunction(-1, "Slerp",                   SceneNodeFFI::Slerp);
                            script.SetTableFunction(-1, "LookAt",                  SceneNodeFFI::LookAt);
                            script.SetTableFunction(-1, "LookAtSceneNode",         SceneNodeFFI::LookAtSceneNode);
                            script.SetTableFunction(-1, "LookInDirection",         SceneNodeFFI::LookInDirection);
                            script.SetTableFunction(-1, "Forward",                 SceneNodeFFI::Forward);
                            script.SetTableFunction(-1, "Up",                      SceneNodeFFI::Up);
                            script.SetTableFunction(-1, "Right",                   SceneNodeFFI::Right);
                            script.SetTableFunction(-1, "WorldForward",            SceneNodeFFI::WorldForward);
                            script.SetTableFunction(-1, "WorldUp",                 SceneNodeFFI::WorldUp);
                            script.SetTableFunction(-1, "WorldRight",              SceneNodeFFI::WorldRight);

                            // Visibility.
                            script.SetTableFunction(-1, "Show",                    SceneNodeFFI::Show);
                            script.SetTableFunction(-1, "Hide",                    SceneNodeFFI::Hide);
                            script.SetTableFunction(-1, "IsVisible",               SceneNodeFFI::IsVisible);

                            // Misc.
                            script.SetTableFunction(-1, "GetScenePtr",             SceneNodeFFI::GetScenePtr);
                        }
                        script.SetTableValue(-3);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }


            return successful;
        }


        void InstantiateSceneNode(GTCore::Script &script, SceneNode &sceneNode)
        {
            // We need to find the scene in GTEngine.RegisteredScenes which will be keyed by the scene. We then want to call InstantiateSceneNode() on it.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(sceneNode.GetScene());
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))                         // <-- Don't want to use an assert here. It's a valid case for this to not exist. In this case, we just ignore.
                    {
                        script.Push("InstantiateSceneNode");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);                               // <-- 'self'
                            script.Push(static_cast<int>(sceneNode.GetID()));   // <-- 'sceneNodeID'
                            script.Push(&sceneNode);                            // <-- 'sceneNodePtr'
                            script.Call(3, 0);
                        }
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void UninstantiateSceneNode(GTCore::Script &script, SceneNode &sceneNode)
        {
            // We need to find the scene in GTEngine.RegisteredScenes which will be keyed by the scene. We then want to call InstantiateSceneNode() on it.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(sceneNode.GetScene());
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))                         // <-- Don't want to use an assert here. It's a valid case for this to not exist. In this case, we just ignore.
                    {
                        script.Push("UninstantiateSceneNode");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);                               // <-- 'self'
                            script.Push(static_cast<int>(sceneNode.GetID()));   // <-- 'sceneNodeID'
                            script.Call(2, 0);
                        }
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void UpdatePublicVariables(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("UpdatePublicVariables");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void RegisterScriptComponent(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("RegisterScriptComponent");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PushSceneNode(GTCore::Script &script, SceneNode &sceneNode)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(sceneNode.GetScene());
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))                         // <-- Don't want to use an assert here. It's a valid case for this to not exist. In this case, we just ignore.
                    {
                        script.Push("GetSceneNodeByID");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);                               // <-- 'self'
                            script.Push(static_cast<int>(sceneNode.GetID()));   // <-- 'sceneNodeID'
                            script.Call(2, 1);

                            script.InsertIntoStack(-4);
                        }
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }
            script.Pop(1);

            assert(script.IsTable(-1));
        }

        void PostSceneNodeEvent_OnUpdate(GTCore::Script &script, SceneNode &sceneNode, double deltaTimeInSeconds)
        {
            // First we push the scene node.
            Scripting::PushSceneNode(script, sceneNode);

            // Now we just call OnUpdate on it.
            assert(script.IsTable(-1));
            {
                script.Push("OnUpdate");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Push(deltaTimeInSeconds);        // <-- 'deltaTimeInSeconds'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnStartup(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnStartup");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnShutdown(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnShutdown");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnShow(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnShow");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnHide(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnHide");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnObjectEnter(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnObjectEnter");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                               // <-- 'self'
                    Scripting::PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnEnterObject(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnEnterObject");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                               // <-- 'self'
                    Scripting::PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnObjectLeave(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnObjectLeave");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                               // <-- 'self'
                    Scripting::PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnLeaveObject(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnLeaveObject");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                               // <-- 'self'
                    Scripting::PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }


        namespace SceneNodeFFI
        {
            int Create(GTCore::Script &script)
            {
                script.Push(new SceneNode());
                return 1;
            }

            int Delete(GTCore::Script &script)
            {
                delete reinterpret_cast<SceneNode*>(script.ToPointer(1));
                return 0;
            }


            int GetID(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    script.Push(static_cast<int>(sceneNode->GetID()));
                }
                else
                {
                    script.Push(0);
                }

                return 1;
            }


            int GetName(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    script.Push(sceneNode->GetName());
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }

            int SetName(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->SetName(script.ToString(2));
                }

                return 0;
            }


            int GetParentPtr(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto parent = sceneNode->GetParent();
                    if (parent != nullptr)
                    {
                        script.Push(sceneNode->GetParent());
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

                return 1;
            }

            int AttachChild(GTCore::Script &script)
            {
                auto parentNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                auto childNode  = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (parentNode != nullptr && childNode != nullptr)
                {
                    parentNode->AttachChild(*childNode, script.ToBoolean(3));
                }

                return 0;
            }

            int Orphan(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->DetachFromParent(script.ToBoolean(2));
                }

                return 0;
            }

            int IsAncestor(GTCore::Script &script)
            {
                auto sceneNode         = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                auto ancestorSceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (sceneNode != nullptr && ancestorSceneNode != nullptr)
                {
                    script.Push(sceneNode->IsAncestor(*ancestorSceneNode));
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }

            int IsDescendant(GTCore::Script &script)
            {
                auto sceneNode           = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                auto descendantSceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (sceneNode != nullptr && descendantSceneNode != nullptr)
                {
                    script.Push(sceneNode->IsDescendant(*descendantSceneNode));
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }

            int IsRelative(GTCore::Script &script)
            {
                auto sceneNode         = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                auto relativeSceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (sceneNode != nullptr && relativeSceneNode != nullptr)
                {
                    script.Push(sceneNode->IsRelated(*relativeSceneNode));
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int GetChildrenIDs(GTCore::Script &script)
            {
                script.PushNewTable();

                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    int index = 0;

                    for (auto childNode = sceneNode->GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
                    {
                        script.SetTableValue(-1, ++index, static_cast<int>(childNode->GetID()));
                    }
                }

                return 1;
            }


            int AddComponent(GTCore::Script &script)
            {
                auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                auto componentName = script.ToString(2);

                if (GTCore::Strings::Equal(componentName, CameraComponent::Name))
                {
                    PushComponent(script, "CameraComponent", sceneNode->AddComponent<CameraComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
                {
                    PushComponent(script, "ModelComponent", sceneNode->AddComponent<ModelComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
                {
                    PushComponent(script, "PointLightComponent", sceneNode->AddComponent<PointLightComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, SpotLightComponent::Name))
                {
                    PushComponent(script, "SpotLightComponent", sceneNode->AddComponent<SpotLightComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, DirectionalLightComponent::Name))
                {
                    PushComponent(script, "DirectionalLightComponent", sceneNode->AddComponent<DirectionalLightComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, AmbientLightComponent::Name))
                {
                    PushComponent(script, "AmbientLightComponent", sceneNode->AddComponent<AmbientLightComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, DynamicsComponent::Name))
                {
                    PushComponent(script, "DynamicsComponent", sceneNode->AddComponent<DynamicsComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, ProximityComponent::Name))
                {
                    PushComponent(script, "ProximityComponent", sceneNode->AddComponent<ProximityComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, ScriptComponent::Name))
                {
                    PushComponent(script, "ScriptComponent", sceneNode->AddComponent<ScriptComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, ParticleSystemComponent::Name))
                {
                    PushComponent(script, "ParticleSystemComponent", sceneNode->AddComponent<ParticleSystemComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
                {
                    PushComponent(script, "EditorMetadataComponent", sceneNode->AddComponent<EditorMetadataComponent>());
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }

            int RemoveComponent(GTCore::Script &script)
            {
                reinterpret_cast<SceneNode*>(script.ToPointer(1))->RemoveComponentByName(script.ToString(2));
                return 0;
            }

            int GetComponent(GTCore::Script &script)
            {
                auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                auto componentName = script.ToString(2);

                if (GTCore::Strings::Equal(componentName, CameraComponent::Name))
                {
                    PushComponent(script, "CameraComponent", sceneNode->GetComponent<CameraComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
                {
                    PushComponent(script, "ModelComponent", sceneNode->GetComponent<ModelComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
                {
                    PushComponent(script, "PointLightComponent", sceneNode->GetComponent<PointLightComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, SpotLightComponent::Name))
                {
                    PushComponent(script, "SpotLightComponent", sceneNode->GetComponent<SpotLightComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, DirectionalLightComponent::Name))
                {
                    PushComponent(script, "DirectionalLightComponent", sceneNode->GetComponent<DirectionalLightComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, AmbientLightComponent::Name))
                {
                    PushComponent(script, "AmbientLightComponent", sceneNode->GetComponent<AmbientLightComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, DynamicsComponent::Name))
                {
                    PushComponent(script, "DynamicsComponent", sceneNode->GetComponent<DynamicsComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, ProximityComponent::Name))
                {
                    PushComponent(script, "ProximityComponent", sceneNode->GetComponent<ProximityComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, ScriptComponent::Name))
                {
                    PushComponent(script, "ScriptComponent", sceneNode->GetComponent<ScriptComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, ParticleSystemComponent::Name))
                {
                    PushComponent(script, "ParticleSystemComponent", sceneNode->GetComponent<ParticleSystemComponent>());
                }
                else if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
                {
                    PushComponent(script, "EditorMetadataComponent", sceneNode->GetComponent<EditorMetadataComponent>());
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }



            int GetPosition(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetPosition());
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, 0.0f, 0.0f);
                }

                return 1;
            }

            int GetWorldPosition(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetWorldPosition());
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, 0.0f, 0.0f);
                }

                return 1;
            }


            int SetPosition(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    if (script.IsTable(2))
                    {
                        sceneNode->SetPosition(Scripting::ToVector3(script, 2));
                    }
                    else
                    {
                        float x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        float y = script.IsNumber(3) ? script.ToFloat(3) : x;
                        float z = script.IsNumber(4) ? script.ToFloat(4) : x;

                        sceneNode->SetPosition(x, y, z);
                    }
                }

                return 0;
            }

            int SetWorldPosition(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    if (script.IsTable(2))
                    {
                        sceneNode->SetWorldPosition(Scripting::ToVector3(script, 2));
                    }
                    else
                    {
                        float x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        float y = script.IsNumber(3) ? script.ToFloat(3) : x;
                        float z = script.IsNumber(4) ? script.ToFloat(4) : x;

                        sceneNode->SetWorldPosition(x, y, z);
                    }
                }

                return 0;
            }


            int GetOrientation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewQuaternion(script, sceneNode->GetOrientation());
                }
                else
                {
                    Scripting::PushNewQuaternion(script, glm::quat());
                }

                return 1;
            }

            int GetWorldOrientation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewQuaternion(script, sceneNode->GetWorldOrientation());
                }
                else
                {
                    Scripting::PushNewQuaternion(script, glm::quat());
                }

                return 1;
            }

            int SetOrientation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    glm::quat orientation;

                    if (script.IsTable(2))
                    {
                        orientation = Scripting::ToQuaternion(script, 2);
                    }
                    else
                    {
                        if (script.IsNumber(2) && script.IsNumber(3) && script.IsNumber(4) && script.IsNumber(5))
                        {
                            orientation.x = script.ToFloat(2);
                            orientation.y = script.ToFloat(3);
                            orientation.z = script.ToFloat(4);
                            orientation.w = script.ToFloat(5);
                        }
                    }

                    sceneNode->SetOrientation(orientation);
                }

                return 0;
            }

            int SetWorldOrientation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    glm::quat orientation;

                    if (script.IsTable(2))
                    {
                        orientation = Scripting::ToQuaternion(script, 2);
                    }
                    else
                    {
                        if (script.IsNumber(2) && script.IsNumber(3) && script.IsNumber(4) && script.IsNumber(5))
                        {
                            orientation.x = script.ToFloat(2);
                            orientation.y = script.ToFloat(3);
                            orientation.z = script.ToFloat(4);
                            orientation.w = script.ToFloat(5);
                        }
                    }

                    sceneNode->SetWorldOrientation(orientation);
                }

                return 0;
            }


            int GetEulerRotation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto &orientation = sceneNode->GetOrientation();
                    Scripting::PushNewVector3(script, glm::pitch(orientation), glm::yaw(orientation), glm::roll(orientation));
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, 0.0f, 0.0f);
                }

                return 1;
            }

            int GetWorldEulerRotation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto orientation = sceneNode->GetWorldOrientation();
                    Scripting::PushNewVector3(script, glm::pitch(orientation), glm::yaw(orientation), glm::roll(orientation));
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, 0.0f, 0.0f);
                }

                return 1;
            }


            int SetEulerRotation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    float pitch;
                    float yaw;
                    float roll;

                    if (script.IsTable(2))
                    {
                        glm::vec3 rotation = Scripting::ToVector3(script, 2);

                        pitch = rotation.x;
                        yaw   = rotation.y;
                        roll  = rotation.z;
                    }
                    else
                    {
                        pitch = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        yaw   = script.IsNumber(3) ? script.ToFloat(3) : pitch;
                        roll  = script.IsNumber(4) ? script.ToFloat(4) : pitch;
                    }

                    pitch = glm::radians(pitch);
                    yaw   = glm::radians(yaw);
                    roll  = glm::radians(roll);

                    sceneNode->SetOrientation(glm::quat(glm::vec3(pitch, yaw, roll)));
                }

                return 0;
            }

            int SetWorldEulerRotation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    float pitch;
                    float yaw;
                    float roll;

                    if (script.IsTable(2))
                    {
                        glm::vec3 rotation = Scripting::ToVector3(script, 2);

                        pitch = rotation.x;
                        yaw   = rotation.y;
                        roll  = rotation.z;
                    }
                    else
                    {
                        pitch = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        yaw   = script.IsNumber(3) ? script.ToFloat(3) : pitch;
                        roll  = script.IsNumber(4) ? script.ToFloat(4) : pitch;
                    }

                    pitch = glm::radians(pitch);
                    yaw   = glm::radians(yaw);
                    roll  = glm::radians(roll);

                    sceneNode->SetWorldOrientation(glm::quat(glm::vec3(pitch, yaw, roll)));
                }

                return 0;
            }



            int GetScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetScale());
                }
                else
                {
                    Scripting::PushNewVector3(script, 1.0f, 1.0f, 1.0f);
                }

                return 1;
            }

            int GetWorldScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetWorldScale());
                }
                else
                {
                    Scripting::PushNewVector3(script, 1.0f, 1.0f, 1.0f);
                }

                return 1;
            }


            int SetScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    if (script.IsTable(2))
                    {
                        sceneNode->SetScale(Scripting::ToVector3(script, 2));
                    }
                    else
                    {
                        float x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        float y = script.IsNumber(3) ? script.ToFloat(3) : x;
                        float z = script.IsNumber(4) ? script.ToFloat(4) : x;

                        sceneNode->SetScale(x, y, z);
                    }
                }

                return 0;
            }

            int SetWorldScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    if (script.IsTable(2))
                    {
                        sceneNode->SetWorldScale(Scripting::ToVector3(script, 2));
                    }
                    else
                    {
                        float x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        float y = script.IsNumber(3) ? script.ToFloat(3) : x;
                        float z = script.IsNumber(4) ? script.ToFloat(4) : x;

                        sceneNode->SetWorldScale(x, y, z);
                    }
                }

                return 0;
            }


            int Translate(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    glm::vec3 translation;

                    if (script.IsTable(2))
                    {
                        translation = Scripting::ToVector3(script, 2);
                    }
                    else
                    {
                        translation.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        translation.y = script.IsNumber(3) ? script.ToFloat(3) : translation.x;
                        translation.z = script.IsNumber(4) ? script.ToFloat(4) : translation.x;
                    }

                    sceneNode->Translate(translation);
                }

                return 0;
            }

            int Rotate(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    float     angle = script.ToFloat(2);
                    glm::vec3 axis;

                    if (script.IsTable(3))
                    {
                        axis = Scripting::ToVector3(script, 3);
                    }
                    else
                    {
                        axis.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                        axis.y = script.IsNumber(4) ? script.ToFloat(4) : axis.x;
                        axis.z = script.IsNumber(5) ? script.ToFloat(5) : axis.x;

                        if (axis.x == 0.0f && axis.y == 0.0f && axis.z == 0.0f)
                        {
                            axis.z = 1.0f;
                        }
                    }

                    sceneNode->Rotate(angle, axis);
                }

                return 0;
            }

            int Scale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    glm::vec3 scale;

                    if (script.IsTable(2))
                    {
                        scale = Scripting::ToVector3(script, 2);
                    }
                    else
                    {
                        scale.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        scale.y = script.IsNumber(3) ? script.ToFloat(3) : scale.x;
                        scale.z = script.IsNumber(4) ? script.ToFloat(4) : scale.x;
                    }

                    sceneNode->Scale(scale);
                }

                return 0;
            }


            int TranslateWorld(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    glm::vec3 translation;

                    if (script.IsTable(2))
                    {
                        translation = Scripting::ToVector3(script, 2);
                    }
                    else
                    {
                        translation.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                        translation.y = script.IsNumber(3) ? script.ToFloat(3) : translation.x;
                        translation.z = script.IsNumber(4) ? script.ToFloat(4) : translation.x;
                    }

                    sceneNode->SetWorldPosition(sceneNode->GetWorldPosition() + translation);
                }

                return 0;
            }

            int RotateWorld(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    float     angle = script.ToFloat(2);
                    glm::vec3 axis;

                    if (script.IsTable(3))
                    {
                        axis = Scripting::ToVector3(script, 3);
                    }
                    else
                    {
                        axis.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                        axis.y = script.IsNumber(4) ? script.ToFloat(4) : axis.x;
                        axis.z = script.IsNumber(5) ? script.ToFloat(5) : axis.x;

                        if (axis.x == 0.0f && axis.y == 0.0f && axis.z == 0.0f)
                        {
                            axis.z = 1.0f;
                        }
                    }

                    sceneNode->RotateAroundWorldAxis(angle, axis);
                }

                return 0;
            }

            int RotateWorldAroundPivot(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    float     angle = script.ToFloat(2);
                    glm::vec3 axis;
                    glm::vec3 pivot;

                    if (script.IsTable(3))
                    {
                        axis = Scripting::ToVector3(script, 3);
                        
                        if (script.IsTable(4))
                        {
                            pivot = Scripting::ToVector3(script, 4);
                        }
                        else
                        {
                            pivot.x = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                            pivot.y = script.IsNumber(5) ? script.ToFloat(5) : pivot.x;
                            pivot.z = script.IsNumber(6) ? script.ToFloat(6) : pivot.x;
                        }
                    }
                    else
                    {
                        axis.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                        axis.y = script.IsNumber(4) ? script.ToFloat(4) : axis.x;
                        axis.z = script.IsNumber(5) ? script.ToFloat(5) : axis.x;

                        if (axis.x == 0.0f && axis.y == 0.0f && axis.z == 0.0f)
                        {
                            axis.z = 1.0f;
                        }


                        if (script.IsTable(6))
                        {
                            pivot = Scripting::ToVector3(script, 6);
                        }
                        else
                        {
                            pivot.x = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;
                            pivot.y = script.IsNumber(7) ? script.ToFloat(7) : pivot.x;
                            pivot.z = script.IsNumber(8) ? script.ToFloat(8) : pivot.x;
                        }
                    }

                    sceneNode->RotateAtPivotAroundWorldAxis(angle, axis, pivot);
                }

                return 0;
            }


            int InterpolatePosition(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->InterpolatePosition(Scripting::ToVector3(script, 2), script.ToFloat(3));
                }

                return 0;
            }

            int InterpolateOrientation(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->InterpolateOrientation(Scripting::ToQuaternion(script, 2), script.ToFloat(3));
                }

                return 0;
            }

            int InterpolateScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->InterpolateScale(Scripting::ToVector3(script, 2), script.ToFloat(3));
                }

                return 0;
            }

            int Slerp(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->Slerp(Scripting::ToQuaternion(script, 2), script.ToFloat(3));
                }

                return 0;
            }


            int LookAt(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    glm::vec3 target = Scripting::ToVector3(script, 2);
                    glm::vec3 up     = (script.IsTable(3)) ? Scripting::ToVector3(script, 3) : glm::vec3(0.0f, 1.0f, 0.0f);

                    sceneNode->LookAt(target, up);
                }

                return 0;
            }

            int LookAtSceneNode(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto targetSceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                    if (targetSceneNode != nullptr)
                    {
                        glm::vec3 up = (script.IsTable(3)) ? Scripting::ToVector3(script, 3) : glm::vec3(0.0f, 1.0f, 0.0f);

                        sceneNode->LookAt(*targetSceneNode, up);
                    }
                }

                return 0;
            }

            int LookInDirection(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    glm::vec3 target = Scripting::ToVector3(script, 2);
                    glm::vec3 up     = (script.IsTable(3)) ? Scripting::ToVector3(script, 3) : glm::vec3(0.0f, 1.0f, 0.0f);

                    sceneNode->LookInDirection(target, up);
                }

                return 0;
            }


            int Forward(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetForwardVector());
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, 0.0f, -1.0f);
                }

                return 1;
            }

            int Up(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetUpVector());
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, 1.0f, 0.0f);
                }

                return 1;
            }

            int Right(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetRightVector());
                }
                else
                {
                    Scripting::PushNewVector3(script, 1.0f, 0.0f, 0.0f);
                }

                return 1;
            }

            int WorldForward(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetWorldForwardVector());
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, 0.0f, -1.0f);
                }

                return 1;
            }

            int WorldUp(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetWorldUpVector());
                }
                else
                {
                    Scripting::PushNewVector3(script, 0.0f, 1.0f, 0.0f);
                }

                return 1;
            }

            int WorldRight(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneNode->GetWorldRightVector());
                }
                else
                {
                    Scripting::PushNewVector3(script, 1.0f, 0.0f, 0.0f);
                }

                return 1;
            }




            int GetAttachedComponentIDs(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    script.PushNewTable();


                    GTCore::Vector<GTCore::String> componentNames;
                    sceneNode->GetAttachedComponentNames(componentNames);

                    for (size_t i = 0; i < componentNames.count; ++i)
                    {
                        script.SetTableValue(-1, i + 1, componentNames[i].c_str());
                    }
                }

                return 1;
            }


            int Show(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->Show();
                }

                return 0;
            }

            int Hide(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->Hide();
                }

                return 0;
            }

            int IsVisible(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    script.Push(sceneNode->IsVisible());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }

            int GetScenePtr(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr && sceneNode->GetScene() != nullptr)
                {
                    script.Push(sceneNode->GetScene());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            void PushComponent(GTCore::Script &script, const char* componentClassName, Component* component)
            {
                if (component != nullptr)
                {
                    // GTEngine.<componentClassName>.Create(component);

                    script.GetGlobal("GTEngine");
                    assert(script.IsTable(-1));
                    {
                        script.Push(componentClassName);
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("Create");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.Push(component);
                                script.Call(1, 1);

                                // GTEngine and GTEngine.<componentClassName> need to be popped, so we need to insert the return value into the location that will eventually place
                                // it at -1 after popping those tables.
                                script.InsertIntoStack(-3);
                            }
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);
                }
                else
                {
                    script.PushNil();
                }
            }
        }
    }
}