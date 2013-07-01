-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditorPanel(_internalPtr)
    self.Body      = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='model-editor-panel-body' style='' />");
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='vertical-scrollbar'      style='' />");
    
    self.MaterialsPanel         = GTGUI.Server.CreateElement(self.Body, "panel-groupbox");
    self.BonesPanel             = GTGUI.Server.CreateElement(self.Body, "panel-groupbox");
    self.AnimationSegmentsPanel = GTGUI.Server.CreateElement(self.Body, "panel-groupbox");
    self.CDPanel                = GTGUI.Server.CreateElement(self.Body, "panel-groupbox");
    
    self.MaterialsPanel:ModelEditor_MaterialsPanel(_internalPtr);
    self.BonesPanel:ModelEditor_BonesPanel(_internalPtr);
    self.AnimationSegmentsPanel:ModelEditor_AnimationSegmentsPanel(_internalPtr);
    self.CDPanel:ModelEditor_CDPanel(_internalPtr);
    self.CDPanel:Collapse();
    
    
    self.Scrollbar:VerticalScrollbar();
    self.Scrollbar:SetPageSize(self:GetHeight());
    self.Scrollbar:EnableAutoVisibility();
    self.Scrollbar:OnScroll(function(data)
        local offset = -data.position;
        self.Body:SetStyle("inner-offset-y", tostring(offset));
    end);
    
    
    function self:UpdateAnimationPlaybackControls()
        self.AnimationSegmentsPanel:UpdatePlaybackControls();
    end
    
    
    
    function self:Refresh()
        self.MaterialsPanel:Refresh();
        self.BonesPanel:Refresh();
        self.AnimationSegmentsPanel:Refresh();
        self.CDPanel:Refresh();
        
        -- Hide the Bones panel if there aren't any.
        if GTEngine.System.ModelEditor.GetBoneCount(_internalPtr) == 0 then
            self.BonesPanel:Hide();
            self.AnimationSegmentsPanel:Hide();
        else
            self.BonesPanel:Show();
            self.AnimationSegmentsPanel:Show();
        end
    end;
    
    
    
    
    self:OnSize(function()
        if self:IsVisible() then
            self.Scrollbar:SetPageSize(self:GetHeight());
        end
    end);
    
    self.Body:OnSize(function()
        if self.Body:IsVisible() then
            self.Scrollbar:SetRange(self.Body:GetHeight());
        end
    end);
    
    
    return self;
end


function LinkModelEditorToSystemAPI(modelEditor)
    function modelEditor:GetViewportCameraSceneNodePtr()
        return GTEngine.System.ModelEditor.GetViewportCameraSceneNodePtr(self._internalPtr);
    end
    
    function modelEditor:GetModelAABB()
        return GTEngine.System.ModelEditor.GetModelAABB(self._internalPtr);
    end
end


function GTGUI.Element:ModelEditor(_internalPtr)
    self:SubEditor(_internalPtr);
    LinkModelEditorToSystemAPI(self);
    
    self.ViewportTimelineContainer = GTGUI.Server.CreateElement(self, "model-editor-viewport-timeline-container");
    self.Viewport = GTGUI.Server.CreateElement(self.ViewportTimelineContainer, "model-editor-viewport");
    self.Timeline = GTGUI.Server.CreateElement(self.ViewportTimelineContainer, "model-editor-timeline");
    self.Panel    = GTGUI.Server.CreateElement(self,                           "model-editor-panel");
    
    
    

    
    self.Viewport:DefaultEditor3DViewport();
    self.Viewport:SetCameraSceneNodePtr(self:GetViewportCameraSceneNodePtr());
    
    self.Panel:ModelEditorPanel(_internalPtr);
    self.Timeline:ModelEditor_Timeline(_internalPtr);
    
    
    function self:UpdateAnimationPlaybackControls()
        self.Panel:UpdateAnimationPlaybackControls();
        self.Timeline:UpdatePlaybackControls();
    end
    
    
    function self:Refresh()
        self.Panel:Refresh();
        
        if GTEngine.System.ModelEditor.GetBoneCount(_internalPtr) == 0 then
            self.Timeline:Hide();
        else
            self.Timeline:Show();
        end
    end
    
    
    function self:ResetCamera()
        local cameraSceneNodePtr = self:GetViewportCameraSceneNodePtr();
        if cameraSceneNodePtr then
            local aabbMin, aabbMax = self:GetModelAABB();
        
            -- Center point (camera look-at target).
            local cameraLookAtTarget = (aabbMin + aabbMax) * 0.5;


            -- Camera position.
            local aabbSize     = aabbMax - aabbMin;
            local maxDimension = math.max(math.max(aabbSize.x, math.max(aabbSize.y, aabbSize.z), 0.01));

            local cameraPosition = cameraLookAtTarget;
            cameraPosition = cameraPosition + math.vec3_normalize(math.vec3(1, 0.5, 1)) * maxDimension * 2;



            -- Position and rotate the camera.
            GTEngine.System.SceneNode.SetPosition(cameraSceneNodePtr, cameraPosition);
            GTEngine.System.SceneNode.LookAt(cameraSceneNodePtr, cameraLookAtTarget);
        
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
    
    
    -- We'll do a refresh to get ourself into the correct state.
    self:Refresh();
    
    return self;
end