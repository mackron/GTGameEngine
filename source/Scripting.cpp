// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Scripting.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/PrefabLibrary.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Physics/CollisionShapeTypes.hpp>
#include <easy_path/easy_path.h>

namespace GTEngine
{
    namespace Scripting
    {
        static GTLib::RandomLCG g_Random;

        bool LoadGTEngineScriptLibrary(GTLib::Script &script)
        {
            bool successful = LoadExtendedMathLibrary(script);


            // GTEngine table.
            script.PushNewTable();
            {
                ///////////////////////////////////////////////////
                // GTEngine.Editor

                script.Push("Editor");
                script.PushNewTable();
                {
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.System

                script.Push("System");
                script.PushNewTable();
                {
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.Display

                script.Push("Display");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, "Width",      1280);
                    script.SetTableValue(-1, "Height",     720);
                    script.SetTableValue(-1, "Fullscreen", false);


                    script.Push("Textures");
                    script.PushNewTable();
                    {
                        script.SetTableValue(-1, "Anisotropy", 16);
                    }
                    script.SetTableValue(-3);
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.CollisionShapeTypes

                script.Push("CollisionShapeTypes");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, "None",             CollisionShapeType_None);
                    script.SetTableValue(-1, "Box",              CollisionShapeType_Box);
                    script.SetTableValue(-1, "Sphere",           CollisionShapeType_Sphere);
                    script.SetTableValue(-1, "Ellipsoid",        CollisionShapeType_Ellipsoid);
                    script.SetTableValue(-1, "CylinderX",        CollisionShapeType_CylinderX);
                    script.SetTableValue(-1, "CylinderY",        CollisionShapeType_CylinderY);
                    script.SetTableValue(-1, "CylinderZ",        CollisionShapeType_CylinderZ);
                    script.SetTableValue(-1, "CapsuleX",         CollisionShapeType_CapsuleX);
                    script.SetTableValue(-1, "CapsuleY",         CollisionShapeType_CapsuleY);
                    script.SetTableValue(-1, "CapsuleZ",         CollisionShapeType_CapsuleZ);
                    script.SetTableValue(-1, "ConvexHull",       CollisionShapeType_ConvexHull);
                    script.SetTableValue(-1, "ModelConvexHulls", CollisionShapeType_ModelConvexHulls);       // A special type representing the case when the convex hulls from the model is being used.
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.ScriptVariableTypes

                script.Push("ScriptVariableTypes");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, "Unknown", ScriptVariableType_Unknown);
                    script.SetTableValue(-1, "None",    ScriptVariableType_None);
                    script.SetTableValue(-1, "Number",  ScriptVariableType_Number);
                    script.SetTableValue(-1, "Vec2",    ScriptVariableType_Vec2);
                    script.SetTableValue(-1, "Vec3",    ScriptVariableType_Vec3);
                    script.SetTableValue(-1, "Vec4",    ScriptVariableType_Vec4);
                    script.SetTableValue(-1, "Boolean", ScriptVariableType_Boolean);
                    script.SetTableValue(-1, "String",  ScriptVariableType_String);
                    script.SetTableValue(-1, "Prefab",  ScriptVariableType_Prefab);
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.ScriptVariableTypes

                script.Push("ShaderParameterTypes");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, "Float",       ShaderParameterType_Float);
                    script.SetTableValue(-1, "Float2",      ShaderParameterType_Float2);
                    script.SetTableValue(-1, "Float3",      ShaderParameterType_Float3);
                    script.SetTableValue(-1, "Float4",      ShaderParameterType_Float4);
                    script.SetTableValue(-1, "Integer",     ShaderParameterType_Integer);
                    script.SetTableValue(-1, "Integer2",    ShaderParameterType_Integer2);
                    script.SetTableValue(-1, "Integer3",    ShaderParameterType_Integer3);
                    script.SetTableValue(-1, "Integer4",    ShaderParameterType_Integer4);
                    script.SetTableValue(-1, "Float2x2",    ShaderParameterType_Float2x2);
                    script.SetTableValue(-1, "Float3x3",    ShaderParameterType_Float3x3);
                    script.SetTableValue(-1, "Float4x4",    ShaderParameterType_Float4x4);
                    script.SetTableValue(-1, "Texture1D",   ShaderParameterType_Texture1D);
                    script.SetTableValue(-1, "Texture2D",   ShaderParameterType_Texture2D);
                    script.SetTableValue(-1, "Texture3D",   ShaderParameterType_Texture3D);
                    script.SetTableValue(-1, "TextureCube", ShaderParameterType_TextureCube);
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.<Misc>

                script.Push("ScriptDefinitions");
                script.PushNewTable();
                script.SetTableValue(-3);
            }
            script.SetGlobal("GTEngine");


