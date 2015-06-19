// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenuBarControl.hpp>
#include <GTGameEngine/Editor/Controls/EditorMenuBarButtonControl.hpp>

namespace GT
{
    void EditorMenuBarButtonControlGUIEventHandler::OnMouseEnter(GUIContext &gui, HGUIElement hElement)
    {
        gui.SetElementTextColor(hElement, GTLib::Colour(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void EditorMenuBarButtonControlGUIEventHandler::OnMouseLeave(GUIContext &gui, HGUIElement hElement)
    {
        gui.SetElementTextColor(hElement, GTLib::Colour(0.75f, 0.75f, 0.75f, 1.0f));
    }

    void EditorMenuBarButtonControlGUIEventHandler::OnMouseButtonPressed(GUIContext &gui, HGUIElement hElement, int, int, int)
    {
        gui.SetElementTextColor(hElement, GTLib::Colour(0.5f, 0.5f, 0.5f, 1.0f));
    }

    void EditorMenuBarButtonControlGUIEventHandler::OnMouseButtonReleased(GUIContext &gui, HGUIElement hElement, int, int, int)
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




    EditorMenuBarControl::EditorMenuBarControl(GUIContext &gui)
        : EditorControl(gui),
          m_buttons(),
          m_buttonEventHandler()
    {
        gui.SetElementChildAxis(this->GetRootGUIElement(), ChildAxis_Horizontal);
        gui.SetElementWidthToChildren(this->GetRootGUIElement());
        gui.SetElementHeightToChildren(this->GetRootGUIElement());
        gui.SetElementFont(this->GetRootGUIElement(), "Segoe UI", FontWeight_Medium, FontSlant_None, 13);
    }

    EditorMenuBarControl::~EditorMenuBarControl()
    {
    }


    EditorMenuBarButtonControl* EditorMenuBarControl::CreateAndInsertButton(const char* buttonText)
    {
        auto pButton = new EditorMenuBarButtonControl(this->GetGUI(), buttonText);
        this->GetGUI().SetElementParent(pButton->GetRootGUIElement(), this->GetRootGUIElement());
        this->GetGUI().SetElementSizeToChildren(pButton->GetRootGUIElement());

        this->GetGUI().AttachLocalEventHandler(pButton->GetRootGUIElement(), m_buttonEventHandler);

        m_buttons.PushBack(pButton);

        return pButton;
    }

    void EditorMenuBarControl::DeleteButton(EditorMenuBarButtonControl* pButton)
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
