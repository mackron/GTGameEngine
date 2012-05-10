
#include <GTEngine/MaterialLibrary.hpp>
#include <GTCore/Dictionary.hpp>

namespace GTEngine
{
    /// The list of loaded material definitions, indexed by their file path, relative to the data directory.
    static GTCore::Dictionary<MaterialDefinition*> MaterialDefinitions;


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
    }



    Material* MaterialLibrary::Create(const char* fileName)
    {
        Material* newMaterial = nullptr;

        auto iMaterialDef = MaterialDefinitions.Find(fileName);
        if (iMaterialDef == nullptr)
        {
            // If we get here, it means we need to load the material file and create a new material definition.
            auto newMaterialDef = new MaterialDefinition;
            if (newMaterialDef->LoadFromFile(fileName))
            {
                MaterialDefinitions.Add(fileName, newMaterialDef);
                
                newMaterial = new Material(*newMaterialDef);
            }
            else
            {
                // The material file could not be loaded. Return null in this case.
                delete newMaterialDef;
                return nullptr;
            }
        }
        else
        {
            // If we get here, it means the material definition already exists, and we just use it directly without having to load the material file.
            assert(iMaterialDef->value);

            newMaterial = new Material(*iMaterialDef->value);
        }

        return newMaterial;
    }

    Material* MaterialLibrary::CreateCopy(const Material &source)
    {
        return new Material(source.GetDefinition());
    }


    void MaterialLibrary::Delete(Material* material)
    {
        delete material;
    }
}
