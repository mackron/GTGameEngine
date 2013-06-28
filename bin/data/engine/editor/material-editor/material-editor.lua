-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:MaterialEditor(_internalPtr)
    self._internalPtr    = _internalPtr;
    
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
    self.ScriptTextBox:MultiLineTextBox();
    
    return self;
end