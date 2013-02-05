-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- Takes the given element and turns it into a menu.
function GTGUI.Element:Menu()

    -- The list of items. This includes separators.
    self.items = {}


    -- Appends a menu item.
    function self:AppendItem(title)
        local item = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='menu-item'>" .. title .. "</div>");
        item:MenuItem();

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

end


-- Takes the given element and turns it into a menu item.
function GTGUI.Element:MenuItem(title, parent)
end

-- Takes the given element and turns it into a meun separator.
function GTGUI.Element:MenuSeparator(parent)
end
