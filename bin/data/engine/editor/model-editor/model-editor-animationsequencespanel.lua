-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_AnimationSequence(name)
    self.NameContainer = GTGUI.Server.CreateElement(self, "model-editor-animation-sequence-name-container");

    self.Name = GTGUI.Server.CreateElement(self.NameContainer, "model-editor-animation-sequence-name");
    self.Name:SetText(name);
    
    self.Cross = GTGUI.Server.CreateElement(self.NameContainer, "cross-button");
    self.Cross:SetStyle("margin-left", "4px");
    
    
    self.PropertiesContainer = GTGUI.Server.CreateElement(self, "model-editor-animation-sequence-properties-container");

    
    
    return self;
end

function GTGUI.Element:ModelEditor_AnimationSequencesPanel(_internalPtr)
    self:PanelGroupBox("Animation Sequences");
    
    self.SequencesContainer = GTGUI.Server.CreateElement(self.Body);
    
    self.AddSequenceButton = GTGUI.Server.CreateElement(self.Body, "add-button");
    self.AddSequenceButton:AddButton("New Sequence");
    self.AddSequenceButton:SetStyle("margin-top", "8px");
    
    self.AddSequenceButton:OnPressed(function()
        self:AddNewSequence("Unnamed", true);
    end);
    
    
    self.Sequences = {};
    
    
    function self:AddNewSequence(name, addToModel)
        local newSequenceElement = GTGUI.Server.CreateElement(self.SequencesContainer, "model-editor-animation-sequence");
        if newSequenceElement then
            newSequenceElement:ModelEditor_AnimationSequence(name);
            
            newSequenceElement.Name:OnTextChanged(function()
                self:UpdateSequenceName(newSequenceElement);
            end);

            newSequenceElement.Cross:OnPressed(function()
                self:DeleteSegment(newSequenceElement, true);
            end);

            self.Sequences[#self.Sequences + 1] = newSequenceElement;
            
            if addToModel then
                --GTEngine.System.ModelEditor.AddNewAnimationSequence(_internalPtr, name);
            end
        end
        
        return newSequenceElement;
    end
    
    function self:DeleteSegment(sequenceElement, removeFromModel)
        local index = table.indexof(self.Sequences, sequenceElement);
        if index ~= nil then
            table.remove(self.Sequences, index);
            GTGUI.Server.DeleteElement(sequenceElement);
            
            if removeFromModel then
                --GTEngine.System.ModelEditor.RemoveAnimationSequenceByIndex(_internalPtr, index);
            end
        end
    end
    
    
    function self:UpdateSequenceName(segmentElement)
        local index = table.indexof(self.Segments, segmentElement);
        if index ~= nil then
            --GTEngine.System.ModelEditor.SetAnimationSegmentName(_internalPtr, index, segmentElement.Name:GetText());
        end
    end
    
    
    function self:Refresh()
    end
    
    return self;
end