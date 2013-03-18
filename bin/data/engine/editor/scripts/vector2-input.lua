-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:Vector2Input()
    self.X = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:2px;' />");
    self.Y = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%;' />");
    
    self.X:NumberTextBox():UseFloatFormat();
    self.Y:NumberTextBox():UseFloatFormat();
    
    self.Locked = false;
    
    
    function self:GetX()
        return self.X:GetValue();
    end
    function self:GetY()
        return self.Y:GetValue();
    end
    
    
    function self:SetX(value)
        if not self.Locked then self.X:SetValue(value) end;
    end
    function self:SetY(value)
        if not self.Locked then self.Y:SetValue(value) end;
    end
    
    
    function self:SetFromXY(x, y)
        self:SetX(x);
        self:SetY(y);
    end
    
    function self:SetFromXYTable(value)
        self:SetFromXY(value.x, value.y);
    end
    
    
    function self:GetXY()
        return self:GetX(), self:GetY();
    end
    
    function self:GetXYTable()
        return {x = self:GetX(), y = self:GetY()};
    end
    
    

    function OnTextChangedHandler()
        self:OnValueChanged(self:GetXYTable());
    end

    self.X:OnTextChanged(OnTextChangedHandler);
    self.Y:OnTextChanged(OnTextChangedHandler);
    
    
    
    self.X:OnKeyDown(function(data)
        if data.key == GTCore.Keys.Tab then
            self.Y:Focus();
            self.Y:SelectAllText();
            
            return false;
        end
    end);
    
    self.Y:OnKeyDown(function(data)
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
    
    return self;
end