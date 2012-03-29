-- This keeps track of the menu that is currently open, if any.
ActiveMenu = nil;
    
-- Hides the active menu, if any.
function HideActiveMenu()
    if ActiveMenu then
        ActiveMenu:Hide();
    end
end


-- Initialises a menu object from associated XML elements.
function InitMenu(menu, button)
    menu.button   = button;
    menu.prevShow = menu.Show;
    menu.prevHide = menu.Hide;
        
    function menu:Show()
        if ActiveMenu ~= nil and ActiveMenu ~= self then
            ActiveMenu:Hide();
        end
    
        self.button:Hide();
        self:prevShow();
        
        ActiveMenu = self;
    end
        
    function menu:Hide()
        self:prevHide();
        self.button:Show();
        
        ActiveMenu = nil;
    end
        
        
    menu:OnLMBDown(function(data)
        if ActiveMenu == menu and menu:IsChild(data.receiver) == false then
            menu:Hide();
        end
    end)
        
    button:OnPush(function()
        menu:Show();
    end)
        
    button:OnMouseEnter(function()
        if ActiveMenu and ActiveMenu ~= menu then
            menu:Show();
        end
    end)
end