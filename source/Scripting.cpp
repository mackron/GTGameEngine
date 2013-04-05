// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/SceneNodeClassLibrary.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Physics/CollisionShapeTypes.hpp>

#include <GTCore/Path.hpp>


namespace GTEngine
{
    namespace Scripting
    {
        bool LoadGTEngineScriptLibrary(GTCore::Script &script)
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
                    script.Push("SubEditor");
                    script.PushNewTable();
                    script.SetTableValue(-3);

                    script.Push("ModelEditor");
                    script.PushNewTable();
                    script.SetTableValue(-3);

                    script.Push("SceneEditor");
                    script.PushNewTable();
                    script.SetTableValue(-3);
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
                // GTEngine.<Misc>

                script.Push("ScriptDefinitions");
                script.PushNewTable();
                script.SetTableValue(-3);
            }
            script.SetGlobal("GTEngine");




            successful = successful && LoadRenderingLibrary(script);
            successful = successful && LoadAudioLibrary(script);
            successful = successful && LoadSceneLibrary(script);
            successful = successful && LoadSceneNodeLibrary(script);
            successful = successful && LoadComponentsLibrary(script);




            // Here we load the FFI.
            script.GetGlobal("GTEngine");
            if (script.IsTable(-1))
            {
                script.SetTableFunction(-1, "GetExecutableDirectory", FFI::GetExecutableDirectory);

                script.SetTableFunction(-1, "IsModelFile",  FFI::IsModelFile);
                script.SetTableFunction(-1, "IsImageFile",  FFI::IsImageFile);
                script.SetTableFunction(-1, "IsSoundFile",  FFI::IsSoundFile);
                script.SetTableFunction(-1, "IsSceneFile",  FFI::IsSceneFile);
                script.SetTableFunction(-1, "IsPrefabFile", FFI::IsPrefabFile);
                script.SetTableFunction(-1, "IsScriptFile", FFI::IsScriptFile);

                script.SetTableFunction(-1, "CreatePrefab", FFI::CreatePrefab);


                script.Push("System");
                script.GetTableValue(-2);
                if (script.IsTable(-1))
                {
                    script.Push("SubEditor");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "GetAbsolutePath",              FFI::SystemFFI::SubEditorFFI::GetAbsolutePath);
                        script.SetTableFunction(-1, "GetRelativePath",              FFI::SystemFFI::SubEditorFFI::GetRelativePath);
                    }
                    script.Pop(1);

