-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

NumberTextBoxFormats = {}
NumberTextBoxFormats.Default = 0;
NumberTextBoxFormats.Float   = 1;
NumberTextBoxFormats.Integer = 2;

-- A text box element more suited for a single number.
--
-- The value can be set with SetValue(). This will never post an OnTextChanged event.
function GTGUI.Element:NumberTextBox(defaultValue)
    self.format = NumberTextBoxFormats.Default;

    function self:GetValue()
        local result = tonumber(self:GetText()) or 0.0;
        
        if self.format == NumberTextBoxFormats.Integer then
            return math.floor(result);
        else
            return result;
        end
    end
    
    function self:SetValue(newValue, force)
        if newValue == -0.0 then newValue = 0.0 end;
        
        if self.format == NumberTextBoxFormats.Integer then
            newValue = math.floor(newValue);
        end
        
        -- We handle the case of 0.0 in a special way because for some reason the GCC build is having it turn into -0.0. Passing 'true' to
        -- SetText() causes the OnTextChanged() event to be blocked (not posted).
        local oldValue = self:GetValue();
        if oldValue ~= newValue or force then
            if self.format == NumberTextBoxFormats.Float then
                if newValue ~= 0.0 then
                    self:SetText(string.format("%.4f", newValue), true);
                else
                    self:SetText("0.0000", true);
                end
            else    -- Default
                if newValue ~= 0.0 then
                    self:SetText(string.format("%.4g", newValue), true);
                else
                    self:SetText("0", true);
                end
            end
        end
    end
    
    
    function self:UseFloatFormat()
        self.useFloatFormat = true;
        
        self.format = NumberTextBoxFormats.Float;
        self:SetValue(self:GetValue(), true);
    end

    function self:UseIntegerFormat()
        self.format = NumberTextBoxFormats.Integer;
        self:SetValue(self:GetValue(), true);
    end
    
    function self:UseDefaultFormat()
        self.useFloatFormat = false;
        
        self.format = NumberTextBoxFormats.Default;
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
    
    
    self:OnKeyDown(function(data)
        if data.key == GTGUI.Keys.Tab or data.key == GTGUI.Keys.Space then
            return false;
        end
        
        if self.format == NumberTextBoxFormats.Integer then
            if data.key == GTGUI.Keys.Period then
                return false;
            end
        end
    end);
    
    
    self:SetValue(defaultValue or 0, true);
    

    return self;
end