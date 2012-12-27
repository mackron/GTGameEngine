
#include <GTEngine/Scripting.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/ApplicationConfig.hpp>

namespace GTEngine
{
    GameScript::GameScript(Game &game)
        : game(game), lastError(), loadedFiles(), errorHandler(*this)
    {
        this->AttachErrorHandler(errorHandler);
    }

    GameScript::~GameScript()
    {
    }

    bool GameScript::Startup()
    {
        // First we load the GTEngine scripting stuff.
        bool success = Scripting::LoadGTEngineScriptLibrary(*this);

        // The first thing we want to do is load some defaults.
        success = success && this->Execute
        (
            "Game = {};"
            
        );

        // Now for some callbacks.
        success = success && this->Execute
        (
            "Game.Callbacks = GTCore.CallbackManager:Create();"

            "function Game.ClearCallbacks(name)"
            "    Game.Callbacks:Clear(name);"
            "end;"

            "function Game.ClearAllCallbacks()"
            "    Game.Callbacks:ClearAll();"
            "end;"


            // Events
            "function Game.OnStartup(arg)"
            "    Game.Callbacks:BindOrCall('OnStartup', arg);"
            "end;"

            "function Game.OnShutdown(arg)"
            "    Game.Callbacks:BindOrCall('OnShutdown', arg);"
            "end;"

            "function Game.OnUpdate(arg)"
            "    Game.Callbacks:BindOrCall('OnUpdate', arg);"
            "end;"

            "function Game.OnDraw(arg)"
            "    Game.Callbacks:BindOrCall('OnDraw', arg);"
            "end;"

            "function Game.OnPostDraw(arg)"
            "    Game.Callbacks:BindOrCall('OnPostDraw', arg);"
            "end;"

            "function Game.OnSize(arg)"
            "    Game.Callbacks:BindOrCall('OnSize', arg);"
            "end;"

            "function Game.OnMouseMove(arg)"
            "    Game.Callbacks:BindOrCall('OnMouseMove', arg);"
            "end;"

            "function Game.OnMouseWheel(arg)"
            "    Game.Callbacks:BindOrCall('OnMouseWheel', arg);"
            "end;"

            "function Game.OnMouseButtonDown(arg)"
            "    Game.Callbacks:BindOrCall('OnMouseButtonDown', arg);"
            "end;"

            "function Game.OnMouseButtonUp(arg)"
            "    Game.Callbacks:BindOrCall('OnMouseButtonUp', arg);"
            "end;"

            "function Game.OnMouseButtonDoubleClick(arg)"
            "    Game.Callbacks:BindOrCall('OnMouseButtonDoubleClick', arg);"
            "end;"

            "function Game.OnKeyPressed(arg)"
            "    Game.Callbacks:BindOrCall('OnKeyPressed', arg);"
            "end;"

            "function Game.OnKeyReleased(arg)"
            "    Game.Callbacks:BindOrCall('OnKeyReleased', arg);"
            "end;"

            "function Game.OnKeyDown(arg)"
            "    Game.Callbacks:BindOrCall('OnKeyDown', arg);"
            "end;"

            "function Game.OnKeyUp(arg)"
            "    Game.Callbacks:BindOrCall('OnKeyUp', arg);"
            "end;"

            "function Game.OnReceiveFocus(arg)"
            "    Game.Callbacks:BindOrCall('OnReceiveFocus', arg);"
            "end;"

            "function Game.OnLoseFocus(arg)"
            "    Game.Callbacks:BindOrCall('OnLoseFocus', arg);"
            "end;"
        );


        // Directory management.
        success = success && this->Execute
        (
            "Directories ="
            "{"
            "    Data = {};"
            "}"
        );

        // TODO: Consider moving these display properties into a namespace like Game or whatnot.
        // Display stuff.
        success = success && this->Execute
        (
            "Display ="
            "{"
            "    Width      = 1280;"
            "    Height     = 720;"
            "    FOV        = 90;"
            "    Fullscreen = false;"
            ""
            "    Textures ="
            "    {"
            "        Anisotropy = 16;"
            "    };"
            "};"
        );

        // We need to initialise some other non-game stuff here so scripts can get early access to some objects; the editor in particular.
        success = success && this->Execute
        (
            "Editor = {};"
        );

        // Here we load the data directories from the application config. We need to do this so that the editor has access to them. Might also come
        // in handy for game code, too. Who knows.
        this->GetGlobal("Directories");
        if (this->IsTable(-1))
        {
            this->Push("Data");
            this->GetTableValue(-2);
            if (this->IsTable(-1))
            {
                auto &dataDirectories = ApplicationConfig::GetDataDirectories();
                for (size_t iDirectory = 0; iDirectory < dataDirectories.count; ++iDirectory)
                {
                    this->SetTableValue(-1, iDirectory + 1, dataDirectories[iDirectory].c_str());
                }
            }
            this->Pop(1);
        }
        this->Pop(1);

        // Here is where we register the foreign function interface.
        success = success && this->RegisterFFI();

        return success;
    }


