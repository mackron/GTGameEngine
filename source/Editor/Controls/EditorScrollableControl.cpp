// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorAssetExplorer.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorScrollableControl::EditorScrollableControl(Editor &editor)
        : EditorQuadControl(editor),
          m_deadSpaceSizingHandler(this->GetBottomRightQuadrant(), this->GetTopRightQuadrant(), this->GetBottomLeftQuadrant())
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();
            const EditorTheme &theme = editor.GetTheme();


            // Bottom container.
            gui.SetElementHeightToChildren(this->GetBottom());

            
            // Vertical scrollbar container.
            HGUIElement hVerticalContainer = this->GetTopRightQuadrant();
            gui.SetElementWidthToChildren(hVerticalContainer);
            //gui.SetElementWidth(hVerticalContainer, theme.scrollbarWidth);
            gui.SetElementBackgroundColor(hVerticalContainer, theme.scrollbarBackgroundColor);
            gui.AttachLocalEventHandler(hVerticalContainer, m_deadSpaceSizingHandler);


            // Horizontal scrollbar container.
            HGUIElement hHorizontalContainer = this->GetBottomLeftQuadrant();
            gui.SetElementHeightToChildren(hHorizontalContainer);
            //gui.SetElementHeight(hHorizontalContainer, theme.scrollbarWidth);
            gui.SetElementBackgroundColor(hHorizontalContainer, theme.scrollbarBackgroundColor);
            gui.AttachLocalEventHandler(hHorizontalContainer, m_deadSpaceSizingHandler);
        }
    }


    HGUIElement EditorScrollableControl::GetContentElement() const
    {
        return this->GetTopLeftQuadrant();
    }
}