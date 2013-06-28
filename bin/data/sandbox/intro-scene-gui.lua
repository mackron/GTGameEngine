self.TransitionTimeInSeconds = 1.0;
self.TargetOpacity           = 1.0;
self.StartOpacity            = 0.0;
self.TargetMarginTop         = 20;
self.StartMarginTop          = 100;

function self:OnStartup()
    GTEngine.LoadGUIStyleScriptFromFile("sandbox/intro-scene-gui.style");

    local parentElement = Game.GetGameWindowGUIElement();
    if parentElement ~= nil then
        self.GUIContainer            = GTEngine.CreateGUIElement(parentElement, "intro-scene-container");
        
        -- Header (tiny bar along the top)
        self.GUIContainerHeader      = GTEngine.CreateGUIElement(self.GUIContainer, "intro-scene-container-header");
        self.GUIContainerHeaderLeft  = GTEngine.CreateGUIElement(self.GUIContainerHeader, "intro-scene-container-header-left");
        self.GUIContainerHeaderRight = GTEngine.CreateGUIElement(self.GUIContainerHeader, "intro-scene-container-header-right");

        -- Body.
        self.GUIContainerBody        = GTEngine.CreateGUIElement(self.GUIContainer, "intro-scene-container-body");
        
        -- Footer.
        self.GUIContainerFooter      = GTEngine.CreateGUIElement(self.GUIContainer, "intro-scene-container-footer");
        self.GUIContainerFooterLeft  = GTEngine.CreateGUIElement(self.GUIContainerFooter, "intro-scene-container-footer-left");
        self.GUIContainerFooterRight = GTEngine.CreateGUIElement(self.GUIContainerFooter, "intro-scene-container-footer-right");
        
        self.GUITitle = GTEngine.CreateGUIElement(self.GUIContainerBody, "intro-scene-jumbo-title");
        self.GUITitle:SetText("Welcome");

        self.GUIInfo = GTEngine.CreateGUIElement(self.GUIContainerBody, "intro-scene-info");
        
        GTEngine.CreateGUIElement(self.GUIInfo, "intro-scene-info-line"):SetText(
            "to the GTGameEngine Sandbox."
        );

        self.GUILinksContainer      = GTEngine.CreateGUIElement(self.GUIContainerBody, "intro-scene-links-container");
        self.GUILinksContainerLeft  = GTEngine.CreateGUIElement(self.GUILinksContainer, "intro-scene-links-container-left");
        self.GUILinksContainerRight = GTEngine.CreateGUIElement(self.GUILinksContainer, "intro-scene-links-container-right");

        self.GUIGetStartedButton = GTEngine.CreateGUIElement(self.GUILinksContainerLeft, "intro-scene-link");
        self.GUIGetStartedButton:SetText("Get Started");
        self.GUIGetStartedButton:SetStyle("margin-top", "0px");
        self.GUIGetStartedButton:OnPressed(function()
            GTCore.System.OpenFileOrURL("http://mackron.github.io/GTGameEngine/getstarted/");
        end);
        self.GUIGetStartedButton:OnMouseEnter(function()
            self.GUIGetStartedInfo:Show();
        end);
        self.GUIGetStartedButton:OnMouseLeave(function()
            self.GUIGetStartedInfo:Hide();
        end);
        
        self.GUIEditorButton = GTEngine.CreateGUIElement(self.GUILinksContainerLeft, "intro-scene-link");
        self.GUIEditorButton:SetText("Open Editor");
        self.GUIEditorButton:OnPressed(function()
            Game.OpenEditor();
            Editor.OpenFile("engine/editor-notes.txt");
        end);
        self.GUIEditorButton:OnMouseEnter(function()
            self.GUIEditorInfo:Show();
        end);
        self.GUIEditorButton:OnMouseLeave(function()
            self.GUIEditorInfo:Hide();
        end);

        self.GUIGetStartedInfo = GTEngine.CreateGUIElement(self.GUILinksContainerRight, "intro-scene-link-info");
        self.GUIGetStartedInfo:SetText(
            "If you're new to GTGameEngine, you'll probably\n" ..
            "want to start here.\n\n" ..
            "(Opens in a browser window.)"
        );

        self.GUIEditorInfo = GTEngine.CreateGUIElement(self.GUILinksContainerRight, "intro-scene-link-info");
        self.GUIEditorInfo:SetText(
            "Jump right in and start making some scenes.\n\n" ..
            "Edit data/sandbox-config.lua to set the\nstartup scene.\n\n"
        );

        
        -- Header Content
        self.GUICloseButton = GTEngine.CreateGUIElement(self.GUIContainerHeaderRight, "intro-scene-url");
        self.GUICloseButton:SetStyle("margin", "0px");
        self.GUICloseButton:SetText("Close");
        self.GUICloseButton:OnPressed(function()
            Game.Close();
        end);
        
        -- Footer Content
        self.GUIHomePage = GTEngine.CreateGUIElement(self.GUIContainerFooterLeft, "intro-scene-url");
        self.GUIHomePage:SetText("Home Page");
        self.GUIHomePage:OnPressed(function()
            GTCore.System.OpenFileOrURL("http://mackron.github.io/GTGameEngine");
        end);

        self.GUIWebSite = GTEngine.CreateGUIElement(self.GUIContainerFooterLeft, "intro-scene-url");
        self.GUIWebSite:SetText("GitHub Project");
        self.GUIWebSite:OnPressed(function()
            GTCore.System.OpenFileOrURL("https://github.com/mackron/GTGameEngine");
        end);

        self.GUIEmail   = GTEngine.CreateGUIElement(self.GUIContainerFooterRight, "intro-scene-email");
        self.GUIEmail:SetText("mackron@gmail.com");

        self.GUIVersion = GTEngine.CreateGUIElement(self.GUIContainerFooterRight, "intro-scene-version");
        self.GUIVersion:SetText("Version " .. GTEngine.GetVersionString());
    end

    self.CurrentOpacity        = self.StartOpacity;
    self.CurrentMarginTop      = self.StartMarginTop;
    self.CurrentTransitionTime = 0.0;
end

function self:OnShutdown()
    GTGUI.Server.DeleteElement(self.GUIContainer);
end

function self:OnUpdate(deltaTimeInSeconds)
    if deltaTimeInSeconds > 0.05 then
        deltaTimeInSeconds = 0.05;
    end

    if self.CurrentOpacity < self.TargetOpacity then
        --local mixDelta = self.CurrentTransitionTime / self.TransitionTimeInSeconds;
        local mixDelta = 2.5 * deltaTimeInSeconds;

        self.CurrentOpacity = math.mix(self.CurrentOpacity, self.TargetOpacity, mixDelta);
        self.GUIContainerBody:SetStyle("opacity", tostring(self.CurrentOpacity));

        self.CurrentMarginTop = math.mix(self.CurrentMarginTop, self.TargetMarginTop, mixDelta);
        self.GUITitle:SetStyle("margin-top", math.floor(self.CurrentMarginTop));


        self.CurrentTransitionTime = self.CurrentTransitionTime + deltaTimeInSeconds;
    end
end