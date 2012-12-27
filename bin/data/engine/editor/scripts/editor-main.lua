




------------------------------------------------
-- Scene.SceneEditor

Editor.SceneEditor.CurrentScene = nil;


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