-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_Timeline(_internalPtr)
    self.Scrubber          = GTGUI.Server.CreateElement(self, "model-editor-timeline-scrubber");
    self.ControlsContainer = GTGUI.Server.CreateElement(self, "model-editor-timeline-controls");
    
    self.PlaybackContainer = GTGUI.Server.CreateElement(self.ControlsContainer, "model-editor-timeline-playback-container");
    self.PlayPauseButton   = GTGUI.Server.CreateElement(self.PlaybackContainer, "model-editor-timeline-play-button");
    self.PlayPauseButton:SetTooltip("Play/Pause/Resume animation playback.");
    self.StopButton        = GTGUI.Server.CreateElement(self.PlaybackContainer, "model-editor-timeline-stop-button");
    self.StopButton:SetTooltip("Stop animation playback.");
    self.StopButton:Disable();
    
    
    
    function self:UpdatePlaybackControls()
        if not GTEngine.System.ModelEditor.IsAnimationPlaying(_internalPtr) or GTEngine.System.ModelEditor.IsAnimationPaused(_internalPtr) then
            self.PlayPauseButton:DetachStyleClass("model-editor-timeline-pause-button");
            self.PlayPauseButton:AttachStyleClass("model-editor-timeline-play-button");
        else
            self.PlayPauseButton:DetachStyleClass("model-editor-timeline-play-button");
            self.PlayPauseButton:AttachStyleClass("model-editor-timeline-pause-button");
        end
        
        if GTEngine.System.ModelEditor.IsAnimationPlaying(_internalPtr) then
            self.StopButton:Enable();
        else
            self.StopButton:Disable();
        end
    end
    
    
    self.PlayPauseButton:OnPressed(function()
        if GTEngine.System.ModelEditor.IsAnimationPaused(_internalPtr) then
            GTEngine.System.ModelEditor.ResumeAnimation(_internalPtr);
        else
            GTEngine.System.ModelEditor.PauseAnimation(_internalPtr);
        end
    end);
    
    self.StopButton:OnPressed(function()
        GTEngine.System.ModelEditor.StopAnimation(_internalPtr);
    end);
    
    
    
    return self;
end