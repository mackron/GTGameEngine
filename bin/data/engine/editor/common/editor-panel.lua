-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- A panel has two parts - a body and scrollbar. The scrollbar will be resized automatically. Content should be
-- placed in the body.
function GTGUI.Element:EditorPanel()
    self.IsMouseOver           = false;
    self.MouseWheelScrollSpeed = 32;


    self.Body = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='editor-panel-body'      />");
    
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='editor-panel-scrollbar' />");
    self.Scrollbar:VerticalScrollbar();
    self.Scrollbar:EnableAutoVisibility();
    self.Scrollbar:SetPageSize(self:GetHeight());
    self.Scrollbar:OnScroll(function(data)
        local offset = -data.position;
        self.Body:SetStyle("inner-offset-y", tostring(offset));
    end);
    
    
    
    self:OnSize(function()
        if self:IsVisible() then
            self.Scrollbar:SetPageSize(self:GetHeight());
        end
    end);
    
    self.Body:OnSize(function()
        if self.Body:IsVisible() then
            self.Scrollbar:SetRange(self.Body:GetHeight());
        end
    end)
    
    self:OnMouseEnter(function()
        self.IsMouseOver = true;
    end);
    
    self:OnMouseLeave(function()
        self.IsMouseOver = false;
    end);
    
    self:WatchMouseWheel(function(data)
        if self.IsMouseOver then
            self.Scrollbar:Scroll(-data.delta * self.MouseWheelScrollSpeed);
        end
    end);
    
    
    return self;
end