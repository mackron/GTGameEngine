// Copyright (C) 2015 David Reid. See included LICENCE file.

#include "AssetExplorer.hpp"
#include <GTGE/Editor2/Editor2.hpp>
#include <easy_appkit/ak_gui.h>
#include <easy_appkit/ak_common_controls.h>
#include <easy_appkit/ak_theme.h>
#include <easy_gui/easy_gui.h>

namespace GT
{
    AssetExplorer::AssetExplorer(Editor2 &editor)
        : EditorTool(editor), m_pTV(NULL)
    {
        
    }

    AssetExplorer::~AssetExplorer()
    {
        // Deleting the tree-view control will delete any items that are still attached.
        ak_delete_tree_view(m_pTV);
    }


    bool AssetExplorer::Init(const char* attributes)
    {
        if (!EditorTool::Init(attributes)) {
            return false;
        }

        // For now, have the tree view control also size itself so that it's the same size as the parent.
        easygui_register_on_size(m_pTool, easygui_on_size_fit_children_to_parent);


        // Grab the theme so we can set the font of the tree-view.
        ak_theme* pTheme = m_editor.GetAKTheme();
        
        easygui_font* pTVFont = NULL;
        easygui_color TVColor = easygui_rgb(192, 192, 192);
        if (pTheme != NULL)
        {
            pTVFont = pTheme->pUIFont;
            TVColor = pTheme->uiFontColor;
        }


        // The tree-view control.
        m_pTV = ak_create_tree_view(m_editor.GetGUI(), m_pTool, pTVFont, TVColor);
        if (m_pTV == NULL) {
            return false;
        }


        // Add some testing items.
        ak_tree_view_item* pItem0 = ak_create_tree_view_item(m_pTV, NULL, "Testing", 0, NULL);
        ak_tree_view_item* pItem0_0 = ak_create_tree_view_item(m_pTV, pItem0, "Sub Item 0", 0, NULL);
        ak_tree_view_item* pItem0_1 = ak_create_tree_view_item(m_pTV, pItem0, "Sub Item 1", 0, NULL);


        return true;
    }


    easygui_element* AssetExplorer::GetTreeView()
    {
        return m_pTV;
    }
}