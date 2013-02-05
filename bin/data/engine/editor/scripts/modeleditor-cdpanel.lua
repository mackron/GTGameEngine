-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_CDPanel(_internalPtr)
    self:PanelGroupBox("Convex Decomposition");
    
    self.ShowConvexDecomposition = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' />");
    
    GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='width:100%; height:1px; margin:0px 8px; background-color:#444;' />");
    
    self.CompacityWeight         = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='labeled-spinner' style='margin:0px 1px;' />");
    self.VolumeWeight            = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='labeled-spinner' style='margin:0px 1px;' />");
    self.MinClusters             = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='labeled-spinner' style='margin:0px 1px;' />");
    self.VerticesPerCH           = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='labeled-spinner' style='margin:0px 1px;' />");
    self.Concavity               = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='labeled-spinner' style='margin:0px 1px;' />");
    self.SmallThreshold          = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='labeled-spinner' style='margin:0px 1px;' />");
    self.ConnectedDistance       = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='labeled-spinner' style='margin:0px 1px;' />");
    self.SimplifiedTriangleCount = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='labeled-spinner' style='margin:0px 1px;' />");
    
    self.AddExtraDistPoints      = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' style='margin-bottom:4px; margin-top:4px;' />");
    self.AddFacePoints           = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='checkbox' />");
    
    GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' style='width:100%; height:1px; margin:0px 8px; background-color:#444;' />");
    
    self.BuildButton             = GTGUI.Server.New("<div parentid='" .. self.Body:GetID() .. "' styleclass='button' style='width:100%;' />");
    
    
    -- "Show in Viewport" CheckBox.
    self.ShowConvexDecomposition:CheckBox("Show in Viewport");
    
    self.ShowConvexDecomposition:OnChecked(function()
        GTEngine.System.ModelEditor.ShowConvexDecomposition(_internalPtr);
    end)
    
    self.ShowConvexDecomposition:OnUnchecked(function()
        GTEngine.System.ModelEditor.HideConvexDecomposition(_internalPtr);
    end)
    
    
    local settings = GTEngine.System.ModelEditor.GetConvexHullBuildSettings(_internalPtr);
    self.CompacityWeight:LabeledSpinner(        "Compacity Weight",          0, 1000000, 1,   settings.compacityWeight);
    self.VolumeWeight:LabeledSpinner(           "Volume Weight",             0, 1000000, 1,   settings.volumeWeight);
    self.MinClusters:LabeledSpinner(            "Min Clusters",              1, 1000000, 1,   settings.minClusters);
    self.VerticesPerCH:LabeledSpinner(          "Vertices Per CH",           0, 1000000, 10,  settings.verticesPerCH);
    self.Concavity:LabeledSpinner(              "Concavity",                 0, 1000000, 1,   settings.concavity);
    self.SmallThreshold:LabeledSpinner(         "Small Threshold",           0, 1000000, 1,   settings.smallClusterThreshold);
    self.ConnectedDistance:LabeledSpinner(      "Connected Distance",        0, 1000000, 1 ,  settings.connectedComponentsDist);
    self.SimplifiedTriangleCount:LabeledSpinner("Simplified Triangle Count", 0, 1000000, 100, settings.simplifiedTriangleCountTarget);
    
    self.AddExtraDistPoints:CheckBox("Add Extra Distance Points");
    self.AddFacePoints:CheckBox("Add Face Points");
    
    
    function self:Refresh()
        -- CD Settings
        local settings = GTEngine.System.ModelEditor.GetConvexHullBuildSettings(_internalPtr);
        
        self.CompacityWeight:SetValue(        settings.compacityWeight);
        self.VolumeWeight:SetValue(           settings.volumeWeight);
        self.MinClusters:SetValue(            settings.minClusters);
        self.VerticesPerCH:SetValue(          settings.verticesPerCH);
        self.Concavity:SetValue(              settings.concavity);
        self.SmallThreshold:SetValue(         settings.smallClusterThreshold);
        self.ConnectedDistance:SetValue(      settings.connectedComponentsDist);
        self.SimplifiedTriangleCount:SetValue(settings.simplifiedTriangleCountTarget);
        
        if settings.addExtraDistPoints then
            self.AddExtraDistPoints:Check();
        end
        
        if settings.addFacesPoints then
            self.AddFacePoints:Check();
        end
    end
    
    

    -- Build button.
    self.BuildButton:Button("Build"):OnPressed(function()
        self.BuildButton:Disable();
            local compacityWeight         = self.CompacityWeight:GetValue();
            local volumeWeight            = self.VolumeWeight:GetValue();
            local minClusters             = self.MinClusters:GetValue();
            local verticesPerCH           = self.VerticesPerCH:GetValue();
            local concavity               = self.Concavity:GetValue();
            local smallThreshold          = self.SmallThreshold:GetValue();
            local connectedDistance       = self.ConnectedDistance:GetValue();
            local simplifiedTriangleCount = self.SimplifiedTriangleCount:GetValue();
            local addExtraDistPoints      = self.AddExtraDistPoints:IsChecked();
            local addFacePoints           = self.AddFacePoints:IsChecked();
            
            GTEngine.System.ModelEditor.BuildConvexDecomposition(_internalPtr,
                compacityWeight,
                volumeWeight,
                minClusters,
                verticesPerCH,
                concavity,
                smallThreshold,
                connectedDistance,
                simplifiedTriangleCount,
                addExtraDistPoints,
                addFacePoints
            );
        self.BuildButton:Enable();
        Editor.MarkFileAsModified(GTEngine.System.SubEditor.GetAbsolutePath(_internalPtr));
    end);
    
    
    
    -- We just do a refresh to get everything setup correctly.
    self:Refresh();
    
    return self;
end