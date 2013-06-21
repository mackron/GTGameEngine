-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:TextEditorErrorListBoxItem(lineNumber, message)
    self.LineNumberBox = GTGUI.Server.CreateElement(self, "text-editor-error-list-box-item-linenumber");
    self.LineNumberBox:SetText(tostring(lineNumber));
    
    self.MessageBox = GTGUI.Server.CreateElement(self, "text-editor-error-list-box-item-message");
    self.MessageBox:SetText(message);
    
    
    function self:GetLineNumber()
        return tonumber(self.LineNumberBox:GetText());
    end
    
    function self:GetMessage()
        return self.MessageBox:GetText();
    end
    
    return self;
end


function GTGUI.Element:TextEditorErrorListBox(textEditor)
    self.Items                 = {};
    self.SelectedItem          = nil;
    self.ItemHeight            = 22;
    self.MouseWheelScrollSpeed = 3;
    
    self.Body = GTGUI.Server.CreateElement(self, "text-editor-error-list-box-body");
    
    self.Scrollbar = GTGUI.Server.CreateElement(self, "text-editor-error-list-box-scrollbar");
    self.Scrollbar:VerticalScrollbar();
    self.Scrollbar:EnableAutoVisibility();
    self.Scrollbar:SetPageSize(self:GetHeight());
    self.Scrollbar:OnScroll(function(data)
        local offset = -data.position;
        self.Body:SetStyle("inner-offset-y", tostring(offset * self.ItemHeight));
    end);
    
    self.Body:OnSize(function()
        if self.Body:IsVisible() then
            self:UpdateScrollbar();
        end
    end);
    
    self.Body:OnShow(function()
        self:UpdateScrollbar();
    end);
    
    self:OnSize(function()
        if self:IsVisible() then
            self:UpdateScrollbar();
        end
    end);
    
    self:OnShow(function()
        self:UpdateScrollbar();
    end);
    
    self:OnMouseEnter(function()
        self.IsMouseOver = true;
    end);
    
    self:OnMouseLeave(function()
        self.IsMouseOver = false;
    end);
    
    self:WatchMouseWheel(function(data)
        if self.IsMouseOver then
            self.Scrollbar:Scroll(-data.delta * self.MouseWheelScrollSpeed);
        end
    end);

    
    function self:Clear()
        self.Items        = {};
        self.SelectedItem = nil;
        
        self.Body:DeleteAllChildren();
    end
    
    function self:AddItem(lineNumber, message)
        local itemElement = GTGUI.Server.CreateElement(self.Body, "text-editor-error-list-box-item");
        itemElement:TextEditorErrorListBoxItem(lineNumber, message);
        
        itemElement:OnPressed(function()
            self:SelectItem(itemElement);
        end);
        
        
        self.Items[#self.Items + 1] = itemElement;
        
        return itemElement;
    end

    
    function self:SelectItem(itemToSelect)
        self:DeselectItem();
        
        self.SelectedItem = itemToSelect;
        if self.SelectedItem then
            self.SelectedItem:AttachStyleClass("text-editor-error-list-box-item-selected");
            
            textEditor:OnErrorSelected(self.SelectedItem:GetLineNumber(), self.SelectedItem:GetMessage());
        end
    end
    
    function self:DeselectItem()
        if self.SelectedItem then
            self.SelectedItem:DetachStyleClass("text-editor-error-list-box-item-selected");
        end
    end
    
    
    function self:CalculateScrollbarRange()
        if #self.Items == 0 then
            return 0;
        else
            return #self.Items + 1;     -- +1 just to make sure everything is visible.
        end
    end
    
    function self:CalculateScrollbarPageSize()
        return math.floor(self:GetInnerHeight() / self.ItemHeight);
    end
    
    function self:UpdateScrollbar()
        local range    = self:CalculateScrollbarRange();
        local pageSize = self:CalculateScrollbarPageSize();
        
        self.Scrollbar:SetRange(range);
        self.Scrollbar:SetPageSize(pageSize);
    end
    
    
    return self;
end


function GTGUI.Element:TextEditorErrorList(textEditor)
    self.ListBox = GTGUI.Server.CreateElement(self, "text-editor-error-list-box");
    self.ListBox:TextEditorErrorListBox(textEditor);
    
    
    function self:Clear()
        self.ListBox:Clear();
    end
    
    function self:AddItem(lineNumber, message)
        local itemElement = self.ListBox:AddItem(lineNumber, message);
        if itemElement then
        end
    end
    
    return self;
end