// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/Editor/EditorGUISurfaceAUXData.hpp>
#include <GTGameEngine/Editor/Controls/EditorHeader.hpp>
#include <GTGameEngine/Editor/Controls/EditorBody.hpp>
#include <GTGameEngine/Editor/Controls/EditorFooter.hpp>
#include <GTGameEngine/Editor/Controls/EditorSubEditor.hpp>
#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/EngineContext.hpp>

#include <GTGameEngine/Editor/Controls/EditorPopupControl.hpp>
#include "../external/easy_fsw/easy_fsw.h"
#include "../external/easy_path/easy_path.h"

#if defined(GT_PLATFORM_WINDOWS)
#include <shellapi.h>
#endif

namespace GT
{
    void EditorFSWProc(void* pEditorIn)
    {
        auto pEditor = reinterpret_cast<Editor*>(pEditorIn);
        if (pEditor != nullptr)
        {
            easyfsw_event e;
            while (easyfsw_nextevent(pEditor->GetFileSystemWatcher(), &e))
            {
                switch (e.type)
                {
                case easyfsw_event_type_created:
                    {
                        pEditor->OnFileCreated(e.absolutePath);
                        break;
                    }

                case easyfsw_event_type_deleted:
                    {
                        pEditor->OnFileDeleted(e.absolutePath);
                        break;
                    }

                case easyfsw_event_type_renamed:
                    {
                        pEditor->OnFileRenamed(e.absolutePath, e.absolutePathNew);
                        break;
                    }

                case easyfsw_event_type_updated:
                    {
                        pEditor->OnFileUpdated(e.absolutePath);
                        break;
                    }

                default: break;
                }
            }
        }
    }


    Editor::Editor(GT::GameContext &gameContext)
        : m_gameContext(gameContext),
          m_guiRenderer(gameContext.GetEngineContext().GetAssetLibrary()),
          m_guiFontManager(),
          m_gui(&m_guiRenderer, &m_guiFontManager, &m_guiRenderer),
          m_globalGUIEventHandler(),
          m_hMainWindow(NULL),
          m_windowSurfaceMap(),
          m_eventHandlers(),
          m_fileSystemWatcherThread(), m_pFSW(nullptr),
          m_isOpen(false),
          m_pHeaderControl(nullptr),
          m_pBodyControl(nullptr),
          m_pFooterControl(nullptr),
          m_openedFiles(),
          m_defaultSubEditorAllocator(), m_pUserSubEditorAllocator(nullptr)
    {
    }

    Editor::~Editor()
    {
    }


    bool Editor::Startup()
    {
        // Try creating the file system watcher. No need to return false here, but we should post a warning showing that auto asset reloading is disabled.
        this->StartupFileSystemWatcher();

        // We need to reuse the main game window. If we don't have at least one we'll have to return false. The main game window is
        // the window at index 0.
        unsigned int windowCount = m_gameContext.GetWindowCount();
        if (windowCount > 0)
        {
            m_hMainWindow = m_gameContext.GetWindowByIndex(0);
            if (m_hMainWindow != NULL)
            {
                // We need a GUI surface for the main game window.
                if (this->CreateWindowSurfaceAndElement(m_hMainWindow))
                {
                    // Attach the GUI event handler.
                    m_gui.AttachGlobalEventHandler(m_globalGUIEventHandler);

                    m_gui.OnGlobalCursorNeedsToChange([&](GUISystemCursor cursor) {
                        this->SetCursorFromGUISystemCursor(cursor);
                    });


                    // Create the header control.
                    HGUIElement hMainWindowElement = this->GetWindowElement(m_hMainWindow);
                    if (hMainWindowElement != 0)
                    {
                        // Create the header control.
                        m_pHeaderControl = new EditorHeader(*this, m_hMainWindow);
                        m_gui.SetElementParent(m_pHeaderControl->GetRootGUIElement(), hMainWindowElement);

                        // Create the body control.
                        m_pBodyControl = new EditorBody(*this);
                        m_gui.SetElementParent(m_pBodyControl->GetRootGUIElement(), hMainWindowElement);

                        // Create the footer control.
                        m_pFooterControl = new EditorFooter(*this);
                        m_gui.SetElementParent(m_pFooterControl->GetRootGUIElement(), hMainWindowElement);



                        


                        return true;
                    }
                }
            }
        }

        return false;
    }

    void Editor::Shutdown()
    {
        // Every open file needs to be saved and closed.
        this->SaveAndCloseAllOpenFiles();



        this->ShutdownFileSystemWatcher();

        delete m_pFooterControl;
        m_pFooterControl = nullptr;

        delete m_pBodyControl;
        m_pBodyControl = nullptr;

        delete m_pHeaderControl;
        m_pHeaderControl = nullptr;


        // Delete all surfaces and elements.
        this->DeleteAllWindowSurfacesAndElements();


        // Ensure the global event handler is detached from the GUI.
        m_gui.DetachGlobalEventHandler(m_globalGUIEventHandler);

        // Remove all event handlers.
        m_eventHandlers.Clear();
    }


