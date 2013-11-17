-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:PickingDropDownBox(titleText)
    self.Icon    = GTGUI.Server.CreateElement(self:GetID(), "picking-dropdown-box-icon");
    self.Label   = GTGUI.Server.CreateElement(self:GetID(), "picking-dropdown-box-label");
    self.ListBox = GTGUI.Server.CreateElement(self:GetID(), "picking-dropdown-box-listbox");
    
    self.Locked  = false;
    self.Items   = {};
    
    
    function self:AppendItem(name)
        local newItem = GTGUI.Server.CreateElement(self.ListBox:GetID(), "picking-dropdown-box-listbox-item");
        newItem:SetText(name);
        
        newItem:OnPressed(function(data)
            self:PickItem(newItem);
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
    
    
    function self:PickItemByIndex(index)
        local pickedItem = self.Items[index];
        if pickedItem ~= nil and pickedItem ~= self.SelectedItem then
            self:OnItemPicked({item = pickedItem});
        end
    end
    
    function self:PickItem(pickedItem)
        self:PickItemByIndex(table.indexof(self.Items, pickedItem));
    end
    
    
    
    function self:Open()
        -- The list box needs to be positioned in a way where it's always visible. For example, if it would be clipped if positioned
        -- below the label, it will instead be positioned above it.
        local xPos, yPos     = self:GetAbsolutePosition();
        local listWidth      = self.ListBox:GetWidth();
        local listHeight     = self.ListBox:GetHeight();
        local viewportWidth  = GTGUI.Server.GetViewportWidth();
        local viewportHeight = GTGUI.Server.GetViewportHeight();
        
        if (xPos + listWidth) > viewportWidth and listWidth > self:GetWidth() then
            self.ListBox:SetStyle("left", self:GetWidth() - listWidth);
        else
            self.ListBox:SetStyle("left", 0);
        end
        
        if (yPos + listHeight + self:GetHeight()) > viewportHeight then
            self.ListBox:SetStyle("top", self:GetHeight() - listHeight - self:GetHeight());
        else
            self.ListBox:SetStyle("top", "100%");
        end

        
        self.ListBox:Show();
        self:AttachStyleClass("picking-dropdown-box-opened");
    end
    
    function self:Close()
        self.ListBox:Hide();
        self:DetachStyleClass("picking-dropdown-box-opened");
    end
    
    function self:IsOpen()
        return self.ListBox:IsVisible();
    end
    
    

    self:OnMouseButtonDown(function(data)
        if not self:IsOpen() then
            self:Open();
        else
            self:Close();
        end
    end)    

    self:WatchMouseButtonDown(function(data)
        if self:IsOpen() then
            if data.receiver ~= self and not self:IsChild(data.receiver) then
                self:Close();
            end
        end
    end)
    
    function self:OnItemPicked(arg1)
        self.Locked = true;
            self.Callbacks:BindOrCall("OnItemPicked", arg1);
        self.Locked = false;
    end
    
    
    -- Need to set the initial title.
    self.Label:SetText(titleText);
    
    return self;
end
