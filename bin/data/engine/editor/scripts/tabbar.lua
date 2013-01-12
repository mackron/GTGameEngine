
function GTGUI.Element:TabBarTab(text)
    self.text        = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='tabbar-tab-text'        />");
    self.borderHider = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='tabbar-tab-borderhider' />");
    self.isActive    = false;

    function self:Activate(blockEvent)
        if not self.isActive then
            self.isActive = true;
            self:AttachStyleClass('tabbar-tab-active');
            self.borderHider:Show();
            self.Parent:__ActivateTab(self, blockEvent);
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
        if tab then
            local newActiveTab = self.activeTab;
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
    end
    
    function self:RemoveActiveTab()
        if self.activeTab then
            self:RemoveTab(self.activeTab);
        end
    end
    
    function self:RemoveAllTabs()
        local tempTable = {};
        for key,value in pairs(self.Children) do
            tempTable[#tempTable + 1] = value;
        end

        for i,value in ipairs(tempTable) do
            self:RemoveTab(value);
        end
    end
    
    
    function self:ActivateTab(tab, blockEvent)
        if tab then tab:Activate(blockEvent); end
    end
    
    function self:__ActivateTab(tab, blockEvent)
        if self.activeTab ~= tab then                       -- This is a super important check. Leave this.
            self:DeactivateActiveTab();
            self.activeTab = tab;
            
            if not blockEvent then self:OnTabActivated({tab = tab}) end;
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
            return self:BindEvent('OnTabAdded', arg1);
        else
            self:CallEvent('OnTabAdded', arg1);
        end
    end
    
    function self:OnTabRemoved(arg1)
        if type(arg1) == 'function' then
            return self:BindEvent('OnTabRemoved', arg1);
        else
            self:CallEvent('OnTabRemoved', arg1);
        end
    end
    
    function self:OnTabActivated(arg1)
        if type(arg1) == 'function' then
            return self:BindEvent('OnTabActivated', arg1);
        else
            self:CallEvent('OnTabActivated', arg1);
        end
    end
    
    function self:OnTabDeactivated(arg1)
        if type(arg1) == 'function' then
            return self:BindEvent('OnTabDeactivated', arg1);
        else
            self:CallEvent('OnTabDeactivated', arg1);
        end
    end
end























































