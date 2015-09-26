// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorPopupControl.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorPopupControl::EditorPopupControl(Editor &editor, HWindow hParentWindow)
        : EditorControl(editor),
          m_hWindow(0)
    {
        HGUIElement hElement = this->GetRootElement();
        if (hElement != 0)
        {
            editor.GetGUI().SetElementBorder(hElement, 1, GTLib::Colour(0.4f, 0.4f, 0.4f, 1.0f));

            m_hWindow = editor.CreateWindow(hParentWindow, WindowType::PopupWindow, 0, 0, 300, 400, hElement);
        }
    }

    EditorPopupControl::~EditorPopupControl()
    {
        this->GetEditor().DeleteWindow(m_hWindow);
    }


    HWindow EditorPopupControl::GetWindow() const
    {
        return m_hWindow;
    }


    void EditorPopupControl::Show()
    {
        this->GetEditor().ShowWindow(m_hWindow);
    }

    void EditorPopupControl::Hide()
    {
        this->GetEditor().HideWindow(m_hWindow);
    }


    void EditorPopupControl::SetPosition(int xPos, int yPos)
    {
        this->GetEditor().SetWindowPosition(m_hWindow, xPos, yPos);
    }

    void EditorPopupControl::SetSize(unsigned int width, unsigned int height)
    {
        this->GetEditor().SetWindowSize(m_hWindow, width, height);
    }
}
