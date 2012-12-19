
#include <GTEngine/Editor.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/IO.hpp>
#include <GTGUI/Server.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/Keyboard.hpp>


#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    Editor::Editor(Game &game)
        : game(game), GUI(),
          modelEditor(*this), imageEditor(*this), textEditor(*this), sceneEditor(*this),
          isStarted(false), isOpen(false), disableFileWatchingAfterClose(true), disableKeyboardAutoRepeatAfterClose(true),
          dataFilesWatcherEventHandler(*this)
    {
    }

    Editor::~Editor()
    {
    }

    bool Editor::Startup(GTGUI::Server &guiServer)
    {
        if (!this->isStarted)
        {
            this->StartupScripting();

            auto &script = guiServer.GetScriptServer().GetScript();

            // We need to grab the main elements from the server.
            if (guiServer.LoadFromFile("engine/editor/main.xml"))
            {
                this->GUI.EditorMain   = guiServer.GetElementByID("EditorMain");
                this->GUI.Editor_Delta = guiServer.GetElementByID("Editor_Delta");
                this->GUI.Editor_FPS   = guiServer.GetElementByID("Editor_FPS");

                // We actually want the editor element to be the first child of the root. If we don't do this, sometimes a game can be in a state
                // where the editor will be placed underneath another GUI element, causing it to not look quite right.
                guiServer.GetRootElement()->PrependChild(*this->GUI.EditorMain);

                // Here we startup our sub-editors.
                this->modelEditor.Startup();
                this->imageEditor.Startup();
                //this->soundEditor.Startup();
                this->textEditor.Startup();
                this->sceneEditor.Startup();


                // Here we need to attach our files watcher event handler.
                this->game.GetDataFilesWatcher().AddEventHandler(this->dataFilesWatcherEventHandler);
                

                this->isStarted = true;
            }
            else
            {
                GTEngine::PostError("Error initializing editor: Failed to load editor/main.xml. Ensure the 'editor' directory exists. %s.", script.ToString(-1));
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

            
            // We want to keep track of whether or not auto-repeating should be disabled when the editor is closed.
            this->disableKeyboardAutoRepeatAfterClose = !GTCore::Keyboard::IsAutoRepeatEnabled();
            GTCore::Keyboard::EnableAutoRepeat();


            this->isOpen = true;
        }
    }

    void Editor::Close()
    {
        if (this->isOpen)
        {
            this->GUI.EditorMain->Hide();

            // We need to make sure everything is saved. We'll do this via the scripting environment.
            this->game.GetScript().Execute("Editor.SaveAllItems()");

            // We want to update the data files so we can see them in-game.
            this->game.GetDataFilesWatcher().CheckForChanges(false);
            this->game.GetDataFilesWatcher().DispatchEvents();

            // We may want to disable file watching.
            if (this->disableFileWatchingAfterClose)
            {
                this->game.DisableDataFilesWatching();
            }


            // We may want to disable auto repeat.
            if (this->disableKeyboardAutoRepeatAfterClose)
            {
                GTCore::Keyboard::DisableAutoRepeat();
            }


            this->isOpen = false;
        }
    }


    void Editor::Update(double deltaTimeInSeconds)
    {
        this->modelEditor.Update(deltaTimeInSeconds);
        this->sceneEditor.Update(deltaTimeInSeconds);


        // We need to update the profiling GUI.
        if (GTCore::Timing::GetTimeInSeconds() - this->lastProfilingUpdateTime > 1.0)
        {
            this->lastProfilingUpdateTime = GTCore::Timing::GetTimeInSeconds();


            double delta = this->game.GetProfiler().GetAverageFrameTime();
            double fps   = 0.0;

            if (delta > 0.0)
            {
                fps = 1.0 / delta;
            }


            char deltaStr[64];
            GTCore::IO::snprintf(deltaStr, 64, "%f", delta);

            char fpsStr[64];
            GTCore::IO::snprintf(fpsStr, 64, "%.1f", fps);
                    
            
            this->GUI.Editor_Delta->SetText(deltaStr);
            this->GUI.Editor_FPS->SetText(fpsStr);
        }
    }


    void Editor::OnModelActivated(const char* fileName)
    {
        this->modelEditor.LoadModel(fileName);
    }

    void Editor::OnImageActivated(const char* fileName)
    {
        this->imageEditor.LoadImage(fileName);
    }

    void Editor::OnSoundActivated(const char* fileName)
    {
        (void)fileName;
    }

    void Editor::OnTextFileActivated(const char* fileName)
    {
        this->textEditor.LoadFile(fileName);
    }

    void Editor::OnSceneActivated(const char* fileName)
    {
        this->sceneEditor.LoadScene(fileName);
    }



    void Editor::OnModelClosed(const char* fileName)
    {
        (void)fileName;
    }

    void Editor::OnImageClosed(const char* fileName)
    {
        (void)fileName;
    }

    void Editor::OnSoundClosed(const char* fileName)
    {
        (void)fileName;
    }

    void Editor::OnTextFileClosed(const char* fileName)
    {
        this->textEditor.CloseFile(fileName);
    }

    void Editor::OnSceneClosed(const char* fileName)
    {
        (void)fileName;
    }



    void Editor::OnFileActivated(const char* fileName)
    {
        if (IO::IsSupportedModelExtension(fileName))
        {
            this->OnModelActivated(fileName);
        }
        else if (IO::IsSupportedImageExtension(fileName))
        {
            this->OnImageActivated(fileName);
        }
        else if (IO::IsSupportedSoundExtension(fileName))
        {
            this->OnSoundActivated(fileName);
        }
        else if (IO::IsSupportedSceneExtension(fileName))
        {
            this->OnSceneActivated(fileName);
        }
        else        // Assume a text file if nothing else.
        {
            this->OnTextFileActivated(fileName);
        }
    }

    void Editor::OnFileClosed(const char* fileName)
    {
        if (IO::IsSupportedModelExtension(fileName))
        {
            this->OnModelClosed(fileName);
        }
        else if (IO::IsSupportedImageExtension(fileName))
        {
            this->OnImageClosed(fileName);
        }
        else if (IO::IsSupportedSoundExtension(fileName))
        {
            this->OnSoundClosed(fileName);
        }
        else if (IO::IsSupportedSceneExtension(fileName))
        {
            this->OnSceneClosed(fileName);
        }
        else        // Assume a text file if nothing else.
        {
            this->OnTextFileClosed(fileName);
        }
    }


    // FIXME: There's a bug here where if a file path includes double quotes, the script will fail to execute.
    void Editor::OnFileInsert(const DataFilesWatcher::Item &item)
    {
        GTCore::String script;
        script.AssignFormatted
        (
            "local info = GTCore.IO.FileInfo:New();"
            "info.path             = \"%s\";"
            "info.absolutePath     = \"%s\";"
            "info.size             = %d;"
            "info.lastModifiedTime = %d;"
            "info.isDirectory      = %s;"
            "Editor.DataFilesWatcher.OnInsert(info);",

            item.info.absolutePath.c_str(),
            item.info.absolutePath.c_str(),
            static_cast<int>(item.info.size),
            static_cast<int>(item.info.lastModifiedTime),
            item.info.isDirectory ? "true" : "false"
        );

        this->game.GetScript().Execute(script.c_str());
    }

    void Editor::OnFileRemove(const DataFilesWatcher::Item &item)
    {
        GTCore::String script;
        script.AssignFormatted
        (
            "local info = GTCore.IO.FileInfo:New();"
            "info.path             = \"%s\";"
            "info.absolutePath     = \"%s\";"
            "info.size             = %d;"
            "info.lastModifiedTime = %d;"
            "info.isDirectory      = %s;"
            "Editor.DataFilesWatcher.OnRemove(info);",

            item.info.absolutePath.c_str(),
            item.info.absolutePath.c_str(),
            static_cast<int>(item.info.size),
            static_cast<int>(item.info.lastModifiedTime),
            item.info.isDirectory ? "true" : "false"
        );

        this->game.GetScript().Execute(script.c_str());
    }

    void Editor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
        GTCore::String script;
        script.AssignFormatted
        (
            "local info = GTCore.IO.FileInfo:New();"
            "info.path             = \"%s\";"
            "info.absolutePath     = \"%s\";"
            "info.size             = %d;"
            "info.lastModifiedTime = %d;"
            "info.isDirectory      = %s;"
            "Editor.DataFilesWatcher.OnUpdate(info);",

            item.info.absolutePath.c_str(),
            item.info.absolutePath.c_str(),
            static_cast<int>(item.info.size),
            static_cast<int>(item.info.lastModifiedTime),
            item.info.isDirectory ? "true" : "false"
        );

        this->game.GetScript().Execute(script.c_str());
    }



    void Editor::StartupScripting()
    {
        auto &script = this->game.GetScript();

        script.Execute
        (
            "Editor.ModelEditor    = {};"
            "Editor.ImageEditor    = {};"
            "Editor.SoundEditor    = {};"
            "Editor.MaterialEditor = {};"
            "Editor.SceneEditor    = {};"
            "Editor.TextEditor     = {};"
        );

        script.GetGlobal("Editor");
        if (script.IsTable(-1))
        {
            script.SetTableFunction(-1, "Open",  FFI::Open);
            script.SetTableFunction(-1, "Close", FFI::Close);

            script.SetTableFunction(-1, "OnModelActivated",    FFI::OnModelActivated);
            script.SetTableFunction(-1, "OnImageActivated",    FFI::OnImageActivated);
            script.SetTableFunction(-1, "OnSoundActivated",    FFI::OnSoundActivated);
            script.SetTableFunction(-1, "OnTextFileActivated", FFI::OnTextFileActivated);
            script.SetTableFunction(-1, "OnSceneActivated",    FFI::OnSceneActivated);

            script.SetTableFunction(-1, "OnFileActivated", FFI::OnFileActivated);
            script.SetTableFunction(-1, "OnFileClosed",    FFI::OnFileClosed);


            script.Push("ModelEditor");
            script.GetTableValue(-2);
            if (script.IsTable(-1))
            {
                script.SetTableFunction(-1, "SaveFile", FFI::ModelEditorFFI::SaveFile);

                script.SetTableFunction(-1, "SetMaterial", FFI::ModelEditorFFI::SetMaterial);

                script.SetTableFunction(-1, "ShowConvexDecomposition",  FFI::ModelEditorFFI::ShowConvexDecomposition);
                script.SetTableFunction(-1, "HideConvexDecomposition",  FFI::ModelEditorFFI::HideConvexDecomposition);
                script.SetTableFunction(-1, "BuildConvexDecomposition", FFI::ModelEditorFFI::BuildConvexDecomposition);

                script.SetTableFunction(-1, "PlayAnimation", FFI::ModelEditorFFI::PlayAnimation);
                script.SetTableFunction(-1, "StopAnimation", FFI::ModelEditorFFI::StopAnimation);
            }
            script.Pop(1);



            script.Push("TextEditor");
            script.GetTableValue(-2);
            if (script.IsTable(-1))
            {
                script.SetTableFunction(-1, "SaveFile", FFI::TextEditorFFI::SaveFile);
            }
            script.Pop(1);
        }
        script.Pop(1);
    }



    /////////////////////////////////////////////
    // FFI.
    Editor & Editor::FFI::GetEditor(GTCore::Script &script)
    {
        return GameScript::FFI::GetGameObject(script).GetEditor();
    }


    int Editor::FFI::Open(GTCore::Script &script)
    {
        GameScript::FFI::GetGameObject(script).OpenEditor();
        return 0;
    }

    int Editor::FFI::Close(GTCore::Script &script)
    {
        GameScript::FFI::GetGameObject(script).CloseEditor();
        return 0;
    }


    int Editor::FFI::OnModelActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnModelActivated(script.ToString(1));
        return 0;
    }

    int Editor::FFI::OnImageActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnImageActivated(script.ToString(1));
        return 0;
    }

    int Editor::FFI::OnSoundActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnSoundActivated(script.ToString(1));
        return 0;
    }

    int Editor::FFI::OnTextFileActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnTextFileActivated(script.ToString(1));
        return 0;
    }

    int Editor::FFI::OnSceneActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnSceneActivated(script.ToString(1));
        return 0;
    }



    int Editor::FFI::OnFileActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnFileActivated(script.ToString(1));
        return 0;
    }

    int Editor::FFI::OnFileClosed(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnFileClosed(script.ToString(1));
        return 0;
    }



    // ModelEditor
    int Editor::FFI::ModelEditorFFI::SaveFile(GTCore::Script &script)
    {
        script.Push(FFI::GetEditor(script).GetModelEditor().SaveModel(script.ToString(1)));
        return 1;
    }

    int Editor::FFI::ModelEditorFFI::SetMaterial(GTCore::Script &script)
    {
        script.Push(FFI::GetEditor(script).GetModelEditor().SetMaterial(script.ToInteger(1) - 1, script.ToString(2)));  // -1 because we will have passed a 1-based index from Lua.
        return 1;
    }

    int Editor::FFI::ModelEditorFFI::ShowConvexDecomposition(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().ShowConvexDecomposition();
        return 0;
    }

    int Editor::FFI::ModelEditorFFI::HideConvexDecomposition(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().HideConvexDecomposition();
        return 0;
    }

    int Editor::FFI::ModelEditorFFI::BuildConvexDecomposition(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        ConvexHullBuildSettings settings;
        settings.compacityWeight               = script.ToFloat(1);
        settings.volumeWeight                  = script.ToFloat(2);
        settings.minClusters                   = static_cast<unsigned int>(script.ToInteger(3));
        settings.verticesPerCH                 = static_cast<unsigned int>(script.ToInteger(4));
        settings.concavity                     = script.ToFloat(5);
        settings.smallClusterThreshold         = script.ToFloat(6);
        settings.connectedComponentsDist       = script.ToFloat(7);
        settings.simplifiedTriangleCountTarget = static_cast<unsigned int>(script.ToInteger(8));
        settings.addExtraDistPoints            = script.ToBoolean(9);
        settings.addFacesPoints                = script.ToBoolean(10);

        game.GetEditor().GetModelEditor().BuildConvexDecomposition(settings);
        return 0;
    }

    int Editor::FFI::ModelEditorFFI::PlayAnimation(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().PlayAnimation();
        return 0;
    }

    int Editor::FFI::ModelEditorFFI::StopAnimation(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetEditor().GetModelEditor().StopAnimation();
        return 0;
    }



    ////////////////////////////////////////////////////
    // TextEditorFFI

    int Editor::FFI::TextEditorFFI::SaveFile(GTCore::Script &script)
    {
        script.Push(FFI::GetEditor(script).GetTextEditor().SaveFile(script.ToString(1)));
        return 1;
    }


    ////////////////////////////////////////////////////
    // SceneEditorFFI

    int Editor::FFI::SceneEditorFFI::SaveFile(GTCore::Script &script)
    {
        script.Push(FFI::GetEditor(script).GetSceneEditor().SaveScene(script.ToString(1)));
        return 1;
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
