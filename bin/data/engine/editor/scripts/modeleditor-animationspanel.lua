-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_AnimationsPanel(_internalPtr)
    self:PanelGroupBox("Animations");
    
    self.Body:SetStyle("horizontal-align", "center");
    self.Body:SetStyle("child-plane",      "horizontal");
    
    self.PlayButton = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='button' style='width:24px; height:24px; background-image:std-arrow-right; background-image-color:#aaa; margin-right:2px;' />");
    self.PlayButton:OnPressed(function()
        GTEngine.System.ModelEditor.PlayAnimation(_internalPtr);
    end);
    
    self.StopButton = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='button' style='width:24px; height:24px; background-image:std-square-8x8; background-image-color:#aaa;' />");
    self.StopButton:OnPressed(function()
        GTEngine.System.ModelEditor.StopAnimation(_internalPtr);
    end);
    
    
    function self:Refresh()
    end
    
    
    -- We just do a refresh to get everything setup correctly.
    self:Refresh();
    
    return self;
end