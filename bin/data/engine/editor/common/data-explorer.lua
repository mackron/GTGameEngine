-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:DataExplorer()
    self.SearchBox  = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='searchbox' style='border-bottom:1px #2b2b2b; background-color:#555;' />");
    self.TreeView   = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='treeview'  style='height:100%; border:none; transparent-mouse-input:true;' />");
    self.FolderMenu = GTGUI.Server.New("<div                                   styleclass='menu'      style='z-index:100; positioning:absolute; visible:false' />");
    self.FileMenu   = GTGUI.Server.New("<div                                   styleclass='menu'      style='z-index:100; positioning:absolute; visible:false' />");
    
    self.SearchBox:SearchBox();
    self.TreeView:TreeView();
    
    self.FolderMenu:Menu();
    self.FolderMenu:EnableDefaultEvents();
    
    self.FolderMenu:AppendItem("New Folder..."):OnPressed(function()
        if self.FolderMenu.DestinationDirectory ~= nil then
            Editor.ShowNewFileDialog("Create New Folder...", self.FolderMenu.DestinationDirectory, nil, function(result, absolutePath)
                if result == Editor.NewFileDialogResult.Create then
                    GTCore.IO.CreateDirectory(absolutePath);
                    Game.ScanDataFilesForChanges();                 -- This will force the files watcher to update.
                    
                    self:SelectAndExpandItemByPath(absolutePath);
                end;
            end);
        end
    end);
    
    self.FolderMenu:AppendSeparator();
    
    self.FolderMenu:AppendItem("New Scene..."):OnPressed(function()
        if self.FolderMenu.DestinationDirectory ~= nil then
            Editor.ShowNewFileDialog("Create New Scene...", self.FolderMenu.DestinationDirectory, "gtscene", function(result, absolutePath)
                if result == Editor.NewFileDialogResult.Create then
                    GTCore.IO.CreateEmptyFile(absolutePath);
                    Game.ScanDataFilesForChanges();
                    
                    Editor.OpenFile(absolutePath, self.FolderMenu.DestinationDirectory);
                    self:SelectAndExpandItemByPath(absolutePath);
                end
            end);
        end
    end);
    
    self.FolderMenu:AppendItem("New Prefab..."):OnPressed(function()
        if self.FolderMenu.DestinationDirectory ~= nil then
            Editor.ShowNewFileDialog("Create New Prefab...", self.FolderMenu.DestinationDirectory, "gtprefab", function(result, absolutePath)
                if result == Editor.NewFileDialogResult.Create then
                    GTCore.IO.CreateEmptyFile(absolutePath);
                    Game.ScanDataFilesForChanges();
                end
            end);
        end
    end);
    
    self.FolderMenu:AppendItem("New Material..."):OnPressed(function()
        if self.FolderMenu.DestinationDirectory ~= nil then
            Editor.ShowNewFileDialog("Create New Material...", self.FolderMenu.DestinationDirectory, "material", function(result, absolutePath)
                if result == Editor.NewFileDialogResult.Create then
                    GTCore.IO.CreateEmptyFile(absolutePath);
                    Game.ScanDataFilesForChanges();
                    
                    Editor.OpenFile(absolutePath, self.FolderMenu.DestinationDirectory);
                    self:SelectAndExpandItemByPath(absolutePath);
                end
            end);
        end
    end);
    
    self.FolderMenu:AppendItem("New Particle System..."):OnPressed(function()
        if self.FolderMenu.DestinationDirectory ~= nil then
            Editor.ShowNewFileDialog("Create New Particle System...", self.FolderMenu.DestinationDirectory, "gtparticle", function(result, absolutePath)
                if result == Editor.NewFileDialogResult.Create then
                    GTCore.IO.CreateEmptyFile(absolutePath);
                    Game.ScanDataFilesForChanges();
                    
                    Editor.OpenFile(absolutePath, self.FolderMenu.DestinationDirectory);
                    self:SelectAndExpandItemByPath(absolutePath);
                end
            end);
        end
    end);
    
    self.FolderMenu:AppendItem("New Text File..."):OnPressed(function()
        if self.FolderMenu.DestinationDirectory ~= nil then
            Editor.ShowNewFileDialog("Create New Text File...", self.FolderMenu.DestinationDirectory, nil, function(result, absolutePath)
                if result == Editor.NewFileDialogResult.Create then
                    GTCore.IO.CreateEmptyFile(absolutePath);
                    Game.ScanDataFilesForChanges();
                    
                    Editor.OpenFile(absolutePath, self.FolderMenu.DestinationDirectory);
                    self:SelectAndExpandItemByPath(absolutePath);
                end
            end);
        end
    end);
    
    self.FolderMenu:AppendSeparator();
    self.FolderMenu:AppendItem("Delete..."):OnPressed(function()
        Editor.ShowYesNoDialog("Are you sure you want to delete '" .. self.FolderMenu.DestinationDirectoryShort .. "'?", function(result)
            if result == Editor.YesNoDialogResult.Yes then
                -- Recursively force close every file in the directory.
                self:ForceCloseFilesInDirectory(self:FindItemByPath(self.FolderMenu.DestinationDirectory));
            
                GTCore.IO.DeleteDirectory(self.FolderMenu.DestinationDirectory);
                Game.ScanDataFilesForChanges();
            end
        end);
    end);
    
    
    
    self.FileMenu:Menu();
    self.FileMenu:EnableDefaultEvents();
    
    self.FileMenu:AppendItem("Delete..."):OnPressed(function()
        Editor.ShowYesNoDialog("Are you sure you want to delete '" .. self.FileMenu.DestinationRelativePath .. "'?", function(result)
            if result == Editor.YesNoDialogResult.Yes then
                local absolutePathToDelete = self.FileMenu.DestinationAbsolutePath;
            
                Editor.ForceCloseFile(absolutePathToDelete);
                GTCore.IO.DeleteFile(absolutePathToDelete);
                
                -- If the file was a model, we need to look for a corresponding .gtmodel file. Also, if the file is a .gtmodel, we need to remove
                -- the extension and also try deleted the base file.
                if GTEngine.IsModelFile(absolutePathToDelete) then
                    if GTCore.IO.GetExtension(absolutePathToDelete) == "gtmodel" then
                        -- The file is a .gtmodel file. We need to remove the extension and delete the base file if it exists.
                        absolutePathToDelete = GTCore.IO.RemoveExtension(absolutePathToDelete);
                        if GTCore.IO.FileExists(absolutePathToDelete) then
                            GTCore.IO.DeleteFile(absolutePathToDelete);
                        end
                    else
                        -- The file is not a .gtmodel file. We need to append the .gtmodel extension and try deleting that file, too.
                        absolutePathToDelete = absolutePathToDelete .. ".gtmodel";
                        if GTCore.IO.FileExists(absolutePathToDelete) then
                            GTCore.IO.DeleteFile(absolutePathToDelete);
                        end
                    end
                end
                
                Game.ScanDataFilesForChanges();
            end
        end);
    end);
    
    
    
    function self:InsertItemFromFileInfo(fileInfo, text)
        -- The first step is to find the name of the parent.
        local parentPath = GTCore.IO.GetParentDirectoryPath(fileInfo.absolutePath);
        assert(parentPath ~= nil);
        
        -- Now we need to find the treeview item of the parent. Once we have this, we can look at the siblings and determine
        -- where to insert the new item.
        local parentItem = self:FindItemByPath(parentPath);

        
        if text == nil then
            text = GTCore.IO.GetFileNameFromPath(fileInfo.absolutePath);
            
            if GTCore.IO.GetExtension(text) == "gtmodel" then
                text = GTCore.IO.RemoveExtension(text);
            end
        end
        
        local item = self.TreeView:AddItem(text, parentItem);
        item.isDirectory  = fileInfo.isDirectory;
        item.path         = fileInfo.absolutePath;
        
        if item.isDirectory then
            item:ShowIcon('folder-icon');
        else
            item:ShowIcon('file-icon');
            item.icon:SetStyle('background-image-color', '#aaa');
        end
        
        
        -- Called when an item is right clicked.
        item.titleContainer:OnMouseButtonUp(function(data)
            if data.button == GTCore.MouseButtons.Right then
                self.TreeView:DeselectAllItems();
                item:Select();
                self:ShowContextMenu(item);
            end
        end)    
        
        -- Called when the item is torn for drag-and-drop.
        item.titleContainer:OnTear(function()
            if not item:GetParent().isRoot then
                -- We need to create an element that we'll use as the drag-and-drop element.
                local dragAndDropElement = GTGUI.Server.New("<div style='visible:false; positioning:absolute; z-index:100; opacity:50%; width:auto; padding:4px 2px; text-color:#ccc; font-weight:bold; background-color:#666; vertical-align:center;' />");
                dragAndDropElement:SetText(item:GetText());
                dragAndDropElement:OnSize(function()
                    GTGUI.Server.SetDragAndDropProxyElementOffset(-(dragAndDropElement:GetWidth() / 2), -(dragAndDropElement:GetHeight() / 2));
                    dragAndDropElement:Show();
                end)
                dragAndDropElement:OnDragAndDropProxyRemoved(function()
                    GTGUI.Server.DeleteElement(dragAndDropElement);
                end)
                
                GTGUI.Server.SetDragAndDropProxyElement(dragAndDropElement, 0, 0);
                
                dragAndDropElement.isAsset = true;
                dragAndDropElement.path    = item:GetShortPath();
            end
        end);
        
        item.titleContainer:OnDrop(function(data)
            if not item.isDirectory and GTEngine.IsPrefabFile(item.path) then
                if data.droppedElement.sceneNodePtr then
                    if data.droppedElement.sceneEditor then
                        data.droppedElement.sceneEditor:CreatePrefab(item.path, item:GetRootPath(), data.droppedElement.sceneNodePtr);
                    else
                        GTEngine.CreatePrefab(item.path, item:GetRootPath(), data.droppedElement.sceneNodePtr);
                        Game.ScanDataFilesForChanges();     -- This will force the data files watcher to update, which will in turn notify the scene of the changes to it can update any nodes linked to this prefab.
                    end
                end
            end
        end);
        
        
        function item:GetShortPath()
            local parent = item:GetParent();
            if parent and not parent:GetParent().isRoot then
                return parent:GetShortPath() .. "/" .. item:GetText();
            else
                return item:GetText();
            end
        end
        
        function item:GetRelativePath()
            return item:GetShortPath();
        end
        
        function item:GetRootPath()
            local parent = item:GetParent();
            if parent then
                if parent.isRoot then
                    return item.path;
                else
                    return parent:GetRootPath();
                end
            else
                return "";                      -- Should never actually get here...
            end
        end
    end
    
    function self:RemoveItemByPath(path)
        self.TreeView:RemoveItem(self:FindItemByPath(path));
    end
    
    function self:FindItemByPath(path, root)
        -- Brute force for now, but we should probably use the hierarchial nature of the file system for optimizing later.
        
        -- If the root was nil, we'll root at the tree-views root item.
        if root == nil then root = self.TreeView:GetRootItem(); end
        
        if root.path == path then
            return root;
        else
            local children = root:GetChildren();
            for key,value in pairs(children) do
                local item = self:FindItemByPath(path, value);
                if item ~= nil then
                    return item;
                end
            end
        end
        
        return nil;
    end
    
	
	function self:SelectAndExpandItemByPath(path)
		local item = self:FindItemByPath(path);
		if item then
			local parentItem = item:GetParent();
			while parentItem do
				parentItem:Expand();
				parentItem = parentItem:GetParent();
			end
			
			self.TreeView:DeselectAllItems();
			item:Select();
		end
	end
    
    
    function self:ForceCloseFilesInDirectory(directoryItem)
        local children = directoryItem:GetChildren();
        for key,childItem in pairs(children) do
            if childItem.isDirectory then
                self:ForceCloseFilesInDirectory(childItem);
            else
                Editor.ForceCloseFile(childItem.path);
            end
        end
    end
	
    
    function self:IsFileIgnored(fileInfo)
        -- Here we'll check the extensions.
        local filename  = GTCore.IO.GetFileNameFromPath(fileInfo.absolutePath);
        local extension = GTCore.IO.GetExtension(fileInfo.absolutePath);
        
        -- Blender will create backup .blend files named as .blend1, .blend2, etc. We want to ignore these.
        if string.sub(extension, 1, 5) == "blend" and string.len(extension) > 5 then
            return true;
        end
        
        if extension == "html" or extension == "HTML" then
            return true;
        end
        
        
        -- There are a few other files that we want to ignore such as fonts.cache.
        if filename == "fonts.cache" then
            return true;
        end
        

        -- If we've made it here, the file is not ignored and will show up.
        return false;
    end
    
    
    
    -- Shows the right-click context menu for the given item.
    function self:ShowContextMenu(item)
        if item.isDirectory then
            local mousePosX, mousePosY = GTGUI.Server.GetMousePosition();
            local xPos           = mousePosX;
            local yPos           = mousePosY;
            local width          = self.FolderMenu:GetWidth();
            local height         = self.FolderMenu:GetHeight();
            local viewportWidth  = GTGUI.Server.GetViewportWidth();
            local viewportHeight = GTGUI.Server.GetViewportHeight();
        
            if xPos + width > viewportWidth then
                xPos = xPos - width;
            end
            
            if yPos + height > viewportHeight then
                yPos = yPos - height;
            end
        
        
            self.FolderMenu:SetPosition(xPos, yPos);
            self.FolderMenu:Show();
            self.FolderMenu.DestinationDirectory = item.path;
            
            if not item:GetParent().isRoot then
                self.FolderMenu.DestinationDirectoryShort = item:GetRelativePath();
            else
                self.FolderMenu.DestinationDirectoryShort = "";
            end
        else
            local mousePosX, mousePosY = GTGUI.Server.GetMousePosition();
            local xPos           = mousePosX;
            local yPos           = mousePosY;
            local width          = self.FileMenu:GetWidth();
            local height         = self.FileMenu:GetHeight();
            local viewportWidth  = GTGUI.Server.GetViewportWidth();
            local viewportHeight = GTGUI.Server.GetViewportHeight();
        
            if xPos + width > viewportWidth then
                xPos = xPos - width;
            end
            
            if yPos + height > viewportHeight then
                yPos = yPos - height;
            end
        
        
            self.FileMenu:SetPosition(xPos, yPos);
            self.FileMenu:Show();
            self.FileMenu.DestinationAbsolutePath = item.path;
            
            if not item:GetParent().isRoot then
                self.FileMenu.DestinationRelativePath = item:GetRelativePath();
            else
                self.FileMenu.DestinationRelativePath = "";
            end
        end
    end
    
    
    
    function self:OnFileInsert(fileInfo)
        -- Some files should be ignored by the explorer. We're going to filter them
        if not self:IsFileIgnored(fileInfo) then
            -- If the file is a .gtmodel file, we actually want to handle this a little differently. Basically, we don't want to show them. However, there
            -- is a case where the original source file may not be present, in which case we still want to see an item for the file. What we do is remove
            -- the .gtmodel extension, leaving only the base name. Then, we check if a file of that new name already exists. If so, we just ignore everything,
            -- otherwise we add a file with that base name.
            if GTCore.IO.GetExtension(fileInfo.absolutePath) == "gtmodel" then
                local baseFileAbsolutePath = GTCore.IO.RemoveExtension(fileInfo.absolutePath);
                if GTCore.IO.FileExists(baseFileAbsolutePath) then
                    fileInfo.absolutePath = baseFileAbsolutePath;
                    return;
                end
            end
        
            self:InsertItemFromFileInfo(fileInfo);
        end
    end
    
    function self:OnFileRemove(fileInfo)
        self:RemoveItemByPath(fileInfo.absolutePath);
    end
    
    function self:OnFileUpdate(fileInfo)
        --print("Update: " .. fileInfo.absolutePath);
    end
    

    
    self.TreeView:OnItemPicked(function(data)
        if not data.item.isDirectory then
            Editor.OpenFile(data.item.path, data.item:GetRootPath());
        end
    end)
    
    
    
    
    for i,value in ipairs(Directories.Data) do
        local text = "(";
        if Editor.DirectoryAliases and Editor.DirectoryAliases.Data[i] then
            text = text .. Editor.DirectoryAliases.Data[i];
        else
            text = text .. GTCore.MakePathRelative(value, GTEngine.GetExecutableDirectory());
        end
        text = text .. ")";
        
        local fileInfo = GTCore.IO.FileInfo.New(value);
        self:InsertItemFromFileInfo(fileInfo, text);
    end
    
    -- Here we will register the explorer with the data files watcher.
    Editor.DataFilesWatcher.RegisterExplorer(self);
end
