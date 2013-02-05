-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SearchBox()
    self.TextBox = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='searchbox-textbox'>Search...</div>");
    self.Icon    = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='searchbox-icon' />");
    
    self.HasContent = false;
    self.TextBox:SetStyle("font-style", "italic");
    
    self.TextBox:OnFocus(function()
        if not self.TextBox.HasContent then
            self.TextBox:SetText("");
        end
        
        self.TextBox:SetStyle("font-style", "none");
        self.Icon:SetStyle("background-image-color", "#aaa");
    end);
    
    self.TextBox:OnBlur(function()
        if not self.TextBox:GetText() then
            self.TextBox.HasContent = false;
            self.TextBox:SetStyle("font-style", "italic");
            self.TextBox:SetText("Search...");
        else
            self.TextBox.HasContent = true;
        end;
        
        self.Icon:SetStyle("background-image-color", "#888");
    end);
end