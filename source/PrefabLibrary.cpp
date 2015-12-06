// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/PrefabLibrary.hpp>
#include <GTGE/GTEngine.hpp>
#include <GTGE/Core/Dictionary.hpp>
#include <easy_path/easy_path.h>
#include <utility>


namespace GT
{
    PrefabLibrary::PrefabLibrary(Context &context)
        : m_context(context)
    {
    }

    PrefabLibrary::~PrefabLibrary()
    {
    }



    //////////////////////////////////////
    // Startup/Shutdown.

    bool PrefabLibrary::Startup()
    {
        return true;
    }

    void PrefabLibrary::Shutdown()
    {
        for (size_t i = 0; i < m_loadedPrefabs.count; ++i)
        {
            delete m_loadedPrefabs.buffer[i]->value.first;
        }
        m_loadedPrefabs.Clear();
    }



    //////////////////////////////////////
    // Acquire/Unacquire

    Prefab* PrefabLibrary::Acquire(const char* fileName, const char* makeRelativeTo)
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
            auto iLoadedPrefab = m_loadedPrefabs.Find(absolutePath);
            if (iLoadedPrefab == nullptr)
            {
                // Does not exist. Needs to be loaded.
                easyvfs_file* pFile = easyvfs_open(g_Context->GetVFS(), absolutePath, EASYVFS_READ, 0);
                if (pFile != nullptr)
                {
                    FileDeserializer deserializer(pFile);
                    
                    auto newPrefab = new Prefab(absolutePath, relativePath);
                    newPrefab->Deserialize(deserializer);

                    m_loadedPrefabs.Add(absolutePath, PrefabReference(newPrefab, 1));

                    easyvfs_close(pFile);
                    return newPrefab;
                }
                else
                {
                    g_Context->LogErrorf("Can not find file: %s", fileName);
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
            g_Context->LogErrorf("Can not find file: %s", fileName);
        }


        return nullptr;
    }

    void PrefabLibrary::Unacquire(const Prefab* prefabToUnacquire)
    {
        if (prefabToUnacquire != nullptr)
        {
            // We need to search by value.
            for (size_t i = 0; i < m_loadedPrefabs.count; ++i)
            {
                auto &value = m_loadedPrefabs.buffer[i]->value;
                if (value.first == prefabToUnacquire)
                {
                    assert(value.second > 0);
                    {
                        --value.second;

                        if (value.second == 0)
                        {
                            delete value.first;
                            m_loadedPrefabs.RemoveByIndex(i);
                        }

                        break;
                    }
                }
            }
        }
    }
}
