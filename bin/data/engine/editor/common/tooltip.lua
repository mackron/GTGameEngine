-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- Assigns a tooltip to the given element.
function GTGUI.Element:SetTooltip(text)
    -- If a tooltip was already set previously, the event handlers need to be unset.
    if self.MyOnShowTooltip ~= nil then
        self:UnbindEvent("OnShowTooltip", self.MyOnShowTooltip);
    end
    if self.MyOnHideTooltip ~= nil then
        self:UnbindEvent("OnHideTooltip", self.MyOnHideTooltip);
    end
    
    
    function self.MyOnShowTooltip()
        GTGUI.ShowDefaultTooltip(text, GTGUI.Server.GetMousePosition());
    end
    
    function self.MyOnHideTooltip()
        GTGUI.HideDefaultTooltip();
    end

    self:OnShowTooltip(self.MyOnShowTooltip);
    self:OnHideTooltip(self.MyOnHideTooltip);
    
    return self;
end