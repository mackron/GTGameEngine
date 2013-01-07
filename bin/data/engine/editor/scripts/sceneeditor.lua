
-- Editor.SceneEditor.CreateCustomComponentPanel()
--
-- This should be overwritten by the client game so that it can create it's own GUI panels for it's custom components, if it has any.
function Editor.SceneEditor.CreateCustomComponentPanel(panelElement, componentID)
end


function Editor.SceneEditor.CreateComponentPanel(parentPanel, componentID)
    local element = GTGUI.Server.New("<div parentid='" .. parentPanel.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");

    
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
    --elseif componentID == GTEngine.Components.Proximity then
    --    element:ProximityComponentPanel();
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
    self:PanelGroupBox("Model");
    
    -- Model
    self.ModelPath   = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    
    -- Shadows
    self.CastShadows = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:8px;' />");
    
    
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
        if self.CurrentComponent ~= nil then self.CurrentComponent:EnableShadowCasting(); end;
    end);
    
    self.CastShadows:OnUnchecked(function()
        if self.CurrentComponent ~= nil then self.CurrentComponent:DisableShadowCasting(); end;
    end);
    
    
    
    
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
        end
    end
    
    function self:UpdateModel()
        if self.CurrentComponent ~= nil and self.CurrentNode ~= nil then
            self.CurrentComponent:SetModel(self.ModelPath:GetText());
            self.CurrentNode:Refresh();
        end
    end
    
    return self;
end

function GTGUI.Element:CameraComponentPanel()
    self:PanelGroupBox("Camera");
    
    function self:Update(node)
    end
    
    return self;
end