    bool Editor::StartupFileSystemWatcher()
    {
        m_pFSW = easyfsw_create_context();
        if (m_pFSW)
        {
            auto &fileSystem = m_gameContext.GetEngineContext().GetFileSystem();

            GTLib::Vector<GTLib::String> pathsToWatch;
            size_t baseDirectoryCount = fileSystem.GetBaseDirectoryCount();
            for (size_t iBaseDirectory = 0; iBaseDirectory < baseDirectoryCount; ++iBaseDirectory)
            {
                auto baseDirectoryAbsolutePath = fileSystem.GetBaseDirectoryByIndex(iBaseDirectory);
                
                // If the base directory is a descendant of any of the paths in pathsToWatch, ignore it.
                bool ignore = false;
                for (size_t iPathToWatch = 0; iPathToWatch < pathsToWatch.GetCount(); ++iPathToWatch)
                {
                    GTLib::String &pathToWatch = pathsToWatch[iPathToWatch];
                    if (easypath_isdescendant(baseDirectoryAbsolutePath.c_str(), pathToWatch.c_str()))
                    {
                        ignore = true;
                    }
                }

                if (!ignore)
                {
                    // If any of the paths in pathsToWatch is a descendant of the base directory, they need to be removed.
                    for (size_t iPathToWatch = 0; iPathToWatch < pathsToWatch.GetCount(); )
                    {
                        GTLib::String &pathToWatch = pathsToWatch[iPathToWatch];
                        if (easypath_isdescendant(pathToWatch.c_str(), baseDirectoryAbsolutePath.c_str()))
                        {
                            pathsToWatch.Remove(iPathToWatch);
                        }
                        else
                        {
                            ++iPathToWatch;
                        }
                    }

                    
                    // Descendants have been handled, so now we just add it.
                    pathsToWatch.PushBack(baseDirectoryAbsolutePath);
                }
            }


            // Now we can let the file system watcher know about our watchable paths.
            for (size_t iPathToWatch = 0; iPathToWatch < pathsToWatch.GetCount(); ++iPathToWatch)
            {
                easyfsw_add_directory(m_pFSW, pathsToWatch[iPathToWatch].c_str());
            }


            // Start watching the file system on another thread.
            m_fileSystemWatcherThread.Start(EditorFSWProc, this);

            return true;
        }

        return false;
    }

    void Editor::ShutdownFileSystemWatcher()
    {
        if (m_pFSW != NULL)
        {
            // When deleting the context, we want to set the pointer to null first so that easyfsw_nextevent() returns 0.
            easyfsw_context* pFSWOld = m_pFSW;
            m_pFSW = nullptr;
            easyfsw_delete_context(pFSWOld);

            // Wait for the watcher thread the close before returning.
            m_fileSystemWatcherThread.Wait();
        }
    }

    easyfsw_context* Editor::GetFileSystemWatcher()
    {
        return m_pFSW;
    }


    EngineContext & Editor::GetEngineContext()
    {
        return m_gameContext.GetEngineContext();
    }


    bool Editor::Open()
    {
        if (!this->IsOpen())
        {
            // Every relevant window needs to be shown.
            if (m_hMainWindow != 0)
            {
                // If the window was already visible it needs to be redrawn.
                HGUISurface hSurface = this->GetWindowSurface(m_hMainWindow);
                if (hSurface != NULL)
                {
                    unsigned int windowWidth;
                    unsigned int windowHeight;
                    if (m_gameContext.GetWindowSize(m_hMainWindow, windowWidth, windowHeight))
                    {
                        m_gui.SetSurfaceSize(hSurface, windowWidth, windowHeight);


                        // If the mouse is within the surface we need to post the OnMouseEnter event to the GUI system.
                        if (m_gameContext.IsMouseInsideWindow(m_hMainWindow))
                        {
                            if (m_gui.GetSurfaceUnderMouse() != hSurface)
                            {
                                m_gui.OnMouseLeave(m_gui.GetSurfaceUnderMouse());
                                m_gui.OnMouseEnter(hSurface);
                            }
                        }
                    }

                    m_gui.InvalidateSurfaceRect(hSurface);


                    // At this point the editor is now open, so post the event and set the flag.
                    this->PostEvent_OnEditorOpened();
                    m_isOpen = true;

                    return true;
                }
            }

            return false;
        }
        else
        {
            // The editor is already open.
            return true;
        }
    }

