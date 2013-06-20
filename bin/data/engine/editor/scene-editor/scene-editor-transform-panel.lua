-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorTransformPanel(parentPanel, sceneEditor)
    self:PanelGroupBox("Transform");
    self.ParentPanel = parentPanel;
    self.SceneNode   = nil;
    
    self.PositionInput = GTGUI.Server.CreateElement(self.Body, "labelled-vector3-input");
    self.PositionInput:LabelledVector3Input("Position");
    self.PositionInput:SetTooltip("Relative position. Forward is -Z. X/Y/Z.");
    
    self.RotationInput = GTGUI.Server.CreateElement(self.Body, "labelled-vector3-input");
    self.RotationInput:LabelledVector3Input("Rotation");
    self.RotationInput:SetStyle("margin-top", "2px");
    self.RotationInput:SetTooltip("Relative rotation in degrees. X/Y/Z.");
    
    self.ScaleInput = GTGUI.Server.CreateElement(self.Body, "labelled-vector3-input");
    self.ScaleInput:LabelledVector3Input("Scale");
    self.ScaleInput:SetStyle("margin-top", "2px");
    self.ScaleInput:SetTooltip("Relative scale. X/Y/Z.");
    
    
    self.SetToCameraButton = GTGUI.Server.CreateElement(self.Body, "scene-editor-transform-panel-settocamera-button");
    self.SetToCameraButton:SetText("Set to Camera");
    self.SetToCameraButton:OnPressed(function()
        sceneEditor:SetSceneNodeTransformToCamera(self.SceneNode);
        sceneEditor:OnSceneNodeChanged();
    end);
    self.SetToCameraButton:SetTooltip("Sets the position and orientation of the selected node\nto that of the viewport camera.");
    
    

    
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