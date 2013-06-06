// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/ApplicationConfig.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    GameScript::GameScript(Game &game)
        : game(game), lastError(), loadedFiles(), errorHandler(*this)
    {
        this->AttachErrorHandler(errorHandler);
    }

    GameScript::~GameScript()
    {
    }

    bool GameScript::Startup()
    {
        // First we load the GTEngine scripting stuff.
        if (Scripting::LoadGTEngineScriptLibrary(*this) && Scripting::LoadGameLibrary(*this, this->game))
        {
            // Here we load the data directories from the application config. We need to do this so that the editor has access to them. Might also come
            // in handy for game code, too. Who knows.
            this->GetGlobal("Directories");
            if (this->IsTable(-1))
            {
                this->Push("Data");
                this->GetTableValue(-2);
                if (this->IsTable(-1))
                {
                    auto &dataDirectories = ApplicationConfig::GetDataDirectories();
                    for (size_t iDirectory = 0; iDirectory < dataDirectories.count; ++iDirectory)
                    {
                        this->SetTableValue(-1, iDirectory + 1, dataDirectories[iDirectory].c_str());
                    }
                }
                this->Pop(1);
            }
            this->Pop(1);


            return true;
        }
        
        return false;
    }


    bool GameScript::LoadFile(const char* fileName)
    {
        if (!this->HasFileBeenLoaded(fileName))
        {
            GTCore::String absolutePath;
            if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
            {
                this->loadedFiles.PushBack(absolutePath);
            }
        }

        return GTCore::Script::LoadFile(fileName);
    }



    bool GameScript::HasFileBeenLoaded(const char* fileName) const
    {
        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            return this->loadedFiles.Exists(absolutePath);
        }

        return false;
    }



    //////////////////////////////////////////////////////
    // ErrorHandler

    GameScript::ErrorHandler::ErrorHandler(GameScript &scriptIn)
        : script(scriptIn)
    {
    }

    void GameScript::ErrorHandler::OnError(GTCore::Script &, const char* message)
    {
        this->script.SetLastError(message);
        PostError("Script Error: %s", message);
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif