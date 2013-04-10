-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorEmitterPanel(emitter, index, ownerEditor)
    self:PanelGroupBox("Emitter " .. tostring(index), true);
    self.Emitter     = emitter;
    self.Index       = index;
    self.OwnerEditor = ownerEditor;
    
    
    self.Burst = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-bottom:4px' />");
    self.Burst:CheckBox("Burst");
    
    self.Duration = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.Duration:PanelNumberInput("Duration", emitter:GetDurationInSeconds());
    
    self.EmissionRate = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.EmissionRate:PanelNumberInput("Emission Rate", emitter:GetEmissionRatePerSecond());
    
    self.StartSpeed = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.StartSpeed:PanelNumberInput("Start Speed", emitter:GetStartSpeed());
    
    self.Lifetime = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.Lifetime:PanelNumberInput("Lifetime", emitter:GetLifetime());
    
    self.GravityFactor = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.GravityFactor:PanelNumberInput("Gravity Factor", emitter:GetGravityFactor());
    
    
    self.Burst:OnChecked(function(data)
        self.Emitter:EnableBurstMode();
        self.OwnerEditor:OnChange();
    end);
    
    self.Burst:OnUnchecked(function(data)
        self.Emitter:DisableBurstMode();
        self.OwnerEditor:OnChange();
    end);
    
    self.Duration:OnValueChanged(function(data)
        self.Emitter:SetDurationInSeconds(self.Duration:GetValue());
        self.OwnerEditor:OnChange();
    end);
    
    self.EmissionRate:OnValueChanged(function(data)
        self.Emitter:SetEmissionRatePerSecond(self.EmissionRate:GetValue());
        self.OwnerEditor:OnChange();
    end);
    
    self.StartSpeed:OnValueChanged(function(data)
        self.Emitter:SetStartSpeed(self.StartSpeed:GetValue());
        self.OwnerEditor:OnChange();
    end);
    
    self.Lifetime:OnValueChanged(function(data)
        self.Emitter:SetLifetime(self.Lifetime:GetValue());
        self.OwnerEditor:OnChange();
    end);
    
    self.GravityFactor:OnValueChanged(function(data)
        self.Emitter:SetGravityFactor(self.GravityFactor:GetValue());
        self.OwnerEditor:OnChange();
    end);
    
    
    return self;
end