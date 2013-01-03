
function Editor.SceneEditor.CreateComponentPanel(element, componentID)
    if componentID == GTEngine.Components.ModelComponent then
        element:ModelComponentPanel();
    elseif componentID == GTEngine.Components.CameraComponent then
        element:CameraComponentPanel();
    elseif componentID == GTEngine.Components.PointLightComponent then
        element:PointLightComponentPanel();
    elseif componentID == GTEngine.Components.SpotLightComponent then
        element:SpotLightComponentPanel();
    elseif componentID == GTEngine.Components.DirectionalLightComponent then
        element:DirectionalLightComponentPanel();
    elseif componentID == GTEngine.Components.AmbientLightComponent then
        element:AmbientLightComponentPanel();
    elseif componentID == GTEngine.Components.EditorMetadataComponent then
        element:EditorMetadataComponentPanel();
    end
end


function GTGUI.Element:ModelComponentPanel()
    self:PanelGroupBox("Model");
    
    
end

function GTGUI.Element:CameraComponentPanel()
    self:PanelGroupBox("Camera");
end

function GTGUI.Element:PointLightComponentPanel()
    self:PanelGroupBox("Point Light");
end

function GTGUI.Element:SpotLightComponentPanel()
    self:PanelGroupBox("Spot Light");
end

function GTGUI.Element:DirectionalLightComponentPanel()
    self:PanelGroupBox("Directional Light");
end

function GTGUI.Element:AmbientLightComponentPanel()
    self:PanelGroupBox("Ambient Light");
end

function GTGUI.Element:EditorMetadataComponentPanel()
    self:PanelGroupBox("Editor Metadata");
end



function GTGUI.Element:SceneEditorDetailsPanel()
    self:PanelGroupBox("Details");
    
    self.NameContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()          .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; vertical-align:center;' />");
    self.NameLabel     = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' style='width:auto; margin-right:4px; text-color:std-text-color;'>Name:</div>");
    self.NameTextBox   = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' styleclass='textbox' style='width:100%;' />");
end

function GTGUI.Element:SceneEditorTransformPanel()
    self:PanelGroupBox("Transform");
    
    self.Container = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()      .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.Left      = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.Right     = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' style='width:100%; height:auto;' />");
    --GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='panel-groupbox-separator' style='' />");
    
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
end


function GTGUI.Element:SceneEditorPanel()
    self.Body      = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='scene-editor-panel-body' style='' />");
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='vertical-scrollbar'      style='' />");
    
    self.Body.MessageContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%; margin-top:8px; horizontal-align:center; font-style:bold; text-color:#555; visible:false;' />")
    
    
    self.Body.PanelsContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='height:auto; width:100%;' />")
    
    self.Body.DetailsPanel   = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' style='' />");
    self.Body.DetailsPanel:SceneEditorDetailsPanel();
    --GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox-separator' style='' />");
    self.Body.TransformPanel = GTGUI.Server.New("<div parentid='" .. self.Body.PanelsContainer:GetID() .. "' styleclass='panel-groupbox' style='' />");
    self.Body.TransformPanel:SceneEditorTransformPanel();
    
    

    self:OnSize(function()
        self.Scrollbar:SetPageSize(self:GetHeight());
    end);
    
    self.Body:OnSize(function()
        self.Scrollbar:SetRange(self.Body:GetHeight());
    end)
    
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
            newNode:AddComponent(GTEngine.Components.ModelComponent):SetModel("engine/models/default-1x1.dae");
            
            -- We want the new node to be selected, and to be the ONLY selected object.
            Editor.SceneEditor.DeselectAll();
            Editor.SceneEditor.SelectSceneNode(newNode);

            
            newNode:Refresh();
        end
        
        self.ContextMenu:Hide();
        self.Viewport:Focus();
    end);
    
    self.ContextMenu:AppendItem("Add Point Light"):OnPressed(function()
        local newNode = Editor.SceneEditor.AddSceneNode("PointLight");
        if newNode ~= nil then
            newNode:AddComponent(GTEngine.Components.PointLightComponent);
            
            -- The point light will have a sprite by default.
            newNode:GetComponent(GTEngine.Components.EditorMetadataComponent):ShowSprite("engine/textures/light-sprite.png");

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
end















