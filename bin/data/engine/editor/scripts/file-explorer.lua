
-- Takes the given element and turns it into a file explorer.
function Server.Element:FileExplorer()
    
    -- The address bar.
    self.addressBar = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='file-explorer-address-bar' />");
    self.addressBar:FileExplorerAddressBar(self);

    -- The main file view.
    self.mainView = Server.New("<div parentid='" .. self:GetID() .. "' styleclass='file-explorer-main-view' />");


end





-- !!! Address Bar !!! --
function Server.Element:FileExplorerAddressBar(parent)
    
end
