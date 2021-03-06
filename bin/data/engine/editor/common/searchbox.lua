-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SearchBox()
    self.TextBox = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='searchbox-textbox'>Search...</div>");
    self.Icon    = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='searchbox-icon' />");
    
    self.HasContent = false;
    self.TextBox:SetStyle("font-slant", "italic");
    
    self.TextBox:OnFocus(function()
        if not self.TextBox.HasContent then
            self.TextBox:SetText("");
        end
        
        self.TextBox:SetStyle("font-slant", "default");
        self.Icon:SetStyle("background-image-color", "#aaa");
    end);
    
    self.TextBox:OnBlur(function()
        local text = self.TextBox:GetText();
        if text == nil or text == "" then
            self.TextBox.HasContent = false;
            self.TextBox:SetStyle("font-slant", "italic");
            self.TextBox:SetText("Search...");
        else
            self.TextBox.HasContent = true;
        end;
        
        self.Icon:SetStyle("background-image-color", "#888");
    end);
end
