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



    void EditorControl::OnMove(GUIContext &, HGUIElement, unsigned int, unsigned int)
    {
    }

    void EditorControl::OnSize(GUIContext &, HGUIElement, int, int)
    {
    }

    void EditorControl::OnMouseEnter(GUIContext &, HGUIElement)
    {
    }

    void EditorControl::OnMouseLeave(GUIContext &, HGUIElement)
    {
    }

    void EditorControl::OnMouseMove(GUIContext &, HGUIElement, int, int)
    {
    }

    void EditorControl::OnMouseButtonPressed(GUIContext &, HGUIElement, int, int, int)
    {
    }

    void EditorControl::OnMouseButtonReleased(GUIContext &, HGUIElement, int, int, int)
    {
    }

    void EditorControl::OnMouseButtonDoubleClicked(GUIContext &, HGUIElement, int, int, int)
    {
    }

    void EditorControl::OnSetMouseEventCapture(GUIContext &, HGUIElement)
    {
    }

    void EditorControl::OnReleaseMouseEventCapture(GUIContext &, HGUIElement)
    {
    }


    EditorControl::ControlGUIEventHandler::ControlGUIEventHandler(EditorControl &control)
        : m_control(control)
    {

    }
}
