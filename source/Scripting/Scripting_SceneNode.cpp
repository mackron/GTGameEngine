// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Scripting/Scripting_SceneNode.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/Scripting.hpp>

namespace GT
{
    bool LoadSceneNodeLibrary(GT::Script &script)
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
            "        new._eventHandlers           = {};"
            "        new._scriptVariables         = {};"
            "        new._scriptFileRelativePaths = {};"
            "        new.Scene                    = nil;"
            ""
            "        new:RegisterComponents();"
            "    return new;"
            "end;"

            "function GTEngine.SceneNode:Delete()"
            "    if self._deleteInternalPtr then"
            "        GTEngine.System.SceneNode.Delete(self._internalPtr);"
            "    end;"
            "end;"


            "function GTEngine.SceneNode:RegisterComponents()"
            "    local componentIDs = self:GetAttachedComponentIDs();"
            "    for i,componentID in ipairs(componentIDs) do"
            "        self:RegisterComponent(componentID);"
            "    end;"
            "end;"

            "function GTEngine.SceneNode:RegisterComponent(componentID)"
            "    if componentID == GTEngine.Components.Script then"
            "        self:RegisterScriptComponent();"
            "    else"
            "        self[componentID] = self:GetComponent(componentID);"
            "    end;"
            "end;"

            "function GTEngine.SceneNode:UnregisterComponents()"
            "    local componentIDs = self:GetAttachedComponentIDs();"
            "    for i,componentID in ipairs(componentIDs) do"
            "        self:UnregisterComponent(componentID);"
            "    end;"
            "end;"

            "function GTEngine.SceneNode:UnregisterComponent(componentID)"
            "    self[componentID] = nil;"
            "end;"


            "function GTEngine.SceneNode:RegisterScriptComponent()"
            "    local prevEventHandlers           = self._eventHandlers;"              // <-- Only used for cleaning up event handlers.
            "    local prevScriptVariables         = self._scriptVariables;"            // <-- Only used for cleanly removing previous variables
            "    local prevScriptFileRelativePaths = self._scriptFileRelativePaths;"    // <-- Only used for cleanly removing previous script files
            "    self._eventHandlers           = {};"
            "    self._scriptVariables         = {};"
            "    self._scriptFileRelativePaths = {};"
            ""
            "    local scriptComponent = self:GetComponent(GTEngine.Components.Script);"
            "    if scriptComponent ~= nil then"
            "        self._scriptFileRelativePaths = scriptComponent:GetScriptRelativeFilePaths();"
            "        for i,scriptPath in ipairs(self._scriptFileRelativePaths) do"
            "            self:LinkToScript(GTEngine.ScriptDefinitions[scriptPath], scriptPath);"
            "            self:UpdatePublicVariables();"
            "        end;"
            "    end;"
            ""
            "    self:RemoveUnusedEventHandlers(prevEventHandlers);"
            "    self:RemoveUnusedScriptVariables(prevScriptVariables);"
            //"    self:RemoveUnusedScriptFiles(prevScriptFileRelativePaths);"
            ""
            "    if self.Scene ~= nil then"
            "        self.Scene:OnSceneNodeEventsChanged(self);"
            "    end;"
            "end;"

            "function GTEngine.SceneNode:LinkToScript(script, scriptRelativePath)"
            "    if script ~= nil then"
            "        for key,value in pairs(script) do"
            "            if not GTEngine.IsSceneNodeEventHandler(key) then"
            "                self[key] = value;"
            "                self._scriptVariables[#self._scriptVariables + 1] = key;"
            "            else"
            "                self:RegisterEventHandler(key, value, scriptRelativePath);"
            "            end;"
            "        end;"
            "    end;"
            "end;"

            /*"function GTEngine.SceneNode:UnlinkFromScript(script, scriptRelativePath)"
            "    if script ~= nil then"
            "        for key,value in pairs(script) do"
            "            if not GTEngine.IsSceneNodeEventHandler(key) then"
            "                self[key] = nil;"
            "                table.remove(self._scriptVariables, GT.table.indexof(self._scriptVariables, key));"
            "            else"
            "                self:UnregisterEventHandler(key, value, scriptRelativePath);"
            "            end;"
            "        end;"
            "    end;"
            "end;"*/

            "function GTEngine.SceneNode:UpdatePublicVariables(scriptComponent)"
            "    if scriptComponent == nil then scriptComponent = self:GetComponent(GTEngine.Components.Script) end;"
            "    if scriptComponent ~= nil then"
            "        local variables = scriptComponent:GetPublicVariableNamesAndValues();"
            "        for name,value in pairs(variables) do"
            "            self[name] = value;"
            "        end;"
            "    end;"
            "end;"

