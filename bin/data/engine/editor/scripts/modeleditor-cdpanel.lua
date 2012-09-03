
function GTGUI.Element:ModelEditor_CDPanel()
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
        Editor.ModelEditor.ShowConvexDecomposition();
    end)
    
    self.ShowConvexDecomposition:OnUnchecked(function()
        Editor.ModelEditor.HideConvexDecomposition();
    end)


    -- CD Settings
    self.CompacityWeight:LabeledSpinner(        "Compacity Weight",          0, 1000000, 1,   0.001);
    self.VolumeWeight:LabeledSpinner(           "Volume Weight",             0, 1000000, 1,   500);
    self.MinClusters:LabeledSpinner(            "Min Clusters",              1, 1000000, 1,   1);
    self.VerticesPerCH:LabeledSpinner(          "Vertices Per CH",           0, 1000000, 10,  100);
    self.Concavity:LabeledSpinner(              "Concavity",                 0, 1000000, 1,   0.001);
    self.SmallThreshold:LabeledSpinner(         "Small Threshold",           0, 1000000, 1,   0.001);
    self.ConnectedDistance:LabeledSpinner(      "Connected Distance",        0, 1000000, 1 ,  0.001);
    self.SimplifiedTriangleCount:LabeledSpinner("Simplified Triangle Count", 0, 1000000, 100, 0);
    
    self.AddExtraDistPoints:CheckBox("Add Extra Distance Points");
    self.AddFacePoints:CheckBox("Add Face Points");
    
    
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
            
            Editor.ModelEditor.BuildConvexDecomposition
            (
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
        Editor_TabBar:GetActiveItem():MarkAsModified();
    end);
end