    bool GameScript::LoadFile(const char* fileName)
    {
        if (!this->HasFileBeenLoaded(fileName))
        {
            GTCore::String absolutePath;
            if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
            {
                this->loadedFiles.PushBack(absolutePath);
            }
        }

        return GTCore::Script::LoadFile(fileName);
    }



    bool GameScript::HasFileBeenLoaded(const char* fileName) const
    {
        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            return this->loadedFiles.Exists(absolutePath);
        }

        return false;
    }
}

///////////////////////////////////////////////////////////
// FFI

namespace GTEngine
{
    Game & GameScript::FFI::GetGameObject(GTCore::Script &script)
    {
        script.GetGlobal("__GamePtr");
        auto game = static_cast<Game*>(script.ToPointer(-1));
        script.Pop(1);

        assert(game != nullptr);

        return *game;
    }


    ////////////////////////////////////////////////////////////////
    // Game FFI

    int FFI_Game_Close(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.Close();
        return 0;
    }

    int FFI_Game_Pause(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.Pause();
        return 0;
    }

    int FFI_Game_Resume(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.Resume();
        return 0;
    }

    int FFI_Game_EnableFullscreen(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.EnableFullscreen();
        return 0;
    }

    int FFI_Game_DisableFullscreen(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.DisableFullscreen();
        return 0;
    }


    int FFI_Game_ExecuteFile(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        script.Push(game.GetScript().ExecuteFile(script.ToString(1)));
        return 1;
    }

    int FFI_Game_ExecuteScript(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        script.Push(game.ExecuteScript(script.ToString(1)));
        return 1;
    }

    int FFI_Game_GetLastScriptError(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        script.Push(game.GetScript().GetLastError());
        return 1;
    }


    int FFI_Game_ShowDebug(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.ShowDebugging();
        return 0;
    }

    int FFI_Game_HideDebug(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.HideDebugging();
        return 0;
    }


    int FFI_Game_IsKeyDown(GTCore::Script &script)
    {
        script.Push(GameScript::FFI::GetGameObject(script).IsKeyDown(static_cast<GTCore::Key>(script.ToInteger(1))));
        return 1;
    }
    
    int FFI_Game_IsMouseButtonDown(GTCore::Script &script)
    {
        script.Push(GameScript::FFI::GetGameObject(script).IsMouseButtonDown(static_cast<GTCore::MouseButton>(script.ToInteger(1))));
        return 1;
    }


    int FFI_Game_ScanDataFilesForChanges(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.GetDataFilesWatcher().CheckForChanges(false);
        game.GetDataFilesWatcher().DispatchEvents();

        return 0;
    }


    int FFI_Game_CaptureMouse(GTCore::Script &script)
    {
        GameScript::FFI::GetGameObject(script).CaptureMouse();
        return 0;
    }

    int FFI_Game_ReleaseMouse(GTCore::Script &script)
    {
        GameScript::FFI::GetGameObject(script).ReleaseMouse();
        return 0;
    } 



    //////////////////////////////////////////////////////
    // RegisterFFI()

    bool GameScript::RegisterFFI()
    {
        // We assert that the tables have already been created.

        this->Push(&this->game);
        this->SetGlobal("__GamePtr");

        this->GetGlobal("Game");
        if (this->IsTable(-1))
        {
            this->SetTableFunction(-1, "Close",                   FFI_Game_Close);

            this->SetTableFunction(-1, "Pause",                   FFI_Game_Pause);
            this->SetTableFunction(-1, "Resume",                  FFI_Game_Resume);

            this->SetTableFunction(-1, "EnableFullscreen",        FFI_Game_EnableFullscreen);
            this->SetTableFunction(-1, "DisableFullscreen",       FFI_Game_DisableFullscreen);

            this->SetTableFunction(-1, "ExecuteFile",             FFI_Game_ExecuteFile);
            this->SetTableFunction(-1, "ExecuteScript",           FFI_Game_ExecuteScript);
            this->SetTableFunction(-1, "GetLastScriptError",      FFI_Game_GetLastScriptError);

            this->SetTableFunction(-1, "ShowDebug",               FFI_Game_ShowDebug);
            this->SetTableFunction(-1, "HideDebug",               FFI_Game_HideDebug);

            this->SetTableFunction(-1, "IsKeyDown",               FFI_Game_IsKeyDown);
            this->SetTableFunction(-1, "IsMouseButtonDown",       FFI_Game_IsMouseButtonDown);

            this->SetTableFunction(-1, "ScanDataFilesForChanges", FFI_Game_ScanDataFilesForChanges);      

            this->SetTableFunction(-1, "CaptureMouse",            FFI_Game_CaptureMouse);
            this->SetTableFunction(-1, "ReleaseMouse",            FFI_Game_ReleaseMouse);
        }
        this->Pop(1);    // Game


        return true;
    }





    //////////////////////////////////////////////////////
    // ErrorHandler

    GameScript::ErrorHandler::ErrorHandler(GameScript &scriptIn)
        : script(scriptIn)
    {
    }

    void GameScript::ErrorHandler::OnError(GTCore::Script &, const char* message)
    {
        this->script.SetLastError(message);
        PostError("Script Error: %s", message);
    }
}
