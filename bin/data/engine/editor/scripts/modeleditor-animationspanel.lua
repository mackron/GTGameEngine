
function GTGUI.Element:ModelEditor_AnimationsPanel()
    self:PanelGroupBox("Animations");
    
    self.Body:SetStyle("horizontal-align", "center");
    self.Body:SetStyle("child-plane",      "horizontal");
    
    self.PlayButton = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='button' style='width:24px; height:24px; background-image:std-arrow-right; background-image-color:#aaa; margin-right:2px;' />");
    self.PlayButton:OnPressed(function()
        Editor.ModelEditor.PlayAnimation();
    end);
    
    self.StopButton = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='button' style='width:24px; height:24px; background-image:std-square-8x8; background-image-color:#aaa;' />");
    self.StopButton:OnPressed(function()
        Editor.ModelEditor.StopAnimation();
    end);
    
    return self;
end