    void Editor::Close()
    {
        if (this->IsOpen())
        {
            // TODO: Every relevant windows needs to be hidden.


            // We are hiding everything surface, so we want to let the GUI know that the mouse has left whatever surface it is
            // currently sitting on.
            m_gui.OnMouseLeave(m_gui.GetSurfaceUnderMouse());


            // At this point the editor is closed, so post the event and set the flag.
            this->PostEvent_OnEditorClosed();
            m_isOpen = false;
        }
    }

    bool Editor::IsOpen() const
    {
        return m_isOpen;
    }


    void Editor::Step(double deltaTimeInSeconds)
    {
        (void)deltaTimeInSeconds;
    }


    bool Editor::OpenFile(const char* absolutePath)
    {
#if defined(GT_PLATFORM_WINDOWS)
        const char* extension = easypath_extension(absolutePath);
        if (GTLib::Strings::Equal<true>(extension, "exe"))
        {
            return ShellExecuteA(NULL, "open", absolutePath, NULL, NULL, SW_SHOWNORMAL) > reinterpret_cast<HINSTANCE>(32);
        }

        if (GTLib::Strings::Equal<true>(extension, "bat"))
        {
            return ShellExecuteA(NULL, "open", absolutePath, NULL, NULL, SW_SHOWNORMAL) > reinterpret_cast<HINSTANCE>(32);
        }
#endif

        // For now we'll add the tab to the first tab group, but later we'll want to make this more intelligent such adding
        // it to the tab group that the user was last interacting with.
        auto pTabGroup = m_pBodyControl->GetTabGroupByIndex(0);
        if (pTabGroup != nullptr)
        {
            auto pExistingTab = this->FindFileTab(absolutePath);
            if (pExistingTab != nullptr)
            {
                this->ActivateTab(pExistingTab);
            }
            else
            {
                auto pNewTab = pTabGroup->CreateTab(easypath_filename(absolutePath));
                if (pNewTab != nullptr)
                {
                    OpenedFile openedFile;
                    openedFile.absolutePath = absolutePath;
                    openedFile.pAsset = m_gameContext.GetEngineContext().GetAssetLibrary().Load(absolutePath);
                    openedFile.pTab = pNewTab;
                    openedFile.pSubEditor = nullptr;
                    openedFile.pAllocator = nullptr;

                    // We now need to create the sub-editor. If we have an asset, we create the sub-editor based on the asset class. If the asset class
                    // is unknown to the engine, we fire a callback to the host application and give it an opportunity to create a sub-editor.
                    if (openedFile.pAsset != nullptr)
                    {
                        openedFile.pSubEditor = m_defaultSubEditorAllocator.CreateSubEditor(*this, absolutePath, openedFile.pAsset);

                        if (openedFile.pSubEditor != nullptr) {
                            openedFile.pAllocator = &m_defaultSubEditorAllocator;
                        }
                    }
                    
                    // It wasn't a known asset class, so try the user-defined sub-editor allocator.
                    if (openedFile.pSubEditor == nullptr && m_pUserSubEditorAllocator != nullptr)
                    {
                        openedFile.pSubEditor = m_pUserSubEditorAllocator->CreateSubEditor(*this, absolutePath, openedFile.pAsset);

                        if (openedFile.pSubEditor != nullptr) {
                            openedFile.pAllocator = m_pUserSubEditorAllocator;
                        }
                    }

                    // The file could not be opened by neither the default nor user-defined sub-editor allocators, so fall back to the text editor.
                    if (openedFile.pSubEditor == nullptr)
                    {
                        openedFile.pSubEditor = m_defaultSubEditorAllocator.CreateTextFileSubEditor(*this, absolutePath);

                        if (openedFile.pSubEditor != nullptr) {
                            openedFile.pAllocator = &m_defaultSubEditorAllocator;
                        }


                        if (openedFile.pAsset != nullptr)
                        {
                            m_gameContext.GetEngineContext().GetAssetLibrary().Unload(openedFile.pAsset);
                            openedFile.pAsset = nullptr;
                        }
                    }


                    // The sub editor needs to be made a child of the tab's page element.
                    if (openedFile.pSubEditor != nullptr)
                    {
                        EditorTabPage* pTabPage = pTabGroup->GetTabPage(pNewTab);
                        assert(pTabPage != nullptr);

                        m_gui.SetElementParent(openedFile.pSubEditor->GetRootGUIElement(), pTabPage->GetRootGUIElement());


                        openedFile.pSubEditor->OnChanged([&, pNewTab]() {
                            GTLib::String newString = easypath_filename(this->FindFileAbsolutePathFromTab(pNewTab));
                            newString += "*";

                            pNewTab->SetText(newString.c_str());
                        });
                    }


                    // Finally, add the file to the local list so we can track it.
                    m_openedFiles.PushBack(openedFile);


                    pTabGroup->ActivateTab(pNewTab);
                    return true;
                }
            }
        }

        return false;
    }

