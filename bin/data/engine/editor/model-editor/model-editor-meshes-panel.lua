-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_MeshesPanelTreeView()
    self:TreeView();
    self:DisableMultiSelect();
    
    self.vertScrollbar:SetStyle("border-right",  "none");
    self.vertScrollbar:SetStyle("border-top",    "none");
    self.vertScrollbar:SetStyle("border-bottom", "none");
    self.horzScrollbar:SetStyle("border-right",  "none");
    self.horzScrollbar:SetStyle("border-bottom", "none");
    self.horzScrollbar:SetStyle("border-left",   "none");

    self.scrollbarFiller:SetStyle("border-left", "1px #5a5a5a");
    self.scrollbarFiller:SetStyle("border-top",  "1px #5a5a5a");

    return self;
end

function GTGUI.Element:ModelEditor_MeshesPanel(_internalPtr)
    self:PanelGroupBox("Meshes");
    
    self.TreeView = GTGUI.Server.CreateElement(self.Body, "model-editor-meshes-panel-treeview");
    self.TreeView:ModelEditor_MeshesPanelTreeView();
    
    self.MeshProperties = GTGUI.Server.CreateElement(self.Body, "model-editor-meshes-panel-properties");
    self.MeshProperties.NoSelectionLabel = GTGUI.Server.CreateElement(self.MeshProperties, "model-editor-meshes-panel-properties-noselection");
    self.MeshProperties.NoSelectionLabel:SetText("No Mesh Selected");
    
    self.MeshProperties.Container        = GTGUI.Server.CreateElement(self.MeshProperties, "model-editor-meshes-panel-properties-container");
    self.MeshProperties.MaterialTextBox  = GTGUI.Server.CreateElement(self.MeshProperties.Container, "labelled-textbox");
    self.MeshProperties.MaterialTextBox:LabelledTextBox("Material");

    
    
    function self:UpdateProperties(meshTreeViewItem)
        self.MeshProperties.MeshIndex = meshTreeViewItem.MeshIndex;
    
        self.MeshProperties.MaterialTextBox.TextBox:UnbindEvent("OnDrop");
        self.MeshProperties.MaterialTextBox.TextBox:OnDrop(function(data)
            if data.droppedElement.isAsset then
                self:ApplyMaterial(self.MeshProperties.MeshIndex, data.droppedElement.path);
            end
        end)
        
        self.MeshProperties.MaterialTextBox:OnKeyPressed(function(data)
            if data.key == GTGUI.Keys.Enter then
                self:ApplyMaterial(self.MeshProperties.MeshIndex, self.MeshProperties.MaterialTextBox:GetText());
            end
        end)
        
        
        local materialRelativePath = GTEngine.System.ModelEditor.GetMaterialRelativePath(_internalPtr, self.MeshProperties.MeshIndex);
        
        self.MeshProperties.MaterialTextBox:SetText(materialRelativePath);
        self.MeshProperties.MaterialTextBox.TextBox:SetTooltip(materialRelativePath);
    end
    
    function self:ShowProperties(meshTreeViewItem)
        self:UpdateProperties(meshTreeViewItem);
            
        self.MeshProperties.NoSelectionLabel:Hide();
        self.MeshProperties.Container:Show();
    end
    
    function self:HideProperties()
        self.MeshProperties.MeshIndex = nil;
        
        self.MeshProperties.Container:Hide();
        self.MeshProperties.NoSelectionLabel:Show();
    end
    
    
    function self:AddMesh(meshName, meshIndex)
        local newItem = self.TreeView:AddItem(meshName);
        if newItem then
            newItem.MeshIndex = meshIndex;
        
            newItem:OnSelected(function()
                self:ShowProperties(newItem);
            end)
            
            newItem:OnDeselected(function()
                self:HideProperties();
            end)
        end
        
        return newItem;
    end
    
    function self:RemoveAllMeshes()
        self.TreeView:RemoveAllItems();
    end
    
    
    function self:ApplyMaterial(meshIndex, materialRelativePath)
        self.MeshProperties.MaterialTextBox:SetText(materialRelativePath);
    
        if GTEngine.System.ModelEditor.SetMaterial(_internalPtr, meshIndex, materialRelativePath) then
            self.MeshProperties.MaterialTextBox.TextBox:SetStyle("border-color", "#6a6a6a");
        else
            self.MeshProperties.MaterialTextBox.TextBox:SetStyle("border-color", "#cc6a6a");
        end
        
        Editor.MarkFileAsModified(GTEngine.System.SubEditor.GetAbsolutePath(_internalPtr));
    end
    
    
    function self:Refresh()
        self:RemoveAllMeshes();
        
        -- We need to add all of the meshes.
        local meshes = GTEngine.System.ModelEditor.GetMeshNames(_internalPtr);
        for i,meshName in ipairs(meshes) do
            self:AddMesh(meshName, i);
        end
    end
    
    
    return self;
end