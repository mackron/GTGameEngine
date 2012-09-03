
function GTGUI.Element:TabBarTab(text)
    self.text        = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='tabbar-tab-text'        />");
    self.borderHider = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='tabbar-tab-borderhider' />");
    
    self.isActive   = false;
    self.isModified = false;
    
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
    
    
    function self:IsModified()
        return self.isModified;
    end
    
    function self:MarkAsModified()
        if not self.isModified then
            self.isModified = true;
            self.text:SetText(self.text:GetText() .. "*");
            
            -- Menu items need to be enabled.
            Editor_MenuBar.File.Save:Enable();
            Editor_MenuBar.File.SaveAll:Enable();
        end
    end
    
    function self:UnmarkAsModified()
        if self.isModified then
            self.isModified = false;
            self.text:SetText(string.sub(self.text:GetText(), 0, -2));
            
            -- Here we keep our menu items up-to-date.
            Editor_MenuBar.File.Save:Disable();
            
            if not Editor_TabBar:HasModifiedItem() then
                Editor_MenuBar.File.SaveAll:Disable();
            end
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
        
        -- The center must be visible. It will be made invisible when there is not tabs open.
        EditorCenterCenterPanel:Show();
        Editor_MenuBar.File.Close:Enable();
        Editor_MenuBar.File.CloseAll:Enable();
        

        return tab:TabBarTab(text);
    end
    
    function self:RemoveTab(tab)
        local newActiveTab = self.activeTab;
        if tab == self.activeTab then
            newActiveTab = self.activeTab:GetNextSibling() or self.activeTab:GetPrevSibling();
            self:DeactivateActiveTab();
        end
        
        self:OnTabRemoved({tab = tab});
        
        GTGUI.Server.DeleteElement(tab);

        if newActiveTab ~= nil then
            self:ActivateTab(newActiveTab);
        else
            EditorCenterCenterPanel:Hide();
            Editor_MenuBar.File.Close:Disable();
            Editor_MenuBar.File.CloseAll:Disable();
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
    
    
    function self:ActivateTab(tab)
        if tab then tab:Activate(); end
    end
    
    function self:__ActivateTab(tab)
        if self.activeTab ~= tab then
            self:DeactivateActiveTab();
            self.activeTab = tab;
            
            self:OnTabActivated({tab = tab});
            
            if self.activeTab:IsModified() then
                Editor_MenuBar.File.Save:Enable();
            else
                Editor_MenuBar.File.Save:Disable();
            end
            
            if Editor_TabBar:HasModifiedItem() then
                Editor_MenuBar.File.SaveAll:Enable();
            else
                Editor_MenuBar.File.SaveAll:Disable();
            end
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
    
    
    function self:HasModifiedItem()
        for key,value in pairs(self.Children) do
            if value.isModified then
                return true;
            end
        end
        return false;
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























