    void Editor::CloseFile(const char* absolutePath)
    {
        auto pFileTab = this->FindFileTab(absolutePath);
        if (pFileTab != nullptr)
        {
            this->CloseTab(pFileTab);
        }
    }

    void Editor::TryCloseFile(const char* absolutePath)
    {
        // TODO: Check if the file has been modified and if so, show a confirmation box.

        this->CloseFile(absolutePath);
    }

    bool Editor::SaveFile(const char* absolutePath)
    {
        for (size_t iOpenedFile = 0; iOpenedFile < m_openedFiles.GetCount(); ++iOpenedFile)
        {
            auto &openedFile = m_openedFiles[iOpenedFile];
            if (openedFile.absolutePath == absolutePath)
            {
                return this->SaveFileByIndex(static_cast<unsigned int>(iOpenedFile));
            }
        }

        return false;
    }

    void Editor::SaveAndCloseAllOpenFiles()
    {
        while (this->GetOpenFileCount() > 0)
        {
            unsigned int iLastOpenFile = this->GetOpenFileCount() - 1;

            this->SaveFileByIndex(iLastOpenFile);
            this->CloseFileByIndex(iLastOpenFile);
        }
    }


    unsigned int Editor::GetOpenFileCount() const
    {
        return m_openedFiles.GetCount();
    }

    void Editor::CloseFileByIndex(unsigned int index)
    {
        OpenedFile &openedFile = m_openedFiles[index];
        if (openedFile.pAllocator != nullptr)
        {
            openedFile.pAllocator->DeleteSubEditor(openedFile.pSubEditor);
        }

        if (openedFile.pAsset != nullptr)
        {
            m_gameContext.GetEngineContext().GetAssetLibrary().Unload(openedFile.pAsset);
        }


        m_pBodyControl->CloseTab(openedFile.pTab);
        m_openedFiles.Remove(index);
    }

    bool Editor::SaveFileByIndex(unsigned int index)
    {
        OpenedFile &openedFile = m_openedFiles[index];
        if (openedFile.pSubEditor != nullptr) {
            if (openedFile.pSubEditor->SaveFile(openedFile.absolutePath.c_str())) {
                openedFile.pTab->SetText(easypath_filename(openedFile.absolutePath.c_str()));
            }
        }

        return false;
    }



    void Editor::CloseTab(EditorTab* pTab)
    {
        // Check if the tab is associated with a file.
        for (size_t iOpenedFile = 0; iOpenedFile < m_openedFiles.GetCount(); ++iOpenedFile)
        {
            auto &openedFile = m_openedFiles[iOpenedFile];
            if (openedFile.pTab == pTab)
            {
                this->CloseFileByIndex(iOpenedFile);
                break;
            }
        }
    }

    void Editor::ActivateTab(EditorTab* pTab)
    {
        auto pOldFocusedSubEditor = this->GetFocusedSubEditor();
        
        m_pBodyControl->ActivateTab(pTab);

        auto pNewFocusedSubEditor = this->GetFocusedSubEditor();
        if (pOldFocusedSubEditor != pNewFocusedSubEditor)
        {
            if (pOldFocusedSubEditor != nullptr) {
                pOldFocusedSubEditor->OnDeactivate();
            }

            if (pNewFocusedSubEditor != nullptr) {
                pNewFocusedSubEditor->OnActivate();
            }
        }
    }

    bool Editor::ActivateFileTab(const char* absolutePath)
    {
        auto pTab = this->FindFileTab(absolutePath);
        if (pTab != nullptr)
        {
            this->ActivateTab(pTab);
            return true;
        }

        return false;
    }


    EditorTab* Editor::GetFocusedTab()
    {
        if (m_pBodyControl != nullptr)
        {
            // NOTE: For now, just look at the first tab group. Need to reconsider this when working with multiple tab groups.
            if (m_pBodyControl->GetTabGroupCount() > 0)
            {
                return m_pBodyControl->GetTabGroupByIndex(0)->GetActiveTab();
            }
        }

        return nullptr;
    }

    const char* Editor::GetFocusedFileAbsolutePath()
    {
        EditorTab* pFocusedTab = this->GetFocusedTab();
        if (pFocusedTab != nullptr)
        {
            return this->FindFileAbsolutePathFromTab(pFocusedTab);
        }

        return nullptr;
    }

    EditorSubEditor* Editor::GetFocusedSubEditor()
    {
        return this->GetSubEditorByTab(this->GetFocusedTab());
    }

