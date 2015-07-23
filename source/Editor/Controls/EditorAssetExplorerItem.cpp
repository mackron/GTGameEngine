// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorAssetExplorerItem.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorAssetExplorerItem::EditorAssetExplorerItem(Editor &editor, bool isFolder)
        : EditorControl(editor),
          m_isFolder(isFolder)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();
            const EditorTheme &theme = editor.GetTheme();

            gui.SetElementSizeRatio(hRootElement, 1.0f, 1.0f);
            gui.SetElementBorder(hRootElement, 1, theme.borderDefault);
            gui.SetElementBackgroundColor(hRootElement, theme.backgroundMid);
        }
    }
}
