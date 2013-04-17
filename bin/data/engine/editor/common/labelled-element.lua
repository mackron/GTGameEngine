-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- Base class for simple input elements that have a label attached to them.
function GTGUI.Element:LabelledElement(labelText)
    self.Left  = GTGUI.Server.CreateElement(self:GetID(),      "labelled-element-left");
    self.Right = GTGUI.Server.CreateElement(self:GetID(),      "labelled-element-right");
    self.Label = GTGUI.Server.CreateElement(self.Left:GetID(), "labelled-element-label");
    self.Label:SetText(labelText);
    
    
    return self;
end