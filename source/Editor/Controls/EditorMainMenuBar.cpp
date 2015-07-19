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
            GUIContext &gui = editor.GetGUI();
            const EditorTheme &theme = editor.GetTheme();

            m_pFileMenuButton = this->CreateAndInsertButton("FILE");
            this->CreateAndInsertButton("EDIT");
            this->CreateAndInsertButton("VIEW");
            this->CreateAndInsertButton("HELP");


            m_pFileMenu = new EditorPopupControl(editor, hParentWindow);
            gui.SetElementBackgroundColor(m_pFileMenu->GetRootGUIElement(), theme.backgroundPopup);
            gui.SetElementBorderColor(m_pFileMenu->GetRootGUIElement(), theme.borderDefault);
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


            // Set the border mask.
            GTLib::Rect<int> buttonInnerRect;
            this->GetGUI().GetElementAbsoluteInnerBorderRect(pButton->GetRootGUIElement(), buttonInnerRect);
            this->GetGUI().SetElementBorderTopMaskInPixels(pMenu->GetRootGUIElement(), buttonInnerRect.left - buttonRect.left, buttonInnerRect.GetWidth());


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
            this->GetGUI().GetElementAbsoluteRect(pActiveButton->GetRootGUIElement(), buttonRect);
            
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
