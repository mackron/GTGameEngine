// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorControl.hpp>

namespace GT
{
    EditorControl::EditorControl(GUIContext &gui)
        : m_gui(gui),
          m_hRootElement(NULL)
    {
        m_hRootElement = m_gui.CreateElement();
    }

    EditorControl::~EditorControl()
    {
    }


    GUIContext & EditorControl::GetGUI()
    {
        return m_gui;
    }

    const GUIContext & EditorControl::GetGUI() const
    {
        return m_gui;
    }


    HGUIElement EditorControl::GetRootGUIElement() const
    {
        return m_hRootElement;
    }
}
