-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_MaterialsPanel(_internalPtr)
    self:PanelGroupBox("Materials");

    self.MaterialTextBoxes = {};    -- An array of elements.
    
    function self:AddMaterial(path)
        local new = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='textbox' style='width:100%; margin:0px 2px;'>" .. path .. "</div>");
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

    function self:RemoveMaterials()
        self.Body:DeleteAllChildren();
        self.MaterialTextBoxes = {};
    end
    
    function self:GetMaterial(index)
        assert(index <= #self.MaterialTextBoxes, "MaterialPanel:GetMaterial() - index out or range (index = " .. tostring(index) .. ", table size = " .. tostring(#self.MaterialTextBoxes));
        return self.MaterialTextBoxes[index]:GetText();
    end
    
    
    function self:Refresh()
        self:RemoveMaterials();
        
        -- We need to add all of the materials.
        local materials = GTEngine.System.ModelEditor.GetMaterials(_internalPtr);
        for i,value in ipairs(materials) do
            self:AddMaterial(value);
        end
    end
    
    
    
    
    -- We just do a refresh to get everything setup correctly.
    self:Refresh();
    
    
    return self;
end