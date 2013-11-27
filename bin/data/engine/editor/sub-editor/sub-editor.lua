-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SubEditor(_internalPtr)
    self._internalPtr = _internalPtr;
    
    self.ToolBar = GTGUI.Server.CreateElement("EditorTopPanel_Right", "sub-editor-toolbar");

    
    function self:DeleteToolBar()
        GTGUI.Server.DeleteElement(self.ToolBar);
    end
    
    function self:ShowToolBar()
        self.ToolBar:Show();
    end
    
    function self:HideToolBar()
        self.ToolBar:Hide();
    end
    
    
    function self:OnShow()
        self:ShowToolBar();
    end
    
    function self:OnHide()
        print("HIDING!");
        self:HideToolBar();
    end
    
    
    -------------------------------
    -- FFI Connectors.
    
    function self:MarkAsModified()
        return GTEngine.System.SubEditor.MarkAsModified(self._internalPtr);
    end
end
