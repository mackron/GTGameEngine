-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_AnimationSegment(name, rangeMin, rangeMax)
    self.Name = GTGUI.Server.CreateElement(self, "model-editor-animation-segment-name");
    self.Name:SetText(name);
    
    self.Range = GTGUI.Server.CreateElement(self, "vector-input");
    self.Range:Vector2Input();
    self.Range.X:UseIntegerFormat();
    self.Range.Y:UseIntegerFormat();
    self.Range.X:SetStyle("horizontal-align", "center");
    self.Range.Y:SetStyle("horizontal-align", "center");
    self.Range:SetStyle("width", "80px");
    self.Range:SetStyle("margin-left", "4px");
    self.Range:SetValue(rangeMin, rangeMax);
    
    self.PlayButton = GTGUI.Server.CreateElement(self, "model-editor-play-button-small");
    self.PlayButton:SetStyle("margin-left", "4px");
    
    self.Cross = GTGUI.Server.CreateElement(self, "cross-button");
    self.Cross:SetStyle("margin-left", "4px");

    
    return self;
end

function GTGUI.Element:ModelEditor_AnimationSegmentsPanel(_internalPtr)
    self:PanelGroupBox("Animation Segments");
    
    self.SegmentsContainer = GTGUI.Server.CreateElement(self.Body);
    
    self.AddSegmentButton = GTGUI.Server.CreateElement(self.Body, "add-button");
    self.AddSegmentButton:AddButton("New Segment");
    self.AddSegmentButton:SetStyle("margin-top", "8px");
    
    self.AddSegmentButton:OnPressed(function()
        self:AddNewSegment("Unnamed", 0, 0, true);
    end);
    
    
    self.Segments = {};
    
    
    function self:AddNewSegment(name, startKeyFrame, endKeyFrame, addToModel)
        local newSegmentElement = GTGUI.Server.CreateElement(self.SegmentsContainer, "model-editor-animation-segment");
        if newSegmentElement then
            newSegmentElement:ModelEditor_AnimationSegment(name, startKeyFrame, endKeyFrame);
            
            newSegmentElement.Name:OnTextChanged(function()
                self:UpdateSegmentName(newSegmentElement);
            end);
            
            newSegmentElement.Range:OnValueChanged(function()
                self:UpdateSegmentRange(newSegmentElement);
            end);
            
            newSegmentElement.PlayButton:OnPressed(function()
                self:PlayAnimationSegment(newSegmentElement);
            end);
            
            newSegmentElement.Cross:OnPressed(function()
                self:DeleteSegment(newSegmentElement, true);
            end);
            
            self.Segments[#self.Segments + 1] = newSegmentElement;
            
            if addToModel then
                GTEngine.System.ModelEditor.AddNewAnimationSegment(_internalPtr, name, startKeyFrame, endKeyFrame);
            end
        end
        
        return newSegmentElement;
    end
    
    function self:DeleteSegment(segmentElement, removeFromModel)
        local index = table.indexof(self.Segments, segmentElement);
        if index ~= nil then
            table.remove(self.Segments, index);
            GTGUI.Server.DeleteElement(segmentElement);
            
            if removeFromModel then
                GTEngine.System.ModelEditor.RemoveAnimationSegmentByIndex(_internalPtr, index);
            end
        end
    end
    
    
    function self:UpdateSegmentName(segmentElement)
        local index = table.indexof(self.Segments, segmentElement);
        if index ~= nil then
            GTEngine.System.ModelEditor.SetAnimationSegmentName(_internalPtr, index, segmentElement.Name:GetText());
        end
    end
    
    function self:UpdateSegmentRange(segmentElement)
        local index = table.indexof(self.Segments, segmentElement);
        if index ~= nil then
            local startKeyFrame = segmentElement.Range.X:GetValue();
            local endKeyFrame   = segmentElement.Range.Y:GetValue();
            GTEngine.System.ModelEditor.SetAnimationSegmentFrameIndices(_internalPtr, index, startKeyFrame, endKeyFrame);
        end
    end
    
    function self:PlayAnimationSegment(segmentElement)
        local index = table.indexof(self.Segments, segmentElement);
        if index ~= nil then
            GTEngine.System.ModelEditor.PlayAnimationSegmentByIndex(_internalPtr, index);
        end
    end
    
    
    function self:Refresh()
        self.Segments = {};
        self.SegmentsContainer:DeleteAllChildren();
        
        local segments = GTEngine.System.ModelEditor.GetAnimationSegments(_internalPtr);
        if segments then
            for i,segment in ipairs(segments) do
                self:AddNewSegment(segment.name, segment.startKeyFrame, segment.endKeyFrame);
            end
        end
    end
    
    
    -- We just refresh to get the panel into the correct state.
    self:Refresh();
    
    
    return self;
end