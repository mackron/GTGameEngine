// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_SceneNode.hpp>

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


                "function GTEngine.SceneNode:GetRotationXYZ()"
                "    return GTEngine.System.SceneNode.GetRotationXYZ(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:GetWorldRotationXYZ()"
                "    return GTEngine.System.SceneNode.GetWorldRotationXYZ(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetRotationXYZ(x, y, z)"
                "    return GTEngine.System.SceneNode.SetRotationXYZ(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:SetWorldRotationXYZ(x, y, z)"
                "    return GTEngine.System.SceneNode.SetWorldRotationXYZ(self._internalPtr, x, y, z);"
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



                "function GTEngine.SceneNode:GetAttachedComponentIDs()"
                "    return GTEngine.System.SceneNode.GetAttachedComponentIDs(self._internalPtr);"
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

                            script.SetTableFunction(-1, "GetID",                   SceneNodeFFI::GetID);
                            script.SetTableFunction(-1, "GetName",                 SceneNodeFFI::GetName);
                            script.SetTableFunction(-1, "SetName",                 SceneNodeFFI::SetName);

                            script.SetTableFunction(-1, "GetParentPtr",            SceneNodeFFI::GetParentPtr);
                            script.SetTableFunction(-1, "AttachChild",             SceneNodeFFI::AttachChild);
                            script.SetTableFunction(-1, "Orphan",                  SceneNodeFFI::Orphan);
                            script.SetTableFunction(-1, "IsAncestor",              SceneNodeFFI::IsAncestor);
                            script.SetTableFunction(-1, "IsDescendant",            SceneNodeFFI::IsDescendant);
                            script.SetTableFunction(-1, "IsRelative",              SceneNodeFFI::IsRelative);
                            script.SetTableFunction(-1, "GetChildrenIDs",          SceneNodeFFI::GetChildrenIDs);

                            script.SetTableFunction(-1, "AddComponent",            SceneNodeFFI::AddComponent);
                            script.SetTableFunction(-1, "RemoveComponent",         SceneNodeFFI::RemoveComponent);
                            script.SetTableFunction(-1, "GetComponent",            SceneNodeFFI::GetComponent);

                            script.SetTableFunction(-1, "GetPosition",             SceneNodeFFI::GetPosition);
                            script.SetTableFunction(-1, "GetWorldPosition",        SceneNodeFFI::GetWorldPosition);
                            script.SetTableFunction(-1, "SetPosition",             SceneNodeFFI::SetPosition);
                            script.SetTableFunction(-1, "SetWorldPosition",        SceneNodeFFI::SetWorldPosition);
                            script.SetTableFunction(-1, "GetRotationXYZ",          SceneNodeFFI::GetRotationXYZ);
                            script.SetTableFunction(-1, "GetWorldRotationXYZ",     SceneNodeFFI::GetWorldRotationXYZ);
                            script.SetTableFunction(-1, "SetRotationXYZ",          SceneNodeFFI::SetRotationXYZ);
                            script.SetTableFunction(-1, "SetWorldRotationXYZ",     SceneNodeFFI::SetWorldRotationXYZ);
                            script.SetTableFunction(-1, "GetScale",                SceneNodeFFI::GetScale);
                            script.SetTableFunction(-1, "GetWorldScale",           SceneNodeFFI::GetWorldScale);
                            script.SetTableFunction(-1, "SetScale",                SceneNodeFFI::SetScale);
                            script.SetTableFunction(-1, "SetWorldScale",           SceneNodeFFI::SetWorldScale);

                            script.SetTableFunction(-1, "GetAttachedComponentIDs", SceneNodeFFI::GetAttachedComponentIDs);

                            script.SetTableFunction(-1, "Show",                    SceneNodeFFI::Show);
                            script.SetTableFunction(-1, "Hide",                    SceneNodeFFI::Hide);
                            script.SetTableFunction(-1, "IsVisible",               SceneNodeFFI::IsVisible);

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
                    auto &position = sceneNode->GetPosition();

                    script.Push(position.x);
                    script.Push(position.y);
                    script.Push(position.z);
                }
                else
                {
                    script.Push(0.0f);
                    script.Push(0.0f);
                    script.Push(0.0f);
                }

                return 3;
            }

            int GetWorldPosition(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto position = sceneNode->GetWorldPosition();

                    script.Push(position.x);
                    script.Push(position.y);
                    script.Push(position.z);
                }
                else
                {
                    script.Push(0.0f);
                    script.Push(0.0f);
                    script.Push(0.0f);
                }

                return 3;
            }


            int SetPosition(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->SetPosition(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                }

                return 0;
            }

            int SetWorldPosition(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->SetWorldPosition(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                }

                return 0;
            }



            int GetRotationXYZ(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto &orientation = sceneNode->GetOrientation();

                    script.Push(glm::pitch(orientation));
                    script.Push(glm::yaw(orientation));
                    script.Push(glm::roll(orientation));
                }
                else
                {
                    script.Push(0.0f);
                    script.Push(0.0f);
                    script.Push(0.0f);
                }

                return 3;
            }

            int GetWorldRotationXYZ(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto orientation = sceneNode->GetWorldOrientation();

                    script.Push(glm::pitch(orientation));
                    script.Push(glm::yaw(orientation));
                    script.Push(glm::roll(orientation));
                }
                else
                {
                    script.Push(0.0f);
                    script.Push(0.0f);
                    script.Push(0.0f);
                }

                return 3;
            }


            int SetRotationXYZ(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    // Order: pitch/yaw/roll
                    sceneNode->SetOrientation(glm::quat(glm::vec3(glm::radians(script.ToFloat(2)), glm::radians(script.ToFloat(3)), glm::radians(script.ToFloat(4)))));
                }

                return 0;
            }

            int SetWorldRotationXYZ(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    // Order: pitch/yaw/roll
                    sceneNode->SetWorldOrientation(glm::quat(glm::vec3(glm::radians(script.ToFloat(2)), glm::radians(script.ToFloat(3)), glm::radians(script.ToFloat(4)))));
                }

                return 0;
            }




            int GetScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto &scale = sceneNode->GetScale();

                    script.Push(scale.x);
                    script.Push(scale.y);
                    script.Push(scale.z);
                }
                else
                {
                    script.Push(1.0f);
                    script.Push(1.0f);
                    script.Push(1.0f);
                }

                return 3;
            }

            int GetWorldScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    auto scale = sceneNode->GetWorldScale();

                    script.Push(scale.x);
                    script.Push(scale.y);
                    script.Push(scale.z);
                }
                else
                {
                    script.Push(1.0f);
                    script.Push(1.0f);
                    script.Push(1.0f);
                }

                return 3;
            }


            int SetScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->SetScale(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                }

                return 0;
            }

            int SetWorldScale(GTCore::Script &script)
            {
                auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                if (sceneNode != nullptr)
                {
                    sceneNode->SetWorldScale(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                }

                return 0;
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