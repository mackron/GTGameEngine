// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorTabGroup.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorTabGroup::EditorTabGroup(Editor &editor)
        : EditorControl(editor),
          m_tabBar(editor),
          m_tabPages(),
          m_hTabPageContainer(NULL)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();
            const EditorTheme &theme = editor.GetTheme();

            gui.EnableElementChildHeightFlexing(hRootElement);
            gui.SetElementSizeRatio(hRootElement, 1.0f, 1.0f);
            gui.SetElementChildAxis(hRootElement, ChildAxis_Vertical);

            // Tab bar.
            gui.SetElementParent(m_tabBar.GetRootGUIElement(), hRootElement);

            // Tab page container.
            m_hTabPageContainer = gui.CreateElement();
            gui.SetElementParent(m_hTabPageContainer, hRootElement);
            gui.SetElementSizeRatio(m_hTabPageContainer, 1.0f, 1.0f);
            gui.SetElementBorder(m_hTabPageContainer, 1, GTLib::Colour(0.4f, 0.4f, 0.4f));
            gui.SetElementBorderTopWidth(m_hTabPageContainer, 0);
            gui.SetElementBackgroundColor(m_hTabPageContainer, theme.backgroundMid);


            // Testing.
            m_tabBar.CreateTab("Hello, World!");
            m_tabBar.CreateTab("MainMenu.gtscene");
        }
    }
}
