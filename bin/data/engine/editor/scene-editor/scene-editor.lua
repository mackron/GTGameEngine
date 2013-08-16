-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:CollisionShapePanel(title, hideOffset)
    self.TitleContainer = GTGUI.Server.CreateElement(self, "collision-shape-panel-title-container");
    self.Title          = GTGUI.Server.CreateElement(self.TitleContainer, "collision-shape-panel-title");
    self.DeleteButton   = GTGUI.Server.CreateElement(self.TitleContainer, "collision-shape-panel-delete-button");
    
    self.Title:SetText(title);


    if not hideOffset then
        self.OffsetInput = GTGUI.Server.CreateElement(self, "labelled-vector3-input");
        self.OffsetInput:LabelledVector3Input("Offset:");
        
        self.OffsetInput:OnValueChanged(function(data)
            self:OnOffsetChanged(data);
        end);
    end
    
    
    self.DeleteButton:OnPressed(function()
        self:OnDelete();
    end);
    
    
    
    -- Retrieves the offset as 3 floats.
    function self:GetOffsetXYZ()
        if self.OffsetInput ~= nil then
            return self.OffsetInput:GetXYZ();
        else
            return 0.0, 0.0, 0.0;
        end
    end
    
    
    function self:UpdateOffset(shape)
        if self.OffsetInput ~= nil then
            if shape.offsetX ~= nil and shape.offsetY ~= nil and shape.offsetZ ~= nil then
                self.OffsetInput:SetValue(shape.offsetX, shape.offsetY, shape.offsetZ);
            end
        end
    end
    


    function self:OnDelete(arg1)
        self.Callbacks:BindOrCall("OnDelete", arg1);
    end
    
    function self:OnOffsetChanged(arg1)
        self.Callbacks:BindOrCall("OnOffsetChanged", arg1);
    end
    
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_Box()
    self:CollisionShapePanel("Box");

    self.ExtentsInput = GTGUI.Server.CreateElement(self, "labelled-vector3-input");
    self.ExtentsInput:LabelledVector3Input("Extents:");
    self.ExtentsInput:SetStyle("margin-top", "4px");
    
    self.ExtentsInput:OnValueChanged(function(data)
        self:OnExtentsChanged(data);
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.ExtentsInput:SetValue(shape.halfX * 2.0, shape.halfY * 2.0, shape.halfZ * 2.0);
    end
    
    function self:OnExtentsChanged(arg1)
        self.Callbacks:BindOrCall("OnExtentsChanged", arg1);
    end
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_Sphere()
    self:CollisionShapePanel("Sphere");
    
    self.RadiusInput = GTGUI.Server.CreateElement(self, "labelled-number-input");
    self.RadiusInput:LabelledNumberInput("Radius:");
    
    self.RadiusInput:OnValueChanged(function()
        self:OnRadiusChanged({radius = self.RadiusInput:GetValue()});
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.RadiusInput:SetValue(shape.radius);
    end
    
    
    function self:OnRadiusChanged(arg1)
        self.Callbacks:BindOrCall("OnRadiusChanged", arg1);
    end
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_Ellipsoid()
    self:CollisionShapePanel("Ellipsoid");
    
    self.RadiusInput = GTGUI.Server.CreateElement(self, "labelled-vector3-input");
    self.RadiusInput:LabelledVector3Input("Radius:");
    self.RadiusInput:SetStyle("margin-top", "4px");
    

    self.RadiusInput:OnValueChanged(function(data)
        self:OnRadiusChanged(data);
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.RadiusInput:SetValue(shape.radiusX, shape.radiusY, shape.radiusZ);
    end
    
    
    function self:OnRadiusChanged(arg1)
        self.Callbacks:BindOrCall("OnRadiusChanged", arg1);
    end
    
    return self;
end


function GTGUI.Element:CollisionShapePanel_Cylinder(title)
    self:CollisionShapePanel(title);
    
    self.RadiusInput = GTGUI.Server.CreateElement(self, "labelled-number-input");
    self.RadiusInput:LabelledNumberInput("Radius:");
    
    self.LengthInput = GTGUI.Server.CreateElement(self, "labelled-number-input");
    self.LengthInput:LabelledNumberInput("Length:");
    
    
    self.RadiusInput:OnValueChanged(function(data)
        self:OnSizeChanged({radius = self.RadiusInput:GetValue(); length = self.LengthInput:GetValue()});
    end);
    
    self.LengthInput:OnValueChanged(function(data)
        self:OnSizeChanged({radius = self.RadiusInput:GetValue(); length = self.LengthInput:GetValue()});
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.RadiusInput:SetValue(shape.radius);
        self.LengthInput:SetValue(shape.length);
    end
    
    
    function self:OnSizeChanged(arg1)
        self.Callbacks:BindOrCall("OnSizeChanged", arg1);
    end
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CylinderX()
    self:CollisionShapePanel_Cylinder("Cylinder X");
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CylinderY()
    self:CollisionShapePanel_Cylinder("Cylinder Y");
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CylinderZ()
    self:CollisionShapePanel_Cylinder("Cylinder Z");
    
    return self;
end


function GTGUI.Element:CollisionShapePanel_Capsule(title)
    self:CollisionShapePanel(title);
    
    self.RadiusInput = GTGUI.Server.CreateElement(self, "labelled-number-input");
    self.RadiusInput:LabelledNumberInput("Radius:");
    
    self.HeightInput = GTGUI.Server.CreateElement(self, "labelled-number-input");
    self.HeightInput:LabelledNumberInput("Height:");

    
    self.RadiusInput:OnValueChanged(function()
        self:OnSizeChanged({radius = self.RadiusInput:GetValue(), height = self.HeightInput:GetValue()});
    end);
    
    self.HeightInput:OnValueChanged(function()
        self:OnSizeChanged({radius = self.RadiusInput:GetValue(), height = self.HeightInput:GetValue()});
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.RadiusInput:SetValue(shape.radius);
        self.HeightInput:SetValue(shape.height);
    end
    
    
    function self:OnSizeChanged(arg1)
        self.Callbacks:BindOrCall("OnSizeChanged", arg1);
    end

    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CapsuleX()
    self:CollisionShapePanel_Capsule("Capsule X");
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CapsuleY()
    self:CollisionShapePanel_Capsule("Capsule Y");
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_CapsuleZ()
    self:CollisionShapePanel_Capsule("Capsule Z");
    
    return self;
end


function GTGUI.Element:CollisionShapePanel_ConvexHull()
    self:CollisionShapePanel("Convex Hull", true);              -- 'true' means to hide the offset properties since they're not used with convex hulls at the moment.
    
    return self;
end

function GTGUI.Element:CollisionShapePanel_ModelConvexHulls()
    self:CollisionShapePanel("Model Convex Hulls", true);       -- 'true' means to hide the offset properties since they're not used with convex hulls at the moment.
    
    self.MarginInput = GTGUI.Server.CreateElement(self, "labelled-number-input");
    self.MarginInput:LabelledNumberInput("Margin:");
    
    self.MarginInput:OnValueChanged(function()
        self:OnSizeChanged({margin = self.MarginInput:GetValue()});
    end);
    
    
    function self:Update(shape)
        self:UpdateOffset(shape);
        self.MarginInput:SetValue(shape.margin);
    end
    
    function self:OnSizeChanged(arg1)
        self.Callbacks:BindOrCall("OnSizeChanged", arg1);
    end
    
    return self;
end


function GTGUI.Element:NewCollisionShapeMenu()
    self.Items = {}
    
    function self:AppendNewItem(title)
        local newItem = GTGUI.Server.CreateElement(self, "collision-shape-panel-new-menu-item");
        newItem:SetText(title);
        
        self.Items[newItem:GetID()] = newItem;
        
        return newItem;
    end
end


function GTGUI.Element:NewComponentMenu()
    self.Items = {}
    
    function self:AppendNewItem(title)
        local newItem = GTGUI.Server.CreateElement(self, "new-component-menu-item");
        newItem:SetText(title);
        
        self.Items[newItem:GetID()] = newItem;
        
        return newItem;
    end
end


-- Editor.SceneEditor.CreateCustomComponentPanel()
--
-- This should be overwritten by the client game so that it can create it's own GUI panels for it's custom components, if it has any.
function Editor.SceneEditor.CreateCustomComponentPanel(panelElement, componentID)
end

function Editor.SceneEditor.CreateComponentPanel(parentPanel, componentID)
    local element = GTGUI.Server.CreateElement(parentPanel.Body.PanelsContainer, "panel-groupbox");
    element.ParentPanel = parentPanel;
    
    
    if     componentID == GTEngine.Components.Camera then
        element:CameraComponentPanel();
    elseif componentID == GTEngine.Components.Model then
        element:ModelComponentPanel();
    elseif componentID == GTEngine.Components.Camera then
        element:CameraComponentPanel();
    elseif componentID == GTEngine.Components.PointLight then
        element:PointLightComponentPanel();
    elseif componentID == GTEngine.Components.SpotLight then
        element:SpotLightComponentPanel();
    elseif componentID == GTEngine.Components.DirectionalLight then
        element:DirectionalLightComponentPanel();
    elseif componentID == GTEngine.Components.AmbientLight then
        element:AmbientLightComponentPanel();
    elseif componentID == GTEngine.Components.Dynamics then
        element:DynamicsComponentPanel();
    elseif componentID == GTEngine.Components.Proximity then
        element:ProximityComponentPanel();
    elseif componentID == GTEngine.Components.ParticleSystem then
        element:ParticleSystemComponentPanel();
    elseif componentID == GTEngine.Components.Script then
        element:ScriptComponentPanel();
    elseif componentID == GTEngine.Components.EditorMetadata then 
        element:EditorMetadataComponentPanel();
    else
        Editor.SceneEditor.CreateCustomComponentPanel(element, componentID);
    end
    
    
    -- Always have the EditorMetadata panel collapsed by default.
    if componentID == GTEngine.Components.EditorMetadata then
        element:Collapse();
    end

    
    return element;
end



function GTGUI.Element:CollisionShapesPanel()
    self.Component = nil;

    self.CollisionShapesContainer      = GTGUI.Server.CreateElement(self);
    self.CollisionShapePanels          = {};
    
    self.NewCollisionShapeDropDownBox  = GTGUI.Server.CreateElement(self, "picking-dropdown-box");
    self.NewCollisionShapeDropDownBox:PickingDropDownBox("Add Shape");
    self.NewCollisionShapeDropDownBox:SetStyle("margin-top", "4px");
    
    self.NewCollisionShapeDropDownBox:AppendItem("Box"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddBoxCollisionShape(0.5, 0.5, 0.5);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Sphere (Does not scale. Try Ellipsoid.)"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddSphereCollisionShape(1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Ellipsoid"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddEllipsoidCollisionShape(1.0, 1.0, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Cylinder X"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCylinderXCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Cylinder Y"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCylinderYCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Cylinder Z"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCylinderZCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Capsule X"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCapsuleXCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Capsule Y"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCapsuleYCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Capsule Z"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddCapsuleZCollisionShape(0.5, 1.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    self.NewCollisionShapeDropDownBox:AppendItem("Model Convex Hulls"):OnPressed(function()
        if self.Component ~= nil then
            self.Component:AddModelConvexHullsCollisionShape(0.0);
            self:Update(self.Component);
            
            self:OnShapesChanged();
        end
    end)
    
    
    

    function self:Update(component)
        self.Component = component;
        
        -- We need to clear the panels.
        self:Clear();
        
        -- Now we need to iterate over and create the panels for each shape.
        local shapeCount = self.Component:GetCollisionShapeCount();
        if shapeCount > 0 then
            for i=1,shapeCount do
                local shape = self.Component:GetCollisionShapeAtIndex(i);
                if shape then
                    local panel = GTGUI.Server.CreateElement(self.CollisionShapesContainer, "collision-shape-panel");

                    if     shape.type == GTEngine.CollisionShapeTypes.Box              then
                        panel:CollisionShapePanel_Box():Update(shape);
                        panel:OnExtentsChanged(function(data)
                            self.Component:SetBoxCollisionShapeHalfExtents(i, data.x * 0.5, data.y * 0.5, data.z * 0.5);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.Sphere           then
                        panel:CollisionShapePanel_Sphere():Update(shape);
                        panel:OnRadiusChanged(function(data)
                            self.Component:SetSphereCollisionShapeRadius(i, data.radius);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.Ellipsoid        then
                        panel:CollisionShapePanel_Ellipsoid():Update(shape);
                        panel:OnRadiusChanged(function(data)
                            self.Component:SetEllipsoidCollisionShapeRadius(i, data.x, data.y, data.z);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CylinderX        then
                        panel:CollisionShapePanel_CylinderX():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCylinderCollisionShapeSize(i, data.radius, data.length);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CylinderY        then
                        panel:CollisionShapePanel_CylinderY():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCylinderCollisionShapeSize(i, data.radius, data.length);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CylinderZ        then
                        panel:CollisionShapePanel_CylinderZ():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCylinderCollisionShapeSize(i, data.radius, data.length);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CapsuleX         then
                        panel:CollisionShapePanel_CapsuleX():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCapsuleCollisionShapeSize(i, data.radius, data.height);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CapsuleY         then
                        panel:CollisionShapePanel_CapsuleY():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCapsuleCollisionShapeSize(i, data.radius, data.height);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.CapsuleZ         then
                        panel:CollisionShapePanel_CapsuleZ():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetCapsuleCollisionShapeSize(i, data.radius, data.height);
                            self:OnShapesChanged();
                        end);
                    elseif shape.type == GTEngine.CollisionShapeTypes.ConvexHull       then
                        panel:CollisionShapePanel_ConvexHull():Update(shape);
                    elseif shape.type == GTEngine.CollisionShapeTypes.ModelConvexHulls then
                        panel:CollisionShapePanel_ModelConvexHulls():Update(shape);
                        panel:OnSizeChanged(function(data)
                            self.Component:SetModelConvexHullsMargins(i, data.margin);
                            self:OnShapesChanged();
                        end);
                    end
                
                    panel:OnDelete(function()
                        self.Component:RemoveCollisionShapeAtIndex(i);
                        self:Update(self.Component);
                        self:OnShapesChanged();
                    end);
                    
                    panel:OnOffsetChanged(function(data)
                        if not self.IsUpdating then
                            self.Component:SetCollisionShapeOffset(i, data.x, data.y, data.z);
                            self:OnShapesChanged();
                        end
                    end);
            
                    self.CollisionShapePanels[#self.CollisionShapePanels + 1] = panel;
                end
            end
        end
        
        -- Add the bottom border for the last collision shape.
        if #self.CollisionShapePanels > 0 then
            self.CollisionShapePanels[#self.CollisionShapePanels]:SetStyle("border-bottom", "1px #2a2a2a");
        end
    end
    
    
    function self:Clear()
        for i,value in ipairs(self.CollisionShapePanels) do
            GTGUI.Server.DeleteElement(value);
        end
        
        self.CollisionShapePanels = {};
    end
    
    
    
    -- Event called when the shapes are changed in some way. The editor needs to know about this so it can setup undo/redo points.
    function self:OnShapesChanged(arg1)
        self.Callbacks:BindOrCall("OnShapesChanged", arg1);
    end
end




function LinkSceneEditorToSystemAPI(sceneEditor)

    -- Helper for retrieving a scene node based on the type of input.
    --
    -- 'sceneNode' can be a table (assuming a SceneNode table), pointer (assuming a SceneNode C++ pointer) or an number (assuming scene node ID).
    function sceneEditor:GetSceneNodePtr(sceneNode)
        if     type(sceneNode) == 'table'    then
            return sceneNode._internalPtr;
        elseif type(sceneNode) == 'userdata' then
            return sceneNode;
        elseif type(sceneNode) == 'number'   then
            return GTEngine.System.SceneEditor.GetSceneNodePtrByID(self._internalPtr, sceneNode);
        end
        
        return nil;
    end



    function sceneEditor:GetScenePtr()
        return GTEngine.System.SceneEditor.GetScenePtr();
    end
    
    function sceneEditor:DeselectAll()
        GTEngine.System.SceneEditor.DeselectAll(self._internalPtr);
    end
    
    function sceneEditor:SelectSceneNode(nodeToSelect, dontPostBackNotification)
        local sceneNodeToSelectPtr = self:GetSceneNodePtr(nodeToSelect);
        
        if sceneNodeToSelectPtr ~= nil then
            GTEngine.System.SceneEditor.SelectSceneNode(self._internalPtr, sceneNodeToSelectPtr, dontPostBackNotification);
        end
    end
    
    function sceneEditor:DeselectSceneNode(nodeToDeselect, dontPostBackNotification)
        local sceneNodeToDeselectPtr = self:GetSceneNodePtr(nodeToDeselect);
    
        if sceneNodeToDeselectPtr ~= nil then
            GTEngine.System.SceneEditor.DeselectSceneNode(self._internalPtr, sceneNodeToDeselectPtr, dontPostBackNotification);
        end
    end
    
    function sceneEditor:GetSelectedSceneNodeCount()
        return GTEngine.System.SceneEditor.GetSelectedSceneNodeCount(self._internalPtr);
    end
    
    function sceneEditor:GetFirstSelectedSceneNodePtr()
        return GTEngine.System.SceneEditor.GetFirstSelectedSceneNodePtr(self._internalPtr);
    end
    
    function sceneEditor:GetSelectedSceneNodeIDs()
        return GTEngine.System.SceneEditor.GetSelectedSceneNodeIDs(self._internalPtr);
    end
    
    function sceneEditor:IsSceneNodeSelectedByID(sceneNodeID)
        return GTEngine.System.SceneEditor.IsSceneNodeSelectedByID(self._internalPtr, sceneNodeID);
    end
    
    
    function sceneEditor:TryGizmoMouseSelect()
        return GTEngine.System.SceneEditor.TryGizmoMouseSelect(self._internalPtr);
    end
    
    function sceneEditor:DoMouseSelection()
        GTEngine.System.SceneEditor.DoMouseSelection(self._internalPtr);
    end
    
    
    function sceneEditor:RemoveSelectedSceneNodes()
        GTEngine.System.SceneEditor.RemoveSelectedSceneNodes(self._internalPtr);
    end
    
    function sceneEditor:RemoveSceneNode(sceneNodeToRemove)
        local sceneNodeToRemovePtr = self:GetSceneNodePtr(sceneNodeToRemove);
        
        if sceneNodeToRemovePtr ~= nil then
            GTEngine.System.SceneEditor.RemoveSceneNode(self._internalPtr, sceneNodeToRemovePtr);
        end
    end
    
    
    function sceneEditor:DuplicateSelectedSceneNodes()
        GTEngine.System.SceneEditor.DuplicateSelectedSceneNodes(self._internalPtr);
    end
    
    function sceneEditor:DuplicateSceneNode(sceneNodeToDuplicate)
        local sceneNodeToDuplicatePtr = self:GetSceneNodePtr(sceneNodeToDuplicate);
    
        if sceneNodeToDuplicatePtr ~= nil then
            GTEngine.System.SceneEditor.DuplicateSceneNode(self._internalPtr, sceneNodeToDuplicatePtr);
        end
    end
    
    
    function sceneEditor:SwitchGizmoToTranslateMode()
        GTEngine.System.SceneEditor.SwitchGizmoToTranslateMode(self._internalPtr);
    end
    
    function sceneEditor:SwitchGizmoToRotateMode()
        GTEngine.System.SceneEditor.SwitchGizmoToRotateMode(self._internalPtr);
    end
    
    function sceneEditor:SwitchGizmoToScaleMode()
        GTEngine.System.SceneEditor.SwitchGizmoToScaleMode(self._internalPtr);
    end
    
    function sceneEditor:SwitchGizmoToLocalSpace()
        GTEngine.System.SceneEditor.SwitchGizmoToLocalSpace(self._internalPtr);
    end
    
    function sceneEditor:SwitchGizmoToGlobalSpace()
        GTEngine.System.SceneEditor.SwitchGizmoToGlobalSpace(self._internalPtr);
    end
    
    function sceneEditor:ToggleGizmoSpace()
        GTEngine.System.SceneEditor.ToggleGizmoSpace(self._internalPtr);
    end
    
    function sceneEditor:IsGizmoInLocalSpace()
        return GTEngine.System.SceneEditor.IsGizmoInLocalSpace(self._internalPtr);
    end
    
    function sceneEditor:IsGizmoInGlobalSpace()
        return GTEngine.System.SceneEditor.IsGizmoInGlobalSpace(self._internalPtr);
    end
    
    function sceneEditor:UpdateSelectionGizmoTransform()
        return GTEngine.System.SceneEditor.UpdateSelectionGizmoTransform(self._internalPtr);
    end
    
    function sceneEditor:SetTranslationSnapSize(snapSize)
        return GTEngine.System.SceneEditor.SetTranslationSnapSize(self._internalPtr, snapSize);
    end
    
    function sceneEditor:GetTranslationSnapSize(snapSize)
        return GTEngine.System.SceneEditor.GetTranslationSnapSize(self._internalPtr);
    end
    
    
    
    function sceneEditor:StartPlaying()
        GTEngine.System.SceneEditor.StartPlaying(self._internalPtr);
    end
    
    function sceneEditor:PausePlaying()
        GTEngine.System.SceneEditor.PausePlaying(self._internalPtr);
    end
    
    function sceneEditor:StopPlaying()
        GTEngine.System.SceneEditor.StopPlaying(self._internalPtr);
    end
    
    function sceneEditor:IsPlaying()
        return GTEngine.System.SceneEditor.IsPlaying(self._internalPtr);
    end
    
    function sceneEditor:IsPaused()
        return GTEngine.System.SceneEditor.IsPaused(self._internalPtr);
    end
    
    function sceneEditor:IsStopped()
        return GTEngine.System.SceneEditor.IsStopped(self._internalPtr);
    end
    
    
    function sceneEditor:EnablePhysicsSimulation()
        GTEngine.System.SceneEditor.EnablePhysicsSimulation(self._internalPtr);
    end
    
    function sceneEditor:DisablePhysicsSimulation()
        GTEngine.System.SceneEditor.DisablePhysicsSimulation(self._internalPtr);
    end
    
    function sceneEditor:IsPhysicsSimulationEnabled()
        return GTEngine.System.SceneEditor.IsPhysicsSimulationEnabled(self._internalPtr);
    end
    
    
    function sceneEditor:CommitStateStackFrame()
        GTEngine.System.SceneEditor.CommitStateStackFrame(self._internalPtr);
    end
    
    function sceneEditor:ClearStateStackStagingArea()
        GTEngine.System.SceneEditor.ClearStateStackStagingArea(self._internalPtr);
    end
    
    function sceneEditor:PushUndoRedoPoint()
        sceneEditor:CommitStateStackFrame();
    end
    
    function sceneEditor:ClearUndoRedoStagingArea()
        sceneEditor:ClearStateStackStagingArea();
    end
    
    function sceneEditor:Undo()
        GTEngine.System.SceneEditor.Undo(self._internalPtr);
    end
    
    function sceneEditor:Redo()
        GTEngine.System.SceneEditor.Redo(self._internalPtr);
    end
    
    
    function sceneEditor:CreatePrefab(path, makeRelativeTo, sceneNode)
        return GTEngine.System.SceneEditor.CreatePrefab(self._internalPtr, path, makeRelativeTo, self:GetSceneNodePtr(sceneNode));
    end
    
    function sceneEditor:UpdatePrefab(path, makeRelativeTo, sceneNode)
        return sceneEditor:CreatePrefab(path, makeRelativeTo, sceneNode);
    end
    
    function sceneEditor:InstantiatePrefab(relativePath)
        local rootSceneNodePtr = GTEngine.System.SceneEditor.InstantiatePrefab(self._internalPtr, relativePath);
        
        GTEngine.System.SceneEditor.PositionSceneNodeInFrontOfCamera(self._internalPtr, rootSceneNodePtr);
        
        return rootSceneNodePtr;
    end
    
    function sceneEditor:LinkSceneNodeToPrefab(sceneNode, prefabRelativePath, isSourceSceneNode)
        return GTEngine.System.SceneEditor.LinkSceneNodeToPrefab(self._internalPtr, sceneEditor:GetSceneNodePtr(sceneNode), prefabRelativePath, isSourceSceneNode);
    end
    
    function sceneEditor:UnlinkSceneNodeFromPrefab(sceneNode)
        return GTEngine.System.SceneEditor.UnlinkSceneNodeFromPrefab(self._internalPtr, sceneEditor:GetSceneNodePtr(sceneNode));
    end
    
    
    function sceneEditor:GetSceneNodePtrByID(sceneNodeID)
        return GTEngine.System.SceneEditor.GetSceneNodePtrByID(self._internalPtr, sceneNodeID);
    end
    
    function sceneEditor:GetSceneNodes()
        return GTEngine.System.SceneEditor.GetSceneNodes(self._internalPtr);
    end
    
    
    function sceneEditor:PositionSceneNodeInFrontOfCamera(sceneNode)
        local sceneNodePtr = self:GetSceneNodePtr(sceneNode);
    
        if sceneNodePtr ~= nil then
            GTEngine.System.SceneEditor.PositionSceneNodeInFrontOfCamera(self._internalPtr, sceneNodePtr);
        end
    end
    
    
    function sceneEditor:ShowGrid()
        GTEngine.System.SceneEditor.ShowGrid(self._internalPtr);
    end
    
    function sceneEditor:HideGrid()
        GTEngine.System.SceneEditor.HideGrid(self._internalPtr);
    end
    
    function sceneEditor:IsShowingGrid()
        return GTEngine.System.SceneEditor.IsShowingGrid(self._internalPtr);
    end
    
    
    function sceneEditor:ShowAxisArrows()
        GTEngine.System.SceneEditor.ShowAxisArrows(self._internalPtr);
    end
    
    function sceneEditor:HideAxisArrows()
        GTEngine.System.SceneEditor.HideAxisArrows(self._internalPtr);
    end
    
    function sceneEditor:IsShowingAxisArrows()
        return GTEngine.System.SceneEditor.IsShowingAxisArrows(self._internalPtr);
    end
    
    
    function sceneEditor:GetViewportCameraSceneNodePtr()
        return GTEngine.System.SceneEditor.GetViewportCameraSceneNodePtr(self._internalPtr);
    end
    
    
    
    function sceneEditor:SetSceneName(name)
        GTEngine.System.SceneEditor.SetSceneName(self._internalPtr, name);
    end
    
    function sceneEditor:GetSceneName()
        return GTEngine.System.SceneEditor.GetSceneName(self._internalPtr);
    end
    
    
    function sceneEditor:EnableSceneBackgroundClearing(colour)
        GTEngine.System.SceneEditor.EnableSceneBackgroundClearing(self._internalPtr, colour);
    end
    
    function sceneEditor:DisableSceneBackgroundClearing()
        GTEngine.System.SceneEditor.DisableSceneBackgroundClearing(self._internalPtr);
    end
    
    function sceneEditor:IsSceneBackgroundClearingEnabled()
        return GTEngine.System.SceneEditor.IsSceneBackgroundClearingEnabled(self._internalPtr);
    end
    
    function sceneEditor:GetSceneBackgroundClearColour()
        return GTEngine.System.SceneEditor.GetSceneBackgroundClearColour(self._internalPtr);
    end
    
    
    function sceneEditor:EnableSceneHDR(colour)
        GTEngine.System.SceneEditor.EnableSceneHDR(self._internalPtr, colour);
    end
    
    function sceneEditor:DisableSceneHDR(colour)
        GTEngine.System.SceneEditor.DisableSceneHDR(self._internalPtr, colour);
    end
    
    function sceneEditor:IsSceneHDREnabled()
        return GTEngine.System.SceneEditor.IsSceneHDREnabled(self._internalPtr);
    end
    
    
    function sceneEditor:EnableSceneBloom(colour)
        GTEngine.System.SceneEditor.EnableSceneBloom(self._internalPtr, colour);
    end
    
    function sceneEditor:DisableSceneBloom(colour)
        GTEngine.System.SceneEditor.DisableSceneBloom(self._internalPtr, colour);
    end
    
    function sceneEditor:IsSceneBloomEnabled()
        return GTEngine.System.SceneEditor.IsSceneBloomEnabled(self._internalPtr);
    end
    
    
    function sceneEditor:ShowNavigationMesh(index)
        GTEngine.System.SceneEditor.ShowNavigationMesh(self._internalPtr, index);
    end
    
    function sceneEditor:HideNavigationMesh(index)
        GTEngine.System.SceneEditor.HideNavigationMesh(self._internalPtr, index);
    end
    
    function sceneEditor:PauseSceneUpdates()
        GTEngine.System.SceneEditor.PauseSceneUpdates(self._internalPtr);
    end
    
    function sceneEditor:ResumeSceneUpdates()
        GTEngine.System.SceneEditor.ResumeSceneUpdates(self._internalPtr);
    end
    
    
    function sceneEditor:SetSceneNodeTransformToCamera(sceneNode)
        return GTEngine.System.SceneEditor.SetSceneNodeTransformToCamera(self._internalPtr, sceneNode._internalPtr);
    end
end


function GTGUI.Element:SceneEditor(_internalPtr)
    self:SubEditor(_internalPtr);
    LinkSceneEditorToSystemAPI(self);
    
    self.Scene             = GTEngine.RegisteredScenes[GTEngine.System.SceneEditor.GetScenePtr(_internalPtr)];
    self.SelectedSceneNode = nil;
    
    self.Viewport        = GTGUI.Server.CreateElement(self, "scene-editor-viewport");
    self.Viewport:SceneEditorViewport(self);
    self.Viewport:SetCameraSceneNodePtr(self:GetViewportCameraSceneNodePtr());
    
    self.Panel           = GTGUI.Server.CreateElement(self, "scene-editor-panel");
    self.Panel:SceneEditorPanel(self);
    self.PropertiesPanel      = self.Panel.PropertiesPanel;
    self.HierarchyPanel       = self.Panel.HierarchyPanel;
    self.ScenePropertiesPanel = self.Panel.ScenePropertiesPanel;
    
    self.ToolBar:SceneEditorToolBar(self);
    
    self.ContextMenu     = GTGUI.Server.CreateElement(nil, "scene-editor-context-menu");
    self.ContextMenu:SceneEditorContextMenu(self);
    

    

    
    -- Updates the playback control buttons.
    function self:UpdatePlaybackControls()
        self.ToolBar:UpdatePlaybackControls();
    end

    
    
    function self:GetSelectedSceneNode()
        return self.SelectedSceneNode;
    end
    
    function self:SetSelectedSceneNode(sceneNodeInternalPtr)
        if sceneNodeInternalPtr ~= nil then
            self.SelectedSceneNode = self.Scene:GetSceneNodeByID(GTEngine.System.SceneNode.GetID(sceneNodeInternalPtr));
        else
            self.SelectedSceneNode = nil;
        end
    end
    
    function self:DeleteSelectedSceneNodes()
        self:RemoveSelectedSceneNodes();
    end
    
    function self:DeleteSceneNode(sceneNode)
        self:RemoveSceneNode(sceneNode);
        self:CommitStateStackFrame();
    end
    
    
    function self:OrphanSceneNode(sceneNode)
        local sceneNodePtr = self:GetSceneNodePtr(sceneNode);
    
        if sceneNodePtr ~= nil then
            GTEngine.System.SceneNode.Orphan(sceneNodePtr, true);
        end
    end
    
    
    function self:GetParentSceneNodePtr(sceneNode)
        local sceneNodePtr = self:GetSceneNodePtr(sceneNode);
    
        if sceneNodePtr ~= nil then
            return GTEngine.System.SceneNode.GetParentPtr(sceneNodePtr);
        end
    end
    
    function self:GetParentSceneNodeID(sceneNode)
        return GTEngine.System.SceneNode.GetID(self:GetParentSceneNodePtr(sceneNode));
    end

    
    
    function self:HidePropertyPanels(message)
        self.PropertiesPanel:HidePanels(message);
    end
    
    function self:ShowPropertyPanels()
        self.PropertiesPanel:ShowPanels();
    end
    
    function self:UpdatePropertyPanels()
        self.PropertiesPanel:Update(self:GetSelectedSceneNode());
    end
    
    function self:UpdatePropertiesTransformPanel()
        self.PropertiesPanel:UpdateTransformPanel();
    end
    
    function self:UpdateScriptProperties()
        self.PropertiesPanel:UpdateScriptProperties();
    end

    
    function self:UpdateViewportMenu()
        self.ToolBar:UpdateViewportMenu();
    end
    
    
    function self:RefreshSceneProperties()
        self.ScenePropertiesPanel:Refresh();
    end
    
    
    
    function self:EnableViewportControls()
        self.Viewport:EnableControls();
    end
    
    function self:DisableViewportControls()
        self.Viewport:DisableControls();
    end
    
    
    function self:EnableViewportCameraControls()
        self.Viewport:EnableCameraControls();
    end
    
    function self:DisableViewportCameraControls()
        self.Viewport:DisableCameraControls();
    end
    
    
    function self:ResetCamera()
        local cameraSceneNodePtr = self:GetViewportCameraSceneNodePtr();
        if cameraSceneNodePtr then
            GTEngine.System.SceneNode.SetPosition(cameraSceneNodePtr, math.vec3(32, 20, 32));
            GTEngine.System.SceneNode.LookAt(cameraSceneNodePtr, math.vec3(0, 0, 0));
            
            local eulerRotation = GTEngine.System.SceneNode.GetWorldEulerRotation(cameraSceneNodePtr);
            self:SetViewportCameraRotation(eulerRotation.x, eulerRotation.y);
        end
    end
    
    function self:SetViewportCameraRotation(rotationX, rotationY)
        self.Viewport:SetCameraRotation(rotationX, rotationY);
        self.Viewport:OnCameraTransformed();
    end
    
    function self:GetViewportCameraRotation()
        return self.Viewport:GetCameraRotation();
    end
    
    
    function self:SetSceneWalkableHeight(value)
        self.Scene:SetWalkableHeight(value);
        self:MarkAsModified();
    end
    
    function self:SetSceneWalkableRadius(value)
        self.Scene:SetWalkableRadius(value);
        self:MarkAsModified();
    end
    
    function self:SetSceneWalkableSlopeAngle(value)
        self.Scene:SetWalkableSlopeAngle(value);
        self:MarkAsModified();
    end
    
    function self:SetSceneWalkableClimbHeight(value)
        self.Scene:SetWalkableClimbHeight(value);
        self:MarkAsModified();
    end
    
    function self:GetSceneWalkableHeight()
        return self.Scene:GetWalkableHeight();
    end
    
    function self:GetSceneWalkableRadius()
        return self.Scene:GetWalkableRadius();
    end
    
    function self:GetSceneWalkableSlopeAngle()
        return self.Scene:GetWalkableSlopeAngle();
    end
    
    function self:GetSceneWalkableClimbHeight()
        return self.Scene:GetWalkableClimbHeight();
    end
    
    function self:BuildNavigationMesh(index)
        self.Scene:BuildNavigationMesh(index);
        self:MarkAsModified();
    end
    
    
    function self:OnSelectionChanged()
        local selectedNodeCount = self:GetSelectedSceneNodeCount();
        
        if selectedNodeCount > 1 then        -- Multiple selections
            self:SetSelectedSceneNode(nil);
            self:HidePropertyPanels("Multiple Objects Selected");
        elseif selectedNodeCount == 0 then   -- Nothing selected
            self:SetSelectedSceneNode(nil);
            self:HidePropertyPanels("Nothing Selected");
        else                                 -- Single selection
            self:SetSelectedSceneNode(GTEngine.System.SceneEditor.GetFirstSelectedSceneNodePtr(self._internalPtr));
            self:UpdatePropertyPanels();
            self:ShowPropertyPanels();
        end
        
        
        self.HierarchyPanel:SetSelectedItemsBySceneNodeIDs(self:GetSelectedSceneNodeIDs(), true);
    end
    
    function self:OnSceneNodeAdded(sceneNodePtr)
        self.HierarchyPanel:AddSceneNode(sceneNodePtr);
    end
    
    function self:OnSceneNodeRemoved(sceneNodePtr)
        self.HierarchyPanel:RemoveSceneNode(sceneNodePtr);
    end
    
    
    function self:OnSceneNodeChanged(sceneNodePtr)
        self:CommitStateStackFrame();
    end
    
    function self:OnSceneNodeNameChanged(sceneNodePtr)
        self.HierarchyPanel:UpdateSceneNode(sceneNodePtr);
    end
    
    function self:OnSceneNodeParentChanged(sceneNodePtr)
        self.HierarchyPanel:RemoveSceneNode(sceneNodePtr);
        self.HierarchyPanel:AddSceneNode(sceneNodePtr);
    end
    
    function self:OnSceneNodePrefabChanged(sceneNodePtr)
        self.HierarchyPanel:UpdateSceneNode(sceneNodePtr);
        
        if self.PropertiesPanel.CurrentSceneNode and self.PropertiesPanel.CurrentSceneNode._internalPtr == sceneNodePtr then
            self.PropertiesPanel:UpdateDetailsPanel();
        end
    end
    
    

    self.Viewport:OnDrop(function(data)
        if data.droppedElement.isAsset then
            if GTEngine.IsModelFile(data.droppedElement.path) then
                local newNode = self.Scene:CreateNewSceneNode();
                newNode:SetName("Model");
                newNode:AddComponent(GTEngine.Components.EditorMetadata);
                newNode:AddComponent(GTEngine.Components.Model):SetModel(data.droppedElement.path);
                
                self:PositionSceneNodeInFrontOfCamera(newNode);
                
                self:DeselectAll();
                self:SelectSceneNode(newNode);
                
                self:CommitStateStackFrame();
            elseif GTEngine.IsPrefabFile(data.droppedElement.path) then
                local newNodePtr = self:InstantiatePrefab(data.droppedElement.path);
                
                self:DeselectAll();
                GTEngine.System.SceneEditor.SelectSceneNode(self._internalPtr, newNodePtr, false);
                
                self:CommitStateStackFrame();
            end
        end
    end);
    
    

    self.Viewport:OnLMBDown(function()
        self:Focus();
    end);
    
    self.Viewport:OnRMBDown(function()
        self:Focus();
    end);
    
    self.Viewport:OnMMBDown(function()
        self:Focus();
    end);


    self:WatchLMBDown(function(data)
        if not self.ContextMenu:IsChild(data.receiver) then self.ContextMenu:Hide() end
    end);
    
    self:WatchRMBDown(function(data)
        if not self.ContextMenu:IsChild(data.receiver) then self.ContextMenu:Hide() end
    end);
    
    self:WatchMMBDown(function(data)
        if not self.ContextMenu:IsChild(data.receiver) then self.ContextMenu:Hide() end
    end);
    
    
    self:WatchKeyPressed(function(data)
        if self:IsVisible() then
            if data.key == GTCore.Keys.Pause then
                if self:IsPaused() or not self:IsPlaying() then
                    self:StartPlaying();
                else
                    self:PausePlaying();
                end
            elseif data.key == GTCore.Keys.Break then
                self:StopPlaying();
            end
        
            if not GTGUI.Server.DoesFocusedElementHaveEditableText() and not self:IsPlaying() then
                if not GTGUI.Server.IsCTRLKeyDown() then
                    if data.key == GTCore.Keys.Delete then
                        self:DeleteSelectedSceneNodes();
                    elseif data.key == GTCore.Keys.T then
                        self:SwitchGizmoToTranslateMode();
                    elseif data.key == GTCore.Keys.R then
                        self:SwitchGizmoToRotateMode();
                    elseif data.key == GTCore.Keys.S then
                        self:SwitchGizmoToScaleMode();
                    elseif data.key == GTCore.Keys.L then
                        self:SwitchGizmoToLocalSpace();
                    elseif data.key == GTCore.Keys.G then
                        self:SwitchGizmoToGlobalSpace();
                    elseif data.key == GTCore.Keys.Q then
                        self:ToggleGizmoSpace();
                    end
                else
                    if data.key == GTCore.Keys.D then
                        self:DuplicateSelectedSceneNodes();           -- This will deselect the source nodes and select the new ones.
                    elseif data.key == GTCore.Keys.Z then
                        self:Undo();
                    elseif data.key == GTCore.Keys.Y then
                        self:Redo();
                    end
                end
            end
        end
    end);
    
    
    -- We'll reset the camera to give it it's initial transform.
    self:ResetCamera();
    
    -- We're going to hide the panels by default since nothing is selected right now.
    self:HidePropertyPanels("Nothing Selected");
    
    return self;
end















