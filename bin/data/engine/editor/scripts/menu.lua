-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- Takes the given element and turns it into a menu.
function GTGUI.Element:Menu()

    -- The list of items. This includes separators.
    self.items = {}
    self.isDefaultEventsEnabled = false;

    -- Appends a menu item.
    function self:AppendItem(title)
        local item = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='menu-item'>" .. title .. "</div>");
        item:MenuItem();
        
        item:OnPressed(function()
            if self.isDefaultEventsEnabled then
                self:Hide();
            end
        end);

        self.items[#self.items + 1] = item;

        return item;
    end

    -- Appends a separator.
    function self:AppendSeparator()
        local item = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='menu-separator-outer'><div styleclass='menu-separator-inner' /></div>");

        self.items[#self.items + 1] = item;

        return item;
    end


    -- Determines whether or not the menu is empty.
    function self:IsEmpty()
        return #self.items == 0;
    end
    
    
    
    function self:EnableDefaultEvents()
        self.isDefaultEventsEnabled = true;
    end

    function self:DisableDefaultEvents()
        self.isDefaultEventsEnabled = false;
    end
    
    
    function self.MouseDownEvent(data)
        if self.isDefaultEventsEnabled then
            if not self:IsChild(data.receiver) and data.receiver ~= GTGUI.Server.GetRootElement() then self:Hide() end
        end
    end
    
    
    self:WatchLMBDown(self.MouseDownEvent);
    self:WatchRMBDown(self.MouseDownEvent);
    self:WatchMMBDown(self.MouseDownEvent);
end


-- Takes the given element and turns it into a menu item.
function GTGUI.Element:MenuItem(title, parent)
end

-- Takes the given element and turns it into a meun separator.
function GTGUI.Element:MenuSeparator(parent)
end
