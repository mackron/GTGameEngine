// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTCore/Dictionary.hpp>
#include <GTCore/List.hpp>
#include <GTCore/Vector.hpp>
#include <GTCore/Path.hpp>

namespace GTEngine
{
    /// The list of loaded material definitions, indexed by their file path, relative to the data directory.
    static GTCore::Dictionary<MaterialDefinition*> MaterialDefinitions;

    /// The list of loaded materials.
    static GTCore::List<Material*> LoadedMaterials;


    /// The list of event handlers.
    static GTCore::Vector<MaterialLibrary::EventHandler*> EventHandlers;

    /// Helper function for calling the OnCreateMaterial() event.
    void MaterialLibrary_OnCreateMaterial(Material &material)
    {
        for (size_t i = 0; i < EventHandlers.count; ++i)
        {
            EventHandlers[i]->OnCreateMaterial(material);
        }
    }

    /// Helper function for calling the OnDeleteMaterial() event.
    void MaterialLibrary_OnDeleteMaterial(Material &material)
    {
        for (size_t i = 0; i < EventHandlers.count; ++i)
        {
            EventHandlers[i]->OnDeleteMaterial(material);
        }
    }

    /// Helper function for calling the OnCreateMaterialDefinition() event.
    void MaterialLibrary_OnCreateMaterialDefinition(MaterialDefinition &definition)
    {
        for (size_t i = 0; i < EventHandlers.count; ++i)
        {
            EventHandlers[i]->OnCreateMaterialDefinition(definition);
        }
    }

    /// Helper function for calling the OnDeleteMaterialDefinition() event.
    void MaterialLibrary_OnDeleteMaterialDefinition(MaterialDefinition &definition)
    {
        for (size_t i = 0; i < EventHandlers.count; ++i)
        {
            EventHandlers[i]->OnDeleteMaterialDefinition(definition);
        }
    }

    /// Helper function for calling the OnReloadMaterialDefinition() event.
    void MaterialLibrary_OnReloadMaterialDefinition(MaterialDefinition &definition)
    {
        for (size_t i = 0; i < EventHandlers.count; ++i)
        {
            EventHandlers[i]->OnReloadMaterialDefinition(definition);
        }
    }



    bool MaterialLibrary::Startup()
    {
        return true;
    }

    void MaterialLibrary::Shutdown()
    {
        while (LoadedMaterials.root != nullptr)
        {
            auto material = LoadedMaterials.root->value;
            assert(material != nullptr);
            {
                MaterialLibrary_OnDeleteMaterial(*material);
                delete material;
            }

            LoadedMaterials.RemoveRoot();
        }

        for (size_t i = 0; i < MaterialDefinitions.count; ++i)
        {
            auto definition = MaterialDefinitions.buffer[i]->value;
            assert(definition != nullptr);
            {
                MaterialLibrary_OnDeleteMaterialDefinition(*definition);
                delete definition;
            }
        }
    }

    
    void MaterialLibrary::AttachEventHandler(EventHandler &eventHandler)
    {
        if (!EventHandlers.Exists(&eventHandler))
        {
            EventHandlers.PushBack(&eventHandler);
        }
    }

    void MaterialLibrary::RemoveEventHandler(EventHandler &eventHandler)
    {
        EventHandlers.RemoveFirstOccuranceOf(&eventHandler);
    }


    Material* MaterialLibrary::Create(const char* fileName, const char* makeRelativeTo)
    {
        GTCore::String relativePath(fileName);

        if (GTCore::Path::IsAbsolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                relativePath = GTCore::IO::ToRelativePath(fileName, makeRelativeTo);
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            MaterialDefinition* definition = nullptr;

            auto iMaterialDefinition = MaterialDefinitions.Find(absolutePath.c_str());
            if (iMaterialDefinition != nullptr)
            {
                // Definition is already loaded.
                definition = iMaterialDefinition->value;
            }
            else
            {
                // Definition is not yet loaded.
                definition = new MaterialDefinition;
                if (definition->LoadFromFile(absolutePath.c_str(), relativePath.c_str()))
                {
                    MaterialDefinitions.Add(absolutePath.c_str(), definition);
                    MaterialLibrary_OnCreateMaterialDefinition(*definition);
                }
                else
                {
                    delete definition;
                    definition = nullptr;
                }
            }

            // With the definition retrieved, we now need to move on to create the actual material.
            if (definition != nullptr)
            {
                auto material = new Material(*definition);
                LoadedMaterials.Append(material);

                MaterialLibrary_OnCreateMaterial(*material);

                return material;
            }
        }

        return nullptr;
    }

    Material* MaterialLibrary::CreateCopy(const Material &source)
    {
        auto newMaterial = new Material(source.GetDefinition());
        LoadedMaterials.Append(newMaterial);

        MaterialLibrary_OnCreateMaterial(*newMaterial);

        return newMaterial;
    }


    void MaterialLibrary::Delete(Material* material)
    {
        if (material != nullptr)
        {
            MaterialLibrary_OnDeleteMaterial(*material);

            LoadedMaterials.Remove(LoadedMaterials.Find(material));

            delete material;
        }
    }


    bool MaterialLibrary::Reload(const char* fileName)
    {
        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            auto iDefinition = MaterialDefinitions.Find(absolutePath.c_str());
            if (iDefinition != nullptr)
            {
                auto definition = iDefinition->value;
                assert(definition != nullptr);
                {
                    // We want to grab a copy of the default parameters before reloading so that we can determine which ones to remove from linked materials.
                    ShaderParameterCache oldDefaultParameters(definition->defaultParams);



                    bool result = definition->LoadFromFile(definition->absolutePath.c_str(), definition->relativePath.c_str());
                    if (result)
                    {
                        // Materials linked to the definition need to have their parameters set to their new defaults. Any default parameters that have been
                        // removed need to be removed from the linked materials. If the default parameter is still present and has not been modified by the
                        // material, it will be changed to the new default.
                        for (auto iMaterial = LoadedMaterials.root; iMaterial != nullptr; iMaterial = iMaterial->next)
                        {
                            auto material = iMaterial->value;
                            assert(material != nullptr);
                            {
                                if (&material->GetDefinition() == definition)
                                {
                                    for (size_t iParameter = 0; iParameter < oldDefaultParameters.GetCount(); ++iParameter)
                                    {
                                        auto parameterName = oldDefaultParameters.GetNameByIndex(iParameter);
                                        assert(parameterName != nullptr);
                                        {
                                            auto oldParameter = oldDefaultParameters.GetByIndex(iParameter);
                                            auto newParameter = definition->defaultParams.Get(parameterName);

                                            if (newParameter == nullptr)
                                            {
                                                // The parameter doesn't exist anymore. It needs to be removed from the material.
                                                material->UnsetParameter(parameterName);
                                            }
                                            else
                                            {
                                                // The parameter still exists, but we need to check if it needs replacing.
                                                auto iMaterialParameter = material->GetParameters().Find(parameterName);
                                                if (iMaterialParameter != nullptr)
                                                {
                                                    if (CompareShaderParameters(oldParameter, iMaterialParameter->value))
                                                    {
                                                        // We'll replace the parameter.
                                                        material->SetParameter(parameterName, newParameter);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        

                        // We need to let everything know that the material has been reloaded.
                        MaterialLibrary_OnReloadMaterialDefinition(*definition);

                        return true;
                    }
                }
            }
        }

        return false;
    }
}
