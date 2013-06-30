// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Editor.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
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
                        script.SetTableFunction(-1, "MarkAsModified",               SubEditorFFI::MarkAsModified);
                        script.SetTableFunction(-1, "UnmarkAsModified",             SubEditorFFI::UnmarkAsModified);
                    }
                    script.SetTableValue(-3);

                    script.Push("ModelEditor");
                    script.PushNewTable();
                    {
                        script.SetTableFunction(-1, "GetMaterials",                    ModelEditorFFI::GetMaterials);
                        script.SetTableFunction(-1, "SetMaterial",                     ModelEditorFFI::SetMaterial);
                        script.SetTableFunction(-1, "GetBoneCount",                    ModelEditorFFI::GetBoneCount);
                        script.SetTableFunction(-1, "GetBones",                        ModelEditorFFI::GetBones);
                        script.SetTableFunction(-1, "PlayAnimationSegmentByIndex",     ModelEditorFFI::PlayAnimationSegmentByIndex);
                        script.SetTableFunction(-1, "PlayAnimation",                   ModelEditorFFI::PlayAnimation);
                        script.SetTableFunction(-1, "ResumeAnimation",                 ModelEditorFFI::ResumeAnimation);
                        script.SetTableFunction(-1, "StopAnimation",                   ModelEditorFFI::StopAnimation);
                        script.SetTableFunction(-1, "PauseAnimation",                  ModelEditorFFI::PauseAnimation);
                        script.SetTableFunction(-1, "IsAnimationPlaying",              ModelEditorFFI::IsAnimationPlaying);
                        script.SetTableFunction(-1, "IsAnimationPaused",               ModelEditorFFI::IsAnimationPaused);
                        script.SetTableFunction(-1, "AddNewAnimationSegment",          ModelEditorFFI::AddNewAnimationSegment);
                        script.SetTableFunction(-1, "RemoveAnimationSegmentByIndex",   ModelEditorFFI::RemoveAnimationSegmentByIndex);
                        script.SetTableFunction(-1, "SetAnimationSegmentName",         ModelEditorFFI::SetAnimationSegmentName);
                        script.SetTableFunction(-1, "GetAnimationSegmentName",         ModelEditorFFI::GetAnimationSegmentName);
                        script.SetTableFunction(-1, "SetAnimationSegmentFrameIndices", ModelEditorFFI::SetAnimationSegmentFrameIndices);
                        script.SetTableFunction(-1, "GetAnimationSegmentFrameIndices", ModelEditorFFI::GetAnimationSegmentFrameIndices);
                        script.SetTableFunction(-1, "GetAnimationSegments",            ModelEditorFFI::GetAnimationSegments);
                        script.SetTableFunction(-1, "GetConvexHullBuildSettings",      ModelEditorFFI::GetConvexHullBuildSettings);
                        script.SetTableFunction(-1, "ShowConvexDecomposition",         ModelEditorFFI::ShowConvexDecomposition);
                        script.SetTableFunction(-1, "HideConvexDecomposition",         ModelEditorFFI::HideConvexDecomposition);
                        script.SetTableFunction(-1, "BuildConvexDecomposition",        ModelEditorFFI::BuildConvexDecomposition);
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
                        script.SetTableFunction(-1, "IsStopped",                           SceneEditorFFI::IsStopped);

                        script.SetTableFunction(-1, "EnablePhysicsSimulation",             SceneEditorFFI::EnablePhysicsSimulation);
                        script.SetTableFunction(-1, "DisablePhysicsSimulation",            SceneEditorFFI::DisablePhysicsSimulation);
                        script.SetTableFunction(-1, "IsPhysicsSimulationEnabled",          SceneEditorFFI::IsPhysicsSimulationEnabled);

                        script.SetTableFunction(-1, "CommitStateStackFrame",               SceneEditorFFI::CommitStateStackFrame);
                        script.SetTableFunction(-1, "ClearStateStackStagingArea",          SceneEditorFFI::ClearStateStackStagingArea);
                        script.SetTableFunction(-1, "Undo",                                SceneEditorFFI::Undo);
                        script.SetTableFunction(-1, "Redo",                                SceneEditorFFI::Redo);

                        script.SetTableFunction(-1, "CreatePrefab",                        SceneEditorFFI::CreatePrefab);
                        script.SetTableFunction(-1, "InstantiatePrefab",                   SceneEditorFFI::InstantiatePrefab);
                        script.SetTableFunction(-1, "LinkSceneNodeToPrefab",               SceneEditorFFI::LinkSceneNodeToPrefab);
                        script.SetTableFunction(-1, "UnlinkSceneNodeFromPrefab",           SceneEditorFFI::UnlinkSceneNodeFromPrefab);

                        script.SetTableFunction(-1, "GetSceneNodePtrByID",                 SceneEditorFFI::GetSceneNodePtrByID);
                        script.SetTableFunction(-1, "GetSceneNodes",                       SceneEditorFFI::GetSceneNodes);
                        script.SetTableFunction(-1, "PositionSceneNodeInFrontOfCamera",    SceneEditorFFI::PositionSceneNodeInFrontOfCamera);
                        script.SetTableFunction(-1, "SetSceneNodeTransformToCamera",       SceneEditorFFI::SetSceneNodeTransformToCamera);
                        script.SetTableFunction(-1, "SetSceneNodePositionToCamera",        SceneEditorFFI::SetSceneNodePositionToCamera);
                        script.SetTableFunction(-1, "SetSceneNodeOrientationToCamera",     SceneEditorFFI::SetSceneNodeOrientationToCamera);
                        script.SetTableFunction(-1, "SetSceneNodeScaleToCamera",           SceneEditorFFI::SetSceneNodeScaleToCamera);

                        script.SetTableFunction(-1, "ShowGrid",                            SceneEditorFFI::ShowGrid);
                        script.SetTableFunction(-1, "HideGrid",                            SceneEditorFFI::HideGrid);
                        script.SetTableFunction(-1, "IsShowingGrid",                       SceneEditorFFI::IsShowingGrid);
                        script.SetTableFunction(-1, "ShowAxisArrows",                      SceneEditorFFI::ShowAxisArrows);
                        script.SetTableFunction(-1, "HideAxisArrows",                      SceneEditorFFI::HideAxisArrows);
                        script.SetTableFunction(-1, "IsShowingAxisArrows",                 SceneEditorFFI::IsShowingAxisArrows);
                        script.SetTableFunction(-1, "ResetCamera",                         SceneEditorFFI::ResetCamera);

                        script.SetTableFunction(-1, "SetSceneName",                        SceneEditorFFI::SetSceneName);
                        script.SetTableFunction(-1, "GetSceneName",                        SceneEditorFFI::GetSceneName);
                        script.SetTableFunction(-1, "EnableSceneBackgroundClearing",       SceneEditorFFI::EnableSceneBackgroundClearing);
                        script.SetTableFunction(-1, "DisableSceneBackgroundClearing",      SceneEditorFFI::DisableSceneBackgroundClearing);
                        script.SetTableFunction(-1, "IsSceneBackgroundClearingEnabled",    SceneEditorFFI::IsSceneBackgroundClearingEnabled);
                        script.SetTableFunction(-1, "GetSceneBackgroundClearColour",       SceneEditorFFI::GetSceneBackgroundClearColour);
                        script.SetTableFunction(-1, "EnableSceneHDR",                      SceneEditorFFI::EnableSceneHDR);
                        script.SetTableFunction(-1, "DisableSceneHDR",                     SceneEditorFFI::DisableSceneHDR);
                        script.SetTableFunction(-1, "IsSceneHDREnabled",                   SceneEditorFFI::IsSceneHDREnabled);
                        script.SetTableFunction(-1, "EnableSceneBloom",                    SceneEditorFFI::EnableSceneBloom);
                        script.SetTableFunction(-1, "DisableSceneBloom",                   SceneEditorFFI::DisableSceneBloom);
                        script.SetTableFunction(-1, "IsSceneBloomEnabled",                 SceneEditorFFI::IsSceneBloomEnabled);
                    }
                    script.SetTableValue(-3);


                    script.Push("ParticleEditor");
                    script.PushNewTable();
                    {
                        script.SetTableFunction(-1, "GetParticleSystemDefinitionPtr",      ParticleEditorFFI::GetParticleSystemDefinitionPtr);
                        script.SetTableFunction(-1, "RefreshViewport",                     ParticleEditorFFI::RefreshViewport);

                        script.SetTableFunction(-1, "SetOrientation",                      ParticleEditorFFI::SetOrientation);
                        script.SetTableFunction(-1, "ShowGrid",                            ParticleEditorFFI::ShowGrid);
                        script.SetTableFunction(-1, "HideGrid",                            ParticleEditorFFI::HideGrid);
                        script.SetTableFunction(-1, "IsShowingGrid",                       ParticleEditorFFI::IsShowingGrid);
                        script.SetTableFunction(-1, "ShowAxisArrows",                      ParticleEditorFFI::ShowAxisArrows);
                        script.SetTableFunction(-1, "HideAxisArrows",                      ParticleEditorFFI::HideAxisArrows);
                        script.SetTableFunction(-1, "IsShowingAxisArrows",                 ParticleEditorFFI::IsShowingAxisArrows);
                        script.SetTableFunction(-1, "ResetCamera",                         ParticleEditorFFI::ResetCamera);
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


            int MarkAsModified(GTCore::Script &script)
            {
                auto subEditor = reinterpret_cast<SubEditor*>(script.ToPointer(1));
                if (subEditor != nullptr)
                {
                    subEditor->MarkAsModified();
                }

                return 0;
            }

            int UnmarkAsModified(GTCore::Script &script)
            {
                auto subEditor = reinterpret_cast<SubEditor*>(script.ToPointer(1));
                if (subEditor != nullptr)
                {
                    subEditor->UnmarkAsModified();
                }

                return 0;
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


            int GetBoneCount(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    script.Push(static_cast<int>(modelEditor->GetModelDefinition().GetBoneCount()));
                }
                else
                {
                    script.Push(0);
                }

                return 1;
            }

            int GetBones(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    script.PushNewTable();

                    auto &definition = modelEditor->GetModelDefinition();
                    for (size_t iBone = 0; iBone < definition.GetBoneCount(); ++iBone)
                    {
                        auto bone = definition.GetBoneByIndex(iBone);
                        assert(bone != nullptr);
                        {
                            script.Push(static_cast<int>(iBone) + 1);     // +1 because Lua is 1 based.
                            script.PushNewTable();

                            // name
                            script.SetTableValue(-1, "name", bone->GetName());

                            // parentName
                            auto parentBone = bone->GetParent();
                            if (parentBone != nullptr)
                            {
                                script.SetTableValue(-1, "parentName", parentBone->GetName());
                            }
                            else
                            {
                                script.Push("parentName");
                                script.PushNil();
                                script.SetTableValue(-3);
                            }

                            script.SetTableValue(-3);
                        }
                    }
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }

            
            int PlayAnimationSegmentByIndex(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    modelEditor->PlayAnimationSegmentByIndex(static_cast<size_t>(script.ToInteger(2)));
                }

                return 0;
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

            int ResumeAnimation(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    modelEditor->ResumeAnimation();
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

            int PauseAnimation(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    modelEditor->PauseAnimation();
                }

                return 0;
            }

            int IsAnimationPlaying(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    script.Push(modelEditor->IsAnimationPlaying());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }

            int IsAnimationPaused(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    script.Push(modelEditor->IsAnimationPaused());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int AddNewAnimationSegment(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    auto name       = script.ToString(2);
                    auto startIndex = static_cast<size_t>(script.ToInteger(3));
                    auto endIndex   = static_cast<size_t>(script.ToInteger(4));

                    modelEditor->AddNewAnimationSegment(name, startIndex, endIndex);
                }

                return 0;
            }

            int RemoveAnimationSegmentByIndex(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    modelEditor->RemoveAnimationSegmentByIndex(static_cast<size_t>(script.ToInteger(2)));
                }

                return 0;
            }

            int SetAnimationSegmentName(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    auto segmentIndex = static_cast<size_t>(script.ToInteger(2));
                    auto segmentName  = script.ToString(3);

                    modelEditor->SetAnimationSegmentName(segmentIndex, segmentName);
                }

                return 0;
            }

            int GetAnimationSegmentName(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    script.Push(modelEditor->GetAnimationSegmentName(static_cast<size_t>(script.ToInteger(2))));
                }
                else
                {
                    script.Push("");
                }
                
                return 1;
            }

            int SetAnimationSegmentFrameIndices(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    auto segmentIndex = static_cast<size_t>(script.ToInteger(2));
                    auto startIndex   = static_cast<size_t>(script.ToInteger(3));
                    auto endIndex     = static_cast<size_t>(script.ToInteger(4));

                    modelEditor->SetAnimationSegmentFrameIndices(segmentIndex, startIndex, endIndex);
                }

                return 0;
            }

            int GetAnimationSegmentFrameIndices(GTCore::Script &script)
            {
                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    size_t startIndex;
                    size_t endIndex;
                    modelEditor->GetAnimationSegmentFrameIndices(static_cast<size_t>(script.ToInteger(2)), startIndex, endIndex);

                    script.Push(static_cast<int>(startIndex));
                    script.Push(static_cast<int>(endIndex));
                }
                else
                {
                    script.Push(0);
                    script.Push(0);
                }

                return 2;
            }

            int GetAnimationSegments(GTCore::Script &script)
            {
                script.PushNewTable();

                auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                if (modelEditor != nullptr)
                {
                    auto &definition = modelEditor->GetModelDefinition();

                    for (size_t i = 0; i < definition.animation.GetNamedSegmentCount(); ++i)
                    {
                        auto segment = definition.animation.GetNamedSegmentByIndex(i);
                        assert(segment != nullptr);
                        {
                            // Key
                            script.Push(static_cast<int>(i));
                                
                            // Value
                            script.PushNewTable();
                            script.SetTableValue(-1, "name",          segment->name.c_str());
                            script.SetTableValue(-1, "startKeyFrame", static_cast<int>(segment->startKeyFrame));
                            script.SetTableValue(-1, "endKeyFrame",   static_cast<int>(segment->endKeyFrame));
                                
                            // Set
                            script.SetTableValue(-3);
                        }
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

            int IsStopped(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    script.Push(sceneEditor->IsStopped());
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

            int ClearStateStackStagingArea(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->ClearStateStackStagingArea();
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


            int CreatePrefab(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    auto absolutePath   = script.ToString(2);
                    auto makeRelativeTo = script.ToString(3);
                    auto sceneNode      = reinterpret_cast<SceneNode*>(script.ToPointer(4));

                    if (sceneNode != nullptr)
                    {
                        script.Push(sceneEditor->CreatePrefab(absolutePath, makeRelativeTo, *sceneNode));
                    }
                }

                return 1;
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

            int LinkSceneNodeToPrefab(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    auto sceneNode          = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                    auto prefabRelativePath = script.ToString(3);
                    auto isSourceSceneNode  = script.ToBoolean(4);

                    if (sceneNode != nullptr)
                    {
                        sceneEditor->LinkSceneNodeToPrefab(*sceneNode, prefabRelativePath, isSourceSceneNode);
                    }
                }

                return 0;
            }

            int UnlinkSceneNodeFromPrefab(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                    if (sceneNode != nullptr)
                    {
                        sceneEditor->UnlinkSceneNodeFromPrefab(*sceneNode);
                    }
                }

                return 0;
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

            int SetSceneNodeTransformToCamera(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (sceneEditor != nullptr && sceneNode != nullptr)
                {
                    glm::vec3 position;
                    glm::quat orientation;
                    glm::vec3 scale;
                    sceneEditor->GetCameraSceneNode().GetWorldTransformComponents(position, orientation, scale);

                    sceneNode->SetWorldPosition(position);
                    sceneNode->SetWorldOrientation(orientation);
                    sceneNode->SetWorldScale(scale);
                }

                return 0;
            }

            int SetSceneNodePositionToCamera(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (sceneEditor != nullptr && sceneNode != nullptr)
                {
                    sceneNode->SetWorldPosition(sceneEditor->GetCameraSceneNode().GetWorldPosition());
                }

                return 0;
            }

            int SetSceneNodeOrientationToCamera(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (sceneEditor != nullptr && sceneNode != nullptr)
                {
                    sceneNode->SetWorldOrientation(sceneEditor->GetCameraSceneNode().GetWorldOrientation());
                }

                return 0;
            }

            int SetSceneNodeScaleToCamera(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                if (sceneEditor != nullptr && sceneNode != nullptr)
                {
                    sceneNode->SetWorldScale(sceneEditor->GetCameraSceneNode().GetWorldScale());
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


            int ResetCamera(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->ResetCamera();
                }

                return 0;
            }


            int SetSceneName(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->SetSceneName(script.ToString(2));
                }

                return 0;
            }

            int GetSceneName(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    script.Push(sceneEditor->GetSceneName());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int EnableSceneBackgroundClearing(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->EnableSceneBackgroundClearing(Scripting::ToVector3(script, 2));
                }

                return 0;
            }

            int DisableSceneBackgroundClearing(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->DisableSceneBackgroundClearing();
                }

                return 0;
            }

            int IsSceneBackgroundClearingEnabled(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    script.Push(sceneEditor->IsSceneBackgroundClearingEnabled());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }

            int GetSceneBackgroundClearColour(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    Scripting::PushNewVector3(script, sceneEditor->GetSceneBackgroundClearColour());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int EnableSceneHDR(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->EnableSceneHDR();
                }

                return 0;
            }

            int DisableSceneHDR(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->DisableSceneHDR();
                }

                return 0;
            }

            int IsSceneHDREnabled(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    script.Push(sceneEditor->IsSceneHDREnabled());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int EnableSceneBloom(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->EnableSceneBloom();
                }

                return 0;
            }

            int DisableSceneBloom(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    sceneEditor->DisableSceneBloom();
                }

                return 0;
            }

            int IsSceneBloomEnabled(GTCore::Script &script)
            {
                auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                if (sceneEditor != nullptr)
                {
                    script.Push(sceneEditor->IsSceneBloomEnabled());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }
        }


        namespace ParticleEditorFFI
        {
            int GetParticleSystemDefinitionPtr(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    script.Push(&particleEditor->GetParticleSystemDefinition());
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }

            int RefreshViewport(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    particleEditor->RefreshViewport();
                }

                return 0;
            }


            int SetOrientation(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    particleEditor->SetOrientation(Scripting::ToQuaternion(script, 2));
                }

                return 0;
            }


            int ShowGrid(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    particleEditor->ShowGrid();
                }

                return 0;
            }

            int HideGrid(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    particleEditor->HideGrid();
                }

                return 0;
            }

            int IsShowingGrid(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    script.Push(particleEditor->IsShowingGrid());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int ShowAxisArrows(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    particleEditor->ShowAxisArrows();
                }

                return 0;
            }

            int HideAxisArrows(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    particleEditor->HideAxisArrows();
                }

                return 0;
            }

            int IsShowingAxisArrows(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    script.Push(particleEditor->IsShowingAxisArrows());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int ResetCamera(GTCore::Script &script)
            {
                auto particleEditor = reinterpret_cast<ParticleEditor*>(script.ToPointer(1));
                if (particleEditor != nullptr)
                {
                    particleEditor->ResetCamera();
                }

                return 0;
            }
        }
    }
}