            ///////////////////////////////////////////
            // GUI Helpers.

            successful = successful && script.Execute
            (
                "GTEngine.CreateGUIElement                        = GTGUI.Server.CreateElement;"
                "GTEngine.DeleteGUIElement                        = GTGUI.Server.DeleteElement;"
                "GTEngine.GetGUIElementByID                       = GTGUI.Server.GetElementByID;"
                "GTEngine.GetRootGUIElement                       = GTGUI.Server.GetRootElement;"
                "GTEngine.LoadGUIStyleScriptFromFile              = GTGUI.Server.LoadStyleScriptFromFile;"
                "GTEngine.SetGUIDragAndDropProxyElement           = GTGUI.Server.SetDragAndDropProxyElement;"
                "GTEngine.GetGUIDragAndDropProxyElement           = GTGUI.Server.GetDragAndDropProxyElement;"
                "GTEngine.RemoveCurrentGUIDragAndDropProxyElement = GTGUI.Server.RemoveCurrentDragAndDropProxyElement;"
                "GTEngine.SetGUIDragAndDropProxyElementOffset     = GTGUI.Server.SetDragAndDropProxyElementOffset;"
                "GTEngine.GetGUIViewportWidth                     = GTGUI.Server.GetViewportWidth;"
                "GTEngine.GetGUIViewportHeight                    = GTGUI.Server.SetDragAndDropProxyElementOffset;"

                "GTEngine.Keys = GTGUI.Keys;"
            );



            ///////////////////////////////////////////
            // CollisionGroupMask.

            successful = successful & script.Execute
            (
                "GTEngine.__CollisionGroupMask = {};"
                "GTEngine.__CollisionGroupMask.__index = GTEngine.__CollisionGroupMask;"

                "GTEngine.__CollisionGroupMask.Add = function(self, groupIndex)"
                "    if groupIndex == -1 then"
                "        self.bitfield = -1;"
                "    elseif groupIndex > 0 then"
                "        self.bitfield = bit.bor(self.bitfield, bit.lshift(1, groupIndex - 1));"
                "    end;"
                "end;"

                "GTEngine.__CollisionGroupMask.Remove = function(self, groupIndex)"
                "    if groupIndex == -1 then"
                "        self.bitfield = -1;"
                "    elseif groupIndex > 0 then"
                "        self.bitfield = bit.band(self.bitfield, bit.bswap(bit.lshift(1, groupIndex - 1)));"
                "    end;"
                "end;"

                "function GTEngine.CollisionGroupMask()"
                "    local new = {};"
                "    setmetatable(new, GTEngine.__CollisionGroupMask);"
                "        new.bitfield = 0;"
                "    return new;"
                "end;"
            );



            successful = successful && LoadRenderingLibrary(script);
            successful = successful && LoadAudioLibrary(script);
            successful = successful && LoadSceneLibrary(script);
            successful = successful && LoadSceneNodeLibrary(script);
            successful = successful && LoadComponentsLibrary(script);
            successful = successful && LoadParticlesLibrary(script);
            successful = successful && LoadEditorLibrary(script);
            successful = successful && LoadAnimationLibrary(script);



            // Here we load the FFI.
            script.GetGlobal("GTEngine");
            if (script.IsTable(-1))
            {
                script.SetTableFunction(-1, "GetExecutableDirectory", FFI::GetExecutableDirectory);
                script.SetTableFunction(-1, "GetVersionString",       FFI::GetVersionString);

                script.SetTableFunction(-1, "IsModelFile",        FFI::IsModelFile);
                script.SetTableFunction(-1, "IsImageFile",        FFI::IsImageFile);
                script.SetTableFunction(-1, "IsSoundFile",        FFI::IsSoundFile);
                script.SetTableFunction(-1, "IsSceneFile",        FFI::IsSceneFile);
                script.SetTableFunction(-1, "IsPrefabFile",       FFI::IsPrefabFile);
                script.SetTableFunction(-1, "IsScriptFile",       FFI::IsScriptFile);

                script.SetTableFunction(-1, "CreatePrefab",       FFI::CreatePrefab);

                script.SetTableFunction(-1, "ExecuteFile",        FFI::ExecuteFile);
                script.SetTableFunction(-1, "ExecuteScript",      FFI::ExecuteScript);
                script.SetTableFunction(-1, "GetLastScriptError", FFI::GetLastScriptError);

                script.SetTableFunction(-1, "RandomInteger",      FFI::RandomInteger);
                script.SetTableFunction(-1, "RandomFloat",        FFI::RandomFloat);
            }
            script.Pop(1);

