-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorToolBar(sceneEditor)
    self.MenuBar = GTGUI.Server.CreateElement(self, "menubar");
    self.MenuBar:MenuBar();
    self.MenuBar.View = self.MenuBar:AppendItem("View");
    self.MenuBar.View.menu:SetStyle("padding",   "8px");
    self.MenuBar.View.menu:SetStyle("min-width", "300px");
    
    self.MenuBar.View.GridCheckBox = GTGUI.Server.CreateElement(self.MenuBar.View.menu, "checkbox");
    self.MenuBar.View.GridCheckBox:CheckBox("Show Grid");
    self.MenuBar.View.GridCheckBox:OnChecked(function()
        sceneEditor:ShowGrid();
    end);
    self.MenuBar.View.GridCheckBox:OnUnchecked(function()
        sceneEditor:HideGrid();
    end);
    
    self.MenuBar.View.AxisArrowsCheckBox = GTGUI.Server.CreateElement(self.MenuBar.View.menu, "checkbox");
    self.MenuBar.View.AxisArrowsCheckBox:SetStyle("margin-top", "4px");
    self.MenuBar.View.AxisArrowsCheckBox:CheckBox("Show Axis Guide");
    self.MenuBar.View.AxisArrowsCheckBox:OnChecked(function()
        sceneEditor:ShowAxisArrows();
    end);
    self.MenuBar.View.AxisArrowsCheckBox:OnUnchecked(function()
        sceneEditor:HideAxisArrows();
    end);

    
    GTGUI.Server.CreateElement(self.MenuBar.View.menu, "scene-editor-toolbar-viewport-menu-separator");
    

    self.MenuBar.View.EnableHDRCheckBox = GTGUI.Server.CreateElement(self.MenuBar.View.menu, "checkbox");
    self.MenuBar.View.EnableHDRCheckBox:CheckBox("Enable HDR");
    self.MenuBar.View.EnableHDRCheckBox:OnChecked(function()
        sceneEditor:EnableHDR();
    end);
    self.MenuBar.View.EnableHDRCheckBox:OnUnchecked(function()
        sceneEditor:DisableHDR();
    end);
    
    self.MenuBar.View.EnableBloomCheckBox = GTGUI.Server.CreateElement(self.MenuBar.View.menu, "checkbox");
    self.MenuBar.View.EnableBloomCheckBox:SetStyle("margin-top", "4px");
    self.MenuBar.View.EnableBloomCheckBox:CheckBox("Enable Bloom");
    self.MenuBar.View.EnableBloomCheckBox:OnChecked(function()
        sceneEditor:EnableBloom();
    end);
    self.MenuBar.View.EnableBloomCheckBox:OnUnchecked(function()
        sceneEditor:DisableBloom();
    end);
    
    
    
    self.PlaybackContainer = GTGUI.Server.CreateElement(self,                   "scene-editor-toolbar-playback-container");
    self.PlayPauseButton   = GTGUI.Server.CreateElement(self.PlaybackContainer, "scene-editor-toolbar-play-button");
    self.StopButton        = GTGUI.Server.CreateElement(self.PlaybackContainer, "scene-editor-toolbar-stop-button");
    self.StopButton:Disable();
    
    
    self.PlayPauseButton:OnPressed(function()
        if not GTEngine.System.SceneEditor.IsPlaying(sceneEditor._internalPtr) or GTEngine.System.SceneEditor.IsPaused(sceneEditor._internalPtr) then
            GTEngine.System.SceneEditor.StartPlaying(sceneEditor._internalPtr);
        else
            GTEngine.System.SceneEditor.PausePlaying(sceneEditor._internalPtr);
        end
    end);
    
    self.StopButton:OnPressed(function()
        GTEngine.System.SceneEditor.StopPlaying(sceneEditor._internalPtr);
    end);
    
    function self:UpdatePlaybackControls()
        if not GTEngine.System.SceneEditor.IsPlaying(sceneEditor._internalPtr) or GTEngine.System.SceneEditor.IsPaused(sceneEditor._internalPtr) then
            self.PlayPauseButton:DetachStyleClass("scene-editor-toolbar-pause-button");
            self.PlayPauseButton:AttachStyleClass("scene-editor-toolbar-play-button");
        else
            self.PlayPauseButton:DetachStyleClass("scene-editor-toolbar-play-button");
            self.PlayPauseButton:AttachStyleClass("scene-editor-toolbar-pause-button");
        end
        
        if GTEngine.System.SceneEditor.IsPlaying(sceneEditor._internalPtr) then
            self.StopButton:Enable();
        else
            self.StopButton:Disable();
        end
    end
    
    
    
    return self;
end