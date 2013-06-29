-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorToolbar(particleEditor)
    self.MenuBar = GTGUI.Server.CreateElement(self, "menubar");
    self.MenuBar:MenuBar();
    self.MenuBar.Viewport = self.MenuBar:AppendItem("Viewport");
    self.MenuBar.Viewport.menu:SetStyle("padding",   "8px");
    self.MenuBar.Viewport.menu:SetStyle("min-width", "300px");
    
    self.MenuBar.Viewport.GridCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "checkbox");
    self.MenuBar.Viewport.GridCheckBox:CheckBox("Show Grid");
    self.MenuBar.Viewport.GridCheckBox:OnChecked(function()
        --sceneEditor:ShowGrid();
    end);
    self.MenuBar.Viewport.GridCheckBox:OnUnchecked(function()
        --sceneEditor:HideGrid();
    end);
    
    self.MenuBar.Viewport.AxisArrowsCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "checkbox");
    self.MenuBar.Viewport.AxisArrowsCheckBox:SetStyle("margin-top", "4px");
    self.MenuBar.Viewport.AxisArrowsCheckBox:CheckBox("Show Axis Guide");
    self.MenuBar.Viewport.AxisArrowsCheckBox:OnChecked(function()
        --sceneEditor:ShowAxisArrows();
    end);
    self.MenuBar.Viewport.AxisArrowsCheckBox:OnUnchecked(function()
        sceneEditor:HideAxisArrows();
    end);

    GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "scene-editor-toolbar-viewport-menu-separator");

    self.MenuBar.Viewport.ResetCameraButton = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "button");
    self.MenuBar.Viewport.ResetCameraButton:SetText("Reset Camera");
    self.MenuBar.Viewport.ResetCameraButton:OnPressed(function()
        --sceneEditor:ResetCamera();
    end);

    return self;
end