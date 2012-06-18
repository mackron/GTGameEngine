
#include <GTEngine/MaterialLibrary.hpp>
#include <GTCore/Dictionary.hpp>
#include <GTCore/List.hpp>

namespace GTEngine
{
    /// The list of loaded material definitions, indexed by their file path, relative to the data directory.
    static GTCore::Dictionary<MaterialDefinition*> MaterialDefinitions;

    /// The list of loaded materials.
    static GTCore::List<Material*> LoadedMaterials;


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

        // With the definition retrieved, we now need to move on to
        if (definition != nullptr)
        {
            auto material = new Material(*definition);
            LoadedMaterials.Append(material);

            return material;
        }

        return nullptr;
    }

    Material* MaterialLibrary::CreateCopy(const Material &source)
    {
        auto newMaterial = new Material(source.GetDefinition());
        LoadedMaterials.Append(newMaterial);

        return newMaterial;
    }


    void MaterialLibrary::Delete(Material* material)
    {
        LoadedMaterials.Remove(LoadedMaterials.Find(material));

        delete material;
    }
}
