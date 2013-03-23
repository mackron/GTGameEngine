-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- An input control for simple numbers in a properties panel.
function GTGUI.Element:PanelNumberInput(title, initialValue)
    self.Left  = GTGUI.Server.New("<div parentid='" .. self:GetID()       .. "' style='width:auto; height:auto; margin-right:4px;' />");
    self.Right = GTGUI.Server.New("<div parentid='" .. self:GetID()       .. "' style='width:100%; height:auto; horizontal-align:right;' />");
    self.Label = GTGUI.Server.New("<div parentid='" .. self.Left:GetID()  .. "' style='width:auto; text-color:std-text-color; padding:0px 2px;' />");
    self.Input = GTGUI.Server.New("<div parentid='" .. self.Right:GetID() .. "' styleclass='textbox' style='width:100%; max-width:72px;' />");
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
    
    self.Label:SetText(title);
end;