// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenuBarButton.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorMenuBarButton::EditorMenuBarButton(Editor &editor, const char* text)
        : EditorControl(editor),
          m_flags(0)
    {
        HGUIElement hElement = this->GetRootGUIElement();
        if (hElement != 0)
        {
            GUIContext &gui = editor.GetGUI();

            gui.SetElementPaddingLeft(hElement, 8);
            gui.SetElementPaddingRight(hElement, 8);
            gui.SetElementPaddingTop(hElement, 1);
            gui.SetElementPaddingBottom(hElement, 2);
            gui.SetElementText(hElement, text);
            gui.SetElementTextColor(hElement, GTLib::Colour(0.75f, 0.75f, 0.75f, 1.0f));
        }
    }

    EditorMenuBarButton::~EditorMenuBarButton()
    {
    }


    void EditorMenuBarButton::Activate()
    {
        if (!this->IsActivated())
        {
            this->ShowBorder();

            m_flags |= Flag_IsActivated;
        }
    }

    void EditorMenuBarButton::Deactivate()
    {
        if (this->IsActivated())
        {
            this->HideBorder();

            m_flags &= ~Flag_IsActivated;
        }
    }

    bool EditorMenuBarButton::IsActivated() const
    {
        return (m_flags & Flag_IsActivated) != 0;
    }





    /////////////////////////////////
    // Private

    void EditorMenuBarButton::ShowBorder()
    {
        HGUIElement hElement = this->GetRootGUIElement();
        if (hElement != NULL)
        {
            GUIContext &gui = this->GetGUI();

            uint32_t paddingLeft   = gui.GetElementPaddingLeft(hElement);
            uint32_t paddingTop    = gui.GetElementPaddingTop(hElement);
            uint32_t paddingRight  = gui.GetElementPaddingRight(hElement);
            uint32_t paddingBottom = gui.GetElementPaddingBottom(hElement);

            uint32_t borderWidth = 1;
            GTLib::Colour borderColor(0.4f, 0.4f, 0.4f, 1.0f);

            paddingLeft   = ((paddingLeft   > borderWidth) ? (paddingLeft   - borderWidth) : (0));
            paddingTop    = ((paddingTop    > borderWidth) ? (paddingTop    - borderWidth) : (0));
            paddingRight  = ((paddingRight  > borderWidth) ? (paddingRight  - borderWidth) : (0));
            paddingBottom = ((paddingBottom > borderWidth) ? (paddingBottom - borderWidth) : (0));


            // Adjust the padding to compensate for the border.
            gui.BeginBatch();
            {
                gui.SetElementPaddingLeft(hElement, paddingLeft);
                gui.SetElementPaddingTop(hElement, paddingTop);
                gui.SetElementPaddingRight(hElement, paddingRight);
                gui.SetElementPaddingBottom(hElement, paddingBottom);

                // Set the border.
                gui.SetElementBorder(hElement, borderWidth, borderColor);
                gui.SetElementBorderBottomWidth(hElement, 0);
            }
            gui.EndBatch();
        }
    }

    void EditorMenuBarButton::HideBorder()
    {
        HGUIElement hElement = this->GetRootGUIElement();
        if (hElement != NULL)
        {
            GUIContext &gui = this->GetGUI();


            // Set the padding to compensate.
            uint32_t paddingLeft   = gui.GetElementPaddingLeft(hElement);
            uint32_t paddingTop    = gui.GetElementPaddingTop(hElement);
            uint32_t paddingRight  = gui.GetElementPaddingRight(hElement);
            uint32_t paddingBottom = gui.GetElementPaddingBottom(hElement);

            paddingLeft   += 1;
            paddingTop    += 1;
            paddingRight  += 1;
            paddingBottom += 1;

            gui.BeginBatch();
            {
                gui.SetElementPaddingLeft(hElement, paddingLeft);
                gui.SetElementPaddingTop(hElement, paddingTop);
                gui.SetElementPaddingRight(hElement, paddingRight);
                gui.SetElementPaddingBottom(hElement, paddingBottom);

                // Just set the border width to 0.
                gui.SetElementBorderWidth(hElement, 0);
            }
            gui.EndBatch();
        }
    }
}
