// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/ModelLibrary.hpp>
#include <GTGE/MaterialLibrary.hpp>
#include <GTGE/VertexArrayLibrary.hpp>
#include <GTGE/GTEngine.hpp>
#include <easy_path/easy_path.h>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <assimp/Importer.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


// Startup/Shutdown
namespace GT
{
    /// Constructor.
    ModelLibrary::ModelLibrary(Context &context)
        : m_context(context), m_loadedDefinitions(), m_instantiatedModels()
    {
    }

    ModelLibrary::~ModelLibrary()
    {
    }


    ////////////////////////////////////////////////
    // Startup/Shutdown

    bool ModelLibrary::Startup()
    {
        return true;
    }

    void ModelLibrary::Shutdown()
    {
        // Instantiated models need to be deleted.
        for (size_t i = 0; i < m_instantiatedModels.count; ++i)
        {
            delete m_instantiatedModels[i];
        }
        m_instantiatedModels.Clear();


        // Definitions now need to be deleted.
        for (size_t i = 0; i < m_loadedDefinitions.count; ++i)
        {
            delete m_loadedDefinitions.buffer[i]->value.definition;
        }
        m_loadedDefinitions.Clear();
    }


    ////////////////////////////////////////////////
    // Create/Delete

    Model* ModelLibrary::Create(const char* fileName, const char* makeRelativeTo)
    {
        char relativePath[DRVFS_MAX_PATH];
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


        // We will first find an existing model definition. If we don't find it, we create one and the load into it.
        String absolutePath;
        if (this->FindAbsolutePath(fileName, absolutePath))
        {
            ModelDefinition* definition = nullptr;

            auto iDefinition = m_loadedDefinitions.Find(absolutePath.c_str());
            if (iDefinition != nullptr)
            {
                // Definition is already loaded. All we do it increment the reference counter.
                iDefinition->value.referenceCount += 1;

                definition = iDefinition->value.definition;
            }
            else
            {
                // Definition is not yet loaded and needs to be loaded now.
                definition = new ModelDefinition(m_context);

                bool needsSerialize;
                if (definition->LoadFromFile(absolutePath.c_str(), relativePath, needsSerialize))
                {
                    m_loadedDefinitions.Add(definition->absolutePath.c_str(), ModelDefinitionReference(definition, 1));

                    if (needsSerialize)
                    {
                        this->WriteToFile(*definition);
                    }
                }
                else
                {
                    delete definition;
                    definition = nullptr;
                }
            }


            // Now all we do is create the model from the definition.
            if (definition != nullptr)
            {
                return this->CreateFromDefinition(*definition);
            }
        }

        return nullptr;
    }

    Model* ModelLibrary::CreateFromDefinition(const ModelDefinition &definition)
    {
        auto model = new Model(definition);
        m_instantiatedModels.PushBack(model);

        return model;
    }


    static int ConvexHullCount = 0;
    Model* ModelLibrary::CreateFromConvexHull(const ConvexHull &convexHull)
    {
        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        IO::snprintf(name, 128, "convexhull(%d)", ConvexHullCount++);

        // We create the model from a primitive. To do this we need a non-const vertex array.
        VertexArray* va = nullptr;

        bool exists = m_loadedDefinitions.Find(name) != nullptr;
        if (!exists)
        {
            va = m_context.GetVertexArrayLibrary().CreateFromConvexHull(convexHull);
        }

        return this->CreateFromPrimitive(name, va);
    }


    void ModelLibrary::Delete(Model* model)
    {
        if (model != nullptr)
        {
            m_instantiatedModels.RemoveFirstOccuranceOf(model);


            // The reference counter needs to be decremented. If this is the last reference to the model we'll delete it.
            String absolutePath(model->GetDefinition().GetAbsolutePath());

            auto iDefinition = m_loadedDefinitions.Find(absolutePath.c_str());
            if (iDefinition != nullptr)
            {
                assert(iDefinition->value.referenceCount >= 1);
                {
                    iDefinition->value.referenceCount -= 1;

                    if (iDefinition->value.referenceCount == 0)
                    {
                        delete iDefinition->value.definition;
                        m_loadedDefinitions.RemoveByKey(absolutePath.c_str());
                    }
                }
            }


            delete model;
        }
    }


    bool ModelLibrary::Reload(const char* fileNameIn)
    {
        char fileName[DRVFS_MAX_PATH];
        strcpy_s(fileName, sizeof(fileName), fileNameIn);

        if (easypath_extension_equal(fileName, "gtmodel"))
        {
            easypath_remove_extension(fileName);
        }


        // We need to find the definition that we're updating.
        auto definition = this->FindDefinition(fileName);
        if (definition != nullptr)
        {
            bool needsSerialize;
            if (definition->LoadFromFile(definition->absolutePath.c_str(), definition->relativePath.c_str(), needsSerialize))
            {
                if (needsSerialize)
                {
                    this->WriteToFile(*definition);
                }
            }

            // Every model with this definition needs to know that it has changed.
            for (size_t iModel = 0; iModel < m_instantiatedModels.count; ++iModel)
            {
                auto model = m_instantiatedModels[iModel];
                assert(model != nullptr);
                {
                    if (&model->GetDefinition() == definition)
                    {
                        model->OnDefinitionChanged();
                    }
                }
            }


            return true;
        }

        return false;
    }


