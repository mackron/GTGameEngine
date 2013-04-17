-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:LabelledVector4Input(title, initialValue)
    self:LabelledElement(title);
    
    self.Input = GTGUI.Server.CreateElement(self.Right:GetID(), "labelled-vector4-input-input");
    self.Input:Vector4Input();
    self.Input:SetValue(initialValue);
    
    
    function self:GetValue()
        return self.Input:GetValue();
    end
    
    function self:SetValue(x, y, z, w)         -- 'x' can be a math.vec4.
        self.Input:SetValue(x, y, z, w);
    end
    
    
    
    self.Input:OnValueChanged(function(data)
        self:OnValueChanged(data);
    end);
    
    
    function self:OnValueChanged(arg1)
        self.Callbacks:BindOrCall("OnValueChanged", arg1);
    end
    
    return self;
end