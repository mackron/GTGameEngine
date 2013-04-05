// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Editor.hpp>
#include <GTEngine/Editor.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadEditorLibrary(GTCore::Script &script)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("System");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push("SubEditor");
                    script.PushNewTable();
                    {
                        script.SetTableFunction(-1, "GetAbsolutePath",              SubEditorFFI::GetAbsolutePath);
                        script.SetTableFunction(-1, "GetRelativePath",              SubEditorFFI::GetRelativePath);
                    }
                    script.SetTableValue(-3);

                    script.Push("ModelEditor");
                    script.PushNewTable();
                    {
                        script.SetTableFunction(-1, "GetMaterials",                 ModelEditorFFI::GetMaterials);
                        script.SetTableFunction(-1, "GetConvexHullBuildSettings",   ModelEditorFFI::GetConvexHullBuildSettings);
                        script.SetTableFunction(-1, "PlayAnimation",                ModelEditorFFI::PlayAnimation);
                        script.SetTableFunction(-1, "StopAnimation",                ModelEditorFFI::StopAnimation);
                        script.SetTableFunction(-1, "SetMaterial",                  ModelEditorFFI::SetMaterial);
                        script.SetTableFunction(-1, "ShowConvexDecomposition",      ModelEditorFFI::ShowConvexDecomposition);
                        script.SetTableFunction(-1, "HideConvexDecomposition",      ModelEditorFFI::HideConvexDecomposition);
                        script.SetTableFunction(-1, "BuildConvexDecomposition",     ModelEditorFFI::BuildConvexDecomposition);
                    }
                    script.SetTableValue(-3);

                    script.Push("SceneEditor");
                    script.PushNewTable();
                    {
                        script.SetTableFunction(-1, "GetScenePtr",                         SceneEditorFFI::GetScenePtr);
                        script.SetTableFunction(-1, "DeselectAll",                         SceneEditorFFI::DeselectAll);
                        script.SetTableFunction(-1, "SelectSceneNode",                     SceneEditorFFI::SelectSceneNode);
                        script.SetTableFunction(-1, "DeselectSceneNode",                   SceneEditorFFI::DeselectSceneNode);
                        script.SetTableFunction(-1, "GetSelectedSceneNodeCount",           SceneEditorFFI::GetSelectedSceneNodeCount);
                        script.SetTableFunction(-1, "GetFirstSelectedSceneNodePtr",        SceneEditorFFI::GetFirstSelectedSceneNodePtr);
                        script.SetTableFunction(-1, "GetSelectedSceneNodeIDs",             SceneEditorFFI::GetSelectedSceneNodeIDs);
                        script.SetTableFunction(-1, "IsSceneNodeSelectedByID",             SceneEditorFFI::IsSceneNodeSelectedByID);
                        script.SetTableFunction(-1, "TryGizmoMouseSelect",                 SceneEditorFFI::TryGizmoMouseSelect);
                        script.SetTableFunction(-1, "DoMouseSelection",                    SceneEditorFFI::DoMouseSelection);
                        script.SetTableFunction(-1, "RemoveSelectedSceneNodes",            SceneEditorFFI::RemoveSelectedSceneNodes);
                        script.SetTableFunction(-1, "RemoveSceneNode",                     SceneEditorFFI::RemoveSceneNode);
                        script.SetTableFunction(-1, "DuplicateSelectedSceneNodes",         SceneEditorFFI::DuplicateSelectedSceneNodes);
                        script.SetTableFunction(-1, "DuplicateSceneNode",                  SceneEditorFFI::DuplicateSceneNode);
                        script.SetTableFunction(-1, "SwitchGizmoToTranslateMode",          SceneEditorFFI::SwitchGizmoToTranslateMode);
                        script.SetTableFunction(-1, "SwitchGizmoToRotateMode",             SceneEditorFFI::SwitchGizmoToRotateMode);
                        script.SetTableFunction(-1, "SwitchGizmoToScaleMode",              SceneEditorFFI::SwitchGizmoToScaleMode);
                        script.SetTableFunction(-1, "SwitchGizmoToLocalSpace",             SceneEditorFFI::SwitchGizmoToLocalSpace);
                        script.SetTableFunction(-1, "SwitchGizmoToGlobalSpace",            SceneEditorFFI::SwitchGizmoToGlobalSpace);
                        script.SetTableFunction(-1, "ToggleGizmoSpace",                    SceneEditorFFI::ToggleGizmoSpace);
                        script.SetTableFunction(-1, "IsGizmoInLocalSpace",                 SceneEditorFFI::IsGizmoInLocalSpace);
                        script.SetTableFunction(-1, "IsGizmoInGlobalSpace",                SceneEditorFFI::IsGizmoInGlobalSpace);

                        script.SetTableFunction(-1, "StartPlaying",                        SceneEditorFFI::StartPlaying);
                        script.SetTableFunction(-1, "PausePlaying",                        SceneEditorFFI::PausePlaying);
                        script.SetTableFunction(-1, "StopPlaying",                         SceneEditorFFI::StopPlaying);
                        script.SetTableFunction(-1, "IsPlaying",                           SceneEditorFFI::IsPlaying);
                        script.SetTableFunction(-1, "IsPaused",                            SceneEditorFFI::IsPaused);

                        script.SetTableFunction(-1, "EnablePhysicsSimulation",             SceneEditorFFI::EnablePhysicsSimulation);
                        script.SetTableFunction(-1, "DisablePhysicsSimulation",            SceneEditorFFI::DisablePhysicsSimulation);
                        script.SetTableFunction(-1, "IsPhysicsSimulationEnabled",          SceneEditorFFI::IsPhysicsSimulationEnabled);
                        script.SetTableFunction(-1, "CommitStateStackFrame",               SceneEditorFFI::CommitStateStackFrame);
                        script.SetTableFunction(-1, "Undo",                                SceneEditorFFI::Undo);
                        script.SetTableFunction(-1, "Redo",                                SceneEditorFFI::Redo);
                        script.SetTableFunction(-1, "InstantiatePrefab",                   SceneEditorFFI::InstantiatePrefab);
                        script.SetTableFunction(-1, "GetSceneNodePtrByID",                 SceneEditorFFI::GetSceneNodePtrByID);
                        script.SetTableFunction(-1, "GetSceneNodes",                       SceneEditorFFI::GetSceneNodes);
                        script.SetTableFunction(-1, "PositionSceneNodeInFrontOfCamera",    SceneEditorFFI::PositionSceneNodeInFrontOfCamera);

                        script.SetTableFunction(-1, "ShowGrid",                            SceneEditorFFI::ShowGrid);
                        script.SetTableFunction(-1, "HideGrid",                            SceneEditorFFI::HideGrid);
                        script.SetTableFunction(-1, "IsShowingGrid",                       SceneEditorFFI::IsShowingGrid);
                        script.SetTableFunction(-1, "ShowAxisArrows",                      SceneEditorFFI::ShowAxisArrows);
                        script.SetTableFunction(-1, "HideAxisArrows",                      SceneEditorFFI::HideAxisArrows);
                        script.SetTableFunction(-1, "IsShowingAxisArrows",                 SceneEditorFFI::IsShowingAxisArrows);
                        script.SetTableFunction(-1, "EnableHDR",                           SceneEditorFFI::EnableHDR);
                        script.SetTableFunction(-1, "DisableHDR",                          SceneEditorFFI::DisableHDR);
                        script.SetTableFunction(-1, "IsHDREnabled",                        SceneEditorFFI::IsHDREnabled);
                        script.SetTableFunction(-1, "EnableBloom",                         SceneEditorFFI::EnableBloom);
                        script.SetTableFunction(-1, "DisableBloom",                        SceneEditorFFI::DisableBloom);
                        script.SetTableFunction(-1, "IsBloomEnabled",                      SceneEditorFFI::IsBloomEnabled);
                    }
                    script.SetTableValue(-3);
                }
                script.Pop(1);
            }
            script.Pop(1);



            return true;
        }


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