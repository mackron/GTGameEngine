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
                self.OffsetInput:SetFromXYZ(shape.offsetX, shape.offsetY, shape.offsetZ);
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
        self.ExtentsInput:SetFromXYZ(shape.halfX * 2.0, shape.halfY * 2.0, shape.halfZ * 2.0);
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
        self.RadiusInput:SetFromXYZ(shape.radiusX, shape.radiusY, shape.radiusZ);
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
        self.ExtentsInput:SetFromXYZ(shape.halfX * 2.0, shape.halfY * 2.0, shape.halfZ * 2.0);
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
    
    
    if componentID == GTEngine.Components.Model then
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

function GTGUI.Element:CameraComponentPanel()
    self:PanelGroupBox("Camera", true);
    
    function self:Update(node)
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
            self.ColourInput:SetFromXYZ(self.CurrentComponent:GetColour());
            
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
            self.ColourInput:SetFromXYZ(self.CurrentComponent:GetColour());
            
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
            self.ColourInput:SetFromXYZ(self.CurrentComponent:GetColour());
            
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
            self.ColourInput:SetFromXYZ(self.CurrentComponent:GetColour());
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
    
    -- Mass
    self.IsKinematic   = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:0px;' />");
    
    self.MassContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()          .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.MassLeft      = GTGUI.Server.New("<div parentid='" .. self.MassContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.MassRight     = GTGUI.Server.New("<div parentid='" .. self.MassContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.MassLabel     = GTGUI.Server.New("<div parentid='" .. self.MassLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Mass (0 = Static):</div>");
    self.MassInput     = GTGUI.Server.New("<div parentid='" .. self.MassRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
    
    -- Friction
    self.FrictionContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()              .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.FrictionLeft      = GTGUI.Server.New("<div parentid='" .. self.FrictionContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.FrictionRight     = GTGUI.Server.New("<div parentid='" .. self.FrictionContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.FrictionLabel     = GTGUI.Server.New("<div parentid='" .. self.FrictionLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Friction:</div>");
    self.FrictionInput     = GTGUI.Server.New("<div parentid='" .. self.FrictionRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
    
    -- Restitution
    self.RestitutionContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.RestitutionLeft      = GTGUI.Server.New("<div parentid='" .. self.RestitutionContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.RestitutionRight     = GTGUI.Server.New("<div parentid='" .. self.RestitutionContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.RestitutionLabel     = GTGUI.Server.New("<div parentid='" .. self.RestitutionLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Resitution:</div>");
    self.RestitutionInput     = GTGUI.Server.New("<div parentid='" .. self.RestitutionRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");

    
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

    
    
    self.IsKinematic:CheckBox("Is Kinematic");
    
    self.IsKinematic:OnChecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:IsKinematic(true);
            self.MassLabel:SetStyle("text-color", "#5a5a5a");
            self.MassInput:Disable();
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    self.IsKinematic:OnUnchecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:IsKinematic(false);
            self.MassLabel:SetStyle("text-color", "std-text-color");
            self.MassInput:Enable();
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    
    self.MassInput:OnTextChanged(function(data)
        if not self.IsUpdating then
            self:ApplyMass();
        end
    end);
    
    self.FrictionInput:OnTextChanged(function(data)
        if not self.IsUpdating then
            self:ApplyFriction();
        end
    end);
    
    self.RestitutionInput:OnTextChanged(function(data)
        if not self.IsUpdating then
            self:ApplyRestitution();
        end
    end);
    
    self.LinearDampingInput:OnValueChanged(function(data)
        if not self.IsUpdating then
            if data.value > 1.0 then
                self.LinearDampingInput:SetValue(1.0);
            end
        
            self:ApplyLinearDamping();
        end
    end);
    
    self.AngularDampingInput:OnValueChanged(function(data)
        if not self.IsUpdating then
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
                self.MassLabel:SetStyle("text-color", "#5a5a5a");
                self.MassInput:Disable();
            else
                self.IsKinematic:Uncheck(true);
                self.MassLabel:SetStyle("text-color", "std-text-color");
                self.MassInput:Enable();
            end
            
            self.MassInput:SetText(       string.format("%.4f", self.CurrentComponent:GetMass()));
            self.FrictionInput:SetText(   string.format("%.4f", self.CurrentComponent:GetFriction()));
            self.RestitutionInput:SetText(string.format("%.4f", self.CurrentComponent:GetRestitution()));
            self.LinearDampingInput:SetValue( self.CurrentComponent:GetLinearDamping());
            self.AngularDampingInput:SetValue(self.CurrentComponent:GetAngularDamping());
            
            -- Collision shapes need to be updated.
            self.CollisionShapes:Update(self.CurrentComponent);
        end
        
        self.IsUpdating = false;
    end
    
    
    function self:ApplyMass()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetMass(tonumber(self.MassInput:GetText()));
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:ApplyFriction()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetFriction(tonumber(self.FrictionInput:GetText()));
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:ApplyRestitution()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetRestitution(tonumber(self.RestitutionInput:GetText()));
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


function GTGUI.Element:ScriptVariableContainer(name, component)
    self.Component = component;
    self.Name      = name;
    
    self.NameValueContainer = GTGUI.Server.New("<div parentid='" .. self:GetID()                    .. "' styleclass='' style='child-plane:horizontal; flex-child-width:true; vertical-align:center;' />");
    self.NameContainer      = GTGUI.Server.New("<div parentid='" .. self.NameValueContainer:GetID() .. "' styleclass='' style='width:auto; vertical-align:center; padding-right:4px;' />");
    self.ValueContainer     = GTGUI.Server.New("<div parentid='" .. self.NameValueContainer:GetID() .. "' styleclass='' style='width:100%; vertical-align:center; horizontal-align:right;' />");
    
    self.NameLabel = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' styleclass='' style='width:auto;' />");
    self.NameLabel:SetText(name .. ":");
    
    
    function self:OnValueChanged(arg1)
        self.Callbacks:BindOrCall("OnValueChanged", arg1);
    end
end

function GTGUI.Element:ScriptVariableContainer_Number(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueTextBox:SetValue(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueTextBox = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='textbox' style='width:100%; max-width:72px' />");
    self.ValueTextBox:NumberTextBox();
    self:UpdateValue();
    
    self.ValueTextBox:OnTextChanged(function()
        self.Component:SetPublicVariableValue(self.Name, tonumber(self.ValueTextBox:GetText()) or 0.0);
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_Vec2(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueInput:SetFromXYTable(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueInput = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='vector-input' style='width:100%; max-width:146px' />");
    self.ValueInput:Vector2Input()
    self:UpdateValue();
    
    self.ValueInput:OnValueChanged(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:GetXYTable());
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_Vec3(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueInput:SetFromXYZTable(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueInput = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='vector-input' style='width:100%; max-width:220px' />");
    self.ValueInput:Vector3Input()
    self:UpdateValue();
    
    self.ValueInput:OnValueChanged(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:GetXYZTable());
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_Vec4(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueInput:SetFromXYZWTable(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueInput = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='vector-input' style='width:100%; max-width:296px' />");
    self.ValueInput:Vector4Input();
    self:UpdateValue();
    
    self.ValueInput:OnValueChanged(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:GetXYZWTable());
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_Boolean(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        self.ValueInput:SetChecked(component:GetPublicVariableValue(name));
    end
    
    
    self.ValueInput = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='checkbox' style='' />");
    self.ValueInput:CheckBox();
    self.ValueInput.text:Hide();        -- This removes the little margin so that it lies flush with the edge.
    self:UpdateValue();
    
    
    self.ValueInput:OnChecked(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:IsChecked());
        self:OnValueChanged();
    end);
    
    self.ValueInput:OnUnchecked(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueInput:IsChecked());
        self:OnValueChanged();
    end);
end

function GTGUI.Element:ScriptVariableContainer_String(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        local newValue = component:GetPublicVariableValue(name);
        if self.ValueTextBox:GetText() ~= newValue then
            self.ValueTextBox:SetText(newValue);
        end
    end
    
    
    self.ValueTextBox = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    self:UpdateValue();
    
    self.ValueTextBox:OnTextChanged(function()
        self.Component:SetPublicVariableValue(self.Name, self.ValueTextBox:GetText() or "");
        self:OnValueChanged();
    end);
    
    self.ValueTextBox:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.ValueTextBox:SetText(data.droppedElement.path);
            -- TODO: When we implement the multi-line text box feature for strings we'll want to do explicit SetPublicVariableValue() and OnValueChanged commands.
        end
    end);
end

function GTGUI.Element:ScriptVariableContainer_Prefab(name, component)
    self:ScriptVariableContainer(name, component);
    
    function self:UpdateValue()
        local newValue = component:GetPublicVariableValue(name);
        if self.ValueTextBox:GetText() ~= newValue then
            self.ValueTextBox:SetText(newValue);
        end
    end
    
    
    self.ValueTextBox = GTGUI.Server.New("<div parentid='" .. self.ValueContainer:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    self:UpdateValue();
    
    self.ValueTextBox:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            self.Component:SetPublicVariableValue(self.Name, self.ValueTextBox:GetText() or "");
            self:OnValueChanged();
        end
    end);
    
    self.ValueTextBox:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.ValueTextBox:SetText(data.droppedElement.path);
            self.Component:SetPublicVariableValue(self.Name, self.ValueTextBox:GetText() or "");
            self:OnValueChanged();
        end
    end);
end


function GTGUI.Element:ScriptComponentPanel()
    self:PanelGroupBox("Script", true);
    self.ScriptsContainer   = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass=''                  style='' />");

    
    self.NewScriptContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()               .. "' styleclass='new-script-container' style='margin-top:4px' />");
    self.NewScriptTextBox   = GTGUI.Server.New("<div parentid='" .. self.NewScriptContainer:GetID() .. "' styleclass='new-script-textbox'   style='' />");
    self.NewScriptIcon      = GTGUI.Server.New("<div parentid='" .. self.NewScriptContainer:GetID() .. "' styleclass='new-script-icon'      style='' />");
    self.ScriptPanels       = {};
    
    self.CurrentNode        = nil;
    self.CurrentComponent   = nil;
    self.IsUpdating         = false;
    
    
    
    self.NewScriptTextBox:OnDrop(function(data)
        if data.droppedElement.isAsset then
            if not self.CurrentComponent:IsUsingScript(data.droppedElement.path) then
                if self:AddScript(data.droppedElement.path) then
                    self.NewScriptTextBox:SetText("");
                end
            end
        end
    end);
    
    
    self.NewScriptTextBox:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            if self:AddScript(self.NewScriptTextBox:GetText()) then
                self.NewScriptTextBox:SetText("");
            end
        end
    end);
    
    self.NewScriptIcon:OnPressed(function(data)
        if self:AddScript(self.NewScriptTextBox:GetText()) then
            self.NewScriptTextBox:SetText("");
        end
    end);
    
    
    
    
    function self:AddScript(name)
        -- If we're not updating we want to add the script to the component itself.
        if not self.IsUpdating then
            if not self.CurrentComponent:IsUsingScript(name) then
                if self.CurrentComponent:AddScript(name) then
                    self.ParentPanel:OnSceneNodeChanged();
                else
                    return false;
                end
            else
                return false;
            end
        end
        
    
        local new = GTGUI.Server.New("<div parentid='" .. self.ScriptsContainer:GetID() .. "' styleclass='' style='margin-bottom:4px;' />");
        
        ---------------------------------
        -- Header
        new.Header             = GTGUI.Server.New("<div parentid='" .. new:GetID()        .. "' styleclass=''                           style='child-plane:horizontal; flex-child-width:true; vertical-align:center;' />");
        new.FilePathTextBox    = GTGUI.Server.New("<div parentid='" .. new.Header:GetID() .. "' styleclass=''                    style='width:100%; text-color:std-text-color; font-style:bold;' />");
        new.CloseButton        = GTGUI.Server.New("<div parentid='" .. new.Header:GetID() .. "' styleclass='panel-groupbox-title-cross' style='margin-left:4px;' />");
        
        new.FilePathTextBox:SetText(name);
        
        
        
        new.CloseButton:OnPressed(function()
            self:RemoveScript(new);
        end);
        
        

        self.ScriptPanels[#self.ScriptPanels + 1] = new;
        
        
        
        
        
        ---------------------------------
        -- Variables.
        --
        -- These need to be done after adding the script to the component.
        new.VariablesContainer = GTGUI.Server.New("<div parentid='" .. new:GetID() .. "' styleclass='' style='padding:4px; padding-left:8px; padding-right:0px; border-bottom:1px #444' />");
        new.Variables          = {};
        
        -- We initialize the variables by just "reloading" them.
        self:ReloadVariables(new);
        
        return true;    -- Successful.
    end
    
    
    function self:RemoveScript(panelToRemove)
        for i,panel in ipairs(self.ScriptPanels) do
            if panel == panelToRemove then
                self:RemoveScriptByIndex(i);
                return;
            end
        end
    end
    
    function self:RemoveScriptByRelativePath(relativePath)
        for i,panel in ipairs(self.ScriptPanels) do
            if panel.RelativePath == relativePath then
                self:RemoveScriptByIndex(i);
                return;
            end
        end
    end
    
    function self:RemoveScriptByIndex(index)
        local panel = self.ScriptPanels[index];
        if panel ~= nil then
            table.remove(self.ScriptPanels, index);
            
        
            -- If we're not updating we want to remove the script the component itself.
            if not self.IsUpdating then
                self.CurrentComponent:RemoveScriptByIndex(index);
                self.ParentPanel:OnSceneNodeChanged();
            end
        
        
            -- And now we can remove the panel completely.
            GTGUI.Server.DeleteElement(panel);
        end
    end
    
    function self:RemoveAllScripts()
        for i,panel in ipairs(self.ScriptPanels) do
            GTGUI.Server.DeleteElement(panel);
        end
        
        self.ScriptPanels = {};
    end
    

    
    function self:ReloadVariables(panel)
        panel.VariablesContainer:DeleteAllChildren();
        panel.Variables = {};
    
        -- We need to grab the names and types of the public variables in the given script.
        local publicVariableNamesAndTypes = self.CurrentComponent:GetPublicVariableNamesAndTypesByIndex(#self.ScriptPanels);
        for i,nameAndType in ipairs(publicVariableNamesAndTypes) do
            if nameAndType.type ~= GTEngine.ScriptVariableTypes.Unknown and nameAndType.type ~= GTEngine.ScriptVariableTypes.None then
                local variableContainer = GTGUI.Server.New("<div parentid='" .. panel.VariablesContainer:GetID() .. "' styleclass='script-variable-container' style='' />");
            
                if     nameAndType.type == GTEngine.ScriptVariableTypes.Number  then variableContainer:ScriptVariableContainer_Number( nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec2    then variableContainer:ScriptVariableContainer_Vec2(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec3    then variableContainer:ScriptVariableContainer_Vec3(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Vec4    then variableContainer:ScriptVariableContainer_Vec4(   nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Boolean then variableContainer:ScriptVariableContainer_Boolean(nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.String  then variableContainer:ScriptVariableContainer_String( nameAndType.name, self.CurrentComponent);
                elseif nameAndType.type == GTEngine.ScriptVariableTypes.Prefab  then variableContainer:ScriptVariableContainer_Prefab( nameAndType.name, self.CurrentComponent);
                end
                
                variableContainer:OnValueChanged(function()
                    self.ParentPanel:OnSceneNodeChanged();
                end);
                
                panel.Variables[#panel.Variables + 1] = variableContainer;
            end
        end
    end
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Script);
        self.IsUpdating       = true;
        
        
        self:RemoveAllScripts();
        
        for i,scriptRelativePath in ipairs(self.CurrentComponent:GetScriptFilePaths()) do
            self:AddScript(scriptRelativePath);
        end
        
        
        self.IsUpdating       = false;
    end
    
    function self:UpdateVariables()
        for i,panel in ipairs(self.ScriptPanels) do
            for i,variableContainer in ipairs(panel.Variables) do
                variableContainer:UpdateValue();
            end
        end
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



function GTGUI.Element:SceneEditorDetailsPanel(parentPanel)
    self:PanelGroupBox("Details");
    self.ParentPanel = parentPanel;
    
    self.NameContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()          .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; vertical-align:center;' />");
    self.NameLabel     = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' style='width:auto; margin-right:4px; text-color:std-text-color;'>Name:</div>");
    self.NameTextBox   = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    
    self.SceneNode = nil;
    
    
    function self:Update(node)
        self.SceneNode = node;
        self.NameTextBox:SetText(node:GetName());
    end
    
    
    self.NameTextBox:OnKeyPressed(function(data)
        if data.key == GTCore.Keys.Enter then
            self.SceneNode:SetName(self.NameTextBox:GetText());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    return self;
end

function GTGUI.Element:SceneEditorTransformPanel(parentPanel)
    self:PanelGroupBox("Transform");
    self.ParentPanel = parentPanel;
    
    self.Container = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()      .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.Left      = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.Right     = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.PositionLabel = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px;'>Position:</div>");
    self.RotationLabel = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px;'>Rotation:</div>");
    self.ScaleLabel    = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; padding-bottom:0px'>Scale:</div>");
    
    self.PositionInput = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true; margin-bottom:4px;' />"):Vector3Input();
    self.RotationInput = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true; margin-bottom:4px;' />"):Vector3Input();
    self.ScaleInput    = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();
    
    self.SceneNode = nil;

    
    function self:Update(node)
        self.SceneNode = node;
        
        self.PositionInput:SetFromXYZ(node:GetPosition());
        self.RotationInput:SetFromXYZ(node:GetRotationXYZ());
        self.ScaleInput:SetFromXYZ(node:GetScale());
    end
    
    
    
    self.PositionInput:OnValueChanged(function(data)
        if self.SceneNode ~= nil then
            self.SceneNode:SetPosition(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    self.RotationInput:OnValueChanged(function(data)
        if self.SceneNode ~= nil then
            self.SceneNode:SetRotationXYZ(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    self.ScaleInput:OnValueChanged(function(data)
        if self.SceneNode ~= nil then
            self.SceneNode:SetScale(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    return self;
end


function GTGUI.Element:SceneEditorPropertiesPanel(sceneEditor)
    self.Body      = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='scene-editor-properties-panel-body' style='' />");
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='vertical-scrollbar'      style='' />");
    
    self.Body.MessageContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%; margin-top:8px; horizontal-align:center; font-style:bold; text-color:#555; visible:false;' />")
    
    
    self.Body.PanelsContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%;' />")
    
    self.Body.DetailsPanel   = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    self.Body.DetailsPanel:SceneEditorDetailsPanel(self);

    self.Body.TransformPanel = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    self.Body.TransformPanel:SceneEditorTransformPanel(self);
    
    
    self.CurrentSceneNode = nil;            -- The scene node whose details are currently being shown on this panel.
    self.SceneEditor      = sceneEditor;
    
    self.IsMouseOver           = false;
    self.MouseWheelScrollSpeed = 32;
    
    
    
    -- This will create the panels for every registered component. We always want the editor metadata to be last.
    self.ComponentPanels = {};
    for key,value in pairs(GTEngine.Components) do
        if value ~= GTEngine.Components.EditorMetadata then
            local panel = Editor.SceneEditor.CreateComponentPanel(self, value);
            self.ComponentPanels[value] = panel;
            
            -- We need to attach an event handler to each component panel for when it is closed. When closing a component panel, that component
            -- will be removed from the scene node. Note that closing a panel is not the same as collapsing it.
            panel:OnClose(function()
                self.CurrentSceneNode:RemoveComponent(value);

                self:UpdateComponentPanels();
                self:OnSceneNodeChanged();
            end);
        end
    end
    
    self.ComponentPanels[GTEngine.Components.EditorMetadata] = Editor.SceneEditor.CreateComponentPanel(self, GTEngine.Components.EditorMetadata);
    
    
    
    -- New Component Button/Menu
    self.NewComponentContainer      = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID()       .. "' styleclass='new-component-container'             style='' />");
    self.NewComponentTitleContainer = GTGUI.Server.New("<div parentid='" .. self.NewComponentContainer:GetID()      .. "' styleclass='new-component-title-container'       style='' />");
    self.NewComponentIcon           = GTGUI.Server.New("<div parentid='" .. self.NewComponentTitleContainer:GetID() .. "' styleclass='new-component-title-container-icon'  style='' />");
    self.NewComponentLabel          = GTGUI.Server.New("<div parentid='" .. self.NewComponentTitleContainer:GetID() .. "' styleclass='new-component-title-container-label' style=''>New Component</div>");
    self.NewComponentBottomBorder   = GTGUI.Server.New("<div parentid='" .. self.NewComponentTitleContainer:GetID() .. "' styleclass='new-component-title-container-bottom-border' />");
    
    self.NewComponentMenu           = GTGUI.Server.New("<div parentid='" .. self.NewComponentContainer:GetID()      .. "' styleclass='new-component-menu'  style='visible:false;' />");
    self.NewComponentMenu:NewComponentMenu();
    
    self.NewComponentTitleContainer:OnLMBDown(function()
        if not self.NewComponentTitleContainer.IsOpen then
            self.NewComponentTitleContainer.IsOpen = true;
            self.NewComponentTitleContainer:AttachStyleClass("new-component-title-container-open");
            self.NewComponentIcon:AttachStyleClass("new-component-title-container-icon-open");
            self.NewComponentBottomBorder:Show();
            self.NewComponentMenu:Show();
        else
            self.NewComponentTitleContainer.IsOpen = false;
            self.NewComponentTitleContainer:DetachStyleClass("new-component-title-container-open");
            self.NewComponentIcon:DetachStyleClass("new-component-title-container-icon-open");
            self.NewComponentBottomBorder:Hide();
            self.NewComponentMenu:Hide();
        end
    end);
    
    
    self.NewComponentMenu:AppendNewItem("Model"):OnPressed(function()
        local component = self.CurrentSceneNode:AddComponent(GTEngine.Components.Model);
        if component ~= nil then
            component:SetModel("engine/models/default.dae");
        end
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Point Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.PointLight);

        self:UpdateComponentPanels();        
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Spot Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.SpotLight);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Directional Light"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.DirectionalLight);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Ambient Light"):OnPressed(function()
        local component = self.CurrentSceneNode:AddComponent(GTEngine.Components.AmbientLight);
        if component ~= nil then
            component:SetColour(0.25, 0.25, 0.25);
        end
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Dynamics (Collision and Physics)"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Dynamics);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Proximity"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Proximity);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    self.NewComponentMenu:AppendNewItem("Script"):OnPressed(function()
        self.CurrentSceneNode:AddComponent(GTEngine.Components.Script);
        
        self:UpdateComponentPanels();
        self:OnSceneNodeChanged();
    end);
    
    
    

    
    self.Scrollbar:VerticalScrollbar();
    self.Scrollbar:SetPageSize(self:GetHeight());
    self.Scrollbar:EnableAutoVisibility();
    self.Scrollbar:OnScroll(function(data)
        local offset = -data.position;
        self.Body:SetStyle("inner-offset-y", tostring(offset));
    end)
    
    
    
    -- Clears the panels and shows a message.
    function self:HidePanels(message)
        self.Body.PanelsContainer:Hide();
        
        self.Body.MessageContainer:Show();
        self.Body.MessageContainer:SetText(message);
    end
    
    -- Shows the panels, but does not update them.
    --
    -- This will also update the details of the panels.
    function self:ShowPanels()
        self.Body.MessageContainer:Hide();
        self.Body.PanelsContainer:Show();
    end
    
    
    -- Updates the details panel.
    function self:UpdateDetailsPanel()
        if self.CurrentSceneNode ~= nil then
            self.Body.DetailsPanel:Update(self.CurrentSceneNode);
        end
    end
    
    -- Updates the transform panel to show the transformation of the current node.
    function self:UpdateTransformPanel()
        if self.CurrentSceneNode ~= nil then
            self.Body.TransformPanel:Update(self.CurrentSceneNode);
        end
    end
    
    
    -- Updates the component panels.
    function self:UpdateComponentPanels()
        self:HideAllComponentPanels();
        self:ShowComponentPanels();
    end
    
    
    -- Updates the properties in the script panel.
    function self:UpdateScriptProperties()
        local scriptPanel = self.ComponentPanels[GTEngine.Components.Script];
        if scriptPanel ~= nil then
            scriptPanel:UpdateVariables();
        end
    end
    
    
    -- Hides every component panel.
    function self:HideAllComponentPanels()
        for key,value in pairs(self.ComponentPanels) do
            value:Hide();
        end
    end
    
    -- Shows the relevant component panels for the current node.
    function self:ShowComponentPanels()
        if self.CurrentSceneNode ~= nil then
            local componentIDs = self.CurrentSceneNode:GetAttachedComponentIDs();
            for i,value in ipairs(componentIDs) do
                self.ComponentPanels[value]:Show();
                self.ComponentPanels[value]:Update(self.CurrentSceneNode);
            end
        end
    end
    
    
    
    -- Updates the panel to show the details of the given scene node.\
    --
    -- This does not actually show the panels.
    function self:Update(node)
        self.CurrentSceneNode = node;
        
        if self.CurrentSceneNode ~= nil then
            self:UpdateDetailsPanel();
            self:UpdateTransformPanel();
            self:UpdateComponentPanels();
        end
    end
    
    
    
    self:OnSize(function()
        if self:IsVisible() then
            self.Scrollbar:SetPageSize(self:GetHeight());
        end
    end);
    
    self.Body:OnSize(function()
        if self.Body:IsVisible() then
            self.Scrollbar:SetRange(self.Body:GetHeight());
        end
    end)
    

    self:OnMouseEnter(function()
        self.IsMouseOver = true;
    end);
    
    self:OnMouseLeave(function()
        self.IsMouseOver = false;
    end);
    
    self:WatchMouseWheel(function(data)
        if self.IsMouseOver then
            self.Scrollbar:Scroll(-data.delta * self.MouseWheelScrollSpeed);
        end
    end);

    
    
    function self:OnSceneNodeChanged(arg1)
        self.Callbacks:BindOrCall("OnSceneNodeChanged", arg1);
    end
    
    
    
    return self;
end


function GTGUI.Element:SceneEditorHierarchyPanel(sceneEditor)
    self.TreeView    = GTGUI.Server.New("<div parentid='" .. self:GetID()      .. "' styleclass='scene-editor-hierarchy-treeview' style='' />");
    self.ContextMenu = GTGUI.Server.New("<div                                        styleclass='menu'                            style='z-index:100; positioning:absolute; visible:false' />");
    self.SceneNodes  = {}
    self.SceneEditor = sceneEditor;
    
    
    
    self.TreeView:TreeView();
    
    self.TreeView.body:OnDrop(function(data)
        if data.droppedElement ~= nil and data.droppedElement.sceneNodeID ~= nil then
            local sceneNodePtr = self.SceneEditor:GetSceneNodePtrByID(data.droppedElement.sceneNodeID);
            
            if GTEngine.System.SceneNode.GetParentPtr(sceneNodePtr) ~= nil then
                GTEngine.System.SceneNode.Orphan(sceneNodePtr, true);
                self.SceneEditor:SelectSceneNodeByID(data.droppedElement.sceneNodeID);
                
                -- Undo/Redo point.
                self.SceneEditor:CommitStateStackFrame();
            end
        end
    end);
    
    
    -- Context Menu
    self.ContextMenu:Menu();
    self.ContextMenu:EnableDefaultEvents();
    
    self.ContextMenu.Duplicate = self.ContextMenu:AppendItem("Duplicate"):OnPressed(function()
        self.SceneEditor:DuplicateSceneNodeByID(self.ContextMenu.SceneNodeID);
    end);
    
    self.ContextMenu.Duplicate = self.ContextMenu:AppendItem("Delete"):OnPressed(function()
        self.SceneEditor:DeleteSceneNodeByID(self.ContextMenu.SceneNodeID);
    end);
    
    
    self.ContextMenu:AppendSeparator();
    
    self.ContextMenu.Orphan = self.ContextMenu:AppendItem("Orphan"):OnPressed(function()
        self.SceneEditor:OrphanSceneNodeByID(self.ContextMenu.SceneNodeID);
        self.SceneEditor:SelectSceneNodeByID(self.ContextMenu.SceneNodeID);
        
        -- Undo/Redo point.
        self.SceneEditor:CommitStateStackFrame();
    end);
    
    
    function self:ShowContextMenu(item)
        self.ContextMenu.SceneNodeID = item.SceneNodeID;
        
        -- Some buttons might need to be disabled.
        if self.SceneEditor:GetParentSceneNodePtrByID(item.SceneNodeID) == nil then
            self.ContextMenu.Orphan:Disable();
        else
            self.ContextMenu.Orphan:Enable();
        end
        
        
        -- The position of the context menu will depend on whether or not it fits in the viewport.
        local xPos, yPos = GTGUI.Server.GetMousePosition();
        if (xPos + self.ContextMenu:GetWidth()) > GTGUI.Server.GetViewportWidth() then
            xPos = xPos - self.ContextMenu:GetWidth();
        end
        
        self.ContextMenu:SetPosition(xPos, yPos);
        self.ContextMenu:Show();
    end
    
    
    
    
    -- Sets the selected items to those of the given scene node ids. Deselects anything not in the list.
    function self:SetSelectedItemsBySceneNodeIDs(sceneNodeIDs)
        -- We're going to use a simple technique here where we just deselect everything and then reselect. Simple, intuitive and fast enough.
        
        -- 1) Deselect Everything.
        self.TreeView:DeselectAllItems(true);        -- 'true' = block OnDeselected events.
        
        -- 2) Reselect.
        for i,sceneNodeID in ipairs(sceneNodeIDs) do
            local treeViewItem = self.SceneNodes[sceneNodeID];
            if treeViewItem ~= nil then
                treeViewItem:Select(true);      -- 'true' = block OnSelected events.
            end
        end
    end
    
    
    function self:AddSceneNode(sceneNodePtr)
        local sceneNodeID       = GTEngine.System.SceneNode.GetID(sceneNodePtr);
        local parentSceneNodeID = GTEngine.System.SceneNode.GetID(GTEngine.System.SceneNode.GetParentPtr(self.SceneEditor:GetSceneNodePtrByID(sceneNodeID)));
        
        local item = self.SceneNodes[sceneNodeID];
        if item == nil then
            local sceneNodeName = GTEngine.System.SceneNode.GetName(sceneNodePtr);
            local parentItem    = self.SceneNodes[parentSceneNodeID];
            
            item = self.TreeView:AddItem(sceneNodeName or "Unnamed", parentItem);
            item.SceneNodeID   = sceneNodeID;
            item.SceneNodeName = sceneNodeName;
            
            if parentItem ~= nil then
                parentItem:Expand();
            end
            
            item.titleContainer:OnTear(function()
                -- We need to create an element that we'll use as the drag-and-drop element.
                local dragAndDropElement = GTGUI.Server.New("<div style='visible:false; positioning:absolute; z-index:100; opacity:50%; width:auto; padding:4px 2px; text-color:#ccc; font-style:bold; background-color:#666; vertical-align:center;' />");
                dragAndDropElement:SetText(item:GetText());
                dragAndDropElement:OnSize(function()
                    GTGUI.Server.SetDragAndDropProxyElement(dragAndDropElement, -(dragAndDropElement:GetWidth() / 2), -(dragAndDropElement:GetHeight() / 2));
                    dragAndDropElement:Show();
                end)
                dragAndDropElement:OnDragAndDropProxyRemoved(function()
                    GTGUI.Server.DeleteElement(dragAndDropElement);
                end)
                
                dragAndDropElement.sceneNodeID  = item.SceneNodeID;
                dragAndDropElement.sceneNodePtr = self.SceneEditor:GetSceneNodePtrByID(item.SceneNodeID);
            end);
            
            item.titleContainer:OnDrop(function(data)
                if data.droppedElement ~= nil and data.droppedElement.sceneNodeID ~= nil then
                    local parentSceneNodeID = item.SceneNodeID;
                    local childSceneNodeID  = data.droppedElement.sceneNodeID;
                    
                    if parentSceneNodeID ~= childSceneNodeID then
                        local parentSceneNodePtr = self.SceneEditor:GetSceneNodePtrByID(parentSceneNodeID);
                        local childSceneNodePtr  = self.SceneEditor:GetSceneNodePtrByID(childSceneNodeID);
                        
                        if parentSceneNodePtr ~= nil and childSceneNodePtr ~= nil and not GTEngine.System.SceneNode.IsAncestor(parentSceneNodePtr, childSceneNodePtr) then
                            GTEngine.System.SceneNode.AttachChild(parentSceneNodePtr, childSceneNodePtr, true);
                            self.SceneEditor:SelectSceneNodeByID(childSceneNodeID);
                            
                            -- Undo/Redo point.
                            self.SceneEditor:CommitStateStackFrame();
                        end
                    end
                end
            end);
            
            item.titleContainer:OnDragAndDropEnter(function(data)
            end);
            
            item.titleContainer:OnDragAndDropLeave(function(data)
            end);
            
            item.titleContainer:OnRMBUp(function()
                self.TreeView:DeselectAllItemsExcept(item);
                item:Select();
                
                self:ShowContextMenu(item);
            end);
            
            
            item:OnSelected(function()
                self.SceneEditor:SelectSceneNodeByID(item.SceneNodeID);
            end);
            
            item:OnDeselected(function()
                self.SceneEditor:DeselectSceneNodeByID(item.SceneNodeID);
            end);
            
            
            self.SceneNodes[sceneNodeID] = item;
        else
            self:UpdateSceneNode(sceneNodePtr);
        end
        
        
        -- We'll select the item (blocking the OnSelected event) if the scene node is marked as selected.
        if self.SceneEditor:IsSceneNodeSelectedByID(sceneNodeID) then
            item:Select(true);      -- 'true' will block the OnSelected event which we don't want.
        end
        
        
        -- We need to make sure children are also added.
        local children = GTEngine.System.SceneNode.GetChildrenIDs(sceneNodePtr);
        if children ~= nil then
            for i,sceneNodeID in ipairs(children) do
                if sceneNodeID ~= 0 then
                    self:AddSceneNode(self.SceneEditor:GetSceneNodePtrByID(sceneNodeID));
                end
            end
        end
        
        
        return item;
    end
    
    function self:RemoveSceneNode(sceneNodePtr)
        local sceneNodeID = GTEngine.System.SceneNode.GetID(sceneNodePtr);
    
        -- Children will be removed first.
        local children = GTEngine.System.SceneNode.GetChildrenIDs(sceneNodePtr);
        if children ~= nil then
            for i,sceneNodeID in ipairs(children) do
                self:RemoveSceneNode(self.SceneEditor:GetSceneNodePtrByID(sceneNodeID));
            end
        end
    
        self.TreeView:RemoveItem(self.SceneNodes[sceneNodeID], true);       -- 'true' means to block the OnDeselected message.
        self.SceneNodes[sceneNodeID] = nil;
    end
    
    function self:UpdateSceneNode(sceneNodePtr)
        local sceneNodeID = GTEngine.System.SceneNode.GetID(sceneNodePtr);
        
        local item = self.SceneNodes[sceneNodeID];
        if item ~= nil then
            local sceneNodeName = GTEngine.System.SceneNode.GetName(sceneNodePtr);
            
            item.SceneNodeName = sceneNodeName
            item:SetText(sceneNodeName or "Unnamed");
        end
    end
end


function GTGUI.Element:SceneEditorPanel(sceneEditor)
    self.TabBar          = GTGUI.Server.New("<div parentid='" .. self:GetID()      .. "' styleclass='scene-editor-panel-tabbar'        style='' />");
    self.Body            = GTGUI.Server.New("<div parentid='" .. self:GetID()      .. "' styleclass='scene-editor-panel-body'          style='' />");
    self.PropertiesPanel = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='scene-editor-properties-panel'    style='visible:false' />");
    self.HierarchyPanel  = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='scene-editor-hierarchy-panel'     style='visible:false' />");
    self.SceneEditor     = sceneEditor;
    
    
    self.TabBar:TabBar();
    self.TabBar.ActiveTabBorderColor     = "#222";
    self.TabBar.HoveredTabBorderColor    = "#222";
    self.TabBar.ActiveTabBackgroundColor = "#363636";
    
    self.HierarchyTab  = self.TabBar:AddTab("Hierarchy");
    self.PropertiesTab = self.TabBar:AddTab("Properties");
    
    
    self.TabBar:OnTabActivated(function(data)
        if data.tab == self.PropertiesTab then
            self.PropertiesPanel:Show();
        elseif data.tab == self.HierarchyTab then
            self.HierarchyPanel:Show();
            self.HierarchyPanel.TreeView:UpdateScrollbars();
        end
    end);
    
    self.TabBar:OnTabDeactivated(function(data)
        if data.tab == self.PropertiesTab then
            self.PropertiesPanel:Hide();
        elseif data.tab == self.HierarchyTab then
            self.HierarchyPanel:Hide();
        end
    end);
    
    
    self.TabBar:ActivateTab(self.PropertiesTab);
    
    
    
    self:OnDrop(function(data)
        if self.PropertiesPanel:IsVisible() then
            -- If we drop a script file onto the properties panel we want to add a script component if it doesn't already have one
            -- and add the given script to it.
            if data.droppedElement.isAsset and GTEngine.IsScriptFile(data.droppedElement.path) then
                if self.PropertiesPanel.CurrentSceneNode ~= nil then
                    local scriptComponent = self.PropertiesPanel.CurrentSceneNode:GetComponent(GTEngine.Components.Script);
                    if scriptComponent == nil then
                        scriptComponent = self.PropertiesPanel.CurrentSceneNode:AddComponent(GTEngine.Components.Script);
                    end
                    
                    self.PropertiesPanel:UpdateComponentPanels();
                    self.PropertiesPanel.ComponentPanels[GTEngine.Components.Script]:AddScript(data.droppedElement.path);
                end
            end
        end
    end);
    
    
    
    self.PropertiesPanel:SceneEditorPropertiesPanel(sceneEditor);
    self.HierarchyPanel:SceneEditorHierarchyPanel(sceneEditor);
    
    
    -- Properties Panel Events.
    self.PropertiesPanel:OnSceneNodeChanged(function()
        self.SceneEditor:CommitStateStackFrame()
    end);
end


function GTGUI.Element:SceneEditor(_internalPtr)
    self:SubEditor();

    self.Viewport          = GTGUI.Server.New("<div parentid='" .. self:GetID()                   .. "' styleclass='scene-editor-viewport'    style='' />");
    self.Panel             = GTGUI.Server.New("<div parentid='" .. self:GetID()                   .. "' styleclass='scene-editor-panel'       style='' />");
    self.ContextMenu       = GTGUI.Server.New("<div                                                     styleclass='menu'                     style='z-index:100; positioning:absolute; visible:false' />");
    
    self.PlaybackContainer = GTGUI.Server.New("<div parentid='" .. self.ToolBar:GetID()           .. "' styleclass='playback-container' />");
    self.PlayPauseButton   = GTGUI.Server.New("<div parentid='" .. self.PlaybackContainer:GetID() .. "' styleclass='play-button' />");
    self.StopButton        = GTGUI.Server.New("<div parentid='" .. self.PlaybackContainer:GetID() .. "' styleclass='stop-button' style='enabled:false;' />");
    
    self.PhysicsButton     = GTGUI.Server.New("<div parentid='" .. self.ToolBar:GetID()           .. "' styleclass='physics-button-container' style='' />");
    self.PhysicsButtonIcon = GTGUI.Server.New("<div parentid='" .. self.PhysicsButton:GetID()     .. "' styleclass='physics-button-icon'      style='' />");
    self.PhysicsButtonText = GTGUI.Server.New("<div parentid='" .. self.PhysicsButton:GetID()     .. "' styleclass='physics-button-text'      style=''>Physics</div>");
    
    
    
    
    
    self._internalPtr            = _internalPtr;
    self.Scene                   = GTEngine.Scene:Create(GTEngine.System.SceneEditor.GetScenePtr(_internalPtr));
    self.SelectedSceneNode       = nil;
    self.IsLMBDown               = false;
    self.IsRMBDown               = false;
    self.MouseMovedWhileCaptured = false;                -- Used to determine whether or not to show the right-click context menu.
    self.HasMouseCapture         = false;
    self.IsMouseOverViewport     = false;
    
    
    self.Panel:SceneEditorPanel(self);
    self.PropertiesPanel = self.Panel.PropertiesPanel;
    self.HierarchyPanel  = self.Panel.HierarchyPanel;

    
    self.ContextMenu:Menu();
    self.ContextMenu:EnableDefaultEvents();
    
    self.ContextMenu:AppendItem("Add Empty"):OnPressed(function()
        local newNode = self.Scene:CreateNewSceneNode();
        newNode:SetName("Empty");
        newNode:AddComponent(GTEngine.Components.EditorMetadata);
        
        self:PositionSceneNodeInFrontOfCamera(newNode);
        
        self:DeselectAll();
        self:SelectSceneNode(newNode);
        
        self:CommitStateStackFrame();
    end);
    
    self.ContextMenu:AppendSeparator();

    self.ContextMenu:AppendItem("Add Cube"):OnPressed(function()
        local newNode = self.Scene:CreateNewSceneNode();
        newNode:SetName("Cube");
        newNode:AddComponent(GTEngine.Components.EditorMetadata);
        newNode:AddComponent(GTEngine.Components.Model):SetModel("engine/models/default-1x1.dae");
        newNode:AddComponent(GTEngine.Components.Dynamics):AddBoxCollisionShape(0.5, 0.5, 0.5);
        
        self:PositionSceneNodeInFrontOfCamera(newNode);
        
        self:DeselectAll();
        self:SelectSceneNode(newNode);
        
        self:CommitStateStackFrame();
    end);
    
    self.ContextMenu:AppendSeparator();
    
    self.ContextMenu:AppendItem("Add Point Light"):OnPressed(function()
        local newNode = self.Scene:CreateNewSceneNode();
        newNode:SetName("PointLight");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/textures/light-sprite.png");
        newNode:AddComponent(GTEngine.Components.PointLight);
        
        self:PositionSceneNodeInFrontOfCamera(newNode);
        
        self:DeselectAll();
        self:SelectSceneNode(newNode);
        
        self:CommitStateStackFrame();
    end);
    
    self.ContextMenu:AppendItem("Add Spot Light"):OnPressed(function()
        local newNode = self.Scene:CreateNewSceneNode();
        newNode:SetName("SpotLight");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/textures/light-sprite.png");
        newNode:AddComponent(GTEngine.Components.SpotLight);
        
        self:PositionSceneNodeInFrontOfCamera(newNode);
        
        self:DeselectAll();
        self:SelectSceneNode(newNode);
        
        self:CommitStateStackFrame();
    end);
    
    self.ContextMenu:AppendItem("Add Directional Light"):OnPressed(function()
        local newNode = self.Scene:CreateNewSceneNode();
        newNode:SetName("DirectionalLight");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/textures/light-sprite.png");
        newNode:AddComponent(GTEngine.Components.DirectionalLight);
        
        self:PositionSceneNodeInFrontOfCamera(newNode);
        
        self:DeselectAll();
        self:SelectSceneNode(newNode);
        
        self:CommitStateStackFrame();
    end);
    
    self.ContextMenu:AppendItem("Add Ambient Light"):OnPressed(function()
        local newNode = self.Scene:CreateNewSceneNode();
        newNode:SetName("AmbientLight");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/textures/light-sprite.png");
        newNode:AddComponent(GTEngine.Components.AmbientLight);
        
        self:PositionSceneNodeInFrontOfCamera(newNode);
        
        self:DeselectAll();
        self:SelectSceneNode(newNode);
        
        self:CommitStateStackFrame();
    end);
    
    
    
    self.PlayPauseButton:OnPressed(function()
        if not GTEngine.System.SceneEditor.IsPlaying(self._internalPtr) or GTEngine.System.SceneEditor.IsPaused(self._internalPtr) then
            GTEngine.System.SceneEditor.StartPlaying(self._internalPtr);
        else
            GTEngine.System.SceneEditor.PausePlaying(self._internalPtr);
        end
    end);
    
    self.StopButton:OnPressed(function()
        GTEngine.System.SceneEditor.StopPlaying(self._internalPtr);
    end);
    
    -- Updates the playback control buttons.
    function self:UpdatePlaybackControls()
        if not GTEngine.System.SceneEditor.IsPlaying(self._internalPtr) or GTEngine.System.SceneEditor.IsPaused(self._internalPtr) then
            self.PlayPauseButton:DetachStyleClass("pause-button");
            self.PlayPauseButton:AttachStyleClass("play-button");
        else
            self.PlayPauseButton:DetachStyleClass("play-button");
            self.PlayPauseButton:AttachStyleClass("pause-button");
        end
        
        if GTEngine.System.SceneEditor.IsPlaying(self._internalPtr) then
            self.StopButton:Enable();
        else
            self.StopButton:Disable();
        end
    end
    
    -- Updates the "Play" button icon to show the correct icon based on whether or not the game is playing. This is never
    -- actually called from the scripting environment, and instead is called from the C++ side.
    function self:UpdatePlayButtonIcon()
        if GTEngine.System.SceneEditor.IsPlaying(self._internalPtr) then
            self.PlayButtonIcon:AttachStyleClass("physics-button-icon-stop");
        else
            self.PlayButtonIcon:DetachStyleClass("physics-button-icon-stop");
        end
    end
    
    
    
    self.PhysicsButton:OnPressed(function()
        if GTEngine.System.SceneEditor.IsPhysicsSimulationEnabled(self._internalPtr) then
            GTEngine.System.SceneEditor.DisablePhysicsSimulation(self._internalPtr);
        else
            GTEngine.System.SceneEditor.EnablePhysicsSimulation(self._internalPtr);
        end
    end);
    
    -- Updates the physics button icon to show the correct icon based on whether or not the simulation is enabled. This is never
    -- actually called from the scripting environment, and instead is called from the C++ side.
    function self:UpdatePhysicsButtonIcon()
        if GTEngine.System.SceneEditor.IsPhysicsSimulationEnabled(self._internalPtr) then
            self.PhysicsButtonIcon:AttachStyleClass("physics-button-icon-stop");
        else
            self.PhysicsButtonIcon:DetachStyleClass("physics-button-icon-stop");
        end
    end
    
    
    
    
    function self:GetScenePtr()
        return self.Scene._internalPtr;
    end
    
    --[[
    function self:AddSceneNode(name)
        local node = GTEngine.SceneNode:Create();
        node:SetName(name);
        
        self.Scene:AddSceneNode(node);

        return node;
    end
    ]]
    
    function self:DeselectAll()
        GTEngine.System.SceneEditor.DeselectAll(self._internalPtr);
    end
    
    function self:SelectSceneNode(nodeToSelect, dontPostBackNotification)
        GTEngine.System.SceneEditor.SelectSceneNode(self._internalPtr, nodeToSelect._internalPtr, dontPostBackNotification);
    end
    
    function self:SelectSceneNodeByID(nodeToSelectID, dontPostBackNotification)
        GTEngine.System.SceneEditor.SelectSceneNode(self._internalPtr, self:GetSceneNodePtrByID(nodeToSelectID), dontPostBackNotification);
    end
    
    
    function self:DeselectSceneNode(nodeToSelect, dontPostBackNotification)
        GTEngine.System.SceneEditor.DeselectSceneNode(self._internalPtr, nodeToSelect._internalPtr, dontPostBackNotification);
    end
    
    function self:DeselectSceneNodeByID(nodeToSelectID, dontPostBackNotification)
        GTEngine.System.SceneEditor.DeselectSceneNode(self._internalPtr, self:GetSceneNodePtrByID(nodeToSelectID), dontPostBackNotification);
    end
    
    
    function self:GetSelectedSceneNodeCount()
        return GTEngine.System.SceneEditor.GetSelectedSceneNodeCount(self._internalPtr);
    end
    
    function self:GetSelectedSceneNode()
        return self.SelectedSceneNode;
    end
    
    function self:SetSelectedSceneNode(internalPtr)
        if internalPtr ~= nil then
            self.SelectedSceneNode = GTEngine.SceneNode:Create(internalPtr);
        else
            self.SelectedSceneNode = nil;
        end
    end
    
    function self:GetSelectedSceneNodeIDs()
        return GTEngine.System.SceneEditor.GetSelectedSceneNodeIDs(self._internalPtr);
    end
    
    function self:IsSceneNodeSelectedByID(sceneNodeID)
        return GTEngine.System.SceneEditor.IsSceneNodeSelectedByID(self._internalPtr, sceneNodeID);
    end
    
    
    function self:OrphanSceneNodeByID(sceneNodeID)
        GTEngine.System.SceneNode.Orphan(self:GetSceneNodePtrByID(sceneNodeID), true);
    end
    
    
    
    
    function self:TryGizmoMouseSelect()
        return GTEngine.System.SceneEditor.TryGizmoMouseSelect(self._internalPtr);
    end
    
    function self:DoMouseSelection()
        GTEngine.System.SceneEditor.DoMouseSelection(self._internalPtr);
    end
    

    
    function self:SwitchGizmoToTranslateMode()
        GTEngine.System.SceneEditor.SwitchGizmoToTranslateMode(self._internalPtr);
    end
    
    function self:SwitchGizmoToRotateMode()
        GTEngine.System.SceneEditor.SwitchGizmoToRotateMode(self._internalPtr);
    end
    
    function self:SwitchGizmoToScaleMode()
        GTEngine.System.SceneEditor.SwitchGizmoToScaleMode(self._internalPtr);
    end
    
    function self:SwitchGizmoToLocalSpace()
        GTEngine.System.SceneEditor.SwitchGizmoToLocalSpace(self._internalPtr);
    end
    
    function self:SwitchGizmoToGlobalSpace()
        GTEngine.System.SceneEditor.SwitchGizmoToGlobalSpace(self._internalPtr);
    end
    
    function self:ToggleGizmoSpace()
        GTEngine.System.SceneEditor.ToggleGizmoSpace(self._internalPtr);
    end
    
    
    function self:DuplicateSelectedSceneNodes()
        GTEngine.System.SceneEditor.DuplicateSelectedSceneNodes(self._internalPtr);
    end
    
    function self:DuplicateSceneNodeByID(sceneNodeID)
        GTEngine.System.SceneEditor.DuplicateSceneNode(self._internalPtr, self:GetSceneNodePtrByID(sceneNodeID));
        self:CommitStateStackFrame();
    end
    
    
    function self:DeleteSelectedSceneNodes()
        GTEngine.System.SceneEditor.RemoveSelectedSceneNodes(self._internalPtr);
    end
    
    function self:DeleteSceneNodeByID(sceneNodeID)
        GTEngine.System.SceneEditor.RemoveSceneNode(self._internalPtr, self:GetSceneNodePtrByID(sceneNodeID));
        self:CommitStateStackFrame();
    end
    
    
    function self:CommitStateStackFrame()
        GTEngine.System.SceneEditor.CommitStateStackFrame(self._internalPtr);
    end
    
    function self:Undo()
        GTEngine.System.SceneEditor.Undo(self._internalPtr);
    end
    
    function self:Redo()
        GTEngine.System.SceneEditor.Redo(self._internalPtr);
    end
    
    function self:InstantiatePrefab(relativePath)
        local rootSceneNodePtr = GTEngine.System.SceneEditor.InstantiatePrefab(self._internalPtr, relativePath);
        
        GTEngine.System.SceneEditor.PositionSceneNodeInFrontOfCamera(self._internalPtr, rootSceneNodePtr);
        
        return rootSceneNodePtr;
    end
    
    
    
    function self:GetSceneNodePtrByID(sceneNodeID)
        return GTEngine.System.SceneEditor.GetSceneNodePtrByID(self._internalPtr, sceneNodeID);
    end
    
    function self:GetParentSceneNodePtrByID(sceneNodeID)
        return GTEngine.System.SceneNode.GetParentPtr(self:GetSceneNodePtrByID(sceneNodeID));
    end
    
    function self:GetParentSceneNodeIDByID(sceneNodeID)
        return GTEngine.System.SceneNode.GetID(self:GetParentSceneNodePtrByID(sceneNodeID));
    end
    
    
    function self:PositionSceneNodeInFrontOfCamera(sceneNode)
        GTEngine.System.SceneEditor.PositionSceneNodeInFrontOfCamera(self._internalPtr, sceneNode._internalPtr);
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
    
    function self:OnSceneNodeNameChanged(sceneNodePtr)
        self.HierarchyPanel:UpdateSceneNode(sceneNodePtr);
    end
    
    function self:OnSceneNodeParentChanged(sceneNodePtr)
        self.HierarchyPanel:RemoveSceneNode(sceneNodePtr);
        self.HierarchyPanel:AddSceneNode(sceneNodePtr);
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
        if self.IsMouseOverViewport and not GTGUI.Server.DoesFocusedElementHaveEditableText() then
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















