function self:OnStartup()
    GTEngine.LoadGUIStyleScriptFromFile("tutorials/gui_basics/main-menu.style");

    local parentElement = Game.GetGameWindowGUIElement();
    if parentElement ~= nil then
        self.GUIContainer = GTEngine.CreateGUIElement(parentElement, "main-menu");

        self.StartGameButton = GTEngine.CreateGUIElement(self.GUIContainer, "main-menu-button");
        self.StartGameButton:SetText("Start Game");
        self.StartGameButton:OnPressed(function()
            Game.LoadScene("tutorials/gui_basics/game.gtscene");
        end);

        self.OpenEditorButton = GTEngine.CreateGUIElement(self.GUIContainer, "main-menu-button");
        self.OpenEditorButton:SetText("Open Editor");
        self.OpenEditorButton:OnPressed(function()
            Game.OpenEditor();
        end);

        self.ExitButton = GTEngine.CreateGUIElement(self.GUIContainer, "main-menu-button");
        self.ExitButton:SetText("Exit to Desktop");
        self.ExitButton:OnPressed(function()
            Game.Close();
        end);
    end
end

function self:OnShutdown()
    GTEngine.DeleteGUIElement(self.GUIContainer);
end