
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
                "GTEngine.System.ModelComponent = {};"
                "GTEngine.System.SceneNode      = {};"
                "GTEngine.System.Scene          = {};"

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
                "    ModelComponent            = 'Model';"
                "    CameraComponent           = 'Camera';"
                "    PointLightComponent       = 'PointLight';"
                "    SpotLightComponent        = 'SpotLight';"
                "    DirectionalLightComponent = 'DirectionalLight';"
                "    AmbientLightComponent     = 'AmbientLight';"
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

                "function GTEngine.ModelComponent:EnableWireframe()"
                "    GTEngine.System.ModelComponent.EnableWireframe(self._internalPtr);"
                "end;"

                "function GTEngine.ModelComponent:DisableWireframe()"
                "    GTEngine.System.ModelComponent.DisableWireframe(self._internalPtr);"
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
                        script.SetTableFunction(-1, "Create",          FFI::SystemFFI::SceneNodeFFI::Create);
                        script.SetTableFunction(-1, "Delete",          FFI::SystemFFI::SceneNodeFFI::Delete);

                        script.SetTableFunction(-1, "GetName",         FFI::SystemFFI::SceneNodeFFI::GetName);
                        script.SetTableFunction(-1, "SetName",         FFI::SystemFFI::SceneNodeFFI::SetName);

                        script.SetTableFunction(-1, "AddComponent",    FFI::SystemFFI::SceneNodeFFI::AddComponent);
                        script.SetTableFunction(-1, "RemoveComponent", FFI::SystemFFI::SceneNodeFFI::RemoveComponent);
                        script.SetTableFunction(-1, "GetComponent",    FFI::SystemFFI::SceneNodeFFI::GetComponent);

                        script.SetTableFunction(-1, "Refresh",         FFI::SystemFFI::SceneNodeFFI::Refresh);
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
