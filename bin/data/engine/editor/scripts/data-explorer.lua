
function GTGUI.Element:DataExplorer()
    self.SearchBox = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='searchbox' style='border-bottom:1px #2b2b2b; background-color:#555;' />");
    self.TreeView  = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='treeview'  style='height:100%; border:none; transparent-mouse-input:true;' />");
    
    self.SearchBox:SearchBox();
    self.TreeView:TreeView();
    
    function self:InsertItemFromFileInfo(fileInfo, text)
        -- The first step is to find the name of the parent.
        local parentPath = GTCore.IO.GetParentDirectoryPath(fileInfo.absolutePath);
        assert(parentPath ~= nil);
        
        -- Now we need to find the treeview item of the parent. Once we have this, we can look at the siblings and determine
        -- where to insert the new item.
        local parentItem = self:FindItemByPath(parentPath);

        
        if text == nil then
            text = GTCore.IO.GetFileNameFromPath(fileInfo.absolutePath);
        end
        
        local item = self.TreeView:AddItem(text, parentItem);
        item.isDirectory = fileInfo.isDirectory;
        item.path        = fileInfo.absolutePath;
        
        if item.isDirectory then
            item:ShowIcon('folder-icon');
        else
            item:ShowIcon('file-icon');
            item.icon:SetStyle('background-image-color', '#aaa');
        end
        
        item.titleContainer:OnTear(function()
            if not item:GetParent().isRoot then
                -- We need to create an element that we'll use as the drag-and-drop element.
                local dragAndDropElement = GTGUI.Server.New("<div style='visible:false; positioning:absolute; z-index:100; opacity:50%; width:auto; padding:4px 2px; text-color:#ccc; font-style:bold; background-color:#666; vertical-align:center;' />");
                dragAndDropElement:SetText(item:GetText());
                dragAndDropElement:OnSize(function()
                    GTGUI.Server.AttachElementToMouse(dragAndDropElement, -(dragAndDropElement:GetWidth() / 2), -(dragAndDropElement:GetHeight() / 2));
                    dragAndDropElement:Show();
                end)
                dragAndDropElement:WatchLMBUp(function()
                    GTGUI.Server.DeleteElement(dragAndDropElement);
                end)
                
                dragAndDropElement.isAsset = true;
                dragAndDropElement.path    = item:GetShortPath();
            end
        end)
        
        
        function item:GetShortPath()
            local parent = item:GetParent();
            if parent and not parent:GetParent().isRoot then
                return parent:GetShortPath() .. "/" .. item:GetText();
            else
                return item:GetText();
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
    
    
    function self:OnFileInsert(fileInfo)
        -- Some files should be ignored by the explorer. We're going to filter them
        if not self:IsFileIgnored(fileInfo) then
            -- If the file is a .gtmodel file, we actually want to handle this a little differently. Basically, we don't want to show them. However, there
            -- is a case where the original source file may not be present, in which case we still want to see an item for the file. What we do is remove
            -- the .gtmodel extension, leaving only the base name. Then, we check if a file of that new name already exists. If so, we just ignore everything,
            -- otherwise we add a file with that base name.
            if GTCore.IO.GetExtension(fileInfo.absolutePath) == "gtmodel" then
                fileInfo.absolutePath = GTCore.IO.RemoveExtension(fileInfo.absolutePath);
                if GTCore.IO.FileExists(fileInfo.absolutePath) then
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
            -- We need to activate the item's tab.
            local tab = Editor_TabBar.OpenTabs[data.item.path];
            if tab == nil then
                tab = Editor_TabBar:AddTab(data.item:GetText());
                tab.path = data.item.path;
                
                Editor_TabBar.OpenTabs[data.item.path] = tab;
            end
            
            Editor_TabBar:ActivateTab(tab);
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
        
        local fileInfo = GTCore.IO.FileInfo:New(value);
        self:InsertItemFromFileInfo(fileInfo, text);
    end
    
    -- Here we will register the explorer with the data files watcher.
    Editor.DataFilesWatcher.RegisterExplorer(self);
end