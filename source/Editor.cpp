// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/GUI/GUIServer.hpp>
#include <GTEngine/Core/Keyboard.hpp>
#include <easy_path/easy_path.h>

using namespace GT;     // <-- Delete me later when we change to the GT namespace!

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
          openedFiles(), currentlyShownEditor(nullptr),
          GUI(),
          lastProfilingUpdateTime(0.0),
          isStarted(false), isOpen(false), disableFileWatchingAfterClose(true),
          dataFilesWatcherEventHandler(*this)
    {
    }

    Editor::~Editor()
    {
        for (size_t i = 0; i < this->openedFiles.count; ++i)
        {
            delete this->openedFiles.buffer[i]->value;
        }
    }

    bool Editor::Startup(GTGUI::GUIServer &guiServer)
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


                // Here we need to attach our files watcher event handler.
                this->game.GetDataFilesWatcher().AddEventHandler(this->dataFilesWatcherEventHandler);


                this->isStarted = true;
            }
            else
            {
                GT::PostError("Error initializing editor: Failed to load editor/main.xml. Ensure the 'editor' directory exists. %s.", script.ToString(-1));
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

            // We always want to watch the data files while in the editor, but the game may or may not want to have watching enabled after the editor is closed. We will need
            // keep track of whether or not we should disable watching when the editor is closed.
            this->disableFileWatchingAfterClose = !this->game.IsDataFilesWatchingEnabled();
            this->game.EnableDataFilesWatching();

            // We also want to get an update on the data files immediately.
            this->game.GetDataFilesWatcher().CheckForChanges(false);
            this->game.GetDataFilesWatcher().DispatchEvents();


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

            // We want to update the data files so we can see them in-game.
            this->game.GetDataFilesWatcher().CheckForChanges(false);
            this->game.GetDataFilesWatcher().DispatchEvents();

            // We may want to disable file watching.
            if (this->disableFileWatchingAfterClose)
            {
                this->game.DisableDataFilesWatching();
            }


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
                if (easyvfs_find_absolute_path(g_EngineContext->GetVFS(), path, absolutePath, sizeof(absolutePath)))
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

                        if (easyvfs_find_absolute_path(g_EngineContext->GetVFS(), pathWithExt, absolutePath, sizeof(absolutePath))) {
                            strcpy_s(relativePath, sizeof(relativePath), path);
                        }
                    }
                    else
                    {
                        GT::PostError("Editor: Can not open file '%s'. Check that the file exists or if it's already in use.\n", path);
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
                if (!easyvfs_is_existing_file(g_EngineContext->GetVFS(), absolutePath))
                {
                    // The file doesn't exist, but it might be a model so we'll need to check if it's got an associated .gtmodel file.
                    if (GT::IsSupportedModelExtension(absolutePath))
                    {
                        char absolutePathWithExt[EASYVFS_MAX_PATH];
                        easypath_copy_and_append_extension(absolutePathWithExt, sizeof(absolutePathWithExt), absolutePath, "gtmodel");

                        if (!easyvfs_is_existing_file(g_EngineContext->GetVFS(), absolutePathWithExt))
                        {
                            GT::PostError("Editor: Can not open model file '%s'. Associated .gtmodel file does not exist.\n", path);
                            return nullptr;
                        }
                    }
                    else
                    {
                        GT::PostError("Editor: Can not open file '%s'. Does not exist.\n", path);
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
                        Log("Warning: Editor: An editor is not currently supported for the given asset. '%s'.", path);
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
                            script.Push("GUIServer");
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


    GTGUI::GUIElement* Editor::GetFileEditorElement(const char* path, const char* relativeTo)
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


    void Editor::OnFileInsert(const DataFilesWatcher::Item &item)
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
                    // GT.IO.FileInfo.New()
                    script.GetGlobal("GT");
                    assert(script.IsTable(-1));
                    {
                        script.Push("IO");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("FileInfo");
                            script.GetTableValue(-2);
                            assert(script.IsTable(-1));
                            {
                                script.Push("New");
                                script.GetTableValue(-2);
                                assert(script.IsFunction(-1));
                                {
                                    script.PushNewTable();
                                    script.SetTableValue(-1, "path",             item.info.absolutePath);
                                    script.SetTableValue(-1, "absolutePath",     item.info.absolutePath);
                                    script.SetTableValue(-1, "size",             (int)item.info.sizeInBytes);
                                    script.SetTableValue(-1, "lastModifiedTime", (int)item.info.lastModifiedTime);
                                    script.SetTableValue(-1, "isDirectory",      (item.info.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) != 0);
                                    script.Call(1, 1);

                                    script.InsertIntoStack(-4);
                                }
                            }
                            script.Pop(1);
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);

                    // The top item in the stack should be the return value from GTLib.IO.FileInfo.New().
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
                subEditor->OnFileInsert(item);
            }
        }
    }

    void Editor::OnFileRemove(const DataFilesWatcher::Item &item)
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
                    // GT.IO.FileInfo.New()
                    script.GetGlobal("GT");
                    assert(script.IsTable(-1));
                    {
                        script.Push("IO");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("FileInfo");
                            script.GetTableValue(-2);
                            assert(script.IsTable(-1));
                            {
                                script.Push("New");
                                script.GetTableValue(-2);
                                assert(script.IsFunction(-1));
                                {
                                    script.PushNewTable();
                                    script.SetTableValue(-1, "path",             item.info.absolutePath);
                                    script.SetTableValue(-1, "absolutePath",     item.info.absolutePath);
                                    script.SetTableValue(-1, "size",             (int)item.info.sizeInBytes);
                                    script.SetTableValue(-1, "lastModifiedTime", (int)item.info.lastModifiedTime);
                                    script.SetTableValue(-1, "isDirectory",      (item.info.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) != 0);
                                    script.Call(1, 1);

                                    script.InsertIntoStack(-4);
                                }
                            }
                            script.Pop(1);
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);

                    // The top item in the stack should be the return value from GTLib.IO.FileInfo.New().
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
                subEditor->OnFileRemove(item);
            }
        }
    }

    void Editor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
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
                    // GT.IO.FileInfo.New()
                    script.GetGlobal("GT");
                    assert(script.IsTable(-1));
                    {
                        script.Push("IO");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("FileInfo");
                            script.GetTableValue(-2);
                            assert(script.IsTable(-1));
                            {
                                script.Push("New");
                                script.GetTableValue(-2);
                                assert(script.IsFunction(-1));
                                {
                                    script.PushNewTable();
                                    script.SetTableValue(-1, "path",             item.info.absolutePath);
                                    script.SetTableValue(-1, "absolutePath",     item.info.absolutePath);
                                    script.SetTableValue(-1, "size",             (int)item.info.sizeInBytes);
                                    script.SetTableValue(-1, "lastModifiedTime", (int)item.info.lastModifiedTime);
                                    script.SetTableValue(-1, "isDirectory",      (item.info.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) != 0);
                                    script.Call(1, 1);

                                    script.InsertIntoStack(-4);
                                }
                            }
                            script.Pop(1);
                        }
                        script.Pop(1);
                    }
                    script.Pop(1);

                    // The top item in the stack should be the return value from GTLib.IO.FileInfo.New().
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
                subEditor->OnFileUpdate(item);
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


    void Editor::FFI::PushElement(GT::Script &script, GTGUI::GUIElement* element)
    {
        if (element != nullptr)
        {
            script.GetGlobal("GTGUI");
            assert(script.IsTable(-1));
            {
                script.Push("GUIServer");
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
