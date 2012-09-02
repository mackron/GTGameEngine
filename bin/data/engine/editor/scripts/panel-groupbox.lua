
function GTGUI.Element:PanelGroupBoxTitle(text)
    self.Arrow = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-title-arrow' />");
    self.Text  = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-title-text' />");
    
    self.Text:SetText(text);
    
    
    self:OnMouseEnter(function()
        self.Arrow:SetStyle("background-image-color", "#aaa");
        self.Text:SetStyle("text-color", "#aaa");
    end)
    
    self:OnMouseLeave(function()
        self.Arrow:SetStyle("background-image-color", "#999");
        self.Text:SetStyle("text-color", "#999");
    end)
    
    self:OnPressed(function()
        if self.Parent:IsCollapsed() then self.Parent:Expand() else self.Parent:Collapse() end;
    end)
end

function GTGUI.Element:PanelGroupBox(titleText)
    self.Title = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-title' />");
    self.Body  = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-body'  />");
    
    self.Title:PanelGroupBoxTitle(titleText);
    
    
    function self:Expand()
        if self:IsCollapsed() then
            self.Title.Arrow:SetStyle("background-image", "std-arrow-down");
            self.Body:Show();
        end
    end
    
    function self:Collapse()
        if not self:IsCollapsed() then
            self.Title.Arrow:SetStyle("background-image", "std-arrow-right");
            self.Body:Hide();
        end
    end
    
    function self:IsCollapsed()
        return self.Body:IsVisible() == false;
    end
end