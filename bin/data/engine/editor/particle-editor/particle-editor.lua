-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function LinkParticleEditorToSystemAPI(particleEditor)
    function particleEditor:RefreshViewport()
        return GTEngine.System.ParticleEditor.RefreshViewport(self._internalPtr);
    end
    

    function particleEditor:SetOrientation(orientation)
        GTEngine.System.ParticleEditor.SetOrientation(self._internalPtr, orientation);
    end
    

    function particleEditor:ShowGrid()
        GTEngine.System.ParticleEditor.ShowGrid(self._internalPtr);
    end
    
    function particleEditor:HideGrid()
        GTEngine.System.ParticleEditor.HideGrid(self._internalPtr);
    end
    
    function particleEditor:IsShowingGrid()
        return GTEngine.System.ParticleEditor.IsShowingGrid(self._internalPtr);
    end
    
    
    function particleEditor:ShowAxisArrows()
        GTEngine.System.ParticleEditor.ShowAxisArrows(self._internalPtr);
    end
    
    function particleEditor:HideAxisArrows()
        GTEngine.System.ParticleEditor.HideAxisArrows(self._internalPtr);
    end
    
    function particleEditor:IsShowingAxisArrows()
        return GTEngine.System.ParticleEditor.IsShowingAxisArrows(self._internalPtr);
    end
    
    
    function particleEditor:GetViewportCameraSceneNodePtr()
        return GTEngine.System.ParticleEditor.GetViewportCameraSceneNodePtr(self._internalPtr);
    end
end

function GTGUI.Element:ParticleEditor(_internalPtr)
    self.IsLoading = true;          -- This is used to prevent OnChange events being posted back and thus causing the editor to think a modification was made.

    self:SubEditor(_internalPtr);
    LinkParticleEditorToSystemAPI(self);
    self.ParticleSystemDefinition = GTEngine.ParticleSystemDefinition:Create(GTEngine.System.ParticleEditor.GetParticleSystemDefinitionPtr(self._internalPtr));
    

    self.Viewport = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='particle-editor-viewport' />");
    self.Viewport:ParticleEditorViewport();
    self.Viewport:SetCameraSceneNodePtr(self:GetViewportCameraSceneNodePtr());
    
    self.Panel    = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='particle-editor-panel' />");
    self.Panel:ParticleEditorPanel(self);
    
    self.ToolBar:ParticleEditorToolbar(self);
    


    function self:AppendNewEmitter()
        self.ParticleSystemDefinition:AppendNewEmitter();
        self:OnChange();
        self:RefreshPanel();
    end
    
    function self:DeleteEmitterByIndex(index)
        self.ParticleSystemDefinition:DeleteEmitterByIndex(index);
        self:OnChange();
        self:RefreshPanel();
    end
    
    
    function self:RefreshPanel()
        self.Panel:Refresh(self.ParticleSystemDefinition);
    end
    
    function self:OnChange()
        if not self.IsLoading then
            self:RefreshViewport();
            self:MarkAsModified();
        end
    end
    
    
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
    

    
    -- At this point we want to make sure everything is up-to-date.
    self:RefreshPanel();
    
    self.IsLoading = false;
    return self;
end