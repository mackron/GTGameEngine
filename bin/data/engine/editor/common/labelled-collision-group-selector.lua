-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:LabelledCollisionGroupSelectorItem(name)
    self.Label = GTGUI.Server.CreateElement(self, "labelled-collision-group-selector-item-label");
    self.Cross = GTGUI.Server.CreateElement(self, "labelled-collision-group-selector-item-cross");

    self.Label:SetText(name);

    return self;
end

function GTGUI.Element:LabelledCollisionGroupSelector(title)
    self:LabelledElement(title);

    -- The right side contains a list of labels with delete buttons on the right side of them. Below these
    -- is a drop-down box for the user to pick from.
    self.SelectedItemsContainer = GTGUI.Server.CreateElement(self.Right);
    self.SelectedItemsContainer:SetStyle("horizontal-align", "right");

    self.DropDownBox = GTGUI.Server.CreateElement(self.Right, "picking-dropdown-box");
    self.DropDownBox:PickingDropDownBox("Add");
    self.DropDownBox:SetStyle("margin-top", "4px");

    self.DropDownBox:AppendItem("All"):OnPressed(function()
        -- This is a special option. No other option can be used with this one.
        self:RemoveAllSelectedGroups();
        self:AddSelectedGroup("All");
    end);

    self.DropDownBox:AppendItem("None"):OnPressed(function()
        -- This is a special option. No other option can be used with this one.
        self:RemoveAllSelectedGroups();
        self:AddSelectedGroup("None");
    end);
    
    -- We now want to loop over each collision group in Game.CollisionGroup and add it to the list.
    if Game.CollisionGroups then
        for key,value in pairs(Game.CollisionGroups) do
            if key ~= "All" and key ~= "None" then
                self.DropDownBox:AppendItem(key):OnPressed(function()
                    self:AddSelectedGroup(key);
                end);
            end
        end
    else
        self.DropDownBox:AppendItem("No Groups");
    end


    function self:RemoveAllSelectedGroups()
        self.SelectedItemsContainer:DeleteAllChildren();
    end

    function self:RemoveSelectedGroupByName(name)
        GTGUI.Server.DeleteElement(self:GetSelectedGroupByName(name));
    end

    function self:AddSelectedGroup(name)
        -- If we already have a selection of the same name, we just return the existing one instead of re-adding it.
        local existingItem = self:GetSelectedGroupByName(name);
        if existingItem then
            return existingItem;
        end

        -- If we have an "All" or "None" group, that also needs to be deleted.
        self:RemoveSelectedGroupByName("All");
        self:RemoveSelectedGroupByName("None");

        local newItem = GTGUI.Server.CreateElement(self.SelectedItemsContainer, "labelled-collision-group-selector-item");
        newItem:LabelledCollisionGroupSelectorItem(name);

        newItem.Cross:OnPressed(function()
            GTGUI.Server.DeleteElement(newItem);
        end);

        
        return newItem;
    end

    function self:GetSelectedGroupByName(name)
        for id,element in pairs(self.SelectedItemsContainer.Children) do
            if element.Label and element.Label:GetText() == name then
                return element;
            end
        end

        return nil;
    end

    return self;
end
