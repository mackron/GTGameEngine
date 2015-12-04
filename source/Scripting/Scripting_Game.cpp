// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Scripting/Scripting_Game.hpp>
#include <GTGE/GTEngine.hpp>    // For g_Context

namespace GT
{
    bool LoadGameLibrary(GT::Script &script, Context &game)
    {
        bool successful = true;


        // Game = {}
        script.PushNewTable();
        script.SetGlobal("Game");

        // Editor = {}
        //
        // We need to create this table here because the editor will try to access it.
        script.PushNewTable();
        script.SetGlobal("Editor");


        // Callbacks.
        successful = successful && script.Execute
        (
            "Game.Callbacks = GT.CallbackManager:Create();"

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

            "function Game.OnPause(arg)"
            "    Game.Callbacks:BindOrCall('OnPause', arg);"
            "end;"

            "function Game.OnResume(arg)"
            "    Game.Callbacks:BindOrCall('OnResume', arg);"
            "end;"
        );


        // Directories =
        // {
        //     Data = {}
        // }
        script.PushNewTable();
        {
            script.Push("Data");
            script.PushNewTable();
            script.SetTableValue(-3);
        }
        script.SetGlobal("Directories");


        ////////////////////////////////////////////////////////////
        // FFI

        script.Push(static_cast<void*>(&game));
        script.SetGlobal("__GamePtr");

        script.GetGlobal("Game");
        assert(script.IsTable(-1));
        {
            script.SetTableFunction(-1, "GetExecutableDirectoryAbsolutePath", GameFFI::GetExecutableDirectoryAbsolutePath);
            script.SetTableFunction(-1, "GetExecutableAbsolutePath",          GameFFI::GetExecutableAbsolutePath);
            script.SetTableFunction(-1, "Close",                              GameFFI::Close);
            script.SetTableFunction(-1, "Pause",                              GameFFI::Pause);
            script.SetTableFunction(-1, "Resume",                             GameFFI::Resume);
            script.SetTableFunction(-1, "IsPaused",                           GameFFI::IsPaused);
            script.SetTableFunction(-1, "EnableFullscreen",                   GameFFI::EnableFullscreen);
            script.SetTableFunction(-1, "DisableFullscreen",                  GameFFI::DisableFullscreen);
            script.SetTableFunction(-1, "OpenEditor",                         GameFFI::OpenEditor);
            script.SetTableFunction(-1, "CloseEditor",                        GameFFI::CloseEditor);
            script.SetTableFunction(-1, "IsKeyDown",                          GameFFI::IsKeyDown);
            script.SetTableFunction(-1, "IsMouseButtonDown",                  GameFFI::IsMouseButtonDown);
            script.SetTableFunction(-1, "CaptureMouse",                       GameFFI::CaptureMouse);
            script.SetTableFunction(-1, "ReleaseMouse",                       GameFFI::ReleaseMouse);
            script.SetTableFunction(-1, "IsMouseCaptured",                    GameFFI::IsMouseCaptured);
            script.SetTableFunction(-1, "GetMouseOffset",                     GameFFI::GetMouseOffset);
            script.SetTableFunction(-1, "EnableMouseSmoothing",               GameFFI::EnableMouseSmoothing);
            script.SetTableFunction(-1, "DisableMouseSmoothing",              GameFFI::DisableMouseSmoothing);
            script.SetTableFunction(-1, "IsMouseSmoothingEnabled",            GameFFI::IsMouseSmoothingEnabled);
            script.SetTableFunction(-1, "GetGameWindowGUIElement",            GameFFI::GetGameWindowGUIElement);
            script.SetTableFunction(-1, "SaveGameState",                      GameFFI::SaveGameState);
            script.SetTableFunction(-1, "LoadGameState",                      GameFFI::LoadGameState);
            script.SetTableFunction(-1, "LoadScene",                          GameFFI::LoadScene);
            script.SetTableFunction(-1, "PackageForDistribution",             GameFFI::PackageForDistribution);


            script.Push("CollisionGroups");
            script.PushNewTable();
            {
                script.SetTableValue(-1, "All",  static_cast<int>(-1));
                script.SetTableValue(-1, "None", static_cast<int>( 0));
            }
            script.SetTableValue(-3);
        }
        script.Pop(1);


        return true;
    }




