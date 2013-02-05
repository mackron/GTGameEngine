-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- Enumerator representing the result from the Yes/No/Cancel dialog.
Editor.YesNoCancelDialogResult = 
{
    No     = 0,
    Yes    = 1,
    Cancel = 2,
}

-- Enumerator representing the result from the New File dialog.
Editor.NewFileDialogResult =
{
    Create = 1,
    Cancel = 2
}



-- Keeps track of whether or not the Yes/No/Cancel dialog has been initialised.
Editor.AreDialogsInitialised = false;

-- Initialises the dialog boxes, if needed.
function Editor.InitialiseDialogs()
    if Editor.AreDialogsInitialised == false then
        -- Yes/No/Cancel
        YesNoCancelDialog_Yes:Button("Yes"):OnPressed(function()
            YesNoCancelDialog:Hide();
            
            if YesNoCancelDialog_Yes.Callback ~= nil then
                YesNoCancelDialog_Yes.Callback(Editor.YesNoCancelDialogResult.Yes);
            end
        end);
        
        YesNoCancelDialog_No:Button("No"):OnPressed(function()
            YesNoCancelDialog:Hide();
            
            if YesNoCancelDialog_No.Callback ~= nil then
                YesNoCancelDialog_No.Callback(Editor.YesNoCancelDialogResult.No);
            end
        end);
        
        YesNoCancelDialog_Cancel:Button("Cancel"):OnPressed(function()
            YesNoCancelDialog:Hide();
            
            if YesNoCancelDialog_Cancel.Callback ~= nil then
                YesNoCancelDialog_Cancel.Callback(Editor.YesNoCancelDialogResult.Cancel);
            end
        end);
        
        
        -- New File
        function NewFileDialog.GetAbsolutePath()
            local absolutePath = NewFileDialog.DestinationDirectory .. "/" .. NewFileDialog_NameTextBox:GetText();
            if NewFileDialog.Extension then
                return absolutePath .. "." .. NewFileDialog.Extension;
            end
            
            return absolutePath;
        end
        
        NewFileDialog_NameTextBox:OnTextChanged(function()
            local text = NewFileDialog_NameTextBox:GetText();
            if text ~= nil and text ~= "" then
                if not GTCore.IO.FileExists(NewFileDialog.GetAbsolutePath()) then
                    NewFileDialog_Create:Enable();
                    NewFileDialog_StatusTextBox:SetText("");
                else
                    NewFileDialog_Create:Disable();
                    NewFileDialog_StatusTextBox:SetText("File already exists.");
                end
            else
                NewFileDialog_Create:Disable();
                NewFileDialog_StatusTextBox:SetText("");
            end
        end);
        
        NewFileDialog_NameTextBox:OnKeyPressed(function(data)
            if data.key == GTCore.Keys.Enter or data.key == GTCore.Keys.Return then
                if NewFileDialog_Create:IsEnabled() then
                    NewFileDialog_Create:OnPressed();
                end
            end
        end);
        
        
        NewFileDialog_Create:Button("Create"):OnPressed(function()
            NewFileDialog:Hide();
            
            if NewFileDialog_Create.Callback ~= nil then
                NewFileDialog_Create.Callback(Editor.NewFileDialogResult.Create, NewFileDialog.GetAbsolutePath());
            end
        end);
        
        NewFileDialog_Cancel:Button("Cancel"):OnPressed(function()
            NewFileDialog:Hide();
        
            if NewFileDialog_Cancel.Callback ~= nil then
                NewFileDialog_Cancel.Callback(Editor.NewFileDialogResult.Cancel, nil);
            end
        end);
        
        Editor.AreDialogsInitialised = true;
    end
end


-- Opens a Yes/No/Cancel dialog box.
--
-- @param text     [in] The text to show on the dialog.
-- @param callback [in] The function that will be called when the user hits one of the buttons. The argument is an integer representing the result. See remarks.
--
-- @remarks
--      'callback' will be passed one argument, which is an integer representing the value
function Editor.ShowYesNoCancelDialog(text, callback)
    Editor.InitialiseDialogs();
    
    GTGUI.Server.BlurFocusedElement();
    
    YesNoCancelDialog_TextBox:SetText(text);
    YesNoCancelDialog_Yes.Callback    = callback;
    YesNoCancelDialog_No.Callback     = callback;
    YesNoCancelDialog_Cancel.Callback = callback;
    
    YesNoCancelDialog:Show();
end


-- Opens a New File dialog box.
--
-- @param titleText      [in] The text to use for the title.
-- @param destinationDir [in] The path of the directory 
-- @param extension      [in] The extension to append for when the file needs to be checked to see if it already exists.
-- @param callback       [in] The function that will be called when the user hits one of the buttons. The argument is an integer representing the result. See remarks.
--
-- @remarks
--      'callback' will be passed two arguments - an integer representing the button that was pressed, and the absolute path of the file being created.
--      @par
--      This does not actually create the file, but it does check whether or not another file already exists.
function Editor.ShowNewFileDialog(titleText, destinationDir, extension, callback)
    Editor.InitialiseDialogs();
    
    NewFileDialog_NameTextBox:SetText("");
    NewFileDialog_NameTextBox:Focus();
    
    NewFileDialog_TextBox:SetText(titleText);
    NewFileDialog_Create.Callback = callback;
    NewFileDialog_Cancel.Callback = callback;
    
    NewFileDialog.DestinationDirectory = destinationDir;
    NewFileDialog.Extension            = extension;
    
    NewFileDialog:Show();
end





















