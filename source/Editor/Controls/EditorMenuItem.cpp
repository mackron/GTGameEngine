// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenuItem.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorMenuItem::EditorMenuItem(Editor &editor, HGUIElement hParent, const char* text, const char* shortcut, bool showArrow)
        : EditorControl(editor),
          m_onPressed(nullptr)
    {
        m_gui.SetElementParent(m_hRootElement, hParent);
        m_gui.SetElementChildAxis(m_hRootElement, ChildAxis::ChildAxis_Horizontal);
        //m_gui.EnableElementChildWidthFlexing(m_hRootElement);
        m_gui.SetElementHeightToChildren(m_hRootElement);
        m_gui.SetElementWidthToChildren(m_hRootElement);
        m_gui.SetElementMinWidthRatio(m_hRootElement, 1);
        //m_gui.SetElementWidthRatio(m_hRootElement, 1);
        m_gui.SetElementPadding(m_hRootElement, 2);
        m_gui.SetElementFont(m_hRootElement, m_editor.GetTheme().defaultFontFamily, FontWeight_Normal, FontSlant_None, 13);

        m_gui.OnElementMouseEnter(m_hRootElement, [&]() {
            m_gui.SetElementBackgroundColor(m_hRootElement, GT::Color(0.4f, 0.4f, 0.4f));
        });
        m_gui.OnElementMouseLeave(m_hRootElement, [&]() {
            m_gui.SetElementBackgroundColor(m_hRootElement, GT::Color(0, 0, 0, 0));
        });
        m_gui.OnElementMouseButtonPressed(m_hRootElement, [&](int button, int, int) {
            if (button == MouseButton_Left) {
                if (m_onPressed != nullptr) {
                    m_onPressed();
                }
            }
        });



        m_hIcon = m_gui.CreateElement();
        m_gui.SetElementParent(m_hIcon, m_hRootElement);
        m_gui.EnableCursorPassThrough(m_hIcon);
        m_gui.SetElementWidth(m_hIcon, 28U);
        m_gui.SetElementPadding(m_hIcon, 2);

        m_hText = m_gui.CreateElement();
        m_gui.SetElementParent(m_hText, m_hRootElement);
        m_gui.EnableCursorPassThrough(m_hText);
        //m_gui.SetElementWidthRatio(m_hText, 1);
        m_gui.SetElementWidthToChildren(m_hText);
        m_gui.SetElementHeightToChildren(m_hText);
        m_gui.SetElementPaddingRight(m_hText, 32);
        m_gui.SetElementText(m_hText, text);
        m_gui.SetElementTextColor(m_hText, m_editor.GetTheme().defaultTextColor);

        m_hShortcut = m_gui.CreateElement();
        m_gui.SetElementParent(m_hShortcut, m_hRootElement);
        m_gui.EnableCursorPassThrough(m_hShortcut);
        m_gui.SetElementWidthToChildren(m_hShortcut);
        m_gui.SetElementHeightToChildren(m_hShortcut);
        //m_gui.SetElementPaddingRight(m_hShortcut, 16);
        m_gui.SetElementText(m_hShortcut, shortcut);
        m_gui.SetElementTextColor(m_hShortcut, m_editor.GetTheme().defaultTextColor);


        m_hArrow = m_gui.CreateElement();
        m_gui.SetElementParent(m_hArrow, m_hRootElement);
        m_gui.EnableCursorPassThrough(m_hArrow);
        m_gui.SetElementWidth(m_hArrow, 16U);
        m_gui.SetElementHeightToChildren(m_hArrow);
        m_gui.SetElementPadding(m_hArrow, 2);
        
        if (showArrow) {
            m_gui.SetElementTextColor(m_hArrow, m_editor.GetTheme().defaultTextColor);
            m_gui.SetElementHorizontalAlign(m_hArrow, HorizontalAlign_Right);
            m_gui.SetElementFont(m_hArrow, m_editor.GetTheme().defaultFontFamily, FontWeight_Normal, FontSlant_None, 8);
            m_gui.SetElementText(m_hArrow, "\xE2\x96\xB6");
        }
    }

    EditorMenuItem::~EditorMenuItem()
    {
    }


    void EditorMenuItem::OnPressed(OnPressedProc proc)
    {
        m_onPressed = proc;
    }
}
