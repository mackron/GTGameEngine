-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- This file is used to create a list box from a GUI element. You create a list box with the
-- ListBox() function, which will take the element that is being turned into a list box.

-- Takes the given element and turns it into a list box.
function GTGUI.Element:ListBox()
    
    -- First we'll create some attributes...
    
    -- The list of items in the list box. The values of each item is an element object with additional
    -- attributes. The title of the item is the 'title' attribute of the table.
    self.ListItems    = {};
    self.SelectedItem = nil;
    
    -- And now we'll create some methods...
    
    -- Appends an item to the end of the list box. Returns the item object.
    function self:AppendItem(title)
        local item = ListBoxItem(title, self);
        self.ListItems[#self.ListItems + 1] = item;
        
        return item;
    end
    
    -- Removes an item. When an item is removed, it is completely deleted.
    function self:RemoveItem(item)
        self:RemoveItemByIndex(table.indexof(item));  -- table.indexof() is NOT standard. It is implemented via GTCore.
    end
    
    -- Removes an item based on it's index.
    function self:RemoveItemByIndex(index)
        local itemToDelete = self.ListItems[index];
        
        if itemToDelete == self.SelectedItem then
            self:DeselectCurrentSelection();
        end
        
        itemToDelete:Delete();
        table.remove(self.ListItems, index);
    end
    
    -- Removes every item.
    function self:RemoveAllItems()
        self:DeselectCurrentSelection();
        
        while #self.ListItems > 0 do
            self:RemoveItemByIndex(1);
        end
    end
    
    
    -- Selects the given item.
    function self:SelectItem(item)
        -- We don't do anything if the input item is alreaddy selected...
        if self.SelectedItem ~= item then
            self:DeselectCurrentSelection();
            
            self.SelectedItem = item;
            self.SelectedItem:AttachStyleClass("listbox-item-selected");
        end
    end
    
    -- Desselects the currently selected item.
    function self:DeselectCurrentSelection()
        if self.SelectedItem ~= nil then
            self.SelectedItem:DetachStyleClass("listbox-item-selected");
            self.SelectedItem = nil;
        end
    end
end


function ListBoxItem(title, parent)
    local item = GTGUI.Server.New("<div parentid='" .. parent:GetID() .. "' styleclass='listbox-item'>" .. title .. "</div>");
    item.title = title;
    
    function item:OnPush()
        item.Parent:SelectItem(self);
    end
    
    return item;
end
















