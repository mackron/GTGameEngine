-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_MeshesPanelTreeView()
    self:TreeView();
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
    
    --[[
    self.MaterialTextBoxes = {};    -- An array of elements.
    
    function self:AddMaterial(path)
        local new = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='textbox' style='width:100%; margin:0px 2px;'>" .. path .. "</div>");
        new:SetTooltip(path);
        new.index = #self.MaterialTextBoxes + 1;
        
        new:OnKeyPressed(function(data)
            if data.key == GTGUI.Keys.Enter then
                new:ApplyMaterial();
            end
        end)
        
        new:OnDrop(function(data)
            if data.droppedElement.isAsset then
                new:SetText(data.droppedElement.path);
                new:ApplyMaterial();
            end
        end)
        
        
        function new:ApplyMaterial()
            if GTEngine.System.ModelEditor.SetMaterial(_internalPtr, self.index, self:GetText()) then
                self:SetStyle("border-color", "#6a6a6a");
            else
                self:SetStyle("border-color", "#cc6a6a");
            end
            
            Editor.MarkFileAsModified(GTEngine.System.SubEditor.GetAbsolutePath(_internalPtr));
        end
        
        
        self.MaterialTextBoxes[new.index] = new;
    end
    --]]

    
    function self:AddMesh(meshName)
        self.TreeView:AddItem(meshName);
    end
    
    function self:RemoveAllMeshes()
        self.TreeView:RemoveAllItems();
    end
    
    --[[
    function self:GetMaterial(index)
        assert(index <= #self.MaterialTextBoxes, "MaterialPanel:GetMaterial() - index out or range (index = " .. tostring(index) .. ", table size = " .. tostring(#self.MaterialTextBoxes));
        return self.MaterialTextBoxes[index]:GetText();
    end
    ]]
    
    function self:Refresh()
        self:RemoveAllMeshes();
        
        -- We need to add all of the meshes.
        local meshes = GTEngine.System.ModelEditor.GetMeshNames(_internalPtr);
        for i,meshName in ipairs(meshes) do
            print("TESTING " .. meshName);
            self:AddMesh(meshName);
        end
    end
    
    
    return self;
end