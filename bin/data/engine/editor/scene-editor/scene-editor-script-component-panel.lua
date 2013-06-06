-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ScriptVariableContainer(name, component)
    self.Component = component;
    self.Name      = name;
    
    self.NameValueContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                    .. "' styleclass='' style='child-plane:horizontal; flex-child-width:true; vertical-align:center;' />");
    self.NameContainer      = GTGUI.Server.New("<div parentid='" .. self.NameValueContainer:GetID() .. "' styleclass='' style='width:auto; vertical-align:center; padding-right:4px;' />");
    self.ValueContainer     = GTGUI.Server.New("<div parentid='" .. self.NameValueContainer:GetID() .. "' styleclass='' style='width:100%; vertical-align:center; horizontal-align:right;' />");
    
    self.NameLabel = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' styleclass='' style='width:auto;' />");
    self.NameLabel:SetText(name .. ":");
    
    
    function self:OnValueChanged(arg1)
        self.Callbacks:BindOrCall("OnValueChanged", arg1);
    end
end

function GTGUI.Element:ScriptVariableContainer_Number(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueTextBox:SetValue(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueTextBox = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='textbox' style='width:100%; max-width:72px' />");
    self.ValueTextBox:NumberTextBox();
    self:UpdateValue();
    
    self.ValueTextBox:OnTextChanged(function()
        self.Component:SetPublicVariableValue(self.Name, tonumber(self.ValueTextBox:GetText()) or 0.0);
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_Vec2(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueInput:SetFromXYTable(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueInput = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='vector-input' style='width:100%; max-width:146px' />");
    self.ValueInput:Vector2Input()
    self:UpdateValue();
    
    self.ValueInput:OnValueChanged(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:GetXYTable());
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_Vec3(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueInput:SetFromXYZTable(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueInput = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='vector-input' style='width:100%; max-width:220px' />");
    self.ValueInput:Vector3Input()
    self:UpdateValue();
    
    self.ValueInput:OnValueChanged(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:GetXYZTable());
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_Vec4(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueInput:SetFromXYZWTable(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueInput = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='vector-input' style='width:100%; max-width:296px' />");
    self.ValueInput:Vector4Input();
    self:UpdateValue();
    
    self.ValueInput:OnValueChanged(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:GetXYZWTable());
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_Boolean(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueInput:SetChecked(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueInput = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='checkbox' style='' />");
    self.ValueInput:CheckBox();
    self.ValueInput.text:Hide();        -- This removes the little margin so that it lies flush with the edge.
    self:UpdateValue();
    
    
    self.ValueInput:OnChecked(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:IsChecked());
        self:OnValueChanged();
    end);
    
    self.ValueInput:OnUnchecked(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:IsChecked());
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_String(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        local newValue = component:GetPublicVariableValue(name);
        if self.ValueTextBox:GetText() ~= newValue then
            self.ValueTextBox:SetText(newValue);
        end
    end
    
    
    self.ValueTextBox = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    self:UpdateValue();
    
    self.ValueTextBox:OnTextChanged(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueTextBox:GetText() or "");
        self:OnValueChanged();
    end);
    
    self.ValueTextBox:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.ValueTextBox:SetText(data.droppedElement.path);
            -- TODO: When we implement the multi-line text box feature for strings we'll want to do explicit SetPublicVariableValue() and OnValueChanged commands.
        end
    end);
end

function GTGUI.Element:ScriptVariableContainer_Prefab(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        local newValue = component:GetPublicVariableValue(name);
        if self.ValueTextBox:GetText() ~= newValue then
            self.ValueTextBox:SetText(newValue);
        end
    end
    
    
    self.ValueTextBox = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    self:UpdateValue();
    
    self.ValueTextBox:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            self.Component:SetPublicVariableValue(self.Name, self.ValueTextBox:GetText() or "");
            self:OnValueChanged();
        end
    end);
    
    self.ValueTextBox:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.ValueTextBox:SetText(data.droppedElement.path);
            self.Component:SetPublicVariableValue(self.Name, self.ValueTextBox:GetText() or "");
            self:OnValueChanged();
        end
    end);
end


function GTGUI.Element:ScriptComponentPanel()
    self:PanelGroupBox("Script", true);
    self.ScriptsContainer   = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass=''                  style='' />");

    
    self.NewScriptContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()               .. "' styleclass='new-script-container' style='margin-top:4px' />");
    self.NewScriptTextBox   = GTGUI.Server.New("<div parentid='" .. self.NewScriptContainer:GetID() .. "' styleclass='new-script-textbox'   style='' />");
    self.NewScriptIcon      = GTGUI.Server.New("<div parentid='" .. self.NewScriptContainer:GetID() .. "' styleclass='new-script-icon'      style='' />");
    self.ScriptPanels       = {};
    
    self.CurrentNode        = nil;
    self.CurrentComponent   = nil;
    self.IsUpdating         = false;
    
    
    
    self.NewScriptTextBox:OnDrop(function(data)
        if data.droppedElement.isAsset then
            if not self.CurrentComponent:IsUsingScript(data.droppedElement.path) then
                if self:AddScript(data.droppedElement.path) then
                    self.NewScriptTextBox:SetText("");
                end
            end
        end
    end);
    
    
    self.NewScriptTextBox:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            if self:AddScript(self.NewScriptTextBox:GetText()) then
                self.NewScriptTextBox:SetText("");
            end
        end
    end);
    
    self.NewScriptIcon:OnPressed(function(data)
        if self:AddScript(self.NewScriptTextBox:GetText()) then
            self.NewScriptTextBox:SetText("");
        end
    end);
    
    
    
    
    function self:AddScript(name)
        -- If we're not updating we want to add the script to the component itself.
        if not self.IsUpdating then
            if not self.CurrentComponent:IsUsingScript(name) then
                if self.CurrentComponent:AddScript(name) then
                    self.ParentPanel:OnSceneNodeChanged();
                else
                    return false;
                end
            else
                return false;
            end
        end
        
    
        local new = GTGUI.Server.New("<div parentid='" .. self.ScriptsContainer:GetID() .. "' styleclass='' style='margin-bottom:4px;' />");
        
        ---------------------------------
        -- Header
        new.Header             = GTGUI.Server.New("<div parentid='" .. new:GetID()        .. "' styleclass=''                           style='child-plane:horizontal; flex-child-width:true; vertical-align:center;' />");
        
        new.FilePathTextBox    = GTGUI.Server.New("<div parentid='" .. new.Header:GetID() .. "' styleclass=''                           style='width:100%; text-color:std-text-color; font-style:bold;' />");
        new.FilePathTextBox:SetText(name);
        
        new.EditButton         = GTGUI.Server.CreateElement(new.Header, "scene-editor-script-component-panel-edit-button");
        new.EditButton:OnPressed(function()
            Editor.OpenFile(new.FilePathTextBox:GetText());
        end);
        
        new.CloseButton        = GTGUI.Server.New("<div parentid='" .. new.Header:GetID() .. "' styleclass='panel-groupbox-title-cross' style='margin-left:4px;' />");
        new.CloseButton:OnPressed(function()
            self:RemoveScript(new);
        end);
        
        

        self.ScriptPanels[#self.ScriptPanels + 1] = new;
        
        
        
        
        
        ---------------------------------
        -- Variables.
        --
        -- These need to be done after adding the script to the component.
        new.VariablesContainer = GTGUI.Server.New("<div parentid='" .. new:GetID() .. "' styleclass='' style='padding:4px; padding-left:8px; padding-right:0px; border-bottom:1px #444' />");
        new.Variables          = {};
        
        -- We initialize the variables by just "reloading" them.
        self:ReloadVariables(new);
        
        return true;    -- Successful.
    end
    
    
    function self:RemoveScript(panelToRemove)
        for i,panel in ipairs(self.ScriptPanels) do
            if panel == panelToRemove then
                self:RemoveScriptByIndex(i);
                return;
            end
        end
    end
    
    function self:RemoveScriptByRelativePath(relativePath)
        for i,panel in ipairs(self.ScriptPanels) do
            if panel.RelativePath == relativePath then
                self:RemoveScriptByIndex(i);
                return;
            end
        end
    end
    
    function self:RemoveScriptByIndex(index)
        local panel = self.ScriptPanels[index];
        if panel ~= nil then
            table.remove(self.ScriptPanels, index);
            
        
            -- If we're not updating we want to remove the script the component itself.
            if not self.IsUpdating then
                self.CurrentComponent:RemoveScriptByIndex(index);
                self.ParentPanel:OnSceneNodeChanged();
            end
        
        
            -- And now we can remove the panel completely.
            GTGUI.Server.DeleteElement(panel);
        end
    end
    
    function self:RemoveAllScripts()
        for i,panel in ipairs(self.ScriptPanels) do
            GTGUI.Server.DeleteElement(panel);
        end
        
        self.ScriptPanels = {};
    end
    

    
    function self:ReloadVariables(panel)
        panel.VariablesContainer:DeleteAllChildren();
        panel.Variables = {};
    
        -- We need to grab the names and types of the public variables in the given script.
        local publicVariableNamesAndTypes = self.CurrentComponent:GetPublicVariableNamesAndTypesByIndex(#self.ScriptPanels);
        for i,nameAndType in ipairs(publicVariableNamesAndTypes) do
            if nameAndType.type ~= GTEngine.ScriptVariableTypes.Unknown and nameAndType.type ~= GTEngine.ScriptVariableTypes.None then
                local variableContainer = GTGUI.Server.New("<div parentid='" .. panel.VariablesContainer:GetID() .. "' styleclass='script-variable-container' style='' />");
            
                if     nameAndType.type == GTEngine.ScriptVariableTypes.Number  then variableContainer:ScriptVariableContainer_Number( nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec2    then variableContainer:ScriptVariableContainer_Vec2(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec3    then variableContainer:ScriptVariableContainer_Vec3(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec4    then variableContainer:ScriptVariableContainer_Vec4(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Boolean then variableContainer:ScriptVariableContainer_Boolean(nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.String  then variableContainer:ScriptVariableContainer_String( nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Prefab  then variableContainer:ScriptVariableContainer_Prefab( nameAndType.name, self.CurrentComponent);
                end
                
                variableContainer:OnValueChanged(function()
                    self.ParentPanel:OnSceneNodeChanged();
                end);
                
                panel.Variables[#panel.Variables + 1] = variableContainer;
            end
        end
    end
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Script);
        self.IsUpdating       = true;
        
        
        self:RemoveAllScripts();
        
        for i,scriptRelativePath in ipairs(self.CurrentComponent:GetScriptFilePaths()) do
            self:AddScript(scriptRelativePath);
        end
        
        
        self.IsUpdating       = false;
    end
    
    function self:UpdateVariables()
        for i,panel in ipairs(self.ScriptPanels) do
            for i,variableContainer in ipairs(panel.Variables) do
                variableContainer:UpdateValue();
            end
        end
    end
    

    return self;
end