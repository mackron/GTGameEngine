// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

#include <GTCore/Path.hpp>
#include <GTCore/IO.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <assimp/Importer.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


// Startup/Shutdown
namespace GTEngine
{
    ////////////////////////////////////////////////
    // Globals

    /// The map of model definitions mapping a definition to a file name.
    static GTCore::Dictionary<ModelDefinition*> LoadedDefinitions;

    /// We need to keep track of the models that are using each definition. What we do here is keep a map with the key being a pointer
    /// to each loaded definition, and the value being a list of every loaded model that is using that definition.
    static GTCore::Map<ModelDefinition*, GTCore::Vector<Model*>*> LoadedModels;


    /// Creates a model from a primitive's vertex array.
    ///
    /// @param name [in] The name of the primitive.
    /// @param va   [in] The vertex array of the primitive. This can be nullptr, but only if the primitive has already been created.
    Model* ModelLibrary_CreateFromPrimitive(const char* name, VertexArray* va);



    ////////////////////////////////////////////////
    // Startup/Shutdown

    bool ModelLibrary::Startup()
    {
        return true;
    }

    void ModelLibrary::Shutdown()
    {
        // All models and definitions need to be deleted.
        for (size_t iDefinition = 0; iDefinition < LoadedModels.count; ++iDefinition)
        {
            auto definition = LoadedModels.buffer[iDefinition]->key;
            auto models     = LoadedModels.buffer[iDefinition]->value;

            if (models != nullptr)
            {
                for (size_t iModel = 0; iModel < models->count; ++iModel)
                {
                    delete models->buffer[iModel];
                }
            }

            delete models;
            delete definition;
        }
    }


    ////////////////////////////////////////////////
    // Create/Delete

    Model* ModelLibrary::Create(const char* fileName, const char* makeRelativeTo)
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


