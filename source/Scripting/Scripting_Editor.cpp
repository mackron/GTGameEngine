// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Scripting/Scripting_Editor.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/Editor.hpp>

namespace GT
{
    bool LoadEditorLibrary(GTLib::Script &script)
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
                    script.SetTableFunction(-1, "GetMeshNames",                    ModelEditorFFI::GetMeshNames);
                    script.SetTableFunction(-1, "GetMaterials",                    ModelEditorFFI::GetMaterials);
                    script.SetTableFunction(-1, "GetMaterialRelativePath",         ModelEditorFFI::GetMaterialRelativePath);
                    script.SetTableFunction(-1, "GetMaterialUniformValues",        ModelEditorFFI::GetMaterialUniformValues);
                    script.SetTableFunction(-1, "SetMaterial",                     ModelEditorFFI::SetMaterial);
                    script.SetTableFunction(-1, "SetMaterialUniform_Float",        ModelEditorFFI::SetMaterialUniform_Float);
                    script.SetTableFunction(-1, "SetMaterialUniform_Float2",       ModelEditorFFI::SetMaterialUniform_Float2);
                    script.SetTableFunction(-1, "SetMaterialUniform_Float3",       ModelEditorFFI::SetMaterialUniform_Float3);
                    script.SetTableFunction(-1, "SetMaterialUniform_Float4",       ModelEditorFFI::SetMaterialUniform_Float4);
                    script.SetTableFunction(-1, "SetMaterialUniform_Texture2D",    ModelEditorFFI::SetMaterialUniform_Texture2D);
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
                    script.SetTableFunction(-1, "GetViewportCameraSceneNodePtr",   ModelEditorFFI::GetViewportCameraSceneNodePtr);
                    script.SetTableFunction(-1, "GetModelAABB",                    ModelEditorFFI::GetModelAABB);
                }
                script.SetTableValue(-3);


                script.Push("MaterialEditor");
                script.PushNewTable();
                {
                    script.SetTableFunction(-1, "GetViewportCameraSceneNodePtr",   MaterialEditorFFI::GetViewportCameraSceneNodePtr);
                }
                script.SetTableValue(-3);


                script.Push("SceneEditor");
                script.PushNewTable();
                {
                    script.SetTableFunction(-1, "GetScenePtr",                         SceneEditorFFI::GetScenePtr);

                    script.SetTableFunction(-1, "SetInsertionPosition",                     SceneEditorFFI::SetInsertionPosition);
                    script.SetTableFunction(-1, "GetInsertionPosition",                     SceneEditorFFI::GetInsertionPosition);
                    script.SetTableFunction(-1, "UpdateInsertionPositionFromMouse",         SceneEditorFFI::UpdateInsertionPositionFromMouse);
                    script.SetTableFunction(-1, "UpdateInsertionPositionToInFrontOfCamera", SceneEditorFFI::UpdateInsertionPositionToInFrontOfCamera);

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
                    script.SetTableFunction(-1, "UpdateSelectionGizmoTransform",       SceneEditorFFI::UpdateSelectionGizmoTransform);
                    script.SetTableFunction(-1, "SetTranslationSnappingInterval",      SceneEditorFFI::SetTranslationSnappingInterval);
                    script.SetTableFunction(-1, "GetTranslationSnappingInterval",      SceneEditorFFI::GetTranslationSnappingInterval);
                    script.SetTableFunction(-1, "EnableSnapToGrid",                    SceneEditorFFI::EnableSnapToGrid);
                    script.SetTableFunction(-1, "DisableSnapToGrid",                   SceneEditorFFI::DisableSnapToGrid);
                    script.SetTableFunction(-1, "IsSnappingToGrid",                    SceneEditorFFI::IsSnappingToGrid);

                    script.SetTableFunction(-1, "StartPlaying",                        SceneEditorFFI::StartPlaying);
                    script.SetTableFunction(-1, "PausePlaying",                        SceneEditorFFI::PausePlaying);
                    script.SetTableFunction(-1, "StopPlaying",                         SceneEditorFFI::StopPlaying);
                    script.SetTableFunction(-1, "IsPlaying",                           SceneEditorFFI::IsPlaying);
                    script.SetTableFunction(-1, "IsPaused",                            SceneEditorFFI::IsPaused);
                    script.SetTableFunction(-1, "IsStopped",                           SceneEditorFFI::IsStopped);

                    script.SetTableFunction(-1, "PauseSceneUpdates",                   SceneEditorFFI::PauseSceneUpdates);
                    script.SetTableFunction(-1, "ResumeSceneUpdates",                  SceneEditorFFI::ResumeSceneUpdates);
                    script.SetTableFunction(-1, "IsSceneUpdatesPaused",                SceneEditorFFI::IsSceneUpdatesPaused);

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
                    script.SetTableFunction(-1, "SetSceneNodePositionToInsertionPosition", SceneEditorFFI::SetSceneNodePositionToInsertionPosition);
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
                    script.SetTableFunction(-1, "GetViewportCameraSceneNodePtr",       SceneEditorFFI::GetViewportCameraSceneNodePtr);

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

                    script.SetTableFunction(-1, "ShowNavigationMesh",                  SceneEditorFFI::ShowNavigationMesh);
                    script.SetTableFunction(-1, "HideNavigationMesh",                  SceneEditorFFI::HideNavigationMesh);
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
                    script.SetTableFunction(-1, "GetViewportCameraSceneNodePtr",       ParticleEditorFFI::GetViewportCameraSceneNodePtr);
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
        int GetAbsolutePath(GTLib::Script &script)
        {
            auto subEditor = reinterpret_cast<GTEngine::SubEditor*>(script.ToPointer(1));
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

        int GetRelativePath(GTLib::Script &script)
        {
            auto subEditor = reinterpret_cast<GTEngine::SubEditor*>(script.ToPointer(1));
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


        int MarkAsModified(GTLib::Script &script)
        {
            auto subEditor = reinterpret_cast<GTEngine::SubEditor*>(script.ToPointer(1));
            if (subEditor != nullptr)
            {
                subEditor->MarkAsModified();
            }

            return 0;
        }

        int UnmarkAsModified(GTLib::Script &script)
        {
            auto subEditor = reinterpret_cast<GTEngine::SubEditor*>(script.ToPointer(1));
            if (subEditor != nullptr)
            {
                subEditor->UnmarkAsModified();
            }

            return 0;
        }
    }



    namespace ModelEditorFFI
    {
        int GetMeshNames(GTLib::Script &script)
        {
            script.PushNewTable();

            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &definition = modelEditor->GetModelDefinition();

                for (size_t i = 0; i < definition.meshes.count; ++i)
                {
                    script.SetTableValue(-1, static_cast<int>(i + 1), definition.meshes[i].name.c_str());
                }
            }

            return 1;
        }


        int GetMaterials(GTLib::Script &script)
        {
            script.PushNewTable();

            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                GTLib::Vector<GTLib::String> materials;
                modelEditor->GetMaterials(materials);

                for (size_t i = 0; i < materials.count; ++i)
                {
                    script.SetTableValue(-1, static_cast<int>(i + 1), materials[i].c_str());
                }
            }

            return 1;
        }

        int GetMaterialRelativePath(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &mesh = modelEditor->GetModelDefinition().meshes[script.ToInteger(2) - 1];     // Minus 1 because Lua is 1-based.
                {
                    script.Push(mesh.material->GetDefinition().relativePath.c_str());
                }
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }

        int GetMaterialUniformValues(GTLib::Script &script)
        {
            script.PushNewTable();

            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &mesh = modelEditor->GetModelDefinition().meshes[script.ToInteger(2) - 1];     // Minus 1 because Lua is 1-based.

                if (mesh.material != nullptr)
                {
                    auto &defaultParameters = mesh.material->GetDefaultParameters();

                    // float
                    auto &floatParameters = defaultParameters.GetFloatParameters();
                    for (size_t iUniform = 0; iUniform < floatParameters.count; ++iUniform)
                    {
                        auto name  = floatParameters.buffer[iUniform]->key;
                        auto value = floatParameters.buffer[iUniform]->value;

                        auto meshValue = mesh.defaultUniforms.GetFloatParameter(name);
                        if (meshValue != nullptr)
                        {
                            value = *meshValue;
                        }


                        script.Push(name);
                        script.PushNewTable();
                        {
                            script.Push("type");
                            script.Push(value.type);
                            script.SetTableValue(-3);

                            script.Push("value");
                            script.Push(value.value);
                            script.SetTableValue(-3);
                        }
                        script.SetTableValue(-3);
                    }

                    // float2
                    auto &float2Parameters = defaultParameters.GetFloat2Parameters();
                    for (size_t iUniform = 0; iUniform < float2Parameters.count; ++iUniform)
                    {
                        auto name  = float2Parameters.buffer[iUniform]->key;
                        auto value = float2Parameters.buffer[iUniform]->value;

                        auto meshValue = mesh.defaultUniforms.GetFloat2Parameter(name);
                        if (meshValue != nullptr)
                        {
                            value = *meshValue;
                        }


                        script.Push(name);
                        script.PushNewTable();
                        {
                            script.Push("type");
                            script.Push(value.type);
                            script.SetTableValue(-3);

                            script.Push("value");
                            PushNewVector2(script, value.value);
                            script.SetTableValue(-3);
                        }
                        script.SetTableValue(-3);
                    }

                    // float3
                    auto &float3Parameters = defaultParameters.GetFloat3Parameters();
                    for (size_t iUniform = 0; iUniform < float3Parameters.count; ++iUniform)
                    {
                        auto name  = float3Parameters.buffer[iUniform]->key;
                        auto value = float3Parameters.buffer[iUniform]->value;

                        auto meshValue = mesh.defaultUniforms.GetFloat3Parameter(name);
                        if (meshValue != nullptr)
                        {
                            value = *meshValue;
                        }


                        script.Push(name);
                        script.PushNewTable();
                        {
                            script.Push("type");
                            script.Push(value.type);
                            script.SetTableValue(-3);

                            script.Push("value");
                            PushNewVector3(script, value.value);
                            script.SetTableValue(-3);
                        }
                        script.SetTableValue(-3);
                    }

                    // float4
                    auto &float4Parameters = defaultParameters.GetFloat4Parameters();
                    for (size_t iUniform = 0; iUniform < float4Parameters.count; ++iUniform)
                    {
                        auto name  = float4Parameters.buffer[iUniform]->key;
                        auto value = float4Parameters.buffer[iUniform]->value;

                        auto meshValue = mesh.defaultUniforms.GetFloat4Parameter(name);
                        if (meshValue != nullptr)
                        {
                            value = *meshValue;
                        }


                        script.Push(name);
                        script.PushNewTable();
                        {
                            script.Push("type");
                            script.Push(value.type);
                            script.SetTableValue(-3);

                            script.Push("value");
                            PushNewVector4(script, value.value);
                            script.SetTableValue(-3);
                        }
                        script.SetTableValue(-3);
                    }

                    // texture2D
                    auto &texture2DParameters = defaultParameters.GetTexture2DParameters();
                    for (size_t iUniform = 0; iUniform < texture2DParameters.count; ++iUniform)
                    {
                        auto name  = texture2DParameters.buffer[iUniform]->key;
                        auto value = texture2DParameters.buffer[iUniform]->value;

                        auto meshValue = mesh.defaultUniforms.GetTexture2DParameter(name);
                        if (meshValue != nullptr)
                        {
                            value = *meshValue;
                        }


                        script.Push(name);
                        script.PushNewTable();
                        {
                            script.Push("type");
                            script.Push(value.type);
                            script.SetTableValue(-3);

                            script.Push("value");
                            if (value.value != nullptr)
                            {
                                script.Push(value.value->GetRelativePath());
                            }
                            else
                            {
                                script.Push("engine/textures/default.png");
                            }
                            script.SetTableValue(-3);
                        }
                        script.SetTableValue(-3);
                    }
                }
            }

            return 1;
        }

        int SetMaterial(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
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


        int SetMaterialUniform_Float(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &mesh = modelEditor->GetModelDefinition().meshes[script.ToInteger(2) - 1];         // Minus 1 because Lua is 1 based.

                auto uniformName  = script.ToString(3);
                auto uniformValue = script.ToFloat(4);

                mesh.defaultUniforms.Set(uniformName, uniformValue);
                mesh.material->SetParameter(uniformName, uniformValue);
                modelEditor->RefreshViewport();

                script.Push(true);
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetMaterialUniform_Float2(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &mesh = modelEditor->GetModelDefinition().meshes[script.ToInteger(2) - 1];         // Minus 1 because Lua is 1 based.

                auto uniformName  = script.ToString(3);
                auto uniformValue = ToVector2(script, 4);

                mesh.defaultUniforms.Set(uniformName, uniformValue);
                mesh.material->SetParameter(uniformName, uniformValue);
                modelEditor->RefreshViewport();

                script.Push(true);
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetMaterialUniform_Float3(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &mesh = modelEditor->GetModelDefinition().meshes[script.ToInteger(2) - 1];         // Minus 1 because Lua is 1 based.

                auto uniformName  = script.ToString(3);
                auto uniformValue = ToVector3(script, 4);

                mesh.defaultUniforms.Set(uniformName, uniformValue);
                mesh.material->SetParameter(uniformName, uniformValue);
                modelEditor->RefreshViewport();

                script.Push(true);
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetMaterialUniform_Float4(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &mesh = modelEditor->GetModelDefinition().meshes[script.ToInteger(2) - 1];         // Minus 1 because Lua is 1 based.

                auto uniformName  = script.ToString(3);
                auto uniformValue = ToVector4(script, 4);

                mesh.defaultUniforms.Set(uniformName, uniformValue);
                mesh.material->SetParameter(uniformName, uniformValue);
                modelEditor->RefreshViewport();

                script.Push(true);
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetMaterialUniform_Texture2D(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &mesh = modelEditor->GetModelDefinition().meshes[script.ToInteger(2) - 1];         // Minus 1 because Lua is 1 based.

                auto uniformName         = script.ToString(3);
                auto textureRelativePath = script.ToString(4);

                auto texture = GTEngine::Texture2DLibrary::Acquire(textureRelativePath);
                if (texture != nullptr)
                {
                    mesh.defaultUniforms.Set(uniformName, texture);
                    mesh.material->SetParameter(uniformName, texture);
                    modelEditor->RefreshViewport();

                    GTEngine::Texture2DLibrary::Unacquire(texture);
                    script.Push(true);
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


        int GetBoneCount(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
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

        int GetBones(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
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

            
        int PlayAnimationSegmentByIndex(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->PlayAnimationSegmentByIndex(static_cast<size_t>(script.ToInteger(2)));
            }

            return 0;
        }

        int PlayAnimation(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->PlayAnimation();
            }

            return 0;
        }

        int ResumeAnimation(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->ResumeAnimation();
            }

            return 0;
        }

        int StopAnimation(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->StopAnimation();
            }

            return 0;
        }

        int PauseAnimation(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->PauseAnimation();
            }

            return 0;
        }

        int IsAnimationPlaying(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
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

        int IsAnimationPaused(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
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


        int AddNewAnimationSegment(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto name       = script.ToString(2);
                auto startIndex = static_cast<size_t>(script.ToInteger(3));
                auto endIndex   = static_cast<size_t>(script.ToInteger(4));

                modelEditor->AddNewAnimationSegment(name, startIndex, endIndex);
            }

            return 0;
        }

        int RemoveAnimationSegmentByIndex(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->RemoveAnimationSegmentByIndex(static_cast<size_t>(script.ToInteger(2)));
            }

            return 0;
        }

        int SetAnimationSegmentName(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto segmentIndex = static_cast<size_t>(script.ToInteger(2));
                auto segmentName  = script.ToString(3);

                modelEditor->SetAnimationSegmentName(segmentIndex, segmentName);
            }

            return 0;
        }

        int GetAnimationSegmentName(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
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

        int SetAnimationSegmentFrameIndices(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto segmentIndex = static_cast<size_t>(script.ToInteger(2));
                auto startIndex   = static_cast<size_t>(script.ToInteger(3));
                auto endIndex     = static_cast<size_t>(script.ToInteger(4));

                modelEditor->SetAnimationSegmentFrameIndices(segmentIndex, startIndex, endIndex);
            }

            return 0;
        }

        int GetAnimationSegmentFrameIndices(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
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

        int GetAnimationSegments(GTLib::Script &script)
        {
            script.PushNewTable();

            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                auto &definition = modelEditor->GetModelDefinition();

                for (size_t i = 0; i < definition.m_animation.GetNamedSegmentCount(); ++i)
                {
                    auto segment = definition.m_animation.GetNamedSegmentByIndex(i);
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


        int GetConvexHullBuildSettings(GTLib::Script &script)
        {
            GTEngine::ConvexHullBuildSettings settings;

            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
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

        int ShowConvexDecomposition(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->ShowConvexDecomposition();
            }

            return 0;
        }

        int HideConvexDecomposition(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->HideConvexDecomposition();
            }

            return 0;
        }

        int BuildConvexDecomposition(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                GTEngine::ConvexHullBuildSettings settings;
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


        int GetViewportCameraSceneNodePtr(GTLib::Script &script)
        {
            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                script.Push(&modelEditor->GetViewportCameraSceneNode());
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int GetModelAABB(GTLib::Script &script)
        {
            glm::vec3 aabbMin;
            glm::vec3 aabbMax;

            auto modelEditor = reinterpret_cast<GTEngine::ModelEditor*>(script.ToPointer(1));
            if (modelEditor != nullptr)
            {
                modelEditor->GetModelAABB(aabbMin, aabbMax);
            }

            PushNewVector3(script, aabbMin);
            PushNewVector3(script, aabbMax);

            return 2;
        }
    }


    namespace MaterialEditorFFI
    {
        int GetViewportCameraSceneNodePtr(GTLib::Script &script)
        {
            auto materialEditor = reinterpret_cast<GTEngine::MaterialEditor*>(script.ToPointer(1));
            if (materialEditor != nullptr)
            {
                script.Push(&materialEditor->GetViewportCameraSceneNode());
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }
    }


    namespace SceneEditorFFI
    {
        int GetScenePtr(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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



        int SetInsertionPosition(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->SetInsertionPosition(ToVector3(script, 2));
            }

            return 0;
        }

        int GetInsertionPosition(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                PushNewVector3(script, sceneEditor->GetInsertionPosition());
            }
            else
            {
                PushNewVector3(script, 0.0f, 0.0f, 0.0f);
            }

            return 1;
        }

        int UpdateInsertionPositionFromMouse(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->UpdateInsertionPositionFromMouse();
            }
                
            return 0;
        }

        int UpdateInsertionPositionToInFrontOfCamera(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->UpdateInsertionPositionToInFrontOfCamera();
            }

            return 0;
        }



        int DeselectAll(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->DeselectAll(0);
            }

            return 0;
        }

        int SelectSceneNode(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                auto sceneNode = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));
                if (sceneNode != nullptr)
                {
                    unsigned int selectionOptions = 0;
                    if (script.ToBoolean(3))
                    {
                        selectionOptions |= GTEngine::SceneEditor::SelectionOption_NoScriptNotify;
                    }

                    sceneEditor->SelectSceneNode(*sceneNode, selectionOptions);
                }
            }

            return 0;
        }

        int DeselectSceneNode(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                auto sceneNode = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));
                if (sceneNode != nullptr)
                {
                    unsigned int selectionOptions = 0;
                    if (script.ToBoolean(3))
                    {
                        selectionOptions |= GTEngine::SceneEditor::SelectionOption_NoScriptNotify;
                    }

                    sceneEditor->DeselectSceneNode(*sceneNode, selectionOptions);
                }
            }

            return 0;
        }

        int GetSelectedSceneNodeCount(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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

        int GetFirstSelectedSceneNodePtr(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int GetSelectedSceneNodeIDs(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                GTLib::Vector<uint64_t> selectedSceneNodeIDs;
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

        int IsSceneNodeSelectedByID(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int TryGizmoMouseSelect(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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

        int DoMouseSelection(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->DoMouseSelection();
            }

            return 0;
        }


        int RemoveSelectedSceneNodes(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->RemoveSelectedSceneNodes();
            }

            return 0;
        }

        int RemoveSceneNode(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            auto sceneNode   = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));

            if (sceneEditor != nullptr && sceneNode != nullptr)
            {
                sceneEditor->RemoveSceneNode(*sceneNode);
            }

            return 0;
        }


        int DuplicateSelectedSceneNodes(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->DuplicateSelectedSceneNodes();
            }

            return 0;
        }

        int DuplicateSceneNode(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            auto sceneNode   = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));

            if (sceneEditor != nullptr && sceneNode != nullptr)
            {
                sceneEditor->DuplicateSceneNode(*sceneNode);
            }

            return 0;
        }



        int SwitchGizmoToTranslateMode(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->SwitchGizmoToTranslateMode();
            }

            return 0;
        }

        int SwitchGizmoToRotateMode(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->SwitchGizmoToRotateMode();
            }

            return 0;
        }

        int SwitchGizmoToScaleMode(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->SwitchGizmoToScaleMode();
            }

            return 0;
        }

        int SwitchGizmoToLocalSpace(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->SwitchGizmoToLocalSpace();
            }

            return 0;
        }

        int SwitchGizmoToGlobalSpace(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->SwitchGizmoToGlobalSpace();
            }

            return 0;
        }

        int ToggleGizmoSpace(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->ToggleGizmoSpace();
            }

            return 0;
        }

        int IsGizmoInLocalSpace(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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

        int IsGizmoInGlobalSpace(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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

        int UpdateSelectionGizmoTransform(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->UpdateGizmoTransform();
            }

            return 0;
        }

        int SetTranslationSnappingInterval(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->SetTranslationSnappingInterval(script.ToFloat(2));
            }

            return 0;
        }

        int GetTranslationSnappingInterval(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                script.Push(sceneEditor->GetTranslationSnappingInterval());
            }
            else
            {
                script.Push(0.25f);
            }

            return 1;
        }


        int EnableSnapToGrid(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->EnableSnapToGrid();
            }

            return 0;
        }

        int DisableSnapToGrid(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->DisableSnapToGrid();
            }

            return 0;
        }

        int IsSnappingToGrid(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                script.Push(sceneEditor->IsSnappingToGrid());
            }
            else
            {
                script.Push(true);      // <-- Snap to grid by default.
            }

            return 1;
        }


        int StartPlaying(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->StartPlaying();
            }

            return 0;
        }

        int PausePlaying(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->PausePlaying();
            }

            return 0;
        }

        int StopPlaying(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->StopPlaying();
            }

            return 0;
        }

        int IsPlaying(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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

        int IsPaused(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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

        int IsStopped(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int PauseSceneUpdates(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->PauseSceneUpdates();
            }

            return 0;
        }

        int ResumeSceneUpdates(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->ResumeSceneUpdates();
            }

            return 0;
        }

        int IsSceneUpdatesPaused(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                script.Push(sceneEditor->IsSceneUpdatesPaused());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }


        int EnablePhysicsSimulation(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->EnablePhysicsSimulation();
            }

            return 0;
        }

        int DisablePhysicsSimulation(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->DisablePhysicsSimulation();
            }

            return 0;
        }

        int IsPhysicsSimulationEnabled(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int CommitStateStackFrame(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->CommitStateStackFrame();
            }

            return 0;
        }

        int ClearStateStackStagingArea(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->ClearStateStackStagingArea();
            }

            return 0;
        }

        int Undo(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->Undo();
            }

            return 0;
        }

        int Redo(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->Redo();
            }

            return 0;
        }


        int CreatePrefab(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                auto absolutePath   = script.ToString(2);
                auto makeRelativeTo = script.ToString(3);
                auto sceneNode      = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(4));

                if (sceneNode != nullptr)
                {
                    script.Push(sceneEditor->CreatePrefab(absolutePath, makeRelativeTo, *sceneNode));
                }
            }

            return 1;
        }

        int InstantiatePrefab(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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

        int LinkSceneNodeToPrefab(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                auto sceneNode          = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));
                auto prefabRelativePath = script.ToString(3);
                auto isSourceSceneNode  = script.ToBoolean(4);

                if (sceneNode != nullptr)
                {
                    sceneEditor->LinkSceneNodeToPrefab(*sceneNode, prefabRelativePath, isSourceSceneNode);
                }
            }

            return 0;
        }

        int UnlinkSceneNodeFromPrefab(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                auto sceneNode = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));
                if (sceneNode != nullptr)
                {
                    sceneEditor->UnlinkSceneNodeFromPrefab(*sceneNode);
                }
            }

            return 0;
        }


        int GetSceneNodePtrByID(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int GetSceneNodes(GTLib::Script &script)
        {
            script.PushNewTable();

            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int PositionSceneNodeInFrontOfCamera(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            auto sceneNode   = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));

            if (sceneEditor != nullptr && sceneNode != nullptr)
            {
                auto &cameraSceneNode = sceneEditor->GetCameraSceneNode();
                {
                    sceneNode->SetWorldPosition(cameraSceneNode.GetWorldPosition() + (cameraSceneNode.GetWorldForwardVector() * 15.0f));
                }
            }

            return 0;
        }

        int SetSceneNodePositionToInsertionPosition(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            auto sceneNode   = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));

            if (sceneEditor != nullptr && sceneNode != nullptr)
            {
                sceneNode->SetWorldPosition(sceneEditor->GetInsertionPosition());
            }

            return 0;
        }

        int SetSceneNodeTransformToCamera(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            auto sceneNode   = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));

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

        int SetSceneNodePositionToCamera(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            auto sceneNode   = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));

            if (sceneEditor != nullptr && sceneNode != nullptr)
            {
                sceneNode->SetWorldPosition(sceneEditor->GetCameraSceneNode().GetWorldPosition());
            }

            return 0;
        }

        int SetSceneNodeOrientationToCamera(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            auto sceneNode   = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));

            if (sceneEditor != nullptr && sceneNode != nullptr)
            {
                sceneNode->SetWorldOrientation(sceneEditor->GetCameraSceneNode().GetWorldOrientation());
            }

            return 0;
        }

        int SetSceneNodeScaleToCamera(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            auto sceneNode   = reinterpret_cast<GTEngine::SceneNode*>(script.ToPointer(2));

            if (sceneEditor != nullptr && sceneNode != nullptr)
            {
                sceneNode->SetWorldScale(sceneEditor->GetCameraSceneNode().GetWorldScale());
            }

            return 0;
        }


        int ShowGrid(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->ShowGrid();
            }

            return 0;
        }

        int HideGrid(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->HideGrid();
            }

            return 0;
        }

        int IsShowingGrid(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int ShowAxisArrows(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->ShowAxisArrows();
            }

            return 0;
        }

        int HideAxisArrows(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->HideAxisArrows();
            }

            return 0;
        }

        int IsShowingAxisArrows(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int GetViewportCameraSceneNodePtr(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                script.Push(&sceneEditor->GetCameraSceneNode());
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int SetSceneName(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->SetSceneName(script.ToString(2));
            }

            return 0;
        }

        int GetSceneName(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int EnableSceneBackgroundClearing(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->EnableSceneBackgroundClearing(ToVector3(script, 2));
            }

            return 0;
        }

        int DisableSceneBackgroundClearing(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->DisableSceneBackgroundClearing();
            }

            return 0;
        }

        int IsSceneBackgroundClearingEnabled(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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

        int GetSceneBackgroundClearColour(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                PushNewVector3(script, sceneEditor->GetSceneBackgroundClearColour());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }


        int EnableSceneHDR(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->EnableSceneHDR();
            }

            return 0;
        }

        int DisableSceneHDR(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->DisableSceneHDR();
            }

            return 0;
        }

        int IsSceneHDREnabled(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int EnableSceneBloom(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->EnableSceneBloom();
            }

            return 0;
        }

        int DisableSceneBloom(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->DisableSceneBloom();
            }

            return 0;
        }

        int IsSceneBloomEnabled(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
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


        int ShowNavigationMesh(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->ShowNavigationMesh(static_cast<size_t>(script.ToInteger(2)));
            }

            return 0;
        }

        int HideNavigationMesh(GTLib::Script &script)
        {
            auto sceneEditor = reinterpret_cast<GTEngine::SceneEditor*>(script.ToPointer(1));
            if (sceneEditor != nullptr)
            {
                sceneEditor->HideNavigationMesh(static_cast<size_t>(script.ToInteger(2)));
            }

            return 0;
        }
    }


    namespace ParticleEditorFFI
    {
        int GetParticleSystemDefinitionPtr(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
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

        int RefreshViewport(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
            if (particleEditor != nullptr)
            {
                particleEditor->RefreshViewport();
            }

            return 0;
        }


        int SetOrientation(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
            if (particleEditor != nullptr)
            {
                particleEditor->SetOrientation(ToQuaternion(script, 2));
            }

            return 0;
        }


        int ShowGrid(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
            if (particleEditor != nullptr)
            {
                particleEditor->ShowGrid();
            }

            return 0;
        }

        int HideGrid(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
            if (particleEditor != nullptr)
            {
                particleEditor->HideGrid();
            }

            return 0;
        }

        int IsShowingGrid(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
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


        int ShowAxisArrows(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
            if (particleEditor != nullptr)
            {
                particleEditor->ShowAxisArrows();
            }

            return 0;
        }

        int HideAxisArrows(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
            if (particleEditor != nullptr)
            {
                particleEditor->HideAxisArrows();
            }

            return 0;
        }

        int IsShowingAxisArrows(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
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


        int GetViewportCameraSceneNodePtr(GTLib::Script &script)
        {
            auto particleEditor = reinterpret_cast<GTEngine::ParticleEditor*>(script.ToPointer(1));
            if (particleEditor != nullptr)
            {
                script.Push(&particleEditor->GetCameraSceneNode());
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }
    }
}