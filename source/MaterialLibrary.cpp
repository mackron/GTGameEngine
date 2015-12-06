// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/MaterialLibrary.hpp>
#include <GTGE/GTEngine.hpp>
#include <GTGE/Core/Dictionary.hpp>
#include <GTGE/Core/List.hpp>
#include <GTGE/Core/Vector.hpp>
#include <easy_path/easy_path.h>
#include <utility>

namespace GT
{
    MaterialLibrary::MaterialLibrary(Context &context)
        : m_context(context)
    {
    }

    MaterialLibrary::~MaterialLibrary()
    {
    }



    bool MaterialLibrary::Startup()
    {
        return true;
    }

    void MaterialLibrary::Shutdown()
    {
        while (m_loadedMaterials.root != nullptr)
        {
            auto material = m_loadedMaterials.root->value;
            assert(material != nullptr);
            {
                this->OnDeleteMaterial(*material);
                delete material;
            }

            m_loadedMaterials.RemoveRoot();
        }

        for (size_t i = 0; i < m_materialDefinitions.count; ++i)
        {
            auto &reference = m_materialDefinitions.buffer[i]->value;
            assert(reference.definition != nullptr);
            {
                this->OnDeleteMaterialDefinition(*reference.definition);
                delete reference.definition;
            }
        }
    }

    
    void MaterialLibrary::AttachEventHandler(EventHandler &eventHandler)
    {
        if (!m_eventHandlers.Exists(&eventHandler))
        {
            m_eventHandlers.PushBack(&eventHandler);
        }
    }

    void MaterialLibrary::RemoveEventHandler(EventHandler &eventHandler)
    {
        m_eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
    }


    Material* MaterialLibrary::Create(const char* fileName, const char* makeRelativeTo)
    {
        char relativePath[EASYVFS_MAX_PATH];
        strcpy_s(relativePath, sizeof(relativePath), fileName);

        if (easypath_is_absolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                easypath_to_relative(fileName, makeRelativeTo, relativePath, sizeof(relativePath));
            }
            else
            {
                g_Context->LogErrorf("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            MaterialDefinition* definition = nullptr;

            auto iMaterialDefinition = m_materialDefinitions.Find(absolutePath);
            if (iMaterialDefinition != nullptr)
            {
                // Definition is already loaded. All we do it increment the reference counter.
                iMaterialDefinition->value.referenceCount += 1;

                definition = iMaterialDefinition->value.definition;
            }
            else
            {
                // Definition is not yet loaded.
                definition = new MaterialDefinition(m_context);
                if (definition->LoadFromFile(absolutePath, relativePath))
                {
                    m_materialDefinitions.Add(absolutePath, MaterialDefinitionReference(definition, 1));
                    this->OnCreateMaterialDefinition(*definition);
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
                m_loadedMaterials.Append(material);

                this->OnCreateMaterial(*material);

                return material;
            }
        }

        return nullptr;
    }

    Material* MaterialLibrary::CreateCopy(const Material &source)
    {
        auto newMaterial = new Material(source.GetDefinition());
        m_loadedMaterials.Append(newMaterial);

        this->OnCreateMaterial(*newMaterial);


        // The reference counter needs to be incremented.
        auto iMaterialDefinition = m_materialDefinitions.Find(source.GetDefinition().absolutePath.c_str());
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
            this->OnDeleteMaterial(*material);

            m_loadedMaterials.Remove(m_loadedMaterials.Find(material));


            // The reference counter needs to be decremented. If this is the last reference to the material we'll delete it.
            String absolutePath(material->GetDefinition().absolutePath);

            auto iMaterialDefinition = m_materialDefinitions.Find(absolutePath.c_str());
            if (iMaterialDefinition != nullptr)
            {
                assert(iMaterialDefinition->value.referenceCount >= 1);
                {
                    iMaterialDefinition->value.referenceCount -= 1;

                    if (iMaterialDefinition->value.referenceCount == 0)
                    {
                        delete iMaterialDefinition->value.definition;
                        m_materialDefinitions.RemoveByKey(absolutePath.c_str());
                    }
                }
            }


            delete material;
        }
    }


    bool MaterialLibrary::Reload(const char* fileName)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            auto iDefinition = m_materialDefinitions.Find(absolutePath);
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
                        for (auto iMaterial = m_loadedMaterials.root; iMaterial != nullptr; iMaterial = iMaterial->next)
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
                        this->OnReloadMaterialDefinition(*reference.definition);

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

        auto iMaterialDefinition = m_materialDefinitions.Find("@NavigationMesh");
        if (iMaterialDefinition == nullptr)
        {
            definition = new MaterialDefinition(m_context);
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
                    "    uniform vec3 ColorF;"
                    "    vec3 Emissive()"
                    "    {"
                    "        return ColorF;"
                    "    }"
                    "</channel>"

                    "<blending>"
                    "    <equation>Add</equation>"
                    "    <sourcefactor>SourceAlpha</sourcefactor>"
                    "    <destinationfactor>OneMinusSourceAlpha</destinationfactor>"
                    "</blending>"

                    "<defaultproperties>"
                    "    <float  name='Alpha' >0.5</float>"
                    "    <float3 name='ColorF'>0.75 0.75 1.0</float3>"
                    "</defaultproperties>"
                "</material>"
            );

            m_materialDefinitions.Add("@NavigationMesh", MaterialDefinitionReference(definition, 1));
        }
        else
        {
            iMaterialDefinition->value.referenceCount += 1;
            definition = iMaterialDefinition->value.definition;
        }


        assert(definition != nullptr);
        {
            auto material = new Material(*definition);
            m_loadedMaterials.Append(material);

            this->OnCreateMaterial(*material);

            return material;
        }
    }





    void MaterialLibrary::OnCreateMaterial(Material &material)
    {
        for (size_t i = 0; i < m_eventHandlers.count; ++i)
        {
            m_eventHandlers[i]->OnCreateMaterial(material);
        }
    }

    void MaterialLibrary::OnDeleteMaterial(Material &material)
    {
        for (size_t i = 0; i < m_eventHandlers.count; ++i)
        {
            m_eventHandlers[i]->OnDeleteMaterial(material);
        }
    }

    void MaterialLibrary::OnCreateMaterialDefinition(MaterialDefinition &definition)
    {
        for (size_t i = 0; i < m_eventHandlers.count; ++i)
        {
            m_eventHandlers[i]->OnCreateMaterialDefinition(definition);
        }
    }

    void MaterialLibrary::OnDeleteMaterialDefinition(MaterialDefinition &definition)
    {
        for (size_t i = 0; i < m_eventHandlers.count; ++i)
        {
            m_eventHandlers[i]->OnDeleteMaterialDefinition(definition);
        }
    }

    void MaterialLibrary::OnReloadMaterialDefinition(MaterialDefinition &definition)
    {
        for (size_t i = 0; i < m_eventHandlers.count; ++i)
        {
            m_eventHandlers[i]->OnReloadMaterialDefinition(definition);
        }
    }
}
