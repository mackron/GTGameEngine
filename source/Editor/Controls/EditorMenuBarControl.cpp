// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenuBarControl.hpp>
#include <GTGameEngine/Editor/Controls/EditorMenuBarButtonControl.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    void EditorMenuBarButtonGUIEventHandler::OnMouseEnter(GUIContext &gui, HGUIElement hElement)
    {
        gui.SetElementTextColor(hElement, GTLib::Colour(1.0f, 1.0f, 1.0f, 1.0f));
        //gui.SetElementPadding(hElement, 0);
    }

    void EditorMenuBarButtonGUIEventHandler::OnMouseLeave(GUIContext &gui, HGUIElement hElement)
    {
        gui.SetElementTextColor(hElement, GTLib::Colour(0.75f, 0.75f, 0.75f, 1.0f));
        //gui.SetElementPaddingLeft(hElement, 8);
        //gui.SetElementPaddingRight(hElement, 8);
    }

    void EditorMenuBarButtonGUIEventHandler::OnMouseButtonPressed(GUIContext &gui, HGUIElement hElement, int, int, int)
    {
        gui.SetElementTextColor(hElement, GTLib::Colour(0.5f, 0.5f, 0.5f, 1.0f));
    }

    void EditorMenuBarButtonGUIEventHandler::OnMouseButtonReleased(GUIContext &gui, HGUIElement hElement, int, int, int)
    {
        if (gui.IsElementUnderMouse(hElement))
        {
            gui.SetElementTextColor(hElement, GTLib::Colour(1.0f, 1.0f, 1.0f, 1.0f));
        }
        else
        {
            gui.SetElementTextColor(hElement, GTLib::Colour(0.75f, 0.75f, 0.75f, 1.0f));
        }
    }




    EditorMenuBar::EditorMenuBar(Editor &editor)
        : EditorControl(editor),
          m_buttons(),
          m_buttonEventHandler()
    {
        GUIContext &gui = editor.GetGUI();

        gui.SetElementChildAxis(this->GetRootGUIElement(), ChildAxis_Horizontal);
        gui.SetElementWidthToChildren(this->GetRootGUIElement());
        gui.SetElementHeightToChildren(this->GetRootGUIElement());
        gui.SetElementFont(this->GetRootGUIElement(), "Segoe UI", FontWeight_Medium, FontSlant_None, 13);
    }

    EditorMenuBar::~EditorMenuBar()
    {
    }


    EditorMenuBarButton* EditorMenuBar::CreateAndInsertButton(const char* buttonText)
    {
        auto pButton = new EditorMenuBarButton(this->GetEditor(), buttonText);
        this->GetGUI().SetElementParent(pButton->GetRootGUIElement(), this->GetRootGUIElement());
        this->GetGUI().SetElementSizeToChildren(pButton->GetRootGUIElement());

        this->GetGUI().AttachLocalEventHandler(pButton->GetRootGUIElement(), m_buttonEventHandler);

        m_buttons.PushBack(pButton);

        return pButton;
    }

    void EditorMenuBar::DeleteButton(EditorMenuBarButton* pButton)
    {
        if (pButton != nullptr)
        {
            size_t index;
            if (m_buttons.FindFirstIndexOf(pButton, index))
            {
                m_buttons.Remove(index);
            }

            delete pButton;
        }
    }
}
