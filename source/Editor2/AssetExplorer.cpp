// Copyright (C) 2015 David Reid. See included LICENCE file.

#include "AssetExplorer.hpp"
#include <GTGE/Editor2/Editor2.hpp>
#include <GTGE/Context.hpp>
#include <dr_appkit/ak_theme.h>
#include <dr_libs/dr_gui.h>
#include <dr_libs/dr_path.h>

#include <stdio.h>  // For testing. Delete me.

namespace GT
{
    static const char* g_PageIconString         = "\xF0\x9F\x93\x84";   // U+1F4C4
    static const char* g_FolderClosedIconString = "\xF0\x9F\x93\x81";   // U+1F4C1
    static const char* g_FolderOpenedIconString = "\xF0\x9F\x93\x82";   // U+1F4C2

    static unsigned int g_PageIconUTF32     = 0x1F4C4;
    static unsigned int g_FolderClosedUTF32 = 0x1F4C1;
    static unsigned int g_FolderOpenedUTF32 = 0x1F4C2;


    static const char* g_TreeView_ArrowFacingRightString = "\xE2\x96\xB6";       // U+25B6
    //static const char* g_TreeView_ArrowFacingDownString  = "\xE2\x96\xBC";       // U+25BC. Straight down.
    static const char* g_TreeView_ArrowFacingDownString  = "\xE2\x97\xA2";       // U+25E2. Diagonal (Windows' style)

    static unsigned int g_TreeView_ArrowFacingRightUTF32 = 0x25B6;
    static unsigned int g_TreeView_ArrowFacingDownUTF32  = 0x25E2;       // U+25E2. Diagonal (Windows' style)


    struct AssetExplorerUserData
    {
        AssetExplorer* pAssetExplorer;
    };

    struct AssetExplorerItemData
    {
        /// The width of the item.
        float width;

        /// The text to display on the item.
        char text[64];

        /// The length of the text.
        int textLength;

        /// The absolute path of the file or directory.
        char absolutePath[DRVFS_MAX_PATH];

        /// Whether or not the mouse is over the arrow.
        bool isMouseOverArrow;

        /// Whether or not the item is a directory.
        bool isDirectory;
    };

    static AssetExplorer* GetAssetExplorerFromItem(drgui_tree_view_item* pItem)
    {
        drgui_element* pTV = drgui_tvi_get_tree_view_element(pItem);
        if (pTV == NULL) {
            return NULL;
        }

        AssetExplorerUserData* pUserData = reinterpret_cast<AssetExplorerUserData*>(drgui_tv_get_extra_data(pTV));
        if (pUserData == NULL) {
            return NULL;
        }

        return pUserData->pAssetExplorer;
    }

