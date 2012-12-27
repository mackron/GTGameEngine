
function GTGUI.Element:SceneEditor()
    self.Viewport    = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='scene-editor-viewport' style='' />");
    self.Panel       = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='scene-editor-panel'    style='' />");
    self.ContextMenu = GTGUI.Server.New("<div                                   styleclass='menu'                  style='z-index:100; positioning:absolute; visible:false' />");
    
    self.IsRMBDown              = false;
    self.MouseMovedWhileRMBDown = false;                -- Used to determine whether or not to show the right-click context menu.
    
    
    self.ContextMenu:Menu();
    
    self.ContextMenu:AppendItem("Add Cube"):OnPressed(function()
        local newNode = Editor.SceneEditor.AddSceneNode("Cube");
        if newNode ~= nil then
            newNode:AddComponent(GTEngine.Components.ModelComponent):SetModel("engine/models/default-1x1.dae");
            newNode:Refresh();
        end
        
        self.ContextMenu:Hide();
    end);
    
    
    
    self.Viewport:OnRMBDown(function()
        self.IsRMBDown = true;
    end);
    
    self.Viewport:OnRMBUp(function()
        if not self.MouseMovedWhileRMBDown then
            self.ContextMenu:SetPosition(GTGUI.Server.GetMousePosition());
            self.ContextMenu:Show();
        end
        
        self.IsRMBDown              = false;
        self.MouseMovedWhileRMBDown = false;
    end);
    
    self.Viewport:OnMouseMove(function(data)
        if self.IsRMBDown then
            self.MouseMovedWhileRMBDown = true;
        end
    end);
    
    self:WatchLMBDown(function(data)
        if not self.ContextMenu:IsChild(data.receiver) then self.ContextMenu:Hide(); end
    end);
    
    self:WatchRMBDown(function(data)
        if not self.ContextMenu:IsChild(data.receiver) then self.ContextMenu:Hide(); end
    end);
end