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
