// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Editor.hpp>
#include <GTGE/Game.hpp>
#include <GTGE/IO.hpp>
#include <GTGE/GTEngine.hpp>
#include <GTGE/ParticleSystemLibrary.hpp>
#include <GTGE/ScriptLibrary.hpp>
#include <GTGE/GUI/GUIServer.hpp>
#include <GTGE/Core/Keyboard.hpp>
#include <easy_path/easy_path.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GT
{
    // The "paths" of special editors such as the packaging tool.
    static const char* PackagingToolPath = "@PackagingTool";


    Editor::Editor(Game &game)
        : game(game),
          m_pFSW(nullptr),// m_FSWThread(nullptr),
          openedFiles(), currentlyShownEditor(nullptr),
          GUI(),
          lastProfilingUpdateTime(0.0),
          isStarted(false), isOpen(false)
    {
    }

    Editor::~Editor()
    {
        for (size_t i = 0; i < this->openedFiles.count; ++i)
        {
            delete this->openedFiles.buffer[i]->value;
        }
    }

    bool Editor::Startup(GUIServer &guiServer)
    {
        if (!this->isStarted)
        {
            this->StartupScripting();

            auto &script = guiServer.GetScriptServer().GetScript();

            // We need to grab the main elements from the server.
            if (guiServer.LoadFromFile("engine/editor/main.xml"))
            {
                this->GUI.EditorMain                  = guiServer.GetElementByID("EditorMain");
                this->GUI.EditorCenterCenterPanel     = guiServer.GetElementByID("EditorCenterCenterPanel");
                this->GUI.EditorCenterCenterPanelHelp = guiServer.GetElementByID("EditorCenterCenterPanelHelp");
                this->GUI.Editor_Delta                = guiServer.GetElementByID("Editor_Delta");
                this->GUI.Editor_FPS                  = guiServer.GetElementByID("Editor_FPS");

                // We need to grab the GUI elements for the menu buttons.
                script.Get("GTGUI.Server.GetElementByID('Editor_MenuBar')");
                assert(script.IsTable(-1));
                {
                    script.Push("File");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.Push("Save");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("GetID");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);   // 'self'
                                script.Call(1, 1);

                                this->GUI.File_Save = guiServer.GetElementByID(script.ToString(-1));

                                script.Pop(1);  // Return value from GetID().
                            }
                        }
                        script.Pop(1);

                        script.Push("SaveAll");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("GetID");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);   // 'self'
                                script.Call(1, 1);

                                this->GUI.File_SaveAll = guiServer.GetElementByID(script.ToString(-1));

                                script.Pop(1);  // Return value from GetID().
                            }
                        }
                        script.Pop(1);

                        script.Push("Close");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("GetID");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);   // 'self'
                                script.Call(1, 1);

                                this->GUI.File_Close = guiServer.GetElementByID(script.ToString(-1));

                                script.Pop(1);  // Return value from GetID().
                            }
                        }
                        script.Pop(1);

                        script.Push("CloseAll");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("GetID");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);   // 'self'
                                script.Call(1, 1);

                                this->GUI.File_CloseAll = guiServer.GetElementByID(script.ToString(-1));

                                script.Pop(1);  // Return value from GetID().
                            }
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);
                }
                script.Pop(1);


                // We actually want the editor element to be the first child of the root. If we don't do this, sometimes a game can be in a state
                // where the editor will be placed underneath another GUI element, causing it to not look quite right.
                guiServer.GetRootElement()->PrependChild(*this->GUI.EditorMain);

                this->isStarted = true;
            }
            else
            {
                g_Context->LogErrorf("Error initializing editor: Failed to load editor/main.xml. Ensure the 'editor' directory exists. %s.", script.ToString(-1));
                return false;
            }
        }

        return true;
    }

    void Editor::Open()
    {
        if (!this->isOpen && this->GUI.EditorMain != nullptr)
        {
            this->game.ShowCursor();
            this->GUI.EditorMain->Show();

            // We start up the file system watcher before doing the initial directory iteration.
            this->StartupFileSystemWatcher();

            // We need to iterate over every file and folder in each base directory and make the editor aware of it.
            for (unsigned int iBaseDir = 0; iBaseDir < easyvfs_get_base_directory_count(g_Context->GetVFS()); ++iBaseDir)
            {
                const char* baseDir = easyvfs_get_base_directory_by_index(g_Context->GetVFS(), iBaseDir);
                assert(baseDir != nullptr);

                this->InsertDirectoryChildren_Recursive(baseDir);
            }


            this->isOpen = true;
        }
    }

    void Editor::Close()
    {
        if (this->isOpen)
        {
            this->GUI.EditorMain->Hide();

            // We need to make sure everything is saved.
            this->SaveAllOpenModifiedFiles();

            this->ShutdownFileSystemWatcher();
            this->isOpen = false;
        }
    }


    SubEditor* Editor::OpenFile(const char* path, const char* relativeTo)
    {
        bool isSpecialEditor = this->IsSpecialPath(path);


        // We need to make sure we have an absolute and relative path.
        char absolutePath[EASYVFS_MAX_PATH];
        char relativePath[EASYVFS_MAX_PATH];

        if (!isSpecialEditor)
        {
            if (easypath_is_absolute(path))
            {
                if (relativeTo != nullptr)
                {
                    strcpy_s(absolutePath, sizeof(absolutePath), path);
                    easypath_to_relative(path, relativeTo, relativePath, sizeof(relativePath));
                }
                else
                {
                    // We're unable to retrieve the relative path because 'path' is absolute and 'relativeTo' is null.
                    return nullptr;
                }
            }
            else
            {
                // The file needs to exist. If it doesn't, we need to return false.
                if (easyvfs_find_absolute_path(g_Context->GetVFS(), path, absolutePath, sizeof(absolutePath)))
                {
                    strcpy_s(relativePath, sizeof(relativePath), path);
                }
                else
                {
                    // The file might have an associated .gtmodel file. We'll let it pass if so.
                    if (GT::IsSupportedModelExtension(path))
                    {
                        char pathWithExt[EASYVFS_MAX_PATH];
                        easypath_copy_and_append_extension(pathWithExt, sizeof(pathWithExt), path, "gtmodel");

                        if (easyvfs_find_absolute_path(g_Context->GetVFS(), pathWithExt, absolutePath, sizeof(absolutePath))) {
                            strcpy_s(relativePath, sizeof(relativePath), path);
                        }
                    }
                    else
                    {
                        g_Context->LogErrorf("Editor: Can not open file '%s'. Check that the file exists or if it's already in use.\n", path);
                        return nullptr;
                    }
                }
            }
        }
        else
        {
            strcpy_s(absolutePath, sizeof(absolutePath), path);
            strcpy_s(relativePath, sizeof(relativePath), path);
        }


        // At this point, we will have absolute and relative paths. We now need to check if the file is already open. If so, we just switch to it. Otherwise, we need
        // to open it.
        SubEditor* newSubEditor = nullptr;

        auto iExistingSubEditor = this->openedFiles.Find(absolutePath);
        if (iExistingSubEditor == nullptr)
        {
            if (!isSpecialEditor)
            {
                // We'll check if the file exists from here.
                if (!easyvfs_is_existing_file(g_Context->GetVFS(), absolutePath))
                {
                    // The file doesn't exist, but it might be a model so we'll need to check if it's got an associated .gtmodel file.
                    if (GT::IsSupportedModelExtension(absolutePath))
                    {
                        char absolutePathWithExt[EASYVFS_MAX_PATH];
                        easypath_copy_and_append_extension(absolutePathWithExt, sizeof(absolutePathWithExt), absolutePath, "gtmodel");

                        if (!easyvfs_is_existing_file(g_Context->GetVFS(), absolutePathWithExt))
                        {
                            g_Context->LogErrorf("Editor: Can not open model file '%s'. Associated .gtmodel file does not exist.\n", path);
                            return nullptr;
                        }
                    }
                    else
                    {
                        g_Context->LogErrorf("Editor: Can not open file '%s'. Does not exist.\n", path);
                        return nullptr;
                    }
                }


                // The file exists, so now we just create our sub-editor. The specific sub-editor will be based on the file name.
                AssetClass type = GT::GetAssetClassFromExtension(absolutePath);

                switch (type)
                {
                case AssetClass_Image:
                    {
                        newSubEditor = new ImageEditor(*this, absolutePath, relativePath);
                        break;
                    }

                case AssetClass_Model:
                    {
                        newSubEditor = new ModelEditor(*this, absolutePath, relativePath);
                        break;
                    }

                case AssetClass_Material:
                    {
                        newSubEditor = new MaterialEditor(*this, absolutePath, relativePath);
                        break;
                    }


                case AssetClass_Scene:
                    {
                        newSubEditor = new SceneEditor(*this, absolutePath, relativePath);
                        break;
                    }

                case AssetClass_ParticleSystem:
                    {
                        newSubEditor = new ParticleEditor(*this, absolutePath, relativePath);
                        break;
                    }

                case AssetClass_Script:
                case AssetClass_TextFile:
                    {
                        newSubEditor = new TextEditor(*this, absolutePath, relativePath);
                        break;
                    }


                case AssetClass_Sound:
                case AssetClass_Prefab:
                case AssetClass_Unknown:
                default:
                    {
                        // If we get here it means we don't have a sub editor for the given asset type. We will post a warning and just create
                        // a SubEditor object for it.
                        g_Context->Logf("Warning: Editor: An editor is not currently supported for the given asset. '%s'.", path);
                        newSubEditor = new SubEditor(*this, absolutePath, relativePath);
                    }
                }
            }
            else
            {
                // Specials.
                if (Strings::Equal<false>(absolutePath, PackagingToolPath))
                {
                    newSubEditor = new PackagingToolEditor(*this, absolutePath);
                }
            }


            if (newSubEditor != nullptr)
            {
                // At this point we will have a sub-editor and all we need to do is add it to our list and show it.
                this->openedFiles.Add(absolutePath, newSubEditor);

                // There is a center panel that needs to be shown. It is the center, center panel.
                this->GUI.EditorCenterCenterPanel->Show();
                this->GUI.EditorCenterCenterPanelHelp->Hide();
            }
        }
        else
        {
            newSubEditor = iExistingSubEditor->value;
        }

        // Now we just to need show the newly opened file.
        this->ShowFile(absolutePath);

        return newSubEditor;
    }

    void Editor::CloseFile(const char* path, const char* relativeTo)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        strcpy_s(absolutePath, sizeof(absolutePath), path);

        if (!this->IsSpecialPath(path))
        {
            if (easypath_is_relative(path))
            {
                if (relativeTo != nullptr)
                {
                    easypath_copy_and_append(absolutePath, sizeof(absolutePath), relativeTo, path);
                }
                else
                {
                    // We can not find the absolute path because 'path' is relative and 'relativeTo' is null.
                    return;
                }
            }
        }

        // At this point we will have our absolute path. We need to retrieve the sub editor, and call it's hide function before we completely delete it.
        auto iEditor = this->openedFiles.Find(absolutePath);
        if (iEditor != nullptr)
        {
            auto editor = iEditor->value;
            assert(editor != nullptr);
            {
                // If the file is modified, what we actually want to do is show the Save File dialog. This dialog will control whether or not
                // the file is saved and closed.
                if (editor->IsMarkedAsModified())
                {
                    this->ShowSaveFileDialog(absolutePath);
                }
                else
                {
                    this->ForceCloseFile(absolutePath);
                }
            }
        }
    }

    void Editor::ForceCloseFile(const char* path, const char* relativeTo)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        strcpy_s(absolutePath, sizeof(absolutePath), path);

        if (!this->IsSpecialPath(path))
        {
            if (easypath_is_relative(path))
            {
                if (relativeTo != nullptr)
                {
                    easypath_copy_and_append(absolutePath, sizeof(absolutePath), relativeTo, path);
                }
                else
                {
                    // We can not find the absolute path because 'path' is relative and 'relativeTo' is null.
                    return;
                }
            }
        }

        // At this point we will have our absolute path. We need to retrieve the sub editor, and call it's hide function before we completely delete it.
        auto iEditor = this->openedFiles.Find(absolutePath);
        if (iEditor != nullptr)
        {
            auto editor = iEditor->value;
            assert(editor != nullptr);
            {
                if (this->currentlyShownEditor != nullptr && absolutePath == this->currentlyShownEditor->GetAbsolutePath())
                {
                    this->HideCurrentlyShownFile();
                }

                editor->DeleteToolbar();

                delete editor;
                this->openedFiles.RemoveByIndex(iEditor->index);


                // If there are no files open, we need to hide the center, center panel.
                if (this->openedFiles.count == 0)
                {
                    this->GUI.EditorCenterCenterPanel->Hide();
                    this->GUI.EditorCenterCenterPanelHelp->Show();
                }


                // Menu buttons need to be updated.
                this->UpdateMenuButtonEnableStates();
            }
        }
    }

    void Editor::CloseAllOpenFiles()
    {
        while (this->openedFiles.count > 0)
        {
            auto subEditor = this->openedFiles.buffer[0]->value;
            assert(subEditor != nullptr);
            {
                if (subEditor->IsMarkedAsModified())
                {
                    subEditor->Save();
                }

                subEditor->Close();
            }
        }
    }

    void Editor::CloseCurrentlyShownFile()
    {
        if (this->currentlyShownEditor != nullptr)
        {
            this->currentlyShownEditor->Close();
        }
    }


    bool Editor::ShowFile(const char* path, const char* relativeTo)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        strcpy_s(absolutePath, sizeof(absolutePath), path);

        if (!this->IsSpecialPath(path))
        {
            if (easypath_is_relative(path))
            {
                if (relativeTo != nullptr)
                {
                    easypath_copy_and_append(absolutePath, sizeof(absolutePath), relativeTo, path);
                }
                else
                {
                    // We can not find the absolute path because 'path' is relative and 'relativeTo' is null.
                    return false;
                }
            }
        }

        auto iEditorToShow = this->openedFiles.Find(absolutePath);
        if (iEditorToShow != nullptr)
        {
            auto editorToShow = iEditorToShow->value;
            assert(editorToShow != nullptr);
            {
                // The previously shown editor needs to be hidden.
                this->HideCurrentlyShownFile();

                // Now what we do is activate the tab. We do NOT want to post an OnTabActivated event from this, because otherwise
                // we'll end up getting stuck recursively. To prevent this, we simple pass 'true' to the second argument of ActivateTab().
                auto &script = this->GetGame().GetScript();

                script.Get("GTGUI.Server.GetElementByID('Editor_TabBar')");
                assert(script.IsTable(-1));
                {
                    script.Push("ActivateTab");
                    script.GetTableValue(-2);
                    assert(script.IsFunction(-1));
                    {
                        script.PushValue(-2);       // 'self'.

                        // We need to get the element.
                        script.GetGlobal("GTGUI");
                        assert(script.IsTable(-1));
                        {
                            script.Push("Server");
                            script.GetTableValue(-2);
                            assert(script.IsTable(-1));
                            {
                                script.Push("GetElementByID");
                                script.GetTableValue(-2);
                                assert(script.IsFunction(-1));
                                {
                                    script.Push(editorToShow->GetTabElement()->id);
                                    script.Call(1, 1);

                                    // The return value needs to be relocated so that it is placed as the second argument to Editor_TabBar:RemoveTab().
                                    script.InsertIntoStack(-3);
                                }
                            }
                            script.Pop(1);  // GTGUI.Server
                        }
                        script.Pop(1);      // GTGUI

                        // We need to pass true as the last argument (blockEvent) so that we don't post an OnTabActivated event, which would in turn result in us getting stuck in a recursive loop.
                        script.Push(true);

                        // Now we need to call the function, ignoring any return values.
                        script.Call(3, 0);
                    }
                }
                script.Pop(1);


                // Now we can set and show the newly showing editor.
                this->currentlyShownEditor = editorToShow;
                this->currentlyShownEditor->Show();
            }
        }

        // Menu buttons need to be updated.
        this->UpdateMenuButtonEnableStates();

        return true;
    }

    void Editor::HideCurrentlyShownFile()
    {
        if (this->currentlyShownEditor != nullptr)
        {
            this->currentlyShownEditor->Hide();
            this->currentlyShownEditor = nullptr;
        }
    }


    bool Editor::SaveFile(const char* path, const char* relativeTo)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        strcpy_s(absolutePath, sizeof(absolutePath), path);

        if (!this->IsSpecialPath(path))
        {
            if (easypath_is_relative(path))
            {
                if (relativeTo != nullptr)
                {
                    //absolutePath = IO::ToAbsolutePath(path, relativeTo);
                    easypath_copy_and_append(absolutePath, sizeof(absolutePath), relativeTo, path);
                }
                else
                {
                    // We can not find the absolute path because 'path' is relative and 'relativeTo' is null.
                    return false;
                }
            }
        }

        auto iSubEditor = this->openedFiles.Find(absolutePath);
        if (iSubEditor != nullptr)
        {
            auto subEditor = iSubEditor->value;
            assert(subEditor != nullptr);
            {
                if (subEditor->Save())
                {
                    this->UpdateMenuButtonEnableStates();
                    return true;
                }
            }
        }

        return false;
    }

    void Editor::SaveAllOpenModifiedFiles()
    {
        for (size_t i = 0; i < this->openedFiles.count; ++i)
        {
            auto subEditor = this->openedFiles.buffer[i]->value;
            assert(subEditor != nullptr);
            {
                if (subEditor->IsMarkedAsModified())
                {
                    subEditor->Save();
                }
            }
        }
    }

    bool Editor::SaveCurrentlyShownFile()
    {
        if (this->currentlyShownEditor != nullptr)
        {
            return this->currentlyShownEditor->Save();
        }

        return false;
    }


    void Editor::MarkFileAsModified(const char* path, const char* relativeTo)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        strcpy_s(absolutePath, sizeof(absolutePath), path);

        if (!this->IsSpecialPath(path))
        {
            if (easypath_is_relative(path))
            {
                if (relativeTo != nullptr)
                {
                    //absolutePath = IO::ToAbsolutePath(path, relativeTo);
                    easypath_copy_and_append(absolutePath, sizeof(absolutePath), relativeTo, path);
                }
                else
                {
                    // We can not find the absolute path because 'path' is relative and 'relativeTo' is null.
                    return;
                }
            }
        }

        auto iSubEditor = this->openedFiles.Find(absolutePath);
        if (iSubEditor != nullptr)
        {
            auto subEditor = iSubEditor->value;
            assert(subEditor != nullptr);
            {
                subEditor->MarkAsModified();
                this->UpdateMenuButtonEnableStates();
            }
        }
    }

    void Editor::UnmarkFileAsModified(const char* path, const char* relativeTo)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        strcpy_s(absolutePath, sizeof(absolutePath), path);

        if (!this->IsSpecialPath(path))
        {
            if (easypath_is_relative(path))
            {
                if (relativeTo != nullptr)
                {
                    //absolutePath = IO::ToAbsolutePath(path, relativeTo);
                    easypath_copy_and_append(absolutePath, sizeof(absolutePath), relativeTo, path);
                }
                else
                {
                    // We can not find the absolute path because 'path' is relative and 'relativeTo' is null.
                    return;
                }
            }
        }

        auto iSubEditor = this->openedFiles.Find(absolutePath);
        if (iSubEditor != nullptr)
        {
            auto subEditor = iSubEditor->value;
            assert(subEditor != nullptr);
            {
                subEditor->UnmarkAsModified();
                this->UpdateMenuButtonEnableStates();
            }
        }
    }


    bool Editor::IsFileMarkedAsModified(const char* path, const char* relativeTo)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        strcpy_s(absolutePath, sizeof(absolutePath), path);

        if (!this->IsSpecialPath(path))
        {
            if (easypath_is_relative(path))
            {
                if (relativeTo != nullptr)
                {
                    easypath_copy_and_append(absolutePath, sizeof(absolutePath), relativeTo, path);
                }
                else
                {
                    // We can not find the absolute path because 'path' is relative and 'relativeTo' is null.
                    return false;
                }
            }
        }

        auto iSubEditor = this->openedFiles.Find(absolutePath);
        if (iSubEditor != nullptr)
        {
            auto subEditor = iSubEditor->value;
            assert(subEditor != nullptr);
            {
                return subEditor->IsMarkedAsModified();
            }
        }

        return false;
    }

    bool Editor::IsAnyOpenFileMarkedAsModified()
    {
        for (size_t i = 0; i < this->openedFiles.count; ++i)
        {
            auto subEditor = this->openedFiles.buffer[i]->value;
            assert(subEditor != nullptr);
            {
                if (subEditor->IsMarkedAsModified())
                {
                    return true;
                }
            }
        }

        return false;
    }


    SubEditor* Editor::OpenPackagingTool()
    {
        return this->OpenFile(PackagingToolPath);
    }


    void Editor::UpdateMenuButtonEnableStates()
    {
        if (this->openedFiles.count > 0)
        {
            this->GUI.File_Close->Enable();
            this->GUI.File_CloseAll->Enable();

            if (this->IsAnyOpenFileMarkedAsModified())
            {
                if (this->currentlyShownEditor != nullptr && this->currentlyShownEditor->IsMarkedAsModified())
                {
                    this->GUI.File_Save->Enable();
                }

                this->GUI.File_SaveAll->Enable();
            }
            else
            {
                this->GUI.File_Save->Disable();
                this->GUI.File_SaveAll->Disable();
            }
        }
        else
        {
            this->GUI.File_Save->Disable();
            this->GUI.File_SaveAll->Disable();
            this->GUI.File_Close->Disable();
            this->GUI.File_CloseAll->Disable();
        }
    }


    GUIElement* Editor::GetFileEditorElement(const char* path, const char* relativeTo)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        strcpy_s(absolutePath, sizeof(absolutePath), path);

        if (!this->IsSpecialPath(path))
        {
            if (easypath_is_relative(path))
            {
                if (relativeTo != nullptr)
                {
                    easypath_copy_and_append(absolutePath, sizeof(absolutePath), relativeTo, path);
                }
                else
                {
                    // We can not find the absolute path because 'path' is relative and 'relativeTo' is null.
                    return nullptr;
                }
            }
        }

        auto iSubEditor = this->openedFiles.Find(absolutePath);
        if (iSubEditor != nullptr)
        {
            auto subEditor = iSubEditor->value;
            assert(subEditor != nullptr);
            {
                return subEditor->GetMainElement();
            }
        }

        return nullptr;
    }


    void Editor::Update(double deltaTimeInSeconds)
    {
        // Check for changes to the file system.
        easyfsw_event e;
        while (easyfsw_peek_event(m_pFSW, &e))
        {
            switch (e.type)
            {
                case easyfsw_event_type_created: this->OnFileInsert(e.absolutePath); break;
                case easyfsw_event_type_deleted: this->OnFileRemove(e.absolutePath); break;
                case easyfsw_event_type_renamed: this->OnFileRename(e.absolutePath, e.absolutePathNew); break;
                case easyfsw_event_type_updated: this->OnFileUpdate(e.absolutePath); break;
                default: break;
            }
        }




        if (this->currentlyShownEditor != nullptr)
        {
            this->currentlyShownEditor->OnUpdate(deltaTimeInSeconds);
        }



        // We need to update the profiling GUI.
        if (Timing::GetTimeInSeconds() - this->lastProfilingUpdateTime > 1.0)
        {
            this->lastProfilingUpdateTime = Timing::GetTimeInSeconds();


            double delta = this->game.GetProfiler().GetAverageFrameTime();
            double fps   = 0.0;

            if (delta > 0.0)
            {
                fps = 1.0 / delta;
            }


            char deltaStr[64];
            IO::snprintf(deltaStr, 64, "%f", delta);

            char fpsStr[64];
            IO::snprintf(fpsStr, 64, "%.1f", fps);


            this->GUI.Editor_Delta->SetText(deltaStr);
            this->GUI.Editor_FPS->SetText(fpsStr);
        }
    }

    void Editor::OnKeyPressed(Key key)
    {
        if (this->currentlyShownEditor != nullptr)
        {
            this->currentlyShownEditor->OnKeyPressed(key);
        }
    }

    void Editor::OnKeyReleased(Key key)
    {
        if (this->currentlyShownEditor != nullptr)
        {
            this->currentlyShownEditor->OnKeyReleased(key);
        }
    }

    void Editor::OnMouseButtonDown(MouseButton button, int x, int y)
    {
        if (this->currentlyShownEditor != nullptr)
        {
            this->currentlyShownEditor->OnMouseButtonDown(button, x, y);
        }
    }

    void Editor::OnMouseButtonUp(MouseButton button, int x, int y)
    {
        if (this->currentlyShownEditor != nullptr)
        {
            this->currentlyShownEditor->OnMouseButtonUp(button, x, y);
        }
    }

    void Editor::OnReceiveFocus()
    {
        for (size_t iSubEditor = 0; iSubEditor < this->openedFiles.count; ++iSubEditor)
        {
            this->openedFiles.buffer[iSubEditor]->value->OnMainWindowReceiveFocus();
        }
    }

    void Editor::OnLoseFocus()
    {
        for (size_t iSubEditor = 0; iSubEditor < this->openedFiles.count; ++iSubEditor)
        {
            this->openedFiles.buffer[iSubEditor]->value->OnMainWindowLoseFocus();
        }
    }


    void Editor::OnFileInsert(const char* absolutePath)
    {
        auto &script = this->game.GetScript();

        script.GetGlobal("Editor");
        assert(script.IsTable(-1));
        {
            script.Push("DataFilesWatcher");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("OnInsert");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(absolutePath);
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }
        script.Pop(1);
        


        // We're now going to let every sub-editor know about this.
        for (size_t i = 0; i < this->openedFiles.count; ++i)
        {
            auto subEditor = this->openedFiles.buffer[i]->value;
            assert(subEditor != nullptr);
            {
                subEditor->OnFileInsert(absolutePath);
            }
        }
    }

    void Editor::OnFileRemove(const char* absolutePath)
    {
        auto &script = this->game.GetScript();

        script.GetGlobal("Editor");
        assert(script.IsTable(-1));
        {
            script.Push("DataFilesWatcher");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("OnRemove");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(absolutePath);
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }
        script.Pop(1);


        // We're now going to let every sub-editor know about this.
        for (size_t i = 0; i < this->openedFiles.count; ++i)
        {
            auto subEditor = this->openedFiles.buffer[i]->value;
            assert(subEditor != nullptr);
            {
                subEditor->OnFileRemove(absolutePath);
            }
        }
    }

    void Editor::OnFileRename(const char* absolutePathOld, const char* absolutePathNew)
    {
        auto &script = this->game.GetScript();

        script.GetGlobal("Editor");
        assert(script.IsTable(-1));
        {
            script.Push("DataFilesWatcher");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("OnRename");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(absolutePathOld);
                    script.Push(absolutePathNew);
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }
        script.Pop(1);


        // We're now going to let every sub-editor know about this.
        for (size_t i = 0; i < this->openedFiles.count; ++i)
        {
            auto subEditor = this->openedFiles.buffer[i]->value;
            assert(subEditor != nullptr);
            {
                subEditor->OnFileRename(absolutePathOld, absolutePathNew);
            }
        }
    }

    void Editor::OnFileUpdate(const char* absolutePath)
    {
        // If the file is an asset, we need to update everything that is using it. We do this via the asset libraries.
        if (!easyvfs_is_existing_directory(g_Context->GetVFS(), absolutePath))
        {
            // It's not a directory.

            auto extension = easypath_extension(absolutePath);

            if (ModelLibrary::IsExtensionSupported(extension))
            {
                ModelLibrary::Reload(absolutePath);
            }
            else if (Texture2DLibrary::IsExtensionSupported(extension))
            {
                Texture2DLibrary::Reload(absolutePath);
            }
            else if (GT::IsSupportedMaterialExtension(absolutePath))
            {
                MaterialLibrary::Reload(absolutePath);
            }
            else if (GT::IsSupportedParticleSystemExtension(absolutePath))
            {
                ParticleSystemLibrary::Reload(absolutePath);
            }
            else
            {
                // It might be a script file. We'll try reloading.
                ScriptLibrary::Reload(absolutePath);

                // If we have a script file we will reload it if applicable.
                if (this->GetGame().GetScript().HasFileBeenLoaded(absolutePath))
                {
                    this->GetGame().GetScript().ExecuteFile(g_Context->GetVFS(), absolutePath);
                }
            }
        }



        auto &script = this->game.GetScript();

        script.GetGlobal("Editor");
        assert(script.IsTable(-1));
        {
            script.Push("DataFilesWatcher");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("OnUpdate");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(absolutePath);
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }
        script.Pop(1);



        // We're now going to let every sub-editor know about this.
        for (size_t i = 0; i < this->openedFiles.count; ++i)
        {
            auto subEditor = this->openedFiles.buffer[i]->value;
            assert(subEditor != nullptr);
            {
                subEditor->OnFileUpdate(absolutePath);
            }
        }
    }





    void Editor::ShowSaveFileDialog(const char* absolutePath)
    {
        // We let the scripting environment handle all of this.
        auto &script = this->game.GetScript();

        script.GetGlobal("Editor");
        assert(script.IsTable(-1));
        {
            script.Push("ShowSaveFileDialog");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.Push(absolutePath);
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }

    void Editor::StartupScripting()
    {
        auto &script = this->game.GetScript();

        script.Execute
        (
            "Editor.ImageEditor       = {};"
            "Editor.ModelEditor       = {};"
            "Editor.MaterialEditor    = {};"
            "Editor.SoundEditor       = {};"
            "Editor.ParticleEditor    = {};"
            "Editor.SceneEditor       = {};"
            "Editor.TextEditor        = {};"
            "Editor.PackingToolEditor = {};"
        );

        script.GetGlobal("Editor");
        if (script.IsTable(-1))
        {
            script.SetTableFunction(-1, "Open",                     FFI::Open);
            script.SetTableFunction(-1, "Close",                    FFI::Close);

            script.SetTableFunction(-1, "OpenFile",                 FFI::OpenFile);
            script.SetTableFunction(-1, "CloseFile",                FFI::CloseFile);
            script.SetTableFunction(-1, "ForceCloseFile",           FFI::ForceCloseFile);
            script.SetTableFunction(-1, "CloseAllOpenFiles",        FFI::CloseAllOpenFiles);
            script.SetTableFunction(-1, "CloseCurrentlyShownFile",  FFI::CloseCurrentlyShownFile);
            script.SetTableFunction(-1, "ShowFile",                 FFI::ShowFile);
            script.SetTableFunction(-1, "HideCurrentlyShownFile",   FFI::HideCurrentlyShownFile);
            script.SetTableFunction(-1, "SaveFile",                 FFI::SaveFile);
            script.SetTableFunction(-1, "SaveAllOpenModifiedFiles", FFI::SaveAllOpenModifiedFiles);
            script.SetTableFunction(-1, "SaveCurrentlyShownFile",   FFI::SaveCurrentlyShownFile);
            script.SetTableFunction(-1, "MarkFileAsModified",       FFI::MarkFileAsModified);
            script.SetTableFunction(-1, "UnmarkFileAsModified",     FFI::UnmarkFileAsModified);
            script.SetTableFunction(-1, "IsFileMarkedAsModified",   FFI::IsFileMarkedAsModified);
            script.SetTableFunction(-1, "GetCurrentlyShownEditor",  FFI::GetCurrentlyShownEditor);
            script.SetTableFunction(-1, "OpenPackagingTool",        FFI::OpenPackagingTool);
        }
        script.Pop(1);
    }


    bool Editor::IsSpecialPath(const char* path) const
    {
        return Strings::Equal<false>(path, PackagingToolPath);
    }




    ///////////////////////////////////////
    // Private

    void Editor::InsertDirectoryChildren_Recursive(const char* baseDir)
    {
        assert(easyvfs_is_existing_directory(g_Context->GetVFS(), baseDir));

        easyvfs_iterator iFile;
        if (easyvfs_begin_iteration(g_Context->GetVFS(), baseDir, &iFile))
        {
            easyvfs_file_info fi;
            while (easyvfs_next_iteration(g_Context->GetVFS(), &iFile, &fi))
            {
                if (!easyvfs_is_base_directory(g_Context->GetVFS(), fi.absolutePath))
                {
                    this->OnFileInsert(fi.absolutePath);

                    // Call this function recursively if the file is a directory and is not another base directory.
                    if ((fi.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) != 0)
                    {
                        this->InsertDirectoryChildren_Recursive(fi.absolutePath);
                    }
                }
            }
        }
    }


#if 0
    static int EditorFSWProc(void* pData)
    {
        Editor* pEditor = reinterpret_cast<Editor*>(pData);
        assert(pEditor != nullptr);

        easyfsw_context* pFSW = pEditor->GetFSW();
        assert(pFSW != nullptr);

        easyfsw_event e;
        while (pFSW != NULL && easyfsw_next_event(pFSW, &e))
        {
            switch (e.type)
            {
                case easyfsw_event_type_created: pEditor->OnFileInsert(e.absolutePath); break;
                case easyfsw_event_type_deleted: pEditor->OnFileRemove(e.absolutePath); break;
                case easyfsw_event_type_renamed: pEditor->OnFileRename(e.absolutePath, e.absolutePathNew); break;
                case easyfsw_event_type_updated: pEditor->OnFileUpdate(e.absolutePath); break;
                default: break;
            }
        }

        return 0;
    }
#endif

    void Editor::StartupFileSystemWatcher()
    {
        // Don't do anything if the file system watcher has already been initialized.
        if (m_pFSW == NULL)
        {
            m_pFSW = easyfsw_create_context();

            // We add the base directories based ont he virtual file system.
            for (unsigned int i = 0; i < easyvfs_get_base_directory_count(g_Context->GetVFS()); ++i)
            {
                easyfsw_add_directory(m_pFSW, easyvfs_get_base_directory_by_index(g_Context->GetVFS(), i));
            }


            // The file system watcher waits for events in the background in another thread.
            //assert(m_FSWThread == nullptr);
            //m_FSWThread = easyutil_create_thread(EditorFSWProc, this);
        }
    }

    void Editor::ShutdownFileSystemWatcher()
    {
        if (m_pFSW != NULL)
        {
            easyfsw_context* pFSW = m_pFSW;
            m_pFSW = NULL;
            
            // Deleting the context will cause the FSW thread to terminate.
            easyfsw_delete_context(pFSW);

            // The thread handle also needs to be deleted.
            //easyutil_wait_and_delete_thread(m_FSWThread);
            //m_FSWThread = NULL;
        }
    }


    /////////////////////////////////////////////
    // FFI.
    Game & Editor::FFI::GetGame(GT::Script &script)
    {
        script.GetGlobal("__GamePtr");
        auto game = static_cast<Game*>(script.ToPointer(-1));
        script.Pop(1);

        assert(game != nullptr);
        return *game;
    }

    Editor & Editor::FFI::GetEditor(GT::Script &script)
    {
        return GetGame(script).GetEditor();
    }


    void Editor::FFI::PushElement(GT::Script &script, GUIElement* element)
    {
        if (element != nullptr)
        {
            script.GetGlobal("GTGUI");
            assert(script.IsTable(-1));
            {
                script.Push("Server");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push("GetElementByID");
                    script.GetTableValue(-2);
                    assert(script.IsFunction(-1));
                    {
                        script.Push(element->id);
                        script.Call(1, 1);

                        script.InsertIntoStack(-3);     // <-- The final return value.
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }
        else
        {
            script.PushNil();
        }
    }


    int Editor::FFI::Open(GT::Script &script)
    {
        GetGame(script).OpenEditor();
        return 0;
    }

    int Editor::FFI::Close(GT::Script &script)
    {
        GetGame(script).CloseEditor();
        return 0;
    }


    int Editor::FFI::OpenFile(GT::Script &script)
    {
        auto subEditor = FFI::GetEditor(script).OpenFile(script.ToString(1), script.ToString(2));
        if (subEditor != nullptr)
        {
            Editor::FFI::PushElement(script, subEditor->GetMainElement());
        }
        else
        {
            script.PushNil();
        }

        return 1;
    }

    int Editor::FFI::CloseFile(GT::Script &script)
    {
        FFI::GetEditor(script).CloseFile(script.ToString(1), script.ToString(2));
        return 0;
    }

    int Editor::FFI::ForceCloseFile(GT::Script &script)
    {
        FFI::GetEditor(script).ForceCloseFile(script.ToString(1), script.ToString(2));
        return 0;
    }

    int Editor::FFI::CloseAllOpenFiles(GT::Script &script)
    {
        FFI::GetEditor(script).CloseAllOpenFiles();
        return 0;
    }

    int Editor::FFI::CloseCurrentlyShownFile(GT::Script &script)
    {
        FFI::GetEditor(script).CloseCurrentlyShownFile();
        return 0;
    }

    int Editor::FFI::ShowFile(GT::Script &script)
    {
        script.Push(FFI::GetEditor(script).ShowFile(script.ToString(1), script.ToString(2)));
        return 1;
    }

    int Editor::FFI::HideCurrentlyShownFile(GT::Script &script)
    {
        FFI::GetEditor(script).HideCurrentlyShownFile();
        return 0;
    }

    int Editor::FFI::SaveFile(GT::Script &script)
    {
        script.Push(FFI::GetEditor(script).SaveFile(script.ToString(1), script.ToString(2)));
        return 1;
    }

    int Editor::FFI::SaveAllOpenModifiedFiles(GT::Script &script)
    {
        FFI::GetEditor(script).SaveAllOpenModifiedFiles();
        return 0;
    }

    int Editor::FFI::SaveCurrentlyShownFile(GT::Script &script)
    {
        script.Push(FFI::GetEditor(script).SaveCurrentlyShownFile());
        return 1;
    }

    int Editor::FFI::MarkFileAsModified(GT::Script &script)
    {
        FFI::GetEditor(script).MarkFileAsModified(script.ToString(1), script.ToString(2));
        return 0;
    }

    int Editor::FFI::UnmarkFileAsModified(GT::Script &script)
    {
        FFI::GetEditor(script).UnmarkFileAsModified(script.ToString(1), script.ToString(2));
        return 0;
    }

    int Editor::FFI::IsFileMarkedAsModified(GT::Script &script)
    {
        script.Push(FFI::GetEditor(script).IsFileMarkedAsModified(script.ToString(1), script.ToString(2)));
        return 1;
    }

    int Editor::FFI::GetCurrentlyShownEditor(GT::Script &script)
    {
        auto subEditor = FFI::GetEditor(script).GetCurrentlyShownEditor();
        if (subEditor != nullptr)
        {
            Editor::FFI::PushElement(script, subEditor->GetMainElement());
        }
        else
        {
            script.PushNil();
        }

        return 1;
    }

    int Editor::FFI::OpenPackagingTool(GT::Script &script)
    {
        auto subEditor = FFI::GetEditor(script).OpenPackagingTool();
        if (subEditor != nullptr)
        {
            Editor::FFI::PushElement(script, subEditor->GetMainElement());
        }
        else
        {
            script.PushNil();
        }

        return 1;
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
