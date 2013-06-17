-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorPropertiesPanel(sceneEditor)
    self.Body      = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='scene-editor-properties-panel-body' style='' />");
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='vertical-scrollbar'      style='' />");
    
    self.Body.MessageContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%; margin-top:8px; horizontal-align:center; font-style:bold; text-color:#555; visible:false;' />")
    
    
    self.Body.PanelsContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%;' />")
    
    self.Body.DetailsPanel   = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    self.Body.DetailsPanel:SceneEditorDetailsPanel(self, sceneEditor);

    self.Body.TransformPanel = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    self.Body.TransformPanel:SceneEditorTransformPanel(self, sceneEditor);
    
    
    self.CurrentSceneNode = nil;            -- The scene node whose details are currently being shown on this panel.
    self.SceneEditor      = sceneEditor;
    
    self.IsMouseOver           = false;
    self.MouseWheelScrollSpeed = 32;
    
    
    
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
    
    
    
    
    

    
    self.Scrollbar:VerticalScrollbar();
    self.Scrollbar:SetPageSize(self:GetHeight());
    self.Scrollbar:EnableAutoVisibility();
    self.Scrollbar:OnScroll(function(data)
        local offset = -data.position;
        self.Body:SetStyle("inner-offset-y", tostring(offset));
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
    
    
    
    self:OnSize(function()
        if self:IsVisible() then
            self.Scrollbar:SetPageSize(self:GetHeight());
        end
    end);
    
    self.Body:OnSize(function()
        if self.Body:IsVisible() then
            self.Scrollbar:SetRange(self.Body:GetHeight());
        end
    end)
    

    self:OnMouseEnter(function()
        self.IsMouseOver = true;
    end);
    
    self:OnMouseLeave(function()
        self.IsMouseOver = false;
    end);
    
    self:WatchMouseWheel(function(data)
        if self.IsMouseOver then
            self.Scrollbar:Scroll(-data.delta * self.MouseWheelScrollSpeed);
        end
    end);

    
    
    function self:OnSceneNodeChanged(arg1)
        self.Callbacks:BindOrCall("OnSceneNodeChanged", arg1);
    end
    
    
    
    return self;
end


function GTGUI.Element:SceneEditorHierarchyPanel(sceneEditor)
    self.TreeView    = GTGUI.Server.New("<div parentid='" .. self:GetID()      .. "' styleclass='scene-editor-hierarchy-treeview' style='' />");
    self.ContextMenu = GTGUI.Server.New("<div                                        styleclass='menu'                            style='z-index:100; positioning:absolute; visible:false' />");
    self.SceneNodes  = {}
    self.SceneEditor = sceneEditor;
    
    
    
    self.TreeView:TreeView();
    
    self.TreeView.body:OnDrop(function(data)
        if data.droppedElement ~= nil and data.droppedElement.sceneNodeID ~= nil then
            local sceneNodePtr = self.SceneEditor:GetSceneNodePtrByID(data.droppedElement.sceneNodeID);
            
            if GTEngine.System.SceneNode.GetParentPtr(sceneNodePtr) ~= nil then
                GTEngine.System.SceneNode.Orphan(sceneNodePtr, true);
                self.SceneEditor:SelectSceneNode(data.droppedElement.sceneNodeID);
                
                -- Undo/Redo point.
                self.SceneEditor:CommitStateStackFrame();
            end
        end
    end);
    
    
    -- Context Menu
    self.ContextMenu:Menu();
    self.ContextMenu:EnableDefaultEvents();
    
    self.ContextMenu.Duplicate = self.ContextMenu:AppendItem("Duplicate"):OnPressed(function()
        self.SceneEditor:DuplicateSceneNode(self.ContextMenu.SceneNodeID);
    end);
    
    self.ContextMenu.Duplicate = self.ContextMenu:AppendItem("Delete"):OnPressed(function()
        self.SceneEditor:DeleteSceneNode(self.ContextMenu.SceneNodeID);
    end);
    
    
    self.ContextMenu:AppendSeparator();
    
    self.ContextMenu.Orphan = self.ContextMenu:AppendItem("Orphan"):OnPressed(function()
        self.SceneEditor:OrphanSceneNode(self.ContextMenu.SceneNodeID);
        self.SceneEditor:SelectSceneNode(self.ContextMenu.SceneNodeID);
        
        -- Undo/Redo point.
        self.SceneEditor:CommitStateStackFrame();
    end);
    
    
    function self:ShowContextMenu(item)
        self.ContextMenu.SceneNodeID = item.SceneNodeID;
        
        -- Some buttons might need to be disabled.
        if self.SceneEditor:GetParentSceneNodePtr(item.SceneNodeID) == nil then
            self.ContextMenu.Orphan:Disable();
        else
            self.ContextMenu.Orphan:Enable();
        end
        
        
        -- The position of the context menu will depend on whether or not it fits in the viewport.
        local xPos, yPos = GTGUI.Server.GetMousePosition();
        if (xPos + self.ContextMenu:GetWidth()) > GTGUI.Server.GetViewportWidth() then
            xPos = xPos - self.ContextMenu:GetWidth();
        end
        
        self.ContextMenu:SetPosition(xPos, yPos);
        self.ContextMenu:Show();
    end
    
    
    
    
    -- Sets the selected items to those of the given scene node ids. Deselects anything not in the list.
    function self:SetSelectedItemsBySceneNodeIDs(sceneNodeIDs)
        -- We're going to use a simple technique here where we just deselect everything and then reselect. Simple, intuitive and fast enough.
        
        -- 1) Deselect Everything.
        self.TreeView:DeselectAllItems(true);        -- 'true' = block OnDeselected events.
        
        -- 2) Reselect.
        for i,sceneNodeID in ipairs(sceneNodeIDs) do
            local treeViewItem = self.SceneNodes[sceneNodeID];
            if treeViewItem ~= nil then
                treeViewItem:Select(true);      -- 'true' = block OnSelected events.
            end
        end
    end
    
    
    function self:AddSceneNode(sceneNodePtr)
        local sceneNodeID       = GTEngine.System.SceneNode.GetID(sceneNodePtr);
        local parentSceneNodeID = GTEngine.System.SceneNode.GetID(GTEngine.System.SceneNode.GetParentPtr(self.SceneEditor:GetSceneNodePtrByID(sceneNodeID)));
        
        local item = self.SceneNodes[sceneNodeID];
        if item == nil then
            local sceneNodeName = GTEngine.System.SceneNode.GetName(sceneNodePtr);
            local parentItem    = self.SceneNodes[parentSceneNodeID];
            
            item = self.TreeView:AddItem(sceneNodeName or "Unnamed", parentItem);
            item.SceneNodeID   = sceneNodeID;
            item.SceneNodeName = sceneNodeName;
            
            if parentItem ~= nil then
                parentItem:Expand();
            end
            
            item.titleContainer:OnTear(function()
                -- We need to create an element that we'll use as the drag-and-drop element.
                local dragAndDropElement = GTGUI.Server.New("<div style='visible:false; positioning:absolute; z-index:100; opacity:50%; width:auto; padding:4px 2px; text-color:#ccc; font-style:bold; background-color:#666; vertical-align:center;' />");
                dragAndDropElement:SetText(item:GetText());
                dragAndDropElement:OnSize(function()
                    GTGUI.Server.SetDragAndDropProxyElementOffset(-(dragAndDropElement:GetWidth() / 2), -(dragAndDropElement:GetHeight() / 2));
                    dragAndDropElement:Show();
                end)
                dragAndDropElement:OnDragAndDropProxyRemoved(function()
                    GTGUI.Server.DeleteElement(dragAndDropElement);
                end)
                
                GTGUI.Server.SetDragAndDropProxyElement(dragAndDropElement);
                
                dragAndDropElement.sceneNodeID  = item.SceneNodeID;
                dragAndDropElement.sceneNodePtr = self.SceneEditor:GetSceneNodePtrByID(item.SceneNodeID);
            end);
            
            item.titleContainer:OnDrop(function(data)
                if data.droppedElement ~= nil and data.droppedElement.sceneNodeID ~= nil then
                    local parentSceneNodeID = item.SceneNodeID;
                    local childSceneNodeID  = data.droppedElement.sceneNodeID;
                    
                    if parentSceneNodeID ~= childSceneNodeID then
                        local parentSceneNodePtr = self.SceneEditor:GetSceneNodePtrByID(parentSceneNodeID);
                        local childSceneNodePtr  = self.SceneEditor:GetSceneNodePtrByID(childSceneNodeID);
                        
                        if parentSceneNodePtr ~= nil and childSceneNodePtr ~= nil and not GTEngine.System.SceneNode.IsAncestor(parentSceneNodePtr, childSceneNodePtr) then
                            GTEngine.System.SceneNode.AttachChild(parentSceneNodePtr, childSceneNodePtr, true);
                            self.SceneEditor:SelectSceneNode(childSceneNodeID);
                            
                            -- Undo/Redo point.
                            self.SceneEditor:CommitStateStackFrame();
                        end
                    end
                end
            end);
            
            item.titleContainer:OnDragAndDropEnter(function(data)
            end);
            
            item.titleContainer:OnDragAndDropLeave(function(data)
            end);
            
            item.titleContainer:OnRMBUp(function()
                self.TreeView:DeselectAllItemsExcept(item);
                item:Select();
                
                self:ShowContextMenu(item);
            end);
            
            
            item:OnSelected(function()
                self.SceneEditor:SelectSceneNode(item.SceneNodeID);
            end);
            
            item:OnDeselected(function()
                self.SceneEditor:DeselectSceneNode(item.SceneNodeID);
            end);
            
            
            self.SceneNodes[sceneNodeID] = item;
        else
            self:UpdateSceneNode(sceneNodePtr);
        end
        
        
        -- We'll select the item (blocking the OnSelected event) if the scene node is marked as selected.
        if self.SceneEditor:IsSceneNodeSelectedByID(sceneNodeID) then
            item:Select(true);      -- 'true' will block the OnSelected event which we don't want.
        end
        
        
        -- We need to make sure children are also added.
        local children = GTEngine.System.SceneNode.GetChildrenIDs(sceneNodePtr);
        if children ~= nil then
            for i,sceneNodeID in ipairs(children) do
                if sceneNodeID ~= 0 then
                    self:AddSceneNode(self.SceneEditor:GetSceneNodePtrByID(sceneNodeID));
                end
            end
        end
        
        
        return item;
    end
    
    function self:RemoveSceneNode(sceneNodePtr)
        local sceneNodeID = GTEngine.System.SceneNode.GetID(sceneNodePtr);
    
        -- Children will be removed first.
        local children = GTEngine.System.SceneNode.GetChildrenIDs(sceneNodePtr);
        if children ~= nil then
            for i,sceneNodeID in ipairs(children) do
                self:RemoveSceneNode(self.SceneEditor:GetSceneNodePtrByID(sceneNodeID));
            end
        end
    
        self.TreeView:RemoveItem(self.SceneNodes[sceneNodeID], true);       -- 'true' means to block the OnDeselected message.
        self.SceneNodes[sceneNodeID] = nil;
    end
    
    function self:UpdateSceneNode(sceneNodePtr)
        local sceneNodeID = GTEngine.System.SceneNode.GetID(sceneNodePtr);
        
        local item = self.SceneNodes[sceneNodeID];
        if item ~= nil then
            local sceneNodeName = GTEngine.System.SceneNode.GetName(sceneNodePtr);
            
            item.SceneNodeName = sceneNodeName
            item:SetText(sceneNodeName or "Unnamed");
        end
    end
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
    self.TabBar          = GTGUI.Server.New("<div parentid='" .. self:GetID()      .. "' styleclass='scene-editor-panel-tabbar'        style='' />");
    self.Body            = GTGUI.Server.New("<div parentid='" .. self:GetID()      .. "' styleclass='scene-editor-panel-body'          style='' />");
    self.PropertiesPanel = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='scene-editor-properties-panel'    style='visible:false' />");
    self.HierarchyPanel  = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='scene-editor-hierarchy-panel'     style='visible:false' />");
    self.ScenePropertiesPanel = GTGUI.Server.CreateElement(self.Body, "scene-editor-properties-panel");
    self.SceneEditor     = sceneEditor;
    
    
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
            self.HierarchyPanel.TreeView:UpdateScrollbars();
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
