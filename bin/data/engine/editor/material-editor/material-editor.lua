-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function LinkMaterialEditorToSystemAPI(materialEditor)
    function materialEditor:GetViewportCameraSceneNodePtr()
        return GTEngine.System.MaterialEditor.GetViewportCameraSceneNodePtr(self._internalPtr);
    end
end

function GTGUI.Element:MaterialEditor(_internalPtr)
    self:SubEditor(_internalPtr);
    LinkMaterialEditorToSystemAPI(self);
    
    ----------------------------------------------
    -- Preview  --          Properties          --
    ----------------------------------------------
    --                Code                      --
    ----------------------------------------------
    self.TopContainer      = GTGUI.Server.New("<div parentid='" .. self:GetID()                  .. "' styleclass='material-editor-top-container'      />");
    self.BottomContainer   = GTGUI.Server.New("<div parentid='" .. self:GetID()                  .. "' styleclass='material-editor-bottom-container'   />");
    self.TopLeftContainer  = GTGUI.Server.New("<div parentid='" .. self.TopContainer:GetID()     .. "' styleclass='material-editor-topleft-container'  />");
    self.TopRightContainer = GTGUI.Server.New("<div parentid='" .. self.TopContainer:GetID()     .. "' styleclass='material-editor-topright-container' />");
    self.Viewport          = GTGUI.Server.New("<div parentid='" .. self.TopLeftContainer:GetID() .. "' styleclass='material-editor-viewport'           />");
    self.ScriptTextBox     = GTGUI.Server.New("<div parentid='" .. self.BottomContainer:GetID()  .. "' styleclass='material-editor-script-textbox'     />");
    
    self.Viewport:DefaultEditor3DViewport();
    self.Viewport:SetCameraSceneNodePtr(self:GetViewportCameraSceneNodePtr());
    
    self.ScriptTextBox:MultiLineTextBox();
    
    
    function self:ResetCamera()
        local cameraSceneNodePtr = self:GetViewportCameraSceneNodePtr();
        if cameraSceneNodePtr then
            GTEngine.System.SceneNode.SetPosition(cameraSceneNodePtr, math.vec3(3.5, 2.0, 3.5));
            GTEngine.System.SceneNode.LookAt(cameraSceneNodePtr, math.vec3(0, 0, 0));
            
            local eulerRotation = GTEngine.System.SceneNode.GetWorldEulerRotation(cameraSceneNodePtr);
            self:SetViewportCameraRotation(eulerRotation.x, eulerRotation.y);
        end
    end
    
    function self:SetViewportCameraRotation(rotationX, rotationY)
        self.Viewport:SetCameraRotation(rotationX, rotationY);
        self.Viewport:OnCameraTransformed();
    end
    
    function self:GetViewportCameraRotation()
        return self.Viewport:GetCameraRotation();
    end
    
    
    return self;
end