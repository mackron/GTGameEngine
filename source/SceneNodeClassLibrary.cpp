// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneNodeClassLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/Dictionary.hpp>
#include <utility>


namespace GTEngine
{
    //////////////////////////////////////
    // Globals.

    typedef std::pair<SceneNodeClass*, size_t> SceneNodeClassReference;

    /// The list of loaded classes, indexed by the absolute path.
    static GTCore::Dictionary<SceneNodeClassReference> LoadedClasses;




    //////////////////////////////////////
    // Startup/Shutdown.

    bool SceneNodeClassLibrary::Startup()
    {
        return true;
    }

    void SceneNodeClassLibrary::Shutdown()
    {
        for (size_t i = 0; i < LoadedClasses.count; ++i)
        {
            delete LoadedClasses.buffer[i]->value.first;
        }
        LoadedClasses.Clear();
    }



    //////////////////////////////////////
    // Acquire/Unacquire

    SceneNodeClass* SceneNodeClassLibrary::Acquire(const char* fileName, const char* makeRelativeTo)
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


        GTCore::String absFileName;
        if (GTCore::IO::FindAbsolutePath(fileName, absFileName))
        {
            auto iLoadedClass = LoadedClasses.Find(absFileName.c_str());
            if (iLoadedClass == nullptr)
            {
                // Does not exist. Needs to be loaded.
                auto file = GTCore::IO::Open(absFileName.c_str(), GTCore::IO::OpenMode::Read);
                if (file != nullptr)
                {
                    // We use a file deserializer for this.
                    GTCore::FileDeserializer deserializer(file);
                    
                    auto newSceneNodeClass = new SceneNodeClass;
                    newSceneNodeClass->Deserialize(deserializer);

                    LoadedClasses.Add(absFileName.c_str(), SceneNodeClassReference(newSceneNodeClass, 1));

                    return newSceneNodeClass;
                }
                else
                {
                    GTEngine::PostError("Can not find file: %s", fileName);
                }
            }
            else
            {
                // Already exists, so we just increment the counter.
                ++iLoadedClass->value.second;

                return iLoadedClass->value.first;
            }
        }
        else
        {
            GTEngine::PostError("Can not find file: %s", fileName);
        }


        return nullptr;
    }

    void SceneNodeClassLibrary::Unacquire(const SceneNodeClass* sceneNodeClassToUnacquire)
    {
        if (sceneNodeClassToUnacquire != nullptr)
        {
            // We need to search by value.
            for (size_t i = 0; i < LoadedClasses.count; ++i)
            {
                auto &value = LoadedClasses.buffer[i]->value;
                if (value.first == sceneNodeClassToUnacquire)
                {
                    assert(value.second > 0);
                    {
                        --value.second;

                        if (value.second == 0)
                        {
                            delete value.first;
                            LoadedClasses.RemoveByIndex(i);
                        }

                        break;
                    }
                }
            }
        }
    }


    SceneNodeClass* SceneNodeClassLibrary::Create(const char* absolutePath, const SceneNode &sceneNode, bool acquire)
    {
        assert(GTCore::Path::IsAbsolute(absolutePath));
        {
            auto file = GTCore::IO::Open(absolutePath, GTCore::IO::OpenMode::Write);
            if (file != nullptr)
            {
                SceneNodeClass* sceneNodeClass = nullptr;


                // Find the class, creating if required.
                auto iClass = LoadedClasses.Find(absolutePath);
                if (iClass != nullptr)
                {
                    sceneNodeClass = iClass->value.first;

                    if (acquire)
                    {
                        ++iClass->value.second;
                    }
                }
                else
                {
                    sceneNodeClass = new SceneNodeClass;

                    if (acquire)
                    {
                        LoadedClasses.Add(absolutePath, SceneNodeClassReference(sceneNodeClass, 1));
                    }
                }


                // Now update and serialize the class.
                assert(sceneNodeClass != nullptr);
                {
                    sceneNodeClass->SetFromSceneNode(sceneNode);

                    GTCore::FileSerializer serializer(file);
                    sceneNodeClass->Serialize(serializer);
                }


                // Can't forget to close the file...
                GTCore::IO::Close(file);


                // If we're not acquiring, we actually want to delete the class.
                if (!acquire)
                {
                    delete sceneNodeClass;
                    sceneNodeClass = nullptr;
                }


                return sceneNodeClass;
            }
            else
            {
                GTEngine::PostError("Can not open file for writing: %s", absolutePath);
            }
        }


        return nullptr;
    }
}
