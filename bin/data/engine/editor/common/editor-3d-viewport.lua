-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:DefaultEditor3DViewport()
    self.isControlsEnabled         = true;
    self.isLMBDown                 = false;
    self.isRMBDown                 = false;
    self.isMMBDown                 = false;
    self.isMouseOver               = false;
    self.hasMouseCapture           = false;
    self.mouseMovedWhileButtonDown = false;
    
    
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
    
    
    self:OnLMBDown(function()
        self.isLMBDown = true;
        
        if self:IsControlsEnabled() then
            self:CaptureMouse();
            
            if self:OnViewportLMBDown() then
                self.mouseMovedWhileButtonDown = true;      -- Trick the viewport into thinking the mouse was moved so that the next OnViewport*WithoutMovement() is disabled.
            end
        end
    end)
    
    self:OnRMBDown(function()
        self.isRMBDown = true;
        
        if self:IsControlsEnabled() then
            self:CaptureMouse();
            
            if self:OnViewportRMBDown() then
                self.mouseMoveWhileButtonDown = true;      -- Trick the viewport into thinking the mouse was moved so that the next OnViewport*WithoutMovement() is disabled.
            end
        end
    end)
    
    self:OnMMBDown(function()
        self.isMMBDown = true;
        
        if self:IsControlsEnabled() then
            self:CaptureMouse();
            
            if self:OnViewportMMBDown() then
                self.mouseMoveWhileButtonDown = true;      -- Trick the viewport into thinking the mouse was moved so that the next OnViewport*WithoutMovement() is disabled.
            end
        end
    end)
    
    
    self:WatchLMBDown(function()
        if self:HasMouseCapture() then
            self.isLMBDown = true;
        end
    end)
    
    self:WatchRMBDown(function()
        if self:HasMouseCapture() then
            self.isRMBDown = true;
        end
    end)
    
    self:WatchMMBDown(function()
        if self:HasMouseCapture() then
            self.isMMBDown = true;
        end
    end)
    
    
    self:WatchLMBUp(function()
        if self:HasMouseCapture() and not self:IsRMBDown() and not self:IsMMBDown() and self:IsControlsEnabled() then
            if not self:MouseMovedWhileButtonDown() and self:IsVisible() then
                self:OnViewportLMBUpWithoutMovement();
            end
            
            self:ReleaseMouse();
        end
        
        self.isLMBDown = false;
    end)
    
    self:WatchRMBUp(function()
        if self:HasMouseCapture() and not self:IsLMBDown() and not self:IsMMBDown() and self:IsControlsEnabled() then
            if not self:MouseMovedWhileButtonDown() and self:IsVisible() then
                self:OnViewportRMBUpWithoutMovement();
            end
            
            self:ReleaseMouse();
        end
        
        self.isRMBDown = false;
    end)
    
    self:WatchMMBUp(function()
        if self:HasMouseCapture() and not self:IsLMBDown() and not self:IsRMBDown() and self:IsControlsEnabled() then
            if not self:MouseMovedWhileButtonDown() and self:IsVisible() then
                self:OnViewportMMBUpWithoutMovement();
            end
            
            self:ReleaseMouse();
        end
        
        self.isMMBDown = false;
    end)
    
    
    self:WatchMouseMove(function(data)
        if self:IsLMBDown() or self:IsRMBDown() or self:IsMMBDown() then
            self.mouseMovedWhileButtonDown = true;
        end
    end)
end