    ///////////////////////////////////////////////////////////
    // FFI

    namespace GameFFI
    {
        Context & GetGame(GT::Script &script)
        {
            script.GetGlobal("__GamePtr");
            auto game = static_cast<Context*>(script.ToPointer(-1));
            script.Pop(1);

            assert(game != nullptr);
            return *game;
        }


        int GetExecutableDirectoryAbsolutePath(GT::Script &script)
        {
            script.Push(g_Context->GetExecutableDirectoryAbsolutePath());
            return 1;
        }

        int GetExecutableAbsolutePath(GT::Script &script)
        {
            script.Push(g_Context->GetExecutableAbsolutePath());
            return 1;
        }


        int Close(GT::Script &script)
        {
            GetGame(script).Close();
            return 0;
        }

        int Pause(GT::Script &script)
        {
            GetGame(script).Pause();
            return 0;
        }

        int Resume(GT::Script &script)
        {
            GetGame(script).Resume();
            return 0;
        }

        int IsPaused(GT::Script &script)
        {
            script.Push(GetGame(script).IsPaused());
            return 1;
        }

        int EnableFullscreen(GT::Script &script)
        {
            GetGame(script).EnableFullscreen();
            return 0;
        }

        int DisableFullscreen(GT::Script &script)
        {
            GetGame(script).DisableFullscreen();
            return 0;
        }


        int OpenEditor(GT::Script &script)
        {
            GetGame(script).OpenEditor();
            return 0;
        }

        int CloseEditor(GT::Script &script)
        {
            GetGame(script).CloseEditor();
            return 0;
        }


        int IsKeyDown(GT::Script &script)
        {
            script.Push(GetGame(script).IsKeyDown(static_cast<Key>(script.ToInteger(1))));
            return 1;
        }

        int IsMouseButtonDown(GT::Script &script)
        {
            script.Push(GetGame(script).IsMouseButtonDown(static_cast<MouseButton>(script.ToInteger(1))));
            return 1;
        }

        int CaptureMouse(GT::Script &script)
        {
            GetGame(script).CaptureMouse();
            return 0;
        }

        int ReleaseMouse(GT::Script &script)
        {
            GetGame(script).ReleaseMouse();
            return 0;
        }

        int IsMouseCaptured(GT::Script &script)
        {
            script.Push(GetGame(script).IsMouseCaptured());
            return 1;
        }

        int GetMouseOffset(GT::Script &script)
        {
            auto &game = GetGame(script);

            if (game.IsMouseCaptured())
            {
                float xOffset;
                float yOffset;
                game.GetMouseOffset(xOffset, yOffset);

                script.Push(xOffset);
                script.Push(yOffset);

                return 2;
            }
            else
            {
                return 0;
            }
        }

        int EnableMouseSmoothing(GT::Script &script)
        {
            GetGame(script).EnableMouseSmoothing();
            return 0;
        }

        int DisableMouseSmoothing(GT::Script &script)
        {
            GetGame(script).DisableMouseSmoothing();
            return 0;
        }

        int IsMouseSmoothingEnabled(GT::Script &script)
        {
            script.Push(GetGame(script).IsMouseSmoothingEnabled());
            return 1;
        }


        int GetGameWindowGUIElement(GT::Script &script)
        {
            auto element = GetGame(script).GetGameWindowGUIElement();
            if (element != nullptr)
            {
                script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", element->id).c_str());
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int SaveGameState(GT::Script &script)
        {
            script.Push(GetGame(script).SaveGameState(script.ToString(1)));
            return 1;
        }

        int LoadGameState(GT::Script &script)
        {
            script.Push(GetGame(script).LoadGameState(script.ToString(1)));
            return 1;
        }

        int LoadScene(GT::Script &script)
        {
            script.Push(GetGame(script).LoadScene(script.ToString(1)));
            return 1;
        }


        int PackageForDistribution(GT::Script &script)
        {
            script.Push(GetGame(script).PackageForDistribution(script.ToString(1), script.ToString(2)));
            return 1;
        }
    }
}