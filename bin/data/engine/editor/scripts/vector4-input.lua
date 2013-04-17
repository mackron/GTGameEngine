-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:Vector4Input()
    self.X = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:2px;' />");
    self.Y = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:2px;' />");
    self.Z = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:2px;' />");
    self.W = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:0px;' />");
    
    self.X:NumberTextBox():UseFloatFormat();
    self.Y:NumberTextBox():UseFloatFormat();
    self.Z:NumberTextBox():UseFloatFormat();
    self.W:NumberTextBox():UseFloatFormat();
    
    self.Locked = false;
    
    
    function self:GetX()
        return self.X:GetValue();
    end
    function self:GetY()
        return self.Y:GetValue();
    end
    function self:GetZ()
        return self.Z:GetValue();
    end
    function self:GetW()
        return self.W:GetValue();
    end

    function self:SetX(value)
        if not self.Locked then self.X:SetValue(value) end;
    end
    function self:SetY(value)
        if not self.Locked then self.Y:SetValue(value) end;
    end
    function self:SetZ(value)
        if not self.Locked then self.Z:SetValue(value) end;
    end
    function self:SetW(value)
        if not self.Locked then self.W:SetValue(value) end;
    end
    
    
    
    function self:SetValue(x, y, z, w)
        if x ~= nil then
            if type(x) == 'table' then      -- If it's a table, assume a math.vec4.
                self:SetX(x.x);
                self:SetY(x.y);
                self:SetX(x.z);
                self:SetW(x.w);
            else
                self:SetX(x);
                self:SetY(y or x);
                self:SetZ(z or x);
                self:SetW(w or x);
            end
        end
    end
    
    function self:GetValue()
        return math.vec4(self:GetX(), self:GetY(), self:GetZ(), self:GetW());
    end
    
    
    
    
    
    function OnTextChangedHandler()
        self:OnValueChanged(self:GetXYZWTable());
    end

    self.X:OnTextChanged(OnTextChangedHandler);
    self.Y:OnTextChanged(OnTextChangedHandler);
    self.Z:OnTextChanged(OnTextChangedHandler);
    self.W:OnTextChanged(OnTextChangedHandler);
    
    
    
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
            self.W:Focus();
            self.W:SelectAllText();
            
            return false;
        end
    end);
    
    self.W:OnKeyDown(function(data)
        if data.key == GTCore.Keys.Tab then
            self.X:Focus();
            self.X:SelectAllText();
            
            return false;
        end
    end);
    
    
    function self:OnValueChanged(arg1)
        self.Locked = true;
            self.Callbacks:BindOrCall("OnValueChanged", arg1);
        self.Locked = false;
    end
    
    
    
    
    ----------------------------------------
    -- Deprecated.
    function self:SetFromXYZW(x, y, z, w)
        self:SetX(x);
        self:SetY(y);
        self:SetZ(z);
        self:SetW(w);
    end
    
    function self:SetFromXYZWTable(value)
        self:SetFromXYZW(value.x, value.y, value.z, value.w);
    end
    
    function self:GetXYZW()
        return self:GetX(), self:GetY(), self:GetZ(), self:GetW();
    end
    
    function self:GetXYZWTable()
        return {x = self:GetX(), y = self:GetY(), z = self:GetZ(), w = self:GetW()};
    end
    
    
    return self;
end