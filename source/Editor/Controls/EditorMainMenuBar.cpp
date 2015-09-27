// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMainMenuBar.hpp>
#include <GTGameEngine/Editor/Controls/EditorMenuBarButton.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/GameContext.hpp>

namespace GT
{
    EditorMainMenuBar::EditorMainMenuBar(Editor &editor, HWindow hParentWindow)
        : EditorMenuBar(editor),
          m_pFileMenuButton(nullptr), m_pFileMenu(nullptr)
    {
        if (m_hRootElement != NULL)
        {
            //const EditorTheme &theme = editor.GetTheme();

            m_pFileMenuButton = this->CreateAndInsertButton("FILE");
            m_pEditMenuButton = this->CreateAndInsertButton("EDIT");
            m_pHelpMenuButton = this->CreateAndInsertButton("HELP");


            m_pFileMenu = new EditorFileMenu(editor, hParentWindow);
            m_pFileMenu->OnClose([&]() {
                this->DeactivateActiveButton();
            });

            m_pEditMenu = new EditorEditMenu(editor, hParentWindow);
            m_pEditMenu->OnClose([&]() {
                this->DeactivateActiveButton();
            });

            m_pHelpMenu = new EditorHelpMenu(editor, hParentWindow);
            m_pHelpMenu->OnClose([&]() {
                this->DeactivateActiveButton();
            });
        }
    }

    EditorMainMenuBar::~EditorMainMenuBar()
    {
        this->DeleteButton(m_pFileMenuButton);
        this->DeleteButton(m_pEditMenuButton);
        this->DeleteButton(m_pHelpMenuButton);

        delete m_pFileMenu;
        delete m_pEditMenu;
        delete m_pHelpMenu;
    }


    void EditorMainMenuBar::OnButtonActivated(EditorMenuBarButton* pButton)
    {
        auto pMenu = this->GetButtonMenu(pButton);
        if (pMenu != nullptr)
        {
            GTLib::Rect<int> buttonRect;
            this->GetGUI().GetElementAbsoluteRect(pButton->GetRootElement(), buttonRect);


            // Set the border mask.
            GTLib::Rect<int> buttonInnerRect;
            this->GetGUI().GetElementAbsoluteInnerBorderRect(pButton->GetRootElement(), buttonInnerRect);
            this->GetGUI().SetElementBorderTopMaskInPixels(pMenu->GetRootElement(), buttonInnerRect.left - buttonRect.left, buttonInnerRect.GetWidth(), this->GetEditor().GetTheme().backgroundPopup);


            // Show the menu.
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

    void EditorMainMenuBar::_OnMouseButtonPressedOnWindow(HWindow hWindow, int button, int xPos, int yPos)
    {
        (void)button;

        EditorMenuBarButton* pActiveButton = this->GetActiveButton();
        if (pActiveButton != nullptr)
        {
            // Don't do anything if we clicked on the active window. We do this because the GUI event handling will handle the button presses.
            GTLib::Rect<int> buttonRect;
            this->GetGUI().GetElementAbsoluteRect(pActiveButton->GetRootElement(), buttonRect);
            
            if (!buttonRect.Contains(xPos, yPos))
            {
                EditorPopupControl* pActiveMenu = this->GetActiveMenu();
                if (pActiveMenu != nullptr)
                {
                    if (pActiveMenu->GetWindow() != hWindow && !this->GetEditor().IsWindowDescendant(pActiveMenu->GetWindow(), hWindow))
                    {
                        this->DeactivateActiveButton();
                    }
                }
                else
                {
                    this->DeactivateActiveButton();
                }
            }
        }
    }



    ///////////////////////////////////
    // Private

    EditorPopupControl* EditorMainMenuBar::GetButtonMenu(EditorMenuBarButton* pButton) const
    {
        if (pButton == m_pFileMenuButton) {
            return m_pFileMenu;
        }
        if (pButton == m_pEditMenuButton) {
            return m_pEditMenu;
        }
        if (pButton == m_pHelpMenuButton) {
            return m_pHelpMenu;
        }


        return nullptr;
    }

    EditorPopupControl* EditorMainMenuBar::GetActiveMenu() const
    {
        return this->GetButtonMenu(this->GetActiveButton());
    }
}
