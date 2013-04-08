-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorPanel(ownerEditor)
    self.OwnerEditor = ownerEditor;

    self.Body = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='particle-editor-panel-body'      />");
    
    self.Scrollbar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='particle-editor-panel-scrollbar' />");
    self.Scrollbar:VerticalScrollbar();
    self.Scrollbar:EnableAutoVisibility();
    self.Scrollbar:SetPageSize(self:GetHeight());
    self.Scrollbar:OnScroll(function(data)
        local offset = -data.position;
        self.Body:SetStyle("inner-offset-y", tostring(offset));
    end)
    
    
    self.EmittersContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.NewEmitterButton = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='particle-editor-newemitter-button' />");
    self.NewEmitterButton:ParticleEditorNewEmitterButton();
    self.NewEmitterButton:OnPressed(function(data)
        self.OwnerEditor:AppendNewEmitter();
    end);
    
    
    
    self.Panels = {};
    
    self.IsMouseOver           = false;
    self.MouseWheelScrollSpeed = 32;
    
    
    function self:Update(definition)
        -- Remove old panels.
        for i,panel in ipairs(self.Panels) do
            GTGUI.Server.DeleteElement(panel);
        end
        
        self.Panels = {};
        
        
    
        -- Add new panels.
        local emitterCount = definition:GetEmitterCount();
        for i=1,emitterCount do
            local panel = GTGUI.Server.New("<div parentid='" .. self.EmittersContainer:GetID() .. "' styleclass='particle-editor-panel-groupbox' />");
            panel:ParticleEditorEmitterPanel("Emitter " .. tostring(i), i);
            
            panel:OnClose(function(data)
                self.OwnerEditor:DeleteEmitterByIndex(panel.Index);
            end);
            
            self.Panels[#self.Panels + 1] = panel;
        end
    end
    
    
    self:OnSize(function()
        if self:IsVisible() then
            self.Scrollbar:SetPageSize(self:GetHeight());
        end
    end);
    
    self.Body:OnSize(function()
        if self.Body:IsVisible() then
            self.Scrollbar:SetRange(self.Body:GetHeight());
        end
    end)
    
    self:OnMouseEnter(function()
        self.IsMouseOver = true;
    end);
    
    self:OnMouseLeave(function()
        self.IsMouseOver = false;
    end);
    
    self:WatchMouseWheel(function(data)
        if self.IsMouseOver then
            self.Scrollbar:Scroll(-data.delta * self.MouseWheelScrollSpeed);
        end
    end);
    

    return self;
end