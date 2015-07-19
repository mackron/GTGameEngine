// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorTab.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorTab::EditorTab(Editor &editor)
        : EditorControl(editor),
          m_hTextElement(NULL),
          m_hCloseElement(NULL)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();
            const EditorTheme &theme = editor.GetTheme();

            //gui.EnableElementChildWidthFlexing(hRootElement);
            gui.SetElementChildAxis(hRootElement, ChildAxis_Horizontal);
            gui.SetElementSizeToChildren(hRootElement);
            gui.SetElementBorder(hRootElement, 1, theme.borderDefault);
            gui.SetElementBorderBottomWidth(hRootElement, 0);
            
            //gui.SetElementSize(hRootElement, 200U, 28U);
            //gui.SetElementFont(hRootElement, "Segoe UI", FontWeight_Medium, FontSlant_None, 13);


            // Text element.
            m_hTextElement = gui.CreateElement();
            gui.SetElementParent(m_hTextElement, hRootElement);
            gui.SetElementSizeToChildren(m_hTextElement);
            gui.SetElementVerticalAlign(m_hTextElement, VerticalAlign_Center);
            gui.SetElementTextColor(m_hTextElement, GTLib::Colour(0.8f, 0.8f, 0.8f));
            gui.SetElementPadding(m_hTextElement, 4);
            gui.SetElementPaddingTop(m_hTextElement, 4);
            gui.SetElementPaddingBottom(m_hTextElement, 2);
            gui.EnableCursorPassThrough(m_hTextElement);
            //gui.SetElementBorder(m_hTextElement, 1, GTLib::Colour(0, 0, 0));
            
            

            // Close button element.
            m_hCloseElement = gui.CreateElement();
            gui.SetElementParent(m_hCloseElement, hRootElement);
            gui.SetElementFont(m_hCloseElement, "Arial", FontWeight_Bold, FontSlant_None, 15);
            gui.SetElementText(m_hCloseElement, "\xC3\x97");    // <-- Multiplication sign - U+00D7
            gui.SetElementVerticalAlign(m_hCloseElement, VerticalAlign_Center);
            gui.SetElementHorizontalAlign(m_hCloseElement, HorizontalAlign_Center);
            gui.SetElementTextColor(m_hCloseElement, GTLib::Colour(0.35f, 0.35f, 0.35f));
            gui.SetElementSizeToChildren(m_hCloseElement);
            gui.SetElementMarginLeft(m_hCloseElement, 4);
            gui.SetElementMarginRight(m_hCloseElement, 4);
            //gui.SetElementInnerOffsetY(m_hCloseElement, -2);
            //gui.SetElementBorder(m_hCloseElement, 1, GTLib::Colour(1, 1, 1));


            // Apply the default style to begin with.
            this->ApplyDefaultStyle();
        }
    }


    void EditorTab::SetText(const char* text)
    {
        this->GetGUI().SetElementText(m_hTextElement, text);
    }

    const char* EditorTab::GetText() const
    {
        return this->GetGUI().GetElementText(m_hTextElement);
    }



    void EditorTab::ApplyDefaultStyle()
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = this->GetGUI();
            const EditorTheme &theme = this->GetEditor().GetTheme();

            // Hide the border, and compensate with padding.
            gui.BeginBatch();
            {
                gui.SetElementPaddingLeft(hRootElement, 1);
                gui.SetElementPaddingRight(hRootElement, 1);
                gui.SetElementPaddingTop(hRootElement, 1);
                gui.SetElementPaddingBottom(hRootElement, 0);
                gui.SetElementBorderWidth(hRootElement, 0U);

                // Background.
                gui.SetElementBackgroundColor(hRootElement, GTLib::Colour(0, 0, 0, 0));

                // Text.
                gui.SetElementTextColor(m_hTextElement, GTLib::Colour(0.75f, 0.75f, 0.75f));


                // Close button.
                gui.SetElementTextColor(m_hCloseElement, theme.backgroundDark);
            }
            gui.EndBatch();
        }
    }

    void EditorTab::ApplyHoveredStyle()
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = this->GetGUI();
            //const EditorTheme &theme = this->GetEditor().GetTheme();

            // Hide the border, and compensate with padding.
            gui.BeginBatch();
            {
                gui.SetElementPaddingLeft(hRootElement, 1);
                gui.SetElementPaddingRight(hRootElement, 1);
                gui.SetElementPaddingTop(hRootElement, 1);
                gui.SetElementPaddingBottom(hRootElement, 0);

                //gui.SetElementBorderWidth(hRootElement, 1U);
                //gui.SetElementBorderBottomWidth(hRootElement, 0U);

                // Background.
                gui.SetElementBackgroundColor(hRootElement, GTLib::Colour(0, 0, 0, 0));

                // Text
                gui.SetElementTextColor(m_hTextElement, GTLib::Colour(0.9f, 0.9f, 0.9f));


                // Close button.
                gui.SetElementTextColor(m_hCloseElement, GTLib::Colour(0.4f, 0.4f, 0.4f));
            }
            gui.EndBatch();
        }
    }

    void EditorTab::ApplyActivatedStyle()
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = this->GetGUI();
            const EditorTheme &theme = this->GetEditor().GetTheme();

            // Hide the border, and compensate with padding.
            gui.BeginBatch();
            {
                gui.SetElementPaddingLeft(hRootElement, 0);
                gui.SetElementPaddingRight(hRootElement, 0);
                gui.SetElementPaddingTop(hRootElement, 0);
                gui.SetElementPaddingBottom(hRootElement, 0);

                // Border.
                gui.SetElementBorderWidth(hRootElement, 1U);
                gui.SetElementBorderBottomWidth(hRootElement, 0U);

                // Background.
                gui.SetElementBackgroundColor(hRootElement, theme.backgroundMid);

                // Text
                gui.SetElementTextColor(m_hTextElement, GTLib::Colour(0.9f, 0.9f, 0.9f));

                // Close button.
                gui.SetElementTextColor(m_hCloseElement, GTLib::Colour(0.4f, 0.4f, 0.4f));
            }
            gui.EndBatch();
        }
    }
}
