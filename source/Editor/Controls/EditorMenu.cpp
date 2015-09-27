// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenu.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorMenu::EditorMenu(Editor &editor, HWindow hParentWindow)
        : EditorPopupControl(editor, hParentWindow),
          m_onClose(nullptr)
    {
        m_gui.SetElementPaddingTop(m_hRootElement, 2);
        m_gui.SetElementPaddingBottom(m_hRootElement, 2);

        m_gui.SetElementWidthToChildren(m_hRootElement);
        m_gui.SetElementHeightToChildren(m_hRootElement);
        m_gui.SetElementMinWidth(m_hRootElement, 100U);
        m_gui.SetElementMinHeight(m_hRootElement, 32U);

        // We size the window based on the size of the element.
        m_gui.OnElementSize(m_hRootElement, [&](unsigned int width, unsigned int height) {
            this->SetSize(width, height);
        });
    }

    EditorMenu::~EditorMenu()
    {
    }


    void EditorMenu::Close()
    {
        m_editor.HideWindow(m_hWindow);

        if (m_onClose != nullptr) {
            m_onClose();
        }
    }

    void EditorMenu::OnClose(OnCloseProc proc)
    {
        m_onClose = proc;
    }


    EditorMenuItem* EditorMenu::AppendMenuItem(const char* text, const char* shortcut)
    {
        EditorMenuItem* pMenuItem = new EditorMenuItem(m_editor, m_hRootElement, text, shortcut, false);
        m_menuItems.PushBack(pMenuItem);

        return pMenuItem;
    }

    void EditorMenu::RemoveMenuItem(EditorMenuItem* pMenuItem)
    {
        m_menuItems.RemoveFirstOccuranceOf(pMenuItem);
        delete pMenuItem;
    }


    EditorMenuSeparator* EditorMenu::AppendSeparator()
    {
        EditorMenuSeparator* pMenuSeparator = new EditorMenuSeparator(m_editor, m_hRootElement);
        m_menuSeparators.PushBack(pMenuSeparator);

        return pMenuSeparator;
    }

    void EditorMenu::RemoveSeparator(EditorMenuSeparator* pSeparator)
    {
        m_menuSeparators.RemoveFirstOccuranceOf(pSeparator);
        delete pSeparator;
    }


    void EditorMenu::RefreshMenuItemAlignments()
    {
        if (m_menuItems.GetCount() > 0)
        {
            // We need to find the items that have the widest text and widest shortcut string.
            EditorMenuItem* pItemWithWidestText     = nullptr;
            EditorMenuItem* pItemWithWidestShortcut = nullptr;

            float widestTextSize = 0;
            float widestShortcutSize = 0;


            for (size_t i = 0; i < m_menuItems.GetCount(); ++i)
            {
                EditorMenuItem* pItem = m_menuItems[i];
                assert(pItem != nullptr);

                float textWidth = m_gui.GetElementWidthInPoints(pItem->GetTextElement());
                if (widestTextSize < textWidth) {
                    widestTextSize = textWidth;
                    pItemWithWidestText = pItem;
                }

                float shortcutWidth = m_gui.GetElementWidthInPoints(pItem->GetShortcutElement());
                if (widestShortcutSize < shortcutWidth) {
                    widestShortcutSize = shortcutWidth;
                    pItemWithWidestShortcut = pItem;
                }
            }


            // We have the widest text and shortcut items so now we need to resize all of the other items to match.
            for (size_t i = 0; i < m_menuItems.GetCount(); ++i)
            {
                EditorMenuItem* pItem = m_menuItems[i];
                assert(pItem != nullptr);

                // Text.
                if (pItem != pItemWithWidestText) {
                    m_gui.SetElementWidth(pItem->GetTextElement(), widestTextSize);
                }

                // Shortcut.
                if (pItem != pItemWithWidestShortcut) {
                    m_gui.SetElementWidth(pItem->GetShortcutElement(), widestShortcutSize);
                }
            }
        }
    }
}
