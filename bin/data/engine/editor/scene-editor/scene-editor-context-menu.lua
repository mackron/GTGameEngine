-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorContextMenu(sceneEditor)
    self:Menu();
    self:EnableDefaultEvents();
    
    
    
    self:AppendItem("Add Empty"):OnPressed(function()
        local newNode = sceneEditor.Scene:CreateNewSceneNode();
        newNode:SetName("Empty");
        newNode:AddComponent(GTEngine.Components.EditorMetadata);
        
        sceneEditor:PositionSceneNodeInFrontOfCamera(newNode);
        
        sceneEditor:DeselectAll();
        sceneEditor:SelectSceneNode(newNode);
        
        sceneEditor:CommitStateStackFrame();
    end);
    
    self:AppendSeparator();

    self:AppendItem("Add Cube"):OnPressed(function()
        local newNode = sceneEditor.Scene:CreateNewSceneNode();
        newNode:SetName("Cube");
        newNode:AddComponent(GTEngine.Components.EditorMetadata);
        newNode:AddComponent(GTEngine.Components.Model):SetModel("engine/models/default.dae");
        newNode:AddComponent(GTEngine.Components.Dynamics):AddBoxCollisionShape(1, 1, 1);
        
        sceneEditor:PositionSceneNodeInFrontOfCamera(newNode);
        
        sceneEditor:DeselectAll();
        sceneEditor:SelectSceneNode(newNode);
        
        sceneEditor:CommitStateStackFrame();
    end);
    
    self:AppendSeparator();
    
    self:AppendItem("Add Point Light"):OnPressed(function()
        local newNode = sceneEditor.Scene:CreateNewSceneNode();
        newNode:SetName("PointLight");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/editor/images/light-sprite.png");
        newNode:AddComponent(GTEngine.Components.PointLight);
        
        sceneEditor:PositionSceneNodeInFrontOfCamera(newNode);
        
        sceneEditor:DeselectAll();
        sceneEditor:SelectSceneNode(newNode);
        
        sceneEditor:CommitStateStackFrame();
    end);
    
    self:AppendItem("Add Spot Light"):OnPressed(function()
        local newNode = sceneEditor.Scene:CreateNewSceneNode();
        newNode:SetName("SpotLight");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/editor/images/light-sprite.png");
        newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowDirectionArrow();
        newNode:AddComponent(GTEngine.Components.SpotLight);
        
        sceneEditor:PositionSceneNodeInFrontOfCamera(newNode);
        
        sceneEditor:DeselectAll();
        sceneEditor:SelectSceneNode(newNode);
        
        sceneEditor:CommitStateStackFrame();
    end);
    
    self:AppendItem("Add Directional Light"):OnPressed(function()
        local newNode = sceneEditor.Scene:CreateNewSceneNode();
        newNode:SetName("DirectionalLight");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/editor/images/light-sprite.png");
        newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowDirectionArrow();
        newNode:AddComponent(GTEngine.Components.DirectionalLight);
        
        sceneEditor:PositionSceneNodeInFrontOfCamera(newNode);
        
        sceneEditor:DeselectAll();
        sceneEditor:SelectSceneNode(newNode);
        
        sceneEditor:CommitStateStackFrame();
    end);
    
    self:AppendItem("Add Ambient Light"):OnPressed(function()
        local newNode = sceneEditor.Scene:CreateNewSceneNode();
        newNode:SetName("AmbientLight");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/editor/images/light-sprite.png");
        newNode:AddComponent(GTEngine.Components.AmbientLight);
        
        sceneEditor:PositionSceneNodeInFrontOfCamera(newNode);
        
        sceneEditor:DeselectAll();
        sceneEditor:SelectSceneNode(newNode);
        
        sceneEditor:CommitStateStackFrame();
    end);
    
    self:AppendSeparator();
    
    self:AppendItem("Add Camera"):OnPressed(function()
        local newNode = sceneEditor.Scene:CreateNewSceneNode();
        newNode:SetName("Camera");
        newNode:AddComponent(GTEngine.Components.EditorMetadata):ShowSprite("engine/editor/images/camera-sprite.png");
        newNode:GetComponent(GTEngine.Components.EditorMetadata):ShowDirectionArrow();
        newNode:AddComponent(GTEngine.Components.Camera):Set3DProjection(90.0, 16/9, 0.1, 1000.0);
        
        sceneEditor:PositionSceneNodeInFrontOfCamera(newNode);
        
        sceneEditor:DeselectAll();
        sceneEditor:SelectSceneNode(newNode);
        
        sceneEditor:CommitStateStackFrame();
    end);

    return self;
end
