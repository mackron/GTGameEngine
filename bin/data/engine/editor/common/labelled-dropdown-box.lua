-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:LabelledDropDownBox(title)
    self.Left        = GTGUI.Server.New("<div parentid='" .. self:GetID()       .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.Right       = GTGUI.Server.New("<div parentid='" .. self:GetID()       .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.Label       = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' styleclass='labelled-dropdown-box-label'        />");
    self.DropDownBox = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' styleclass='labelled-dropdown-box-dropdown-box' />");
    self.DropDownBox:DropDownBox();
    
    self.Locked = false;
    
    
    function self:AppendItem(name)
        return self.DropDownBox:AppendItem(name);
    end
    
    function self:RemoveItemByIndex(index)
        self.DropDownBox:RemoveItemByIndex(index);
    end
    
    function self:RemoveItem(itemToRemove)
        self.DropDownBox:RemoveItem(itemToRemove);
    end
    
    function self:SelectItemByIndex(index)
        self.DropDownBox:SelectItemByIndex(index);
    end
    
    function self:SelectItem(itemToSelect)
        self.DropDownBox:SelectItem(itemToSelect);
    end
    
    
    
    function self:OnSelectionChanged(arg1)
        self.Locked = true;
            self.Callbacks:BindOrCall("OnSelectionChanged", arg1);
        self.Locked = false;
    end
    
    
    
    self.DropDownBox:OnSelectionChanged(function(data)
        self:OnSelectionChanged(data);
    end);
    
    
    
    self.Label:SetText(title);
    
    return self;
end