// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMainMenuBar.hpp>
#include <GTGameEngine/Editor/Controls/EditorMenuBarButton.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorMainMenuBar::EditorMainMenuBar(Editor &editor, HWindow hParentWindow)
        : EditorMenuBar(editor),
          m_pFileMenuButton(nullptr), m_pFileMenu(nullptr)
    {
        HGUIElement hElement = this->GetRootGUIElement();
        if (hElement != NULL)
        {
            m_pFileMenuButton = this->CreateAndInsertButton("FILE");
            this->CreateAndInsertButton("EDIT");
            this->CreateAndInsertButton("VIEW");
            this->CreateAndInsertButton("HELP");


            m_pFileMenu = new EditorPopupControl(editor, hParentWindow);
        }
    }

    EditorMainMenuBar::~EditorMainMenuBar()
    {
    }


    void EditorMainMenuBar::OnButtonActivated(EditorMenuBarButton* pButton)
    {
        auto pMenu = this->GetButtonMenu(pButton);
        if (pMenu != nullptr)
        {
            GTLib::Rect<int> buttonRect;
            this->GetGUI().GetElementAbsoluteRect(pButton->GetRootGUIElement(), buttonRect);

            pMenu->SetPosition(buttonRect.left, buttonRect.bottom);
            pMenu->Show();
        }
    }

    void EditorMainMenuBar::OnButtonDeactivated(EditorMenuBarButton* pButton)
    {
        auto pMenu = this->GetButtonMenu(pButton);
        if (pMenu != nullptr)
        {
            pMenu->Hide();
        }
    }



    //////////////////////////////////
    // Inbound Events

    void EditorMainMenuBar::_OnGlobalMouseButtonPressed(HGUIElement hElement)
    {
        // If the element is that of the currently active button, don't close anything.
        EditorMenuBarButton* pActiveButton = this->GetActiveButton();
        if (pActiveButton != nullptr)
        {
            if (pActiveButton->GetRootGUIElement() != hElement)
            {
                EditorPopupControl* pActiveMenu = this->GetActiveMenu();
                if (pActiveMenu != nullptr)
                {
                    if (pActiveMenu->GetRootGUIElement() != hElement && !this->GetGUI().IsElementDescendant(pActiveMenu->GetRootGUIElement(), hElement))
                    {
                        this->DeactivateActiveButton();
                    }
                    else
                    {
                        // It may be a sub-menu, so we want to check is the window itself is a descendant of the active menu.
                    }
                }
            }
        }
    }



    ///////////////////////////////////
    // Private

    EditorPopupControl* EditorMainMenuBar::GetButtonMenu(EditorMenuBarButton* pButton) const
    {
        if (pButton == m_pFileMenuButton)
        {
            return m_pFileMenu;
        }


        return nullptr;
    }

    EditorPopupControl* EditorMainMenuBar::GetActiveMenu() const
    {
        return this->GetButtonMenu(this->GetActiveButton());
    }
}
