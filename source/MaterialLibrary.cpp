// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/MaterialLibrary.hpp>
#include <GTCore/Dictionary.hpp>
#include <GTCore/List.hpp>
#include <GTCore/Vector.hpp>

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




    bool MaterialLibrary::Startup()
    {
        return true;
    }

    void MaterialLibrary::Shutdown()
    {
        for (size_t i = 0; i < MaterialDefinitions.count; ++i)
        {
            delete MaterialDefinitions.buffer[i]->value;
        }

        while (LoadedMaterials.root != nullptr)
        {
            delete LoadedMaterials.root->value;
            LoadedMaterials.RemoveRoot();
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


    Material* MaterialLibrary::Create(const char* fileName)
    {
        // We create materials from material definitions. Thus, we'll need one first. If one already exists, we reuse that. Otherwise, we create a new one.
        MaterialDefinition* definition = nullptr;
        
        auto iMaterialDefinition = MaterialDefinitions.Find(fileName);
        if (iMaterialDefinition != nullptr)
        {
            definition = iMaterialDefinition->value;
        }
        else
        {
            definition = new MaterialDefinition;
            if (definition->LoadFromFile(fileName))
            {
                MaterialDefinitions.Add(fileName, definition);
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
