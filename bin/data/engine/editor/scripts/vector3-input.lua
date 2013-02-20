-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:Vector3Input()
    self.X = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:2px;' />");
    self.Y = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:2px;' />");
    self.Z = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:0px;' />");
    
    self.X:NumberTextBox():UseFloatFormat();
    self.Y:NumberTextBox():UseFloatFormat();
    self.Z:NumberTextBox():UseFloatFormat();
    

    function self:GetX()
        return self.X:GetValue();
    end
    function self:GetY()
        return self.Y:GetValue();
    end
    function self:GetZ()
        return self.Z:GetValue();
    end
    
    
    function self:SetX(value)
        self.X:SetValue(value);
    end
    function self:SetY(value)
        self.Y:SetValue(value);
    end
    function self:SetZ(value)
        self.Z:SetValue(value);
    end
    
    
    function self:SetFromXYZ(x, y, z)
        self:SetX(x);
        self:SetY(y);
        self:SetZ(z);
    end
    
    function self:SetFromXYZTable(value)
        self:SetFromXYZ(value.x, value.y, value.z);
    end
    
    function self:GetXYZ()
        return self:GetX(), self:GetY(), self:GetZ();
        --return tonumber(self.X:GetText()) or 0.0, tonumber(self.Y:GetText()) or 0.0, tonumber(self.Z:GetText()) or 0.0;
    end
    
    function self:GetXYZTable()
        return {x = self:GetX(), y = self:GetY(), z = self:GetZ()};
    end
    
    

    function OnTextChangedHandler()
        self:OnValueChanged(self:GetXYZTable());
    end

    self.X:OnTextChanged(OnTextChangedHandler);
    self.Y:OnTextChanged(OnTextChangedHandler);
    self.Z:OnTextChanged(OnTextChangedHandler);
    
    
    
    self.X:OnKeyDown(function(data)
        if data.key == GTCore.Keys.Tab then
            self.Y:Focus();
            self.Y:SelectAllText();
            
            return false;
        end
    end);
    
    self.Y:OnKeyDown(function(data)
        if data.key == GTCore.Keys.Tab then
            self.Z:Focus();
            self.Z:SelectAllText();
            
            return false;
        end
    end);
    
    self.Z:OnKeyDown(function(data)
        if data.key == GTCore.Keys.Tab then
            self.X:Focus();
            self.X:SelectAllText();
            
            return false;
        end
    end);
    
    
    function self:OnValueChanged(arg1)
        self.Callbacks:BindOrCall("OnValueChanged", arg1);
    end
    
    return self;
end