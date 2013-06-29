-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:TextEditor(_internalPtr)
    self:SubEditor(_internalPtr);

    self.TextBox = GTGUI.Server.CreateElement(self, "text-editor-textbox");
    self.TextBox:MultiLineTextBox();
    
    self.Panel = GTGUI.Server.CreateElement(self, "text-editor-panel");
    self.Panel:TextEditorPanel(self);
    
    
    function self:OnErrorSelected(lineNumber, message)
        self.TextBox:GoToLine(lineNumber);
    end
    
    
    return self;
end