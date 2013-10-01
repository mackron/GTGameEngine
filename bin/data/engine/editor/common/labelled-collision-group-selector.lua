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
    



    function self:RemoveAllSelectedGroups()
        self.SelectedItemsContainer:DeleteAllChildren();
    end

    function self:AddSelectedGroup(name)
        local newItem = GTGUI.Server.CreateElement(self.SelectedItemsContainer, "labelled-collision-group-selector-item");
        newItem:LabelledCollisionGroupSelectorItem(name);

        newItem.Cross:OnPressed(function()
            GTGUI.Server.DeleteElement(newItem);
        end);

        
        return newItem;
    end


    return self;
end
