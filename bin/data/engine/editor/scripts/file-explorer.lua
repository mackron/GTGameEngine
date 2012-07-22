
-- Takes the given element and turns it into a file explorer.
function GTGUI.Element:FileExplorer()
    
    -- The address bar.
    self.addressBar = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='file-explorer-address-bar' />");
    self.addressBar:FileExplorerAddressBar(self);

    -- The main file view.
    self.mainView = GTGUI.Server.New("<div parentid='" .. self:GetID() .. "' styleclass='file-explorer-main-view' />");


end





-- !!! Address Bar !!! --
function GTGUI.Element:FileExplorerAddressBar(parent)
    
end
