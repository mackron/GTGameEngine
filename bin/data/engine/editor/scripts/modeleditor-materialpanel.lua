
function GTGUI.Element:ModelEditor_MaterialPanel()
    self:PanelGroupBox("Materials");

    self.MaterialTextBoxes = {};    -- An array of elements.
    
    function Editor_ModelEditor_Panel_Materials:AddMaterial(path)
        local new = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='textbox' style='width:100%; margin:0px 2px;'>" .. path .. "</div>");
        new.index = #self.MaterialTextBoxes + 1;
        
        new:OnKeyPressed(function(data)
            if data.key == GTGUI.Keys.Enter then
                if Editor.ModelEditor.SetMaterial(new.index, new:GetText()) then
                    new:SetStyle("border-color", "#6a6a6a");
                else
                    new:SetStyle("border-color", "#cc6a6a");
                end
            end
        end)
        
        self.MaterialTextBoxes[new.index] = new;
    end
    
    function Editor_ModelEditor_Panel_Materials:RemoveMaterials()
        self.Body:DeleteAllChildren();
        self.MaterialTextBoxes = {};
    end
    
    function Editor_ModelEditor_Panel_Materials:GetMaterial(index)
        assert(index <= #self.MaterialTextBoxes, "MaterialPanel:GetMaterial() - index out or range (index = " .. tostring(index) .. ", table size = " .. tostring(#self.MaterialTextBoxes));
        return self.MaterialTextBoxes[index]:GetText();
    end
end