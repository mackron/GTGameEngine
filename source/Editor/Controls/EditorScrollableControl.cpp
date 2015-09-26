// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorAssetExplorer.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorScrollableControl::EditorScrollableControl(Editor &editor)
        : EditorQuadControl(editor),
          m_verticalScrollbar(editor.GetGUI(), 0, ScrollbarOrientation::Vertical),
          m_deadSpaceSizingHandler(this->GetBottomRightQuadrant(), this->GetTopRightQuadrant(), this->GetBottomLeftQuadrant())
    {
        HGUIElement hRootElement = this->GetRootElement();
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

            // Vertical scrollbar.
            gui.SetElementParent(m_verticalScrollbar, hVerticalContainer);
            gui.SetElementWidth(m_verticalScrollbar, 16U);
            gui.SetElementHeightRatio(m_verticalScrollbar, 1);
            m_verticalScrollbar.SetRange(0, 2);
            m_verticalScrollbar.SetPageSize(1);
            m_verticalScrollbar.SetScrollPosition(0);


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



    EditorScrollableControl::DeadSpaceSizingEventHandler::DeadSpaceSizingEventHandler(HGUIElement hDeadSpace, HGUIElement hVerticalNeighbour, HGUIElement hHorizontalNeighbour)
        : m_hDeadSpace(hDeadSpace), m_hVerticalNeighbour(hVerticalNeighbour), m_hHorizontalNeighbour(hHorizontalNeighbour)
    {
    }

    void EditorScrollableControl::DeadSpaceSizingEventHandler::OnSize(GUIContext &context, HGUIElement hElement, unsigned int width, unsigned int height)
    {
        if (hElement == m_hVerticalNeighbour)
        {
            context.SetElementWidth(m_hDeadSpace, context.PixelsToPointsX(hElement, static_cast<int>(width)));
        }
        else if (hElement == m_hHorizontalNeighbour)
        {
            context.SetElementHeight(m_hDeadSpace, context.PixelsToPointsY(hElement, static_cast<int>(height)));
        }
    }
}
