
function GTGUI.Element:Vector3Input()
    self.X = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:2px;' />");
    self.Y = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:2px;' />");
    self.Z = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='textbox' style='width:32%; margin-right:0px;' />");
    
    self.BlockSetFromXYZ     = false;
    self.BlockOnValueChanged = false;
    
    function self:SetFromXYZ(x, y, z)
        if not self.BlockSetFromXYZ then
            if x == -0.0 then x = 0.0 end;
            if y == -0.0 then y = 0.0 end;
            if z == -0.0 then z = 0.0 end;

            self.BlockOnValueChanged = true;
            self.X:SetText(string.format("%.4f", x));
            self.Y:SetText(string.format("%.4f", y));
            self.Z:SetText(string.format("%.4f", z));
            self.BlockOnValueChanged = false;
        end
    end
    
    function self:GetXYZ()
        return tonumber(self.X:GetText()) or 0.0, tonumber(self.Y:GetText()) or 0.0, tonumber(self.Z:GetText()) or 0.0;
    end
    
    function self:GetXYZTable()
        local xValue, yValue, zValue = self:GetXYZ();
        return {x = xValue, y = yValue, z = zValue};
    end
    
    
    -- This function is called when one of the text boxes has changed. We use the same event handler for all three.
    --
    -- When the text of one of these elements changes, we will post an event back to the engine to have it update the position of the
    -- object. The problem with this, is that when the position of the selected object changes via other means, these text boxes will
    -- be updated. We end up with a sort of cylclic dependency. What we do to resolve is block calls to SetFromXYZ() during processing
    -- of OnValueChanged() events.
    function OnTextChangedHandler()
        if not self.BlockOnValueChanged then
            self.BlockSetFromXYZ = true;
            self:OnValueChanged(self:GetXYZTable());
            self.BlockSetFromXYZ = false;
        end
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