-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorEmitterPanel(emitter, index)
    self:PanelGroupBox("Emitter " .. tostring(index), true);
    self.Emitter = emitter;
    self.Index   = index;
    
    
    self.Burst = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-bottom:4px' />");
    self.Burst:CheckBox("Burst");
    
    self.Duration = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.Duration:PanelNumberInput("Duration", emitter:GetDurationInSeconds());
    
    self.EmissionRate = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.EmissionRate:PanelNumberInput("Emission Rate", emitter:GetEmissionRatePerSecond());
    
    
    
    return self;
end