-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorEmitterPanel(title, index)
    self:PanelGroupBox(title, true);
    self.Index = index;
    
    
    return self;
end