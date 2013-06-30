-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorFunctionPanelTitle(text, showCross)
    self.Arrow = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-title-arrow");
    self.Text  = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-title-text");

    self.Text:SetText(text);
    
    if showCross then
        self.Cross = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-title-cross");
        self.Cross:SetTooltip("Delete this function.");
        
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

function GTGUI.Element:ParticleEditorFunctionPanel(titleText)
    self.Title        = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-title");
    self.Body         = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-body");
    self.BottomBorder = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-separator");
    
    self.isCollapsed = false;
    
    self.Title:ParticleEditorFunctionPanelTitle(titleText, true);
    
    
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
    
    
    
    function self:HideSeparator()
        self.BottomBorder:SetStyle("border-top-width",    "0px");
        self.BottomBorder:SetStyle("border-bottom-width", "0px");
    end
    
    function self:ShowSeparator()
        self.BottomBorder:SetStyle("border-top-width",    "1px");
        self.BottomBorder:SetStyle("border-bottom-width", "1px");
    end
    
    
    function self:OnClose(arg1)
        self.Callbacks:BindOrCall("OnClose", arg1);
    end
end



-----------------------------------------
-- Scalar Function Panel.

function GTGUI.Element:ScalarParticleFunctionPanel(titleText, minValue, maxValue)
    self:ParticleEditorFunctionPanel(titleText);
    
    self.MinInput = GTGUI.Server.CreateElement(self.Body:GetID(), "labelled-number-input");
    self.MinInput:LabelledNumberInput("Min", minValue);
    self.MinInput:SetTooltip("The value at the start of the particle's life.");
    
    self.MaxInput = GTGUI.Server.CreateElement(self.Body:GetID(), "labelled-number-input");
    self.MaxInput:LabelledNumberInput("Max", maxValue);
    self.MaxInput:SetTooltip("The value at the end of the particle's life.");
    
    
    self.MinInput:OnValueChanged(function(data)
        self:OnRangeChanged({minValue = self.MinInput:GetValue(), maxValue = self.MaxInput:GetValue()});
    end);
    
    self.MaxInput:OnValueChanged(function(data)
        self:OnRangeChanged({minValue = self.MinInput:GetValue(), maxValue = self.MaxInput:GetValue()});
    end);
    
    
    function self:SetRange(minValue, maxValue)
        self.MinInput:SetValue(minValue);
        self.MaxInput:SetValue(maxValue);
    end
    
    
    
    function self:OnRangeChanged(arg1)
        self.Callbacks:BindOrCall("OnRangeChanged", arg1);
    end
end


-----------------------------------------
-- Vector Function Panel.

function GTGUI.Element:Vector3ParticleFunctionPanel(titleText, minValue, maxValue)
    self:ParticleEditorFunctionPanel(titleText);
    
    self.MinInput = GTGUI.Server.CreateElement(self.Body:GetID(), "labelled-vector3-input");
    self.MinInput:LabelledVector3Input("Min", minValue);
    self.MinInput:SetStyle("margin-top", "4px");
    self.MinInput:SetTooltip("The value at the start of the particle's life.");
    
    self.MaxInput = GTGUI.Server.CreateElement(self.Body:GetID(), "labelled-vector3-input");
    self.MaxInput:LabelledVector3Input("Max", maxValue);
    self.MaxInput:SetStyle("margin-top", "4px");
    self.MaxInput:SetTooltip("The value at the end of the particle's life.");
    
    
    self.MinInput:OnValueChanged(function(data)
        self:OnRangeChanged({minValue = self.MinInput:GetValue(), maxValue = self.MaxInput:GetValue()});
    end);
    
    self.MaxInput:OnValueChanged(function(data)
        self:OnRangeChanged({minValue = self.MinInput:GetValue(), maxValue = self.MaxInput:GetValue()});
    end);
    
    
    function self:SetRange(minValue, maxValue)
        self.MinInput:SetValue(minValue);
        self.MaxInput:SetValue(maxValue);
    end
    
    
    
    function self:OnRangeChanged(arg1)
        self.Callbacks:BindOrCall("OnRangeChanged", arg1);
    end
end



































