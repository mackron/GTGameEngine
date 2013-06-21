-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:TextEditorPanel(textEditor)

    -- Tab Bar
    self.TabBar = GTGUI.Server.CreateElement(self, "text-editor-panel-tabbar");
    self.TabBar:TabBar();
    self.TabBar.ActiveTabBorderColor     = "#222";
    self.TabBar.HoveredTabBorderColor    = "#222";
    self.TabBar.ActiveTabBackgroundColor = "#363636";
    
    self.TabBar:OnTabActivated(function(data)
        if data.tab == self.ErrorListTab then
            self.ErrorList:Show();
        end
    end);
    
    self.TabBar:OnTabDeactivated(function(data)
        if data.tab == self.ErrorListTab then
            self.ErrorList:Hide();
        end
    end);
    
    
    self.ErrorListTab = self.TabBar:AddTab("Error List");
    
    
    -- Body
    self.Body = GTGUI.Server.CreateElement(self, "text-editor-panel-body");
    
    
    -- Panels
    self.ErrorList = GTGUI.Server.CreateElement(self.Body, "text-editor-error-list");
    self.ErrorList:TextEditorErrorList(textEditor);
    
    
    
    -- Show the error list by default.
    self.TabBar:ActivateTab(self.ErrorListTab);
    
    return self;
end