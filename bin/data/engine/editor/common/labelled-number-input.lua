-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- An input control for simple numbers in a properties panel.
function GTGUI.Element:LabelledNumberInput(title, initialValue)
    self:LabelledElement(title);
    
    self.Input = GTGUI.Server.CreateElement(self.Right, "labelled-number-input-textbox");
    self.Input:NumberTextBox(initialValue);
    
    -- Sets the value of the input.
    --
    -- An important note here is that OnValueChanged() will NOT be called.
    function self:SetValue(value, force)
        self.Input:SetValue(value, force);
    end
    
    -- Retrieves the value of the input.
    function self:GetValue()
        return self.Input:GetValue();
    end
    
    
    function self:UseFloatFormat()
        self.Input:UseFloatFormat();
    end
    
    function self:UseDefaultFormat()
        self.Input:UseDefaultFormat();
    end
    
    
    -- Called when the text of the input control has been changed. We need to call OnValueChanged when this occurs.
    --
    -- Note that this event will not be posted when the text is changed via SetValue().
    self.Input:OnTextChanged(function(data)
        self:OnValueChanged({value = self:GetValue()});
    end);
    
    
    function self:OnValueChanged(arg1)
        self.Locked = true;
            self.Callbacks:BindOrCall("OnValueChanged", arg1);
        self.Locked = false;
    end
    
    return self;
end;