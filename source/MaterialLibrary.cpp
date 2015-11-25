// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTLib/Dictionary.hpp>
#include <GTLib/List.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/Path.hpp>
#include <easy_path/easy_path.h>
#include <utility>

namespace GTEngine
{
    ///////////////////////////////////////
    // Globals

    struct MaterialDefinitionReference
    {
        MaterialDefinition* definition;
        size_t              referenceCount;


        MaterialDefinitionReference(MaterialDefinition* definitionIn, size_t referenceCountIn)
            : definition(definitionIn), referenceCount(referenceCountIn)
        {
        }

        MaterialDefinitionReference(const MaterialDefinitionReference &other)
            : definition(other.definition), referenceCount(other.referenceCount)
        {
        }


        MaterialDefinitionReference & operator=(const MaterialDefinitionReference &other)
        {
            this->definition     = other.definition;
            this->referenceCount = other.referenceCount;

            return *this;
        }
    };


    /// The list of loaded material definitions, indexed by their absolute path.
    static GTLib::Dictionary<MaterialDefinitionReference> MaterialDefinitions;

    /// The list of loaded materials.
    static GTLib::List<Material*> LoadedMaterials;



    /// The list of event handlers.
    static GTLib::Vector<MaterialLibrary::EventHandler*> EventHandlers;

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
            auto &reference = MaterialDefinitions.buffer[i]->value;
            assert(reference.definition != nullptr);
            {
                MaterialLibrary_OnDeleteMaterialDefinition(*reference.definition);
                delete reference.definition;
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
        char relativePath[EASYVFS_MAX_PATH];
        strcpy_s(relativePath, sizeof(relativePath), fileName);

        if (GTLib::Path::IsAbsolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                easypath_to_relative(fileName, makeRelativeTo, relativePath, sizeof(relativePath));
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_EngineContext->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            MaterialDefinition* definition = nullptr;

            auto iMaterialDefinition = MaterialDefinitions.Find(absolutePath);
            if (iMaterialDefinition != nullptr)
            {
                // Definition is already loaded. All we do it increment the reference counter.
                iMaterialDefinition->value.referenceCount += 1;

                definition = iMaterialDefinition->value.definition;
            }
            else
            {
                // Definition is not yet loaded.
                definition = new MaterialDefinition;
                if (definition->LoadFromFile(absolutePath, relativePath))
                {
                    MaterialDefinitions.Add(absolutePath, MaterialDefinitionReference(definition, 1));
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


        // The reference counter needs to be incremented.
        auto iMaterialDefinition = MaterialDefinitions.Find(source.GetDefinition().absolutePath.c_str());
        if (iMaterialDefinition != nullptr)
        {
            iMaterialDefinition->value.referenceCount += 1;
        }


        return newMaterial;
    }


    void MaterialLibrary::Delete(Material* material)
    {
        if (material != nullptr)
        {
            MaterialLibrary_OnDeleteMaterial(*material);

            LoadedMaterials.Remove(LoadedMaterials.Find(material));


            // The reference counter needs to be decremented. If this is the last reference to the material we'll delete it.
            GTLib::String absolutePath(material->GetDefinition().absolutePath);

            auto iMaterialDefinition = MaterialDefinitions.Find(absolutePath.c_str());
            if (iMaterialDefinition != nullptr)
            {
                assert(iMaterialDefinition->value.referenceCount >= 1);
                {
                    iMaterialDefinition->value.referenceCount -= 1;

                    if (iMaterialDefinition->value.referenceCount == 0)
                    {
                        delete iMaterialDefinition->value.definition;
                        MaterialDefinitions.RemoveByKey(absolutePath.c_str());
                    }
                }
            }


            delete material;
        }
    }


    bool MaterialLibrary::Reload(const char* fileName)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_EngineContext->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            auto iDefinition = MaterialDefinitions.Find(absolutePath);
            if (iDefinition != nullptr)
            {
                auto &reference = iDefinition->value;
                assert(reference.definition != nullptr);
                {
                    // We want to grab a copy of the default parameters before reloading so that we can determine which ones to remove from linked materials.
                    ShaderParameterCache oldDefaultParameters(reference.definition->defaultParams);


                    bool result = reference.definition->LoadFromFile(reference.definition->absolutePath.c_str(), reference.definition->relativePath.c_str());
                    if (result)
                    {
                        // Now we just iterate over material that uses the definition in question and update the default parameters.
                        for (auto iMaterial = LoadedMaterials.root; iMaterial != nullptr; iMaterial = iMaterial->next)
                        {
                            auto material = iMaterial->value;
                            assert(material != nullptr);
                            {
                                if (&material->GetDefinition() == reference.definition)
                                {
                                    material->ResetDefaultParameters(oldDefaultParameters);
                                }
                            }
                        }
                        

                        // We need to let everything know that the material has been reloaded.
                        MaterialLibrary_OnReloadMaterialDefinition(*reference.definition);

                        return true;
                    }
                }
            }
        }

        return false;
    }



    Material* MaterialLibrary::CreateNavigationMeshMaterial()
    {
        MaterialDefinition* definition = nullptr;

        auto iMaterialDefinition = MaterialDefinitions.Find("@NavigationMesh");
        if (iMaterialDefinition == nullptr)
        {
            definition = new MaterialDefinition;
            definition->LoadFromXML
            (
                "<material>"
                    "<channel name='diffuse'>"
                    "    uniform float Alpha;"
                    "    vec4 Diffuse()"
                    "    {"
                    "        return vec4(0.0, 0.0, 0.0, Alpha);"
                    "    }"
                    "</channel>"

                    "<channel name='emissive'>"
                    "    uniform vec3 Colour;"
                    "    vec3 Emissive()"
                    "    {"
                    "        return Colour;"
                    "    }"
                    "</channel>"

                    "<blending>"
                    "    <equation>Add</equation>"
                    "    <sourcefactor>SourceAlpha</sourcefactor>"
                    "    <destinationfactor>OneMinusSourceAlpha</destinationfactor>"
                    "</blending>"

                    "<defaultproperties>"
                    "    <float  name='Alpha' >0.5</float>"
                    "    <float3 name='Colour'>0.75 0.75 1.0</float3>"
                    "</defaultproperties>"
                "</material>"
            );

            MaterialDefinitions.Add("@NavigationMesh", MaterialDefinitionReference(definition, 1));
        }
        else
        {
            iMaterialDefinition->value.referenceCount += 1;
            definition = iMaterialDefinition->value.definition;
        }


        assert(definition != nullptr);
        {
            auto material = new Material(*definition);
            LoadedMaterials.Append(material);

            MaterialLibrary_OnCreateMaterial(*material);

            return material;
        }
    }
}
