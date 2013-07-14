-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.


function GTGUI.Element:PointLightComponentPanel()
    self:PanelGroupBox("Point Light", true);
    
    -- Colour
    self.ColourInput = GTGUI.Server.CreateElement(self.Body, "labelled-vector3-input");
    self.ColourInput:LabelledVector3Input("Colour:");
    
    -- Radius
    self.RadiusInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.RadiusInput:LabelledNumberInput("Radius:");
    
    -- Falloff
    self.FalloffInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.FalloffInput:LabelledNumberInput("Falloff:");
    
    -- Shadows
    self.CastShadows = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.CastShadows:SetStyle("margin-top", "8px");
    
    
    
    self.CurrentNode           = nil;
    self.CurrentComponent      = nil;
    self.LockAttenuationEvents = false;
    
    
    
    self.ColourInput:OnValueChanged(function(data)
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetColour(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    
    self.RadiusInput:OnValueChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateRadius() end;
    end);
    
    self.FalloffInput:OnValueChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateFalloff() end;
    end);
    

    
    self.CastShadows:CheckBox("Cast Shadows");
    
    self.CastShadows:OnChecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:EnableShadowCasting();
            self.ParentPanel:OnSceneNodeChanged();
        end;
    end);
    
    self.CastShadows:OnUnchecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:DisableShadowCasting();
            self.ParentPanel:OnSceneNodeChanged();
        end;
    end);
    
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.PointLight);
        
        if self.CurrentComponent ~= nil then
            self.ColourInput:SetValue(self.CurrentComponent:GetColour());
            
            self.LockAttenuationEvents = true;
                self.RadiusInput:SetValue(self.CurrentComponent:GetRadius());
                self.FalloffInput:SetValue(self.CurrentComponent:GetFalloff());
            self.LockAttenuationEvents = false;
            
            if self.CurrentComponent:IsShadowCastingEnabled() then
                self.CastShadows:Check(true);
            else
                self.CastShadows:Uncheck(true);
            end
        end
    end
    
    function self:UpdateRadius()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetRadius(self.RadiusInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateFalloff()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetFalloff(self.FalloffInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    

    return self;
end

function GTGUI.Element:SpotLightComponentPanel()
    self:PanelGroupBox("Spot Light", true);
    
    -- Colour
    self.ColourInput = GTGUI.Server.CreateElement(self.Body, "labelled-vector3-input");
    self.ColourInput:LabelledVector3Input("Colour:");
    
    -- Length
    self.LengthInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.LengthInput:LabelledNumberInput("Length:");
    
    --[[
    self.LengthContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.LengthLeft      = GTGUI.Server.New("<div parentid='" .. self.LengthContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.LengthRight     = GTGUI.Server.New("<div parentid='" .. self.LengthContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.LengthLabel     = GTGUI.Server.New("<div parentid='" .. self.LengthLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Length:</div>");
    self.LengthInput     = GTGUI.Server.New("<div parentid='" .. self.LengthRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
    ]]
    
    -- Falloff
    self.FalloffInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.FalloffInput:LabelledNumberInput("Falloff:");
    
    --[[
    self.FalloffContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()             .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.FalloffLeft      = GTGUI.Server.New("<div parentid='" .. self.FalloffContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.FalloffRight     = GTGUI.Server.New("<div parentid='" .. self.FalloffContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.FalloffLabel     = GTGUI.Server.New("<div parentid='" .. self.FalloffLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Falloff:</div>");
    self.FalloffInput     = GTGUI.Server.New("<div parentid='" .. self.FalloffRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
    ]]

    -- Angles
    self.InnerAngleInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.InnerAngleInput:LabelledNumberInput("Inner Angle:");
    
    self.OuterAngleInput = GTGUI.Server.CreateElement(self.Body, "labelled-number-input");
    self.OuterAngleInput:LabelledNumberInput("Outer Angle:");
    
    --[[
    self.AnglesContainer  = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:8px;' />");
    self.AnglesLeft       = GTGUI.Server.New("<div parentid='" .. self.AnglesContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.AnglesRight      = GTGUI.Server.New("<div parentid='" .. self.AnglesContainer:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.InnerAngleLabel  = GTGUI.Server.New("<div parentid='" .. self.AnglesLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px; margin-bottom:4px;'>Inner Angle:</div>");
    self.OuterAngleLabel  = GTGUI.Server.New("<div parentid='" .. self.AnglesLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding-top:2px;'                   >Outer Angle:</div>");
    
    self.InnerAngleInput  = GTGUI.Server.New("<div parentid='" .. self.AnglesRight:GetID()  .. "' styleclass='textbox' style='width:100%; margin-bottom:2px;'></div>");
    self.OuterAngleInput  = GTGUI.Server.New("<div parentid='" .. self.AnglesRight:GetID()  .. "' styleclass='textbox' style='width:100%;'></div>");
    ]]
    
    -- Shadows
    self.CastShadows = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.CastShadows:SetStyle("margin-top", "8px");
    
    
    
    self.CurrentNode           = nil;
    self.CurrentComponent      = nil;
    self.LockAttenuationEvents = false;
    self.LockAnglesEvents      = false;
    
    
    
    self.ColourInput:OnValueChanged(function(data)
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetColour(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    
    self.LengthInput:OnValueChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateLength() end;
    end);
    
    self.FalloffInput:OnValueChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateFalloff() end;
    end);

    
    self.InnerAngleInput:OnValueChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAnglesEvents then self:UpdateComponentAngles() end
    end);
    
    self.OuterAngleInput:OnValueChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAnglesEvents then self:UpdateComponentAngles() end
    end);
    
    
    
    self.CastShadows:CheckBox("Cast Shadows");
    
    self.CastShadows:OnChecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:EnableShadowCasting();
            self.ParentPanel:OnSceneNodeChanged();
        end;
    end);
    
    self.CastShadows:OnUnchecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:DisableShadowCasting();
            self.ParentPanel:OnSceneNodeChanged();
        end;
    end);
    
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.SpotLight);
        
        if self.CurrentComponent ~= nil then
            self.ColourInput:SetValue(self.CurrentComponent:GetColour());
            
            self.LockAttenuationEvents = true;
                self.LengthInput:SetValue(self.CurrentComponent:GetLength());
                self.FalloffInput:SetValue(self.CurrentComponent:GetFalloff());
            self.LockAttenuationEvents = false;
            
            self.LockAnglesEvents = true;
                local innerAngle, outerAngle = self.CurrentComponent:GetAngles();
                self.InnerAngleInput:SetValue(innerAngle);
                self.OuterAngleInput:SetValue(outerAngle);
            self.LockAnglesEvents = false;
            
            if self.CurrentComponent:IsShadowCastingEnabled() then
                self.CastShadows:Check(true);
            else
                self.CastShadows:Uncheck(true);
            end
        end
    end
    
    function self:UpdateLength()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetLength(self.LengthInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateFalloff()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetFalloff(self.FalloffInput:GetValue());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateComponentAngles()
        if self.CurrentComponent ~= nil then
            local innerAngle = self.InnerAngleInput:GetValue();
            local outerAngle = self.OuterAngleInput:GetValue();
            
            self.CurrentComponent:SetAngles(innerAngle, outerAngle);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    return self;
end

function GTGUI.Element:DirectionalLightComponentPanel()
    self:PanelGroupBox("Directional Light", true);
    
    -- Colour
    self.ColourInput = GTGUI.Server.CreateElement(self.Body, "labelled-vector3-input");
    self.ColourInput:LabelledVector3Input("Colour:");
    
    -- Shadows
    self.CastShadows = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.CastShadows:SetStyle("margin-top", "8px");
    
    
    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;

    
    self.ColourInput:OnValueChanged(function(data)
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetColour(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    
    
    self.CastShadows:CheckBox("Cast Shadows");
    
    self.CastShadows:OnChecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:EnableShadowCasting();
            self.ParentPanel:OnSceneNodeChanged();
        end;
    end);
    
    self.CastShadows:OnUnchecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:DisableShadowCasting();
            self.ParentPanel:OnSceneNodeChanged();
        end;
    end);
    
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.DirectionalLight);
        
        if self.CurrentComponent ~= nil then
            self.ColourInput:SetValue(self.CurrentComponent:GetColour());
            
            if self.CurrentComponent:IsShadowCastingEnabled() then
                self.CastShadows:Check(true);
            else
                self.CastShadows:Uncheck(true);
            end
        end
    end
    
    return self;
end

function GTGUI.Element:AmbientLightComponentPanel()
    self:PanelGroupBox("Ambient Light", true);
    
    -- Colour
    self.ColourInput = GTGUI.Server.CreateElement(self.Body, "labelled-vector3-input");
    self.ColourInput:LabelledVector3Input("Colour:");
    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;

    
    self.ColourInput:OnValueChanged(function(data)
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetColour(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);

    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.AmbientLight);
        
        if self.CurrentComponent ~= nil then
            self.ColourInput:SetValue(self.CurrentComponent:GetColour());
        end
    end
    
    return self;
end