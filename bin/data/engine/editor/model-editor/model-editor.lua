-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditorPanel(_internalPtr)
    self.Body      = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='model-editor-panel-body' style='' />");
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='vertical-scrollbar'      style='' />");
    
    self.MaterialsPanel          = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-groupbox' style='' />");
    --self.AnimationsPanel         = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-groupbox' style='' />");
    self.AnimationSegmentsPanel  = GTGUI.Server.CreateElement(self.Body, "panel-groupbox");
    self.AnimationSequencesPanel = GTGUI.Server.CreateElement(self.Body, "panel-groupbox");
    self.CDPanel                 = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='panel-groupbox' style='' />");
    
    self.MaterialsPanel:ModelEditor_MaterialsPanel(_internalPtr);
    --self.AnimationsPanel:ModelEditor_AnimationsPanel(_internalPtr);
    self.AnimationSegmentsPanel:ModelEditor_AnimationSegmentsPanel(_internalPtr);
    self.AnimationSequencesPanel:ModelEditor_AnimationSequencesPanel(_internalPtr);
    self.CDPanel:ModelEditor_CDPanel(_internalPtr);
    
    
    self.Scrollbar:VerticalScrollbar();
    self.Scrollbar:SetPageSize(self:GetHeight());
    self.Scrollbar:EnableAutoVisibility();
    self.Scrollbar:OnScroll(function(data)
        local offset = -data.position;
        self.Body:SetStyle("inner-offset-y", tostring(offset));
    end);
    
    
    
    function self:Refresh()
        self.MaterialsPanel:Refresh();
        --self.AnimationsPanel:Refresh();
        self.AnimationSegmentsPanel:Refresh();
        self.AnimationSequencesPanel:Refresh();
        self.CDPanel:Refresh();
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

function GTGUI.Element:ModelEditor(_internalPtr)
    self.ViewportTimelineContainer = GTGUI.Server.CreateElement(self, "model-editor-viewport-timeline-container");
    self.Viewport = GTGUI.Server.CreateElement(self.ViewportTimelineContainer, "model-editor-viewport");
    self.Timeline = GTGUI.Server.CreateElement(self.ViewportTimelineContainer, "model-editor-timeline");

    self.Panel    = GTGUI.Server.CreateElement(self, "model-editor-panel");
    
    
    self._internalPtr    = _internalPtr;
    self.IsLMBDown       = false;
    self.IsRMBDown       = false;
    self.HasMouseCapture = false;
    
    self.Panel:ModelEditorPanel(self._internalPtr);
    
    
    function self:Refresh()
        self.Panel:Refresh();
    end
    
    
    
    
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