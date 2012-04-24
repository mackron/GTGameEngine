
-- Takes the given element and turns it into a combo box.
function Server.Element:ComboBox()
   
    -- The title element.
    self.TitleElement = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='combobox-title' />");

    -- The element containing the arrow graphic.
    self.ArrowElement = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='combobox-arrow' />");

    -- The container for the item list.
    self.MenuElement = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='combobox-menu' />");


    -- The selected element.
    self.SelectedItem = nil;

    -- The list of items in the combo box's list.
    self.ListItems = {};


    -- Appends an item to the combo box's list. If this is the first item, it will be set as the title element.
    function self:AppendItem(title)
        local item = ComboBoxMenuItem(title, self.MenuElement, self);
        self.ListItems[#self.ListItems + 1] = item;

        if #self.ListItems == 1 then
            self:SelectItem(item);
        end
        
        return item;
    end

    -- Selects the given item.
    function self:SelectItem(item)
        -- We don't do anything if the input item is alreaddy selected...
        if self.SelectedItem ~= item then
            self:DeselectCurrentSelection();
            
            self.SelectedItem = item;

            self.TitleElement:SetText(self.SelectedItem:GetText());
        end

        -- When an item is selected we want the combo box to collapse.
        self:Collapse();
    end
    
    -- Desselects the currently selected item.
    function self:DeselectCurrentSelection()
        if self.SelectedItem ~= nil then
            self.SelectedItem = nil;
        end
    end

    -- Expands the combo box (shows the menu/list)
    function self:Expand()
        if not self.MenuElement:IsVisible() then
            self.MenuElement:Show();
            self:AttachStyleClass("combobox-expanded");
        end
    end

    -- Collapses the combo box (hides the menu/list)
    function self:Collapse()
        if self.MenuElement:IsVisible() then
            self.MenuElement:Hide();
            self:DetachStyleClass("combobox-expanded");
        end
    end


    -- Called when the mouse is clicked on the element.
    function self:OnPush()
        if self.MenuElement:IsVisible() then
            self:Collapse();
        else
            self:Expand();
        end
    end

    -- Called when the left mouse button is pressed somewhere.
    self:WatchLMBDown(function(data)
        if self.MenuElement:IsVisible() and data.receiver ~= self and self:IsChild(data.receiver) == false then
            self:Collapse();
        end
    end)

end


function ComboBoxMenuItem(title, parent, combobox)
    local item = Server.New("<div parentid='" .. parent:GetID() .. "' styleclass='combobox-menu-item'>" .. title .. "</div>");

    item.title    = title;
    item.combobox = combobox;
    
    function item:OnPush()
        item.combobox:SelectItem(self);
    end
    
    return item;
end
