-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditor(_internalPtr)
    self:SubEditor(_internalPtr);
    self.ParticleSystemDefinition = GTEngine.ParticleSystemDefinition:Create(GTEngine.System.ParticleEditor.GetParticleSystemDefinitionPtr(self._internalPtr));
    

    self.Viewport = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='particle-editor-viewport' />");
    self.Viewport:ParticleEditorViewport();
    
    self.Panel    = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='particle-editor-panel' />");
    self.Panel:ParticleEditorPanel(self);
    


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
        self:RefreshViewport();
        self:MarkAsModified();
    end
    
    
    
    
    
    -------------------------------
    -- FFI Connectors.
    
    function self:RefreshViewport()
        return GTEngine.System.ParticleEditor.RefreshViewport(self._internalPtr);
    end

    
    
    
    
    
    
    -- At this point we want to make sure everything is up-to-date.
    self:RefreshPanel();
    
    return self;
end