    EditorSubEditor* Editor::GetSubEditorByTab(EditorTab* pTab)
    {
        for (size_t iOpenedFile = 0; iOpenedFile < m_openedFiles.GetCount(); ++iOpenedFile)
        {
            auto &openedFile = m_openedFiles[iOpenedFile];
            if (openedFile.pTab == pTab)
            {
                return openedFile.pSubEditor;
            }
        }

        return nullptr;
    }


    EditorTab* Editor::FindFileTab(const char* absolutePath)
    {
        for (size_t iOpenedFile = 0; iOpenedFile < m_openedFiles.GetCount(); ++iOpenedFile)
        {
            auto &openedFile = m_openedFiles[iOpenedFile];
            if (openedFile.absolutePath == absolutePath)
            {
                return openedFile.pTab;
            }
        }

        return nullptr;
    }

    const char* Editor::FindFileAbsolutePathFromTab(EditorTab* pTab)
    {
        for (size_t iOpenedFile = 0; iOpenedFile < m_openedFiles.GetCount(); ++iOpenedFile)
        {
            auto &openedFile = m_openedFiles[iOpenedFile];
            if (openedFile.pTab == pTab)
            {
                return openedFile.absolutePath.c_str();
            }
        }

        return nullptr;
    }


    void Editor::AttachEventHandler(EditorEventHandler &eventHandler)
    {
        m_eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
        m_eventHandlers.PushBack(&eventHandler);
    }

    void Editor::DetachEventHandler(EditorEventHandler &eventHandler)
    {
        m_eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
    }


    HWindow Editor::CreateWindow(HWindow hParentWindow, WindowType windowType, int xPos, int yPos, unsigned int width, unsigned int height, HGUIElement hElement)
    {
        HWindow hWindow = m_gameContext.GetWindowManager().CreateWindow(hParentWindow, windowType, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            WindowGUISurfaceAndElement windowSurfaceAndElement;
            windowSurfaceAndElement.hElement = hElement;
            if (this->CreateWindowSurfaceAndElement(hWindow, windowSurfaceAndElement))
            {
            }
            else
            {
                m_gameContext.GetWindowManager().DeleteWindow(hWindow);
                hWindow = 0;
            }
        }

        return hWindow;
    }

    void Editor::DeleteWindow(HWindow hWindow)
    {
        this->DeleteWindowSurfaceAndElement(hWindow);
        m_gameContext.GetWindowManager().DeleteWindow(hWindow);
    }

    void Editor::ShowWindow(HWindow hWindow)
    {
        m_gameContext.ShowWindow(hWindow);
    }

    void Editor::HideWindow(HWindow hWindow)
    {
        m_gameContext.HideWindow(hWindow);
    }

    void Editor::SetWindowPosition(HWindow hWindow, int xPos, int yPos)
    {
        m_gameContext.GetWindowManager().SetWindowPosition(hWindow, xPos, yPos);
    }

    void Editor::SetWindowSize(HWindow hWindow, unsigned int width, unsigned int height)
    {
        m_gameContext.GetWindowManager().SetWindowSize(hWindow, width, height);
    }

    bool Editor::IsWindowDescendant(HWindow hParentWindow, HWindow hChildWindow) const
    {
        return m_gameContext.GetWindowManager().IsWindowDescendant(hParentWindow, hChildWindow);
    }


    HGUISurface Editor::GetWindowSurface(HWindow hWindow) const
    {
        auto iSurface = m_windowSurfaceMap.Find(hWindow);
        if (iSurface != nullptr)
        {
            return iSurface->value.hSurface;
        }
        else
        {
            return NULL;
        }
    }

    HGUISurface Editor::GetWindowElement(HWindow hWindow) const
    {
        auto iSurface = m_windowSurfaceMap.Find(hWindow);
        if (iSurface != nullptr)
        {
            return iSurface->value.hElement;
        }
        else
        {
            return NULL;
        }
    }

    HWindow Editor::GetSurfaceWindow(HGUISurface hSurface) const
    {
        EditorGUISurfaceAUXData* pSurfaceData = m_gui.GetSurfaceUserData<EditorGUISurfaceAUXData>(hSurface);
        if (pSurfaceData != nullptr)
        {
            return pSurfaceData->hWindow;
        }

        return NULL;
    }


    void Editor::SetCursor(SystemCursorType cursor)
    {
        // TODO: Set the cursor for all windows.
        m_gameContext.GetWindowManager().SetWindowCursor(m_hMainWindow, m_gameContext.GetWindowManager().GetSystemCursor(cursor));
    }

    void Editor::SetCursorFromGUISystemCursor(GUISystemCursor cursor)
    {
        switch (cursor)
        {
        case GUISystemCursor::IBeam:
            {
                this->SetCursor(SystemCursorType::IBeam);
                break;
            }

        case GUISystemCursor::Default:
        default:
            {
                this->SetCursor(SystemCursorType::Arrow);
                break;
            }
        }
    }




