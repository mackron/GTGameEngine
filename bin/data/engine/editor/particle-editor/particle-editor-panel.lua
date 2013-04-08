-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorPanel()
    self.NewEmitterButton = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='particle-editor-newemitter-button' />");
    self.NewEmitterButton:ParticleEditorNewEmitterButton();
    
    
    function self:Update(definition)
        
    end

    return self;
end