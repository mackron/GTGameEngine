-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:CameraComponentPanel()
    self:PanelGroupBox("Camera", true);
    
    -- FOV
    self.FOVInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.FOVInput:LabelledNumberInput("FOV");
    self.FOVInput:UseFloatFormat();
    
    -- zNear
    self.NearInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.NearInput:LabelledNumberInput("Near Plane");
    self.NearInput:UseFloatFormat();
    
    -- zFar
    self.FarInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.FarInput:LabelledNumberInput("Far Plane");
    self.FarInput:UseFloatFormat();
    
    
    
    self.FOVInput:OnValueChanged(function(data)
        self:Update3DProjection();
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    self.NearInput:OnValueChanged(function(data)
        self:Update3DProjection();
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    self.FarInput:OnValueChanged(function(data)
        self:Update3DProjection();
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Camera);
        
        self.FOVInput:SetValue(self.CurrentComponent:GetFOV());
        self.NearInput:SetValue(self.CurrentComponent:GetNearClippingPlane());
        self.FarInput:SetValue(self.CurrentComponent:GetFarClippingPlane());
    end
    
    function self:Update3DProjection()
        local fov    = self.FOVInput:GetValue();
        local aspect = 16.0 / 9.0;
        local znear  = self.NearInput:GetValue();
        local zfar   = self.FarInput:GetValue();
        self.CurrentComponent:Set3DProjection(fov, aspect, znear, zfar);
    end
    
    return self;
end