                    script.Push("ModelEditor");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "GetMaterials",                 FFI::SystemFFI::ModelEditorFFI::GetMaterials);
                        script.SetTableFunction(-1, "GetConvexHullBuildSettings",   FFI::SystemFFI::ModelEditorFFI::GetConvexHullBuildSettings);
                        script.SetTableFunction(-1, "PlayAnimation",                FFI::SystemFFI::ModelEditorFFI::PlayAnimation);
                        script.SetTableFunction(-1, "StopAnimation",                FFI::SystemFFI::ModelEditorFFI::StopAnimation);
                        script.SetTableFunction(-1, "SetMaterial",                  FFI::SystemFFI::ModelEditorFFI::SetMaterial);
                        script.SetTableFunction(-1, "ShowConvexDecomposition",      FFI::SystemFFI::ModelEditorFFI::ShowConvexDecomposition);
                        script.SetTableFunction(-1, "HideConvexDecomposition",      FFI::SystemFFI::ModelEditorFFI::HideConvexDecomposition);
                        script.SetTableFunction(-1, "BuildConvexDecomposition",     FFI::SystemFFI::ModelEditorFFI::BuildConvexDecomposition);
                    }
                    script.Pop(1);

                    script.Push("SceneEditor");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "GetScenePtr",                         FFI::SystemFFI::SceneEditorFFI::GetScenePtr);
                        script.SetTableFunction(-1, "DeselectAll",                         FFI::SystemFFI::SceneEditorFFI::DeselectAll);
                        script.SetTableFunction(-1, "SelectSceneNode",                     FFI::SystemFFI::SceneEditorFFI::SelectSceneNode);
                        script.SetTableFunction(-1, "DeselectSceneNode",                   FFI::SystemFFI::SceneEditorFFI::DeselectSceneNode);
                        script.SetTableFunction(-1, "GetSelectedSceneNodeCount",           FFI::SystemFFI::SceneEditorFFI::GetSelectedSceneNodeCount);
                        script.SetTableFunction(-1, "GetFirstSelectedSceneNodePtr",        FFI::SystemFFI::SceneEditorFFI::GetFirstSelectedSceneNodePtr);
                        script.SetTableFunction(-1, "GetSelectedSceneNodeIDs",             FFI::SystemFFI::SceneEditorFFI::GetSelectedSceneNodeIDs);
                        script.SetTableFunction(-1, "IsSceneNodeSelectedByID",             FFI::SystemFFI::SceneEditorFFI::IsSceneNodeSelectedByID);
                        script.SetTableFunction(-1, "TryGizmoMouseSelect",                 FFI::SystemFFI::SceneEditorFFI::TryGizmoMouseSelect);
                        script.SetTableFunction(-1, "DoMouseSelection",                    FFI::SystemFFI::SceneEditorFFI::DoMouseSelection);
                        script.SetTableFunction(-1, "RemoveSelectedSceneNodes",            FFI::SystemFFI::SceneEditorFFI::RemoveSelectedSceneNodes);
                        script.SetTableFunction(-1, "RemoveSceneNode",                     FFI::SystemFFI::SceneEditorFFI::RemoveSceneNode);
                        script.SetTableFunction(-1, "DuplicateSelectedSceneNodes",         FFI::SystemFFI::SceneEditorFFI::DuplicateSelectedSceneNodes);
                        script.SetTableFunction(-1, "DuplicateSceneNode",                  FFI::SystemFFI::SceneEditorFFI::DuplicateSceneNode);
                        script.SetTableFunction(-1, "SwitchGizmoToTranslateMode",          FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToTranslateMode);
                        script.SetTableFunction(-1, "SwitchGizmoToRotateMode",             FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToRotateMode);
                        script.SetTableFunction(-1, "SwitchGizmoToScaleMode",              FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToScaleMode);
                        script.SetTableFunction(-1, "SwitchGizmoToLocalSpace",             FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToLocalSpace);
                        script.SetTableFunction(-1, "SwitchGizmoToGlobalSpace",            FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToGlobalSpace);
                        script.SetTableFunction(-1, "ToggleGizmoSpace",                    FFI::SystemFFI::SceneEditorFFI::ToggleGizmoSpace);
                        script.SetTableFunction(-1, "IsGizmoInLocalSpace",                 FFI::SystemFFI::SceneEditorFFI::IsGizmoInLocalSpace);
                        script.SetTableFunction(-1, "IsGizmoInGlobalSpace",                FFI::SystemFFI::SceneEditorFFI::IsGizmoInGlobalSpace);

                        script.SetTableFunction(-1, "StartPlaying",                        FFI::SystemFFI::SceneEditorFFI::StartPlaying);
                        script.SetTableFunction(-1, "PausePlaying",                        FFI::SystemFFI::SceneEditorFFI::PausePlaying);
                        script.SetTableFunction(-1, "StopPlaying",                         FFI::SystemFFI::SceneEditorFFI::StopPlaying);
                        script.SetTableFunction(-1, "IsPlaying",                           FFI::SystemFFI::SceneEditorFFI::IsPlaying);
                        script.SetTableFunction(-1, "IsPaused",                            FFI::SystemFFI::SceneEditorFFI::IsPaused);

                        script.SetTableFunction(-1, "EnablePhysicsSimulation",             FFI::SystemFFI::SceneEditorFFI::EnablePhysicsSimulation);
                        script.SetTableFunction(-1, "DisablePhysicsSimulation",            FFI::SystemFFI::SceneEditorFFI::DisablePhysicsSimulation);
                        script.SetTableFunction(-1, "IsPhysicsSimulationEnabled",          FFI::SystemFFI::SceneEditorFFI::IsPhysicsSimulationEnabled);
                        script.SetTableFunction(-1, "CommitStateStackFrame",               FFI::SystemFFI::SceneEditorFFI::CommitStateStackFrame);
                        script.SetTableFunction(-1, "Undo",                                FFI::SystemFFI::SceneEditorFFI::Undo);
                        script.SetTableFunction(-1, "Redo",                                FFI::SystemFFI::SceneEditorFFI::Redo);
                        script.SetTableFunction(-1, "InstantiatePrefab",                   FFI::SystemFFI::SceneEditorFFI::InstantiatePrefab);
                        script.SetTableFunction(-1, "GetSceneNodePtrByID",                 FFI::SystemFFI::SceneEditorFFI::GetSceneNodePtrByID);
                        script.SetTableFunction(-1, "GetSceneNodes",                       FFI::SystemFFI::SceneEditorFFI::GetSceneNodes);
                        script.SetTableFunction(-1, "PositionSceneNodeInFrontOfCamera",    FFI::SystemFFI::SceneEditorFFI::PositionSceneNodeInFrontOfCamera);

                        script.SetTableFunction(-1, "ShowGrid",                            FFI::SystemFFI::SceneEditorFFI::ShowGrid);
                        script.SetTableFunction(-1, "HideGrid",                            FFI::SystemFFI::SceneEditorFFI::HideGrid);
                        script.SetTableFunction(-1, "IsShowingGrid",                       FFI::SystemFFI::SceneEditorFFI::IsShowingGrid);
                        script.SetTableFunction(-1, "ShowAxisArrows",                      FFI::SystemFFI::SceneEditorFFI::ShowAxisArrows);
                        script.SetTableFunction(-1, "HideAxisArrows",                      FFI::SystemFFI::SceneEditorFFI::HideAxisArrows);
                        script.SetTableFunction(-1, "IsShowingAxisArrows",                 FFI::SystemFFI::SceneEditorFFI::IsShowingAxisArrows);
                        script.SetTableFunction(-1, "EnableHDR",                           FFI::SystemFFI::SceneEditorFFI::EnableHDR);
                        script.SetTableFunction(-1, "DisableHDR",                          FFI::SystemFFI::SceneEditorFFI::DisableHDR);
                        script.SetTableFunction(-1, "IsHDREnabled",                        FFI::SystemFFI::SceneEditorFFI::IsHDREnabled);
                        script.SetTableFunction(-1, "EnableBloom",                         FFI::SystemFFI::SceneEditorFFI::EnableBloom);
                        script.SetTableFunction(-1, "DisableBloom",                        FFI::SystemFFI::SceneEditorFFI::DisableBloom);
                        script.SetTableFunction(-1, "IsBloomEnabled",                      FFI::SystemFFI::SceneEditorFFI::IsBloomEnabled);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }
            script.Pop(1);

            return successful;
        }


        


        void PostEvent_OnMouseMove(GTCore::Script &script, int mousePosX, int mousePosY)
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

        void PostEvent_OnMouseWheel(GTCore::Script &script, int mousePosX, int mousePosY, int delta)
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

        void PostEvent_OnMouseButtonDown(GTCore::Script &script, int mousePosX, int mousePosY, GTCore::MouseButton button)
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

        void PostEvent_OnMouseButtonUp(GTCore::Script &script, int mousePosX, int mousePosY, GTCore::MouseButton button)
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

        void PostEvent_OnMouseButtonDoubleClick(GTCore::Script &script, int mousePosX, int mousePosY, GTCore::MouseButton button)
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

        void PostEvent_OnKeyPressed(GTCore::Script &script, GTCore::Key key)
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

        void PostEvent_OnKeyReleased(GTCore::Script &script, GTCore::Key key)
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



        bool LoadScriptDefinition(GTCore::Script &script, const char* scriptRelativePath, const char* scriptString)
        {
            // We actually want to do this as a text script for now.
            GTCore::Strings::List<char> fullScriptString;
            fullScriptString.Append("GTEngine.__CreateScriptClass = function()");
            fullScriptString.Append("    local self = {}");
            fullScriptString.Append("    "); fullScriptString.Append(scriptString);
            fullScriptString.Append("    return self;");
            fullScriptString.Append("end;");
            fullScriptString.Append("GTEngine.ScriptDefinitions['"); fullScriptString.Append(scriptRelativePath); fullScriptString.Append("'] = GTEngine.__CreateScriptClass();");

            return script.Execute(fullScriptString.c_str());
        }

        void UnloadScriptDefinition(GTCore::Script &script, const char* scriptRelativePath)
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

        void SyncScriptDefinitionsWithLibrary(GTCore::Script &script)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("ScriptDefinitions");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    // 1) Find the definitions to remove.
                    GTCore::Vector<GTCore::String> definitionsToRemove;

                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        auto relativePath = script.ToString(-2);
                        if (!ScriptLibrary::IsLoaded(relativePath))
                        {
                            definitionsToRemove.PushBack(relativePath);
                        }
                    }

                    // 2) Remove applicable definitions.
                    for (size_t i = 0; i < definitionsToRemove.count; ++i)
                    {
                        script.Push(definitionsToRemove[i].c_str());
                        script.PushNil();
                        script.SetTableValue(-3);
                    }


                    // 3) Update existing definitions and add new definitions.
                    size_t definitionCount = ScriptLibrary::GetLoadedDefinitionCount();
                    for (size_t i = 0; i < definitionCount; ++i)
                    {
                        auto definition = ScriptLibrary::GetLoadedDefinitionByIndex(i);
                        assert(definition != nullptr);
                        {
                            Scripting::LoadScriptDefinition(script, definition->GetRelativePath(), definition->GetScriptString());
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }




        namespace FFI
        {
            int GetExecutableDirectory(GTCore::Script &script)
            {
                script.Push(GTEngine::GetExecutableDirectory());
                return 1;
            }


            int IsModelFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedModelExtension(script.ToString(1)));
                return 1;
            }

            int IsImageFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedImageExtension(script.ToString(1)));
                return 1;
            }

            int IsSoundFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedSoundExtension(script.ToString(1)));
                return 1;
            }

            int IsSceneFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedSceneExtension(script.ToString(1)));
                return 1;
            }

            int IsPrefabFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedPrefabExtension(script.ToString(1)));
                return 1;
            }

            int IsScriptFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedScriptExtension(script.ToString(1)));
                return 1;
            }

            int IsTextFile(GTCore::Script &script)
            {
                // There can be any number of text files. Perhaps we should assume that if it's not a resouce file like a model and texture, we should assume a text file?

                auto extension = GTCore::Path::Extension(script.ToString(1));

                bool result = GTCore::Strings::Equal<false>(extension, "")       ||
                              GTCore::Strings::Equal<false>(extension, "txt")    ||
                              GTCore::Strings::Equal<false>(extension, "lua")    ||
                              GTCore::Strings::Equal<false>(extension, "cfg")    ||
                              GTCore::Strings::Equal<false>(extension, "xml")    ||
                              GTCore::Strings::Equal<false>(extension, "script") ||
                              GTCore::Strings::Equal<false>(extension, "style");

                script.Push(result);
                return 1;
            }




            void AddSceneNodeAndChildrenToPrefab(SceneNodeClass &prefab, SceneNode &sceneNode, uint64_t parentID)
            {
                auto metadata = sceneNode.GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    uint64_t id = 0;

                    // If the prefab is already linked to a different node, we actually want to auto-generate every ID by leaving the IDs set at 0.
                    if (GTCore::Strings::Equal<false>(prefab.GetRelativePath(), metadata->GetPrefabRelativePath()))
                    {
                        // If a prefab scene node of the same ID already exists, we'll need to regenerate an ID for that node.
                        if (prefab.GetSerializerByID(metadata->GetPrefabID()) == nullptr)
                        {
                            id = metadata->GetPrefabID();
                        }
                    }

                    id = prefab.AddSingleSceneNode(sceneNode, id, parentID);


                    // The scene node needs to be linked to the prefab.
                    metadata->LinkToPrefab(prefab.GetRelativePath(), id);


                    // And now we need to add the children.
                    for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
                    {
                        AddSceneNodeAndChildrenToPrefab(prefab, *childNode, id);
                    }
                }
            }

            // TODO: Move this to GTEngine.Editor (not SceneEditor). Rename to UpdatePrefab().
            int CreatePrefab(GTCore::Script &script)
            {
                auto absolutePath   = script.ToString(1);
                auto makeRelativeTo = script.ToString(2);
                auto sceneNode      = static_cast<SceneNode*>(script.ToPointer(3));

                if (absolutePath != nullptr && makeRelativeTo != nullptr && sceneNode != nullptr)
                {
                    auto prefab = SceneNodeClassLibrary::Acquire(absolutePath, makeRelativeTo);
                    if (prefab != nullptr)
                    {
                        prefab->Clear();
                        AddSceneNodeAndChildrenToPrefab(*prefab, *sceneNode, 0);

                        prefab->WriteToFile();

                        SceneNodeClassLibrary::Unacquire(prefab);
                    }
                }

                return 0;
            }



            //////////////////////////////////////////////////
            // GTEngine.System
            namespace SystemFFI
            {
                namespace SubEditorFFI
                {
                    int GetAbsolutePath(GTCore::Script &script)
                    {
                        auto subEditor = reinterpret_cast<SubEditor*>(script.ToPointer(1));
                        if (subEditor != nullptr)
                        {
                            script.Push(subEditor->GetAbsolutePath());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }

                    int GetRelativePath(GTCore::Script &script)
                    {
                        auto subEditor = reinterpret_cast<SubEditor*>(script.ToPointer(1));
                        if (subEditor != nullptr)
                        {
                            script.Push(subEditor->GetRelativePath());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }
                }

                namespace ModelEditorFFI
                {
                    int GetMaterials(GTCore::Script &script)
                    {
                        script.PushNewTable();

                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            GTCore::Vector<GTCore::String> materials;
                            modelEditor->GetMaterials(materials);

                            for (size_t i = 0; i < materials.count; ++i)
                            {
                                script.SetTableValue(-1, static_cast<int>(i + 1), materials[i].c_str());
                            }
                        }

                        return 1;
                    }

                    int GetConvexHullBuildSettings(GTCore::Script &script)
                    {
                        ConvexHullBuildSettings settings;

                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->GetConvexHullBuildSettings(settings);
                        }

                        script.PushNewTable();
                        script.SetTableValue(-1, "compacityWeight",               settings.compacityWeight);
                        script.SetTableValue(-1, "volumeWeight",                  settings.volumeWeight);
                        script.SetTableValue(-1, "minClusters",                   static_cast<int>(settings.minClusters));
                        script.SetTableValue(-1, "verticesPerCH",                 static_cast<int>(settings.verticesPerCH));
                        script.SetTableValue(-1, "concavity",                     settings.concavity);
                        script.SetTableValue(-1, "smallClusterThreshold",         settings.smallClusterThreshold);
                        script.SetTableValue(-1, "connectedComponentsDist",       settings.connectedComponentsDist);
                        script.SetTableValue(-1, "simplifiedTriangleCountTarget", static_cast<int>(settings.simplifiedTriangleCountTarget));
                        script.SetTableValue(-1, "addExtraDistPoints",            settings.addExtraDistPoints);
                        script.SetTableValue(-1, "addFacesPoints",                settings.addFacesPoints);

                        return 1;
                    }

                    int PlayAnimation(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->PlayAnimation();
                        }

                        return 0;
                    }

                    int StopAnimation(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->StopAnimation();
                        }

                        return 0;
                    }

                    int SetMaterial(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            script.Push(modelEditor->SetMaterial(script.ToInteger(2) - 1, script.ToString(3)));     // Minus 1 in the first argument because Lua is 1 based.
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }

                    int ShowConvexDecomposition(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->ShowConvexDecomposition();
                        }

                        return 0;
                    }

                    int HideConvexDecomposition(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->HideConvexDecomposition();
                        }

                        return 0;
                    }

                    int BuildConvexDecomposition(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            ConvexHullBuildSettings settings;
                            settings.compacityWeight               = script.ToFloat(2);
                            settings.volumeWeight                  = script.ToFloat(3);
                            settings.minClusters                   = static_cast<unsigned int>(script.ToInteger(4));
                            settings.verticesPerCH                 = static_cast<unsigned int>(script.ToInteger(5));
                            settings.concavity                     = script.ToFloat(6);
                            settings.smallClusterThreshold         = script.ToFloat(7);
                            settings.connectedComponentsDist       = script.ToFloat(8);
                            settings.simplifiedTriangleCountTarget = static_cast<unsigned int>(script.ToInteger(9));
                            settings.addExtraDistPoints            = script.ToBoolean(10);
                            settings.addFacesPoints                = script.ToBoolean(11);

                            modelEditor->BuildConvexDecomposition(settings);
                        }

                        return 0;
                    }
                }

                namespace SceneEditorFFI
                {
                    int GetScenePtr(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(&sceneEditor->GetScene());
                        }
                        else
                        {
                            script.Push(static_cast<void*>(nullptr));
                        }

                        return 1;
                    }

                    int DeselectAll(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DeselectAll(0);
                        }

                        return 0;
                    }

                    int SelectSceneNode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                            if (sceneNode != nullptr)
                            {
                                unsigned int selectionOptions = 0;
                                if (script.ToBoolean(3))
                                {
                                    selectionOptions |= SceneEditor::SelectionOption_NoScriptNotify;
                                }

                                sceneEditor->SelectSceneNode(*sceneNode, selectionOptions);
                            }
                        }

                        return 0;
                    }

                    int DeselectSceneNode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                            if (sceneNode != nullptr)
                            {
                                unsigned int selectionOptions = 0;
                                if (script.ToBoolean(3))
                                {
                                    selectionOptions |= SceneEditor::SelectionOption_NoScriptNotify;
                                }

                                sceneEditor->DeselectSceneNode(*sceneNode, selectionOptions);
                            }
                        }

                        return 0;
                    }

                    int GetSelectedSceneNodeCount(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(static_cast<int>(sceneEditor->GetSelectedSceneNodeCount()));
                        }
                        else
                        {
                            script.Push(0);
                        }

                        return 1;
                    }

                    int GetFirstSelectedSceneNodePtr(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->GetFirstSelectedSceneNode());
                        }
                        else
                        {
                            script.Push(static_cast<void*>(nullptr));
                        }

                        return 1;
                    }


                    int GetSelectedSceneNodeIDs(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            GTCore::Vector<uint64_t> selectedSceneNodeIDs;
                            sceneEditor->GetSelectedSceneNodeIDs(selectedSceneNodeIDs);

                            script.PushNewTable();
                            {
                                for (size_t i = 0; i < selectedSceneNodeIDs.count; ++i)
                                {
                                    script.SetTableValue(-1, static_cast<int>(i + 1), static_cast<int>(selectedSceneNodeIDs[i]));
                                }
                            }
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }

                    int IsSceneNodeSelectedByID(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            auto sceneNode = sceneEditor->GetSceneNodeByID(static_cast<uint64_t>(script.ToInteger(2)));
                            if (sceneNode != nullptr)
                            {
                                script.Push(sceneEditor->IsSceneNodeSelected(*sceneNode));
                            }
                            else
                            {
                                script.Push(false);
                            }
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int TryGizmoMouseSelect(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->TryGizmoMouseSelect());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }

                    int DoMouseSelection(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DoMouseSelection();
                        }

                        return 0;
                    }


                    int RemoveSelectedSceneNodes(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->RemoveSelectedSceneNodes();
                        }

                        return 0;
                    }

                    int RemoveSceneNode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneEditor != nullptr && sceneNode != nullptr)
                        {
                            sceneEditor->RemoveSceneNode(*sceneNode);
                        }

                        return 0;
                    }


                    int DuplicateSelectedSceneNodes(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DuplicateSelectedSceneNodes();
                        }

                        return 0;
                    }

                    int DuplicateSceneNode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneEditor != nullptr && sceneNode != nullptr)
                        {
                            sceneEditor->DuplicateSceneNode(*sceneNode);
                        }

                        return 0;
                    }



                    int SwitchGizmoToTranslateMode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToTranslateMode();
                        }

                        return 0;
                    }

                    int SwitchGizmoToRotateMode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToRotateMode();
                        }

                        return 0;
                    }

                    int SwitchGizmoToScaleMode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToScaleMode();
                        }

                        return 0;
                    }

                    int SwitchGizmoToLocalSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToLocalSpace();
                        }

                        return 0;
                    }

                    int SwitchGizmoToGlobalSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToGlobalSpace();
                        }

                        return 0;
                    }

                    int ToggleGizmoSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->ToggleGizmoSpace();
                        }

                        return 0;
                    }

                    int IsGizmoInLocalSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsGizmoInLocalSpace());
                        }
                        else
                        {
                            script.Push(true);      // Local space by default.
                        }

                        return 1;
                    }

                    int IsGizmoInGlobalSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsGizmoInGlobalSpace());
                        }
                        else
                        {
                            script.Push(true);      // Global space by default.
                        }

                        return 1;
                    }


                    int StartPlaying(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->StartPlaying();
                        }

                        return 0;
                    }

                    int PausePlaying(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->PausePlaying();
                        }

                        return 0;
                    }

                    int StopPlaying(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->StopPlaying();
                        }

                        return 0;
                    }

                    int IsPlaying(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsPlaying());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }

                    int IsPaused(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsPaused());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int EnablePhysicsSimulation(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->EnablePhysicsSimulation();
                        }

                        return 0;
                    }

                    int DisablePhysicsSimulation(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DisablePhysicsSimulation();
                        }

                        return 0;
                    }

                    int IsPhysicsSimulationEnabled(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsPhysicsSimulationEnabled());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int CommitStateStackFrame(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->CommitStateStackFrame();
                        }

                        return 0;
                    }

                    int Undo(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->Undo();
                        }

                        return 0;
                    }

                    int Redo(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->Redo();
                        }

                        return 0;
                    }


                    int InstantiatePrefab(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->InstantiatePrefab(script.ToString(2)));
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }


                    int GetSceneNodePtrByID(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            uint64_t sceneNodeID = static_cast<uint64_t>(script.ToInteger(2));
                            script.Push(sceneEditor->GetScene().GetSceneNodeByID(sceneNodeID));
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }


                    int GetSceneNodes(GTCore::Script &script)
                    {
                        script.PushNewTable();

                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            auto &sceneNodes = sceneEditor->GetScene().GetSceneNodes();

                            for (size_t i = 0; i < sceneNodes.GetCount(); ++i)
                            {
                                script.SetTableValue(-1, static_cast<int>(i + 1), sceneNodes.GetSceneNodeAtIndex(i));
                            }
                        }

                        return 1;
                    }


                    int PositionSceneNodeInFrontOfCamera(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneEditor != nullptr && sceneNode != nullptr)
                        {
                            auto &cameraSceneNode = sceneEditor->GetCameraSceneNode();
                            {
                                sceneNode->SetWorldPosition(cameraSceneNode.GetWorldPosition() + (cameraSceneNode.GetWorldForwardVector() * 15.0f));
                            }
                        }

                        return 0;
                    }


                    int ShowGrid(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->ShowGrid();
                        }

                        return 0;
                    }

                    int HideGrid(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->HideGrid();
                        }

                        return 0;
                    }

                    int IsShowingGrid(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsShowingGrid());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int ShowAxisArrows(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->ShowAxisArrows();
                        }

                        return 0;
                    }

                    int HideAxisArrows(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->HideAxisArrows();
                        }

                        return 0;
                    }

                    int IsShowingAxisArrows(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsShowingAxisArrows());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int EnableHDR(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->EnableHDR();
                        }

                        return 0;
                    }

                    int DisableHDR(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DisableHDR();
                        }

                        return 0;
                    }

                    int IsHDREnabled(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsHDREnabled());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int EnableBloom(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->EnableBloom();
                        }

                        return 0;
                    }

                    int DisableBloom(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DisableBloom();
                        }

                        return 0;
                    }

                    int IsBloomEnabled(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsBloomEnabled());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }
                }
            }
        }
    }
}
