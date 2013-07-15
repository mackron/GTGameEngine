-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:DynamicsComponentPanel()
    self:PanelGroupBox("Dynamics", true);
    
    -- Kinematic
    self.IsKinematic = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.IsKinematic:CheckBox("Is Kinematic");
    self.IsKinematic:SetStyle("margin-top", "0px");
    
    -- Mass
    self.MassInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.MassInput:LabelledNumberInput("Mass");
    
    -- Friction
    self.FrictionInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.FrictionInput:LabelledNumberInput("Friction");
    
    -- Restitution
    self.RestitutionInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.RestitutionInput:LabelledNumberInput("Restitution", 0.0);

    
    -- Linear Damping.
    self.LinearDampingInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.LinearDampingInput:LabelledNumberInput("Linear Damping", 0.0);
    
    -- Angular Damping.
    self.AngularDampingInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.AngularDampingInput:LabelledNumberInput("Angular Damping", 0.0);
    
    
    -- Collision Shapes
    self.CollisionShapesLabel = GTGUI.Server.CreateElement(self.Body, "dynamics-component-panel-shapes-label");
    self.CollisionShapesLabel:SetText("Collision Shapes:");
    
    self.CollisionShapes = GTGUI.Server.CreateElement(self.Body);
    self.CollisionShapes:CollisionShapesPanel();
    
    self.CollisionShapes:OnShapesChanged(function()
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;
    self.IsUpdating       = false;

    
    
    
    
    self.IsKinematic:OnChecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:IsKinematic(true);
            
            self.MassInput:SetValue(self.CurrentComponent:GetMass());
            self.MassInput:Disable();
            
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    self.IsKinematic:OnUnchecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:IsKinematic(false);
            
            self.MassInput:SetValue(self.CurrentComponent:GetMass());
            self.MassInput:Enable();
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    
    self.MassInput:OnValueChanged(function(data)
        if not self.IsUpdating then
            self:ApplyMass();
        end
    end);
    
    self.FrictionInput:OnValueChanged(function(data)
        if not self.IsUpdating then
            self:ApplyFriction();
        end
    end);
    
    self.RestitutionInput:OnValueChanged(function(data)
        if not self.IsUpdating then
            self:ApplyRestitution();
        end
    end);
    
    self.LinearDampingInput:OnValueChanged(function(data)
        if not self.IsUpdating then
            if data.value > 1.0 then
                self.LinearDampingInput:SetValue(1.0);
            elseif data.value < 0.0 then
                self.LinearDampingInput:SetValue(0.0);
            end
        
            self:ApplyLinearDamping();
        end
    end);
    
    self.AngularDampingInput:OnValueChanged(function(data)
        if not self.IsUpdating then
            if data.value > 1.0 then
                self.AngularDampingInput:SetValue(1.0);
            elseif data.value < 0.0 then
                self.AngularDampingInput:SetValue(0.0);
            end
        
            self:ApplyAngularDamping();
        end
    end);

    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Dynamics);
        self.IsUpdating       = true;
        
        if self.CurrentComponent ~= nil then
            if self.CurrentComponent:IsKinematic() then
                self.IsKinematic:Check(true);
                self.MassInput:Disable();
            else
                self.IsKinematic:Uncheck(true);
                self.MassInput:Enable();
            end
            
            self.MassInput:SetValue(          self.CurrentComponent:GetMass());
            self.FrictionInput:SetValue(      self.CurrentComponent:GetFriction());
            self.RestitutionInput:SetValue(   self.CurrentComponent:GetRestitution());
            self.LinearDampingInput:SetValue( self.CurrentComponent:GetLinearDamping());
            self.AngularDampingInput:SetValue(self.CurrentComponent:GetAngularDamping());
            
            -- Collision shapes need to be updated.
            self.CollisionShapes:Update(self.CurrentComponent);
        end
        
        self.IsUpdating = false;
    end
    
    
    function self:ApplyMass()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetMass(self.MassInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:ApplyFriction()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetFriction(self.FrictionInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:ApplyRestitution()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetRestitution(self.RestitutionInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:ApplyLinearDamping()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetDamping(self.LinearDampingInput:GetValue(), self.AngularDampingInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:ApplyAngularDamping()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetDamping(self.LinearDampingInput:GetValue(), self.AngularDampingInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    

    return self;
end
