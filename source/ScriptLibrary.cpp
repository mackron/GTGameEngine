// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTCore/IO.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/Dictionary.hpp>
#include <utility>

namespace GTEngine
{
    //////////////////////////////////////
    // Globals.

    typedef std::pair<ScriptDefinition*, size_t> ScriptDefinitionReference;

    /// The list of loaded classes, indexed by the absolute path.
    static GTCore::Dictionary<ScriptDefinitionReference> LoadedDefinitions;

    /// A pointer to the working script. This is instantiated in Startup() and deleted in Shutdown().
    static GTCore::Script* WorkingScript = nullptr;



    /////////////////////////////////////////////////
    // Startup/Shutdown

    bool ScriptLibrary::Startup()
    {
        WorkingScript = new GTCore::Script;
        if (!Scripting::LoadExtendedMathLibrary(*WorkingScript))
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


        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            auto iLoadedClass = LoadedDefinitions.Find(absolutePath.c_str());
            if (iLoadedClass == nullptr)
            {
                // Does not exist. Needs to be loaded.
                GTCore::String scriptString;
                if (GTCore::IO::OpenAndReadTextFile(absolutePath.c_str(), scriptString))
                {
                    auto newDefinition = new ScriptDefinition(absolutePath.c_str(), relativePath.c_str(), scriptString.c_str());
                    LoadedDefinitions.Add(absolutePath.c_str(), ScriptDefinitionReference(newDefinition, 1));

                    return newDefinition;
                }
                else
                {
                    if (!silenceMissingFileWarning)
                    {
                        GTEngine::PostError("Can not find file: %s", fileName);
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
                GTEngine::PostError("Can not find file: %s", fileName);
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
                            delete value.first;
                            LoadedDefinitions.RemoveByIndex(i);
                        }

                        break;
                    }
                }
            }
        }
    }

    bool ScriptLibrary::IsLoaded(const char* fileName, const char* makeRelativeTo)
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


        // We key the definitions by their absolute path, so we'll need to retrieve that.
        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            return LoadedDefinitions.Exists(absolutePath.c_str());
        }
        
        return false;
    }

    bool ScriptLibrary::Reload(const char* fileName, const char* makeRelativeTo)
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
                GTEngine::PostError("Attempting to reload a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return false;
            }
        }


        // We key the definitions by their absolute path, so we'll need to retrieve that.
        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            auto iDefinition = LoadedDefinitions.Find(absolutePath.c_str());
            if (iDefinition != nullptr)
            {
                auto definition = iDefinition->value.first;
                assert(definition != nullptr);
                {
                    // This next part is easy. We just destruct and reconstruct, while leaving the pointer as-is. It's important that we keep the same pointer because
                    // other objects may be referencing it.
                    GTCore::String scriptString;
                    if (GTCore::IO::OpenAndReadTextFile(absolutePath.c_str(), scriptString))
                    {
                        definition->~ScriptDefinition();
                        new (definition) ScriptDefinition(absolutePath.c_str(), relativePath.c_str(), scriptString.c_str());

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

    GTCore::Script & ScriptLibrary::GetWorkingScript()
    {
        assert(WorkingScript != nullptr);
        return *WorkingScript;
    }
}