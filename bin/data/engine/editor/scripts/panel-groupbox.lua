-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:PanelGroupBoxTitle(text, showCross)
    self.Arrow        = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-title-arrow' />");
    self.Text         = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-title-text'  />");
    
    self.Text:SetText(text);
    
    if showCross then
        self.Cross = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-title-cross' />");
        
        self.Cross:OnPressed(function()
            self.Parent:OnClose();
        end)
    end
    
    
    
    self:OnMouseEnter(function()
        self.Arrow:SetStyle("background-image-color", "#ddd");
        self.Text:SetStyle("text-color", "#ddd");
    end)
    
    self:OnMouseLeave(function()
        self.Arrow:SetStyle("background-image-color", "#bbb");
        self.Text:SetStyle("text-color", "#bbb");
    end)
    
    self:OnPressed(function()
        if self.Parent:IsCollapsed() then self.Parent:Expand() else self.Parent:Collapse() end;
    end)
end

function GTGUI.Element:PanelGroupBox(titleText, showCross)
    self.Title        = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-title'     />");
    self.Body         = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-body'      />");
    self.BottomBorder = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-separator' />");
    
    self.isCollapsed = false;
    
    self.Title:PanelGroupBoxTitle(titleText, showCross);
    
    
    function self:Expand()
        if self:IsCollapsed() then
            self.Title.Arrow:SetStyle("background-image", "std-arrow-down");
            self.Body:Show();
            self.isCollapsed = false;
        end
    end
    
    function self:Collapse()
        if not self:IsCollapsed() then
            self.Title.Arrow:SetStyle("background-image", "std-arrow-right");
            self.Body:Hide();
            self.isCollapsed = true;
        end
    end
    
    function self:IsCollapsed()
        return self.isCollapsed;
    end
    
    
    function self:OnClose(arg1)
        self.Callbacks:BindOrCall("OnClose", arg1);
    end
end