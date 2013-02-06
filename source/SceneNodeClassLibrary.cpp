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
                    return nullptr;
                }
            }
            else
            {
                // Already exists, so we just increment the counter.
                ++iLoadedClass->value.second;

                return iLoadedClass->value.first;
            }
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
}
