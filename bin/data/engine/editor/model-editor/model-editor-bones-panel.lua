-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:ModelEditor_BonesPanelTreeView()
    self:TreeView();
    self.vertScrollbar:SetStyle("border-right",  "none");
    self.vertScrollbar:SetStyle("border-top",    "none");
    self.vertScrollbar:SetStyle("border-bottom", "none");
    self.horzScrollbar:SetStyle("border-right",  "none");
    self.horzScrollbar:SetStyle("border-bottom", "none");
    self.horzScrollbar:SetStyle("border-left",   "none");
    
    self.scrollbarFiller:SetStyle("border-left", "1px #5a5a5a");
    self.scrollbarFiller:SetStyle("border-top",  "1px #5a5a5a");

    return self;
end

function GTGUI.Element:ModelEditor_BonesPanel(_internalPtr)
    self:PanelGroupBox("Bones");
    self.Bones = {};                -- Keyed by the bone name. The value is the tree-view item.
    
    self.TreeView = GTGUI.Server.CreateElement(self.Body, "model-editor-bones-panel-treeview");
    self.TreeView:ModelEditor_BonesPanelTreeView();
    
    
    function self:AddBone(bone)
        -- We assert that the parent is already added.
        if bone and self.Bones[bone.name] == nil then
            self.Bones[bone.name] = self.TreeView:AddItem(bone.name, self.Bones[bone.parentName]);
        end
    end
    
    function self:Refresh()
        self.Bones = {};
        self.TreeView:RemoveAllItems();
        
        local bones = GTEngine.System.ModelEditor.GetBones(_internalPtr);
        if bones then
            local _GetBoneByName = function(name)
                for i,bone in ipairs(bones) do
                    if bone.name == name then return bone end;
                end
                
                return nil;
            end
        
            local _AddBone = function(bone)
                if bone.parentName then
                    if self.Bones[bone.parentName] == nil then
                        self:AddBone(_GetBoneByName(bone.parentName));
                    end
                end
                    
                self:AddBone(bone);
            end
        
            for i,bone in ipairs(bones) do
                _AddBone(bone);
            end
        end
    end
    
    return self;
end