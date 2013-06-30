-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorToolbar(particleEditor)
    self.MenuBar = GTGUI.Server.CreateElement(self, "menubar");
    self.MenuBar:MenuBar();
    self.MenuBar.Viewport = self.MenuBar:AppendItem("Viewport");
    self.MenuBar.Viewport.menu:SetStyle("padding",   "8px");
    self.MenuBar.Viewport.menu:SetStyle("min-width", "300px");
    
    
    self.MenuBar.Viewport.OrientationLabel = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "particle-editor-toolbar-orientation-label");
    self.MenuBar.Viewport.OrientationLabel:SetText("Preview Orientation");
    
    self.MenuBar.Viewport.OrientationContainerTop = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "particle-editor-toolbar-orientation-container");
    self.MenuBar.Viewport.OrientationPosXCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.OrientationContainerTop, "particle-editor-toolbar-orientation-checkbox");
    self.MenuBar.Viewport.OrientationPosXCheckBox:CheckBox("+X");
    self.MenuBar.Viewport.OrientationPosXCheckBox:OnChecked(function()
        self:UncheckAllOrientationsExcept(self.MenuBar.Viewport.OrientationPosXCheckBox);
        particleEditor:SetOrientation(math.quat_angleaxis(-90, math.vec3(0, 1, 0)));
    end);
    self.MenuBar.Viewport.OrientationPosXCheckBox:OnUnchecked(function()
        self.MenuBar.Viewport.OrientationPosXCheckBox:Check(true);
    end);
    
    self.MenuBar.Viewport.OrientationPosYCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.OrientationContainerTop, "particle-editor-toolbar-orientation-checkbox");
    self.MenuBar.Viewport.OrientationPosYCheckBox:CheckBox("+Y");
    self.MenuBar.Viewport.OrientationPosYCheckBox:OnChecked(function()
        self:UncheckAllOrientationsExcept(self.MenuBar.Viewport.OrientationPosYCheckBox);
        particleEditor:SetOrientation(math.quat_angleaxis(90, math.vec3(1, 0, 0)));
    end);
    self.MenuBar.Viewport.OrientationPosYCheckBox:OnUnchecked(function()
        self.MenuBar.Viewport.OrientationPosYCheckBox:Check(true);
    end);
    
    self.MenuBar.Viewport.OrientationPosZCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.OrientationContainerTop, "particle-editor-toolbar-orientation-checkbox");
    self.MenuBar.Viewport.OrientationPosZCheckBox:CheckBox("+Z");
    self.MenuBar.Viewport.OrientationPosZCheckBox:OnChecked(function()
        self:UncheckAllOrientationsExcept(self.MenuBar.Viewport.OrientationPosZCheckBox);
        particleEditor:SetOrientation(math.quat_angleaxis(180, math.vec3(0, 1, 0)));
    end);
    self.MenuBar.Viewport.OrientationPosZCheckBox:OnUnchecked(function()
        self.MenuBar.Viewport.OrientationPosZCheckBox:Check(true);
    end);
    
    self.MenuBar.Viewport.OrientationContainerBottom = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "particle-editor-toolbar-orientation-container");
    self.MenuBar.Viewport.OrientationNegXCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.OrientationContainerBottom, "particle-editor-toolbar-orientation-checkbox");
    self.MenuBar.Viewport.OrientationNegXCheckBox:CheckBox("-X");
    self.MenuBar.Viewport.OrientationNegXCheckBox:OnChecked(function()
        self:UncheckAllOrientationsExcept(self.MenuBar.Viewport.OrientationNegXCheckBox);
        particleEditor:SetOrientation(math.quat_angleaxis(90, math.vec3(0, 1, 0)));
    end);
    self.MenuBar.Viewport.OrientationNegXCheckBox:OnUnchecked(function()
        self.MenuBar.Viewport.OrientationNegXCheckBox:Check(true);
    end);
    
    self.MenuBar.Viewport.OrientationNegYCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.OrientationContainerBottom, "particle-editor-toolbar-orientation-checkbox");
    self.MenuBar.Viewport.OrientationNegYCheckBox:CheckBox("-Y");
    self.MenuBar.Viewport.OrientationNegYCheckBox:OnChecked(function()
        self:UncheckAllOrientationsExcept(self.MenuBar.Viewport.OrientationNegYCheckBox);
        particleEditor:SetOrientation(math.quat_angleaxis(-90, math.vec3(1, 0, 0)));
    end);
    self.MenuBar.Viewport.OrientationNegYCheckBox:OnUnchecked(function()
        self.MenuBar.Viewport.OrientationNegYCheckBox:Check(true);
    end);
    
    self.MenuBar.Viewport.OrientationNegZCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.OrientationContainerBottom, "particle-editor-toolbar-orientation-checkbox");
    self.MenuBar.Viewport.OrientationNegZCheckBox:CheckBox("-Z");
    self.MenuBar.Viewport.OrientationNegZCheckBox:OnChecked(function()
        self:UncheckAllOrientationsExcept(self.MenuBar.Viewport.OrientationNegZCheckBox);
        particleEditor:SetOrientation(math.quat_identity());
    end);
    self.MenuBar.Viewport.OrientationNegZCheckBox:OnUnchecked(function()
        self.MenuBar.Viewport.OrientationNegZCheckBox:Check(true);
    end);
    
    GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "scene-editor-toolbar-viewport-menu-separator");

    self.MenuBar.Viewport.GridCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "checkbox");
    self.MenuBar.Viewport.GridCheckBox:CheckBox("Show Grid");
    self.MenuBar.Viewport.GridCheckBox:Check();
    self.MenuBar.Viewport.GridCheckBox:OnChecked(function()
        particleEditor:ShowGrid();
    end);
    self.MenuBar.Viewport.GridCheckBox:OnUnchecked(function()
        particleEditor:HideGrid();
    end);
    
    self.MenuBar.Viewport.AxisArrowsCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "checkbox");
    self.MenuBar.Viewport.AxisArrowsCheckBox:SetStyle("margin-top", "4px");
    self.MenuBar.Viewport.AxisArrowsCheckBox:CheckBox("Show Axis Guide");
    self.MenuBar.Viewport.AxisArrowsCheckBox:Check();
    self.MenuBar.Viewport.AxisArrowsCheckBox:OnChecked(function()
        particleEditor:ShowAxisArrows();
    end);
    self.MenuBar.Viewport.AxisArrowsCheckBox:OnUnchecked(function()
        particleEditor:HideAxisArrows();
    end);
    
    GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "scene-editor-toolbar-viewport-menu-separator");

    self.MenuBar.Viewport.ResetCameraButton = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "button");
    self.MenuBar.Viewport.ResetCameraButton:SetText("Reset Camera");
    self.MenuBar.Viewport.ResetCameraButton:OnPressed(function()
        particleEditor:ResetCamera();
    end);
    
    
    function self:UncheckAllOrientationsExcept(exception)
        if exception ~= self.MenuBar.Viewport.OrientationPosXCheckBox then self.MenuBar.Viewport.OrientationPosXCheckBox:Uncheck(true) end;
        if exception ~= self.MenuBar.Viewport.OrientationPosYCheckBox then self.MenuBar.Viewport.OrientationPosYCheckBox:Uncheck(true) end;
        if exception ~= self.MenuBar.Viewport.OrientationPosZCheckBox then self.MenuBar.Viewport.OrientationPosZCheckBox:Uncheck(true) end;
        if exception ~= self.MenuBar.Viewport.OrientationNegXCheckBox then self.MenuBar.Viewport.OrientationNegXCheckBox:Uncheck(true) end;
        if exception ~= self.MenuBar.Viewport.OrientationNegYCheckBox then self.MenuBar.Viewport.OrientationNegYCheckBox:Uncheck(true) end;
        if exception ~= self.MenuBar.Viewport.OrientationNegZCheckBox then self.MenuBar.Viewport.OrientationNegZCheckBox:Uncheck(true) end;
    end;
    
    -- Check the +Y orientation otion by default. The 'true' argument simply prevents the OnChecked event from getting called.
    self.MenuBar.Viewport.OrientationPosYCheckBox:Check(true);

    return self;
end