-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

-- A text box element more suited for a single number.
--
-- The value can be set with SetValue(). This will never post an OnTextChanged event.
function GTGUI.Element:NumberTextBox(defaultValue)
    function self:GetValue()
        return tonumber(self:GetText()) or 0.0;
    end
    
    function self:SetValue(newValue, force)
        if newValue == -0.0 then newValue = 0.0 end;
        
        local oldValue = self:GetValue();
        if oldValue ~= newValue or force then
            if self.useFloatFormat then
                self:SetText(string.format("%.4f", newValue), true);     -- 'true' = block the OnTextChanged event.
            else
                self:SetText(tostring(newValue), true);
            end
        end
    end
    
    
    function self:UseFloatFormat()
        self.useFloatFormat = true;
        self:SetValue(self:GetValue(), true);
    end
    
    function self:UseDefaultFormat()
        self.useFloatFormat = false;
        self:SetValue(self:GetValue(), true);
    end
    
    
    self:OnFocus(function()
        self.isFocused = true;
    end);
    
    self:OnBlur(function()
        self.isFocused = false;
        
        if self:GetValue() == 0.0 then
            self:SetValue(0, true);
        end
    end);
    
    
    self:SetValue(defaultValue or 0);
    

    return self;
end