// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorQuadControl.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorQuadControl::EditorQuadControl(Editor &editor)
        : EditorControl(editor)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();

            // Root element.
            gui.SetElementChildAxis(hRootElement, ChildAxis_Vertical);
            gui.EnableElementChildHeightFlexing(hRootElement);

            
            // Top row.
            m_hTopContainer = gui.CreateElement();
            gui.SetElementParent(m_hTopContainer, hRootElement);
            gui.SetElementChildAxis(m_hTopContainer, ChildAxis_Horizontal);
            gui.SetElementSizeRatio(m_hTopContainer, 1.0f, 0.5f);
            gui.EnableElementChildWidthFlexing(m_hTopContainer);

            m_hTopLeft = gui.CreateElement();
            gui.SetElementParent(m_hTopLeft, m_hTopContainer);
            gui.SetElementSizeRatio(m_hTopLeft, 0.5f, 1.0f);

            m_hTopRight = gui.CreateElement();
            gui.SetElementParent(m_hTopRight, m_hTopContainer);
            gui.SetElementSizeRatio(m_hTopRight, 0.5f, 1.0f);


            // Bottom row.
            m_hBottomContainer = gui.CreateElement();
            gui.SetElementParent(m_hBottomContainer, hRootElement);
            gui.SetElementChildAxis(m_hBottomContainer, ChildAxis_Horizontal);
            gui.SetElementSizeRatio(m_hBottomContainer, 1.0f, 0.5f);
            gui.EnableElementChildWidthFlexing(m_hBottomContainer);

            m_hBottomLeft = gui.CreateElement();
            gui.SetElementParent(m_hBottomLeft, m_hBottomContainer);
            gui.SetElementSizeRatio(m_hBottomLeft, 0.5f, 1.0f);

            m_hBottomRight = gui.CreateElement();
            gui.SetElementParent(m_hBottomRight, m_hBottomContainer);
            gui.SetElementSizeRatio(m_hBottomRight, 0.5f, 1.0f);
        }
    }


    HGUIElement EditorQuadControl::GetTop()
    {
        return m_hTopContainer;
    }

    HGUIElement EditorQuadControl::GetBottom()
    {
        return m_hBottomContainer;
    }


    HGUIElement EditorQuadControl::GetTopLeftQuadrant() const
    {
        return m_hTopLeft;
    }

    HGUIElement EditorQuadControl::GetTopRightQuadrant() const
    {
        return m_hTopRight;
    }

    HGUIElement EditorQuadControl::GetBottomLeftQuadrant() const
    {
        return m_hBottomLeft;
    }

    HGUIElement EditorQuadControl::GetBottomRightQuadrant() const
    {
        return m_hBottomRight;
    }
}