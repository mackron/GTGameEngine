-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SubEditor()
    self.ToolBar = GTGUI.Server.New("<div parentid='EditorTopPanel_Right' style='width:auto; visible:false; height:100%; vertical-align:center;' />");

    function self:DeleteToolBar()
        GTGUI.Server.DeleteElement(self.ToolBar);
    end
    
    function self:ShowToolBar()
        self.ToolBar:Show();
    end
    
    function self:HideToolBar()
        self.ToolBar:Hide();
    end
end