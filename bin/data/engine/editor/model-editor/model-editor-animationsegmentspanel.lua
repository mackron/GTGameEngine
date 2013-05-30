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
        self:AddNewSegment("Unnamed", true);
    end);
    
    
    self.Segments = {};
    
    
    function self:AddNewSegment(name, addToModel)
        local newSegmentElement = GTGUI.Server.CreateElement(self.SegmentsContainer, "model-editor-animation-segment");
        if newSegmentElement then
            newSegmentElement:ModelEditor_AnimationSegment(name, 0, 0);
            
            newSegmentElement.Cross:OnPressed(function()
                self:DeleteSegment(newSegmentElement, true);
            end);
            
            self.Segments[#self.Segments + 1] = newSegmentElement;
            
            if addToModel then
                GTEngine.System.ModelEditor.AddNewAnimationSegment(_internalPtr, name);
            end
        end
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
    
    
    function self:Refresh()
    end
    
    return self;
end