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
    self.Items        = {};
    self.SelectedItem = nil;
    
    function self:Clear()
        self.Items        = {};
        self.SelectedItem = nil;
        
        self:DeleteAllChildren();
    end
    
    function self:AddItem(lineNumber, message)
        local itemElement = GTGUI.Server.CreateElement(self, "text-editor-error-list-box-item");
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