
-- Enumerator representing the result from the Yes/No/Cancel dialog.
Editor.YesNoCancelDialogResult = 
{
    No     = 0,
    Yes    = 1,
    Cancel = 2,
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
        
        
        Editor.AreDialogsInitialised = true;
    end
end


-- Opens a Yes/No/Cancel dialog box.
--
-- @param text     [in] The text to show on the dialog.
-- @param callback [in] The function that will be called when the use hits one of the buttons. The argument is a integer representing the result. See remarks.
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