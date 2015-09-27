// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorFooter.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorFooter::EditorFooter(Editor &editor)
        : EditorControl(editor),
          m_hCommandTextBox(NULL)
    {
        GUIContext &gui = editor.GetGUI();
        const EditorTheme &theme = editor.GetTheme();

        HGUIElement hRootElement = this->GetRootElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeight(hRootElement, 28U);
            gui.SetElementWidthRatio(hRootElement, 1.0f);
            gui.SetElementVerticalAlign(hRootElement, VerticalAlign_Center);
            gui.SetElementChildAxis(hRootElement, ChildAxis_Horizontal);
            gui.SetElementPadding(hRootElement, 2);
            gui.SetElementFont(hRootElement, theme.defaultFontFamily, FontWeight_Medium, FontSlant_None, 12);

            //gui.SetElementBorderTop(hRootElement, 1, GT::Color(0.2f, 0.2f, 0.2f, 1.0f));


            // The left side container.
            m_hLeftContainer = gui.CreateElement();
            gui.SetElementParent(m_hLeftContainer, hRootElement);
            gui.SetElementChildAxis(m_hLeftContainer, ChildAxis_Horizontal);
            gui.SetElementHorizontalAlign(m_hLeftContainer, HorizontalAlign_Left);
            gui.SetElementVerticalAlign(m_hLeftContainer, VerticalAlign_Center);
            gui.SetElementSizeRatio(m_hLeftContainer, 0.5f, 1.0f);
            gui.EnableElementChildWidthFlexing(m_hLeftContainer);


            // The right side container.
            m_hRightContainer = gui.CreateElement();
            gui.SetElementParent(m_hRightContainer, hRootElement);
            gui.SetElementChildAxis(m_hRightContainer, ChildAxis_Horizontal);
            gui.SetElementHorizontalAlign(m_hRightContainer, HorizontalAlign_Right);
            gui.SetElementVerticalAlign(m_hRightContainer, VerticalAlign_Center);
            gui.SetElementSizeRatio(m_hRightContainer, 0.5f, 1.0f);
            gui.EnableElementChildWidthFlexing(m_hRightContainer);


            // Command label.
            m_hCommandLabel = gui.CreateElement();
            gui.SetElementParent(m_hCommandLabel, m_hLeftContainer);
            gui.SetElementSizeToChildren(m_hCommandLabel);
            gui.SetElementText(m_hCommandLabel, "Command:");
            gui.SetElementTextColor(m_hCommandLabel, theme.defaultTextColor);
            gui.SetElementPaddingRight(m_hCommandLabel, 4);
            gui.SetElementPaddingLeft(m_hCommandLabel, 2);

            // Command text box.
            m_hCommandTextBox = gui.CreateElement();
            gui.SetElementParent(m_hCommandTextBox, m_hLeftContainer);
            gui.SetElementHeightRatio(m_hCommandTextBox, 1);
            gui.SetElementWidthRatio(m_hCommandTextBox, 1);
            gui.SetElementBorder(m_hCommandTextBox, 1, GT::Color(0.4f, 0.4f, 0.4f));
            gui.SetElementBackgroundColor(m_hCommandTextBox, GT::Color(0.25f, 0.25f, 0.25f));
            gui.SetElementPadding(m_hCommandTextBox, 2);
            gui.SetElementVerticalAlign(m_hCommandTextBox, VerticalAlign_Center);
            gui.SetElementCursor(m_hCommandTextBox, GUISystemCursor::IBeam);
            gui.EnableEditableText(m_hCommandTextBox);
            gui.EnableFocusOnMouseClick(m_hCommandTextBox);
            gui.SetElementFont(m_hCommandTextBox, "Courier New", FontWeight_Medium, FontSlant_None, 15);
            gui.SetElementTextColor(m_hCommandTextBox, theme.defaultTextColor);
        }
    }

    EditorFooter::~EditorFooter()
    {

    }
}
