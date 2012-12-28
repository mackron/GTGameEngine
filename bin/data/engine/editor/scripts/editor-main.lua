




------------------------------------------------
-- Scene.SceneEditor

Editor.SceneEditor.CurrentScene              = nil;
Editor.SceneEditor.CurrentSceneEditorElement = nil;
Editor.SceneEditor.SelectedNodes = {};


function Editor.SceneEditor.SetCurrentScene(sceneInternalPtr, elementID)
    if sceneInternalPtr ~= nil then
        Editor.SceneEditor.CurrentScene = GTEngine.Scene:Create(sceneInternalPtr);
    else
        Editor.SceneEditor.CurrentScene = nil;
    end
    
    Editor.SceneEditor.CurrentSceneEditorElement = GTGUI.Server.GetElementByID(elementID);
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
    Editor.SceneEditor.UpdatePanel();
end

function Editor.SceneEditor.MarkNodeAsDeselected(internalPtr)
    Editor.SceneEditor.SelectedNodes[internalPtr] = nil;
    Editor.SceneEditor.UpdatePanel();
end

function Editor.SceneEditor.MarkAllNodesAsDeselected()
    Editor.SceneEditor.SelectedNodes = {};
    Editor.SceneEditor.UpdatePanel();
end

function Editor.SceneEditor.GetSelectedNodeCount()
    local count = 0;
    local firstSelectedNode = nil;
    
    for key,value in pairs(Editor.SceneEditor.SelectedNodes) do
        count = count + 1;
        
        if firstSelectedNode == nil then
            firstSelectedNode = value;
        end
    end
    
    return count, firstSelectedNode;
end

function Editor.SceneEditor.UpdatePanel()
    local numSelectedNodes, firstSelectedNode = Editor.SceneEditor.GetSelectedNodeCount();
    
    if numSelectedNodes > 1 then        -- Multiple selections
        Editor.SceneEditor.CurrentSceneEditorElement:HidePanels("Multiple Objects Selected");
    elseif numSelectedNodes == 0 then   -- Nothing selected
        Editor.SceneEditor.CurrentSceneEditorElement:HidePanels("Nothing Selected");
    else                                -- Single selection
        Editor.SceneEditor.CurrentSceneEditorElement:ShowPanels(firstSelectedNode);
    end
end





















