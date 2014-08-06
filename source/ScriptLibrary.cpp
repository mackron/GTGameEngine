// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/GTEngine.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTLib/IO.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Dictionary.hpp>
#include <utility>

namespace GTEngine
{
    //////////////////////////////////////
    // Globals.

    typedef std::pair<ScriptDefinition*, size_t> ScriptDefinitionReference;

    /// The list of loaded classes, indexed by the absolute path.
    static GTLib::Dictionary<ScriptDefinitionReference> LoadedDefinitions;

    /// A pointer to the working script. This is instantiated in Startup() and deleted in Shutdown().
    static GTLib::Script* WorkingScript = nullptr;



    /////////////////////////////////////////////////
    // Startup/Shutdown

    bool ScriptLibrary::Startup()
    {
        WorkingScript = new GTLib::Script;
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
        GTLib::String relativePath(fileName);

        if (GTLib::Path::IsAbsolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                relativePath = GTLib::IO::ToRelativePath(fileName, makeRelativeTo);
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        GTLib::String absolutePath;
        if (GTLib::IO::FindAbsolutePath(fileName, absolutePath))
        {
            auto iLoadedClass = LoadedDefinitions.Find(absolutePath.c_str());
            if (iLoadedClass == nullptr)
            {
                // Does not exist. Needs to be loaded.
                GTLib::String scriptString;
                if (GTLib::IO::OpenAndReadTextFile(absolutePath.c_str(), scriptString))
                {
                    auto newDefinition = new ScriptDefinition(absolutePath.c_str(), relativePath.c_str(), scriptString.c_str());
                    LoadedDefinitions.Add(absolutePath.c_str(), ScriptDefinitionReference(newDefinition, 1));


                    assert(GlobalGame != nullptr);
                    {
                        Scripting::LoadScriptDefinition(GlobalGame->GetScript(), relativePath.c_str(), scriptString.c_str());
                    }

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
                            assert(GlobalGame != nullptr);
                            {
                                Scripting::UnloadScriptDefinition(GlobalGame->GetScript(), scriptDefinitionToUnacquire->GetRelativePath());
                            }

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
        GTLib::String relativePath(fileName);

        if (GTLib::Path::IsAbsolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                relativePath = GTLib::IO::ToRelativePath(fileName, makeRelativeTo);
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        // We key the definitions by their absolute path, so we'll need to retrieve that.
        GTLib::String absolutePath;
        if (GTLib::IO::FindAbsolutePath(fileName, absolutePath))
        {
            return LoadedDefinitions.Exists(absolutePath.c_str());
        }
        
        return false;
    }

    bool ScriptLibrary::Reload(const char* fileName, const char* makeRelativeTo)
    {
        GTLib::String relativePath(fileName);

        if (GTLib::Path::IsAbsolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                relativePath = GTLib::IO::ToRelativePath(fileName, makeRelativeTo);
            }
            else
            {
                GTEngine::PostError("Attempting to reload a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return false;
            }
        }


        // We key the definitions by their absolute path, so we'll need to retrieve that.
        GTLib::String absolutePath;
        if (GTLib::IO::FindAbsolutePath(fileName, absolutePath))
        {
            auto iDefinition = LoadedDefinitions.Find(absolutePath.c_str());
            if (iDefinition != nullptr)
            {
                auto definition = iDefinition->value.first;
                assert(definition != nullptr);
                {
                    // This next part is easy. We just destruct and reconstruct, while leaving the pointer as-is. It's important that we keep the same pointer because
                    // other objects may be referencing it.
                    GTLib::String scriptString;
                    if (GTLib::IO::OpenAndReadTextFile(absolutePath.c_str(), scriptString))
                    {
                        definition->~ScriptDefinition();
                        new (definition) ScriptDefinition(absolutePath.c_str(), relativePath.c_str(), scriptString.c_str());


                        assert(GlobalGame != nullptr);
                        {
                            Scripting::LoadScriptDefinition(GlobalGame->GetScript(), relativePath.c_str(), scriptString.c_str());
                        }

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

    GTLib::Script & ScriptLibrary::GetWorkingScript()
    {
        assert(WorkingScript != nullptr);
        return *WorkingScript;
    }
}