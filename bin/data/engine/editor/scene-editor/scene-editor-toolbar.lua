-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorToolBar(sceneEditor)
    self.MenuBar = GTGUI.Server.CreateElement(self, "menubar");
    self.MenuBar:MenuBar();
    self.MenuBar.Viewport = self.MenuBar:AppendItem("Viewport");
    self.MenuBar.Viewport.menu:SetStyle("padding",   "8px");
    self.MenuBar.Viewport.menu:SetStyle("min-width", "300px");
    
    self.MenuBar.Viewport.GridCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "checkbox");
    self.MenuBar.Viewport.GridCheckBox:CheckBox("Show Grid");
    self.MenuBar.Viewport.GridCheckBox:OnChecked(function()
        sceneEditor:ShowGrid();
    end);
    self.MenuBar.Viewport.GridCheckBox:OnUnchecked(function()
        sceneEditor:HideGrid();
    end);
    
    self.MenuBar.Viewport.AxisArrowsCheckBox = GTGUI.Server.CreateElement(self.MenuBar.Viewport.menu, "checkbox");
    self.MenuBar.Viewport.AxisArrowsCheckBox:SetStyle("margin-top", "4px");
    self.MenuBar.Viewport.AxisArrowsCheckBox:CheckBox("Show Axis Guide");
    self.MenuBar.Viewport.AxisArrowsCheckBox:OnChecked(function()
        sceneEditor:ShowAxisArrows();
    end);
    self.MenuBar.Viewport.AxisArrowsCheckBox:OnUnchecked(function()
        sceneEditor:HideAxisArrows();
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
    
    
    function self:UpdateViewportMenu()
        self.MenuBar.Viewport.GridCheckBox:SetChecked(sceneEditor:IsShowingGrid(), true);
        self.MenuBar.Viewport.AxisArrowsCheckBox:SetChecked(sceneEditor:IsShowingAxisArrows(), true);
    end
    
    
    
    return self;
end