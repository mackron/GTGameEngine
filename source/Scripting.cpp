
#include <GTEngine/Scripting.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/Scene.hpp>

#include <GTCore/Path.hpp>


namespace GTEngine
{
    namespace Scripting
    {
        bool LoadGTEngineScriptLibrary(GTCore::Script &script)
        {
            bool successful = true;

            successful = successful && script.Execute
            (
                "GTEngine          = {};"
                
                "GTEngine.System   = {};"
                "GTEngine.System.ModelComponent            = {};"
                "GTEngine.System.CameraComponent           = {};"
                "GTEngine.System.PointLightComponent       = {};"
                "GTEngine.System.SpotLightComponent        = {};"
                "GTEngine.System.DirectionalLightComponent = {};"
                "GTEngine.System.AmbientLightComponent     = {};"
                "GTEngine.System.EditorMetadataComponent   = {};"
                "GTEngine.System.SceneNode                 = {};"
                "GTEngine.System.Scene                     = {};"

                "GTEngine.Renderer = {};"
                "GTEngine.Audio    = {};"
            );


            // Components
            //
            // Component script objects are actually very temporary. They are only used to give an interface to allow the internal representation to be modified. When a component is retrieved
            // from a scene node, it will actually create a whole new instantiation of the object.
            successful = successful && script.Execute
            (
                "GTEngine.Components = "
                "{"
                "    Model            = 'Model';"
                "    Camera           = 'Camera';"
                "    PointLight       = 'PointLight';"
                "    SpotLight        = 'SpotLight';"
                "    DirectionalLight = 'DirectionalLight';"
                "    AmbientLight     = 'AmbientLight';"
                "    EditorMetadata   = 'EditorMetadata';"
                "};"



                // ModelComponent
                "GTEngine.ModelComponent = {};"
                "GTEngine.ModelComponent.__index = GTEngine.ModelComponent;"

                "function GTEngine.ModelComponent.Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.ModelComponent);"
                "        new._internalPtr = internalPtr;"
                "    return new;"
                "end;"

                "function GTEngine.ModelComponent:SetModel(filePath)"
                "    GTEngine.System.ModelComponent.SetModel(self._internalPtr, filePath);"
                "end;"



                // PointLightComponent
                "GTEngine.PointLightComponent = {};"
                "GTEngine.PointLightComponent.__index = GTEngine.PointLightComponent;"

                "function GTEngine.PointLightComponent.Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.PointLightComponent);"
                "        new._internalPtr = internalPtr;"
                "    return new;"
                "end;"

                "function GTEngine.PointLightComponent:SetColour(r, g, b)"
                "    GTEngine.System.PointLightComponent.SetColour(self._internalPtr, r, g, b);"
                "end;"

                "function GTEngine.PointLightComponent:GetColour()"
                "    return GTEngine.System.PointLightComponent.GetColour(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:EnableShadowCasting()"
                "    GTEngine.System.PointLightComponent.EnableShadowCasting(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:DisableShadowCasting()"
                "    GTEngine.System.PointLightComponent.DisableShadowCasting(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:IsShadowCastingEnabled()"
                "    return GTEngine.System.PointLightComponent.IsShadowCastingEnabled(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:GetConstantAttenuation()"
                "    return GTEngine.System.PointLightComponent.GetConstantAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:GetLinearAttenuation()"
                "    return GTEngine.System.PointLightComponent.GetLinearAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:GetQuadraticAttenuation()"
                "    return GTEngine.System.PointLightComponent.GetQuadraticAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:SetAttenuation(constant, linear, quadratic)"
                "    GTEngine.System.PointLightComponent.SetAttenuation(self._internalPtr, constant, linear, quadratic);"
                "end;"




                // EditorMetadataComponent
                "GTEngine.EditorMetadataComponent = {};"
                "GTEngine.EditorMetadataComponent.__index = GTEngine.EditorMetadataComponent;"

                "function GTEngine.EditorMetadataComponent.Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.EditorMetadataComponent);"
                "        new._internalPtr = internalPtr;"
                "    return new;"
                "end;"

                "function GTEngine.EditorMetadataComponent:ShowSprite(texturePath, colour)"
                "    GTEngine.System.EditorMetadataComponent.ShowSprite(self._internalPtr, texturePath, colour);"
                "end;"

                "function GTEngine.EditorMetadataComponent:HideSprite()"
                "    GTEngine.System.EditorMetadataComponent.HideSprite(self._internalPtr);"
                "end;"
            );


            // SceneNode
            successful = successful && script.Execute
            (
                "GTEngine.SceneNode = {};"
                "GTEngine.SceneNode.__index = GTEngine.SceneNode;"
                
                "function GTEngine.SceneNode:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.SceneNode);"
                "        if internalPtr == nil then"
                "            new._internalPtr       = GTEngine.System.SceneNode.Create();"
                "            new._deleteInternalPtr = true;"
                "        else"
                "            new._internalPtr       = internalPtr;"
                "            new._deleteInternalPtr = false;"
                "        end"
                "    return new;"
                "end;"

                "function GTEngine.SceneNode:Delete()"
                "    if self._deleteInternalPtr then"
                "        GTEngine.System.SceneNode.Delete(self._internalPtr);"
                "    end;"
                "end;"


                "function GTEngine.SceneNode:GetName()"
                "    return GTEngine.System.SceneNode.GetName(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetName(name)"
                "    GTEngine.System.SceneNode.SetName(self._internalPtr, name);"
                "end;"


                "function GTEngine.SceneNode:AddComponent(componentID)"
                "    return GTEngine.System.SceneNode.AddComponent(self._internalPtr, componentID);"
                "end;"

                "function GTEngine.SceneNode:RemoveComponent(componentID)"
                "    return GTEngine.System.SceneNode.RemoveComponent(self._internalPtr, componentID);"
                "end;"

                "function GTEngine.SceneNode:GetComponent(componentID)"
                "    return GTEngine.System.SceneNode.GetComponent(self._internalPtr, componentID);"
                "end;"


                "function GTEngine.SceneNode:Refresh()"
                "    return GTEngine.System.SceneNode.Refresh(self._internalPtr);"
                "end;"


                "function GTEngine.SceneNode:GetPosition()"
                "    return GTEngine.System.SceneNode.GetPosition(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetPosition(x, y, z)"
                "    return GTEngine.System.SceneNode.SetPosition(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:GetRotationXYZ()"
                "    return GTEngine.System.SceneNode.GetRotationXYZ(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetRotationXYZ(x, y, z)"
                "    return GTEngine.System.SceneNode.SetRotationXYZ(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:GetScale()"
                "    return GTEngine.System.SceneNode.GetScale(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetScale(x, y, z)"
                "    return GTEngine.System.SceneNode.SetScale(self._internalPtr, x, y, z);"
                "end;"


                "function GTEngine.SceneNode:GetAttachedComponentIDs()"
                "    return GTEngine.System.SceneNode.GetAttachedComponentIDs(self._internalPtr);"
                "end;"
            );


            // Scene
            successful = successful && script.Execute
            (
                "GTEngine.Scene = {};"
                "GTEngine.Scene.__index = GTEngine.Scene;"
                
                "function GTEngine.Scene:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.Scene);"
                "        new._internalPtr = internalPtr;"
                "    return new;"
                "end;"


                "function GTEngine.Scene:AddSceneNode(sceneNode)"
                "    GTEngine.System.Scene.AddSceneNode(self._internalPtr, sceneNode._internalPtr);"
                "end;"

                "function GTEngine.Scene.RemoveSceneNode(sceneNode)"
                "    GTEngine.System.Scene.RemoveSceneNode(self._internalPtr, sceneNode._internalPtr);"
                "end;"



                "GTEngine.RegisteredScenes = {};"
            );



            // Here we load the FFI.
            script.GetGlobal("GTEngine");
            if (script.IsTable(-1))
            {
                script.SetTableFunction(-1, "GetExecutableDirectory", FFI::GetExecutableDirectory);
                
                script.SetTableFunction(-1, "IsModelFile", FFI::IsModelFile);
                script.SetTableFunction(-1, "IsImageFile", FFI::IsImageFile);
                script.SetTableFunction(-1, "IsSoundFile", FFI::IsSoundFile);
                script.SetTableFunction(-1, "IsSceneFile", FFI::IsSceneFile);



                script.Push("System");
                script.GetTableValue(-2);
                if (script.IsTable(-1))
                {
                    script.Push("SceneNode");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "Create",                  FFI::SystemFFI::SceneNodeFFI::Create);
                        script.SetTableFunction(-1, "Delete",                  FFI::SystemFFI::SceneNodeFFI::Delete);

                        script.SetTableFunction(-1, "GetName",                 FFI::SystemFFI::SceneNodeFFI::GetName);
                        script.SetTableFunction(-1, "SetName",                 FFI::SystemFFI::SceneNodeFFI::SetName);

                        script.SetTableFunction(-1, "AddComponent",            FFI::SystemFFI::SceneNodeFFI::AddComponent);
                        script.SetTableFunction(-1, "RemoveComponent",         FFI::SystemFFI::SceneNodeFFI::RemoveComponent);
                        script.SetTableFunction(-1, "GetComponent",            FFI::SystemFFI::SceneNodeFFI::GetComponent);

                        script.SetTableFunction(-1, "Refresh",                 FFI::SystemFFI::SceneNodeFFI::Refresh);

                        script.SetTableFunction(-1, "GetPosition",             FFI::SystemFFI::SceneNodeFFI::GetPosition);
                        script.SetTableFunction(-1, "SetPosition",             FFI::SystemFFI::SceneNodeFFI::SetPosition);
                        script.SetTableFunction(-1, "GetRotationXYZ",          FFI::SystemFFI::SceneNodeFFI::GetRotationXYZ);
                        script.SetTableFunction(-1, "SetRotationXYZ",          FFI::SystemFFI::SceneNodeFFI::SetRotationXYZ);
                        script.SetTableFunction(-1, "GetScale",                FFI::SystemFFI::SceneNodeFFI::GetScale);
                        script.SetTableFunction(-1, "SetScale",                FFI::SystemFFI::SceneNodeFFI::SetScale);

                        script.SetTableFunction(-1, "GetAttachedComponentIDs", FFI::SystemFFI::SceneNodeFFI::GetAttachedComponentIDs);
                    }
                    script.Pop(1);


                    script.Push("Scene");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "AddSceneNode",    FFI::SystemFFI::SceneFFI::AddSceneNode);
                        script.SetTableFunction(-1, "RemoveSceneNode", FFI::SystemFFI::SceneFFI::RemoveSceneNode);
                    }
                    script.Pop(1);