            return successful;
        }





        void PostEvent_OnMouseMove(GTLib::Script &script, int mousePosX, int mousePosY)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnMouseMove");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushNewTable();

                    script.Push("x");
                    script.Push(mousePosX);
                    script.SetTableValue(-3);

                    script.Push("y");
                    script.Push(mousePosY);
                    script.SetTableValue(-3);

                    script.Call(1, 0);
                }
            }
            script.Pop(1);


            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnMouseMove");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Push(mousePosX);
                                script.Push(mousePosY);
                                script.Call(3, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void PostEvent_OnMouseWheel(GTLib::Script &script, int mousePosX, int mousePosY, int delta)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnMouseWheel");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushNewTable();

                    script.Push("x");
                    script.Push(mousePosX);
                    script.SetTableValue(-3);

                    script.Push("y");
                    script.Push(mousePosY);
                    script.SetTableValue(-3);

                    script.Push("delta");
                    script.Push(delta);
                    script.SetTableValue(-3);

                    script.Call(1, 0);
                }
            }
            script.Pop(1);


            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnMouseWheel");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Push(mousePosX);
                                script.Push(mousePosY);
                                script.Push(delta);
                                script.Call(4, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void PostEvent_OnMouseButtonDown(GTLib::Script &script, int mousePosX, int mousePosY, GTLib::MouseButton button)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnMouseButtonDown");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushNewTable();

                    script.Push("x");
                    script.Push(mousePosX);
                    script.SetTableValue(-3);

                    script.Push("y");
                    script.Push(mousePosY);
                    script.SetTableValue(-3);

                    script.Push("button");
                    script.Push(button);
                    script.SetTableValue(-3);

                    script.Call(1, 0);
                }
            }
            script.Pop(1);



            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnMouseButtonDown");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Push(mousePosX);
                                script.Push(mousePosY);
                                script.Push(static_cast<int>(button));
                                script.Call(4, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void PostEvent_OnMouseButtonUp(GTLib::Script &script, int mousePosX, int mousePosY, GTLib::MouseButton button)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnMouseButtonUp");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushNewTable();

                    script.Push("x");
                    script.Push(mousePosX);
                    script.SetTableValue(-3);

                    script.Push("y");
                    script.Push(mousePosY);
                    script.SetTableValue(-3);

                    script.Push("button");
                    script.Push(static_cast<int>(button));
                    script.SetTableValue(-3);

                    script.Call(1, 0);
                }
            }
            script.Pop(1);



            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnMouseButtonUp");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Push(mousePosX);
                                script.Push(mousePosY);
                                script.Push(static_cast<int>(button));
                                script.Call(4, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void PostEvent_OnMouseButtonDoubleClick(GTLib::Script &script, int mousePosX, int mousePosY, GTLib::MouseButton button)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnMouseButtonDoubleClick");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushNewTable();

                    script.Push("x");
                    script.Push(mousePosX);
                    script.SetTableValue(-3);

                    script.Push("y");
                    script.Push(mousePosY);
                    script.SetTableValue(-3);

                    script.Push("button");
                    script.Push(static_cast<int>(button));
                    script.SetTableValue(-3);

                    script.Call(1, 0);
                }
            }
            script.Pop(1);



            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnMouseButtonDoubleClick");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Push(mousePosX);
                                script.Push(mousePosY);
                                script.Push(static_cast<int>(button));
                                script.Call(4, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void PostEvent_OnKeyPressed(GTLib::Script &script, GTLib::Key key)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnKeyPressed");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushNewTable();

                    script.Push("key");
                    script.Push(static_cast<int>(key));
                    script.SetTableValue(-3);

                    script.Call(1, 0);
                }
            }
            script.Pop(1);



            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnKeyPressed");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Push(static_cast<int>(key));
                                script.Call(2, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void PostEvent_OnKeyReleased(GTLib::Script &script, GTLib::Key key)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnKeyReleased");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushNewTable();

                    script.Push("key");
                    script.Push(static_cast<int>(key));
                    script.SetTableValue(-3);

                    script.Call(1, 0);
                }
            }
            script.Pop(1);



            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnKeyReleased");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Push(static_cast<int>(key));
                                script.Call(2, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void PostEvent_OnGamePause(GTLib::Script &script)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnPause");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Call(0, 0);
                }
            }
            script.Pop(1);



            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnGamePause");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Call(1, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void PostEvent_OnGameResume(GTLib::Script &script)
        {
            // Game.
            script.GetGlobal("Game");
            assert(script.IsTable(-1));
            {
                script.Push("OnResume");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Call(0, 0);
                }
            }
            script.Pop(1);



            // Scene Nodes.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("PostSceneNodeEvent_OnGameResume");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);       // <-- 'self'
                                script.Call(1, 0);
                            }
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }



        bool LoadScriptDefinition(GTLib::Script &script, const char* scriptRelativePath, const char* scriptString)
        {
            // We actually want to do this as a text script for now.
            GTLib::Strings::List<char> fullScriptString;
            fullScriptString.Append("GTEngine.__CreateScriptClass = function()");
            fullScriptString.Append("    local self = {}");
            fullScriptString.Append("    "); fullScriptString.Append(scriptString);
            fullScriptString.Append("    return self;");
            fullScriptString.Append("end;");
            fullScriptString.Append("GTEngine.ScriptDefinitions['"); fullScriptString.Append(scriptRelativePath); fullScriptString.Append("'] = GTEngine.__CreateScriptClass();");

            return script.Execute(fullScriptString.c_str());
        }

        void UnloadScriptDefinition(GTLib::Script &script, const char* scriptRelativePath)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("ScriptDefinitions");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(scriptRelativePath);    // Key   - The file path.
                    script.PushNil();                   // Value - Lua object, or in this case nil so that it's removed.
                    script.SetTableValue(-3);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }


        namespace FFI
        {
            int GetExecutableDirectory(GTLib::Script &script)
            {
                script.Push(GTEngine::GetExecutableDirectory());
                return 1;
            }

            int GetVersionString(GTLib::Script &script)
            {
                script.Push(GTEngine::GetVersionString());
                return 1;
            }


            int IsModelFile(GTLib::Script &script)
            {
                script.Push(GT::IsSupportedModelExtension(script.ToString(1)));
                return 1;
            }

            int IsImageFile(GTLib::Script &script)
            {
                script.Push(GT::IsSupportedImageExtension(script.ToString(1)));
                return 1;
            }

            int IsSoundFile(GTLib::Script &script)
            {
                script.Push(GT::IsSupportedSoundExtension(script.ToString(1)));
                return 1;
            }

            int IsSceneFile(GTLib::Script &script)
            {
                script.Push(GT::IsSupportedSceneExtension(script.ToString(1)));
                return 1;
            }

            int IsPrefabFile(GTLib::Script &script)
            {
                script.Push(GT::IsSupportedPrefabExtension(script.ToString(1)));
                return 1;
            }

            int IsScriptFile(GTLib::Script &script)
            {
                script.Push(GT::IsSupportedScriptExtension(script.ToString(1)));
                return 1;
            }

            int IsTextFile(GTLib::Script &script)
            {
                // There can be any number of text files. Perhaps we should assume that if it's not a resouce file like a model and texture, we should assume a text file?

                auto extension = easypath_extension(script.ToString(1));

                bool result = GTLib::Strings::Equal<false>(extension, "")       ||
                              GTLib::Strings::Equal<false>(extension, "txt")    ||
                              GTLib::Strings::Equal<false>(extension, "lua")    ||
                              GTLib::Strings::Equal<false>(extension, "cfg")    ||
                              GTLib::Strings::Equal<false>(extension, "xml")    ||
                              GTLib::Strings::Equal<false>(extension, "script") ||
                              GTLib::Strings::Equal<false>(extension, "style");

                script.Push(result);
                return 1;
            }


            int CreatePrefab(GTLib::Script &script)
            {
                auto absolutePath   = script.ToString(1);
                auto makeRelativeTo = script.ToString(2);
                auto sceneNode      = static_cast<SceneNode*>(script.ToPointer(3));

                if (absolutePath != nullptr && makeRelativeTo != nullptr && sceneNode != nullptr)
                {
                    auto prefab = PrefabLibrary::Acquire(absolutePath, makeRelativeTo);
                    if (prefab != nullptr)
                    {
                        prefab->SetFromSceneNode(*sceneNode);
                        prefab->WriteToFile();

                        PrefabLibrary::Unacquire(prefab);
                    }
                }

                return 0;
            }

            int ExecuteFile(GTLib::Script &script)
            {
                script.Push(GameFFI::GetGame(script).GetScript().ExecuteFile(g_EngineContext->GetVFS(), script.ToString(1)));
                return 1;
            }

            int ExecuteScript(GTLib::Script &script)
            {
                script.Push(GameFFI::GetGame(script).ExecuteScript(script.ToString(1)));
                return 1;
            }

            int GetLastScriptError(GTLib::Script &script)
            {
                script.Push(GameFFI::GetGame(script).GetScript().GetLastError());
                return 1;
            }


            int RandomInteger(GTLib::Script &script)
            {
                script.Push(g_Random.Next(script.ToInteger(1), script.ToInteger(2)));
                return 1;
            }

            int RandomFloat(GTLib::Script &script)
            {
                script.Push(g_Random.Next(script.ToDouble(1), script.ToDouble(2)));
                return 1;
            }
        }
    }
}
