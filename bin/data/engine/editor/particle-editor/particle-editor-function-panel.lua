-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorFunctionPanelTitle(text, showCross)
    self.Arrow = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-title-arrow");
    self.Text  = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-title-text");

    self.Text:SetText(text);
    
    if showCross then
        self.Cross = GTGUI.Server.CreateElement(self, "particle-editor-function-panel-title-cross");
        
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

function GTGUI.Element:ScalarParticleFunctionPanel(titleText)
    self:ParticleEditorFunctionPanel(titleText);
end


-----------------------------------------
-- Vector Function Panel.

function GTGUI.Element:Vector3ParticleFunctionPanel(titleText)
    self:ParticleEditorFunctionPanel(titleText);
end



































