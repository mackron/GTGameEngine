-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:LabelledVector2Input(title, initialValue)
    self:LabelledElement(title);
    
    self.Input = GTGUI.Server.CreateElement(self.Right:GetID(), "labelled-vector2-input-input");
    self.Input:Vector2Input();
    self.Input:SetValue(initialValue);
    
    
    function self:GetValue()
        return self.Input:GetValue();
    end
    
    function self:SetValue(x, y)         -- 'x' can be a math.vec2.
        self.Input:SetValue(x, y);
    end
    
    
    
    self.Input:OnValueChanged(function(data)
        self:OnValueChanged(data);
    end);
    
    
    function self:OnValueChanged(arg1)
        self.Callbacks:BindOrCall("OnValueChanged", arg1);
    end
    
    return self;
end