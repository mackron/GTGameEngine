-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ProximityComponentPanel()
    self:PanelGroupBox("Proximity", true);
    
    -- Shapes
    self.CollisionShapesLabel = GTGUI.Server.CreateElement(self.Body, "proximity-component-panel-shapes-label");
    self.CollisionShapesLabel:SetText("Shapes:");

    
    self.CollisionShapes = GTGUI.Server.CreateElement(self.Body);
    self.CollisionShapes:CollisionShapesPanel();
    
    self.CollisionShapes:OnShapesChanged(function()
        self.ParentPanel:OnSceneNodeChanged();
    end);
    
    
    self.CurrentNode      = nil;
    self.CurrentComponent = nil;
    self.IsUpdating       = false;
    
    
    function self:Update(node)
        self.CurrentNode      = node;
        self.CurrentComponent = node:GetComponent(GTEngine.Components.Proximity);
        self.IsUpdating       = true;
        
        if self.CurrentComponent ~= nil then
            self.CollisionShapes:Update(self.CurrentComponent);
        end
        
        self.IsUpdating = false;
    end
    
    return self;
end

