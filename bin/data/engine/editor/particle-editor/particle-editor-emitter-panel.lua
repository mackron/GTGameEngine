-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorEmitterPanel(emitter, index, ownerEditor)
    self:PanelGroupBox("Emitter " .. tostring(index), true);
    self.Emitter     = emitter;
    self.Index       = index;
    self.OwnerEditor = ownerEditor;
    
    self.EmissionShapeDropDownItems = {};       -- Indexed by the shape type.
    self.ParticleShapeDropDownItems = {};       -- Indexed by the shape type.
    
    
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
    self.EmissionShapeDropDownItems[GTEngine.ParticleEmissionShapes.Cone]   = self.EmissionShape:AppendItem("Cone");
    self.EmissionShapeDropDownItems[GTEngine.ParticleEmissionShapes.Sphere] = self.EmissionShape:AppendItem("Sphere");
    self.EmissionShapeDropDownItems[GTEngine.ParticleEmissionShapes.Box]    = self.EmissionShape:AppendItem("Box");
    self.EmissionShapeDropDownItems[GTEngine.ParticleEmissionShapes.Cone].ShapeType   = GTEngine.ParticleEmissionShapes.Cone;
    self.EmissionShapeDropDownItems[GTEngine.ParticleEmissionShapes.Sphere].ShapeType = GTEngine.ParticleEmissionShapes.Sphere;
    self.EmissionShapeDropDownItems[GTEngine.ParticleEmissionShapes.Box].ShapeType    = GTEngine.ParticleEmissionShapes.Box;
    
    
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
    
    
    self.ParticleShape = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-dropdown-box' />");
    self.ParticleShape:PanelDropDownBox("Particle Shape");
    self.ParticleShapeDropDownItems[GTEngine.ParticleShapeTypes.Billboard] = self.ParticleShape:AppendItem("Billboard");
    self.ParticleShapeDropDownItems[GTEngine.ParticleShapeTypes.Model]     = self.ParticleShape:AppendItem("Model");
    self.ParticleShapeDropDownItems[GTEngine.ParticleShapeTypes.Billboard].ShapeType = GTEngine.ParticleShapeTypes.Billboard;
    self.ParticleShapeDropDownItems[GTEngine.ParticleShapeTypes.Model].ShapeType     = GTEngine.ParticleShapeTypes.Model;
    
    self.BillboardShapeProperties          = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                     .. "' styleclass='particle-editor-panel-particle-shape-container' />");
    self.BillboardShapeProperties.Material = GTGUI.Server.New("<div parentid='" .. self.BillboardShapeProperties:GetID() .. "' styleclass='textbox' style='width:100%; margin:0px 2px;' />");
    
    self.ModelShapeProperties          = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                 .. "' styleclass='particle-editor-panel-particle-shape-container' />");
    self.ModelShapeProperties.Message  = GTGUI.Server.New("<div parentid='" .. self.ModelShapeProperties:GetID() .. "' style='margin:0px 2px; text-color:#666; font-style:bold; horizontal-align:center;'>Not currently supported.</div>");
    
    
    
    
    function self:SetEmissionShape(shapeType)
        self.Emitter:SetEmissionShapeType(shapeType);
        
        if      shapeType == GTEngine.ParticleEmissionShapes.Cone   then
            local radius,angle = self.Emitter:GetEmissionShapeProperties();
            self.ConeShapeProperties.Radius:SetValue(radius);
            self.ConeShapeProperties.Angle:SetValue(angle);
            
            self.ConeShapeProperties:Show();
            self.SphereShapeProperties:Hide();
            self.BoxShapeProperties:Hide();
        elseif shapeType == GTEngine.ParticleEmissionShapes.Sphere then
            local radius = self.Emitter:GetEmissionShapeProperties();
            self.SphereShapeProperties.Radius:SetValue(radius);
            
            self.ConeShapeProperties:Hide();
            self.SphereShapeProperties:Show();
            self.BoxShapeProperties:Hide();
        elseif shapeType == GTEngine.ParticleEmissionShapes.Box    then
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
    
    
    function self:SetParticleShape(shapeType)
        if     shapeType == GTEngine.ParticleShapeTypes.Billboard then
            self.BillboardShapeProperties.Material:SetText(self.Emitter:GetBillboardMaterialRelativePath());
        
            self.BillboardShapeProperties:Show();
            self.ModelShapeProperties:Hide();
        elseif shapeType == GTEngine.ParticleShapeTypes.Model     then
            self.BillboardShapeProperties:Hide();
            self.ModelShapeProperties:Show();
        else
            return;
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
    
    
    
    self.BillboardShapeProperties.Material:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            self.Emitter:SetBillboardMaterial(self.BillboardShapeProperties.Material:GetText());
            self.OwnerEditor:OnChange();
        end
    end)
    
    self.BillboardShapeProperties.Material:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.BillboardShapeProperties.Material:SetText(data.droppedElement.path);
            self.Emitter:SetBillboardMaterial(data.droppedElement.path);
            self.OwnerEditor:OnChange();
        end
    end)
    
    
    
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
        self:SetEmissionShape(data.selectedItem.ShapeType);
    end);
    
    self.ParticleShape:OnSelectionChanged(function(data)
        self:SetParticleShape(data.selectedItem.ShapeType);
    end);
    
    
    -- Select the emission shape from the dropdown.
    self.EmissionShape:SelectItem(self.EmissionShapeDropDownItems[self.Emitter:GetEmissionShapeType()]);
    
    -- Select the particle shape from the dropdown.
    self.ParticleShape:SelectItemByIndex(1);
    
    
    
    return self;
end