            "function GTEngine.SceneNode:RegisterEventHandler(name, value, scriptRelativePath)"
            "    if name ~= nil and value ~= nil then"
            "        if self._eventHandlers[name] == nil then"
            "            self._eventHandlers[name] = {};"
            ""
            "            self[name] = function(self, ...)"
            "                for i,eventHandler in ipairs(self._eventHandlers[name]) do"
            "                    eventHandler.method(self, ...);"
            "                end;"
            "            end;"
            "        end;"
            ""
            "        local eventHandler = {};"
            "        eventHandler.method             = value;"
            "        eventHandler.scriptRelativePath = scriptRelativePath;"
            ""
            "        self._eventHandlers[name][#self._eventHandlers[name] + 1] = eventHandler;"
            "    end;"
            "end;"


            "function GTEngine.SceneNode:RemoveUnusedEventHandlers(prevEventHandlers)"
            "    for key,eventHandler in pairs(prevEventHandlers) do"
            "        if self._eventHandlers[key] == nil then"
            "            self[key] = nil;"
            "        end;"
            "    end;"
            "end;"

            "function GTEngine.SceneNode:RemoveUnusedScriptVariables(prevScriptVariables)"
            "    for i,variableName in ipairs(prevScriptVariables) do"
            "        if GT.table.indexof(self._scriptVariables, variableName) == nil then"
            "            self[variableName] = nil;"
            "        end;"
            "    end;"
            "end;"

            /*"function GTEngine.SceneNode:RemoveUnusedScriptFiles(prevScriptFileRelativePaths)"
            "    local removedScriptFileRelativePaths = GT.table.idifference(prevScriptFileRelativePaths, self._scriptFileRelativePaths);"
            ""
            "    for key,eventHandlers in pairs(self._eventHandlers) do"
            "        local eventHandlersToRemove = {};"
            "        for i,eventHandler in ipairs(eventHandlers) do"
            "            print('Removing1... ' .. removedScriptFileRelativePaths[1] .. ' ' .. eventHandler.scriptRelativePath);"
            "            if GT.table.indexof(removedScriptFileRelativePaths, eventHandler.scriptRelativePath) ~= nil then"
            "                eventHandlersToRemove[#eventHandlersToRemove + 1] = eventHandler;"
            "            end;"
            "        end;"
            ""
            "        for i,eventHandlerToRemove in ipairs(eventHandlersToRemove) do"
            "            table.remove(eventHandlers, GT.table.indexof(eventHandlers, eventHandlerToRemove.eventHandler));"
            "        end;"
            "    end;"
            "end;"*/



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


            "function GTEngine.SceneNode:GetParent()"
            "    return self.Scene:GetSceneNodeByPtr(GTEngine.System.SceneNode.GetParentPtr(self._internalPtr));"
            "end;"

            "function GTEngine.SceneNode:GetChildByName(name)"
            "    return self.Scene:GetSceneNodeByID(self:GetChildIDByName(name));"
            "end;"

            "function GTEngine.SceneNode:GetChildIDByName(name)"
            "    return GTEngine.System.SceneNode.GetChildIDByName(self._internalPtr, name);"
            "end;"

            "function GTEngine.SceneNode:GetDescendantIDByName(name)"
            "    return GTEngine.System.SceneNode.GetDescendantIDByName(self._internalPtr, name);"
            "end;"



            "function GTEngine.SceneNode:DisablePositionInheritance()"
            "    GTEngine.System.SceneNode.DisablePositionInheritance(self._internalPtr);"
            "end;"

            "function GTEngine.SceneNode:EnablePositionInheritance()"
            "    GTEngine.System.SceneNode.EnablePositionInheritance(self._internalPtr);"
            "end;"

            "function GTEngine.SceneNode:IsPositionInheritanceEnabled()"
            "    return GTEngine.System.SceneNode.IsPositionInheritanceEnabled(self._internalPtr);"
            "end;"


            "function GTEngine.SceneNode:DisableOrientationInheritance()"
            "    GTEngine.System.SceneNode.DisableOrientationInheritance(self._internalPtr);"
            "end;"

            "function GTEngine.SceneNode:EnableOrientationInheritance()"
            "    GTEngine.System.SceneNode.EnableOrientationInheritance(self._internalPtr);"
            "end;"

            "function GTEngine.SceneNode:IsOrientationInheritanceEnabled()"
            "    return GTEngine.System.SceneNode.IsOrientationInheritanceEnabled(self._internalPtr);"
            "end;"