    ////////////////////////////////////
    // Common Shortcuts

    void Editor::DoCut()
    {
        auto pFocusedSubEditor = this->GetFocusedSubEditor();
        if (pFocusedSubEditor != nullptr) {
            pFocusedSubEditor->Cut();
        }
    }

    void Editor::DoCopy()
    {
        auto pFocusedSubEditor = this->GetFocusedSubEditor();
        if (pFocusedSubEditor != nullptr) {
            pFocusedSubEditor->Copy();
        }
    }

    void Editor::DoPaste()
    {
        auto pFocusedSubEditor = this->GetFocusedSubEditor();
        if (pFocusedSubEditor != nullptr) {
            pFocusedSubEditor->Paste();
        }
    }

    void Editor::DoUndo()
    {
        auto pFocusedSubEditor = this->GetFocusedSubEditor();
        if (pFocusedSubEditor != nullptr) {
            pFocusedSubEditor->Undo();
        }
    }

    void Editor::DoRedo()
    {
        auto pFocusedSubEditor = this->GetFocusedSubEditor();
        if (pFocusedSubEditor != nullptr) {
            pFocusedSubEditor->Redo();
        }
    }

    void Editor::DoSelectAll()
    {
        auto pFocusedSubEditor = this->GetFocusedSubEditor();
        if (pFocusedSubEditor != nullptr) {
            pFocusedSubEditor->SelectAll();
        }
    }



    ////////////////////////////////////
    // Inbound Events

    bool Editor::OnWantToClose()
    {
        return true;
    }

    void Editor::OnWindowCreated(HWindow hWindow)
    {
        (void)hWindow;
    }

    void Editor::OnWindowDeleting(HWindow hWindow)
    {
        (void)hWindow;
    }

    void Editor::OnWindowActivated(HWindow hWindow)
    {
        this->PostEvent_OnWindowActivated(hWindow);
    }

    void Editor::OnWindowDeactivated(HWindow hWindow)
    {
        this->PostEvent_OnWindowDeactivated(hWindow);
    }

    void Editor::OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.SetSurfaceSize(hSurface, width, height);
        }
    }

    void Editor::OnWindowMoved(HWindow hWindow, int xPos, int yPos)
    {
#if 0
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
        }
#else
        (void)hWindow;
        (void)xPos;
        (void)yPos;
