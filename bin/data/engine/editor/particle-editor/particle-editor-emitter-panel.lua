-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorEmitterPanel(emitter, index, ownerEditor)
    self:PanelGroupBox("Emitter " .. tostring(index), true);
    self.Emitter     = emitter;
    self.Index       = index;
    self.OwnerEditor = ownerEditor;
    
    self.ShapeDropDownItems = {};       -- Indexed by the shape type.
    
    
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
    
    
    self.EmissionShape = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-dropdown-box' />");
    self.EmissionShape:PanelDropDownBox("Emission Shape");
    self.ShapeDropDownItems[GTEngine.ParticleEmissionShapes.Cone]   = self.EmissionShape:AppendItem("Cone");
    self.ShapeDropDownItems[GTEngine.ParticleEmissionShapes.Sphere] = self.EmissionShape:AppendItem("Sphere");
    self.ShapeDropDownItems[GTEngine.ParticleEmissionShapes.Box]    = self.EmissionShape:AppendItem("Box");
    
    self.ShapeDropDownItems[GTEngine.ParticleEmissionShapes.Cone].ShapeType   = GTEngine.ParticleEmissionShapes.Cone;
    self.ShapeDropDownItems[GTEngine.ParticleEmissionShapes.Sphere].ShapeType = GTEngine.ParticleEmissionShapes.Sphere;
    self.ShapeDropDownItems[GTEngine.ParticleEmissionShapes.Box].ShapeType    = GTEngine.ParticleEmissionShapes.Box;
    
    
    self.ConeShapeProperties        = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                .. "' styleclass='particle-editor-panel-emitter-shape-container' />");
    self.ConeShapeProperties.Radius = GTGUI.Server.New("<div parentid='" .. self.ConeShapeProperties:GetID() .. "' styleclass='panel-number-input' />");
    self.ConeShapeProperties.Radius:PanelNumberInput("Radius", 0.0);
    self.ConeShapeProperties.Angle  = GTGUI.Server.New("<div parentid='" .. self.ConeShapeProperties:GetID() .. "' styleclass='panel-number-input' />");
    self.ConeShapeProperties.Angle:PanelNumberInput("Angle", 0.0);
    
    self.SphereShapeProperties        = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                  .. "' styleclass='particle-editor-panel-emitter-shape-container' />");
    self.SphereShapeProperties.Radius = GTGUI.Server.New("<div parentid='" .. self.SphereShapeProperties:GetID() .. "' styleclass='panel-number-input' />");
    self.SphereShapeProperties.Radius:PanelNumberInput("Radius", 0.0);
    
    self.BoxShapeProperties   = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()               .. "' styleclass='particle-editor-panel-emitter-shape-container' />");
    self.BoxShapeProperties.X = GTGUI.Server.New("<div parentid='" .. self.BoxShapeProperties:GetID() .. "' styleclass='panel-number-input' />");
    self.BoxShapeProperties.X:PanelNumberInput("X", 0.0);
    self.BoxShapeProperties.Y = GTGUI.Server.New("<div parentid='" .. self.BoxShapeProperties:GetID() .. "' styleclass='panel-number-input' />");
    self.BoxShapeProperties.Y:PanelNumberInput("Y", 0.0);
    self.BoxShapeProperties.Z = GTGUI.Server.New("<div parentid='" .. self.BoxShapeProperties:GetID() .. "' styleclass='panel-number-input' />");
    self.BoxShapeProperties.Z:PanelNumberInput("Z", 0.0);
    
    
    function self:SetShape(shapeType)
        if      shapeType == GTEngine.ParticleEmissionShapes.Cone   then
            self.Emitter:SetConeEmissionShape(1.0, 25.0);
            
            local radius,angle = self.Emitter:GetEmissionShapeProperties();
            self.ConeShapeProperties.Radius:SetValue(radius);
            self.ConeShapeProperties.Angle:SetValue(angle);
            
            self.ConeShapeProperties:Show();
            self.SphereShapeProperties:Hide();
            self.BoxShapeProperties:Hide();
        elseif shapeType == GTEngine.ParticleEmissionShapes.Sphere then
            self.Emitter:SetSphereEmissionShape(1.0);
            
            local radius = self.Emitter:GetEmissionShapeProperties();
            self.SphereShapeProperties.Radius:SetValue(radius);
            
            self.ConeShapeProperties:Hide();
            self.SphereShapeProperties:Show();
            self.BoxShapeProperties:Hide();
        elseif shapeType == GTEngine.ParticleEmissionShapes.Box    then
            self.Emitter:SetBoxEmissionShape(1.0, 1.0, 1.0);
            
            local x,y,z = self.Emitter:GetEmissionShapeProperties();
            self.BoxShapeProperties.X:SetValue(x);
            self.BoxShapeProperties.Y:SetValue(y);
            self.BoxShapeProperties.Z:SetValue(z);
            
            self.ConeShapeProperties:Hide();
            self.SphereShapeProperties:Hide();
            self.BoxShapeProperties:Show();
        else
            return;     -- Unknown shape.
        end
        
        self.OwnerEditor:OnChange();
    end
    
    
    
    -----------------------------------------------
    -- Changes to shape properties.
    
    function ApplyEmissionShape(data, blockOnChangeEvent)
        local shapeType = self.Emitter:GetEmissionShapeType();
        
        if      shapeType == GTEngine.ParticleEmissionShapes.Cone   then
            local radius = self.ConeShapeProperties.Radius:GetValue();
            local angle  = self.ConeShapeProperties.Angle:GetValue();
            self.Emitter:SetConeEmissionShape(radius, angle);
        elseif shapeType == GTEngine.ParticleEmissionShapes.Sphere then
            local radius = self.SphereShapeProperties.Radius:GetValue();
            self.Emitter:SetSphereEmissionShape(radius);
        elseif shapeType == GTEngine.ParticleEmissionShapes.Box    then
            local x = self.BoxShapeProperties.X:GetValue();
            local y = self.BoxShapeProperties.Y:GetValue();
            local z = self.BoxShapeProperties.Z:GetValue();
            self.Emitter:SetBoxEmissionShape(x, y, z);
        end
        
        if not blockOnChangeEvent then
            self.OwnerEditor:OnChange();
        end
    end

    self.ConeShapeProperties.Radius:OnValueChanged(ApplyEmissionShape);
    self.ConeShapeProperties.Angle:OnValueChanged(ApplyEmissionShape);
    
    self.SphereShapeProperties.Radius:OnValueChanged(ApplyEmissionShape);
    
    self.BoxShapeProperties.X:OnValueChanged(ApplyEmissionShape);
    self.BoxShapeProperties.Y:OnValueChanged(ApplyEmissionShape);
    self.BoxShapeProperties.Z:OnValueChanged(ApplyEmissionShape);
    
    
    
    
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
    
    
    self.EmissionShape:OnSelectionChanged(function(data)
        self:SetShape(data.selectedItem.ShapeType);
    end);
    
    
    -- Select the shape from the dropdown.
    self.EmissionShape:SelectItem(self.ShapeDropDownItems[self.Emitter:GetEmissionShapeType()]);
    
    
    
    return self;
end