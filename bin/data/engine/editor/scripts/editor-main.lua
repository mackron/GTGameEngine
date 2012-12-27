




------------------------------------------------
-- Scene.SceneEditor

Editor.SceneEditor.CurrentScene  = nil;
Editor.SceneEditor.SelectedNodes = {};


function Editor.SceneEditor.SetCurrentScene(sceneInternalPtr)
    if sceneInternalPtr ~= nil then
        Editor.SceneEditor.CurrentScene = GTEngine.Scene:Create(sceneInternalPtr);
    else
        Editor.SceneEditor.CurrentScene = nil;
    end
end

function Editor.SceneEditor.GetCurrentScene()
    return Editor.SceneEditor.CurrentScene;
end

function Editor.SceneEditor.AddSceneNode(name)
    local scene = Editor.SceneEditor.GetCurrentScene();
    if scene ~= nil then
        local sceneNode = GTEngine.SceneNode:Create();
        sceneNode:SetName(name);
        
        
        scene:AddSceneNode(sceneNode);
        return sceneNode;
    end
    
    return nil;
end


function Editor.SceneEditor.MarkNodeAsSelected(internalPtr)
    Editor.SceneEditor.SelectedNodes[internalPtr] = GTEngine.SceneNode:Create(internalPtr);
end

function Editor.SceneEditor.MarkNodeAsDeselected(internalPtr)
    Editor.SceneEditor.SelectedNodes[internalPtr] = nil;
end

function Editor.SceneEditor.MarkAllNodesAsDeselected()
    Editor.SceneEditor.SelectedNodes = {};
end
