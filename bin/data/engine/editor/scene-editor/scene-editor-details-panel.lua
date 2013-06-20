-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorDetailsPanel(parentPanel, sceneEditor)
    self:PanelGroupBox("Details");
    self.ParentPanel = parentPanel;
    
    self.NameTextBox = GTGUI.Server.CreateElement(self.Body, "labelled-textbox");
    self.NameTextBox:LabelledTextBox("Name");
    self.NameTextBox:SetTooltip("The name of the scene node. Does not need to be unique.");
    
    self.PrefabContainer    = GTGUI.Server.CreateElement(self.Body,            "scene-editor-details-panel-prefab-container");
    self.PrefabLabel        = GTGUI.Server.CreateElement(self.PrefabContainer, "scene-editor-details-panel-prefab-label");
    self.PrefabLabel:SetText("Prefab");
    
    self.PrefabName         = GTGUI.Server.CreateElement(self.PrefabContainer, "scene-editor-details-panel-prefab-name");
    self.PrefabName:SetText("None");
    self.PrefabName:SetTooltip("The prefab this node is linked to.");
    
    self.PrefabUpdateButton = GTGUI.Server.CreateElement(self.PrefabContainer, "scene-editor-details-panel-prefab-update-icon");
    self.PrefabUpdateButton:OnPressed(function()
        sceneEditor:UpdatePrefab(self.PrefabName.RelativePath, nil, self.SceneNode);
    end);
    self.PrefabUpdateButton:SetTooltip("Update the prefab.");
    
    self.PrefabCrossButton  = GTGUI.Server.CreateElement(self.PrefabContainer, "cross-button");
    self.PrefabCrossButton:OnPressed(function()
        sceneEditor:UnlinkSceneNodeFromPrefab(self.SceneNode);
    end);
    self.PrefabCrossButton:SetTooltip("Unlink from prefab.");
    
    
    
    self.SceneNode = nil;
    
    
    function self:Update(node)
        self.SceneNode = node;
        self.NameTextBox:SetText(node:GetName());
        
        if self.SceneNode.Prefab then
            self.PrefabName.RelativePath = self.SceneNode.Prefab:GetPrefabRelativePath();
            self.PrefabName:SetText(GTCore.IO.GetFileNameFromPath(self.PrefabName.RelativePath));
            
            self.PrefabCrossButton:Show();      -- It's possible for the cross button to be hidden, so ensure it's shown here.
            
            if self.SceneNode.Prefab:GetLocalHierarchyID() == 1 then
                self.PrefabUpdateButton:Show();
            else
                self.PrefabUpdateButton:Hide();
            end
        else
            self.PrefabName:SetText("None");
            
            self.PrefabUpdateButton:Hide();
            self.PrefabCrossButton:Hide();
        end
    end
    
    
    self.NameTextBox:OnKeyPressed(function(data)
        if data.key == GTCore.Keys.Enter then
            self.SceneNode:SetName(self.NameTextBox:GetText());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    return self;
end