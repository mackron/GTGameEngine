// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GTEngine.hpp>
#include <GTGE/ScriptLibrary.hpp>
#include <GTGE/Scripting.hpp>
#include <easy_path/easy_path.h>

namespace GT
{
    ScriptLibrary::ScriptLibrary(Context &context)
        : m_context(context), m_workingScript(), m_loadedDefinitions()
    {
    }

    ScriptLibrary::~ScriptLibrary()
    {
    }


    /////////////////////////////////////////////////
    // Startup/Shutdown

    bool ScriptLibrary::Startup()
    {
        if (!GT::LoadExtendedMathLibrary(m_workingScript))
        {
            return false;
        }

        return true;
    }

    void ScriptLibrary::Shutdown()
    {
        for (size_t i = 0; i < m_loadedDefinitions.count; ++i)
        {
            delete m_loadedDefinitions.buffer[i]->value.first;
        }
        m_loadedDefinitions.Clear();
    }


    /////////////////////////////////////////////////
    // Acquire/Unacquire

    ScriptDefinition* ScriptLibrary::Acquire(const char* fileName, const char* makeRelativeTo, bool silenceMissingFileWarning)
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


        char absolutePath[DRVFS_MAX_PATH];
        if (drvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            auto iLoadedClass = m_loadedDefinitions.Find(absolutePath);
            if (iLoadedClass == nullptr)
            {
                // Does not exist. Needs to be loaded.
                char* scriptString = drvfs_open_and_read_text_file(g_Context->GetVFS(), absolutePath, nullptr);
                if (scriptString != nullptr)
                {
                    auto newDefinition = new ScriptDefinition(absolutePath, scriptString);
                    m_loadedDefinitions.Add(absolutePath, ScriptDefinitionReference(newDefinition, 1));

                    GT::LoadScriptDefinition(m_context.GetScript(), absolutePath, scriptString);

                    drvfs_free(scriptString);
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
            for (size_t i = 0; i < m_loadedDefinitions.count; ++i)
            {
                auto &value = m_loadedDefinitions.buffer[i]->value;
                if (value.first == scriptDefinitionToUnacquire)
                {
                    assert(value.second > 0);
                    {
                        --value.second;

                        if (value.second == 0)
                        {
                            GT::UnloadScriptDefinition(m_context.GetScript(), scriptDefinitionToUnacquire->GetAbsolutePath());

                            delete value.first;
                            m_loadedDefinitions.RemoveByIndex(i);
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
        char absolutePath[DRVFS_MAX_PATH];
        if (drvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            return m_loadedDefinitions.Exists(absolutePath);
        }

        return false;
    }

    bool ScriptLibrary::Reload(const char* fileName)
    {
        // We key the definitions by their absolute path, so we'll need to retrieve that.
        char absolutePath[DRVFS_MAX_PATH];
        if (drvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            auto iDefinition = m_loadedDefinitions.Find(absolutePath);
            if (iDefinition != nullptr)
            {
                auto definition = iDefinition->value.first;
                assert(definition != nullptr);
                {
                    // This next part is easy. We just destruct and reconstruct, while leaving the pointer as-is. It's important that we keep the same pointer because
                    // other objects may be referencing it.
                    char* scriptString = drvfs_open_and_read_text_file(g_Context->GetVFS(), absolutePath, nullptr);
                    if (scriptString != nullptr)
                    {
                        definition->~ScriptDefinition();
                        new (definition) ScriptDefinition(absolutePath, scriptString);

                        GT::LoadScriptDefinition(m_context.GetScript(), definition->GetAbsolutePath(), scriptString);

                        drvfs_free(scriptString);
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
        return m_loadedDefinitions.count;
    }

    const ScriptDefinition* ScriptLibrary::GetLoadedDefinitionByIndex(size_t index)
    {
        return m_loadedDefinitions.buffer[index]->value.first;
    }




    /////////////////////////////////////////////////
    // Misc.

    GT::Script & ScriptLibrary::GetWorkingScript()
    {
        return m_workingScript;
    }
}
