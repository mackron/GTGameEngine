-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorPropertiesPanel(sceneEditor)
    self:EditorPanel();

    self.Body.MessageContainer = GTGUI.Server.CreateElement(self.Body, "scene-editor-properties-panel-body-message");
    self.Body.PanelsContainer  = GTGUI.Server.CreateElement(self.Body, "scene-editor-properties-panel-panels-container");
    
    self.Body.DetailsPanel     = GTGUI.Server.CreateElement(self.Body.PanelsContainer, "panel-groupbox");
    self.Body.DetailsPanel:SceneEditorDetailsPanel(self, sceneEditor);

    self.Body.TransformPanel   = GTGUI.Server.CreateElement(self.Body.PanelsContainer, "panel-groupbox");
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
    
    
    
    ---------------------------------------------------
    -- New Component Drop-Down Box.
    self.NewComponentDropDownBox = GTGUI.Server.CreateElement(self.Body.PanelsContainer, "picking-dropdown-box");
    self.NewComponentDropDownBox:PickingDropDownBox("New Component");
    self.NewComponentDropDownBox:SetStyle("margin", "4px 8px");
    
    self.NewComponentDropDownBox:AppendItem("Camera"):OnPressed(function()
        local component = self.CurrentSceneNode:AddComponent(GTEngine.Components.Camera);
        if component ~= nil then
            component:Set3DProjection(90.0, 16.0 / 9.0, 0.1, 1000.0);
        end
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Model"):OnPressed(function()
        local component = self.CurrentSceneNode:AddComponent(GTEngine.Components.Model);
        if component ~= nil then
            component:SetModel("engine/models/default.dae");
        end
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Point Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.PointLight);

        self:UpdateComponentPanels();        
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Spot Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.SpotLight);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Directional Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.DirectionalLight);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Ambient Light"):OnPressed(function()
        local component = self.CurrentSceneNode:AddComponent(GTEngine.Components.AmbientLight);
        if component ~= nil then
            component:SetColour(0.25, 0.25, 0.25);
        end
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Dynamics (Collision and Physics)"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Dynamics);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Proximity"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Proximity);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Particle System"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.ParticleSystem);

        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    self.NewComponentDropDownBox:AppendItem("Script"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Script);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end)
    
    

    
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
        -- We need to determine which panels need to be shown and which need to be hidden. We'll hide and then show.
        local componentIDs = {};
        if self.CurrentSceneNode ~= nil then
            componentIDs = self.CurrentSceneNode:GetAttachedComponentIDs();
        end
        
        -- We need to hide every panel that is not contained in componentIDs.
        for componentID,componentPanel in pairs(self.ComponentPanels) do
            if table.indexof(componentIDs, componentID) == nil then
                componentPanel:Hide();
            end
        end
        
        -- Now we need to show and update every panel that is contained in componentIDs.
        for i,componentID in ipairs(componentIDs) do
            local panel = self.ComponentPanels[componentID];
            if panel then
                panel:Show();
                panel:Update(self.CurrentSceneNode);
            end
        end
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

function GTGUI.Element:SceneEditorSceneNavigationPropertiesPanel(sceneEditor)
    self:PanelGroupBox("Navigation");
    
    self.ShowInViewport = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.ShowInViewport:CheckBox("Show in Viewport");
    self.ShowInViewport:SetStyle("margin-bottom", "4px");
    
    self.WalkableHeight       = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.WalkableHeight:LabelledNumberInput("Walkable Height");
    
    self.WalkableRadius       = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.WalkableRadius:LabelledNumberInput("Walkable Radius");
    
    self.WalkableSlopeAngle   = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.WalkableSlopeAngle:LabelledNumberInput("Walkable Slope Angle");
    
    self.WalkableClimbHeight  = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.WalkableClimbHeight:LabelledNumberInput("Walkable Climb Height");
    
    
    self.BuildButtonContainer = GTGUI.Server.CreateElement(self.Body, "");
    self.BuildButtonContainer:SetStyle("horizontal-align", "center");
    
    self.BuildButton = GTGUI.Server.CreateElement(self.BuildButtonContainer, "button");
    self.BuildButton:Button("Build");
    self.BuildButton:SetStyle("margin-top", "8px");
    self.BuildButton:SetStyle("padding", "32px 4px");
    self.BuildButton:OnPressed(function()
        sceneEditor:BuildNavigationMesh(0);
        
        -- If the "Show in Viewport" checkbox is checked, we want to show it after building. It looks strange, but hiding before showing will ensure
        -- that the visualization is refreshed and up-to-date.
        if self.ShowInViewport:IsChecked() then
            sceneEditor:HideNavigationMesh(0);
            sceneEditor:ShowNavigationMesh(0);
        end
    end)
    
    
    self.ShowInViewport:OnChecked(function()
        sceneEditor:ShowNavigationMesh(0);      -- '0' is the navigation mesh index. Always 0 for now, but will change later on when we add support for multiple navigation meshes.
    end)
    
    self.ShowInViewport:OnUnchecked(function()
        sceneEditor:HideNavigationMesh(0);      -- '0' has the same meaning as above.
    end)
    

    self.WalkableHeight:OnValueChanged(function()
        sceneEditor:SetSceneWalkableHeight(self.WalkableHeight:GetValue());
        sceneEditor:SetSceneWalkableRadius(self.WalkableRadius:GetValue());
        sceneEditor:SetSceneWalkableSlopeAngle(self.WalkableSlopeAngle:GetValue());
        sceneEditor:SetSceneWalkableClimbHeight(self.WalkableClimbHeight:GetValue());
    end);
    
    function self:Refresh()
        self.WalkableHeight:SetValue(sceneEditor:GetSceneWalkableHeight());
        self.WalkableRadius:SetValue(sceneEditor:GetSceneWalkableRadius());
        self.WalkableSlopeAngle:SetValue(sceneEditor:GetSceneWalkableSlopeAngle());
        self.WalkableClimbHeight:SetValue(sceneEditor:GetSceneWalkableClimbHeight());
    end
    
    return self;
end

function GTGUI.Element:SceneEditorScenePropertiesPanel(sceneEditor)
    self:EditorPanel();
    self.SceneEditor = sceneEditor;

    self.PanelsContainer = GTGUI.Server.CreateElement(self.Body);
    
    self.DetailsPanel    = GTGUI.Server.CreateElement(self.PanelsContainer, "panel-groupbox");
    self.DetailsPanel:SceneEditorSceneDetailsPropertiesPanel(sceneEditor);
    
    self.RenderingPanel  = GTGUI.Server.CreateElement(self.PanelsContainer, "panel-groupbox");
    self.RenderingPanel:SceneEditorSceneRenderingPropertiesPanel(sceneEditor);
    
    self.NavigationPanel = GTGUI.Server.CreateElement(self.PanelsContainer, "panel-groupbox");
    self.NavigationPanel:SceneEditorSceneNavigationPropertiesPanel(sceneEditor);



    function self:Refresh()
        self.DetailsPanel:Refresh();
        self.RenderingPanel:Refresh();
        self.NavigationPanel:Refresh();
    end
    
    return self;
end


function GTGUI.Element:SceneEditorPanel(sceneEditor)
    self.TabBar               = GTGUI.Server.CreateElement(self, "scene-editor-panel-tabbar");
    self.Body                 = GTGUI.Server.CreateElement(self, "scene-editor-panel-body");
    self.PropertiesPanel      = GTGUI.Server.CreateElement(self.Body, "scene-editor-properties-panel");
    self.HierarchyPanel       = GTGUI.Server.CreateElement(self.Body, "scene-editor-hierarchy-panel");
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
