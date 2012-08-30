
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
    
    
    function self:OnFileInsert(fileInfo)
        self:InsertItemFromFileInfo(fileInfo);
    end
    
    function self:OnFileRemove(fileInfo)
        self:RemoveItemByPath(fileInfo.absolutePath);
    end
    
    function self:OnFileUpdate(fileInfo)
        print("Update: " .. fileInfo.absolutePath);
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
end