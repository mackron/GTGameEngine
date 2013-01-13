
function GTGUI.Element:ModelEditorPanel(_internalPtr)
    self.Body      = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='model-editor-panel-body' style='' />");
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='vertical-scrollbar'      style='' />");
    
    self.MaterialsPanel  = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-groupbox' style='' />");
    self.AnimationsPanel = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-groupbox' style='' />");
    self.CDPanel         = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-groupbox' style='' />");
    
    self.MaterialsPanel:ModelEditor_MaterialsPanel(_internalPtr);
    self.AnimationsPanel:ModelEditor_AnimationsPanel(_internalPtr);
    self.CDPanel:ModelEditor_CDPanel(_internalPtr);
    
    
    self.Scrollbar:VerticalScrollbar();
    self.Scrollbar:SetPageSize(self:GetHeight());
    self.Scrollbar:EnableAutoVisibility();
    self.Scrollbar:OnScroll(function(data)
        local offset = -data.position;
        self.Body:SetStyle("inner-offset-y", tostring(offset));
    end);
    
    
    
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

function GTGUI.Element:ModelEditor(_internalPtr)
    self.Viewport = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='model-editor-viewport' style='' />");
    self.Panel    = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='model-editor-panel'    style='' />");
    
    self._internalPtr    = _internalPtr;
    self.IsLMBDown       = false;
    self.IsRMBDown       = false;
    self.HasMouseCapture = false;
    
    self.Panel:ModelEditorPanel(self._internalPtr);
    
    
    
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