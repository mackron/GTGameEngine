-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.


function GTGUI.Element:ParticleSystemComponentPanel()
    self:PanelGroupBox("Particle System", true);
    
    -- File Path
    self.FilePath = GTGUI.Server.CreateElement(self.Body, "particle-component-panel-file-path-textbox");
    
    -- Play on Startup.
    self.PlayOnStartup = GTGUI.Server.CreateElement(self.Body, "checkbox");
    self.PlayOnStartup:CheckBox("Play on Startup");
    self.PlayOnStartup:SetStyle("margin-top", "4px");
    
    
    self.FilePath:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            self:UpdateFile();
        end
    end);
    
    self.FilePath:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.FilePath:SetText(data.droppedElement.path);
            self:UpdateFile();
        end
    end);
    
    
    
    self.PlayOnStartup:OnChecked(function(data)
        self.CurrentComponent:PlayOnStartup(true);
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    self.PlayOnStartup:OnUnchecked(function(data)
        self.CurrentComponent:PlayOnStartup(false);
        self.CurrentComponent:Reset();
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    

    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.ParticleSystem);
        
        self.FilePath:SetText(self.CurrentComponent:GetRelativeFilePath(), true);           -- 'true' = block OnTextChanged event.
        self.PlayOnStartup:SetChecked(self.CurrentComponent:IsPlayingOnStartup(), true);    -- 'true' = block OnChecked/OnUnchecked event.
    end
    
    
    function self:UpdateFile()
        self.CurrentComponent:SetFromFile(self.FilePath:GetText());
        self.ParentPanel:OnSceneNodeChanged();
    end
    

    return self;
end