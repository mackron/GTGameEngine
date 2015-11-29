// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/PrefabLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTLib/Dictionary.hpp>
#include <GTEngine/GTEngine.hpp>
#include <easy_path/easy_path.h>
#include <utility>


namespace GTEngine
{
    //////////////////////////////////////
    // Globals.

    typedef std::pair<Prefab*, size_t> PrefabReference;

    /// The list of loaded classes, indexed by the absolute path.
    static GTLib::Dictionary<PrefabReference> LoadedPrefabs;




    //////////////////////////////////////
    // Startup/Shutdown.

    bool PrefabLibrary::Startup()
    {
        return true;
    }

    void PrefabLibrary::Shutdown()
    {
        for (size_t i = 0; i < LoadedPrefabs.count; ++i)
        {
            delete LoadedPrefabs.buffer[i]->value.first;
        }
        LoadedPrefabs.Clear();
    }



    //////////////////////////////////////
    // Acquire/Unacquire

    Prefab* PrefabLibrary::Acquire(const char* fileName, const char* makeRelativeTo)
    {
        char relativePath[EASYVFS_MAX_PATH];
        strcpy_s(relativePath, sizeof(relativePath), fileName);

        if (easypath_isabsolute(fileName))
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
            auto iLoadedPrefab = LoadedPrefabs.Find(absolutePath);
            if (iLoadedPrefab == nullptr)
            {
                // Does not exist. Needs to be loaded.
                easyvfs_file* pFile = easyvfs_open(g_EngineContext->GetVFS(), absolutePath, EASYVFS_READ, 0);
                if (pFile != nullptr)
                {
                    GTLib::FileDeserializer deserializer(pFile);
                    
                    auto newPrefab = new Prefab(absolutePath, relativePath);
                    newPrefab->Deserialize(deserializer);

                    LoadedPrefabs.Add(absolutePath, PrefabReference(newPrefab, 1));

                    easyvfs_close(pFile);
                    return newPrefab;
                }
                else
                {
                    GTEngine::PostError("Can not find file: %s", fileName);
                }
            }
            else
            {
                // Already exists, so we just increment the counter.
                ++iLoadedPrefab->value.second;

                return iLoadedPrefab->value.first;
            }
        }
        else
        {
            GTEngine::PostError("Can not find file: %s", fileName);
        }


        return nullptr;
    }

    void PrefabLibrary::Unacquire(const Prefab* prefabToUnacquire)
    {
        if (prefabToUnacquire != nullptr)
        {
            // We need to search by value.
            for (size_t i = 0; i < LoadedPrefabs.count; ++i)
            {
                auto &value = LoadedPrefabs.buffer[i]->value;
                if (value.first == prefabToUnacquire)
                {
                    assert(value.second > 0);
                    {
                        --value.second;

                        if (value.second == 0)
                        {
                            delete value.first;
                            LoadedPrefabs.RemoveByIndex(i);
                        }

                        break;
                    }
                }
            }
        }
    }
}
