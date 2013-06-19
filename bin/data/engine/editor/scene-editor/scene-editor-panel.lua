-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorPropertiesPanel(sceneEditor)
    self:EditorPanel();


    self.Body.MessageContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%; margin-top:8px; horizontal-align:center; font-style:bold; text-color:#555; visible:false;' />")
    
    
    self.Body.PanelsContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%;' />")
    
    self.Body.DetailsPanel   = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    self.Body.DetailsPanel:SceneEditorDetailsPanel(self, sceneEditor);

    self.Body.TransformPanel = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    self.Body.TransformPanel:SceneEditorTransformPanel(self, sceneEditor);
    
    
    self.CurrentSceneNode = nil;            -- The scene node whose details are currently being shown on this panel.
    self.SceneEditor      = sceneEditor;

    
    -- This will create the panels for every registered component. We always want the editor metadata to be last.
    self.ComponentPanels = {};
    for key,value in pairs(GTEngine.Components) do
        if value ~= GTEngine.Components.EditorMetadata and value ~= GTEngine.Components.Prefab then
            local panel = Editor.SceneEditor.CreateComponentPanel(self, value);
            if panel then
                self.ComponentPanels[value] = panel;
                
                -- We need to attach an event handler to each component panel for when it is closed. When closing a component panel, that component
                -- will be removed from the scene node. Note that closing a panel is not the same as collapsing it.
                panel:OnClose(function()
                    self.CurrentSceneNode:RemoveComponent(value);

                    self:UpdateComponentPanels();
                    self:OnSceneNodeChanged();
                end);
            end
        end
    end
    
    self.ComponentPanels[GTEngine.Components.EditorMetadata] = Editor.SceneEditor.CreateComponentPanel(self, GTEngine.Components.EditorMetadata);
    
    
    
    -- New Component Button/Menu
    self.NewComponentContainer      = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID()       .. "' styleclass='new-component-container'             style='' />");
    self.NewComponentTitleContainer = GTGUI.Server.New("<div parentid='" .. self.NewComponentContainer:GetID()      .. "' styleclass='new-component-title-container'       style='' />");
    self.NewComponentIcon           = GTGUI.Server.New("<div parentid='" .. self.NewComponentTitleContainer:GetID() .. "' styleclass='new-component-title-container-icon'  style='' />");
    self.NewComponentLabel          = GTGUI.Server.New("<div parentid='" .. self.NewComponentTitleContainer:GetID() .. "' styleclass='new-component-title-container-label' style=''>New Component</div>");
    self.NewComponentBottomBorder   = GTGUI.Server.New("<div parentid='" .. self.NewComponentTitleContainer:GetID() .. "' styleclass='new-component-title-container-bottom-border' />");
    
    self.NewComponentMenu           = GTGUI.Server.New("<div parentid='" .. self.NewComponentContainer:GetID()      .. "' styleclass='new-component-menu'  style='visible:false;' />");
    self.NewComponentMenu:NewComponentMenu();
    
    self.NewComponentTitleContainer:OnLMBDown(function()
        if not self.NewComponentTitleContainer.IsOpen then
            self.NewComponentTitleContainer.IsOpen = true;
            self.NewComponentTitleContainer:AttachStyleClass("new-component-title-container-open");
            self.NewComponentIcon:AttachStyleClass("new-component-title-container-icon-open");
            self.NewComponentBottomBorder:Show();
            self.NewComponentMenu:Show();
        else
            self.NewComponentTitleContainer.IsOpen = false;
            self.NewComponentTitleContainer:DetachStyleClass("new-component-title-container-open");
            self.NewComponentIcon:DetachStyleClass("new-component-title-container-icon-open");
            self.NewComponentBottomBorder:Hide();
            self.NewComponentMenu:Hide();
        end
    end);
    
    
    self.NewComponentMenu:AppendNewItem("Camera"):OnPressed(function()
        local component = self.CurrentSceneNode:AddComponent(GTEngine.Components.Camera);
        if component ~= nil then
            component:Set3DProjection(90.0, 16.0 / 9.0, 0.1, 1000.0);
        end
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Model"):OnPressed(function()
        local component = self.CurrentSceneNode:AddComponent(GTEngine.Components.Model);
        if component ~= nil then
            component:SetModel("engine/models/default.dae");
        end
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Point Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.PointLight);

        self:UpdateComponentPanels();        
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Spot Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.SpotLight);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Directional Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.DirectionalLight);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Ambient Light"):OnPressed(function()
        local component = self.CurrentSceneNode:AddComponent(GTEngine.Components.AmbientLight);
        if component ~= nil then
            component:SetColour(0.25, 0.25, 0.25);
        end
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Dynamics (Collision and Physics)"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Dynamics);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Proximity"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Proximity);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Particle System"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.ParticleSystem);

        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Script"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Script);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    

    
    -- Clears the panels and shows a message.
    function self:HidePanels(message)
        self.Body.PanelsContainer:Hide();
        
        self.Body.MessageContainer:Show();
        self.Body.MessageContainer:SetText(message);
    end
    
    -- Shows the panels, but does not update them.
    --
    -- This will also update the details of the panels.
    function self:ShowPanels()
        self.Body.MessageContainer:Hide();
        self.Body.PanelsContainer:Show();
    end
    
    
    -- Updates the details panel.
    function self:UpdateDetailsPanel()
        if self.CurrentSceneNode ~= nil then
            self.Body.DetailsPanel:Update(self.CurrentSceneNode);
        end
    end
    
    -- Updates the transform panel to show the transformation of the current node.
    function self:UpdateTransformPanel()
        if self.CurrentSceneNode ~= nil then
            self.Body.TransformPanel:Update(self.CurrentSceneNode);
        end
    end
    
    
    -- Updates the component panels.
    function self:UpdateComponentPanels()
        self:HideAllComponentPanels();
        self:ShowComponentPanels();
    end
    
    
    -- Updates the properties in the script panel.
    function self:UpdateScriptProperties()
        local scriptPanel = self.ComponentPanels[GTEngine.Components.Script];
        if scriptPanel ~= nil then
            scriptPanel:UpdateVariables();
        end
    end
    
    
    -- Hides every component panel.
    function self:HideAllComponentPanels()
        for key,value in pairs(self.ComponentPanels) do
            value:Hide();
        end
    end
    
    -- Shows the relevant component panels for the current node.
    function self:ShowComponentPanels()
        if self.CurrentSceneNode ~= nil then
            local componentIDs = self.CurrentSceneNode:GetAttachedComponentIDs();
            for i,value in ipairs(componentIDs) do
                local panel = self.ComponentPanels[value];
                if panel then
                    panel:Show();
                    panel:Update(self.CurrentSceneNode);
                end
            end
        end
    end
    
    
    
    -- Updates the panel to show the details of the given scene node.\
    --
    -- This does not actually show the panels.
    function self:Update(node)
        self.CurrentSceneNode = node;
        
        if self.CurrentSceneNode ~= nil then
            self:UpdateDetailsPanel();
            self:UpdateTransformPanel();
            self:UpdateComponentPanels();
        end
    end
    

    function self:OnSceneNodeChanged(arg1)
        self.Callbacks:BindOrCall("OnSceneNodeChanged", arg1);
    end
    
    
    
    return self;