    static void AssetExplorer_OnItemMouseMove(drgui_tree_view_item* pItem, int relativeMousePosX, int relativeMousePosY, bool* pIsOverArrow)
    {
        AssetExplorer* pAssetExplorer = GetAssetExplorerFromItem(pItem);
        if (pAssetExplorer == NULL) {
            return;
        }

        AssetExplorerItemData* pItemData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pItem));
        if (pItemData == NULL) {
            return;
        }

        float padding = pAssetExplorer->GetItemPadding();

        drgui_rect arrowRect = drgui_make_rect(padding, padding, padding + pAssetExplorer->GetItemArrowWidth(), pAssetExplorer->GetDefaultItemHeight() - padding);
        pItemData->isMouseOverArrow = drgui_rect_contains_point(arrowRect, (float)relativeMousePosX, (float)relativeMousePosY);

        if (pIsOverArrow) {
            *pIsOverArrow = pItemData->isMouseOverArrow;
        }
    }

    static void AssetExplorer_OnItemMouseLeave(drgui_tree_view_item* pItem)
    {
        AssetExplorerItemData* pItemData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pItem));
        if (pItemData == NULL) {
            return;
        }

        pItemData->isMouseOverArrow = false;
    }

    static void AssetExplorer_OnItemMeasure(drgui_tree_view_item* pItem, float* pWidthOut, float* pHeightOut)
    {
        assert(pItem      != NULL);
        assert(pWidthOut  != NULL);
        assert(pHeightOut != NULL);

        AssetExplorer* pAssetExplorer = GetAssetExplorerFromItem(pItem);
        if (pAssetExplorer == NULL) {
            return;
        }

        AssetExplorerItemData* pItemData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pItem));
        if (pItemData == NULL) {
            return;
        }


        if (pWidthOut != NULL) {
            *pWidthOut = pItemData->width;
        }

        if (pHeightOut != NULL) {
            *pHeightOut = pAssetExplorer->GetDefaultItemHeight();
        }
    }

    static void AssetExplorer_OnItemPaint(drgui_element* pTVElement, drgui_tree_view_item* pItem, drgui_rect relativeClippingRect, drgui_color backgroundColor, float offsetX, float offsetY, float width, float height, void* pPaintData)
    {
        assert(pTVElement != NULL);
        assert(pItem      != NULL);

        AssetExplorer* pAssetExplorer = GetAssetExplorerFromItem(pItem);
        if (pAssetExplorer == NULL) {
            return;
        }

        AssetExplorerItemData* pItemData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pItem));
        if (pItemData == NULL) {
            return;
        }

        float padding = pAssetExplorer->GetItemPadding();

        

        // Arrow.
        {
            drgui_rect arrowRect = drgui_make_rect(offsetX + padding, offsetY + padding, offsetX + padding + pAssetExplorer->GetItemArrowWidth(), offsetY + height - padding);

            drgui_font* pArrowFont = pAssetExplorer->GetArrowFont();
            drgui_font_metrics arrowFontMetrics = pAssetExplorer->GetArrowFontMetrics();

            if (drgui_tvi_has_children(pItem))
            {
                drgui_set_clip(pTVElement, arrowRect, pPaintData);

                drgui_glyph_metrics arrowMetrics = pAssetExplorer->GetArrowGlyphMetrics();

                float arrowPosX = offsetX + padding;
                float arrowPosY = offsetY - (arrowFontMetrics.ascent - arrowMetrics.originY) + ((pAssetExplorer->GetDefaultItemHeight() - arrowMetrics.height)/2);

                drgui_color arrowColor = drgui_rgb(224, 224, 224);
                if (pItemData->isMouseOverArrow) {
                    arrowColor = drgui_rgb(255, 255, 255);
                }

                if (drgui_tvi_is_expanded(pItem)) {
                    drgui_draw_text(pTVElement, pArrowFont, g_TreeView_ArrowFacingDownString, strlen(g_TreeView_ArrowFacingDownString), arrowPosX, arrowPosY, arrowColor, backgroundColor, pPaintData);
                } else {
                    drgui_draw_text(pTVElement, pArrowFont, g_TreeView_ArrowFacingRightString, strlen(g_TreeView_ArrowFacingRightString), arrowPosX, arrowPosY, arrowColor, backgroundColor, pPaintData);
                }


                // Space between the arrow and the text.
                drgui_draw_rect(pTVElement, drgui_make_rect(arrowPosX + arrowMetrics.advanceX, arrowPosY, arrowPosX + pAssetExplorer->GetItemArrowWidth(), arrowPosY + height), backgroundColor, pPaintData);       // Right
                drgui_draw_rect(pTVElement, drgui_make_rect(arrowPosX, offsetY + padding, arrowPosX + pAssetExplorer->GetItemArrowWidth(), arrowPosY), backgroundColor, pPaintData); // Top
                drgui_draw_rect(pTVElement, drgui_make_rect(arrowPosX, arrowPosY + arrowFontMetrics.lineHeight, arrowPosX + pAssetExplorer->GetItemArrowWidth(), arrowPosY + height - padding), backgroundColor, pPaintData); // Bottom

                // Restore the clip for future drawing.
                drgui_set_clip(pTVElement, relativeClippingRect, pPaintData);
            }
            else
            {
                // The item does not have any children. Don't draw the arrow, and instead just draw the a rectangle of the background color.
                drgui_draw_rect(pTVElement, arrowRect, backgroundColor, pPaintData);
            }
        }


        drgui_font* pSymbolFont = pAssetExplorer->GetEditor().GetSymbolFont();
        if (pSymbolFont == NULL) {
            return;
        }

        drgui_font_metrics symbolFontMetrics = pAssetExplorer->GetEditor().GetSymbolFontMetrics();

        // Icon.
        {
            const char*   iconStr = g_PageIconString;
            unsigned int  iconUTF = g_PageIconUTF32;
            drgui_color iconCol = drgui_rgb(240, 240, 240);
            drgui_glyph_metrics iconMetrics;
            float glyphWidth;

            if (pItemData->isDirectory || drgui_tvi_has_children(pItem))
            {
                if (drgui_tvi_is_expanded(pItem))
                {
                    iconStr = g_FolderOpenedIconString;
                    iconUTF = g_FolderOpenedUTF32;
                    iconCol = drgui_rgb(224, 186, 120);
                }
                else
                {
                    iconStr = g_FolderClosedIconString;
                    iconUTF = g_FolderClosedUTF32;
                    iconCol = drgui_rgb(224, 186, 120);
                }

                iconMetrics = pAssetExplorer->GetFolderIconGlyphMetrics();
                pAssetExplorer->GetFolderIconStringSize(&glyphWidth, NULL);
            }
            else
            {
                iconStr = g_PageIconString;
                iconUTF = g_PageIconUTF32;
                iconCol = drgui_rgb(240, 240, 240);

                iconMetrics = pAssetExplorer->GetPageIconGlyphMetrics();
                pAssetExplorer->GetPageIconStringSize(&glyphWidth, NULL);
            }


            //float glyphWidth;
            //drgui_measure_string(pSymbolFont, iconStr, strlen(iconStr), &glyphWidth, NULL);


            // We draw the text in such a position that it places the icon directly in the middle of the bounds.
            float iconPosX = offsetX + padding + pAssetExplorer->GetItemArrowWidth();
            float iconPosY = offsetY + padding;
            iconPosY -= 2;      // Temporary hack until we can fix a glyph metrics bug in dr2d.
            drgui_draw_text(pTVElement, pSymbolFont, iconStr, strlen(iconStr), iconPosX, iconPosY, iconCol, backgroundColor, pPaintData);

            // Space between the icon and the text.
            drgui_draw_rect(pTVElement, drgui_make_rect(iconPosX + glyphWidth, iconPosY, iconPosX + pAssetExplorer->GetItemIconWidth(), iconPosY + height - padding), backgroundColor, pPaintData);
        }


        // Text.
        {
            float textPosX = offsetX + padding + pAssetExplorer->GetItemArrowWidth() + pAssetExplorer->GetItemIconWidth();
            float textPosY = offsetY + padding;
            drgui_draw_text(pTVElement, pAssetExplorer->GetItemFont(), pItemData->text, pItemData->textLength, textPosX, textPosY, pAssetExplorer->GetItemTextColor(), backgroundColor, pPaintData);
        }


        // Padding.
        {
            drgui_draw_rect(pTVElement, drgui_make_rect(offsetX,                   offsetY,                    offsetX + padding,         offsetY + height),  backgroundColor, pPaintData);
            drgui_draw_rect(pTVElement, drgui_make_rect(offsetX + width - padding, offsetY,                    offsetX + width,           offsetY + height),  backgroundColor, pPaintData);
            drgui_draw_rect(pTVElement, drgui_make_rect(offsetX + padding,         offsetY,                    offsetX + width - padding, offsetY + padding), backgroundColor, pPaintData);
            drgui_draw_rect(pTVElement, drgui_make_rect(offsetX + padding,         offsetY + height - padding, offsetX + width - padding, offsetY + height),  backgroundColor, pPaintData);
        }
    }


    static void AssetExplorer_OnTreeViewItemPicked(drgui_tree_view_item* pItem)
    {
        AssetExplorer* pAssetExplorer = GetAssetExplorerFromItem(pItem);
        if (pAssetExplorer == NULL) {
            return;
        }

        pAssetExplorer->OnItemPicked(pItem);
    }

    static void AssetExplorer_OnTreeViewMouseButtonUp(drgui_element* pTVElement, int mouseButton, int relativeMousePosX, int relativeMousePosY, int stateFlags)
    {
        AssetExplorerUserData* pUserData = reinterpret_cast<AssetExplorerUserData*>(drgui_tv_get_extra_data(pTVElement));
        if (pUserData == NULL) {
            return;
        }

        AssetExplorer* pAssetExplorer = pUserData->pAssetExplorer;
        if (pAssetExplorer == NULL) {
            return;
        }

        if (mouseButton == DRGUI_MOUSE_BUTTON_RIGHT)
        {
            ak_menu_set_position(pAssetExplorer->m_pTestMenu, relativeMousePosX, relativeMousePosY);
            ak_menu_show(pAssetExplorer->m_pTestMenu);
        }

        drgui_tv_on_mouse_button_up(pTVElement, mouseButton, relativeMousePosX, relativeMousePosY, stateFlags);
    }


    AssetExplorer::AssetExplorer(Editor2 &editor)
        : EditorTool(editor), m_pTV(NULL)
    {
        
    }

    AssetExplorer::~AssetExplorer()
    {
        ak_delete_menu(m_pTestMenu);

        // Deleting the tree-view control will delete any items that are still attached.
        drgui_delete_tree_view(m_pTV);
    }


    bool AssetExplorer::Init(const char* attributes)
    {
        if (!EditorTool::Init(attributes)) {
            return false;
        }

        // Load the default font for items.
        m_pItemFont = m_editor.GetAKTheme()->pUIFont;
        drgui_get_font_metrics(m_pItemFont, 1, 1, &m_itemFontMetrics);

        m_pArrowFont = drgui_create_font(m_editor.GetGUI(), "Segoe UI Symbol", 10, drgui_font_weight_normal, drgui_font_slant_none, 0);
        drgui_get_font_metrics(m_pArrowFont, 1, 1, &m_arrowFontMetrics);
        drgui_get_glyph_metrics(m_pArrowFont, g_TreeView_ArrowFacingRightUTF32, 1, 1, &m_arrowGlyphMetrics);


        drgui_get_glyph_metrics(m_editor.GetSymbolFont(), g_FolderOpenedUTF32, 1, 1, &m_folderIconGlyphMetrics);
        drgui_measure_string(m_editor.GetSymbolFont(), g_FolderOpenedIconString, strlen(g_FolderOpenedIconString), 1, 1, &m_folderIconStringWidth, &m_folderIconStringHeight);
        
        drgui_get_glyph_metrics(m_editor.GetSymbolFont(), g_PageIconUTF32, 1, 1, &m_pageIconGlyphMetrics);
        drgui_measure_string(m_editor.GetSymbolFont(), g_PageIconString, strlen(g_PageIconString), 1, 1, &m_pageIconStringWidth, &m_pageIconStringHeight);


        // For now, have the tree view control also size itself so that it's the same size as the parent.
        drgui_set_on_size(m_pTool, drgui_on_size_fit_children_to_parent);



        // The tree-view control.
        AssetExplorerUserData userData;
        userData.pAssetExplorer = this;

        m_pTV = drgui_create_tree_view(m_editor.GetGUI(), m_pTool, sizeof(userData), &userData);
        if (m_pTV == NULL) {
            return false;
        }

        // Event handlers.
        drgui_tv_set_on_item_mouse_move(m_pTV, AssetExplorer_OnItemMouseMove);
        drgui_tv_set_on_item_mouse_leave(m_pTV, AssetExplorer_OnItemMouseLeave);
        drgui_tv_set_on_item_paint(m_pTV, AssetExplorer_OnItemPaint);
        drgui_tv_set_on_item_measure(m_pTV, AssetExplorer_OnItemMeasure);
        drgui_tv_set_on_item_picked(m_pTV, AssetExplorer_OnTreeViewItemPicked);
        drgui_set_on_mouse_button_up(m_pTV, AssetExplorer_OnTreeViewMouseButtonUp);


        // The base directories needs to be added first.
        for (unsigned int iBasePath = 0; iBasePath < drvfs_get_base_directory_count(this->GetVFS()); ++iBasePath)
        {
            AssetExplorerItemData itemData;
            
            drpath_to_relative(drvfs_get_base_directory_by_index(this->GetVFS(), iBasePath), m_editor.GetContext().GetExecutableDirectoryAbsolutePath(), itemData.text, sizeof(itemData.text));
            if (itemData.text[0] == '\0') {
                strcpy_s(itemData.text, sizeof(itemData.text), "[root]");
            }

            itemData.textLength = (int)strlen(itemData.text);
            itemData.width = this->CalculateItemWidth(itemData.text);
            itemData.isMouseOverArrow = false;
            itemData.isDirectory = true;
            strcpy_s(itemData.absolutePath, sizeof(itemData.absolutePath), drvfs_get_base_directory_by_index(this->GetVFS(), iBasePath));

            drgui_tv_create_item(m_pTV, NULL, sizeof(itemData), &itemData);


            this->InsertDirectoryRecursive(itemData.absolutePath);
        }


        // Add some testing items.
        m_pTestMenu = ak_create_menu(m_editor.GetAKApplication(), ak_get_window_by_name(m_editor.GetAKApplication(), "MainWindow"), 0, NULL);
        ak_menu_set_size(m_pTestMenu, 256, 512);
        ak_menu_show(m_pTestMenu);

        return true;
    }


    drgui_element* AssetExplorer::GetTreeView()
    {
        return m_pTV;
    }



    void AssetExplorer::OnItemPicked(drgui_tree_view_item* pItem)
    {
        AssetExplorerItemData* pItemData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pItem));
        if (pItemData == NULL) {
            return;
        }
        
        printf("Item Picked: %s\n", pItemData->absolutePath);
    }


