-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:DefaultEditor3DViewport()
    self.isControlsEnabled         = true;
    self.isCameraControlsEnabled   = true;
    self.isLMBDown                 = false;
    self.isRMBDown                 = false;
    self.isMMBDown                 = false;
    self.isMouseOver               = false;
    self.hasMouseCapture           = false;
    self.mouseMovedWhileButtonDown = false;
    self.cameraSceneNodePtr        = nil;
    self.cameraXRotation           = 0;
    self.cameraYRotation           = 0;
    self.cameraRotationSpeed       = 0.1;
    self.cameraMoveSpeed           = 0.05;
    
    
    
    -- Virtual Methods
    --
    -- These methods should be overridden in order to provide functionality in response to these events.
    
    -- Called when the left mouse button is raised without any movement while a mouse button was down.
    function self:OnViewportLMBUpWithoutMovement()
    end
    
    -- Called when the right mouse button is raised without any movement while a mouse button was down.
    function self:OnViewportRMBUpWithoutMovement()
    end
    
    -- Called when the middle mouse button is raised without any movement while a mouse button was down.
    function self:OnViewportMMBUpWithoutMovement()
    end
    
    
    -- Called when the left mouse button is pressed while over the viewport.
    --
    -- The return value is important. If it returns true, the next OnViewport*WithoutMovement() call will be disabled.
    function self:OnViewportLMBDown()
        return false;
    end
    
    -- Same as OnViewportLMBDown() except for the right mouse button.
    function self:OnViewportRMBDown()
        return false;
    end
    
    -- Same as OnViewportLMBDown() except for the middle mouse button.
    function self:OnViewportMMBDown()
        return false;
    end
    
    
    -- Called when the camera has been transformed.
    function self:OnCameraTransformed()
    end
    
    
    
    -- Camera
    function self:SetCameraSceneNodePtr(cameraSceneNodePtr)
        self.cameraSceneNodePtr = cameraSceneNodePtr;
        
        -- The X and Y rotations need to be set.
        local eulerRotation = GTEngine.System.SceneNode.GetWorldEulerRotation(cameraSceneNodePtr);
        self.cameraXRotation = eulerRotation.x;
        self.cameraYRotation = eulerRotation.y;
    end
    
    function self:GetCameraSceneNodePtr()
        return self.cameraSceneNodePtr;
    end
    
    function self:MoveCameraForward(distance)
        if self:IsCameraControlsEnabled() then
            GTEngine.System.SceneNode.Translate(self.cameraSceneNodePtr, math.vec3(0, 0, -distance));
        end
    end
    
    function self:MoveCameraUp(distance)
        if self:IsCameraControlsEnabled() then
            GTEngine.System.SceneNode.Translate(self.cameraSceneNodePtr, math.vec3(0, distance, 0));
        end
    end
    
    function self:MoveCameraRight(distance)
        if self:IsCameraControlsEnabled() then
            GTEngine.System.SceneNode.Translate(self.cameraSceneNodePtr, math.vec3(distance, 0, 0));
        end
    end
    
    function self:RotateCamera(rotationX, rotationY)
        if self:IsCameraControlsEnabled() then
            self.cameraXRotation = self.cameraXRotation + rotationX;
            self.cameraYRotation = self.cameraYRotation + rotationY;
            self:ApplyCameraRotation();
        end
    end
    
    function self:SetCameraRotation(rotationX, rotationY)
        if self:IsCameraControlsEnabled() then
            self.cameraXRotation = rotationX;
            self.cameraYRotation = rotationY;
            self:ApplyCameraRotation();
        end
    end
    
    function self:GetCameraRotation()
        return self.cameraXRotation, self.cameraYRotation;
    end
    
    function self:ApplyCameraRotation()
        if self.cameraSceneNodePtr then
            GTEngine.System.SceneNode.SetOrientation(self.cameraSceneNodePtr, math.quat_angleaxis(self.cameraYRotation, math.vec3(0, 1, 0)) * math.quat_angleaxis(self.cameraXRotation, math.vec3(1, 0, 0)));
        end
    end
    
    
    
    
    -- Enable/Disable
    function self:EnableControls()
        self.isControlsEnabled = true;
    end
    
    function self:DisableControls()
        self.isControlsEnabled = false;
    end
    
    function self:IsControlsEnabled()
        return self.isControlsEnabled;
    end
    
    
    function self:EnableCameraControls()
        self.isCameraControlsEnabled = true;
    end
    
    function self:DisableCameraControls()
        self.isCameraControlsEnabled = false;
    end
    
    function self:IsCameraControlsEnabled()
        return self.isCameraControlsEnabled and self.cameraSceneNodePtr and self:IsControlsEnabled();
    end
    
    
    function self:IsLMBDown()
        return self.isLMBDown;
    end
    
    function self:IsRMBDown()
        return self.isRMBDown;
    end
    
    function self:IsMMBDown()
        return self.isMMBDown;
    end
    
    function self:IsMouseOver()
        return self.isMouseOver;
    end
    
    function self:HasMouseCapture()
        return self.hasMouseCapture;
    end
    
    function self:MouseMovedWhileButtonDown()
        return self.mouseMovedWhileButtonDown;
    end
    
    
    function self:CaptureMouse()
        Game.CaptureMouse();
        self.hasMouseCapture = true;
    end
    
    function self:ReleaseMouse()
        Game.ReleaseMouse();
        self.hasMouseCapture           = false;
        self.mouseMovedWhileButtonDown = false;
    end
    
    
    -- Mouse Events

    self:OnMouseEnter(function()
        self.isMouseOver = true;
    end)
    
    self:OnMouseLeave(function()
        self.isMouseOver = false;
    end)
    

    self:OnMouseButtonDown(function(data)
        if data.button == GTCore.MouseButtons.Left then
            self.isLMBDown = true;
        
            if self:IsControlsEnabled() then
                self:CaptureMouse();
                
                if self:OnViewportLMBDown() then
                    self.mouseMovedWhileButtonDown = true;      -- Trick the viewport into thinking the mouse was moved so that the next OnViewport*WithoutMovement() is disabled.
                end
            end
        elseif data.button == GTCore.MouseButtons.Right then
            self.isRMBDown = true;
        
            if self:IsControlsEnabled() then
                self:CaptureMouse();
                
                if self:OnViewportRMBDown() then
                    self.mouseMoveWhileButtonDown = true;      -- Trick the viewport into thinking the mouse was moved so that the next OnViewport*WithoutMovement() is disabled.
                end
            end
        elseif data.button == GTCore.MouseButtons.Middle then
            self.isMMBDown = true;
        
            if self:IsControlsEnabled() then
                self:CaptureMouse();
                
                if self:OnViewportMMBDown() then
                    self.mouseMoveWhileButtonDown = true;      -- Trick the viewport into thinking the mouse was moved so that the next OnViewport*WithoutMovement() is disabled.
                end
            end
        end
    end)

    self:WatchMouseButtonDown(function(data)
        if self:HasMouseCapture() then
            if data.button == GTCore.MouseButtons.Left then
                self.isLMBDown = true;
            elseif data.button == GTCore.MouseButtons.Right then
                self.isRMBDown = true;
            elseif data.button == GTCore.MouseButtons.Middle then
                self.isMMBDown = true;
            end
        end
    end)
    
    self:WatchMouseButtonUp(function(data)
        if self:HasMouseCapture() and self:IsControlsEnabled() then
            if ((data.button == GTCore.MouseButtons.Left   and not self:IsRMBDown() and not self:IsMMBDown()) or
                (data.button == GTCore.MouseButtons.Right  and not self:IsLMBDown() and not self:IsMMBDown()) or 
                (data.button == GTCore.MouseButtons.Middle and not self:IsLMBDown() and not self:IsRMBDown())) then
                local mouseMoved = self:MouseMovedWhileButtonDown();

                -- Release the mouse before posting the event. This will move the mouse, so we need to check for mouse movement before releasing.
                self:ReleaseMouse();

                if not mouseMoved and self:IsVisible() then
                    if data.button == GTCore.MouseButtons.Left then
                        self:OnViewportLMBUpWithoutMovement();
                    elseif data.button == GTCore.MouseButtons.Right then
                        self:OnViewportRMBUpWithoutMovement();
                    elseif data.button == GTCore.MouseButtons.Middle then
                        self:OnViewportMMBUpWithoutMovement();
                    end
                end
            end
        end

        if data.button == GTCore.MouseButtons.Left then
            self.isLMBDown = false;
        elseif data.button == GTCore.MouseButtons.Right then
            self.isRMBDown = false;
        elseif data.button == GTCore.MouseButtons.Middle then
            self.isMMBDown = false;
        end
    end)
    
    
    self:WatchMouseMove(function(data)
        if (self:IsLMBDown() or self:IsRMBDown() or self:IsMMBDown()) and Game.IsMouseCaptured() then
            local cameraTransformed = false;
            local mouseOffsetX, mouseOffsetY = Game.GetMouseOffset();
            
            if mouseOffsetX ~= 0 or mouseOffsetY ~= 0 then
                self.mouseMovedWhileButtonDown = true;
            
                if self:IsLMBDown() then
                    if self:IsRMBDown() then
                        if self:IsMMBDown() then
                            -- Left, middle and right buttons are down.
                        else
                            -- Left and right buttons are down.
                            self:MoveCameraUp(  -mouseOffsetY * self.cameraMoveSpeed);
                            self:MoveCameraRight(mouseOffsetX * self.cameraMoveSpeed);
                            cameraTransformed = true;
                        end
                    else
                        if self:IsMMBDown() then
                            -- Left and middle buttons are down.
                        else
                            -- Only the left button is down.
                            self:MoveCameraForward(-mouseOffsetY * self.cameraMoveSpeed);
                            self:RotateCamera(0, -mouseOffsetX * self.cameraRotationSpeed);
                            cameraTransformed = true;
                        end
                    end
                else
                    if self:IsRMBDown() then
                        if self:IsMMBDown() then
                            -- Middle and right buttons are down.
                        else
                            -- Only the right button is down.
                            self:RotateCamera(-mouseOffsetY * self.cameraRotationSpeed, -mouseOffsetX * self.cameraRotationSpeed);
                            cameraTransformed = true;
                        end
                    else
                        -- Only the middle mouse button is down.
                    end
                end
                
                if cameraTransformed then
                    self:OnCameraTransformed();
                end
            end
        end
    end)
    
    
    self:OnMouseWheel(function(data)
        self:MoveCameraForward(data.delta * 1.0);
        self:ApplyCameraRotation();
        self:OnCameraTransformed();
    end)
end
