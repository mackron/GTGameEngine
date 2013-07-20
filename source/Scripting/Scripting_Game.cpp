// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Game.hpp>
#include <GTEngine/Game.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadGameLibrary(GTCore::Script &script, Game &game)
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
                script.SetTableFunction(-1, "EnableFullscreen",                   GameFFI::EnableFullscreen);
                script.SetTableFunction(-1, "DisableFullscreen",                  GameFFI::DisableFullscreen);
                script.SetTableFunction(-1, "OpenEditor",                         GameFFI::OpenEditor);
                script.SetTableFunction(-1, "CloseEditor",                        GameFFI::CloseEditor);
                script.SetTableFunction(-1, "ShowDebug",                          GameFFI::ShowDebug);
                script.SetTableFunction(-1, "HideDebug",                          GameFFI::HideDebug);
                script.SetTableFunction(-1, "IsKeyDown",                          GameFFI::IsKeyDown);
                script.SetTableFunction(-1, "IsMouseButtonDown",                  GameFFI::IsMouseButtonDown);
                script.SetTableFunction(-1, "CaptureMouse",                       GameFFI::CaptureMouse);
                script.SetTableFunction(-1, "ReleaseMouse",                       GameFFI::ReleaseMouse);
                script.SetTableFunction(-1, "IsMouseCaptured",                    GameFFI::IsMouseCaptured);
                script.SetTableFunction(-1, "GetMouseOffset",                     GameFFI::GetMouseOffset);
                script.SetTableFunction(-1, "EnableMouseSmoothing",               GameFFI::EnableMouseSmoothing);
                script.SetTableFunction(-1, "DisableMouseSmoothing",              GameFFI::DisableMouseSmoothing);
                script.SetTableFunction(-1, "IsMouseSmoothingEnabled",            GameFFI::IsMouseSmoothingEnabled);
                script.SetTableFunction(-1, "ScanDataFilesForChanges",            GameFFI::ScanDataFilesForChanges);
                script.SetTableFunction(-1, "GetGameWindowGUIElement",            GameFFI::GetGameWindowGUIElement);
                script.SetTableFunction(-1, "LoadScene",                          GameFFI::LoadScene);
                script.SetTableFunction(-1, "PackageForDistribution",             GameFFI::PackageForDistribution);

                // TODO: Move these to GTEngine.
                script.SetTableFunction(-1, "ExecuteFile",        GameFFI::ExecuteFile);
                script.SetTableFunction(-1, "ExecuteScript",      GameFFI::ExecuteScript);
                script.SetTableFunction(-1, "GetLastScriptError", GameFFI::GetLastScriptError);
            }
            script.Pop(1);


            return true;
        }




        ///////////////////////////////////////////////////////////
        // FFI

        namespace GameFFI
        {
            Game & GetGame(GTCore::Script &script)
            {
                script.GetGlobal("__GamePtr");
                auto game = static_cast<Game*>(script.ToPointer(-1));
                script.Pop(1);

                assert(game != nullptr);
                return *game;
            }


            int GetExecutableDirectoryAbsolutePath(GTCore::Script &script)
            {
                script.Push(GetGame(script).GetExecutableDirectoryAbsolutePath());
                return 1;
            }

            int GetExecutableAbsolutePath(GTCore::Script &script)
            {
                script.Push(GetGame(script).GetExecutableAbsolutePath());
                return 1;
            }


            int Close(GTCore::Script &script)
            {
                GetGame(script).Close();
                return 0;
            }

            int Pause(GTCore::Script &script)
            {
                GetGame(script).Pause();
                return 0;
            }

            int Resume(GTCore::Script &script)
            {
                GetGame(script).Resume();
                return 0;
            }

            int EnableFullscreen(GTCore::Script &script)
            {
                GetGame(script).EnableFullscreen();
                return 0;
            }

            int DisableFullscreen(GTCore::Script &script)
            {
                GetGame(script).DisableFullscreen();
                return 0;
            }


            int OpenEditor(GTCore::Script &script)
            {
                GetGame(script).OpenEditor();
                return 0;
            }

            int CloseEditor(GTCore::Script &script)
            {
                GetGame(script).CloseEditor();
                return 0;
            }


            int ShowDebug(GTCore::Script &script)
            {
                GetGame(script).ShowDebugging();
                return 0;
            }

            int HideDebug(GTCore::Script &script)
            {
                GetGame(script).HideDebugging();
                return 0;
            }

            int IsKeyDown(GTCore::Script &script)
            {
                script.Push(GetGame(script).IsKeyDown(static_cast<GTCore::Key>(script.ToInteger(1))));
                return 1;
            }

            int IsMouseButtonDown(GTCore::Script &script)
            {
                script.Push(GetGame(script).IsMouseButtonDown(static_cast<GTCore::MouseButton>(script.ToInteger(1))));
                return 1;
            }

            int CaptureMouse(GTCore::Script &script)
            {
                GetGame(script).CaptureMouse();
                return 0;
            }

            int ReleaseMouse(GTCore::Script &script)
            {
                GetGame(script).ReleaseMouse();
                return 0;
            }

            int IsMouseCaptured(GTCore::Script &script)
            {
                script.Push(GetGame(script).IsMouseCaptured());
                return 1;
            }

            int GetMouseOffset(GTCore::Script &script)
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

            int EnableMouseSmoothing(GTCore::Script &script)
            {
                GetGame(script).EnableMouseSmoothing();
                return 0;
            }

            int DisableMouseSmoothing(GTCore::Script &script)
            {
                GetGame(script).DisableMouseSmoothing();
                return 0;
            }

            int IsMouseSmoothingEnabled(GTCore::Script &script)
            {
                script.Push(GetGame(script).IsMouseSmoothingEnabled());
                return 1;
            }


            int ScanDataFilesForChanges(GTCore::Script &script)
            {
                GetGame(script).GetDataFilesWatcher().CheckForChangesAndDispatchEvents();
                return 0;
            }


            int GetGameWindowGUIElement(GTCore::Script &script)
            {
                auto element = GetGame(script).GetGameWindowGUIElement();
                if (element != nullptr)
                {
                    script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", element->id).c_str());
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }


            int LoadScene(GTCore::Script &script)
            {
                script.Push(GetGame(script).OnLoadScene(script.ToString(1)));
                return 1;
            }


            int PackageForDistribution(GTCore::Script &script)
            {
                script.Push(GetGame(script).PackageForDistribution(script.ToString(1), script.ToString(2)));
                return 1;
            }



            int ExecuteFile(GTCore::Script &script)
            {
                script.Push(GetGame(script).GetScript().ExecuteFile(script.ToString(1)));
                return 1;
            }

            int ExecuteScript(GTCore::Script &script)
            {
                script.Push(GetGame(script).ExecuteScript(script.ToString(1)));
                return 1;
            }

            int GetLastScriptError(GTCore::Script &script)
            {
                script.Push(GetGame(script).GetScript().GetLastError());
                return 1;
            }
        }
    }
}