-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:LabelledTextBox(title, initialValue)
    self:LabelledElement(title);
    
    self.TextBox = GTGUI.Server.CreateElement(self.Right, "labelled-textbox-textbox");
    self.TextBox:SetText(initialValue);
    

    function self:GetText()
        return self.TextBox:GetText();
    end
    
    function self:SetText(text)
        self.TextBox:SetText(text);
    end
    
    
    self.TextBox:OnTextChanged(function(data)
        self:OnTextChanged(data);
    end);
    
    return self;
end