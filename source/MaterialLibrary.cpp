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

            auto iMaterialDefinition = MaterialDefinitions.Find(fileName);
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
                    MaterialDefinitions.Add(fileName, definition);
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
}
