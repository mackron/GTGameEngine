-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

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
    
    self.ContextMenu.Delete = self.ContextMenu:AppendItem("Delete"):OnPressed(function()
        self.SceneEditor:DeleteSceneNode(self.ContextMenu.SceneNodeID);
    end);
    
    
    self.ContextMenu:AppendSeparator();
    
    self.ContextMenu.Orphan = self.ContextMenu:AppendItem("Orphan"):OnPressed(function()
        self.SceneEditor:OrphanSceneNode(self.ContextMenu.SceneNodeID);
        self.SceneEditor:SelectSceneNode(self.ContextMenu.SceneNodeID);
        self.SceneEditor:PushUndoRedoPoint();
    end);
    
    self.ContextMenu:AppendSeparator();
    
    self.ContextMenu.UnlinkFromPrefab = self.ContextMenu:AppendItem("Unlink from Prefab"):OnPressed(function()
        self.SceneEditor:UnlinkSceneNodeFromPrefab(self.ContextMenu.SceneNodeID);
        self.SceneEditor:PushUndoRedoPoint();
    end);
    
    
    
    function self:ShowContextMenu(item)
        self.ContextMenu.SceneNodeID = item.SceneNodeID;
        
        -- Some buttons might need to be disabled.
        if self.SceneEditor:GetParentSceneNodePtr(item.SceneNodeID) ~= nil then
            self.ContextMenu.Orphan:Enable();
        else
            self.ContextMenu.Orphan:Disable();
        end
        
        if item.PrefabRelativePath then
            self.ContextMenu.UnlinkFromPrefab:Enable();
        else
            self.ContextMenu.UnlinkFromPrefab:Disable();
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
                self:ApplyPrefabStyling(treeViewItem);
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
            
            local prefabComponent = GTEngine.System.SceneNode.GetComponent(sceneNodePtr, GTEngine.Components.Prefab);
            if prefabComponent then
                item.PrefabRelativePath = prefabComponent:GetPrefabRelativePath();
            end
            
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
                self:ApplyPrefabStyling(item);
            end);
            
            item:OnDeselected(function()
                self.SceneEditor:DeselectSceneNode(item.SceneNodeID);
                self:ApplyPrefabStyling(item);
            end);
            
            self:ApplyPrefabStyling(item, sceneNodePtr);
            
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
    
    function self:ApplyPrefabStyling(treeViewItem)
        treeViewItem.titleContainer:DetachStyleClass("scene-editor-hierarchy-panel-item-prefab");
        treeViewItem.titleContainer:DetachStyleClass("scene-editor-hierarchy-panel-item-prefab-selected");
    
        if treeViewItem.PrefabRelativePath then
            treeViewItem.titleContainer:AttachStyleClass("scene-editor-hierarchy-panel-item-prefab");
            
            if treeViewItem:IsSelected() then
                treeViewItem.titleContainer:AttachStyleClass("scene-editor-hierarchy-panel-item-prefab-selected");
            end
        end
    end
    
    function self:UpdateSceneNode(sceneNodePtr)
        local sceneNodeID = GTEngine.System.SceneNode.GetID(sceneNodePtr);
        
        local item = self.SceneNodes[sceneNodeID];
        if item ~= nil then
            local sceneNodeName = GTEngine.System.SceneNode.GetName(sceneNodePtr);
            
            item.SceneNodeName = sceneNodeName
            item:SetText(sceneNodeName or "Unnamed");
            
            local prefabComponent = GTEngine.System.SceneNode.GetComponent(GTEngine.Components.Prefab);
            if prefabComponent then
                item.PrefabRelativePath = prefabComponent:GetComponentRelativePath();
            else
                item.PrefabRelativePath = nil;
            end
            
            self:ApplyPrefabStyling();
        end
    end
end