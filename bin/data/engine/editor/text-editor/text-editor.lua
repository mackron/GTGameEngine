-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:TextEditor()
    self.TextBox = GTGUI.Server.CreateElement(self, "text-editor-textbox");
    self.TextBox:MultiLineTextBox();
    
    return self;
end