            "function GTEngine.SceneNode:DisableScaleInheritance()"
            "    GTEngine.System.SceneNode.DisableScaleInheritance(self._internalPtr);"
            "end;"

            "function GTEngine.SceneNode:EnableScaleInheritance()"
            "    GTEngine.System.SceneNode.EnableScaleInheritance(self._internalPtr);"
            "end;"

            "function GTEngine.SceneNode:IsScaleInheritanceEnabled()"
            "    return GTEngine.System.SceneNode.IsScaleInheritanceEnabled(self._internalPtr);"
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


            "function GTEngine.SceneNode:RotateX(angle)"
            "    return GTEngine.System.SceneNode.Rotate(self._internalPtr, angle, 1.0, 0.0, 0.0);"
            "end;"

            "function GTEngine.SceneNode:RotateY(angle)"
            "    return GTEngine.System.SceneNode.Rotate(self._internalPtr, angle, 0.0, 1.0, 0.0);"
            "end;"

            "function GTEngine.SceneNode:RotateZ(angle)"
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


            "function GTEngine.SceneNode:ClampPosition(posMin, posMax)"
            "    return GTEngine.System.SceneNode.ClampPosition(self._internalPtr, posMin, posMax);"
            "end;"

            "function GTEngine.SceneNode:ClampWorldPosition(posMin, posMax)"
            "    return GTEngine.System.SceneNode.ClampWorldPosition(self._internalPtr, posMin, posMax);"
            "end;"

            "function GTEngine.SceneNode:ClampScale(scaleMin, scaleMax)"
            "    return GTEngine.System.SceneNode.ClampScale(self._internalPtr, scaleMin, scaleMax);"
            "end;"

            "function GTEngine.SceneNode:ClampWorldScale(scaleMin, scaleMax)"
            "    return GTEngine.System.SceneNode.ClampWorldScale(self._internalPtr, scaleMin, scaleMax);"
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
            "    return GT.table.indexof(GTEngine.SceneNodeEventHandlers, name) ~= nil;"
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
                    script.SetTableValue(-1, 6,  "OnSceneNodeEnter");
                    script.SetTableValue(-1, 7,  "OnEnterSceneNode");
                    script.SetTableValue(-1, 8,  "OnSceneNodeLeave");
                    script.SetTableValue(-1, 9,  "OnLeaveSceneNode");
                    script.SetTableValue(-1, 10, "OnMouseMove");
                    script.SetTableValue(-1, 11, "OnMouseWheel");
                    script.SetTableValue(-1, 12, "OnMouseButtonDown");
                    script.SetTableValue(-1, 13, "OnMouseButtonUp");
                    script.SetTableValue(-1, 14, "OnMouseButtonDoubleClick");
                    script.SetTableValue(-1, 15, "OnKeyPressed");
                    script.SetTableValue(-1, 16, "OnKeyReleased");
                    script.SetTableValue(-1, 17, "OnSerialize");
                    script.SetTableValue(-1, 18, "OnDeserialize");
                    script.SetTableValue(-1, 19, "OnSerializeGlobalData");
                    script.SetTableValue(-1, 20, "OnDeserializeGlobalData");
                    script.SetTableValue(-1, 21, "OnGamePause");
                    script.SetTableValue(-1, 22, "OnGameResume");
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
                        script.SetTableFunction(-1, "GetChildIDByName",        SceneNodeFFI::GetChildIDByName);
                        script.SetTableFunction(-1, "GetDescendantIDByName",   SceneNodeFFI::GetDescendantIDByName);

                        // Components.
                        script.SetTableFunction(-1, "AddComponent",            SceneNodeFFI::AddComponent);
                        script.SetTableFunction(-1, "RemoveComponent",         SceneNodeFFI::RemoveComponent);
                        script.SetTableFunction(-1, "GetComponent",            SceneNodeFFI::GetComponent);
                        script.SetTableFunction(-1, "GetAttachedComponentIDs", SceneNodeFFI::GetAttachedComponentIDs);

