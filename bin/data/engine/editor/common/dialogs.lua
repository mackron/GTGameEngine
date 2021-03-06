-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- Enumerator representing the result from the Yes/No dialog.
Editor.YesNoDialogResult =
{
    No  = 0,
    Yes = 1,
}

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


-- The dialog box components.
YesNoDialog                 = nil;
YesNoDialog_TextBox         = nil;
YesNoDialog_Yes             = nil;
YesNoDialog_No              = nil;

YesNoCancelDialog           = nil;
YesNoCancelDialog_TextBox   = nil;
YesNoCancelDialog_Yes       = nil;
YesNoCancelDialog_No        = nil;
YesNoCancelDialog_Cancel    = nil;

NewFileDialog               = nil;
NewFileDialog_NameTextBox   = nil;
NewFileDialog_TextBox       = nil;
NewFileDialog_Create        = nil;
NewFileDialog_Cancel        = nil;
NewFileDialog_StatusTextBox = nil;



-- Initialises the dialog boxes, if needed.
function Editor.InitialiseDialogs()
    if Editor.AreDialogsInitialised == false then
        YesNoDialog                 = GTGUI.Server.GetElementByID("YesNoDialog");
        YesNoDialog_TextBox         = GTGUI.Server.GetElementByID("YesNoDialog_TextBox");
        YesNoDialog_Yes             = GTGUI.Server.GetElementByID("YesNoDialog_Yes");
        YesNoDialog_No              = GTGUI.Server.GetElementByID("YesNoDialog_No");
    
        YesNoCancelDialog           = GTGUI.Server.GetElementByID("YesNoCancelDialog");
        YesNoCancelDialog_TextBox   = GTGUI.Server.GetElementByID("YesNoCancelDialog_TextBox");
        YesNoCancelDialog_Yes       = GTGUI.Server.GetElementByID("YesNoCancelDialog_Yes");
        YesNoCancelDialog_No        = GTGUI.Server.GetElementByID("YesNoCancelDialog_No");
        YesNoCancelDialog_Cancel    = GTGUI.Server.GetElementByID("YesNoCancelDialog_Cancel");
        
        NewFileDialog               = GTGUI.Server.GetElementByID("NewFileDialog");
        NewFileDialog_NameTextBox   = GTGUI.Server.GetElementByID("NewFileDialog_NameTextBox");
        NewFileDialog_TextBox       = GTGUI.Server.GetElementByID("NewFileDialog_TextBox");
        NewFileDialog_Create        = GTGUI.Server.GetElementByID("NewFileDialog_Create");
        NewFileDialog_Cancel        = GTGUI.Server.GetElementByID("NewFileDialog_Cancel");
        NewFileDialog_StatusTextBox = GTGUI.Server.GetElementByID("NewFileDialog_StatusTextBox");
        
        
        -- Yes/No
        YesNoDialog_Yes:Button("Yes"):OnPressed(function()
            YesNoDialog:Hide();
            
            if YesNoDialog_Yes.Callback ~= nil then
                YesNoDialog_Yes.Callback(Editor.YesNoDialogResult.Yes);
            end
        end);
        
        YesNoDialog_No:Button("No"):OnPressed(function()
            YesNoDialog:Hide();
            
            if YesNoDialog_No.Callback ~= nil then
                YesNoDialog_No.Callback(Editor.YesNoDialogResult.No);
            end
        end);
        
        
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
                if not GT.IO.FileExists(NewFileDialog.GetAbsolutePath()) then
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
            if data.key == GT.Keys.Enter or data.key == GT.Keys.Return then
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


-- Opens a Yes/No dialog box.
--
-- @param text     [in] The text to show on the dialog.
-- @param callback [in] The function that will be called when the user hits one of the buttons. The argument is an integer representing the result. See remarks.
--
-- @remarks
--      'callback' will be passed one argument, which is an integer representing the value
function Editor.ShowYesNoDialog(text, callback)
    Editor.InitialiseDialogs();
    
    GTGUI.Server.BlurFocusedElement();
    
    YesNoDialog_TextBox:SetText(text);
    YesNoDialog_Yes.Callback = callback;
    YesNoDialog_No.Callback  = callback;
    
    YesNoDialog:Show();
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
    
    NewFileDialog_Create:Disable();
    NewFileDialog_StatusTextBox:SetText("");
    
    NewFileDialog.DestinationDirectory = destinationDir;
    NewFileDialog.Extension            = extension;
    
    
    NewFileDialog:Show();
end





















