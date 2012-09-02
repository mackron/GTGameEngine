
function GTGUI.Element:TabBarTab(text)
    self.text        = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='tabbar-tab-text'        />");
    self.borderHider = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='tabbar-tab-borderhider' />");
    
    self.isActive = false;
    
    function self:Activate()
        if not self.isActive then
            self.isActive = true;
            self:AttachStyleClass('tabbar-tab-active');
            self.borderHider:Show();
            self.Parent:__ActivateTab(self);
        end
    end
    
    function self:Deactivate()
        if self.isActive then
            self.isActive = false;
            self:DetachStyleClass('tabbar-tab-active');
            self.borderHider:Hide();
        end
    end
    
    
    self:OnLMBDown(function()
        self:Activate();
    end)
    
    self:OnMMBDown(function()
        self.Parent:RemoveTab(self);
    end)
    
    
    self.text:SetText(text);
    
    return self;
end


function GTGUI.Element:TabBar()
    self.activeTab = nil;
    
    function self:AddTab(text)
        local tab = GTGUI.Server.New("<div styleclass='tabbar-tab' />");
        self:PrependChild(tab);
        
        self:OnTabAdded({tab = tab});
        
        return tab:TabBarTab(text);
    end
    
    function self:RemoveTab(tab)
        local newActiveTab = nil;
        if tab == self.activeTab then
            newActiveTab = self.activeTab:GetNextSibling() or self.activeTab:GetPrevSibling();
            self:DeactivateActiveTab();
        end
        
        self:OnTabRemoved({tab = tab});
        
        GTGUI.Server.DeleteElement(tab);
        
        if newActiveTab ~= nil then
            self:ActivateTab(newActiveTab);
        end
    end
    
    
    function self:ActivateTab(tab)
        if tab then tab:Activate() end;
    end
    
    function self:__ActivateTab(tab)
        if self.activeTab ~= tab then
            self:DeactivateActiveTab();
            self.activeTab = tab;
            
            self:OnTabActivated({tab = tab});
        end
    end
    
    function self:DeactivateActiveTab()
        if self.activeTab ~= nil then
            self.activeTab:Deactivate();
            
            self:OnTabDeactivated({tab = self.activeTab});
            
            self.activeTab = nil;
        end
    end
    
    
    function self:GetActiveTab()
        return self.activeTab;
    end
    
    
    function self:OnTabAdded(arg1)
        if type(arg1) == 'function' then
            self:BindEvent('OnTabAdded', arg1);
        else
            self:CallEvent('OnTabAdded', arg1);
        end
    end
    
    function self:OnTabRemoved(arg1)
        if type(arg1) == 'function' then
            self:BindEvent('OnTabRemoved', arg1);
        else
            self:CallEvent('OnTabRemoved', arg1);
        end
    end
    
    function self:OnTabActivated(arg1)
        if type(arg1) == 'function' then
            self:BindEvent('OnTabActivated', arg1);
        else
            self:CallEvent('OnTabActivated', arg1);
        end
    end
    
    function self:OnTabDeactivated(arg1)
        if type(arg1) == 'function' then
            self:BindEvent('OnTabDeactivated', arg1);
        else
            self:CallEvent('OnTabDeactivated', arg1);
        end
    end
end























































