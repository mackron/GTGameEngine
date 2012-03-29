
function ShowNewSceneDialog()
    NewSceneDialog:Show();
	NewSceneDialog_Width:SetText("128");
	NewSceneDialog_Width:Focus();
end


function ResetOpenSceneDialog()
    -- We just empty the list...
    OpenSceneDialog_FileList:RemoveAllItems();
end

function ShowOpenSceneDialog()
    -- We're going to reset the dialog first...
    ResetOpenSceneDialog()

    -- We need to grab the list of scenes in the "scenes" folder. We then dynamically create the appropriate
    -- elements for each scene. Each scene is a table containing information about that secne.
    OpenSceneDialog.Scenes = Game.Editor.GetSceneFiles();

    -- For each scene we need to create a selectable element. The key is the display name of the scene. The
    -- value is a table with additional information about the scene.
    for key,value in pairs(OpenSceneDialog.Scenes) do
        local item = OpenSceneDialog_FileList:AppendItem(key);
        item.FileName = value.FileName;
    end

    OpenSceneDialog:Show();
end

function CloseOpenSceneDialog()
    OpenSceneDialog:Hide();
end


NewSceneDialog_Cancel:OnPressed(function()
	NewSceneDialog:Hide();
end)


OpenSceneDialog_OK:OnPressed(function()
    CloseOpenSceneDialog();

    Game.Editor.OpenScene(OpenSceneDialog_FileList.SelectedItem.FileName);
end)

OpenSceneDialog_Cancel:OnPressed(function()
	CloseOpenSceneDialog();
end)