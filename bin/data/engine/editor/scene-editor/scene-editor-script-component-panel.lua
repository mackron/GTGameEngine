-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ScriptComponentPanelVariable(name, component)
    self.Name      = name;
    self.Component = component;
    
    if self.OnValueChanged == nil then
        function self:OnValueChanged(arg1)
            self.Callbacks:BindOrCall("OnValueChanged", arg1);
        end
    end
end


function GTGUI.Element:ScriptComponentPanelVariable_Number(name, component)
    self:ScriptComponentPanelVariable(name, component);
    self:LabelledNumberInput(name);
    
    function self:UpdateValue()
        self:SetValue(component:GetPublicVariableValue(name));
    end
    
    self:OnValueChanged(function()
        component:SetPublicVariableValue(name, self:GetValue());
    end)
    
    
    self:UpdateValue();
    return self;
end

function GTGUI.Element:ScriptComponentPanelVariable_Vec2(name, component)
    self:ScriptComponentPanelVariable(name, component);
    self:LabelledVector2Input(name);
    
    function self:UpdateValue()
        self:SetValue(component:GetPublicVariableValue(name));
    end
    
    self:OnValueChanged(function()
        component:SetPublicVariableValue(name, self:GetValue());
    end);
    
    
    self:UpdateValue();
    return self;
end

function GTGUI.Element:ScriptComponentPanelVariable_Vec3(name, component)
    self:ScriptComponentPanelVariable(name, component);
    self:LabelledVector3Input(name);
    
    function self:UpdateValue()
        self:SetValue(component:GetPublicVariableValue(name));
    end
    
    self:OnValueChanged(function()
        component:SetPublicVariableValue(name, self:GetValue());
    end);
    
    
    self:UpdateValue();
    return self;
end

function GTGUI.Element:ScriptComponentPanelVariable_Vec4(name, component)
    self:ScriptComponentPanelVariable(name, component);
    self:LabelledVector4Input(name);
    
    function self:UpdateValue()
        self:SetValue(component:GetPublicVariableValue(name));
    end
    
    self:OnValueChanged(function()
        component:SetPublicVariableValue(name, self:GetValue());
    end);
    
    
    self:UpdateValue();
    return self;
end

function GTGUI.Element:ScriptComponentPanelVariable_Boolean(name, component)
    self:ScriptComponentPanelVariable(name, component);
    self:LabelledElement(name);
    
    self.CheckBox = GTGUI.Server.CreateElement(self.Right, "checkbox");
    self.CheckBox:CheckBox();
    self.CheckBox.text:Hide();          -- This removes the little margin so that it lies flush with the edge.
    
    function self:UpdateValue()
        self.CheckBox:SetChecked(component:GetPublicVariableValue(name));
    end
    
    self.CheckBox:OnChecked(function()
        component:SetPublicVariableValue(name, self.CheckBox:IsChecked());
        self:OnValueChanged();
    end)
    
    self.CheckBox:OnUnchecked(function()
        component:SetPublicVariableValue(name, self.CheckBox:IsChecked());
        self:OnValueChanged();
    end)
    
    
    self:UpdateValue();
    return self;
end

function GTGUI.Element:ScriptComponentPanelVariable_String(name, component)
    self:ScriptComponentPanelVariable(name, component);
    self:LabelledTextBox(name);
    
    function self:UpdateValue()
        local newValue = component:GetPublicVariableValue(name);
        if self:GetText() ~= newValue then
            self:SetText(newValue);
        end
    end
    

    self:OnTextChanged(function()
        component:SetPublicVariableValue(name, self:GetText() or "");
        self:OnValueChanged();
    end);
    
    self.TextBox:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self:SetText(data.droppedElement.path);
            self:OnTextChanged();                       -- Need to explicitly call this because SetText() won't.
            
            -- TODO: When we implement the multi-line text box feature for strings we'll want to do explicit SetPublicVariableValue() and OnValueChanged commands.
        end
    end);
    
    
    self:UpdateValue();
    return self;
end

function GTGUI.Element:ScriptComponentPanelVariable_Prefab(name, component)
    self:ScriptComponentPanelVariable(name, component);
    self:LabelledTextBox(name);
    
    function self:UpdateValue()
        local newValue = component:GetPublicVariableValue(name);
        if self:GetText() ~= newValue then
            self:SetText(newValue);
        end
    end
    

    self:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            component:SetPublicVariableValue(name, self:GetText() or "");
            self:OnValueChanged();
        end
    end);
    
    self.TextBox:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self:SetText(data.droppedElement.path);
            component:SetPublicVariableValue(name, self:GetText() or "");
            self:OnValueChanged();
        end
    end);
    
    
    self:UpdateValue();
    return self;
end


function GTGUI.Element:ScriptComponentPanel()
    self:PanelGroupBox("Script", true);
    self.ScriptsContainer   = GTGUI.Server.CreateElement(self.Body);

    self.NewScriptContainer = GTGUI.Server.CreateElement(self.Body, "new-script-container");
    self.NewScriptTextBox   = GTGUI.Server.CreateElement(self.NewScriptContainer, "new-script-textbox");
    self.NewScriptIcon      = GTGUI.Server.CreateElement(self.NewScriptContainer, "new-script-icon");
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
        
    
        local new = GTGUI.Server.CreateElement(self.ScriptsContainer, "script-component-panel-script-container");
        
        ---------------------------------
        -- Header
        new.Header             = GTGUI.Server.CreateElement(new, "script-component-panel-script-container-header");
        
        new.FilePathTextBox    = GTGUI.Server.CreateElement(new.Header, "script-component-panel-script-container-text-box");
        new.FilePathTextBox:SetText(name);
        
        new.EditButton         = GTGUI.Server.CreateElement(new.Header, "script-component-panel-script-container-edit-button");
        new.EditButton:OnPressed(function()
            Editor.OpenFile(new.FilePathTextBox:GetText());
        end);
        
        new.CloseButton        = GTGUI.Server.CreateElement(new.Header, "panel-groupbox-title-cross");
        new.CloseButton:SetStyle("margin-left", "4px");
        new.CloseButton:OnPressed(function()
            self:RemoveScript(new);
        end);
        
        self.ScriptPanels[#self.ScriptPanels + 1] = new;
        

        ---------------------------------
        -- Variables.
        --
        -- These need to be done after adding the script to the component.
        new.VariablesContainer = GTGUI.Server.CreateElement(new, "script-component-panel-variables-container");
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
                local variableContainer = GTGUI.Server.CreateElement(panel.VariablesContainer, "script-component-panel-variable");
            
                if     nameAndType.type == GTEngine.ScriptVariableTypes.Number  then variableContainer:ScriptComponentPanelVariable_Number( nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec2    then variableContainer:ScriptComponentPanelVariable_Vec2(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec3    then variableContainer:ScriptComponentPanelVariable_Vec3(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec4    then variableContainer:ScriptComponentPanelVariable_Vec4(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Boolean then variableContainer:ScriptComponentPanelVariable_Boolean(nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.String  then variableContainer:ScriptComponentPanelVariable_String( nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Prefab  then variableContainer:ScriptComponentPanelVariable_Prefab( nameAndType.name, self.CurrentComponent);
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