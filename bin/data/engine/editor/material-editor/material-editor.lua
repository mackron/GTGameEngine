-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:MaterialEditor(_internalPtr)
    self._internalPtr    = _internalPtr;
    self.IsLMBDown       = false;
    self.IsRMBDown       = false;
    self.HasMouseCapture = false;
    
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
    
    
    self.ScriptTextBox:MultiLineTextBox();
    
    
    
    
    self.Viewport:OnLMBDown(function()
        self.IsLMBDown       = true;
        self.HasMouseCapture = true;
        self:Focus();
    end);
    
    self.Viewport:OnRMBDown(function()
        self.IsRMBDown       = true;
        self.HasMouseCapture = true;
        self:Focus();
    end);

    
    
    self:WatchLMBDown(function(data)
        if self.HasMouseCapture then
            self.IsLMBDown = true;
        end
    end);
    
    self:WatchRMBDown(function(data)
        if self.HasMouseCapture then
            self.IsRMBDown = true;
        end
    end);
    
    self:WatchLMBUp(function(data)
        if not GTGUI.Server.IsRMBDown() then
            Game.ReleaseMouse();
            self.HasMouseCapture = false;
        end
        
        self.IsLMBDown = false;
    end);
    
    self:WatchRMBUp(function(data)
        if not GTGUI.Server.IsLMBDown() then
            Game.ReleaseMouse();
            self.HasMouseCapture = false;
        end
        
        self.IsRMBDown = false;
    end);
    
    return self;
end