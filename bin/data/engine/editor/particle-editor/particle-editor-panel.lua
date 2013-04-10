-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ParticleEditorPanel(ownerEditor)
    self:EditorPanel();
    self.OwnerEditor = ownerEditor;

    self.EmittersContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='width:100%; height:auto;' />");
    
    self.NewEmitterButton  = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='particle-editor-newemitter-button' />");
    self.NewEmitterButton:ParticleEditorNewEmitterButton();
    self.NewEmitterButton:OnPressed(function(data)
        self.OwnerEditor:AppendNewEmitter();
    end);
    
    
    
    self.Panels = {};
    
    
    
    
    function self:Refresh(definition)
        -- Remove old panels.
        for i,panel in ipairs(self.Panels) do
            GTGUI.Server.DeleteElement(panel);
        end
        
        self.Panels = {};
        
        
    
        -- Add new panels.
        local emitterCount = definition:GetEmitterCount();
        for i=1,emitterCount do
            local panel = GTGUI.Server.New("<div parentid='" .. self.EmittersContainer:GetID() .. "' styleclass='particle-editor-panel-groupbox' />");
            panel:ParticleEditorEmitterPanel(definition:GetEmitterByIndex(i), i, ownerEditor);
            
            panel:OnClose(function(data)
                self.OwnerEditor:DeleteEmitterByIndex(panel.Index);
            end);
            
            self.Panels[#self.Panels + 1] = panel;
        end
    end
    
    
    
    return self;
end