                    script.Push("ModelComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetModel", FFI::SystemFFI::ModelComponentFFI::SetModel);
                    }
                    script.Pop(1);


                    script.Push("PointLightComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetColour",               FFI::SystemFFI::PointLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               FFI::SystemFFI::PointLightComponentFFI::GetColour);
                        script.SetTableFunction(-1, "EnableShadowCasting",     FFI::SystemFFI::PointLightComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",    FFI::SystemFFI::PointLightComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",  FFI::SystemFFI::PointLightComponentFFI::IsShadowCastingEnabled);
                        script.SetTableFunction(-1, "GetConstantAttenuation",  FFI::SystemFFI::PointLightComponentFFI::GetConstantAttenuation);
                        script.SetTableFunction(-1, "GetLinearAttenuation",    FFI::SystemFFI::PointLightComponentFFI::GetLinearAttenuation);
                        script.SetTableFunction(-1, "GetQuadraticAttenuation", FFI::SystemFFI::PointLightComponentFFI::GetQuadraticAttenuation);
                        script.SetTableFunction(-1, "SetAttenuation",          FFI::SystemFFI::PointLightComponentFFI::SetAttenuation);
                    }
                    script.Pop(1);



                    script.Push("EditorMetadataComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "ShowSprite", FFI::SystemFFI::EditorMetadataComponentFFI::ShowSprite);
                        script.SetTableFunction(-1, "HideSprite", FFI::SystemFFI::EditorMetadataComponentFFI::HideSprite);
                    }
                    script.Pop(1);
                }
                script.Pop(1);



                script.Push("Renderer");
                script.GetTableValue(-2);
                if (script.IsTable(-1))
                {
                    script.SetTableFunction(-1, "EnableVSync",  FFI::RendererFFI::EnableVSync);
                    script.SetTableFunction(-1, "DisableVSync", FFI::RendererFFI::DisableVSync);
                }
                script.Pop(1);


                script.Push("Audio");
                script.GetTableValue(-2);
                if (script.IsTable(-1))
                {
                    script.SetTableFunction(-1, "Play", FFI::AudioFFI::Play);
                }
                script.Pop(1);
            }
            script.Pop(1);

            return successful;
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


            //////////////////////////////////////////////////
            // GTEngine.System
            namespace SystemFFI
            {
                //////////////////////////////////////////////////
                // GTEngine.System.SceneNode
                namespace SceneNodeFFI
                {
                    int Create(GTCore::Script &script)
                    {
                        script.Push(new SceneNode());
                        return 1;
                    }

                    int Delete(GTCore::Script &script)
                    {
                        delete reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        return 0;
                    }


                    int GetName(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            script.Push(sceneNode->GetName());
                        }
                        else
                        {
                            script.PushNil();
                        }
                        
                        return 1;
                    }

                    int SetName(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetName(script.ToString(2));
                        }

                        return 0;
                    }


                    int AddComponent(GTCore::Script &script)
                    {
                        auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        auto componentName = script.ToString(2);

                        if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
                        {
                            PushComponent(script, "ModelComponent", sceneNode->AddComponent<ModelComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
                        {
                            PushComponent(script, "PointLightComponent", sceneNode->AddComponent<PointLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
                        {
                            PushComponent(script, "EditorMetadataComponent", sceneNode->AddComponent<EditorMetadataComponent>());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }

                    int RemoveComponent(GTCore::Script &script)
                    {
                        reinterpret_cast<SceneNode*>(script.ToPointer(1))->RemoveComponentByName(script.ToString(2));
                        return 0;
                    }

                    int GetComponent(GTCore::Script &script)
                    {
                        auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        auto componentName = script.ToString(2);

                        if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
                        {
                            PushComponent(script, "ModelComponent", sceneNode->GetComponent<ModelComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
                        {
                            PushComponent(script, "PointLightComponent", sceneNode->GetComponent<PointLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
                        {
                            PushComponent(script, "EditorMetadataComponent", sceneNode->GetComponent<EditorMetadataComponent>());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }


                    int Refresh(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->Refresh();
                        }

                        return 0;
                    }


                    int GetPosition(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto &position = sceneNode->GetPosition();

                            script.Push(position.x);
                            script.Push(position.y);
                            script.Push(position.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }

                    int SetPosition(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetPosition(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetRotationXYZ(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto &orientation = sceneNode->GetOrientation();

                            script.Push(glm::pitch(orientation));
                            script.Push(glm::yaw(orientation));
                            script.Push(glm::roll(orientation));
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }

                    int SetRotationXYZ(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            // Order: pitch/yaw/roll
                            sceneNode->SetOrientation(glm::quat(glm::vec3(glm::radians(script.ToFloat(2)), glm::radians(script.ToFloat(3)), glm::radians(script.ToFloat(4)))));
                        }

                        return 0;
                    }

                    int GetScale(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto &scale = sceneNode->GetScale();

                            script.Push(scale.x);
                            script.Push(scale.y);
                            script.Push(scale.z);
                        }
                        else
                        {
                            script.Push(1.0f);
                            script.Push(1.0f);
                            script.Push(1.0f);
                        }

                        return 3;
                    }

                    int SetScale(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetScale(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetAttachedComponentIDs(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            script.PushNewTable();
                            

                            GTCore::Vector<GTCore::String> componentNames;
                            sceneNode->GetAttachedComponentNames(componentNames);

                            for (size_t i = 0; i < componentNames.count; ++i)
                            {
                                script.SetTableValue(-1, i + 1, componentNames[i].c_str());
                            }
                        }

                        return 1;
                    }


                    void PushComponent(GTCore::Script &script, const char* componentClassName, Component* component)
                    {
                        if (component != nullptr)
                        {
                            // GTEngine.<componentClassName>.Create(component);

                            script.GetGlobal("GTEngine");
                            assert(script.IsTable(-1));
                            {
                                script.Push(componentClassName);
                                script.GetTableValue(-2);
                                assert(script.IsTable(-1));
                                {
                                    script.Push("Create");
                                    script.GetTableValue(-2);
                                    assert(script.IsFunction(-1));
                                    {
                                        script.Push(component);
                                        script.Call(1, 1);

                                        // GTEngine and GTEngine.<componentClassName> need to be popped, so we need to insert the return value into the location that will eventually place
                                        // it at -1 after popping those tables.
                                        script.InsertIntoStack(-3);
                                    }
                                }
                                script.Pop(1);
                            }
                            script.Pop(1);
                        }
                        else
                        {
                            script.PushNil();
                        }
                    }
                }


                //////////////////////////////////////////////////
                // GTEngine.System.Scene
                namespace SceneFFI
                {
                    int AddSceneNode(GTCore::Script &script)
                    {
                        auto scene     = reinterpret_cast<Scene*    >(script.ToPointer(1));
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (scene != nullptr && sceneNode != nullptr)
                        {
                            scene->AddSceneNode(*sceneNode);
                        }

                        return 0;
                    }

                    int RemoveSceneNode(GTCore::Script &script)
                    {
                        auto scene     = reinterpret_cast<Scene*    >(script.ToPointer(1));
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (scene != nullptr && sceneNode != nullptr)
                        {
                            scene->RemoveSceneNode(*sceneNode);
                        }

                        return 0;
                    }
                }


                //////////////////////////////////////////////////
                // GTEngine.System.ModelComponent
                namespace ModelComponentFFI
                {
                    int SetModel(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
                        auto filePath  = script.ToString(2);

                        if (component != nullptr && filePath != nullptr)
                        {
                            component->SetModel(filePath);
                        }

                        return 0;
                    }
                }


                //////////////////////////////////////////////////
                // GTEngine.System.PointLightComponent
                namespace PointLightComponentFFI
                {
                    int SetColour(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->SetColour(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetColour(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            auto &colour = component->GetColour();

                            script.Push(colour.x);
                            script.Push(colour.y);
                            script.Push(colour.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }

                    int EnableShadowCasting(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->EnableShadowCasting();
                        }

                        return 0;
                    }

                    int DisableShadowCasting(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->DisableShadowCasting();
                        }

                        return 0;
                    }

                    int IsShadowCastingEnabled(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->IsShadowCastingEnabled());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int GetConstantAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetConstantAttenuation());
                        }
                        else
                        {
                            script.Push(1.0f);
                        }

                        return 1;
                    }

                    int GetLinearAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetLinearAttenuation());
                        }
                        else
                        {
                            script.Push(0.0f);
                        }

                        return 1;
                    }

                    int GetQuadraticAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetQuadraticAttenuation());
                        }
                        else
                        {
                            script.Push(0.0666f);
                        }

                        return 1;
                    }

                    int SetAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float constant  = script.ToFloat(2);
                            float linear    = script.ToFloat(3);
                            float quadratic = script.ToFloat(4);
                            component->SetAttenuation(constant, linear, quadratic);
                        }

                        return 0;
                    }
                }



                //////////////////////////////////////////////////
                // GTEngine.System.EditorMetadataComponent
                namespace EditorMetadataComponentFFI
                {
                    int ShowSprite(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            if (script.IsString(2))
                            {
                                glm::vec3 colour(1.0f, 1.0f, 1.0f);
                                if (script.IsTable(3))
                                {
                                    script.Push("r");
                                    script.GetTableValue(3);
                                    {
                                        colour.x = script.ToFloat(-1);
                                    }
                                    script.Pop(1);

                                    script.Push("g");
                                    script.GetTableValue(3);
                                    {
                                        colour.y = script.ToFloat(-1);
                                    }
                                    script.Pop(1);

                                    script.Push("b");
                                    script.GetTableValue(3);
                                    {
                                        colour.z = script.ToFloat(-1);
                                    }
                                    script.Pop(1);
                                }

                                component->ShowSprite(script.ToString(2), colour);
                            }
                        }

                        return 0;
                    }


                    int HideSprite(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->HideSprite();
                        }

                        return 0;
                    }
                }
            }



            //////////////////////////////////////////////////
            // GTEngine.Rendering
            namespace RendererFFI
            {
                int EnableVSync(GTCore::Script &)
                {
                    Renderer::SetSwapInterval(1);
                    return 0;
                }

                int DisableVSync(GTCore::Script &)
                {
                    Renderer::SetSwapInterval(0);
                    return 0;
                }
            }

            //////////////////////////////////////////////////
            // GTEngine.Audio
            namespace AudioFFI
            {
                int Play(GTCore::Script &script)
                {
                    AudioComposer::Play(script.ToString(1));
                    return 0;
                }
            }
        }
    }
}