        // We will first find an existing model definition. If we don't find it, we create one and the load into it.
        GTCore::String absolutePath;
        if (ModelLibrary::FindAbsolutePath(fileName, absolutePath))
        {
            auto definition = ModelLibrary::FindDefinition(absolutePath.c_str());
            if (definition == nullptr)
            {
                definition = new ModelDefinition;

                bool needsSerialize;
                if (definition->LoadFromFile(absolutePath.c_str(), relativePath.c_str(), needsSerialize))
                {
                    LoadedDefinitions.Add(definition->absolutePath.c_str(), definition);
                    LoadedModels.Add(definition, new GTCore::Vector<Model*>);

                    if (needsSerialize)
                    {
                        ModelLibrary::WriteToFile(*definition);
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
                return ModelLibrary::CreateFromDefinition(*definition);
            }
        }

        return nullptr;
    }

    Model* ModelLibrary::CreateFromDefinition(const ModelDefinition &definition)
    {
        auto model = new Model(definition);

        auto iDefinitionModels = LoadedModels.Find(const_cast<ModelDefinition*>(&definition));        // <-- Naughty const_cast is OK here.
        assert(iDefinitionModels        != nullptr);
        assert(iDefinitionModels->value != nullptr);

        iDefinitionModels->value->PushBack(model);

        return model;
    }


    static int ConvexHullCount = 0;
    Model* ModelLibrary::CreateFromConvexHull(const ConvexHull &convexHull)
    {
        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        GTCore::IO::snprintf(name, 128, "convexhull(%d)", ConvexHullCount++);

        // We create the model from a primitive. To do this we need a non-const vertex array.
        VertexArray* va = nullptr;

        bool exists = LoadedDefinitions.Find(name) != nullptr;
        if (!exists)
        {
            va = VertexArrayLibrary::CreateFromConvexHull(convexHull);
        }

        return ModelLibrary_CreateFromPrimitive(name, va);
    }


    void ModelLibrary::Delete(Model* model)
    {
        if (model != nullptr)
        {
            // We need to find the list this model is part of and remove it.
            auto iDefinitionModels = LoadedModels.Find(const_cast<ModelDefinition*>(&model->GetDefinition()));      // <-- const_cast is safe here.
            if (iDefinitionModels != nullptr)
            {
                assert(iDefinitionModels->value != nullptr);
                {
                    iDefinitionModels->value->RemoveFirstOccuranceOf(model);
                }
            }

            delete model;
        }
    }


    bool ModelLibrary::Reload(const char* fileName)
    {
        // We need to find the definition that we're updating.
        auto definition = ModelLibrary::FindDefinition(fileName);
        if (definition != nullptr)
        {
            bool needsSerialize;
            if (definition->LoadFromFile(definition->absolutePath.c_str(), definition->relativePath.c_str(), needsSerialize))
            {
                if (needsSerialize)
                {
                    ModelLibrary::WriteToFile(*definition);
                }
            }


            // Every model with this definition needs to know that it has changed.
            auto iDefinitionModels = LoadedModels.Find(definition);
            assert(iDefinitionModels        != nullptr);
            assert(iDefinitionModels->value != nullptr);

            auto modelList = iDefinitionModels->value;
            for (size_t iModel = 0; iModel < modelList->count; ++iModel)
            {
                auto model = modelList->buffer[iModel];
                assert(model != nullptr);

                model->OnDefinitionChanged();
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
        GTCore::String fileName(fileNameIn);
        if (!GTCore::Path::ExtensionEqual(fileNameIn, "gtmodel"))
        {
            fileName += ".gtmodel";
        }

        auto file = GTCore::IO::Open(fileName.c_str(), GTCore::IO::OpenMode::Write);
        if (file != nullptr)
        {
            GTCore::FileSerializer serializer(file);
            definition.Serialize(serializer);

            GTCore::IO::Close(file);
            return true;
        }

        return false;
    }

    bool ModelLibrary::WriteToFile(const ModelDefinition &definition)
    {
        return ModelLibrary::WriteToFile(definition, definition.absolutePath.c_str());
    }

    bool ModelLibrary::WriteToFile(const char* fileName)
    {
        // We first need to find the definition. If we can't find it with the original file name, we add '.gtmodel' and try again. If both fail, we need
        // to return false.
        auto iDefinition = LoadedDefinitions.Find(fileName);
        if (iDefinition == nullptr)
        {
            iDefinition = LoadedDefinitions.Find((GTCore::String(fileName) + ".gtmodel").c_str());
        }

        if (iDefinition != nullptr)
        {
            return ModelLibrary::WriteToFile(*iDefinition->value, fileName);
        }

        return false;
    }



    void ModelLibrary::DeleteUnreferenceDefinitions()
    {
        GTCore::List<ModelDefinition*> definitionsToDelete;

        for (size_t i = 0; i < LoadedModels.count; ++i)
        {
            auto iDefinitionModels = LoadedModels.buffer[i];
            assert(iDefinitionModels        != nullptr);
            assert(iDefinitionModels->value != nullptr);

            if (iDefinitionModels->value->count == 0)
            {
                auto definition = iDefinitionModels->key;
                auto modelsList = iDefinitionModels->value;

                definitionsToDelete.Append(definition);

                delete definition;
                delete modelsList;
            }
        }


        // Now all we do is remove the old pointers.
        while (definitionsToDelete.root != nullptr)
        {
            auto definition = definitionsToDelete.root->value;

            for (size_t i = 0; i < LoadedDefinitions.count; ++i)
            {
                if (LoadedDefinitions.buffer[i]->value == definition)
                {
                    LoadedDefinitions.RemoveByIndex(i);
                    break;
                }
            }

            LoadedModels.Remove(definition);
        }
    }


    bool ModelLibrary::IsExtensionSupported(const char* extension)
    {
        if (GTCore::Strings::Equal<false>(extension, "gtmodel"))
        {
            return true;
        }
        else
        {
            // Assimp actually supports XML by default, but we would rather them be text files. Thus, we're going
            // to return false if XML is specified.
            if (GTCore::Strings::Equal<false>(extension, "xml"))
            {
                return false;
            }

            GTCore::String assimpExt(".");
            assimpExt += extension;

            Assimp::Importer importer;
            return importer.IsExtensionSupported(assimpExt.c_str());
        }
    }

    



    ////////////////////////////////////////////////////////
    // Private

    bool ModelLibrary::FindAbsolutePath(const char* relativePath, GTCore::String &absolutePath)
    {
        if (!GTCore::IO::FindAbsolutePath(relativePath, absolutePath))
        {
            // The file doesn't exist. We need to either remove or add the .gtmodel extension and try again.
            if (GTCore::Path::ExtensionEqual(relativePath, "gtmodel"))
            {
                return GTCore::IO::FindAbsolutePath(GTCore::IO::RemoveExtension(relativePath).c_str(), absolutePath);
            }
            else
            {
                return GTCore::IO::FindAbsolutePath((GTCore::String(relativePath) + ".gtmodel").c_str(), absolutePath);
            }
        }

        return true;
    }

    ModelDefinition* ModelLibrary::FindDefinition(const char* absolutePath)
    {
        auto iDefinition = LoadedDefinitions.Find(absolutePath);
        if (iDefinition != nullptr)
        {
            return iDefinition->value;
        }

        return nullptr;
    }
}





// These are private functions implementations for ModelLibrary.
namespace GTEngine
{
    Model* ModelLibrary_CreateFromPrimitive(const char* name, VertexArray* va)
    {
        ModelDefinition* definition = nullptr;

        // We first need to retrieve our model info.
        auto iModelInfo = LoadedDefinitions.Find(name);
        if (iModelInfo == nullptr)
        {
            definition = new ModelDefinition;
            definition->meshGeometries.PushBack(va);
            definition->meshMaterials.PushBack(MaterialLibrary::Create("engine/materials/simple-diffuse.material"));
            definition->meshSkinningVertexAttributes.PushBack(nullptr);

            LoadedDefinitions.Add(name, definition);
            LoadedModels.Add(definition, new GTCore::Vector<Model*>);
        }
        else
        {
            definition = iModelInfo->value;
        }


        // Now that we have the model information we can create a model from it.
        if (definition != nullptr)
        {
            return ModelLibrary::CreateFromDefinition(*definition);
        }

        return nullptr;
    }
}
