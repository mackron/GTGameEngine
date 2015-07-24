// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorAssetExplorerItem.hpp>
#include <GTGameEngine/Editor/Controls/EditorAssetExplorer.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>
#include <GTLib/Strings/Equal.hpp>
#include "../../external/easy_path/easy_path.h"

namespace GT
{
    static const char* g_ArrowFacingRightString = "\xE2\x96\xB6";       // U+25B6
    //static const char* g_ArrowFacingDownString  = "\xE2\x96\xBC";       // U+25BC. Straight down.
    static const char* g_ArrowFacingDownString  = "\xE2\x97\xA2";       // U+25E2. Diagonal (Windows' style)
    static const char* g_PageIconString         = "\xF0\x9F\x93\x84";   // U+1F4C4
    static const char* g_FoldeClosedIconString  = "\xF0\x9F\x93\x81";   // U+1F4C1
    static const char* g_FoldeOpenedIconString  = "\xF0\x9F\x93\x82";   // U+1F4C2

    EditorAssetExplorerItem::EditorAssetExplorerItem(Editor &editor, EditorAssetExplorerItem* pParentItem, EditorAssetExplorer &explorer, FileSystem &fileSystem, const char* absolutePath, const char* displayName)
        : EditorControl(editor),
          m_pParent(pParentItem),
          m_explorer(explorer),
          m_fileSystem(fileSystem),
          m_absolutePath(absolutePath),
          m_isExpanded(false),
          m_isSelected(false),
          m_titleEventHandler(*this)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();
            const EditorTheme &theme = editor.GetTheme();

            gui.SetElementWidthRatio(hRootElement, 1.0f);
            gui.SetElementBackgroundColor(hRootElement, theme.backgroundMid);
            gui.SetElementFont(hRootElement, theme.defaultFontFamily, FontWeight_Medium, FontSlant_None, 12);
            gui.SetElementTextColor(hRootElement, theme.defaultTextColor);
            gui.SetElementHeightToChildren(hRootElement);
            //gui.SetElementBorder(hRootElement, 1, GTLib::Colour::Black);


            // Title.
            m_hTitleContainer = gui.CreateElement();
            gui.SetElementParent(m_hTitleContainer, hRootElement);
            gui.SetElementChildAxis(m_hTitleContainer, ChildAxis_Horizontal);
            gui.SetElementSizeToChildren(m_hTitleContainer);
            gui.SetElementMinWidthRatio(m_hTitleContainer, 1.0f);
            gui.SetElementVerticalAlign(m_hTitleContainer, VerticalAlign_Center);
            gui.SetElementPaddingLeft(m_hTitleContainer, 4 + (this->GetDepth() * 16));
            gui.SetElementPaddingTop(m_hTitleContainer, 1);
            gui.SetElementPaddingBottom(m_hTitleContainer, 1);

            m_hTitleArrow = gui.CreateElement();
            gui.SetElementParent(m_hTitleArrow, m_hTitleContainer);
            gui.SetElementText(m_hTitleArrow, g_ArrowFacingRightString);
            gui.SetElementTextColor(m_hTitleArrow, theme.defaultTextColor);
            gui.SetElementFont(m_hTitleArrow, theme.defaultSymbolFontFamily, FontWeight_Medium, FontSlant_None, 9);
            gui.SetElementHeightToChildren(m_hTitleArrow);
            gui.SetElementWidth(m_hTitleArrow, 16U);

            m_hTitleIcon = gui.CreateElement();
            gui.SetElementParent(m_hTitleIcon, m_hTitleContainer);
            gui.SetElementFont(m_hTitleIcon, theme.defaultSymbolFontFamily, FontWeight_Medium, FontSlant_None, 14);
            if (fileSystem.IsDirectory(absolutePath))
            {
                gui.SetElementText(m_hTitleIcon, g_FoldeClosedIconString);
                gui.SetElementTextColor(m_hTitleIcon, GTLib::Colour(0.8f, 0.7f, 0.4f));
            }
            else
            {
                gui.SetElementText(m_hTitleIcon, g_PageIconString);
                gui.SetElementTextColor(m_hTitleIcon, theme.defaultTextColor);

                gui.SetElementText(m_hTitleArrow, "");
            }
            gui.SetElementHeightToChildren(m_hTitleIcon);
            gui.SetElementWidth(m_hTitleIcon, 18U);
            gui.EnableCursorPassThrough(m_hTitleIcon);

