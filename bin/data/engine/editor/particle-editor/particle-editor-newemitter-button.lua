-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorNewEmitterButton()
    self.Container = GTGUI.Server.New("<div parentid='" .. self:GetID()           .. "' styleclass='particle-editor-newemitter-button-container' />");
    self.Icon      = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' styleclass='particle-editor-newemitter-button-icon'      />");
    self.Label     = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' styleclass='particle-editor-newemitter-button-label'      >New Emitter</div>");
    self.Container:SetTooltip("Add a new emitter to the particle system.");
    
    self.Container:OnPressed(function(data)
        self:OnPressed(data);
    end);
    
    return self;
end