    ////////////////////////////////////////////////
    // Writing

    bool ModelLibrary::WriteToFile(const ModelDefinition &definition, const char* fileNameIn)
    {
        // We have a model, so now we need to check that we can open the file.
        String fileName(fileNameIn);
        if (!easypath_extension_equal(fileNameIn, "gtmodel"))
        {
            fileName += ".gtmodel";
        }

        drvfs_file* pFile = drvfs_open(g_Context->GetVFS(), fileName.c_str(), DRVFS_WRITE, 0);
        if (pFile != nullptr)
        {
            FileSerializer serializer(pFile);
            definition.Serialize(serializer);

            drvfs_close(pFile);
            return true;
        }

        return false;
    }

    bool ModelLibrary::WriteToFile(const ModelDefinition &definition)
    {
        return this->WriteToFile(definition, definition.absolutePath.c_str());
    }

    bool ModelLibrary::WriteToFile(const char* fileName)
    {
        // We first need to find the definition.
        auto iDefinition = m_loadedDefinitions.Find(fileName);
        if (iDefinition == nullptr)
        {
            if (easypath_extension_equal(fileName, "gtmodel"))
            {
                char fileNameWithoutExtension[DRVFS_MAX_PATH];
                easypath_copy_and_remove_extension(fileNameWithoutExtension, sizeof(fileNameWithoutExtension), fileName);

                iDefinition = m_loadedDefinitions.Find(fileNameWithoutExtension);
            }
        }

        if (iDefinition != nullptr)
        {
            return this->WriteToFile(*iDefinition->value.definition, fileName);
        }

        return false;
    }



    ////////////////////////////////////////////////
    // Misc.

    bool ModelLibrary::IsExtensionSupported(const char* extension)
    {
        if (Strings::Equal<false>(extension, "gtmodel"))
        {
            return true;
        }
        else
        {
            // Assimp actually supports XML by default, but we would rather them be text files. Thus, we're going
            // to return false if XML is specified.
            if (Strings::Equal<false>(extension, "xml"))
            {
                return false;
            }

            String assimpExt(".");
            assimpExt += extension;

            Assimp::Importer importer;
            return importer.IsExtensionSupported(assimpExt.c_str());
        }
    }

    

    ////////////////////////////////////////////////////////
    // Private

    bool ModelLibrary::FindAbsolutePath(const char* relativePath, String &absolutePath)
    {
        char absolutePathTemp[DRVFS_MAX_PATH];
        if (!drvfs_find_absolute_path(g_Context->GetVFS(), relativePath, absolutePathTemp, sizeof(absolutePathTemp)))
        {
            char adjustedRelativePath[DRVFS_MAX_PATH];
            strcpy_s(adjustedRelativePath, sizeof(adjustedRelativePath), relativePath);

            if (easypath_extension_equal(relativePath, "gtmodel")) {
                easypath_remove_extension(adjustedRelativePath);
            } else {
                easypath_append_extension(adjustedRelativePath, sizeof(adjustedRelativePath), "gtmodel");
            }

            if (!drvfs_find_absolute_path(g_Context->GetVFS(), adjustedRelativePath, absolutePathTemp, sizeof(absolutePathTemp))) {
                return false;
            }
        }

        absolutePath = absolutePathTemp;
        return true;
    }

    ModelDefinition* ModelLibrary::FindDefinition(const char* absolutePath)
    {
        auto iDefinition = m_loadedDefinitions.Find(absolutePath);
        if (iDefinition != nullptr)
        {
            return iDefinition->value.definition;
        }

        return nullptr;
    }

    Model* ModelLibrary::CreateFromPrimitive(const char* name, VertexArray* va)
    {
        ModelDefinition* definition = nullptr;

        // We first need to retrieve our model info.
        auto iDefinition = m_loadedDefinitions.Find(name);
        if (iDefinition == nullptr)
        {
            definition = new ModelDefinition(m_context);
            
            ModelDefinition::Mesh mesh(m_context);
            mesh.geometry = va;
            mesh.material = m_context.GetMaterialLibrary().Create("engine/materials/simple-diffuse.material");
            definition->AddMesh(mesh);

            m_loadedDefinitions.Add(name, ModelDefinitionReference(definition, 1));
        }
        else
        {
            definition = iDefinition->value.definition;
        }


        // Now that we have the model information we can create a model from it.
        if (definition != nullptr)
        {
            return this->CreateFromDefinition(*definition);
        }

        return nullptr;
    }
}
