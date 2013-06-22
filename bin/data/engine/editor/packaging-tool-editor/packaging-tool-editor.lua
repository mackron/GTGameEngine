-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:PackagingToolField()
    self.Left  = GTGUI.Server.CreateElement(self, "packaging-tool-editor-field-left");
    self.Right = GTGUI.Server.CreateElement(self, "packaging-tool-editor-field-right");
    
    return self;
end

function GTGUI.Element:PackagingToolEditor()
    self.Title = GTGUI.Server.CreateElement(self, "packaging-tool-editor-title");
    self.Title:SetText("Package for Distribution");
    
    self.Info  = GTGUI.Server.CreateElement(self, "packaging-tool-editor-info");
    self.Info:SetText("Note: This tool is currently very bare-bones. A complete overhaul will be done later on.");

    
    self.Body = GTGUI.Server.CreateElement(self, "packaging-tool-editor-body");
    
    
    -- Output directory
    self.OutputDirectoryField = GTGUI.Server.CreateElement(self.Body, "packaging-tool-editor-field");
    self.OutputDirectoryField:PackagingToolField();
    self.OutputDirectoryField:SetTooltip("Relative to the main executable.");
    
    self.OutputDirectoryLabel = GTGUI.Server.CreateElement(self.OutputDirectoryField.Left,  "packaging-tool-editor-label");
    self.OutputDirectoryLabel:SetText("Output Directory");
    self.OutputDirectory      = GTGUI.Server.CreateElement(self.OutputDirectoryField.Right, "packaging-tool-editor-textbox");
    self.OutputDirectory:SetText("releases/mygame");
    
    
    -- Executable name
    self.ExecutableNameField = GTGUI.Server.CreateElement(self.Body, "packaging-tool-editor-field");
    self.ExecutableNameField:PackagingToolField();
    self.ExecutableNameField:SetTooltip("The .exe extension will be added automatically for Windows builds.");
    
    self.ExecutableNameLabel = GTGUI.Server.CreateElement(self.ExecutableNameField.Left,  "packaging-tool-editor-label");
    self.ExecutableNameLabel:SetText("Executable Name");
    self.ExecutableName      = GTGUI.Server.CreateElement(self.ExecutableNameField.Right, "packaging-tool-editor-textbox");
    self.ExecutableName:SetText("mygame");

    
    -- Build button
    self.BuildButton = GTGUI.Server.CreateElement(self.Body, "packaging-tool-editor-build-button");
    self.BuildButton:SetText("Build");
    self.BuildButton:OnPressed(function()
        if self:DoBuild() then
            self.ResultMessage:SetText("Game built successfully.");
        else
            self.ResultMessage:SetText("An error occured.");
        end
    end);
    
    
    -- Result message.
    self.ResultMessage = GTGUI.Server.CreateElement(self.Body, "packaging-tool-editor-build-message");
    
    
    
    
    function self:DoBuild()
        local relativeOutputDirectory = self.OutputDirectory:GetText();
        local executableName          = self.ExecutableName:GetText();
    
        return Game.PackageForDistribution(relativeOutputDirectory, executableName);
    end
    
    
    return self;
end