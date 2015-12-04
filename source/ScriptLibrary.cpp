// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GTEngine.hpp>
#include <GTGE/ScriptLibrary.hpp>
#include <GTGE/Scripting.hpp>
#include <GTGE/Core/Dictionary.hpp>
#include <easy_path/easy_path.h>
#include <utility>

namespace GT
{
    //////////////////////////////////////
    // Globals.

    typedef std::pair<ScriptDefinition*, size_t> ScriptDefinitionReference;

    /// The list of loaded classes, indexed by the absolute path.
    static Dictionary<ScriptDefinitionReference> LoadedDefinitions;

    /// A pointer to the working script. This is instantiated in Startup() and deleted in Shutdown().
    static GT::Script* WorkingScript = nullptr;

    /// A pointer to the main game object. This is passed into the Startup() routine.
    static Game* g_pGame = nullptr;



    /////////////////////////////////////////////////
    // Startup/Shutdown

    bool ScriptLibrary::Startup(Game* pGame)
    {
        if (pGame == nullptr) {
            return false;
        }

        g_pGame = pGame;

        WorkingScript = new GT::Script;
        if (!GT::LoadExtendedMathLibrary(*WorkingScript))
        {
            delete WorkingScript;
            return false;
        }

        return true;
    }

    void ScriptLibrary::Shutdown()
    {
        for (size_t i = 0; i < LoadedDefinitions.count; ++i)
        {
            delete LoadedDefinitions.buffer[i]->value.first;
        }
        LoadedDefinitions.Clear();


        delete WorkingScript;
        WorkingScript = nullptr;
    }


    /////////////////////////////////////////////////
    // Acquire/Unacquire

    ScriptDefinition* ScriptLibrary::Acquire(const char* fileName, const char* makeRelativeTo, bool silenceMissingFileWarning)
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
            auto iLoadedClass = LoadedDefinitions.Find(absolutePath);
            if (iLoadedClass == nullptr)
            {
                // Does not exist. Needs to be loaded.
                char* scriptString = easyvfs_open_and_read_text_file(g_Context->GetVFS(), absolutePath, nullptr);
                if (scriptString != nullptr)
                {
                    auto newDefinition = new ScriptDefinition(absolutePath, scriptString);
                    LoadedDefinitions.Add(absolutePath, ScriptDefinitionReference(newDefinition, 1));

                    GT::LoadScriptDefinition(g_pGame->GetScript(), absolutePath, scriptString);

                    easyvfs_free(scriptString);
                    return newDefinition;
                }
                else
                {
                    if (!silenceMissingFileWarning)
                    {
                        g_Context->LogErrorf("Can not find file: %s", fileName);
                    }
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
            if (!silenceMissingFileWarning)
            {
                g_Context->LogErrorf("Can not find file: %s", fileName);
            }
        }

        return nullptr;
    }

    void ScriptLibrary::Unacquire(const ScriptDefinition* scriptDefinitionToUnacquire)
    {
        if (scriptDefinitionToUnacquire != nullptr)
        {
            // We need to search by value.
            for (size_t i = 0; i < LoadedDefinitions.count; ++i)
            {
                auto &value = LoadedDefinitions.buffer[i]->value;
                if (value.first == scriptDefinitionToUnacquire)
                {
                    assert(value.second > 0);
                    {
                        --value.second;

                        if (value.second == 0)
                        {
                            GT::UnloadScriptDefinition(g_pGame->GetScript(), scriptDefinitionToUnacquire->GetAbsolutePath());

                            delete value.first;
                            LoadedDefinitions.RemoveByIndex(i);
                        }

                        break;
                    }
                }
            }
        }
    }

    bool ScriptLibrary::IsLoaded(const char* fileName)
    {
        // We key the definitions by their absolute path, so we'll need to retrieve that.
        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            return LoadedDefinitions.Exists(absolutePath);
        }

        return false;
    }

    bool ScriptLibrary::Reload(const char* fileName)
    {
        // We key the definitions by their absolute path, so we'll need to retrieve that.
        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            auto iDefinition = LoadedDefinitions.Find(absolutePath);
            if (iDefinition != nullptr)
            {
                auto definition = iDefinition->value.first;
                assert(definition != nullptr);
                {
                    // This next part is easy. We just destruct and reconstruct, while leaving the pointer as-is. It's important that we keep the same pointer because
                    // other objects may be referencing it.
                    char* scriptString = easyvfs_open_and_read_text_file(g_Context->GetVFS(), absolutePath, nullptr);
                    if (scriptString != nullptr)
                    {
                        definition->~ScriptDefinition();
                        new (definition) ScriptDefinition(absolutePath, scriptString);

                        GT::LoadScriptDefinition(g_pGame->GetScript(), definition->GetAbsolutePath(), scriptString);

                        easyvfs_free(scriptString);
                        return true;
                    }
                }
            }
        }

        // We fall through to here if we fail to reload for any reason.
        return false;
    }




    /////////////////////////////////////////////////
    // Iteration

    size_t ScriptLibrary::GetLoadedDefinitionCount()
    {
        return LoadedDefinitions.count;
    }

    const ScriptDefinition* ScriptLibrary::GetLoadedDefinitionByIndex(size_t index)
    {
        return LoadedDefinitions.buffer[index]->value.first;
    }




    /////////////////////////////////////////////////
    // Misc.

    GT::Script & ScriptLibrary::GetWorkingScript()
    {
        assert(WorkingScript != nullptr);
        return *WorkingScript;
    }
}
