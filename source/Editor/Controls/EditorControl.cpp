// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorControl.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorControl::EditorControl(Editor &editor)
        : m_editor(editor),
          m_hRootElement(NULL),
          m_eventHandler(*this)
    {
        m_hRootElement = editor.GetGUI().CreateElement();
        editor.GetGUI().AttachLocalEventHandler(m_hRootElement, m_eventHandler);
    }

    EditorControl::~EditorControl()
    {
        m_editor.GetGUI().DeleteElement(m_hRootElement);
    }


    Editor & EditorControl::GetEditor()
    {
        return m_editor;
    }

    const Editor & EditorControl::GetEditor() const
    {
        return m_editor;
    }


    GUIContext & EditorControl::GetGUI()
    {
        return m_editor.GetGUI();
    }

    const GUIContext & EditorControl::GetGUI() const
    {
        return m_editor.GetGUI();
    }


    HGUIElement EditorControl::GetRootGUIElement() const
    {
        return m_hRootElement;
    }



    void EditorControl::OnMove(int, int)
    {
    }

    void EditorControl::OnSize(unsigned int, unsigned int)
    {
    }

    void EditorControl::OnMouseEnter()
    {
    }

    void EditorControl::OnMouseLeave()
    {
    }

    void EditorControl::OnMouseMove(int, int)
    {
    }

    void EditorControl::OnMouseButtonPressed(int, int, int)
    {
    }

    void EditorControl::OnMouseButtonReleased(int, int, int)
    {
    }

    void EditorControl::OnMouseButtonDoubleClicked(int, int, int)
    {
    }

    void EditorControl::OnSetMouseEventCapture()
    {
    }

    void EditorControl::OnReleaseMouseEventCapture()
    {
    }


    EditorControl::ControlGUIEventHandler::ControlGUIEventHandler(EditorControl &control)
        : m_control(control)
    {

    }
}