                        // Transformation Inheritance.
                        script.SetTableFunction(-1, "DisablePositionInheritance",      SceneNodeFFI::DisablePositionInheritance);
                        script.SetTableFunction(-1, "EnablePositionInheritance",       SceneNodeFFI::EnablePositionInheritance);
                        script.SetTableFunction(-1, "IsPositionInheritanceEnabled",    SceneNodeFFI::IsPositionInheritanceEnabled);
                        script.SetTableFunction(-1, "DisableOrientationInheritance",   SceneNodeFFI::DisableOrientationInheritance);
                        script.SetTableFunction(-1, "EnableOrientationInheritance",    SceneNodeFFI::EnableOrientationInheritance);
                        script.SetTableFunction(-1, "IsOrientationInheritanceEnabled", SceneNodeFFI::IsOrientationInheritanceEnabled);
                        script.SetTableFunction(-1, "DisableScaleInheritance",         SceneNodeFFI::DisableScaleInheritance);
                        script.SetTableFunction(-1, "EnableScaleInheritance",          SceneNodeFFI::EnableScaleInheritance);
                        script.SetTableFunction(-1, "IsScaleInheritanceEnabled",       SceneNodeFFI::IsScaleInheritanceEnabled);

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
                        script.SetTableFunction(-1, "ClampPosition",           SceneNodeFFI::ClampPosition);
                        script.SetTableFunction(-1, "ClampWorldPosition",      SceneNodeFFI::ClampWorldPosition);
                        script.SetTableFunction(-1, "ClampScale",              SceneNodeFFI::ClampScale);
                        script.SetTableFunction(-1, "ClampWorldScale",         SceneNodeFFI::ClampWorldScale);
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


    void InstantiateSceneNode(GT::Script &script, SceneNode &sceneNode)
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

    void UninstantiateSceneNode(GT::Script &script, SceneNode &sceneNode)
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

