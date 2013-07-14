-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelComponentPanel()
    self:PanelGroupBox("Model", true);
    
    -- Model
    self.ModelPath   = GTGUI.Server.CreateElement(self.Body, "model-component-panel-model-textbox");
    
    -- Shadows
    self.CastShadows = GTGUI.Server.CreateElement(self.Body, "model-component-panel-cast-shadows-checkbox");
    
    -- Materials
    self.MaterialsLabel = GTGUI.Server.CreateElement(self.Body, "model-component-panel-materials-label");
    self.MaterialsLabel:SetText("Materials:");

    self.MaterialTextBoxes = {};
    
    
    
    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;
    
    
    self.ModelPath:OnKeyPressed(function(data)
        if data.key == GTGUI.Keys.Enter then
            self:UpdateModel();
        end
    end);
    
    self.ModelPath:OnDrop(function(data)
        if data.droppedElement.isAsset then
            self.ModelPath:SetText(data.droppedElement.path);
            self:UpdateModel();
        end
    end);
    
    
    
    self.CastShadows:CheckBox("Cast Shadows");
    
    self.CastShadows:OnChecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:EnableShadowCasting();
            self.ParentPanel:OnSceneNodeChanged();
        end;
    end);
    
    self.CastShadows:OnUnchecked(function()
        if self.CurrentComponent ~= nil then
            self.CurrentComponent:DisableShadowCasting();
            self.ParentPanel:OnSceneNodeChanged();
        end;
    end);
    
    
    
    function self:AddMaterialTextBox(path)
        local new = GTGUI.Server.CreateElement(self.Body, "model-component-panel-material-textbox");
        new:SetText(path);
        new.index = #self.MaterialTextBoxes + 1;
        
        new:OnKeyPressed(function(data)
            if data.key == GTGUI.Keys.Enter then
                self:UpdateModelMaterial(new.index);
            end
        end)
        
        new:OnDrop(function(data)
            if data.droppedElement.isAsset then
                new:SetText(data.droppedElement.path);
                self:UpdateModelMaterial(new.index);
            end
        end)

        
        self.MaterialTextBoxes[new.index] = new;
    end
    
    
    function self:RemoveMaterials()
        for i,value in ipairs(self.MaterialTextBoxes) do
            GTGUI.Server.DeleteElement(value);
        end
        
        self.MaterialTextBoxes = {};
    end
    
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Model);
        
        if self.CurrentComponent ~= nil then
            self.ModelPath:SetText(self.CurrentComponent:GetModelPath());
            
            if self.CurrentComponent:IsShadowCastingEnabled() then
                self.CastShadows:Check(true);
            else
                self.CastShadows:Uncheck(true);
            end
            
            -- Materials
            self:RemoveMaterials();
            
            local materialCount = self.CurrentComponent:GetMaterialCount();
            for i=1,materialCount do
                self:AddMaterialTextBox(self.CurrentComponent:GetMaterialPath(i));
            end
        end
    end
    
    function self:UpdateModel()
        if self.CurrentComponent ~= nil and self.CurrentNode ~= nil then
            self.CurrentComponent:SetModel(self.ModelPath:GetText());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    function self:UpdateModelMaterial(index)
        if self.CurrentComponent ~= nil and self.CurrentNode ~= nil then
            local textbox = self.MaterialTextBoxes[index];
        
            if self.CurrentComponent:SetMaterial(index, textbox:GetText()) then
                textbox:SetStyle("border-color", "#6a6a6a");
            else
                textbox:SetStyle("border-color", "#cc6a6a");
            end
            
            self.ParentPanel:OnSceneNodeChanged();
        end
    end
    
    
    return self;
end