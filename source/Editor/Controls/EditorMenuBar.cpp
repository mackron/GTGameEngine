// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenuBar.hpp>
#include <GTGameEngine/Editor/Controls/EditorMenuBarButton.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorMenuBarButtonGUIEventHandler::EditorMenuBarButtonGUIEventHandler(EditorMenuBar* pMenuBar)
        : GUIEventHandler(), m_pMenuBar(pMenuBar)
    {
    }

    void EditorMenuBarButtonGUIEventHandler::OnMouseEnter(GUIContext &, HGUIElement hElement)
    {
        m_pMenuBar->_OnButtonMouseEnter(hElement);
    }

    void EditorMenuBarButtonGUIEventHandler::OnMouseLeave(GUIContext &, HGUIElement hElement)
    {
        m_pMenuBar->_OnButtonMouseLeave(hElement);
    }

    void EditorMenuBarButtonGUIEventHandler::OnMouseButtonPressed(GUIContext &, HGUIElement hElement, int, int, int)
    {
        m_pMenuBar->_OnButtonPressed(hElement);
    }

    void EditorMenuBarButtonGUIEventHandler::OnMouseButtonReleased(GUIContext &, HGUIElement, int, int, int)
    {
#if 0
        if (gui.IsElementUnderMouse(hElement))
        {
            gui.SetElementTextColor(hElement, GTLib::Colour(1.0f, 1.0f, 1.0f, 1.0f));
        }
        else
        {
            gui.SetElementTextColor(hElement, GTLib::Colour(0.75f, 0.75f, 0.75f, 1.0f));
        }
#endif
    }


    EditorMenuBarEditorEventHandler::EditorMenuBarEditorEventHandler(EditorMenuBar* pMenuBar)
        : EditorEventHandler(), m_pMenuBar(pMenuBar)
    {
    }

    void EditorMenuBarEditorEventHandler::OnWindowDeactivated(HWindow)
    {
        m_pMenuBar->DeactivateActiveButton();
    }

    void EditorMenuBarEditorEventHandler::OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos)
    {
        m_pMenuBar->_OnMouseButtonPressedOnWindow(hWindow, button, xPos, yPos);
    }




    EditorMenuBar::EditorMenuBar(Editor &editor)
        : EditorControl(editor),
          m_buttons(),
          m_buttonEventHandler(this),
          m_editorEventHandler(this),
          m_pActiveButton(nullptr)
    {
        GUIContext &gui = editor.GetGUI();
        const EditorTheme &theme = editor.GetTheme();

        gui.SetElementChildAxis(this->GetRootElement(), ChildAxis_Horizontal);
        gui.SetElementWidthToChildren(this->GetRootElement());
        gui.SetElementHeightToChildren(this->GetRootElement());
        gui.SetElementBackgroundColor(this->GetRootElement(), theme.backgroundMid);
        gui.SetElementFont(this->GetRootElement(), "Segoe UI", FontWeight_Medium, FontSlant_None, 13);


        editor.AttachEventHandler(m_editorEventHandler);
    }

    EditorMenuBar::~EditorMenuBar()
    {
        this->DeleteAllButtons();
        this->GetEditor().DetachEventHandler(m_editorEventHandler);
    }


    EditorMenuBarButton* EditorMenuBar::CreateAndInsertButton(const char* buttonText)
    {
        auto pButton = new EditorMenuBarButton(this->GetEditor(), buttonText);
        this->GetGUI().SetElementParent(pButton->GetRootElement(), this->GetRootElement());
        this->GetGUI().SetElementSizeToChildren(pButton->GetRootElement());

        this->GetGUI().AttachLocalEventHandler(pButton->GetRootElement(), m_buttonEventHandler);

        m_buttons.PushBack(pButton);

        return pButton;
    }

    void EditorMenuBar::DeleteButton(EditorMenuBarButton* pButton)
    {
        if (pButton != nullptr)
        {
            if (m_pActiveButton == pButton)
            {
                this->DeactivateActiveButton();
            }


            size_t index;
            if (m_buttons.FindFirstIndexOf(pButton, index))
            {
                m_buttons.Remove(index);
            }

            delete pButton;
        }
    }

    void EditorMenuBar::DeleteAllButtons()
    {
        while (m_buttons.GetCount() > 0)
        {
            this->DeleteButton(m_buttons[0]);
        }
    }


    void EditorMenuBar::ActivateButton(EditorMenuBarButton* pButton)
    {
        if (m_pActiveButton != pButton)
        {
            // Deactivate the previously active button first.
            this->DeactivateActiveButton();


            // Now actiate the new button.
            if (pButton != nullptr)
            {
                this->GetGUI().SetElementTextColor(pButton->GetRootElement(), GTLib::Colour(0.75f, 0.75f, 0.75f, 1.0f));

                pButton->Activate();
                m_pActiveButton = pButton;

                this->OnButtonActivated(m_pActiveButton);
            }
        }
    }

    void EditorMenuBar::DeactivateActiveButton()
    {
        if (m_pActiveButton != nullptr)
        {
            auto pPrevActiveButton = m_pActiveButton;
            {
                m_pActiveButton->Deactivate();
                m_pActiveButton = nullptr;
            }
            this->OnButtonDeactivated(pPrevActiveButton);
        }
    }

    EditorMenuBarButton* EditorMenuBar::GetActiveButton() const
    {
        return m_pActiveButton;
    }


    //////////////////////////////////
    // Events

    void EditorMenuBar::OnButtonActivated(EditorMenuBarButton* pButton)
    {
        (void)pButton;
    }

    void EditorMenuBar::OnButtonDeactivated(EditorMenuBarButton* pButton)
    {
        (void)pButton;
    }



    //////////////////////////////////
    // Inbound Events

    void EditorMenuBar::_OnButtonMouseEnter(HGUIElement hButtonElement)
    {
        auto pActiveButton = this->GetActiveButton();
        if (pActiveButton != nullptr)
        {
            auto pTargetButton = this->GetButtonFromGUIElement(hButtonElement);
            if (pTargetButton != nullptr && pTargetButton != pActiveButton)
            {
                this->DeactivateActiveButton();
                this->ActivateButton(pTargetButton);
            }
        }
        else
        {
            this->GetGUI().SetElementTextColor(hButtonElement, GTLib::Colour(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }

    void EditorMenuBar::_OnButtonMouseLeave(HGUIElement hButtonElement)
    {
        this->GetGUI().SetElementTextColor(hButtonElement, GTLib::Colour(0.75f, 0.75f, 0.75f, 1.0f));
    }

    void EditorMenuBar::_OnButtonPressed(HGUIElement hButtonElement)
    {
        auto pButton = this->GetButtonFromGUIElement(hButtonElement);
        if (pButton != nullptr)
        {
            if (this->GetActiveButton() != pButton)
            {
                this->ActivateButton(pButton);
            }
            else
            {
                this->DeactivateActiveButton();
            }
        }
    }

    void EditorMenuBar::_OnMouseButtonPressedOnWindow(HWindow, int, int, int)
    {
    }




    //////////////////////////////////
    // Private

    EditorMenuBarButton* EditorMenuBar::GetButtonFromGUIElement(HGUIElement hElement) const
    {
        for (size_t iButton = 0; iButton < m_buttons.GetCount(); ++iButton)
        {
            auto pButton = m_buttons[iButton];
            assert(pButton != nullptr);
            {
                if (pButton->GetRootElement() == hElement)
                {
                    return pButton;
                }
            }
        }

        return nullptr;
    }
}