end



function GTGUI.Element:SceneEditorSceneDetailsPropertiesPanel(sceneEditor)
    self:PanelGroupBox("Details");
    
    self.NameTextBox = GTGUI.Server.CreateElement(self.Body, "labelled-textbox");
    self.NameTextBox:LabelledTextBox("Name", "");
    
    self.NameTextBox:OnTextChanged(function()
        sceneEditor:SetSceneName(self.NameTextBox:GetText());
    end);
    
    function self:Refresh()
        self.NameTextBox:SetText(sceneEditor:GetSceneName());
    end
    
    return self;
end

function GTGUI.Element:SceneEditorSceneRenderingPropertiesPanel(sceneEditor)
    self:PanelGroupBox("Rendering");
    
    -- Clear Background
    self.ClearBackgroundCheckBox = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.ClearBackgroundCheckBox:CheckBox("Clear Background");
    self.ClearBackgroundCheckBox:Check();
    
    self.ClearBackgroundColour = GTGUI.Server.CreateElement(self.Body, "vector3-input");
    self.ClearBackgroundColour:Vector3Input();
    self.ClearBackgroundColour:SetValue(0.5);
    self.ClearBackgroundColour:SetStyle("margin-top", "2px");
    
    self.ClearBackgroundCheckBox:OnChecked(function()
        self.ClearBackgroundColour:Enable();
        sceneEditor:EnableSceneBackgroundClearing(self.ClearBackgroundColour:GetValue());
    end);
    
    self.ClearBackgroundCheckBox:OnUnchecked(function()
        self.ClearBackgroundColour:Disable();
        sceneEditor:DisableSceneBackgroundClearing();
    end);
    
    self.ClearBackgroundColour:OnValueChanged(function()
        sceneEditor:EnableSceneBackgroundClearing(self.ClearBackgroundColour:GetValue());
    end);
    
    
    -- HDR
    self.EnableHDRCheckBox = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.EnableHDRCheckBox:CheckBox("Enable HDR");
    self.EnableHDRCheckBox:Uncheck();
    self.EnableHDRCheckBox:SetStyle("margin-top", "8px");
    
    self.EnableHDRCheckBox:OnChecked(function()
        sceneEditor:EnableSceneHDR();
    end);
    
    self.EnableHDRCheckBox:OnUnchecked(function()
        sceneEditor:DisableSceneHDR();
    end);
    
    
    -- Bloom
    self.EnableBloomCheckBox = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.EnableBloomCheckBox:CheckBox("Enable Bloom");
    self.EnableBloomCheckBox:Uncheck();
    self.EnableBloomCheckBox:SetStyle("margin-top", "8px");
    
    self.EnableBloomCheckBox:OnChecked(function()
        sceneEditor:EnableSceneBloom();
    end);
    
    self.EnableBloomCheckBox:OnUnchecked(function()
        sceneEditor:DisableSceneBloom();
    end);
    
    
    function self:Refresh()
        if sceneEditor:IsSceneBackgroundClearingEnabled() then
            self.ClearBackgroundCheckBox:Check(true);       -- 'true' means to block posting the event.
            self.ClearBackgroundColour:Enable();
        else
            self.ClearBackgroundCheckBox:Uncheck(false);    -- 'true' means to block posting the event.
            self.ClearBackgroundColour:Disable();
        end
        self.ClearBackgroundColour:SetValue(sceneEditor:GetSceneBackgroundClearColour());
        
        
        if sceneEditor:IsSceneHDREnabled() then
            self.EnableHDRCheckBox:Check(true);             -- 'true' means to block posting the event.
        else
            self.EnableHDRCheckBox:Uncheck(true);           -- 'true' means to block posting the event.
        end
        
        if sceneEditor:IsSceneBloomEnabled() then
            self.EnableBloomCheckBox:Check(true);           -- 'true' means to block posting the event.
        else
            self.EnableBloomCheckBox:Uncheck(true);         -- 'true' means to block posting the event.
        end
    end
    
    return self;
