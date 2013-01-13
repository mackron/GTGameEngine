
--[[


-- This object is used to manage all of the currently open files in the editor. Any operations such as opening, closing and saving
-- is done through this object.

Editor.OpenFilesManager = {};
Editor.OpenFilesManager.OpenFiles         = {};         -- Indexed by absolute file name.
Editor.OpenFilesManager.OpenedSubEditor   = nil;
Editor.OpenFilesManager.PreviousSubEditor = nil;

function Editor.OpenFilesManager.Startup(tabbar, modelEditor, imageEditor, textEditor, sceneEditor)
    Editor.OpenFilesManager.TabBar      = tabbar;
    Editor.OpenFilesManager.ModelEditor = modelEditor;
    Editor.OpenFilesManager.ImageEditor = imageEditor;
    Editor.OpenFilesManager.TextEditor  = textEditor;
    Editor.OpenFilesManager.SceneEditor = sceneEditor;
end


function Editor.OpenFilesManager.OpenFile(filename, relativeTo)     -- 'filename' should be absolute.
    local file = Editor.OpenFilesManager.OpenFiles[filename];
    if file == nil then
        file = {};
        file.path         = filename;
        file.relativeTo   = relativeTo;
        file.isModified   = false;
        file.tab          = Editor.OpenFilesManager.TabBar:AddTab(GTCore.IO.GetFileNameFromPath(filename));
        file.tab.path     = filename;
        
        -- This will allow us to close a file with the middle mouse button.
        file.tab:OnMMBDown(function()
            Editor.OpenFilesManager.CloseFile(file.tab.path, true);
        end);
        
        Editor.OpenFilesManager.OpenFiles[filename] = file;
    end
    
    -- The center must be visible. It will be made invisible when there are no tabs open. Also, the Close and Close All buttons
    -- need to be activated.
    EditorCenterCenterPanel:Show();
    Editor_MenuBar.File.Close:Enable();
    Editor_MenuBar.File.CloseAll:Enable();
    
    Editor.OpenFilesManager.ShowFile(filename);
end


function Editor.OpenFilesManager.CloseFile(filename, showSavePrompt)
    Editor.OpenFilesManager._CloseFile(Editor.OpenFilesManager.OpenFiles[filename], showSavePrompt);
end

function Editor.OpenFilesManager.CloseAllFiles()
    local tempTable = {};
    for key,value in pairs(Editor.OpenFilesManager.OpenFiles) do
        tempTable[#tempTable + 1] = value;
    end

    for i,value in ipairs(tempTable) do
        Editor.OpenFilesManager._CloseFile(value, false);            -- Passing 'false' here means the save prompt will not be displayed if the file is marked as modified.
    end
end

function Editor.OpenFilesManager.CloseActiveFile()
    local activeTab = Editor.OpenFilesManager.TabBar:GetActiveTab();
    if activeTab ~= nil then
        Editor.OpenFilesManager.CloseFile(activeTab.path, true);
    end
end



function Editor.OpenFilesManager.SaveFile(filename)
    if Editor.OpenFilesManager.IsFileModified(filename) then
        local result = false;
        if GTEngine.IsModelFile(filename) then
            result = Editor.ModelEditor.SaveFile(filename);
        elseif GTEngine.IsImageFile(filename) then
        elseif GTEngine.IsSceneFile(filename) then
            result = Editor.SceneEditor.SaveFile(filename);
        else                                                        -- Assume a text file if nothing else.
            result = Editor.TextEditor.SaveFile(filename);
        end
        
        if result then
            Editor.OpenFilesManager.UnmarkFileAsModified(filename);
        else
            CommandErrorBox:SetText("Failed to save '" .. filename .. "'");
        end
        
        return result;
    end
    
    return false;
end

function Editor.OpenFilesManager.SaveAllFiles()
    for key,value in pairs(Editor.OpenFilesManager.OpenFiles) do
        Editor.OpenFilesManager.SaveFile(value.path);
    end
end

function Editor.OpenFilesManager.SaveActiveFile()       -- Saves the file that's currently being viewed.
    local activeTab = Editor.OpenFilesManager.TabBar:GetActiveTab();
    if activeTab ~= nil then
        Editor.OpenFilesManager.SaveFile(activeTab.path);
    end
end



function Editor.OpenFilesManager.ShowFile(filename)
    local file = Editor.OpenFilesManager.OpenFiles[filename];
    if file ~= nil then
        if Editor.OpenFilesManager.TabBar:GetActiveTab() ~= file.tab then       -- This check isn't really needed, but I like to do it for my own sanity.
            Editor.OpenFilesManager.TabBar:ActivateTab(file.tab);
        end
        
        if GTEngine.IsModelFile(filename) then
            Editor.OpenFilesManager.OpenedSubEditor = Editor.OpenFilesManager.ModelEditor;
            Editor.OnModelActivated(filename, file.relativeTo);
        elseif GTEngine.IsImageFile(filename) then
            Editor.OpenFilesManager.OpenedSubEditor = Editor.OpenFilesManager.ImageEditor;
            Editor.OnImageActivated(filename, file.relativeTo);
        elseif GTEngine.IsSceneFile(filename) then
            Editor.OpenFilesManager.OpenedSubEditor = Editor.OpenFilesManager.SceneEditor;
            Editor.OnSceneActivated(filename, file.relativeTo);
        else                                                        -- Assume a text file if nothing else.
            Editor.OpenFilesManager.OpenedSubEditor = Editor.OpenFilesManager.TextEditor;
            Editor.OnTextFileActivated(filename, file.relativeTo);
        end
        
        -- The way we do tab switching is to only hide/show the sub editor if the new one is different to the old. The reason for this is that
        -- hiding a 3D viewport will cause a resize which will in turn cause a new framebuffer to be generated. Due to the multithreaded environment,
        -- this will cause a slight flicker.
        if Editor.OpenFilesManager.OpenedSubEditor ~= Editor.OpenFilesManager.PreviousSubEditor and Editor.OpenFilesManager.PreviousSubEditor ~= nil then
            Editor.OpenFilesManager.PreviousSubEditor:Hide();
        end
        
        if Editor.OpenFilesManager.OpenedSubEditor ~= nil then
            Editor.OpenFilesManager.OpenedSubEditor:Show();
        end
        
        
        -- This will enable/disable the Save and Save All buttons on the main menu.
        if Editor.OpenFilesManager.IsFileModified(filename) then
            Editor_MenuBar.File.Save:Enable();
        else
            Editor_MenuBar.File.Save:Disable();
        end
        
        if Editor.OpenFilesManager.IsAnyOpenFileModified() then
            Editor_MenuBar.File.SaveAll:Enable();
        else
            Editor_MenuBar.File.SaveAll:Disable();
        end
    end
end

function Editor.OpenFilesManager.HideFile()
    if Editor.OpenFilesManager.OpenedSubEditor ~= nil then
        Editor.OpenFilesManager.PreviousSubEditor = Editor.OpenFilesManager.OpenedSubEditor;
        Editor.OpenFilesManager.OpenedSubEditor   = nil;
    end
end



function Editor.OpenFilesManager.MarkFileAsModified(filename)
    local file = Editor.OpenFilesManager.OpenFiles[filename];
    if file ~= nil and not file.isModified then
        file.isModified = true;
        file.tab.text:SetText(file.tab.text:GetText() .. "*");
        
        -- This enables the Save and Save All menu items.
        Editor_MenuBar.File.Save:Enable();
        Editor_MenuBar.File.SaveAll:Enable();
    end
end

function Editor.OpenFilesManager.MarkActiveFileAsModified(filename)
    local activeTab = Editor.OpenFilesManager.TabBar:GetActiveTab();
    if activeTab ~= nil then
        Editor.OpenFilesManager.MarkFileAsModified(activeTab.path);
    end
end

function Editor.OpenFilesManager.UnmarkFileAsModified(filename)
    local file = Editor.OpenFilesManager.OpenFiles[filename];
    if file ~= nil and file.isModified then
        file.isModified = false;
        file.tab.text:SetText(string.sub(file.tab.text:GetText(), 0, -2));
        
        -- This disables the Save and Save All menu items, where appropriate.
        Editor_MenuBar.File.Save:Disable();
            
        if not Editor.OpenFilesManager.IsAnyOpenFileModified() then
            Editor_MenuBar.File.SaveAll:Disable();
        end
    end
end

function Editor.OpenFilesManager.UnmarkActiveFileAsModified(filename)
    local activeTab = Editor.OpenFilesManager.TabBar:GetActiveTab();
    if activeTab ~= nil then
        Editor.OpenFilesManager.UnmarkFileAsModified(activeTab.path);
    end
end

function Editor.OpenFilesManager.IsAnyOpenFileModified()
    for key,value in pairs(Editor.OpenFilesManager.OpenFiles) do
        if value.isModified then
            return true;
        end
    end
    
    return false;
end

function Editor.OpenFilesManager.IsFileModified(filename)
    local file = Editor.OpenFilesManager.OpenFiles[filename];
    if file ~= nil then
        return file.isModified;
    end
    
    return false;
end



------------------------------------------------------
-- Private Functions

-- Helper function for closing a file.
--
-- @param file [in] The file object from Editor.OpenFilesManager.OpenFiles that is being closed.
function Editor.OpenFilesManager._CloseFile(file, showSavePrompt)
    if file ~= nil then
        if showSavePrompt and file.isModified then
            Editor.ShowYesNoCancelDialog("Save '" .. GTCore.IO.GetFileNameFromPath(file.path) .. "'?", function(result)
                if result == Editor.YesNoCancelDialogResult.Yes then
                    Editor.OpenFilesManager.SaveFile(file.path);
                end
                
                if result ~= Editor.YesNoCancelDialogResult.Cancel then
                    Editor.OpenFilesManager._CloseFile(file, false);    -- Call this same function again, but skip the dialog (the 'false' argument). This will cause the actual close operation to be performed.
                end
            end);
        else
            Editor.OnFileClosed(file.path);                             -- This is for the C++ side.

            Editor.OpenFilesManager.TabBar:RemoveTab(file.tab);
            Editor.OpenFilesManager.OpenFiles[file.path] = nil;
            
            if Editor.OpenFilesManager.TabBar:GetActiveTab() == nil then
                EditorCenterCenterPanel:Hide();
                Editor_MenuBar.File.Close:Disable();
                Editor_MenuBar.File.CloseAll:Disable();
            end
        end
    end
end


]]




