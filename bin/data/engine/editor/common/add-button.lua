-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- A simple button with a plus symbol on the left side.
function GTGUI.Element:AddButton(title)
    self.Icon  = GTGUI.Server.CreateElement(self, "add-button-icon");
    self.Label = GTGUI.Server.CreateElement(self, "add-button-label");
    self.Label:SetText(title);
    
    return self;
end