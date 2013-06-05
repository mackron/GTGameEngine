-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorTransformPanel(parentPanel)
    self:PanelGroupBox("Transform");
    self.ParentPanel = parentPanel;
    
    self.Container = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()      .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true;' />");
    self.Left      = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.Right     = GTGUI.Server.New("<div parentid='" .. self.Container:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.PositionLabel = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px;'>Position</div>");
    self.RotationLabel = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; margin-bottom:4px;'>Rotation</div>");
    self.ScaleLabel    = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 3px; padding-bottom:0px'>Scale</div>");
    
    self.PositionInput = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true; margin-bottom:4px;' />"):Vector3Input();
    self.RotationInput = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true; margin-bottom:4px;' />"):Vector3Input();
    self.ScaleInput    = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' style='width:100%; height:auto; horizontal-align:right; child-plane:horizontal; flex-child-width:true;' />"):Vector3Input();
    
    self.SceneNode = nil;

    
    function self:Update(node)
        self.SceneNode = node;
        
        self.PositionInput:SetValue(node:GetPosition());
        self.RotationInput:SetValue(node:GetEulerRotation());
        self.ScaleInput:SetValue(node:GetScale());
    end
    
    
    
    self.PositionInput:OnValueChanged(function(data)
        if self.SceneNode ~= nil then
            self.SceneNode:SetPosition(data.x, data.y, data.z);
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    self.RotationInput:OnValueChanged(function(data)
        if self.SceneNode ~= nil then
            self.SceneNode:SetEulerRotation(data.x, data.y, data.z);
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