    void UpdatePublicVariables(GT::Script &script, SceneNode &sceneNode)
    {
        PushSceneNode(script, sceneNode);
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

    void RegisterComponent(GT::Script &script, SceneNode &sceneNode, const char* componentID)
    {
        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("RegisterComponent");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);           // <-- 'self'
                script.Push(componentID);       // <-- 'componentID'
                script.Call(2, 0);
            }
        }
        script.Pop(1);
    }

    void UnregisterComponent(GT::Script &script, SceneNode &sceneNode, const char* componentID)
    {
        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("UnregisterComponent");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);           // <-- 'self'
                script.Push(componentID);       // <-- 'componentID'
                script.Call(2, 0);
            }
        }
        script.Pop(1);
    }

    void PushSceneNode(GT::Script &script, SceneNode &sceneNode)
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


    void DoOnSerialize(GT::Script &script, SceneNode &sceneNode, Serializer &serializer)
    {
        // We want to keep track of the number of serialized data chunks that are written to the serializer so we can know how many iterations to do when deserializing.
        uint32_t dataCount = 0;

        // Because we want to store the count at the top, we will write our data to an intermediary serializer first. After serializing everything to that, we can then
        // write the counter and then the data in the intermediary serializer.
        BasicSerializer intermediarySerializer;



        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("_eventHandlers");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("OnSerialize");
                script.GetTableValue(-2);
                if (script.IsTable(-1))  // <-- Could possibly be null if there is no OnSerialize() implementation. This is a valid case, so don't want to use an assert here.
                {
                    // At this point the top item on the stack is a table containing a list of tables, each representing an OnSerialize() implementation. We need to
                    // keep track of both the method and the name because we'll be serializing that for use when doing deserialization.
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        if (script.IsTable(-1))
                        {
                            script.Push("scriptRelativePath");
                            script.GetTableValue(-2);
                            if (script.IsString(-1))
                            {
                                String scriptRelativePath = script.ToString(-1);

                                script.Push("method");
                                script.GetTableValue(-3);       // <-- -3 instead of -2 because we have scriptRelativePath on the stack, too.
                                if (script.IsFunction(-1))
                                {
                                    // We have found the method. We need to call it, but we don't want to pass the input serializer. We instead want to pass an intermediary one
                                    // so we can get an accurate size.
                                    BasicSerializer localSerializer;

                                    script.PushValue(-7);                                // <-- 'self'
                                    GT::PushNewSerializer(script, localSerializer);   // <-- 'serializer'

                                    script.Call(2, 1);      // <-- Two arguments (self, serializer) and 1 return value (the version).


                                    // At this point, the top item on the stack (the return value from OnSerialize()) is the version number.
                                    uint32_t version = static_cast<uint32_t>(script.ToInteger(-1));

                                    // Now we want to write the data to the main serializer. The way we save the serialized data is as such:
                                    //   - relative file name (string)
                                    //   - version (int32)
                                    //   - size of the data contained in 'intermediarySerializer' as a uint32_t
                                    //   - actual data contained in 'intermediarySerializer'
                                    intermediarySerializer.WriteString(scriptRelativePath);
                                    intermediarySerializer.Write(version);
                                    intermediarySerializer.Write(static_cast<uint32_t>(localSerializer.GetBufferSizeInBytes()));
                                    intermediarySerializer.Write(localSerializer.GetBuffer(), localSerializer.GetBufferSizeInBytes());


                                    // The data counter needs to be incremented.
                                    dataCount += 1;

                                    script.Pop(1);  // <-- Pop the return value.
                                }
                                else
                                {
                                    // 'method' was not found or was not a method. Should never get here, but good for sanity.
                                    script.Pop(1);
                                }
                            }
                            script.Pop(1);
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }
        script.Pop(1);


        // Now we just write the count and the main data.
        serializer.Write(dataCount);
        serializer.Write(intermediarySerializer.GetBuffer(), intermediarySerializer.GetBufferSizeInBytes());
    }

    void DoOnDeserialize(GT::Script &script, SceneNode &sceneNode, Deserializer &deserializer)
    {
        uint32_t dataCount;
        deserializer.Read(dataCount);


        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("_eventHandlers");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("OnDeserialize");
                script.GetTableValue(-2);
                if (script.IsTable(-1))      // <-- Could possibly be null if there is no OnDeserialize() implementation. This is a valid case, so don't want to use an assert here.
                {
                    for (uint32_t i = 0; i < dataCount; ++i)
                    {
                        String scriptRelativePath;
                        uint32_t version;
                        uint32_t dataSizeInBytes;

                        deserializer.ReadString(scriptRelativePath);
                        deserializer.Read(version);
                        deserializer.Read(dataSizeInBytes);

                        // We now need to find the specific event handler based on the relative path of the script.
                        bool found = false;
                        for (script.PushNil(); script.Next(-2); script.Pop(1))
                        {
                            if (script.IsTable(-1))
                            {
                                script.Push("scriptRelativePath");
                                script.GetTableValue(-2);
                                if (script.IsString(-1))
                                {
                                    if (scriptRelativePath == script.ToString(-1))
                                    {
                                        found = true;

                                        // We have found the event handler, so now we need to call the method.
                                        script.Push("method");
                                        script.GetTableValue(-3);          // <-- -3 instead of -2 because we have scriptRelativePath on the stack, too.
                                        if (script.IsFunction(-1))
                                        {
                                            // It is possible for an implementation of OnDeserialize() to read too little or too much data. We will handle this problem by using the deserializer's chunk stack system.
                                            deserializer.StartChunk(dataSizeInBytes);
                                            {
                                                script.PushValue(-7);                                                   // <-- 'self'
                                                GT::PushNewDeserializer(script, deserializer);            // <-- 'deserializer'
                                                script.Push(static_cast<int>(version));                                 // <-- 'version'

                                                script.Call(3, 0);
                                            }
                                            deserializer.EndChunk();
                                        }

                                        script.Pop(1);  // scriptRelativePath
                                        break;
                                    }
                                    else
                                    {
                                        script.Pop(1);  // scriptRelativePath
                                    }
                                }
                            }
                        }


                        // If the script was not found, just seek past the data.
                        if (!found)
                        {
                            deserializer.Seek(static_cast<size_t>(dataSizeInBytes));
                        }
                    }


                    return;     // <-- Important! If we don't return here we'll end up falling through to the error handler below.
                }
            }
        }


        // Error
        //
        // If we get here there was some kind of high-level error. We need to loop through each chunk of data and seek past it based on the data size.

        for (uint32_t i = 0; i < dataCount; ++i)
        {
            String scriptRelativePath;
            uint32_t version;
            uint32_t dataSizeInBytes;

            deserializer.ReadString(scriptRelativePath);
            deserializer.Read(version);
            deserializer.Read(dataSizeInBytes);
            deserializer.Seek(dataSizeInBytes);     // <-- Just seek past the data. Don't waste time reading it.
        }
    }


    void PostSceneNodeEvent_OnUpdate(GT::Script &script, SceneNode &sceneNode, double deltaTimeInSeconds)
    {
        // First we push the scene node.
        PushSceneNode(script, sceneNode);

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

    void PostSceneNodeEvent_OnStartup(GT::Script &script, SceneNode &sceneNode)
    {
        PushSceneNode(script, sceneNode);
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

    void PostSceneNodeEvent_OnShutdown(GT::Script &script, SceneNode &sceneNode)
    {
        PushSceneNode(script, sceneNode);
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

    void PostSceneNodeEvent_OnShow(GT::Script &script, SceneNode &sceneNode)
    {
        PushSceneNode(script, sceneNode);
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

    void PostSceneNodeEvent_OnHide(GT::Script &script, SceneNode &sceneNode)
    {
        PushSceneNode(script, sceneNode);
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

    void PostSceneNodeEvent_OnSceneNodeEnter(GT::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
    {
        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("OnSceneNodeEnter");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);                               // <-- 'self'
                PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                script.Call(2, 0);
            }
        }
        script.Pop(1);
    }

    void PostSceneNodeEvent_OnEnterSceneNode(GT::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
    {
        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("OnEnterSceneNode");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);                               // <-- 'self'
                PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                script.Call(2, 0);
            }
        }
        script.Pop(1);
    }

    void PostSceneNodeEvent_OnSceneNodeLeave(GT::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
    {
        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("OnSceneNodeLeave");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);                               // <-- 'self'
                PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                script.Call(2, 0);
            }
        }
        script.Pop(1);
    }

    void PostSceneNodeEvent_OnLeaveSceneNode(GT::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
    {
        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("OnLeaveSceneNode");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);                               // <-- 'self'
                PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                script.Call(2, 0);
            }
        }
        script.Pop(1);
    }

    void PostSceneNodeEvent_OnSerializeGlobalData(GT::Script &script, SceneNode &sceneNode, Serializer &serializer)
    {
        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("OnSerializeGlobalData");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);                                       // <-- 'self'
                GT::PushNewSerializer(script, serializer);    // <-- 'serializer'
                script.Call(2, 0);
            }
        }
        script.Pop(1);
    }

    void PostSceneNodeEvent_OnDeserializeGlobalData(GT::Script &script, SceneNode &sceneNode, Deserializer &deserializer)
    {
        PushSceneNode(script, sceneNode);
        assert(script.IsTable(-1));
        {
            script.Push("OnDeserializeGlobalData");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);                                           // <-- 'self'
                GT::PushNewDeserializer(script, deserializer);    // <-- 'deserializer'
                script.Call(2, 0);
            }
        }
        script.Pop(1);
    }


    namespace SceneNodeFFI
    {
        int Create(GT::Script &script)
        {
            script.Push(new SceneNode());
            return 1;
        }

        int Delete(GT::Script &script)
        {
            delete reinterpret_cast<SceneNode*>(script.ToPointer(1));
            return 0;
        }


        int GetID(GT::Script &script)
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


        int GetName(GT::Script &script)
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

        int SetName(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->SetName(script.ToString(2));
            }

            return 0;
        }


        int GetParentPtr(GT::Script &script)
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

        int AttachChild(GT::Script &script)
        {
            auto parentNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            auto childNode  = reinterpret_cast<SceneNode*>(script.ToPointer(2));

            if (parentNode != nullptr && childNode != nullptr)
            {
                parentNode->AttachChild(*childNode, script.ToBoolean(3));
            }

            return 0;
        }

        int Orphan(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->DetachFromParent(script.ToBoolean(2));
            }

            return 0;
        }

        int IsAncestor(GT::Script &script)
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

        int IsDescendant(GT::Script &script)
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

        int IsRelative(GT::Script &script)
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


        int GetChildrenIDs(GT::Script &script)
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

        int GetChildIDByName(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                auto child = sceneNode->FindFirstChild(script.ToString(2));
                if (child != nullptr)
                {
                    script.Push(static_cast<int>(child->GetID()));
                }
                else
                {
                    script.Push(0);
                }
            }
            else
            {
                script.Push(0);
            }

            return 1;
        }

        int GetDescendantIDByName(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                auto child = sceneNode->FindFirstChild(script.ToString(2), true);
                if (child != nullptr)
                {
                    script.Push(static_cast<int>(child->GetID()));
                }
                else
                {
                    script.Push(0);
                }
            }
            else
            {
                script.Push(0);
            }

            return 1;
        }



        int AddComponent(GT::Script &script)
        {
            auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            auto componentName = script.ToString(2);

            PushComponent(script, sceneNode->AddComponentByName(componentName));

            return 1;
        }

        int RemoveComponent(GT::Script &script)
        {
            auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            auto componentName = script.ToString(2);

            sceneNode->RemoveComponentByName(componentName);

            return 0;
        }

        int GetComponent(GT::Script &script)
        {
            auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            auto componentName = script.ToString(2);

            PushComponent(script, sceneNode->GetComponentByName(componentName));

            return 1;
        }



        int DisablePositionInheritance(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->DisablePositionInheritance();
            }

            return 0;
        }

        int EnablePositionInheritance(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->EnablePositionInheritance();
            }

            return 0;
        }

        int IsPositionInheritanceEnabled(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                script.Push(sceneNode->IsPositionInheritanceEnabled());
            }

            return 1;
        }


        int DisableOrientationInheritance(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->DisableOrientationInheritance();
            }

            return 0;
        }

        int EnableOrientationInheritance(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->EnableOrientationInheritance();
            }

            return 0;
        }

        int IsOrientationInheritanceEnabled(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                script.Push(sceneNode->IsOrientationInheritanceEnabled());
            }

            return 1;
        }


        int DisableScaleInheritance(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->DisableScaleInheritance();
            }

            return 0;
        }

        int EnableScaleInheritance(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->EnableScaleInheritance();
            }

            return 0;
        }

        int IsScaleInheritanceEnabled(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                script.Push(sceneNode->IsScaleInheritanceEnabled());
            }

            return 1;
        }




        int GetPosition(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetPosition());
            }
            else
            {
                PushNewVector3(script, 0.0f, 0.0f, 0.0f);
            }

            return 1;
        }

        int GetWorldPosition(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetWorldPosition());
            }
            else
            {
                PushNewVector3(script, 0.0f, 0.0f, 0.0f);
            }

            return 1;
        }


        int SetPosition(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                if (script.IsTable(2))
                {
                    sceneNode->SetPosition(ToVector3(script, 2));
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

        int SetWorldPosition(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                if (script.IsTable(2))
                {
                    sceneNode->SetWorldPosition(ToVector3(script, 2));
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


        int GetOrientation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewQuaternion(script, sceneNode->GetOrientation());
            }
            else
            {
                PushNewQuaternion(script, glm::quat());
            }

            return 1;
        }

        int GetWorldOrientation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewQuaternion(script, sceneNode->GetWorldOrientation());
            }
            else
            {
                PushNewQuaternion(script, glm::quat());
            }

            return 1;
        }

        int SetOrientation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                glm::quat orientation;

                if (script.IsTable(2))
                {
                    orientation = ToQuaternion(script, 2);
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

        int SetWorldOrientation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                glm::quat orientation;

                if (script.IsTable(2))
                {
                    orientation = ToQuaternion(script, 2);
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


        int GetEulerRotation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                auto &orientation = sceneNode->GetOrientation();
                PushNewVector3(script, glm::degrees(glm::pitch(orientation)), glm::degrees(glm::yaw(orientation)), glm::degrees(glm::roll(orientation)));
            }
            else
            {
                PushNewVector3(script, 0.0f, 0.0f, 0.0f);
            }

            return 1;
        }

        int GetWorldEulerRotation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                auto orientation = sceneNode->GetWorldOrientation();
                PushNewVector3(script, glm::degrees(glm::pitch(orientation)), glm::degrees(glm::yaw(orientation)), glm::degrees(glm::roll(orientation)));
            }
            else
            {
                PushNewVector3(script, 0.0f, 0.0f, 0.0f);
            }

            return 1;
        }


        int SetEulerRotation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                float pitch;
                float yaw;
                float roll;

                if (script.IsTable(2))
                {
                    glm::vec3 rotation = ToVector3(script, 2);

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

        int SetWorldEulerRotation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                float pitch;
                float yaw;
                float roll;

                if (script.IsTable(2))
                {
                    glm::vec3 rotation = ToVector3(script, 2);

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



        int GetScale(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetScale());
            }
            else
            {
                PushNewVector3(script, 1.0f, 1.0f, 1.0f);
            }

            return 1;
        }

        int GetWorldScale(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetWorldScale());
            }
            else
            {
                PushNewVector3(script, 1.0f, 1.0f, 1.0f);
            }

            return 1;
        }


        int SetScale(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                if (script.IsTable(2))
                {
                    sceneNode->SetScale(ToVector3(script, 2));
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

        int SetWorldScale(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                if (script.IsTable(2))
                {
                    sceneNode->SetWorldScale(ToVector3(script, 2));
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


        int Translate(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                glm::vec3 translation;

                if (script.IsTable(2))
                {
                    translation = ToVector3(script, 2);
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

        int Rotate(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                float     angle = script.ToFloat(2);
                glm::vec3 axis;

                if (script.IsTable(3))
                {
                    axis = ToVector3(script, 3);
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

        int Scale(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                glm::vec3 scale;

                if (script.IsTable(2))
                {
                    scale = ToVector3(script, 2);
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


        int TranslateWorld(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                glm::vec3 translation;

                if (script.IsTable(2))
                {
                    translation = ToVector3(script, 2);
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

        int RotateWorld(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                float     angle = script.ToFloat(2);
                glm::vec3 axis;

                if (script.IsTable(3))
                {
                    axis = ToVector3(script, 3);
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

        int RotateWorldAroundPivot(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                float     angle = script.ToFloat(2);
                glm::vec3 axis;
                glm::vec3 pivot;

                if (script.IsTable(3))
                {
                    axis = ToVector3(script, 3);

                    if (script.IsTable(4))
                    {
                        pivot = ToVector3(script, 4);
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
                        pivot = ToVector3(script, 6);
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


        int InterpolatePosition(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->InterpolatePosition(ToVector3(script, 2), script.ToFloat(3));
            }

            return 0;
        }

        int InterpolateOrientation(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->InterpolateOrientation(ToQuaternion(script, 2), script.ToFloat(3));
            }

            return 0;
        }

        int InterpolateScale(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->InterpolateScale(ToVector3(script, 2), script.ToFloat(3));
            }

            return 0;
        }

        int Slerp(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->Slerp(ToQuaternion(script, 2), script.ToFloat(3));
            }

            return 0;
        }


        int ClampPosition(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->ClampPosition(ToVector3(script, 2), ToVector3(script, 3));
            }

            return 0;
        }

        int ClampWorldPosition(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->ClampWorldPosition(ToVector3(script, 2), ToVector3(script, 3));
            }

            return 0;
        }

        int ClampScale(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->ClampScale(ToVector3(script, 2), ToVector3(script, 3));
            }

            return 0;
        }

        int ClampWorldScale(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->ClampWorldScale(ToVector3(script, 2), ToVector3(script, 3));
            }

            return 0;
        }



        int LookAt(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                glm::vec3 target = ToVector3(script, 2);
                glm::vec3 up     = (script.IsTable(3)) ? ToVector3(script, 3) : glm::vec3(0.0f, 1.0f, 0.0f);

                sceneNode->LookAt(target, up);
            }

            return 0;
        }

        int LookAtSceneNode(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                auto targetSceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                if (targetSceneNode != nullptr)
                {
                    glm::vec3 up = (script.IsTable(3)) ? ToVector3(script, 3) : glm::vec3(0.0f, 1.0f, 0.0f);

                    sceneNode->LookAt(*targetSceneNode, up);
                }
            }

            return 0;
        }

        int LookInDirection(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                glm::vec3 target = ToVector3(script, 2);
                glm::vec3 up     = (script.IsTable(3)) ? ToVector3(script, 3) : glm::vec3(0.0f, 1.0f, 0.0f);

                sceneNode->LookInDirection(target, up);
            }

            return 0;
        }


        int Forward(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetForwardVector());
            }
            else
            {
                PushNewVector3(script, 0.0f, 0.0f, -1.0f);
            }

            return 1;
        }

        int Up(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetUpVector());
            }
            else
            {
                PushNewVector3(script, 0.0f, 1.0f, 0.0f);
            }

            return 1;
        }

        int Right(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetRightVector());
            }
            else
            {
                PushNewVector3(script, 1.0f, 0.0f, 0.0f);
            }

            return 1;
        }

        int WorldForward(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetWorldForwardVector());
            }
            else
            {
                PushNewVector3(script, 0.0f, 0.0f, -1.0f);
            }

            return 1;
        }

        int WorldUp(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetWorldUpVector());
            }
            else
            {
                PushNewVector3(script, 0.0f, 1.0f, 0.0f);
            }

            return 1;
        }

        int WorldRight(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                PushNewVector3(script, sceneNode->GetWorldRightVector());
            }
            else
            {
                PushNewVector3(script, 1.0f, 0.0f, 0.0f);
            }

            return 1;
        }




        int GetAttachedComponentIDs(GT::Script &script)
        {
            script.PushNewTable();

            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                Vector<String> componentNames;
                sceneNode->GetAttachedComponentNames(componentNames);

                for (size_t i = 0; i < componentNames.count; ++i)
                {
                    script.SetTableValue(-1, i + 1, componentNames[i].c_str());
                }
            }

            return 1;
        }


        int Show(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->Show();
            }

            return 0;
        }

        int Hide(GT::Script &script)
        {
            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
            if (sceneNode != nullptr)
            {
                sceneNode->Hide();
            }

            return 0;
        }

        int IsVisible(GT::Script &script)
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

        int GetScenePtr(GT::Script &script)
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


        void PushComponent(GT::Script &script, Component* component)
        {
            if (component != nullptr)
            {
                // GTEngine.<component->GetClassName()>.Create(component);

                script.GetGlobal("GTEngine");
                assert(script.IsTable(-1));
                {
                    script.Push(component->GetClassName());
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
