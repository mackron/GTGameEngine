// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Scripting.hpp>
#include <GTGE/GTEngine.hpp>           // For g_Context. Remove this when the global context is removed.

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GT
{
    GameScript::GameScript(Context &context)
        : context(context), lastError(), loadedFiles(), errorHandler(*this)
    {
        this->AttachErrorHandler(errorHandler);

        // Load the core library.
        //
        // TODO: Move this into Startup() so we can handle errors properly. Can't do it right now because the GUI system depends on this being
        //       initialized during the constructor.
        GT::LoadGTCoreScriptLibrary(*this);
    }

    GameScript::~GameScript()
    {
    }

    bool GameScript::Startup()
    {
        // First we load the GTEngine scripting stuff.
        if (GT::LoadGTEngineScriptLibrary(*this) && GT::LoadGameLibrary(*this, this->context))
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
                    assert(drvfs_get_base_directory_count(g_Context->GetVFS()) > 0);
                    for (unsigned int iBaseDir = 0; iBaseDir < drvfs_get_base_directory_count(g_Context->GetVFS()) - 1; ++iBaseDir)     // -1 because we want to ignore the executable directory.
                    {
                        this->SetTableValue(-1, iBaseDir + 1, drvfs_get_base_directory_by_index(g_Context->GetVFS(), iBaseDir));
                    }
                }
                this->Pop(1);
            }
            this->Pop(1);


            return true;
        }
        
        return false;
    }


    bool GameScript::LoadFile(drvfs_context* pVFS, const char* fileName)
    {
        if (!this->HasFileBeenLoaded(fileName))
        {
            char absolutePath[DRVFS_MAX_PATH];
            if (drvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
            {
                this->loadedFiles.PushBack(absolutePath);
            }
        }

        return GT::Script::LoadFile(pVFS, fileName);
    }



    bool GameScript::HasFileBeenLoaded(const char* fileName) const
    {
        char absolutePath[DRVFS_MAX_PATH];
        if (drvfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
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

    void GameScript::ErrorHandler::OnError(GT::Script &, const char* message)
    {
        this->script.SetLastError(message);
        g_Context->LogErrorf("Script Error: %s", message);
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
