-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.




function GTGUI.Element:CollisionShapePanel(title, hideOffset)
    self.TitleContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                .. "' style='width:100%; flex-child-width:true; child-plane:horizontal; vertical-align:center; margin-bottom:4px;' />");
    self.Title          = GTGUI.Server.New("<div parentid='" .. self.TitleContainer:GetID() .. "' style='width:100%; vertical-align:center; text-color:std-text-color; font-style:bold;' />");
    self.DeleteButton   = GTGUI.Server.New("<div parentid='" .. self.TitleContainer:GetID() .. "' styleclass='collision-shape-panel-delete-button' style='' />");
    
    self.Title:SetText(title);

    -- TODO: Looks like a layout bug. self.Title is being left with a height of 0...
    --         Only happens when the parent has flex-child-width set to true.
    --         Only happens when the width of self.DeleteButton is auto.
    
    if not hideOffset then
        self.OffsetContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; padding-left:4px' />");
        self.OffsetLabel     = GTGUI.Server.New("<div parentid='" .. self.OffsetContainer:GetID() .. "' styleclass='vector3-label' style='width:50px; height:100%'>Offset:</div>");
        self.OffsetInput     = GTGUI.Server.New("<div parentid='" .. self.OffsetContainer:GetID() .. "' styleclass='vector3-input' style='width:100%; height:auto;' />"):Vector3Input();
        
        self.OffsetInput:OnValueChanged(function(data)
            self:OnOffsetChanged(data);
        end);
    end
    
    
    self.DeleteButton:OnPressed(function()
        self:OnDelete();
    end);
    
    
    
    -- Retrieves the offset as 3 floats.
    function self:GetOffsetXYZ()
        if self.OffsetInput ~= nil then
            return self.OffsetInput:GetXYZ();
        else
            return 0.0, 0.0, 0.0;
        end
    end
    
    
    function self:UpdateOffset(shape)
        if self.OffsetInput ~= nil then
            if shape.offsetX ~= nil and shape.offsetY ~= nil and shape.offsetZ ~= nil then
                self.OffsetInput:SetValue(shape.offsetX, shape.offsetY, shape.offsetZ);
            end
        end
    end
    


    function self:OnDelete(arg1)
        self.Callbacks:BindOrCall("OnDelete", arg1);
    end
    
    function self:OnOffsetChanged(arg1)
        self.Callbacks:BindOrCall("OnOffsetChanged", arg1);
    end
    
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_Box()
    self:CollisionShapePanel("Box");

    self.ExtentsContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                  .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; padding-left:4px; margin-top:4px;' />");
    self.ExtentsLabel     = GTGUI.Server.New("<div parentid='" .. self.ExtentsContainer:GetID() .. "' styleclass='vector3-label' style='width:50px; height:100%'>Extents:</div>");
    self.ExtentsInput     = GTGUI.Server.New("<div parentid='" .. self.ExtentsContainer:GetID() .. "' styleclass='vector3-input' style='width:100%; height:auto;' />"):Vector3Input();
    
    self.ExtentsInput:OnValueChanged(function(data)
        self:OnExtentsChanged(data);
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.ExtentsInput:SetValue(shape.halfX * 2.0, shape.halfY * 2.0, shape.halfZ * 2.0);
    end
    
    function self:OnExtentsChanged(arg1)
        self.Callbacks:BindOrCall("OnExtentsChanged", arg1);
    end
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_Sphere()
    self:CollisionShapePanel("Sphere");
    
    self.RadiusContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; padding-left:4px; margin-top:4px;' />");
    self.RadiusLabel     = GTGUI.Server.New("<div parentid='" .. self.RadiusContainer:GetID() .. "' styleclass='vector3-label' style='width:50px; height:100%; horizontal-align:left;'>Radius:</div>");
    self.RadiusInput     = GTGUI.Server.New("<div parentid='" .. self.RadiusContainer:GetID() .. "' styleclass='textbox' style='width:100%; max-width:72px; height:auto;' />");
    
    
    self.RadiusInput:OnTextChanged(function()
        self:OnRadiusChanged({radius = tonumber(self.RadiusInput:GetText())});
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.RadiusInput:SetText(string.format("%.4f", shape.radius));
    end
    
    
    function self:OnRadiusChanged(arg1)
        self.Callbacks:BindOrCall("OnRadiusChanged", arg1);
    end
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_Ellipsoid()
    self:CollisionShapePanel("Ellipsoid");
    
    self.RadiusContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; padding-left:4px; margin-top:4px;' />");
    self.RadiusLabel     = GTGUI.Server.New("<div parentid='" .. self.RadiusContainer:GetID() .. "' styleclass='vector3-label' style='width:50px; height:100%'>Extents:</div>");
    self.RadiusInput     = GTGUI.Server.New("<div parentid='" .. self.RadiusContainer:GetID() .. "' styleclass='vector3-input' style='width:100%; height:auto;' />"):Vector3Input();
    
    
    self.RadiusInput:OnValueChanged(function(data)
        self:OnRadiusChanged(data);
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.RadiusInput:SetValue(shape.radiusX, shape.radiusY, shape.radiusZ);
    end
    
    
    function self:OnRadiusChanged(arg1)
        self.Callbacks:BindOrCall("OnRadiusChanged", arg1);
    end
    
    return self;
end


function GTGUI.Element:CollisionShapePanel_Cylinder(title)
    self:CollisionShapePanel(title);
    
    self.ExtentsContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                  .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; padding-left:4px; margin-top:4px;' />");
    self.ExtentsLabel     = GTGUI.Server.New("<div parentid='" .. self.ExtentsContainer:GetID() .. "' styleclass='vector3-label' style='width:50px; height:100%'>Extents:</div>");
    self.ExtentsInput     = GTGUI.Server.New("<div parentid='" .. self.ExtentsContainer:GetID() .. "' styleclass='vector3-input' style='width:100%; height:auto;' />"):Vector3Input();
    
    
    self.ExtentsInput:OnValueChanged(function(data)
        self:OnExtentsChanged(data);
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.ExtentsInput:SetValue(shape.halfX * 2.0, shape.halfY * 2.0, shape.halfZ * 2.0);
    end
    
    
    function self:OnExtentsChanged(arg1)
        self.Callbacks:BindOrCall("OnExtentsChanged", arg1);
    end
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CylinderX()
    self:CollisionShapePanel_Cylinder("Cylinder X");
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CylinderY()
    self:CollisionShapePanel_Cylinder("Cylinder Y");
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CylinderZ()
    self:CollisionShapePanel_Cylinder("Cylinder Z");
    
    return self;
end


function GTGUI.Element:CollisionShapePanel_Capsule(title)
    self:CollisionShapePanel(title);
    
    self.RadiusContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; padding-left:4px; margin-top:4px;' />");
    self.RadiusLabel     = GTGUI.Server.New("<div parentid='" .. self.RadiusContainer:GetID() .. "' styleclass='vector3-label' style='width:50px; height:100%; horizontal-align:left;'>Radius:</div>");
    self.RadiusInput     = GTGUI.Server.New("<div parentid='" .. self.RadiusContainer:GetID() .. "' styleclass='textbox' style='width:100%; max-width:72px; height:auto;' />");
    
    self.HeightContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; padding-left:4px; margin-top:4px;' />");
    self.HeightLabel     = GTGUI.Server.New("<div parentid='" .. self.HeightContainer:GetID() .. "' styleclass='vector3-label' style='width:50px; height:100%; horizontal-align:left;'>Height:</div>");
    self.HeightInput     = GTGUI.Server.New("<div parentid='" .. self.HeightContainer:GetID() .. "' styleclass='textbox' style='width:100%; max-width:72px; height:auto;' />");
    
    
    self.RadiusInput:OnTextChanged(function()
        self:OnSizeChanged({radius = tonumber(self.RadiusInput:GetText()), height = tonumber(self.HeightInput:GetText())});
    end);
    
    self.HeightInput:OnTextChanged(function()
        self:OnSizeChanged({radius = tonumber(self.RadiusInput:GetText()), height = tonumber(self.HeightInput:GetText())});
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.RadiusInput:SetText(string.format("%.4f", shape.radius));
        self.HeightInput:SetText(string.format("%.4f", shape.height));
    end
    
    
    function self:OnSizeChanged(arg1)
        self.Callbacks:BindOrCall("OnSizeChanged", arg1);
    end

    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CapsuleX()
    self:CollisionShapePanel_Capsule("Capsule X");
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CapsuleY()
    self:CollisionShapePanel_Capsule("Capsule Y");
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CapsuleZ()
    self:CollisionShapePanel_Capsule("Capsule Z");
    
    return self;
end


function GTGUI.Element:CollisionShapePanel_ConvexHull()
    self:CollisionShapePanel("Convex Hull", true);              -- 'true' means to hide the offset properties since they're not used with convex hulls at the moment.
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_ModelConvexHulls()
    self:CollisionShapePanel("Model Convex Hulls", true);       -- 'true' means to hide the offset properties since they're not used with convex hulls at the moment.
    
    return self;
end


function GTGUI.Element:NewCollisionShapeMenu()
    self.Items = {}
    
    function self:AppendNewItem(title)
        local newItem = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='collision-shape-panel-new-menu-item' />");
        newItem:SetText(title);
        
        self.Items[newItem:GetID()] = newItem;
        
        return newItem;
    end
end


function GTGUI.Element:NewComponentMenu()
    self.Items = {}
    
    function self:AppendNewItem(title)
        local newItem = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='new-component-menu-item' />");
        newItem:SetText(title);
        
        self.Items[newItem:GetID()] = newItem;
        
        return newItem;
    end
end


-- Editor.SceneEditor.CreateCustomComponentPanel()
--
-- This should be overwritten by the client game so that it can create it's own GUI panels for it's custom components, if it has any.
function Editor.SceneEditor.CreateCustomComponentPanel(panelElement, componentID)
end

function Editor.SceneEditor.CreateComponentPanel(parentPanel, componentID)
    local element = GTGUI.Server.New("<div parentid='" .. parentPanel.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    element.ParentPanel = parentPanel;
    
    
    if     componentID == GTEngine.Components.Camera then
        element:CameraComponentPanel();
    elseif componentID == GTEngine.Components.Model then
        element:ModelComponentPanel();
    elseif componentID == GTEngine.Components.Camera then
        element:CameraComponentPanel();
    elseif componentID == GTEngine.Components.PointLight then
        element:PointLightComponentPanel();
    elseif componentID == GTEngine.Components.SpotLight then
        element:SpotLightComponentPanel();
    elseif componentID == GTEngine.Components.DirectionalLight then
        element:DirectionalLightComponentPanel();
    elseif componentID == GTEngine.Components.AmbientLight then
        element:AmbientLightComponentPanel();
    elseif componentID == GTEngine.Components.Dynamics then
        element:DynamicsComponentPanel();
    elseif componentID == GTEngine.Components.Proximity then
        element:ProximityComponentPanel();
    elseif componentID == GTEngine.Components.ParticleSystem then
        element:ParticleSystemComponentPanel();
    elseif componentID == GTEngine.Components.Script then
        element:ScriptComponentPanel();
    elseif componentID == GTEngine.Components.EditorMetadata then 
        element:EditorMetadataComponentPanel();
    else
        Editor.SceneEditor.CreateCustomComponentPanel(element, componentID);
    end
    
    
    -- Always have the EditorMetadata panel collapsed by default.
    if componentID == GTEngine.Components.EditorMetadata then
        element:Collapse();
    end

    
    return element;
end


function GTGUI.Element:CameraComponentPanel()
    self:PanelGroupBox("Camera", true);
    
    -- FOV
    self.FOVInput = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.FOVInput:PanelNumberInput("FOV");
    self.FOVInput:UseFloatFormat();
    
    -- zNear
    self.NearInput = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.NearInput:PanelNumberInput("Near Plane");
    self.NearInput:UseFloatFormat();
    
    -- zFar
    self.FarInput = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.FarInput:PanelNumberInput("Far Plane");
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


function GTGUI.Element:ModelComponentPanel()
    self:PanelGroupBox("Model", true);
    
    -- Model
    self.ModelPath   = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    
    -- Shadows
    self.CastShadows = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:8px;' />");
    
    -- Materials
    self.MaterialsTitle     = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='width:100%; text-color:#bbb; margin-top:8px; flex-child-width:true; child-plane:horizontal; vertical-align:center;' />");
    self.MaterialsLeft      = GTGUI.Server.New("<div parentid='" .. self.MaterialsTitle:GetID() .. "' style='width:100%; height:1px; background-color:#4a4a4a;' />");
    self.MaterialsLabel     = GTGUI.Server.New("<div parentid='" .. self.MaterialsTitle:GetID() .. "' style='width:auto; text-color:#777; margin:4px 0px; vertical-align:center;'>Materials</div>");
    self.MaterialsRight     = GTGUI.Server.New("<div parentid='" .. self.MaterialsTitle:GetID() .. "' style='width:100%; height:1px; background-color:#4a4a4a;' />");
    
    self.MaterialTextBoxes = {};
    
    
    
    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;
    
    
    self.ModelPath:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            self:UpdateModel();
        end
    end);
    
    self.ModelPath:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.ModelPath:SetText(data.droppedElement.path);
            self:UpdateModel();
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
    
    
    
    function self:AddMaterialTextBox(path)
        local new = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='textbox' style='width:100%; margin:0px 2px;'>" .. path .. "</div>");
        new.index = #self.MaterialTextBoxes + 1;
        
        new:OnKeyPressed(function(data)
            if data.key == GTGUI.Keys.Enter then
                self:UpdateModelMaterial(new.index);
            end
        end)
        
        new:OnDrop(function(data)
            if data.droppedElement.isAsset then
                new:SetText(data.droppedElement.path);
                self:UpdateModelMaterial(new.index);
            end
        end)

        
        self.MaterialTextBoxes[new.index] = new;
    end
    
    
    function self:RemoveMaterials()
        for i,value in ipairs(self.MaterialTextBoxes) do
            GTGUI.Server.DeleteElement(value);
        end
        
        self.MaterialTextBoxes = {};
    end
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Model);
        
        if self.CurrentComponent ~= nil then
            self.ModelPath:SetText(self.CurrentComponent:GetModelPath());
            
            if self.CurrentComponent:IsShadowCastingEnabled() then
                self.CastShadows:Check(true);
            else
                self.CastShadows:Uncheck(true);
            end
            
            -- Materials
            self:RemoveMaterials();
            
            local materialCount = self.CurrentComponent:GetMaterialCount();
            for i=1,materialCount do
                self:AddMaterialTextBox(self.CurrentComponent:GetMaterialPath(i));
            end
        end
    end
    
    function self:UpdateModel()
        if self.CurrentComponent ~= nil and self.CurrentNode ~= nil then
            self.CurrentComponent:SetModel(self.ModelPath:GetText());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateModelMaterial(index)
        if self.CurrentComponent ~= nil and self.CurrentNode ~= nil then
            local textbox = self.MaterialTextBoxes[index];
        
            if self.CurrentComponent:SetMaterial(index, textbox:GetText()) then
                textbox:SetStyle("border-color", "#6a6a6a");
            else
                textbox:SetStyle("border-color", "#cc6a6a");
            end
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    
    return self;
end


function GTGUI.Element:PointLightComponentPanel()
    self:PanelGroupBox("Point Light", true);
    
    -- Colour
    self.ColourContainer  = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.ColourLabel      = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px; margin-right:8px;'>Colour:</div>");
    self.ColourInput      = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();
    
    -- Radius
    self.RadiusContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.RadiusLeft      = GTGUI.Server.New("<div parentid='" .. self.RadiusContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.RadiusRight     = GTGUI.Server.New("<div parentid='" .. self.RadiusContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.RadiusLabel     = GTGUI.Server.New("<div parentid='" .. self.RadiusLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Radius:</div>");
    self.RadiusInput     = GTGUI.Server.New("<div parentid='" .. self.RadiusRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
    
    -- Falloff
    self.FalloffContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()             .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.FalloffLeft      = GTGUI.Server.New("<div parentid='" .. self.FalloffContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.FalloffRight     = GTGUI.Server.New("<div parentid='" .. self.FalloffContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.FalloffLabel     = GTGUI.Server.New("<div parentid='" .. self.FalloffLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Falloff:</div>");
    self.FalloffInput     = GTGUI.Server.New("<div parentid='" .. self.FalloffRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
    
    
    
    -- Shadows
    self.CastShadows = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:8px;' />");
    
    
    
    self.CurrentNode           = nil;
    self.CurrentComponent      = nil;
    self.LockAttenuationEvents = false;
    
    
    
    self.ColourInput:OnValueChanged(function(data)
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetColour(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    
    self.RadiusInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateRadius() end;
    end);
    
    self.FalloffInput:OnTextChanged(function()
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
                self.RadiusInput:SetText( string.format("%.4f", self.CurrentComponent:GetRadius()));
                self.FalloffInput:SetText(string.format("%.4f", self.CurrentComponent:GetFalloff()));
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
            self.CurrentComponent:SetRadius(tonumber(self.RadiusInput:GetText()));
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateFalloff()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetFalloff(tonumber(self.FalloffInput:GetText()));
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateComponentAttenuation()
        if self.CurrentComponent ~= nil then
            local constant  = tonumber(self.ConstantAttenuationInput:GetText());
            local linear    = tonumber(self.LinearAttenuationInput:GetText());
            local quadratic = tonumber(self.QuadraticAttenuationInput:GetText());
            
            self.CurrentComponent:SetAttenuation(constant, linear, quadratic);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    return self;
end

function GTGUI.Element:SpotLightComponentPanel()
    self:PanelGroupBox("Spot Light", true);
    
    -- Colour
    self.ColourContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.ColourLabel     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px; margin-right:8px;'>Colour:</div>");
    self.ColourInput     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();
    
    -- Length
    self.LengthContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.LengthLeft      = GTGUI.Server.New("<div parentid='" .. self.LengthContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.LengthRight     = GTGUI.Server.New("<div parentid='" .. self.LengthContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.LengthLabel     = GTGUI.Server.New("<div parentid='" .. self.LengthLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Length:</div>");
    self.LengthInput     = GTGUI.Server.New("<div parentid='" .. self.LengthRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
    
    -- Falloff
    self.FalloffContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()             .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.FalloffLeft      = GTGUI.Server.New("<div parentid='" .. self.FalloffContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.FalloffRight     = GTGUI.Server.New("<div parentid='" .. self.FalloffContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.FalloffLabel     = GTGUI.Server.New("<div parentid='" .. self.FalloffLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Falloff:</div>");
    self.FalloffInput     = GTGUI.Server.New("<div parentid='" .. self.FalloffRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
    

    -- Angles
    self.AnglesContainer  = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:8px;' />");
    self.AnglesLeft       = GTGUI.Server.New("<div parentid='" .. self.AnglesContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.AnglesRight      = GTGUI.Server.New("<div parentid='" .. self.AnglesContainer:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.InnerAngleLabel  = GTGUI.Server.New("<div parentid='" .. self.AnglesLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px; margin-bottom:4px;'>Inner Angle:</div>");
    self.OuterAngleLabel  = GTGUI.Server.New("<div parentid='" .. self.AnglesLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding-top:2px;'                   >Outer Angle:</div>");
    
    self.InnerAngleInput  = GTGUI.Server.New("<div parentid='" .. self.AnglesRight:GetID()  .. "' styleclass='textbox' style='width:100%; margin-bottom:2px;'></div>");
    self.OuterAngleInput  = GTGUI.Server.New("<div parentid='" .. self.AnglesRight:GetID()  .. "' styleclass='textbox' style='width:100%;'></div>");
    
    
    -- Shadows
    self.CastShadows = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:8px;' />");
    
    
    
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
    
    
    self.LengthInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateLength() end;
    end);
    
    self.FalloffInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateFalloff() end;
    end);

    
    self.InnerAngleInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAnglesEvents then self:UpdateComponentAngles() end
    end);
    
    self.OuterAngleInput:OnTextChanged(function()
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
                self.LengthInput:SetText( string.format("%.4f", self.CurrentComponent:GetLength()));
                self.FalloffInput:SetText(string.format("%.4f", self.CurrentComponent:GetFalloff()));
            self.LockAttenuationEvents = false;
            
            self.LockAnglesEvents = true;
                local innerAngle, outerAngle = self.CurrentComponent:GetAngles();
                self.InnerAngleInput:SetText(string.format("%.4g", innerAngle));
                self.OuterAngleInput:SetText(string.format("%.4g", outerAngle));
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
            self.CurrentComponent:SetLength(tonumber(self.LengthInput:GetText()));
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateFalloff()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetFalloff(tonumber(self.FalloffInput:GetText()));
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateComponentAttenuation()
        if self.CurrentComponent ~= nil then
            local constant  = tonumber(self.ConstantAttenuationInput:GetText());
            local linear    = tonumber(self.LinearAttenuationInput:GetText());
            local quadratic = tonumber(self.QuadraticAttenuationInput:GetText());
            
            self.CurrentComponent:SetAttenuation(constant, linear, quadratic);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateComponentAngles()
        if self.CurrentComponent ~= nil then
            local innerAngle = tonumber(self.InnerAngleInput:GetText());
            local outerAngle = tonumber(self.OuterAngleInput:GetText());
            
            self.CurrentComponent:SetAngles(innerAngle, outerAngle);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    return self;
end

function GTGUI.Element:DirectionalLightComponentPanel()
    self:PanelGroupBox("Directional Light", true);
    
    -- Colour
    self.ColourContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.ColourLabel     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px; margin-right:8px;'>Colour:</div>");
    self.ColourInput     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();
    
    -- Shadows
    self.CastShadows = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:8px;' />");
    
    
    
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
    self.ColourContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.ColourLabel     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-right:8px;'>Colour:</div>");
    self.ColourInput     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();

    
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


function GTGUI.Element:CollisionShapesPanel()
    self.Component = nil;

    self.CollisionShapesContainer      = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' />");
    self.CollisionShapePanels          = {};
    
    self.NewCollisionShapeContainer    = GTGUI.Server.New("<div parentid='" .. self:GetID()                            .. "' styleclass='collision-shape-panel-new'       style='' />");
    self.NewCollisionShapeIcon         = GTGUI.Server.New("<div parentid='" .. self.NewCollisionShapeContainer:GetID() .. "' styleclass='collision-shape-panel-new-icon'  style='' />");
    self.NewCollisionShapeLabel        = GTGUI.Server.New("<div parentid='" .. self.NewCollisionShapeContainer:GetID() .. "' styleclass='collision-shape-panel-new-label' style=''>Add Shape</div>");
    self.NewCollisionShapeBottomBorder = GTGUI.Server.New("<div parentid='" .. self.NewCollisionShapeContainer:GetID() .. "' styleclass='collision-shape-panel-new-bottom-border' />");
    
    self.NewCollisionShapeMenu         = GTGUI.Server.New("<div parentid='" .. self:GetID()                            .. "' styleclass='collision-shape-panel-new-menu'  style='visible:false;' />");
    
    self.NewCollisionShapeContainer:OnLMBDown(function()
        if not self.NewCollisionShapeContainer.IsOpen then
            self.NewCollisionShapeContainer.IsOpen = true;
            self.NewCollisionShapeContainer:AttachStyleClass("collision-shape-panel-new-open");
            self.NewCollisionShapeIcon:AttachStyleClass("collision-shape-panel-new-icon-open");
            self.NewCollisionShapeBottomBorder:Show();
            self.NewCollisionShapeMenu:Show();
        else
            self.NewCollisionShapeContainer.IsOpen = false;
            self.NewCollisionShapeContainer:DetachStyleClass("collision-shape-panel-new-open");
            self.NewCollisionShapeIcon:DetachStyleClass("collision-shape-panel-new-icon-open");
            self.NewCollisionShapeBottomBorder:Hide();
            self.NewCollisionShapeMenu:Hide();
        end
    end);
    
    self.NewCollisionShapeMenu:NewCollisionShapeMenu();
    
    self.NewCollisionShapeMenu:AppendNewItem("Box"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddBoxCollisionShape(0.5, 0.5, 0.5);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);
    
    self.NewCollisionShapeMenu:AppendNewItem("Sphere (Does not scale. Try Ellipsoid.)"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddSphereCollisionShape(1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);

    self.NewCollisionShapeMenu:AppendNewItem("Ellipsoid"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddEllipsoidCollisionShape(1.0, 1.0, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);
    
    self.NewCollisionShapeMenu:AppendNewItem("Cylinder X"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCylinderXCollisionShape(1.0, 0.5, 0.5);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);
    
    self.NewCollisionShapeMenu:AppendNewItem("Cylinder Y"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCylinderYCollisionShape(0.0, 1.5, 0.5);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);
    
    self.NewCollisionShapeMenu:AppendNewItem("Cylinder Z"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCylinderZCollisionShape(0.0, 0.5, 1.5);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);
    
    self.NewCollisionShapeMenu:AppendNewItem("Capsule X"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCapsuleXCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);
    
    self.NewCollisionShapeMenu:AppendNewItem("Capsule Y"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCapsuleYCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);
    
    self.NewCollisionShapeMenu:AppendNewItem("Capsule Z"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCapsuleZCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end);
    
    

    function self:Update(component)
        self.Component = component;
        
        -- We need to clear the panels.
        self:Clear();
        
        -- Now we need to iterate over and create the panels for each shape.
        local shapeCount = self.Component:GetCollisionShapeCount();
        if shapeCount > 0 then
            for i=1,shapeCount do
                local shape = self.Component:GetCollisionShapeAtIndex(i);
                if shape then
                    local panel = GTGUI.Server.New("<div parentid='" .. self.CollisionShapesContainer:GetID() .. "' styleclass='collision-shape-panel' />");

                    if     shape.type == GTEngine.CollisionShapeTypes.Box              then
                        panel:CollisionShapePanel_Box():Update(shape);
                        panel:OnExtentsChanged(function(data)
                            self.Component:SetBoxCollisionShapeHalfExtents(i, data.x * 0.5, data.y * 0.5, data.z * 0.5);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.Sphere           then
                        panel:CollisionShapePanel_Sphere():Update(shape);
                        panel:OnRadiusChanged(function(data)
                            self.Component:SetSphereCollisionShapeRadius(i, data.radius);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.Ellipsoid        then
                        panel:CollisionShapePanel_Ellipsoid():Update(shape);
                        panel:OnRadiusChanged(function(data)
                            self.Component:SetEllipsoidCollisionShapeRadius(i, data.x, data.y, data.z);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CylinderX        then
                        panel:CollisionShapePanel_CylinderX():Update(shape);
                        panel:OnExtentsChanged(function(data)
                            self.Component:SetCylinderCollisionShapeHalfExtents(i, data.x, data.y, data.z);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CylinderY        then
                        panel:CollisionShapePanel_CylinderY():Update(shape);
                        panel:OnExtentsChanged(function(data)
                            self.Component:SetCylinderCollisionShapeHalfExtents(i, data.x, data.y, data.z);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CylinderZ        then
                        panel:CollisionShapePanel_CylinderZ():Update(shape);
                        panel:OnExtentsChanged(function(data)
                            self.Component:SetCylinderCollisionShapeHalfExtents(i, data.x, data.y, data.z);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CapsuleX         then
                        panel:CollisionShapePanel_CapsuleX():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCapsuleCollisionShapeSize(i, data.radius, data.height);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CapsuleY         then
                        panel:CollisionShapePanel_CapsuleY():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCapsuleCollisionShapeSize(i, data.radius, data.height);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CapsuleZ         then
                        panel:CollisionShapePanel_CapsuleZ():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCapsuleCollisionShapeSize(i, data.radius, data.height);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.ConvexHull       then
                        panel:CollisionShapePanel_ConvexHull():Update(shape);
                    elseif shape.type == GTEngine.CollisionShapeTypes.ModelConvexHulls then
                        panel:CollisionShapePanel_ModelConvexHulls():Update(shape);
                    end
                
                    panel:OnDelete(function()
                        self.Component:RemoveCollisionShapeAtIndex(i);
                        self:Update(self.Component);
                        self:OnShapesChanged();
                    end);
                    
                    panel:OnOffsetChanged(function(data)
                        if not self.IsUpdating then
                            self.Component:SetCollisionShapeOffset(i, data.x, data.y, data.z);
                            self:OnShapesChanged();
                        end
                    end);
            
                    self.CollisionShapePanels[#self.CollisionShapePanels + 1] = panel;
                end
            end
        end
    end
    
    
    function self:Clear()
        for i,value in ipairs(self.CollisionShapePanels) do
            GTGUI.Server.DeleteElement(value);
        end
        
        self.CollisionShapePanels = {};
    end
    
    
    
    -- Event called when the shapes are changed in some way. The editor needs to know about this so it can setup undo/redo points.
    function self:OnShapesChanged(arg1)
        self.Callbacks:BindOrCall("OnShapesChanged", arg1);
    end
end

function GTGUI.Element:DynamicsComponentPanel()
    self:PanelGroupBox("Dynamics", true);
    
    -- Kinematic
    self.IsKinematic = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:0px;' />");
    self.IsKinematic:CheckBox("Is Kinematic");
    
    -- Mass
    self.MassInput = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.MassInput:PanelNumberInput("Mass");
    
    -- Friction
    self.FrictionInput = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.FrictionInput:PanelNumberInput("Friction");
    
    -- Restitution
    self.RestitutionInput = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.RestitutionInput:PanelNumberInput("Restitution", 0.0);

    
    -- Linear Damping.
    self.LinearDampingInput = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.LinearDampingInput:PanelNumberInput("Linear Damping", 0.0);
    
    -- Angular Damping.
    self.AngularDampingInput = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-number-input' />");
    self.AngularDampingInput:PanelNumberInput("Angular Damping", 0.0);
    
    
    -- Collision Shapes
    self.CollisionShapesTitle = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='width:100%; text-color:#bbb; margin-top:8px; flex-child-width:true; child-plane:horizontal; vertical-align:center;' />");
    self.CollisionShapesLeft  = GTGUI.Server.New("<div parentid='" .. self.CollisionShapesTitle:GetID() .. "' style='width:100%; height:1px; background-color:#4a4a4a;' />");
    self.CollisionShapesLabel = GTGUI.Server.New("<div parentid='" .. self.CollisionShapesTitle:GetID() .. "' style='width:auto; text-color:#777; margin:4px 0px; vertical-align:center;'>Collision Shapes</div>");
    self.CollisionShapesRight = GTGUI.Server.New("<div parentid='" .. self.CollisionShapesTitle:GetID() .. "' style='width:100%; height:1px; background-color:#4a4a4a;' />");
    
    self.CollisionShapes = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' />");
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

function GTGUI.Element:ProximityComponentPanel()
    self:PanelGroupBox("Proximity", true);
    
    -- Shapes
    self.CollisionShapesTitle = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='width:100%; text-color:#bbb; margin-top:0px; flex-child-width:true; child-plane:horizontal; vertical-align:center;' />");
    self.CollisionShapesLeft  = GTGUI.Server.New("<div parentid='" .. self.CollisionShapesTitle:GetID() .. "' style='width:100%; height:1px; background-color:#4a4a4a;' />");
    self.CollisionShapesLabel = GTGUI.Server.New("<div parentid='" .. self.CollisionShapesTitle:GetID() .. "' style='width:auto; text-color:#777; margin:4px 0px; vertical-align:center;'>Shapes</div>");
    self.CollisionShapesRight = GTGUI.Server.New("<div parentid='" .. self.CollisionShapesTitle:GetID() .. "' style='width:100%; height:1px; background-color:#4a4a4a;' />");
    
    self.CollisionShapes = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' />");
    self.CollisionShapes:CollisionShapesPanel();
    
    self.CollisionShapes:OnShapesChanged(function()
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;
    self.IsUpdating       = false;
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Proximity);
        self.IsUpdating       = true;
        
        if self.CurrentComponent ~= nil then
            self.CollisionShapes:Update(self.CurrentComponent);
        end
        
        self.IsUpdating = false;
    end
    
    return self;
end




function GTGUI.Element:ParticleSystemComponentPanel()
    self:PanelGroupBox("Particle System", true);
    
    -- File Path
    self.FilePath = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    
    -- Play on Startup.
    self.PlayOnStartup = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.PlayOnStartup:CheckBox("Play on Startup");
    self.PlayOnStartup:SetStyle("margin-top", "4px");
    
    
    self.FilePath:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            self:UpdateFile();
        end
    end);
    
    self.FilePath:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.FilePath:SetText(data.droppedElement.path);
            self:UpdateFile();
        end
    end);
    
    
    
    self.PlayOnStartup:OnChecked(function(data)
        self.CurrentComponent:PlayOnStartup(true);
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    self.PlayOnStartup:OnUnchecked(function(data)
        self.CurrentComponent:PlayOnStartup(false);
        self.CurrentComponent:Reset();
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    

    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.ParticleSystem);
        
        self.FilePath:SetText(self.CurrentComponent:GetRelativeFilePath(), true);           -- 'true' = block OnTextChanged event.
        self.PlayOnStartup:SetChecked(self.CurrentComponent:IsPlayingOnStartup(), true);    -- 'true' = block OnChecked/OnUnchecked event.
    end
    
    
    function self:UpdateFile()
        self.CurrentComponent:SetFromFile(self.FilePath:GetText());
        self.ParentPanel:OnSceneNodeChanged();
    end
    

    return self;
end



function GTGUI.Element:EditorMetadataComponentPanel()
    self:PanelGroupBox("Editor Metadata");
    
    -- Show in editor.
    self.ShowInEditor       = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-bottom:8px;' />")
    
    -- Sprite
    self.ShowSprite         = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-bottom:2px;' />");
    self.SpriteTexture      = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='textbox'  style='width:100%; enabled:false;' />");
    
    -- Direction Arrow
    self.ShowDirectionArrow = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:8px;' />");
    
    
    
    self.ShowInEditor:CheckBox("Show in Editor");
    
    self.ShowInEditor:OnChecked(function()
        self:UpdateVisibility();
    end);
    
    self.ShowInEditor:OnUnchecked(function()
        self:UpdateVisibility();
    end);
    
    
    
    self.ShowSprite:CheckBox("Show Sprite");
    
    self.ShowSprite:OnChecked(function()
        self.SpriteTexture:Enable();
        self:UpdateSprite();
    end);
    
    self.ShowSprite:OnUnchecked(function()
        self.SpriteTexture:Disable();
        self:UpdateSprite();
    end);
    
    

    self.SpriteTexture:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            self:UpdateSprite();
        end
    end);
    
    self.SpriteTexture:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.SpriteTexture:SetText(data.droppedElement.path);
            self:UpdateSprite();
        end
    end);
    
    
    
    self.ShowDirectionArrow:CheckBox("Show Direction Arrow");
    
    self.ShowDirectionArrow:OnChecked(function()
        self:UpdateDirectionArrow();
    end);
    
    self.ShowDirectionArrow:OnUnchecked(function()
        self:UpdateDirectionArrow();
    end);
    
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.EditorMetadata);
        
        if self.CurrentComponent ~= nil then
            if self.CurrentNode:IsVisible() then
                self.ShowInEditor:Check(true);
            else
                self.ShowInEditor:Uncheck(true);
            end
            
            
            if self.CurrentComponent:IsShowingSprite() then
                self.ShowSprite:Check(true);
                self.SpriteTexture:Enable();
            else
                self.ShowSprite:Uncheck(true);
                self.SpriteTexture:Disable();
            end
            
            self.SpriteTexture:SetText(self.CurrentComponent:GetSpriteTexturePath() or "");
            
            
            if self.CurrentComponent:IsShowingDirectionArrow() then
                self.ShowDirectionArrow:Check(true);
            else
                self.ShowDirectionArrow:Uncheck(true);
            end
        end
    end
    
    function self:UpdateVisibility()
        if self.CurrentNode ~= nil then
            if self.ShowInEditor:IsChecked() then
                self.CurrentNode:Show();
            else
                self.CurrentNode:Hide();
            end
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateSprite()
        if self.CurrentComponent ~= nil and self.CurrentNode ~= nil then
            if self.ShowSprite:IsChecked() then
                self.CurrentComponent:ShowSprite(self.SpriteTexture:GetText());
            else
                self.CurrentComponent:HideSprite();
            end
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateDirectionArrow()
        if self.CurrentComponent ~= nil then
            if self.ShowDirectionArrow:IsChecked() then
                self.CurrentComponent:ShowDirectionArrow();
            else
                self.CurrentComponent:HideDirectionArrow();
            end
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end

    
    return self;
end



function LinkSceneEditorToSystemAPI(sceneEditor)

    -- Helper for retrieving a scene node based on the type of input.
    --
    -- 'sceneNode' can be a table (assuming a SceneNode table), pointer (assuming a SceneNode C++ pointer) or an number (assuming scene node ID).
    function sceneEditor:GetSceneNodePtr(sceneNode)
        if     type(sceneNode) == 'table'    then
            return sceneNode._internalPtr;
        elseif type(sceneNode) == 'userdata' then
            return sceneNode;
        elseif type(sceneNode) == 'number'   then
            return GTEngine.System.SceneEditor.GetSceneNodePtrByID(self._internalPtr, sceneNode);
        end
        
        return nil;
    end



    function sceneEditor:GetScenePtr()
        return GTEngine.System.SceneEditor.GetScenePtr();
    end
    
    function sceneEditor:DeselectAll()
        GTEngine.System.SceneEditor.DeselectAll(self._internalPtr);
    end
    
    function sceneEditor:SelectSceneNode(nodeToSelect, dontPostBackNotification)
        local sceneNodeToSelectPtr = self:GetSceneNodePtr(nodeToSelect);
        
        if sceneNodeToSelectPtr ~= nil then
            GTEngine.System.SceneEditor.SelectSceneNode(self._internalPtr, sceneNodeToSelectPtr, dontPostBackNotification);
        end
    end
    
    function sceneEditor:DeselectSceneNode(nodeToDeselect, dontPostBackNotification)
        local sceneNodeToDeselectPtr = self:GetSceneNodePtr(nodeToDeselect);
    
        if sceneNodeToDeselectPtr ~= nil then
            GTEngine.System.SceneEditor.DeselectSceneNode(self._internalPtr, sceneNodeToDeselectPtr, dontPostBackNotification);
        end
    end
    
    function sceneEditor:GetSelectedSceneNodeCount()
        return GTEngine.System.SceneEditor.GetSelectedSceneNodeCount(self._internalPtr);
    end
    
    function sceneEditor:GetFirstSelectedSceneNodePtr()
        return GTEngine.System.SceneEditor.GetFirstSelectedSceneNodePtr(self._internalPtr);
    end
    
    function sceneEditor:GetSelectedSceneNodeIDs()
        return GTEngine.System.SceneEditor.GetSelectedSceneNodeIDs(self._internalPtr);
    end
    
    function sceneEditor:IsSceneNodeSelectedByID(sceneNodeID)
        return GTEngine.System.SceneEditor.IsSceneNodeSelectedByID(self._internalPtr, sceneNodeID);
    end
    
    
    function sceneEditor:TryGizmoMouseSelect()
        return GTEngine.System.SceneEditor.TryGizmoMouseSelect(self._internalPtr);
    end
    
    function sceneEditor:DoMouseSelection()
        GTEngine.System.SceneEditor.DoMouseSelection(self._internalPtr);
    end
    
    
    function sceneEditor:RemoveSelectedSceneNodes()
        GTEngine.System.SceneEditor.RemoveSelectedSceneNodes(self._internalPtr);
    end
    
    function sceneEditor:RemoveSceneNode(sceneNodeToRemove)
        local sceneNodeToRemovePtr = self:GetSceneNodePtr(sceneNodeToRemove);
        
        if sceneNodeToRemovePtr ~= nil then
            GTEngine.System.SceneEditor.RemoveSceneNode(self._internalPtr, sceneNodeToRemovePtr);
        end
    end
    
    
    function sceneEditor:DuplicateSelectedSceneNodes()
        GTEngine.System.SceneEditor.DuplicateSelectedSceneNodes(self._internalPtr);
    end
    
    function sceneEditor:DuplicateSceneNode(sceneNodeToDuplicate)
        local sceneNodeToDuplicatePtr = self:GetSceneNodePtr(sceneNodeToDuplicate);
    
        if sceneNodeToDuplicatePtr ~= nil then
            GTEngine.System.SceneEditor.DuplicateSceneNode(self._internalPtr, sceneNodeToDuplicatePtr);
        end
    end
    
    
    function sceneEditor:SwitchGizmoToTranslateMode()
        GTEngine.System.SceneEditor.SwitchGizmoToTranslateMode(self._internalPtr);
    end
    
    function sceneEditor:SwitchGizmoToRotateMode()
        GTEngine.System.SceneEditor.SwitchGizmoToRotateMode(self._internalPtr);
    end
    
    function sceneEditor:SwitchGizmoToScaleMode()
        GTEngine.System.SceneEditor.SwitchGizmoToScaleMode(self._internalPtr);
    end
    
    function sceneEditor:SwitchGizmoToLocalSpace()
        GTEngine.System.SceneEditor.SwitchGizmoToLocalSpace(self._internalPtr);
    end
    
    function sceneEditor:SwitchGizmoToGlobalSpace()
        GTEngine.System.SceneEditor.SwitchGizmoToGlobalSpace(self._internalPtr);
    end
    
    function sceneEditor:ToggleGizmoSpace()
        GTEngine.System.SceneEditor.ToggleGizmoSpace(self._internalPtr);
    end
    
    function sceneEditor:IsGizmoInLocalSpace()
        return GTEngine.System.SceneEditor.IsGizmoInLocalSpace(self._internalPtr);
    end
    
    function sceneEditor:IsGizmoInGlobalSpace()
        return GTEngine.System.SceneEditor.IsGizmoInGlobalSpace(self._internalPtr);
    end
    
    
    function sceneEditor:StartPlaying()
        GTEngine.System.SceneEditor.StartPlaying(self._internalPtr);
    end
    
    function sceneEditor:PausePlaying()
        GTEngine.System.SceneEditor.PausePlaying(self._internalPtr);
    end
    
    function sceneEditor:StopPlaying()
        GTEngine.System.SceneEditor.StopPlaying(self._internalPtr);
    end
    
    function sceneEditor:IsPlaying()
        return GTEngine.System.SceneEditor.IsPlaying(self._internalPtr);
    end
    
    function sceneEditor:IsPaused()
        return GTEngine.System.SceneEditor.IsPaused(self._internalPtr);
    end
    
    
    function sceneEditor:EnablePhysicsSimulation()
        GTEngine.System.SceneEditor.EnablePhysicsSimulation(self._internalPtr);
    end
    
    function sceneEditor:DisablePhysicsSimulation()
        GTEngine.System.SceneEditor.DisablePhysicsSimulation(self._internalPtr);
    end
    
    function sceneEditor:IsPhysicsSimulationEnabled()
        return GTEngine.System.SceneEditor.IsPhysicsSimulationEnabled(self._internalPtr);
    end
    
    
    function sceneEditor:CommitStateStackFrame()
        GTEngine.System.SceneEditor.CommitStateStackFrame(self._internalPtr);
    end
    
    function sceneEditor:PushUndoRedoPoint()
        sceneEditor:CommitStateStackFrame();
    end
    
    function sceneEditor:Undo()
        GTEngine.System.SceneEditor.Undo(self._internalPtr);
    end
    
    function sceneEditor:Redo()
        GTEngine.System.SceneEditor.Redo(self._internalPtr);
    end
    
    
    function sceneEditor:InstantiatePrefab(relativePath)
        local rootSceneNodePtr = GTEngine.System.SceneEditor.InstantiatePrefab(self._internalPtr, relativePath);
        
        GTEngine.System.SceneEditor.PositionSceneNodeInFrontOfCamera(self._internalPtr, rootSceneNodePtr);
        
        return rootSceneNodePtr;
    end
    
    function sceneEditor:LinkSceneNodeToPrefab(sceneNode, prefabRelativePath)
        return GTEngine.System.SceneEditor.LinkSceneNodeToPrefab(self._internalPtr, sceneEditor:GetSceneNodePtr(sceneNode), prefabRelativePath);
    end
    
    function sceneEditor:UnlinkSceneNodeFromPrefab(sceneNode)
        return GTEngine.System.SceneEditor.UnlinkSceneNodeFromPrefab(self._internalPtr, sceneEditor:GetSceneNodePtr(sceneNode));
    end
    
    
    function sceneEditor:GetSceneNodePtrByID(sceneNodeID)
        return GTEngine.System.SceneEditor.GetSceneNodePtrByID(self._internalPtr, sceneNodeID);
    end
    
    function sceneEditor:GetSceneNodes()
        return GTEngine.System.SceneEditor.GetSceneNodes(self._internalPtr);
    end
    
    
    function sceneEditor:PositionSceneNodeInFrontOfCamera(sceneNode)
        local sceneNodePtr = self:GetSceneNodePtr(sceneNode);
    
        if sceneNodePtr ~= nil then
            GTEngine.System.SceneEditor.PositionSceneNodeInFrontOfCamera(self._internalPtr, sceneNodePtr);
        end
    end
    
    
    function sceneEditor:ShowGrid()
        GTEngine.System.SceneEditor.ShowGrid(self._internalPtr);
    end
    
    function sceneEditor:HideGrid()
        GTEngine.System.SceneEditor.HideGrid(self._internalPtr);
    end
    
    function sceneEditor:IsShowingGrid()
        return GTEngine.System.SceneEditor.IsShowingGrid(self._internalPtr);
    end
    
    
    function sceneEditor:ShowAxisArrows()
        GTEngine.System.SceneEditor.ShowAxisArrows(self._internalPtr);
    end
    
    function sceneEditor:HideAxisArrows()
        GTEngine.System.SceneEditor.HideAxisArrows(self._internalPtr);
    end
    
    function sceneEditor:IsShowingAxisArrows()
        return GTEngine.System.SceneEditor.IsShowingAxisArrows(self._internalPtr);
    end
    
    
    function sceneEditor:ResetCamera()
        return GTEngine.System.SceneEditor.ResetCamera(self._internalPtr);
    end
    
    
    
    function sceneEditor:SetSceneName(name)
        GTEngine.System.SceneEditor.SetSceneName(self._internalPtr, name);
    end
    
    function sceneEditor:GetSceneName()
        return GTEngine.System.SceneEditor.GetSceneName(self._internalPtr);
    end
    
    
    function sceneEditor:EnableSceneBackgroundClearing(colour)
        GTEngine.System.SceneEditor.EnableSceneBackgroundClearing(self._internalPtr, colour);
    end
    
    function sceneEditor:DisableSceneBackgroundClearing()
        GTEngine.System.SceneEditor.DisableSceneBackgroundClearing(self._internalPtr);
    end
    
    function sceneEditor:IsSceneBackgroundClearingEnabled()
        return GTEngine.System.SceneEditor.IsSceneBackgroundClearingEnabled(self._internalPtr);
    end
    
    function sceneEditor:GetSceneBackgroundClearColour()
        return GTEngine.System.SceneEditor.GetSceneBackgroundClearColour(self._internalPtr);
    end
    
    
    function sceneEditor:EnableSceneHDR(colour)
        GTEngine.System.SceneEditor.EnableSceneHDR(self._internalPtr, colour);
    end
    
    function sceneEditor:DisableSceneHDR(colour)
        GTEngine.System.SceneEditor.DisableSceneHDR(self._internalPtr, colour);
    end
    
    function sceneEditor:IsSceneHDREnabled()
        return GTEngine.System.SceneEditor.IsSceneHDREnabled(self._internalPtr);
    end
    
    
    function sceneEditor:EnableSceneBloom(colour)
        GTEngine.System.SceneEditor.EnableSceneBloom(self._internalPtr, colour);
    end
    
    function sceneEditor:DisableSceneBloom(colour)
        GTEngine.System.SceneEditor.DisableSceneBloom(self._internalPtr, colour);
    end
    
    function sceneEditor:IsSceneBloomEnabled()
        return GTEngine.System.SceneEditor.IsSceneBloomEnabled(self._internalPtr);
    end
    
    
    function sceneEditor:SetSceneNodeTransformToCamera(sceneNode)
        return GTEngine.System.SceneEditor.SetSceneNodeTransformToCamera(self._internalPtr, sceneNode._internalPtr);
    end
end


function GTGUI.Element:SceneEditor(_internalPtr)
    self:SubEditor(_internalPtr);
    LinkSceneEditorToSystemAPI(self);
    
    self.Viewport        = GTGUI.Server.CreateElement(self, "scene-editor-viewport");
    self.Viewport:SceneEditorViewport();
    
    self.Panel           = GTGUI.Server.CreateElement(self, "scene-editor-panel");
    self.Panel:SceneEditorPanel(self);
    self.PropertiesPanel      = self.Panel.PropertiesPanel;
    self.HierarchyPanel       = self.Panel.HierarchyPanel;
    self.ScenePropertiesPanel = self.Panel.ScenePropertiesPanel;
    
    self.ToolBar:SceneEditorToolBar(self);
    
    self.ContextMenu     = GTGUI.Server.CreateElement(nil, "scene-editor-context-menu");
    self.ContextMenu:SceneEditorContextMenu(self);
    

    self.Scene                   = GTEngine.RegisteredScenes[GTEngine.System.SceneEditor.GetScenePtr(_internalPtr)];
    self.SelectedSceneNode       = nil;
    self.IsLMBDown               = false;
    self.IsRMBDown               = false;
    self.MouseMovedWhileCaptured = false;                -- Used to determine whether or not to show the right-click context menu.
    self.HasMouseCapture         = false;
    self.IsMouseOverViewport     = false;
    

    
    -- Updates the playback control buttons.
    function self:UpdatePlaybackControls()
        self.ToolBar:UpdatePlaybackControls();
    end

    
    
    function self:GetSelectedSceneNode()
        return self.SelectedSceneNode;
    end
    
    function self:SetSelectedSceneNode(sceneNodeInternalPtr)
        if sceneNodeInternalPtr ~= nil then
            self.SelectedSceneNode = self.Scene:GetSceneNodeByID(GTEngine.System.SceneNode.GetID(sceneNodeInternalPtr));
        else
            self.SelectedSceneNode = nil;
        end
    end
    
    function self:DeleteSelectedSceneNodes()
        self:RemoveSelectedSceneNodes();
    end
    
    function self:DeleteSceneNode(sceneNode)
        self:RemoveSceneNode(sceneNode);
        self:CommitStateStackFrame();
    end
    
    
    function self:OrphanSceneNode(sceneNode)
        local sceneNodePtr = self:GetSceneNodePtr(sceneNode);
    
        if sceneNodePtr ~= nil then
            GTEngine.System.SceneNode.Orphan(sceneNodePtr, true);
        end
    end
    
    
    function self:GetParentSceneNodePtr(sceneNode)
        local sceneNodePtr = self:GetSceneNodePtr(sceneNode);
    
        if sceneNodePtr ~= nil then
            return GTEngine.System.SceneNode.GetParentPtr(sceneNodePtr);
        end
    end
    
    function self:GetParentSceneNodeID(sceneNode)
        return GTEngine.System.SceneNode.GetID(self:GetParentSceneNodePtr(sceneNode));
    end

    
    
    function self:HidePropertyPanels(message)
        self.PropertiesPanel:HidePanels(message);
    end
    
    function self:ShowPropertyPanels()
        self.PropertiesPanel:ShowPanels();
    end
    
    function self:UpdatePropertyPanels()
        self.PropertiesPanel:Update(self:GetSelectedSceneNode());
    end
    
    function self:UpdatePropertiesTransformPanel()
        self.PropertiesPanel:UpdateTransformPanel();
    end
    
    function self:UpdateScriptProperties()
        self.PropertiesPanel:UpdateScriptProperties();
    end

    
    function self:UpdateViewportMenu()
        self.ToolBar:UpdateViewportMenu();
    end
    
    
    function self:RefreshSceneProperties()
        self.ScenePropertiesPanel:Refresh();
    end
    
    
    
    function self:OnSelectionChanged()
        local selectedNodeCount = self:GetSelectedSceneNodeCount();
        
        if selectedNodeCount > 1 then        -- Multiple selections
            self:SetSelectedSceneNode(nil);
            self:HidePropertyPanels("Multiple Objects Selected");
        elseif selectedNodeCount == 0 then   -- Nothing selected
            self:SetSelectedSceneNode(nil);
            self:HidePropertyPanels("Nothing Selected");
        else                                 -- Single selection
            self:SetSelectedSceneNode(GTEngine.System.SceneEditor.GetFirstSelectedSceneNodePtr(self._internalPtr));
            self:UpdatePropertyPanels();
            self:ShowPropertyPanels();
        end
        
        
        self.HierarchyPanel:SetSelectedItemsBySceneNodeIDs(self:GetSelectedSceneNodeIDs(), true);
    end
    
    function self:OnSceneNodeAdded(sceneNodePtr)
        self.HierarchyPanel:AddSceneNode(sceneNodePtr);
    end
    
    function self:OnSceneNodeRemoved(sceneNodePtr)
        self.HierarchyPanel:RemoveSceneNode(sceneNodePtr);
    end
    
    
    function self:OnSceneNodeChanged(sceneNodePtr)
        self:CommitStateStackFrame();
    end
    
    function self:OnSceneNodeNameChanged(sceneNodePtr)
        self.HierarchyPanel:UpdateSceneNode(sceneNodePtr);
    end
    
    function self:OnSceneNodeParentChanged(sceneNodePtr)
        self.HierarchyPanel:RemoveSceneNode(sceneNodePtr);
        self.HierarchyPanel:AddSceneNode(sceneNodePtr);
    end
    
    function self:OnSceneNodePrefabChanged(sceneNodePtr)
        self.HierarchyPanel:UpdateSceneNode(sceneNodePtr);
    end
    
    

    
    self.Viewport:OnMouseEnter(function()
        self.IsMouseOverViewport = true;
    end);
    
    self.Viewport:OnMouseLeave(function()
        self.IsMouseOverViewport = false;
    end);
    
    self.Viewport:OnDrop(function(data)
        if data.droppedElement.isAsset then
            if GTEngine.IsModelFile(data.droppedElement.path) then
                local newNode = self.Scene:CreateNewSceneNode();
                newNode:SetName("Model");
                newNode:AddComponent(GTEngine.Components.EditorMetadata);
                newNode:AddComponent(GTEngine.Components.Model):SetModel(data.droppedElement.path);
                
                self:PositionSceneNodeInFrontOfCamera(newNode);
                
                self:DeselectAll();
                self:SelectSceneNode(newNode);
                
                self:CommitStateStackFrame();
            elseif GTEngine.IsPrefabFile(data.droppedElement.path) then
                local newNodePtr = self:InstantiatePrefab(data.droppedElement.path);
                
                self:DeselectAll();
                GTEngine.System.SceneEditor.SelectSceneNode(self._internalPtr, newNodePtr, false);
                
                self:CommitStateStackFrame();
            end
        end
    end);
    
    
    
    
    
    self.Viewport:OnLMBDown(function()
        self.IsLMBDown       = true;
        self.HasMouseCapture = true;
        self:Focus();
        
        -- We will check for a selection on the gizmo. If we click on a gizmo, we don't want to do a mouse selection when the 
        -- button is raised. What we'll do is trick it into thinking the mouse was moved while it was captured which will
        -- cause the editor to not try and select anything.
        if self:TryGizmoMouseSelect() then
            self.MouseMovedWhileCaptured = true;
        end
    end);
    
    self.Viewport:OnRMBDown(function()
        self.IsRMBDown       = true;
        self.HasMouseCapture = true;
        self:Focus();
    end);

    
    
    self:WatchLMBDown(function(data)
        if not self.ContextMenu:IsChild(data.receiver) then self.ContextMenu:Hide(); end
        
        if self.HasMouseCapture then
            self.IsLMBDown = true;
        end
    end);
    
    self:WatchRMBDown(function(data)
        if not self.ContextMenu:IsChild(data.receiver) then self.ContextMenu:Hide(); end
        
        if self.HasMouseCapture then
            self.IsRMBDown = true;
        end
    end);
    
    self:WatchLMBUp(function(data)
        if not self.MouseMovedWhileCaptured and self.HasMouseCapture and not self.IsRMBDown then
            self:DoMouseSelection();
        end
        
        if not GTGUI.Server.IsRMBDown() then
            Game.ReleaseMouse();
            self.HasMouseCapture         = false;
            self.MouseMovedWhileCaptured = false;
        end
        
        self.IsLMBDown = false;
    end);
    
    self:WatchRMBUp(function(data)
        if not self.MouseMovedWhileCaptured and self.HasMouseCapture and not self.IsLMBDown then
            self.ContextMenu:SetPosition(GTGUI.Server.GetMousePosition());
            self.ContextMenu:Show();
        end
        
        if not GTGUI.Server.IsLMBDown() then
            Game.ReleaseMouse();
            self.HasMouseCapture         = false;
            self.MouseMovedWhileCaptured = false;
        end
        
        self.IsRMBDown = false;
    end);
    
    self:WatchMouseMove(function(data)
        if self.HasMouseCapture then
            self.MouseMovedWhileCaptured = true;
        end
    end);
    
    
    self:WatchKeyPressed(function(data)
        if self.IsMouseOverViewport and not GTGUI.Server.DoesFocusedElementHaveEditableText() and not self:IsPlaying() then
            if not GTGUI.Server.IsCTRLKeyDown() then
                if data.key == GTCore.Keys.Delete then
                    self:DeleteSelectedSceneNodes();
                elseif data.key == GTCore.Keys.T then
                    self:SwitchGizmoToTranslateMode();
                elseif data.key == GTCore.Keys.R then
                    self:SwitchGizmoToRotateMode();
                elseif data.key == GTCore.Keys.S then
                    self:SwitchGizmoToScaleMode();
                elseif data.key == GTCore.Keys.L then
                    self:SwitchGizmoToLocalSpace();
                elseif data.key == GTCore.Keys.G then
                    self:SwitchGizmoToGlobalSpace();
                elseif data.key == GTCore.Keys.Q then
                    self:ToggleGizmoSpace();
                end
            else
                if data.key == GTCore.Keys.D then
                    self:DuplicateSelectedSceneNodes();           -- This will deselect the source nodes and select the new ones.
                elseif data.key == GTCore.Keys.Z then
                    self:Undo();
                elseif data.key == GTCore.Keys.Y then
                    self:Redo();
                end
            end
        end
    end);
    
    
    
    -- We're going to hide the panels by default since nothing is selected right now.
    self:HidePropertyPanels("Nothing Selected");
    
    return self;
end