function GTGUI.Element:PointLightComponentPanel()
    self:PanelGroupBox("Point Light");
    
    -- Colour
    self.ColourContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.ColourLabel     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px; margin-right:8px;'>Colour:</div>");
    self.ColourInput     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();
    
    -- Attenuation
    self.AttenuationContainer      = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.AttenuationLeft           = GTGUI.Server.New("<div parentid='" .. self.AttenuationContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.AttenuationRight          = GTGUI.Server.New("<div parentid='" .. self.AttenuationContainer:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.ConstantAttenuationLabel  = GTGUI.Server.New("<div parentid='" .. self.AttenuationLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px; margin-bottom:4px;'>Constant Attenuation:</div>");
    self.LinearAttenuationLabel    = GTGUI.Server.New("<div parentid='" .. self.AttenuationLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px; margin-bottom:4px;'>Linear Attenuation:</div>");
    self.QuadraticAttenuationLabel = GTGUI.Server.New("<div parentid='" .. self.AttenuationLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px; padding-bottom:0px'>Quadratic Attenuation:</div>");
    
    self.ConstantAttenuationInput  = GTGUI.Server.New("<div parentid='" .. self.AttenuationRight:GetID()  .. "' styleclass='textbox' style='width:100%; margin-bottom:2px;'></div>");
    self.LinearAttenuationInput    = GTGUI.Server.New("<div parentid='" .. self.AttenuationRight:GetID()  .. "' styleclass='textbox' style='width:100%; margin-bottom:2px;'></div>");
    self.QuadraticAttenuationInput = GTGUI.Server.New("<div parentid='" .. self.AttenuationRight:GetID()  .. "' styleclass='textbox' style='width:100%;'></div>");
    
    
    -- Shadows
    self.CastShadows = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:8px;' />");
    
    
    
    self.CurrentNode           = nil;
    self.CurrentComponent      = nil;
    self.LockAttenuationEvents = false;
    
    
    
    self.ColourInput:OnValueChanged(function(data)
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetColour(data.x, data.y, data.z);
        end
    end);
    
    
    
    self.ConstantAttenuationInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateComponentAttenuation() end
    end);
    
    self.LinearAttenuationInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateComponentAttenuation() end
    end);
    
    self.QuadraticAttenuationInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateComponentAttenuation() end
    end);
    
    
    
    self.CastShadows:CheckBox("Cast Shadows");
    
    self.CastShadows:OnChecked(function()
        if self.CurrentComponent ~= nil then self.CurrentComponent:EnableShadowCasting(); end;
    end);
    
    self.CastShadows:OnUnchecked(function()
        if self.CurrentComponent ~= nil then self.CurrentComponent:DisableShadowCasting(); end;
    end);
    
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.PointLight);
        
        if self.CurrentComponent ~= nil then
            self.ColourInput:SetFromXYZ(self.CurrentComponent:GetColour());
            
            self.LockAttenuationEvents = true;
            self.ConstantAttenuationInput:SetText( string.format("%.4f", self.CurrentComponent:GetConstantAttenuation()));
            self.LinearAttenuationInput:SetText(   string.format("%.4f", self.CurrentComponent:GetLinearAttenuation()));
            self.QuadraticAttenuationInput:SetText(string.format("%.4f", self.CurrentComponent:GetQuadraticAttenuation()));
            self.LockAttenuationEvents = false;
            
            if self.CurrentComponent:IsShadowCastingEnabled() then
                self.CastShadows:Check(true);
            else
                self.CastShadows:Uncheck(true);
            end
        end
    end
    
    function self:UpdateComponentAttenuation()
        if self.CurrentComponent ~= nil then
            local constant  = tonumber(self.ConstantAttenuationInput:GetText());
            local linear    = tonumber(self.LinearAttenuationInput:GetText());
            local quadratic = tonumber(self.QuadraticAttenuationInput:GetText());
            
            self.CurrentComponent:SetAttenuation(constant, linear, quadratic);
        end
    end
    
    return self;
end

function GTGUI.Element:SpotLightComponentPanel()
    self:PanelGroupBox("Spot Light");
    
    -- Colour
    self.ColourContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.ColourLabel     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px; margin-right:8px;'>Colour:</div>");
    self.ColourInput     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();
    
    -- Attenuation
    self.AttenuationContainer      = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()                 .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.AttenuationLeft           = GTGUI.Server.New("<div parentid='" .. self.AttenuationContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.AttenuationRight          = GTGUI.Server.New("<div parentid='" .. self.AttenuationContainer:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.ConstantAttenuationLabel  = GTGUI.Server.New("<div parentid='" .. self.AttenuationLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px; margin-bottom:4px;'>Constant Attenuation:</div>");
    self.LinearAttenuationLabel    = GTGUI.Server.New("<div parentid='" .. self.AttenuationLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px; margin-bottom:4px;'>Linear Attenuation:</div>");
    self.QuadraticAttenuationLabel = GTGUI.Server.New("<div parentid='" .. self.AttenuationLeft:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px; padding-bottom:0px'>Quadratic Attenuation:</div>");
    
    self.ConstantAttenuationInput  = GTGUI.Server.New("<div parentid='" .. self.AttenuationRight:GetID()  .. "' styleclass='textbox' style='width:100%; margin-bottom:2px;'></div>");
    self.LinearAttenuationInput    = GTGUI.Server.New("<div parentid='" .. self.AttenuationRight:GetID()  .. "' styleclass='textbox' style='width:100%; margin-bottom:2px;'></div>");
    self.QuadraticAttenuationInput = GTGUI.Server.New("<div parentid='" .. self.AttenuationRight:GetID()  .. "' styleclass='textbox' style='width:100%;'></div>");
    
    
    -- Shadows
    self.CastShadows = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:8px;' />");
    
    
    
    self.CurrentNode           = nil;
    self.CurrentComponent      = nil;
    self.LockAttenuationEvents = false;
    
    
    
    self.ColourInput:OnValueChanged(function(data)
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetColour(data.x, data.y, data.z);
        end
    end);
    
    
    
    self.ConstantAttenuationInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateComponentAttenuation() end
    end);
    
    self.LinearAttenuationInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateComponentAttenuation() end
    end);
    
    self.QuadraticAttenuationInput:OnTextChanged(function()
        if self.CurrentComponent ~= nil and not self.LockAttenuationEvents then self:UpdateComponentAttenuation() end
    end);
    
    
    
    self.CastShadows:CheckBox("Cast Shadows");
    
    self.CastShadows:OnChecked(function()
        if self.CurrentComponent ~= nil then self.CurrentComponent:EnableShadowCasting(); end;
    end);
    
    self.CastShadows:OnUnchecked(function()
        if self.CurrentComponent ~= nil then self.CurrentComponent:DisableShadowCasting(); end;
    end);
    
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.SpotLight);
        
        if self.CurrentComponent ~= nil then
            self.ColourInput:SetFromXYZ(self.CurrentComponent:GetColour());
            
            self.LockAttenuationEvents = true;
            self.ConstantAttenuationInput:SetText( string.format("%.4f", self.CurrentComponent:GetConstantAttenuation()));
            self.LinearAttenuationInput:SetText(   string.format("%.4f", self.CurrentComponent:GetLinearAttenuation()));
            self.QuadraticAttenuationInput:SetText(string.format("%.4f", self.CurrentComponent:GetQuadraticAttenuation()));
            self.LockAttenuationEvents = false;
            
            if self.CurrentComponent:IsShadowCastingEnabled() then
                self.CastShadows:Check(true);
            else
                self.CastShadows:Uncheck(true);
            end
        end
    end
    
    function self:UpdateComponentAttenuation()
        if self.CurrentComponent ~= nil then
            local constant  = tonumber(self.ConstantAttenuationInput:GetText());
            local linear    = tonumber(self.LinearAttenuationInput:GetText());
            local quadratic = tonumber(self.QuadraticAttenuationInput:GetText());
            
            self.CurrentComponent:SetAttenuation(constant, linear, quadratic);
        end
    end
    
    return self;
end

function GTGUI.Element:DirectionalLightComponentPanel()
    self:PanelGroupBox("Directional Light");
    
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
        end
    end);
    
    
    
    self.CastShadows:CheckBox("Cast Shadows");
    
    self.CastShadows:OnChecked(function()
        if self.CurrentComponent ~= nil then self.CurrentComponent:EnableShadowCasting(); end;
    end);
    
    self.CastShadows:OnUnchecked(function()
        if self.CurrentComponent ~= nil then self.CurrentComponent:DisableShadowCasting(); end;
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
    self:PanelGroupBox("Ambient Light");
    
    -- Colour
    self.ColourContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()            .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.ColourLabel     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px; margin-right:8px;'>Colour:</div>");
    self.ColourInput     = GTGUI.Server.New("<div parentid='" .. self.ColourContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();

    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;

    
    self.ColourInput:OnValueChanged(function(data)
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetColour(data.x, data.y, data.z);
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

function GTGUI.Element:DynamicsComponentPanel()
    self:PanelGroupBox("Dynamics");
    
    -- Mass
    self.IsKinematic   = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-top:0px;' />");
    
    self.MassContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()          .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; horizontal-align:right; margin-top:4px;' />");
    self.MassLeft      = GTGUI.Server.New("<div parentid='" .. self.MassContainer:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.MassRight     = GTGUI.Server.New("<div parentid='" .. self.MassContainer:GetID() .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    
    self.MassLabel     = GTGUI.Server.New("<div parentid='" .. self.MassLeft:GetID()      .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;'>Mass (0 = Static):</div>");
    self.MassInput     = GTGUI.Server.New("<div parentid='" .. self.MassRight:GetID()     .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");

    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;

    
    
    self.IsKinematic:CheckBox("Is Kinematic");
    
    self.IsKinematic:OnChecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:IsKinematic(true);
            self.MassLabel:SetStyle("text-color", "#5a5a5a");
            self.MassInput:Disable();
        end
    end);
    
    self.IsKinematic:OnUnchecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:IsKinematic(false);
            self.MassLabel:SetStyle("text-color", "std-text-color");
            self.MassInput:Enable();
        end
    end);
    
    
    self.MassInput:OnTextChanged(function(data)
        self:UpdateMass();
    end);

    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Dynamics);
        
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
            
            self.MassInput:SetText(string.format("%.4f", self.CurrentComponent:GetMass()));
        end
    end
    
    function self:UpdateMass()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:SetMass(tonumber(self.MassInput:GetText()));
        end
    end
    
    return self;
end

function GTGUI.Element:ProximityComponentPanel()
    self:PanelGroupBox("Proximity");
    
    function self:Update(node)
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
        end
    end
    
    function self:UpdateSprite()
        if self.CurrentComponent ~= nil and self.CurrentNode ~= nil then
            if self.ShowSprite:IsChecked() then
                self.CurrentComponent:ShowSprite(self.SpriteTexture:GetText());
            else
                self.CurrentComponent:HideSprite();
            end
            
            self.CurrentNode:Refresh();
        end
    end
    
    function self:UpdateDirectionArrow()
        if self.CurrentComponent ~= nil then
            if self.ShowDirectionArrow:IsChecked() then
                self.CurrentComponent:ShowDirectionArrow();
            else
                self.CurrentComponent:HideDirectionArrow();
            end
            
            self.CurrentNode:Refresh();
        end
    end

    
    return self;
end



function GTGUI.Element:SceneEditorDetailsPanel()
    self:PanelGroupBox("Details");
    
    self.NameContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()          .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; vertical-align:center;' />");
    self.NameLabel     = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' style='width:auto; margin-right:4px; text-color:std-text-color;'>Name:</div>");
    self.NameTextBox   = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    
    self.NameTextBox:OnTextChanged(function()
        local selectedNode = Editor.SceneEditor.GetFirstSelectedNode();
        if selectedNode ~= nil then
            selectedNode:SetName(self.NameTextBox:GetText());
        end
    end);
    
    return self;
end

function GTGUI.Element:SceneEditorTransformPanel()
    self:PanelGroupBox("Transform");
    
    self.Container = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()      .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.Left      = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.Right     = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.PositionLabel = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px;'>Position:</div>");
    self.RotationLabel = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px;'>Rotation:</div>");
    self.ScaleLabel    = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; padding-bottom:0px'>Scale:</div>");
    
    self.PositionInput = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true; margin-bottom:4px;' />"):Vector3Input();
    self.RotationInput = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true; margin-bottom:4px;' />"):Vector3Input();
    self.ScaleInput    = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();

    
    self.PositionInput:OnValueChanged(function(data)
        local selectedNode = Editor.SceneEditor.GetFirstSelectedNode();
        if selectedNode ~= nil then
            selectedNode:SetPosition(data.x, data.y, data.z);
        end
    end);
    
    self.RotationInput:OnValueChanged(function(data)
        local selectedNode = Editor.SceneEditor.GetFirstSelectedNode();
        if selectedNode ~= nil then
            selectedNode:SetRotationXYZ(data.x, data.y, data.z);
        end
    end);
    
    self.ScaleInput:OnValueChanged(function(data)
        local selectedNode = Editor.SceneEditor.GetFirstSelectedNode();
        if selectedNode ~= nil then
            selectedNode:SetScale(data.x, data.y, data.z);
        end
    end);
    
    return self;
end


function GTGUI.Element:SceneEditorPanel()
    self.Body      = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='scene-editor-panel-body' style='' />");
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='vertical-scrollbar'      style='' />");
    
    self.Body.MessageContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%; margin-top:8px; horizontal-align:center; font-style:bold; text-color:#555; visible:false;' />")
    
    
    self.Body.PanelsContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%;' />")
    
    self.Body.DetailsPanel   = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    self.Body.DetailsPanel:SceneEditorDetailsPanel();

    self.Body.TransformPanel = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' />");
    self.Body.TransformPanel:SceneEditorTransformPanel();
    
    
    -- This will create the panels for every registered component. We always want the editor metadata to be last.
    self.ComponentPanels = {};
    for key,value in pairs(GTEngine.Components) do
        if value ~= GTEngine.Components.EditorMetadata then
            self.ComponentPanels[value] = Editor.SceneEditor.CreateComponentPanel(self, value);
        end
    end
    
    self.ComponentPanels[GTEngine.Components.EditorMetadata] = Editor.SceneEditor.CreateComponentPanel(self, GTEngine.Components.EditorMetadata);


    
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
    
    -- Shows the panels.
    function self:ShowPanels(node)
        if node ~= nil then
            self.Body.MessageContainer:Hide();
            self.Body.PanelsContainer:Show();
            
            self.Body.DetailsPanel.NameTextBox:SetText(node:GetName());
            self:UpdateTransformPanel(node);
            
            -- Here is where we will show the panels for the individual components. What we will do is first hide every panel, and then show
            -- the ones that the input node actually uses.
            self:HideAllComponentPanels();
            self:ShowComponentPanels(node);
        end
    end
    
    -- Updates the transform panel to show the transformation of the given node.
    function self:UpdateTransformPanel(node)
        if node ~= nil then
            self.Body.TransformPanel.PositionInput:SetFromXYZ(node:GetPosition());
            self.Body.TransformPanel.RotationInput:SetFromXYZ(node:GetRotationXYZ());
            self.Body.TransformPanel.ScaleInput:SetFromXYZ(node:GetScale());
        end
    end
    
    
    -- Hides every component panel.
    function self:HideAllComponentPanels()
        for key,value in pairs(self.ComponentPanels) do
            value:Hide();
        end
    end
    
    -- Shows the relevant component panels for the given node.
    function self:ShowComponentPanels(node)
        if node ~= nil then
            local componentIDs = node:GetAttachedComponentIDs();
            for i,value in ipairs(componentIDs) do
                self.ComponentPanels[value]:Show();
                self.ComponentPanels[value]:Update(node);
            end
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
    
    
    return self;
end


function GTGUI.Element:SceneEditor()
    self.Viewport    = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='scene-editor-viewport' style='' />");
    self.Panel       = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='scene-editor-panel'    style='' />");
    self.ContextMenu = GTGUI.Server.New("<div                                   styleclass='menu'                  style='z-index:100; positioning:absolute; visible:false' />");
    
    self.IsLMBDown               = false;
    self.IsRMBDown               = false;
    self.MouseMovedWhileCaptured = false;                -- Used to determine whether or not to show the right-click context menu.
    self.HasMouseCapture         = false;
    
    
    self.Panel:SceneEditorPanel();
    self.ContextMenu:Menu();
    
    self.ContextMenu:AppendItem("Add Cube"):OnPressed(function()
        local newNode = Editor.SceneEditor.AddSceneNode("Cube");
        if newNode ~= nil then
            newNode:AddComponent(GTEngine.Components.Model):SetModel("engine/models/default-1x1.dae");
            newNode:AddComponent(GTEngine.Components.Dynamics):AddBoxCollisionShape(0.5, 0.5, 0.5);
            
            -- We want the new node to be selected, and to be the ONLY selected object.
            Editor.SceneEditor.DeselectAll();
            Editor.SceneEditor.SelectSceneNode(newNode);

            
            newNode:Refresh();
        end
        
        self.ContextMenu:Hide();
        self.Viewport:Focus();
    end);
    
    self.ContextMenu:AppendSeparator();
    
    self.ContextMenu:AppendItem("Add Point Light"):OnPressed(function()
        local newNode = Editor.SceneEditor.AddSceneNode("PointLight");
        if newNode ~= nil then
            newNode:AddComponent(GTEngine.Components.PointLight);
            
            -- The light will have a sprite by default.
            newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/textures/light-sprite.png");

            -- We want the new node to be selected, and to be the ONLY selected object.
            Editor.SceneEditor.DeselectAll();
            Editor.SceneEditor.SelectSceneNode(newNode);

            
            newNode:Refresh();
        end
        
        self.ContextMenu:Hide();
        self.Viewport:Focus();
    end);
    
    self.ContextMenu:AppendItem("Add Spot Light"):OnPressed(function()
        local newNode = Editor.SceneEditor.AddSceneNode("SpotLight");
        if newNode ~= nil then
            newNode:AddComponent(GTEngine.Components.SpotLight);
            
            -- The light will have a sprite by default.
            newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/textures/light-sprite.png");
            newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowDirectionArrow();
            
            -- We want the new node to be selected, and to be the ONLY selected object.
            Editor.SceneEditor.DeselectAll();
            Editor.SceneEditor.SelectSceneNode(newNode);

            
            newNode:Refresh();
        end
        
        self.ContextMenu:Hide();
        self.Viewport:Focus();
    end);
    
    self.ContextMenu:AppendItem("Add Directional Light"):OnPressed(function()
        local newNode = Editor.SceneEditor.AddSceneNode("DirectionalLight");
        if newNode ~= nil then
            newNode:AddComponent(GTEngine.Components.DirectionalLight);
            
            -- The light will have a sprite by default.
            newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/textures/light-sprite.png");
            newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowDirectionArrow();

            -- We want the new node to be selected, and to be the ONLY selected object.
            Editor.SceneEditor.DeselectAll();
            Editor.SceneEditor.SelectSceneNode(newNode);

            
            newNode:Refresh();
        end
        
        self.ContextMenu:Hide();
        self.Viewport:Focus();
    end);
    
    self.ContextMenu:AppendItem("Add Ambient Light"):OnPressed(function()
        local newNode = Editor.SceneEditor.AddSceneNode("AmbientLight");
        if newNode ~= nil then
            newNode:AddComponent(GTEngine.Components.AmbientLight);
            newNode:GetComponent(GTEngine.Components.AmbientLight):SetColour(0.25, 0.25, 0.25);
            
            -- The light will have a sprite by default.
            newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/textures/light-sprite.png");

            -- We want the new node to be selected, and to be the ONLY selected object.
            Editor.SceneEditor.DeselectAll();
            Editor.SceneEditor.SelectSceneNode(newNode);

            
            newNode:Refresh();
        end
        
        self.ContextMenu:Hide();
        self.Viewport:Focus();
    end);
    
    
    
    function self:HidePanels(message)
        self.Panel:HidePanels(message);
    end
    
    function self:ShowPanels(node)
        self.Panel:ShowPanels(node);
    end
    
    function self:UpdateTransformPanel(node)
        self.Panel:UpdateTransformPanel(node);
    end
    
    
    
    self.Viewport:OnKeyPressed(function(data)
        if data.key == GTCore.Keys.Delete then
            Editor.SceneEditor.DeleteSelectedSceneNodes();
        elseif data.key == GTCore.Keys.T then
            Editor.SceneEditor.SwitchGizmoToTranslateMode();
        elseif data.key == GTCore.Keys.R then
            Editor.SceneEditor.SwitchGizmoToRotateMode();
        elseif data.key == GTCore.Keys.S then
            Editor.SceneEditor.SwitchGizmoToScaleMode();
        elseif data.key == GTCore.Keys.L then
            Editor.SceneEditor.SwitchGizmoToLocalSpace();
        elseif data.key == GTCore.Keys.G then
            Editor.SceneEditor.SwitchGizmoToGlobalSpace();
        elseif data.key == GTCore.Keys.Q then
            Editor.SceneEditor.ToggleGizmoSpace();
        end
    end);
    
    
    
    self.Viewport:OnLMBDown(function()
        self.IsLMBDown       = true;
        self.HasMouseCapture = true;
        self:Focus();
        
        -- We will check for a selection on the gizmo. If we click on a gizmo, we don't want to do a mouse selection when the 
        -- button is raised. What we'll do is trick it into thinking the mouse was moved while it was captured which will
        -- cause the editor to not try and select anything.
        if Editor.SceneEditor.TryGizmoMouseSelect() then
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
            Editor.SceneEditor.DoMouseSelection();
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
    
    
    
    -- We're going to hide the panels by default since nothing is selected right now.
    self:HidePanels("Nothing Selected");
    
    
    return self;
end















