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

            m_pFileMenuButton = this->CreateAndInsertButton("FILE");
            this->CreateAndInsertButton("EDIT");
            this->CreateAndInsertButton("VIEW");
            this->CreateAndInsertButton("HELP");


            m_pFileMenu = new EditorPopupControl(editor, hParentWindow);
            gui.SetElementBorderTopMask(m_pFileMenu->GetRootGUIElement(), 1, static_cast<uint32_t>(gui.GetElementWidthInPoints(m_pFileMenuButton->GetRootGUIElement()) - 2));
            gui.SetElementBackgroundColor(m_pFileMenu->GetRootGUIElement(), GTLib::Colour(0.2f, 0.2f, 0.2f, 1.0f));
            gui.SetElementBorderColor(m_pFileMenu->GetRootGUIElement(), GTLib::Colour(0.3f, 0.3f, 0.3f, 1.0f));
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

    void EditorMainMenuBar::_OnMouseButtonPressedOnWindow(HWindow hWindow)
    {
        EditorMenuBarButton* pActiveButton = this->GetActiveButton();
        if (pActiveButton != nullptr)
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