            m_hTitleText = gui.CreateElement();
            gui.SetElementParent(m_hTitleText, m_hTitleContainer);
            gui.SetElementWidthToChildren(m_hTitleText);
            gui.SetElementTextColor(m_hTitleText, theme.defaultTextColor);
            gui.SetElementHeightToChildren(m_hTitleText);
            if (displayName != nullptr)
            {
                gui.SetElementText(m_hTitleText, displayName);
            }
            else
            {
                gui.SetElementText(m_hTitleText, easypath_filename(absolutePath));
            }
            gui.EnableCursorPassThrough(m_hTitleText);
            gui.SetElementPaddingBottom(m_hTitleText, 2);
            
            gui.AttachLocalEventHandler(m_hTitleContainer, m_titleEventHandler);



            // Child container.
            m_hChildContainer = gui.CreateElement();
            gui.SetElementParent(m_hChildContainer, hRootElement);
            gui.SetElementSizeToChildren(m_hChildContainer);
            gui.SetElementMinWidthRatio(m_hChildContainer, 1.0f);
            gui.HideElement(m_hChildContainer);
        }
    }

    EditorAssetExplorerItem::~EditorAssetExplorerItem()
    {
        m_explorer.DeselectItem(*this);

        this->DeleteAllChildren();
    }


    EditorAssetExplorerItem* EditorAssetExplorerItem::CreateAndInsertChild(const char* absolutePath)
    {
        if (easypath_isdescendant(absolutePath, m_absolutePath.c_str()))
        {
            if (!m_explorer.IsBaseDirectory(absolutePath))
            {
                // If an item of the same absolute path already exists, just return the existing item.
                EditorAssetExplorerItem* pExistingItem = this->FindChildByAbsolutePath(absolutePath);
                if (pExistingItem != nullptr)
                {
                    return pExistingItem;
                }

            
                auto &gui = this->GetGUI();

                // If we get here it means the item does not already exist. We want to the add the item in alphabetical order so we'll need
                // to do an insertion rather than a push-back.
                size_t insertIndex;
                this->FindChildAlphabeticalInsertIndex(absolutePath, insertIndex);
            
            

                EditorAssetExplorerItem* pNewItem = new EditorAssetExplorerItem(this->GetEditor(), this, m_explorer, this->GetEditor().GetEngineContext().GetFileSystem(), absolutePath, nullptr);
            
                EditorAssetExplorerItem* pItemToInsertAfter = nullptr;
                if (insertIndex > 0)
                {
                    pItemToInsertAfter = m_children[insertIndex - 1];
                    assert(pItemToInsertAfter != nullptr);

                    gui.AppendSiblingElement(pItemToInsertAfter->GetRootGUIElement(), pNewItem->GetRootGUIElement());
                }
                else
                {
                    // It's the first in the list. The GUI element is appended to the child container.
                    gui.PrependChildElement(m_hChildContainer, pNewItem->GetRootGUIElement());
                }

                m_children.InsertAt(pNewItem, insertIndex);
            }
        }

        return nullptr;
    }

    void EditorAssetExplorerItem::DeleteAllChildren()
    {
        while (m_children.GetCount() > 0)
        {
            auto pChild = m_children.GetBack();
            assert(pChild != nullptr);

            pChild->DeleteAllChildren();
            m_children.PopBack();

            delete pChild;
        }
    }


    EditorAssetExplorerItem* EditorAssetExplorerItem::FindChildByAbsolutePath(const char* absolutePath)
    {
        for (size_t i = 0; i < m_children.GetCount(); ++i)
        {
            auto pChildItem = m_children[i];
            if (pChildItem != nullptr)
            {
                if (GTLib::Strings::Equal(absolutePath, pChildItem->GetAbsolutePath()))
                {
                    return pChildItem;
                }
            }
        }

        return nullptr;
    }

    EditorAssetExplorerItem* EditorAssetExplorerItem::FindDescendantByAbsolutePath(const char* absolutePath)
    {
        for (size_t i = 0; i < m_children.GetCount(); ++i)
        {
            auto pChildItem = m_children[i];
            if (pChildItem != nullptr)
            {
                if (GTLib::Strings::Equal(absolutePath, pChildItem->GetAbsolutePath()))
                {
                    return pChildItem;
                }
                else
                {
                    // Not equal so check descendants.
                    pChildItem = pChildItem->FindDescendantByAbsolutePath(absolutePath);
                    if (pChildItem != nullptr)
                    {
                        return pChildItem;
                    }
                }
            }
        }

        return nullptr;
    }

    void EditorAssetExplorerItem::FindChildAlphabeticalInsertIndex(const char* absolutePath, size_t &insertIndexOut) const
    {
        insertIndexOut = 0;
        size_t firstFileIndex = 0;
        if (this->FirstFirstFileIndex(firstFileIndex))
        {
            if (m_fileSystem.IsDirectory(absolutePath))
            {
                insertIndexOut = 0;
                for (size_t iChild = firstFileIndex; iChild > 0; --iChild)
                {
                    auto pChild = m_children[iChild - 1];
                    assert(pChild != nullptr);

                    if (GTLib::Strings::CompareNoCase(easypath_filename(pChild->GetAbsolutePath()), easypath_filename(absolutePath)) < 0)
                    {
                        insertIndexOut = iChild;
                        return;
                    }
                }
            }
            else
            {
                insertIndexOut = firstFileIndex;
                for (size_t iChild = m_children.GetCount(); iChild > firstFileIndex; --iChild)
                {
                    auto pChild = m_children[iChild - 1];
                    assert(pChild != nullptr);

                    if (GTLib::Strings::CompareNoCase(easypath_filename(pChild->GetAbsolutePath()), easypath_filename(absolutePath)) < 0)
                    {
                        insertIndexOut = iChild;
                        return;
                    }
                }
            }
        }
    }



    void EditorAssetExplorerItem::Expand()
    {
        if (!this->IsExpanded())
        {
            if (m_fileSystem.IsDirectory(m_absolutePath.c_str()))
            {
                auto &gui   = this->GetGUI();
                //auto &theme = this->GetEditor().GetTheme();

                gui.BeginBatch();
                {
                    gui.SetElementText(m_hTitleArrow, g_ArrowFacingDownString);
                    gui.SetElementText(m_hTitleIcon, g_FoldeOpenedIconString);


                    // We now need to iterate over each file/folder in this folder and make sure an item is added for it. We also want to make sure deleted
                    // elements are remove from here. To do this we just build a list of files that should be included and cross reference.
                    GTLib::Vector<GTLib::String> pathsToAdd;
                    m_fileSystem.IterateFiles(m_absolutePath.c_str(), [&](const GT::FileInfo &fi) -> bool {
                        pathsToAdd.PushBack(fi.absolutePath);
                        return true;
                    });


                    // First we delete any items that no longer exist.
                    for (size_t iChild = 0; iChild < m_children.GetCount(); )
                    {
                        auto pChild = m_children[iChild];
                        assert(pChild != nullptr);

                        if (!pathsToAdd.Exists(pChild->GetAbsolutePath()))
                        {
                            m_children.Remove(iChild);
                            delete pChild;
                        }
                        else
                        {
                            ++iChild;
                        }
                    }


                    // Now add the new files.
                    for (size_t iPathToAdd = 0; iPathToAdd < pathsToAdd.GetCount(); ++iPathToAdd)
                    {
                        this->CreateAndInsertChild(pathsToAdd[iPathToAdd].c_str());
                    }


                    gui.ShowElement(m_hChildContainer);
                }
                gui.EndBatch();

                m_isExpanded = true;
            }
        }
    }

    void EditorAssetExplorerItem::Collapse()
    {
        if (this->IsExpanded())
        {
            if (m_fileSystem.IsDirectory(m_absolutePath.c_str()))
            {
                auto &gui   = this->GetGUI();
                //auto &theme = this->GetEditor().GetTheme();

                gui.SetElementText(m_hTitleArrow, g_ArrowFacingRightString);
                gui.SetElementText(m_hTitleIcon, g_FoldeClosedIconString);

                gui.HideElement(m_hChildContainer);

                m_isExpanded = false;
            }
        }
    }

    void EditorAssetExplorerItem::ToggleExpand()
    {
        if (!this->IsExpanded())
        {
            this->Expand();
        }
        else
        {
            this->Collapse();
        }
    }

    bool EditorAssetExplorerItem::IsExpanded() const
    {
        return m_isExpanded;
    }


    void EditorAssetExplorerItem::Select()
    {
        if (!this->IsSelected())
        {
            this->EnabledSelectedStyle();
            m_isSelected = true;
        }
    }

    void EditorAssetExplorerItem::Deselect()
    {
        if (this->IsSelected())
        {
            this->DisableSelectedStyle();
            m_isSelected = false;
        }
    }

    void EditorAssetExplorerItem::ToggleSelect()
    {
        if (!this->IsSelected())
        {
            this->Select();
        }
        else
        {
            this->Select();
        }
    }

    bool EditorAssetExplorerItem::IsSelected() const
    {
        return m_isSelected;
    }


    void EditorAssetExplorerItem::EnableHoveredStyle()
    {
        if (!this->IsSelected())
        {
            auto &gui = this->GetGUI();
        
            gui.SetElementBackgroundColor(m_hTitleContainer, GTLib::Colour(0.35f, 0.35f, 0.35f));
        }
    }

    void EditorAssetExplorerItem::DisableHoveredStyle()
    {
        if (!this->IsSelected())
        {
            auto &gui = this->GetGUI();
        
            gui.SetElementBackgroundColor(m_hTitleContainer, GTLib::Colour(0, 0, 0, 0));
        }
    }


    void EditorAssetExplorerItem::EnabledSelectedStyle()
    {
        auto &gui   = this->GetGUI();
        auto &theme = this->GetEditor().GetTheme();
        
        gui.SetElementBackgroundColor(m_hTitleContainer, theme.borderDefault);
    }

    void EditorAssetExplorerItem::DisableSelectedStyle()
    {
        auto &gui = this->GetGUI();
        
        gui.SetElementBackgroundColor(m_hTitleContainer, GTLib::Colour(0, 0, 0, 0));
    }



    unsigned int EditorAssetExplorerItem::GetDepth() const
    {
        if (m_pParent != nullptr)
        {
            return m_pParent->GetDepth() + 1;
        }

        return 0;
    }



    ///////////////////////////////////////////////////////
    // Private

    bool EditorAssetExplorerItem::FirstFirstFileIndex(size_t &indexOut) const
    {
        for (size_t i = 0; i < m_children.GetCount(); ++i)
        {
            auto pChild = m_children[i];
            assert(pChild != nullptr);

            if (!m_fileSystem.IsDirectory(pChild->GetAbsolutePath()))
            {
                indexOut = i;
                return true;
            }
        }

        return false;
    }



    ///////////////////////////////////////////////////////
    // EditorAssetExplorerItem::TitleEventHandler

    void EditorAssetExplorerItem::TitleEventHandler::OnMouseButtonPressed(GUIContext &, HGUIElement, int mouseButton, int, int)
    {
        if (mouseButton == 1)   // Left
        {
            m_explorerItem.GetAssetExplorer().SelectItemFromMouseInput(m_explorerItem);
        }
    }
}
