// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/ApplicationConfig.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

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
        if (Scripting::LoadGTEngineScriptLibrary(*this) && Scripting::LoadGameLibrary(*this, this->game))
        {
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


            return true;
        }
        
        return false;
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


#if 0

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
}

#endif


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