end

function GTGUI.Element:SceneEditorScenePropertiesPanel(sceneEditor)
    self:EditorPanel();
    self.SceneEditor = sceneEditor;

    self.PanelsContainer = GTGUI.Server.CreateElement(self.Body);
    
    self.DetailsPanel   = GTGUI.Server.CreateElement(self.PanelsContainer, "panel-groupbox");
    self.DetailsPanel:SceneEditorSceneDetailsPropertiesPanel(sceneEditor);
    
    self.RenderingPanel = GTGUI.Server.CreateElement(self.PanelsContainer, "panel-groupbox");
    self.RenderingPanel:SceneEditorSceneRenderingPropertiesPanel(sceneEditor);

    
    
    
    function self:Refresh()
        self.DetailsPanel:Refresh();
        self.RenderingPanel:Refresh();
    end
    

    return self;
end


function GTGUI.Element:SceneEditorPanel(sceneEditor)
    self.TabBar               = GTGUI.Server.New("<div parentid='" .. self:GetID()      .. "' styleclass='scene-editor-panel-tabbar'        style='' />");
    self.Body                 = GTGUI.Server.New("<div parentid='" .. self:GetID()      .. "' styleclass='scene-editor-panel-body'          style='' />");
    self.PropertiesPanel      = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='scene-editor-properties-panel'    style='visible:false' />");
    self.HierarchyPanel       = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='scene-editor-hierarchy-panel'     style='visible:false' />");
    self.ScenePropertiesPanel = GTGUI.Server.CreateElement(self.Body, "scene-editor-properties-panel");
    
    self.SceneEditor          = sceneEditor;
    
    
    self.TabBar:TabBar();
    self.TabBar.ActiveTabBorderColor     = "#222";
    self.TabBar.HoveredTabBorderColor    = "#222";
    self.TabBar.ActiveTabBackgroundColor = "#363636";
    
    self.ScenePropertiesTab = self.TabBar:AddTab("Scene Properties");
    self.HierarchyTab       = self.TabBar:AddTab("Hierarchy");
    self.PropertiesTab      = self.TabBar:AddTab("Properties");
    
    
    self.TabBar:OnTabActivated(function(data)
        if data.tab == self.PropertiesTab then
            self.PropertiesPanel:Show();
        elseif data.tab == self.HierarchyTab then
            self.HierarchyPanel:Show();
        elseif data.tab == self.ScenePropertiesTab then
            self.ScenePropertiesPanel:Show();
        end
    end);
    
    self.TabBar:OnTabDeactivated(function(data)
        if data.tab == self.PropertiesTab then
            self.PropertiesPanel:Hide();
        elseif data.tab == self.HierarchyTab then
            self.HierarchyPanel:Hide();
        elseif data.tab == self.ScenePropertiesTab then
            self.ScenePropertiesPanel:Hide();
        end
    end);
    
    
    self.TabBar:ActivateTab(self.PropertiesTab);
    
    
    
    self:OnDrop(function(data)
        if self.PropertiesPanel:IsVisible() then
            -- If we drop a script file onto the properties panel we want to add a script component if it doesn't already have one
            -- and add the given script to it.
            if data.droppedElement.isAsset and GTEngine.IsScriptFile(data.droppedElement.path) then
                if self.PropertiesPanel.CurrentSceneNode ~= nil then
                    local scriptComponent = self.PropertiesPanel.CurrentSceneNode:GetComponent(GTEngine.Components.Script);
                    if scriptComponent == nil then
                        scriptComponent = self.PropertiesPanel.CurrentSceneNode:AddComponent(GTEngine.Components.Script);
                    end
                    
                    self.PropertiesPanel:UpdateComponentPanels();
                    self.PropertiesPanel.ComponentPanels[GTEngine.Components.Script]:AddScript(data.droppedElement.path);
                end
            end
        end
    end);
    
    
    
    self.PropertiesPanel:SceneEditorPropertiesPanel(sceneEditor);
    self.HierarchyPanel:SceneEditorHierarchyPanel(sceneEditor);
    self.ScenePropertiesPanel:SceneEditorScenePropertiesPanel(sceneEditor);
    
    
    -- Properties Panel Events.
    self.PropertiesPanel:OnSceneNodeChanged(function()
        self.SceneEditor:CommitStateStackFrame()
    end);
end
