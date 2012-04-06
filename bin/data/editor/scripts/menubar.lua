
-- Takes the given element and turns it into a menu bar. Note this is only the bar itself and it's buttons and not the actual sub-menus. Instead you attach
-- menus to the menu bar items.
function Server.Element:MenuBar()
    
    -- The list of menu bar items.
    self.items = {};

    -- The currently selected item.
    self.selectedItem = nil;


    -- Attaches a new menu bar item.
    function self:AppendItem(title)
        local item = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='menubar-item'>" .. title .. "</div>");
        item:MenuBarItem(title, self);

        self.items[#self.items + 1] = item;

        return item;
    end

    -- Activates the menu of the given item.
    function self:ExpandMenu(item)
        if self.selectedItem ~= item then
            self:CollapseMenu();
        end

        if item.menu:IsEmpty() == false then
            item.menu:Show();
            item.borderMask:Show();
            item:AttachStyleClass("menubar-item-active");

            self.selectedItem = item;
        end
    end

    -- Collapses the currently expanded menu.
    function self:CollapseMenu()
        if self.selectedItem ~= nil then
            self.selectedItem.menu:Hide();
            self.selectedItem.borderMask:Hide();
            self.selectedItem:DetachStyleClass("menubar-item-active");

            self.selectedItem = nil;
        end
    end


    -- This is called by the items when the mouse enters them.
    function self:OnMouseEnterItem(item)
        -- If we have an expanded menu, it means the menu is "active". In this case we expand the menu of the item
        -- that just had the mouse enter.
        if self.selectedItem ~= nil and self.selectedItem ~= item then
            self:ExpandMenu(item);
        end
    end


    -- Here is where we watch for mouse-down events. We use these in determining when the menu should be collapsed.
    self:WatchLMBDown(function(data)
        if self.selectedItem ~= nil and self:IsChild(data.receiver) == false then
            self:CollapseMenu();
        end
    end)
end


-- Takes the given element and turns it into a menu bar item.
function Server.Element:MenuBarItem(title, parent)

    -- The title element.
    self.title = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='menubar-item-title' />");

    -- The menu associated with the item. Do not set this directly. Instead, use SetMenu().
    self.menu = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='menu' style='visible:false; position-origin:outer; top:100%; left:0px;' />");
    self.menu:Menu();

    -- This sneaky little element is going to mask out a section of the border created by the menu element.
    self.borderMask = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='menubar-item-border-mask' />");

    -- Handles cases when the mouse is clicked on the item.
    function self:OnPush()
        self.Parent:ExpandMenu(self);
    end

    -- Handles cases when the mouse enters the element.
    function self:OnMouseEnter()
        self.Parent:OnMouseEnterItem(self);
    end

    -- Handles cases when the mouse leaves the element.
    function self:OnMouseLeave()
    end
end