#if 0
    ak_tree_view_item* AssetExplorer::FindItemByAbsolutePath(const char* absolutePath)
    {
        if (absolutePath == NULL) {
            return false;
        }

        return NULL;
    }
#endif


    drgui_tree_view_item* AssetExplorer::InsertItem(const char* absolutePath)
    {
        if (absolutePath == NULL) {
            return NULL;
        }

        // Ignore this item if it's a base path.
        for (drgui_tree_view_item* pItem = drgui_tvi_get_first_child(drgui_tv_get_root_item(m_pTV)); pItem != NULL; pItem = drgui_tvi_get_next_sibling(pItem))
        {
            AssetExplorerItemData* pItemData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pItem));
            if (pItemData != NULL)
            {
                if (strcmp(absolutePath, pItemData->absolutePath) == 0) {
                    return pItem;
                }
            }
        }

        
        // We first need to check if the path is a descendant of a base path.
        for (drgui_tree_view_item* pItem = drgui_tvi_get_first_child(drgui_tv_get_root_item(m_pTV)); pItem != NULL; pItem = drgui_tvi_get_next_sibling(pItem))
        {
            AssetExplorerItemData* pItemData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pItem));
            if (pItemData != NULL)
            {
                if (drpath_is_descendant(absolutePath, pItemData->absolutePath))
                {
                    // The new item is a descendant of this base directory - we now need to build each item segment by segment.
                    char relativePath[DRVFS_MAX_PATH];
                    if (drpath_to_relative(absolutePath, pItemData->absolutePath, relativePath, sizeof(relativePath)))
                    {
                        drgui_tree_view_item* pNewItem = pItem;

                        drpath_iterator iseg = drpath_first(relativePath);
                        while (drpath_next(&iseg))
                        {
                            char segment[DRVFS_MAX_PATH];
                            strncpy_s(segment, sizeof(segment), iseg.path + iseg.segment.offset, iseg.segment.length);

                            pNewItem = this->InsertChildItem(pNewItem, segment);
                            if (pNewItem == NULL) {
                                return NULL;
                            }
                        }

                        return pNewItem;
                    }
                }

                if (strcmp(absolutePath, pItemData->absolutePath) == 0) {
                    return pItem;
                }
            }
        }

        return NULL;
    }

    void AssetExplorer::RemoveItem(const char* absolutePath)
    {
        if (absolutePath == NULL) {
            return;
        }


    }

    void AssetExplorer::RenameItem(const char* absolutePathOld, const char* absolutePathNew)
    {
        if (absolutePathOld == NULL || absolutePathNew == NULL) {
            return;
        }


    }

    drgui_font* AssetExplorer::GetItemFont() const
    {
        return m_pItemFont;
    }

    drgui_color AssetExplorer::GetItemTextColor() const
    {
        ak_theme* pTheme = m_editor.GetAKTheme();
        if (pTheme == NULL) {
            return drgui_rgb(240, 240, 240);
        }

        return pTheme->uiFontColor;
    }

    float AssetExplorer::GetItemPadding() const
    {
        return 2;
    }

    float AssetExplorer::GetDefaultItemHeight() const
    {
        return (float)m_itemFontMetrics.lineHeight + (this->GetItemPadding() * 2);
    }

    float AssetExplorer::GetItemArrowWidth() const
    {
        return 16;
    }

    float AssetExplorer::GetItemIconWidth() const
    {
        return 20;
    }


    drgui_font * AssetExplorer::GetArrowFont() const
    {
        return m_pArrowFont;
    }

    drgui_font_metrics AssetExplorer::GetArrowFontMetrics() const
    {
        return m_arrowFontMetrics;
    }

    drgui_glyph_metrics AssetExplorer::GetArrowGlyphMetrics() const
    {
        return m_arrowGlyphMetrics;
    }


    drgui_glyph_metrics AssetExplorer::GetFolderIconGlyphMetrics() const
    {
        return m_folderIconGlyphMetrics;
    }

    void AssetExplorer::GetFolderIconStringSize(float* pWidthOut, float* pHeightOut)
    {
        if (pWidthOut != NULL) {
            *pWidthOut = m_folderIconStringWidth;
        }

        if (pHeightOut != NULL) {
            *pHeightOut = m_folderIconStringHeight;
        }
    }


    drgui_glyph_metrics AssetExplorer::GetPageIconGlyphMetrics() const
    {
        return m_pageIconGlyphMetrics;
    }

    void AssetExplorer::GetPageIconStringSize(float* pWidthOut, float* pHeightOut)
    {
        if (pWidthOut != NULL) {
            *pWidthOut = m_pageIconStringWidth;
        }

        if (pHeightOut != NULL) {
            *pHeightOut = m_pageIconStringHeight;
        }
    }




    /////////////////////////////////////////////
    // Private

    drgui_tree_view_item* AssetExplorer::InsertChildItem(drgui_tree_view_item* pParentItem, const char* relativePath)
    {
        AssetExplorerItemData* pParentData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pParentItem));
        assert(pParentData != NULL);

        char absolutePath[DRVFS_MAX_PATH];
        drpath_copy_and_append(absolutePath, sizeof(absolutePath), pParentData->absolutePath, relativePath);
        

        // If the child already exists, just return that one.
        for (drgui_tree_view_item* pChild = drgui_tvi_get_first_child(pParentItem); pChild != NULL; pChild = drgui_tvi_get_next_sibling(pChild))
        {
            AssetExplorerItemData* pChildData = reinterpret_cast<AssetExplorerItemData*>(drgui_tvi_get_extra_data(pChild));
            assert(pChildData != NULL);

            if (drpath_equal(absolutePath, pChildData->absolutePath))
            {
                return pChild;
            }
        }


        // At this point we know the item does not already exist - create it.

        // TODO: Sort this alphabetically and by type (directory, archive, file).
        AssetExplorerItemData itemData;
        strcpy_s(itemData.text, sizeof(itemData.text), drpath_file_name(absolutePath));
        itemData.textLength = strlen(itemData.text);
        itemData.width = this->CalculateItemWidth(itemData.text);
        itemData.isMouseOverArrow = false;
        itemData.isDirectory = drvfs_is_existing_directory(this->GetVFS(), absolutePath);
        strcpy_s(itemData.absolutePath, sizeof(itemData.absolutePath), absolutePath);

        drgui_tree_view_item* pNewItem = drgui_tv_create_item(drgui_tvi_get_tree_view_element(pParentItem), pParentItem, sizeof(itemData), &itemData);
        

        return pNewItem;
    }

    void AssetExplorer::InsertDirectoryRecursive(const char* absolutePath)
    {
        drgui_tree_view_item* pDirItem = this->InsertItem(absolutePath);
        if (pDirItem == NULL) {
            return;
        }


        // Children.
        drvfs_iterator iFile;
        if (drvfs_begin(this->GetVFS(), absolutePath, &iFile))
        {
            do
            {
                if ((iFile.info.attributes & DRVFS_FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    this->InsertDirectoryRecursive(iFile.info.absolutePath);
                }
                else
                {
                    this->InsertChildItem(pDirItem, drpath_file_name(iFile.info.absolutePath));
                }
            } while (drvfs_next(this->GetVFS(), &iFile));
        }
    }

    float AssetExplorer::CalculateItemWidth(const char * text) const
    {
        float textWidth;
        drgui_measure_string(m_pItemFont, text, strlen(text), 1, 1, &textWidth, NULL);

        return this->GetItemArrowWidth() + this->GetItemIconWidth() + textWidth + (this->GetItemPadding()*2);   // Times 2 for the padding because we want to include both left and right sides.
    }
}