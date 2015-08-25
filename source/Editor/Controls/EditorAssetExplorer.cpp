// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorAssetExplorer.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    EditorAssetExplorer::EditorAssetExplorer(Editor &editor)
        : EditorScrollableControl(editor),
          m_eventHandler(*this)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();
            const EditorTheme &theme = editor.GetTheme();

            gui.SetElementSizeRatio(hRootElement, 1.0f, 1.0f);
            gui.SetElementBorder(hRootElement, 1, theme.borderDefault);



            HGUIElement hContentElement = this->GetContentElement();
            gui.SetElementBackgroundColor(hContentElement, theme.backgroundMid);


            // Add the base directory.
            auto &fileSystem = editor.GetEngineContext().GetFileSystem();
            size_t baseDirectoryCount = fileSystem.GetBaseDirectoryCount();
            for (size_t i = 0; i < baseDirectoryCount; ++i)
            {
                this->AddBaseDirectory(fileSystem.GetBaseDirectoryByIndex(i).c_str(), nullptr);
            }



            // Attach the event handler.
            gui.AttachGlobalEventHandler(m_eventHandler);
        }
    }

    EditorAssetExplorer::~EditorAssetExplorer()
    {
        for (size_t i = 0; i < m_baseItems.GetCount(); ++i)
        {
            delete m_baseItems[i];
        }

        this->GetGUI().DetachGlobalEventHandler(m_eventHandler);
    }


    void EditorAssetExplorer::AddBaseDirectory(const char* absolutePath, const char* displayName)
    {
        if (absolutePath != nullptr)
        {
            GUIContext &gui = this->GetGUI();

            EditorAssetExplorerItem* pItem = new EditorAssetExplorerItem(this->GetEditor(), nullptr, *this, this->GetEditor().GetEngineContext().GetFileSystem(), absolutePath, displayName);
            HGUIElement hItemElement = pItem->GetRootGUIElement();
            if (hItemElement != NULL)
            {
                gui.SetElementParent(hItemElement, this->GetContentElement());
            }


            m_baseItems.PushBack(pItem);
        }
    }


#if 0
    void EditorAssetExplorer::AddItem(const char* absolutePath)
    {
        EditorAssetExplorerItem* pItem = new EditorAssetExplorerItem(this->GetEditor(), *this, this->GetEditor().GetEngineContext().GetFileSystem(), absolutePath, nullptr);

        this->GetGUI().SetElementParent(pItem->GetRootGUIElement(), this->GetContentElement());
        this->GetGUI().SetElementText(pItem->GetRootGUIElement(), absolutePath);
    }
#endif


    bool EditorAssetExplorer::IsBaseDirectory(const char* absolutePath) const
    {
        for (size_t i = 0; i < m_baseItems.GetCount(); ++i)
        {
            if (strcmp(m_baseItems[i]->GetAbsolutePath(), absolutePath) == 0)
            {
                return true;
            }
        }

        return false;
    }


    void EditorAssetExplorer::SelectItemFromMouseInput(EditorAssetExplorerItem &item)
    {
        // If the control key is down we need to keep other items selected. If shift is down we need to select a range. If no keys are down, we need to deselect.
        //
        // TODO: Handle CTRL and Shift keys.
        this->DeselectAllItems();
        this->SelectItem(item);
    }

    void EditorAssetExplorer::SelectItem(EditorAssetExplorerItem &item)
    {
        // We remove and re-insert the selected item to push it down to the end of the list. That way we can know whether or not it was the last item to be selected.
        m_selectedItems.RemoveFirstOccuranceOf(&item);
        m_selectedItems.PushBack(&item);
        item.Select();
    }

    void EditorAssetExplorer::DeselectItem(EditorAssetExplorerItem &item)
    {
        item.Deselect();
        m_selectedItems.RemoveFirstOccuranceOf(&item);
    }

    void EditorAssetExplorer::DeselectAllItems()
    {
        for (size_t i = 0; i < m_selectedItems.GetCount(); ++i)
        {
            auto pItem = m_selectedItems[i];
            assert(pItem != nullptr);
            {
                pItem->Deselect();
            }
        }
    }


    void EditorAssetExplorer::ScrollY(int delta)
    {
        int tempScrollDistance = 21;        // This is temporary. Later we'll make scrolling work by placing elements at the top based on which items are expended and whatnot.

        HGUIElement hContentElement = this->GetContentElement();

        auto &gui = this->GetGUI();

        float newOffsetY = gui.GetElementInnerOffsetY(hContentElement) + (delta * tempScrollDistance);
        if (newOffsetY > 0)
        {
            newOffsetY = 0;
        }
        gui.SetElementInnerOffsetY(hContentElement, newOffsetY);
    }



    EditorAssetExplorer::ExplorerEventHandler::ExplorerEventHandler(EditorAssetExplorer &explorer)
        : m_explorer(explorer)
    {
    }

    void EditorAssetExplorer::ExplorerEventHandler::OnMouseWheel(GUIContext &context, HGUIElement, int delta, int, int)
    {
        if (context.IsElementUnderMouse(m_explorer.GetRootGUIElement()))
        {
            m_explorer.ScrollY(delta);
        }
    }
}