#endif
    }

    void Editor::OnMouseMove(HWindow hWindow, int xPos, int yPos)
    {
        this->PostEvent_OnMouseMove(hWindow, xPos, yPos);

        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            // The cursor position may be on the client area which means it wasn't moved while on top of the GUI surface. Don't post
            // the event to the GUI if the position is not inside the surface's area.
            unsigned int surfaceWidth;
            unsigned int surfaceHeight;
            m_gui.GetSurfaceSize(hSurface, surfaceWidth, surfaceHeight);

            if (xPos > 0 && yPos > 0 && xPos < static_cast<int>(surfaceWidth) && yPos < static_cast<int>(surfaceHeight))
            {
                m_gui.OnMouseMove(hSurface, xPos, yPos);
            }
        }
    }

    void Editor::OnMouseEnter(HWindow hWindow)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.OnMouseEnter(hSurface);
        }
    }

    void Editor::OnMouseLeave(HWindow hWindow)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.OnMouseLeave(hSurface);
        }
    }

    void Editor::OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos)
    {
        this->PostEvent_OnMouseButtonPressed(hWindow, button, xPos, yPos);

        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            // The cursor position may be on the client area which means it wasn't moved while on top of the GUI surface. Don't post
            // the event to the GUI if the position is not inside the surface's area.
            unsigned int surfaceWidth;
            unsigned int surfaceHeight;
            m_gui.GetSurfaceSize(hSurface, surfaceWidth, surfaceHeight);

            if (xPos > 0 && yPos > 0 && xPos < static_cast<int>(surfaceWidth) && yPos < static_cast<int>(surfaceHeight))
            {
                m_gui.OnMouseButtonPressed(hSurface, button, xPos, yPos);
            }
        }
    }

    void Editor::OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos)
    {
        this->PostEvent_OnMouseButtonReleased(hWindow, button, xPos, yPos);

        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.OnMouseButtonReleased(hSurface, button, xPos, yPos);
        }
    }

    void Editor::OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos)
    {
        this->PostEvent_OnMouseButtonDoubleClick(hWindow, button, xPos, yPos);

        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            // The cursor position may be on the client area which means it wasn't moved while on top of the GUI surface. Don't post
            // the event to the GUI if the position is not inside the surface's area.
            unsigned int surfaceWidth;
            unsigned int surfaceHeight;
            m_gui.GetSurfaceSize(hSurface, surfaceWidth, surfaceHeight);

            if (xPos > 0 && yPos > 0 && xPos < static_cast<int>(surfaceWidth) && yPos < static_cast<int>(surfaceHeight))
            {
                m_gui.OnMouseButtonDoubleClicked(hSurface, button, xPos, yPos);
            }
        }
    }

    void Editor::OnMouseWheel(HWindow hWindow, int delta, int xPos, int yPos)
    {
        this->PostEvent_OnMouseWheel(hWindow, delta, xPos, yPos);

        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            // The cursor position may be on the client area which means it wasn't moved while on top of the GUI surface. Don't post
            // the event to the GUI if the position is not inside the surface's area.
            unsigned int surfaceWidth;
            unsigned int surfaceHeight;
            m_gui.GetSurfaceSize(hSurface, surfaceWidth, surfaceHeight);

            if (xPos > 0 && yPos > 0 && xPos < static_cast<int>(surfaceWidth) && yPos < static_cast<int>(surfaceHeight))
            {
                m_gui.OnMouseWheel(hSurface, delta, xPos, yPos);
            }
        }
    }

    void Editor::OnKeyPressed(HWindow hWindow, GTLib::Key key)
    {
        (void)hWindow;


        // Editor toggle.
        if (key == GTLib::Keys::Tab)
        {
            if (m_gameContext.IsKeyDown(GTLib::Keys::Shift))
            {
                if (this->IsOpen())
                {
                    this->Close();
                }
                else
                {
                    this->Open();
                }
            }
        }


        // Shortcuts
        if (m_gameContext.IsKeyDown(GTLib::Keys::Ctrl))
        {
            // Save
            if (key == GTLib::Keys::S) {
                this->SaveFile(this->GetFocusedFileAbsolutePath());
            }


            // Cut
            if (key == GTLib::Keys::X) {
                this->DoCut();
            }

            // Copy
            if (key == GTLib::Keys::C) {
                this->DoCopy();
            }

            // Paste
            if (key == GTLib::Keys::V) {
                this->DoPaste();
            }

            // Undo
            if (key == GTLib::Keys::Z) {
                this->DoUndo();
            }

            // Redo
            if (key == GTLib::Keys::Y) {
                this->DoRedo();
            }


            // Select All
            if (key == GTLib::Keys::A) {
                this->DoSelectAll();
            }
        }
        


        m_gui.OnKeyPressed(key);
    }

    void Editor::OnKeyPressedAutoRepeat(HWindow hWindow, GTLib::Key key)
    {
        (void)hWindow;

        m_gui.OnKeyPressedAutoRepeat(key);
    }

    void Editor::OnKeyReleased(HWindow hWindow, GTLib::Key key)
    {
        (void)hWindow;

        m_gui.OnKeyReleased(key);
    }

    void Editor::OnPrintableKeyDown(HWindow hWindow, char32_t character)
    {
        (void)hWindow;

        m_gui.OnPrintableKeyDown(character);
    }

    void Editor::OnPaintWindow(HWindow hWindow, const GTLib::Rect<int> &rect)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.PaintSurface(hSurface, rect, nullptr);
        }
    }


    void Editor::OnFileCreated(const char* absolutePath)
    {
        printf("FILE CREATED: %s\n", absolutePath);
    }

    void Editor::OnFileDeleted(const char* absolutePath)
    {
        printf("FILE DELETED: %s\n", absolutePath);
    }

    void Editor::OnFileRenamed(const char* absolutePathOld, const char* absolutePathNew)
    {
        printf("FILE RENAMED: %s -> %s\n", absolutePathOld, absolutePathNew);
    }

    void Editor::OnFileUpdated(const char* absolutePath)
    {
        printf("FILE UPDATED: %s\n", absolutePath);
    }



    //////////////////////////////////////////////
    // Private

    bool Editor::GetWindowSurfaceAndElement(HWindow hWindow, WindowGUISurfaceAndElement &surfaceAndElementOut) const
    {
        auto iSurface = m_windowSurfaceMap.Find(hWindow);
        if (iSurface != nullptr)
        {
            surfaceAndElementOut = iSurface->value;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Editor::CreateWindowSurfaceAndElement(HWindow hWindow, WindowGUISurfaceAndElement &surfaceAndElementOut)
    {
        if (hWindow != NULL)
        {
            HGUISurface hSurface = surfaceAndElementOut.hSurface;
            HGUISurface hElement = surfaceAndElementOut.hElement;

            // If the window already has a surface and element, return immediately.
            if (this->GetWindowSurfaceAndElement(hWindow, surfaceAndElementOut))
            {
                return true;
            }


            // If we get here the surface does not already exist.
            unsigned int windowWidth;
            unsigned int windowHeight;
            if (m_gameContext.GetWindowSize(hWindow, windowWidth, windowHeight))
            {
                if (hSurface == NULL)
                {
                    hSurface = m_gui.CreateSurface();
                }

                if (hSurface != NULL)
                {
                    unsigned int xDPI;
                    unsigned int yDPI;
                    m_gameContext.GetWindowManager().GetWindowDPI(hWindow, xDPI, yDPI);
                    m_gui.SetDPI(hSurface, xDPI, yDPI);


                    m_gui.SetSurfaceSize(hSurface, windowWidth, windowHeight);
                    m_gui.SetSurfaceUserData(hSurface, new EditorGUISurfaceAUXData(hWindow));

                    if (hElement == NULL)
                    {
                        hElement = m_gui.CreateElement();
                    }
                    
                    if (hElement != NULL)
                    {
                        m_gui.AttachElementToSurface(hElement, hSurface);
                        m_gui.SetElementSizeRatio(hElement, 1.0f, 1.0f);
                        m_gui.SetElementBackgroundColor(hElement, m_theme.backgroundDark);
                        m_gui.EnableElementChildWidthFlexing(hElement);
                        m_gui.EnableElementChildHeightFlexing(hElement);

                        surfaceAndElementOut.hSurface = hSurface;
                        surfaceAndElementOut.hElement = hElement;

                        // The window needs to be mapped to the surface.
                        m_windowSurfaceMap.Add(hWindow, surfaceAndElementOut);

                        return true;
                    }
                }
            }
        }
        
        return false;
    }

    bool Editor::CreateWindowSurfaceAndElement(HWindow hWindow)
    {
        WindowGUISurfaceAndElement unused;
        return this->CreateWindowSurfaceAndElement(hWindow, unused);
    }


    void Editor::DeleteWindowSurfaceAndElement(HWindow hWindow)
    {
        WindowGUISurfaceAndElement surfaceAndElement;
        if (this->GetWindowSurfaceAndElement(hWindow, surfaceAndElement))
        {
            // TODO: Delete the elements that are attached to the surface.

            EditorGUISurfaceAUXData* pSurfaceUserData = m_gui.GetSurfaceUserData<EditorGUISurfaceAUXData>(surfaceAndElement.hSurface);
            if (pSurfaceUserData != nullptr)
            {
                // TODO: Improve this a bit - don't like having platform-specific code here. Also looks a bit out of place...
#if defined(GT_PLATFORM_WINDOWS)
                if (pSurfaceUserData->hMemDC != NULL)
                {
                    DeleteObject(reinterpret_cast<HANDLE>(pSurfaceUserData->hMemBitmap));
                    DeleteDC(reinterpret_cast<HDC>(pSurfaceUserData->hMemDC));
                }
#endif

                delete pSurfaceUserData;
            }

            m_gui.DeleteSurface(surfaceAndElement.hSurface);
            m_gui.DeleteElement(surfaceAndElement.hElement);


            // Remove the window/surface mapping.
            m_windowSurfaceMap.RemoveByKey(hWindow);
        }
    }

    void Editor::DeleteAllWindowSurfacesAndElements()
    {
        while (m_windowSurfaceMap.count > 0)
        {
            this->DeleteWindowSurfaceAndElement(m_windowSurfaceMap.buffer[0]->key);
        }

        assert(m_windowSurfaceMap.count == 0);
    }


    void Editor::PostEvent_OnEditorOpened()
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnEditorOpened();
            }
        }
    }

    void Editor::PostEvent_OnEditorClosed()
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnEditorClosed();
            }
        }
    }


    void Editor::PostEvent_OnWindowActivated(HWindow hWindow)
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnWindowActivated(hWindow);
            }
        }
    }

    void Editor::PostEvent_OnWindowDeactivated(HWindow hWindow)
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnWindowDeactivated(hWindow);
            }
        }
    }

    void Editor::PostEvent_OnMouseMove(HWindow hWindow, int xPos, int yPos)
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnMouseMove(hWindow, xPos, yPos);
            }
        }
    }

    void Editor::PostEvent_OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos)
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnMouseButtonPressed(hWindow, button, xPos, yPos);
            }
        }
    }

    void Editor::PostEvent_OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos)
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnMouseButtonReleased(hWindow, button, xPos, yPos);
            }
        }
    }

    void Editor::PostEvent_OnMouseButtonDoubleClick(HWindow hWindow, int button, int xPos, int yPos)
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnMouseButtonDoubleClick(hWindow, button, xPos, yPos);
            }
        }
    }

    void Editor::PostEvent_OnMouseWheel(HWindow hWindow, int delta, int xPos, int yPos)
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnMouseWheel(hWindow, delta, xPos, yPos);
            }
        }
    }
}
