-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:DropDownBox()
    self.TitleContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                .. "' styleclass='dropdown-box-title-container'    />");
    self.TitleLabel     = GTGUI.Server.New("<div parentid='" .. self.TitleContainer:GetID() .. "' styleclass='dropdown-box-title-label'        />");
    self.TitleArrow     = GTGUI.Server.New("<div parentid='" .. self.TitleContainer:GetID() .. "' styleclass='dropdown-box-title-arrow'        />");
    self.ListBox        = GTGUI.Server.New("<div parentid='" .. self.TitleContainer:GetID() .. "' styleclass='dropdown-box-listbox'            />");
    self.BorderHider    = GTGUI.Server.New("<div parentid='" .. self.TitleContainer:GetID() .. "' styleclass='dropdown-box-title-border-hider' />");
    
    self.Locked       = false;
    self.Items        = {};
    self.SelectedItem = nil;
    
    
    function self:AppendItem(name)
        local newItem = GTGUI.Server.New("<div parentid='" .. self.ListBox:GetID() .. "' styleclass='dropdown-box-item' />");
        newItem:SetText(name);
        
        newItem:OnPressed(function(data)
            self:SelectItem(newItem);
            self:Close();
        end);
        
        
        self.Items[#self.Items + 1] = newItem;
        return newItem;
    end
    
    function self:RemoveItemByIndex(index)
        local itemToRemove = self.Items[index];
        if itemToRemove ~= nil then
            GTGUI.Server.DeleteElement(itemToRemove);
        end
        
        table.remove(self.Items, index);
    end
    
    function self:RemoveItem(itemToRemove)
        self:RemoveItemByIndex(table.indexof(self.Items, itemToRemove));
    end
    
    
    
    function self:SelectItemByIndex(index, blockEvent)
        local itemToSelect = self.Items[index];
        if itemToSelect ~= nil and itemToSelect ~= self.SelectedItem then
            self.TitleLabel:SetText(itemToSelect:GetText());
            self.SelectedItem = itemToSelect;
            
            if not blockEvent then
                self:OnSelectionChanged({selectedItem = self.SelectedItem});
            end
        end
    end
    
    function self:SelectItem(itemToSelect, blockEvent)
        self:SelectItemByIndex(table.indexof(self.Items, itemToSelect), blockEvent);
    end
    
    
    
    function self:Open()
        self.ListBox:Show();
        self.BorderHider:Show();
        self.TitleContainer:AttachStyleClass("dropdown-box-title-container-opened");
    end
    
    function self:Close()
        self.ListBox:Hide();
        self.BorderHider:Hide();
        self.TitleContainer:DetachStyleClass("dropdown-box-title-container-opened");
    end
    
    function self:IsOpen()
        return self.ListBox:IsVisible();
    end
    
    
    
    self:OnLMBDown(function(data)
        if not self:IsOpen() then
            self:Open();
        else
            self:Close();
        end
    end);
    
    self:WatchLMBDown(function(data)
        if self:IsOpen() then
            if data.receiver ~= self and not self:IsChild(data.receiver) then
                self:Close();
            end
        end
    end);
    
    self:WatchRMBDown(function(data)
        if self:IsOpen() then
            if data.receiver ~= self and not self:IsChild(data.receiver) then
                self:Close();
            end
        end
    end);
    
    self:WatchMMBDown(function(data)
        if self:IsOpen() then
            if data.receiver ~= self and not self:IsChild(data.receiver) then
                self:Close();
            end
        end
    end);
    
    
    function self:OnSelectionChanged(arg1)
        self.Locked = true;
            self.Callbacks:BindOrCall("OnSelectionChanged", arg1);
        self.Locked = false;
    end
    

    return self;
end