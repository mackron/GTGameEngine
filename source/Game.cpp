// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Game.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/ThreadCache.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/ApplicationConfig.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/ParticleSystemLibrary.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/GamePackager.hpp>
#include <GTEngine/GTEngine.hpp>           // For g_EngineContext. Remove this when the global context is removed.
#include <GTLib/System.hpp>
#include <GTLib/Strings/Tokenizer.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Keyboard.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior: elements of array 'array' will be default initialized
    #pragma warning(disable:4355)   // 'this' used in initialise list.
    #pragma warning(disable:4482)   // nonstandard extension used: enum used in qualified name
#endif

namespace GTEngine
{
    Game::Game(GameStateManager &gameStateManager)
        : m_gameStateManager(gameStateManager),
          isInitialised(false), closing(false),
          eventQueue(), eventQueueLock(),
          eventFilter(nullptr),
          window(nullptr), windowEventHandler(*this),
          script(*this),
          updateThread(nullptr), updateJob(*this),
          deltaTimeInSeconds(0.0), totalRunninTimeInSeconds(0.0), updateTimer(),
          guiImageManager(), gui(&script, &guiImageManager), guiEventHandler(*this), guiRenderer(), gameWindowGUIElement(nullptr),
          paused(false), focused(true),
          isCursorVisible(true),
          isAutoScriptReloadEnabled(false),
          keyDownMap(), mouseButtonDownMap(),
          editor(*this),
          profiler(),
          DebuggingGUI(*this),
          isMouseSmoothingEnabled(false),
          mouseCaptured(false), captureMouseOnReceiveFocus(false), captureMouseOnEditorClose(false), mouseCapturePosX(0), mouseCapturePosY(0),
          mouseCenterX(0), mouseCenterY(0),
          mousePosXBuffer(), mousePosYBuffer(), mousePosBufferIndex(0),
          mousePosX(0), mousePosY(0), mouseMoveLockCounter(0),
          dataFilesWatcher(), lastDataFilesWatchTime(0.0f), isDataFilesWatchingEnabled(false),
          dataFilesWatcherEventHandler(*this),
          profilerToggleKey(GTLib::Keys::F11),
          editorToggleKeyCombination(GTLib::Keys::Shift, GTLib::Keys::Tab)
    {
        // The main game window GUI element needs to be created. It is just a 100% x 100% invisible element off the root element.
        this->gui.Load("<div id='MainGameWindow' style='width:100%; height:100%' />");
        this->gameWindowGUIElement = this->gui.GetElementByID("MainGameWindow");

        this->gui.SetRenderer(this->guiRenderer);
    }

    Game::~Game()
    {
    }

    int Game::Run()
    {
        // All we do now is enter the game loop... Once this returns, we're finished with the game and we can clean up.
        this->Loop();

        // If we made it here, it means the game was run and closed normally.
        return 0;
    }

    void Game::Close()
    {
        // If the editor is open, we want to close it. We do not use this->IsEditorOpen()/this->CloseEditor() because that will
        // cause a call this this->Resume() which will then modify the position of the mouse cursor.
        if (this->editor.IsOpen())
        {
            this->editor.Close();
        }

        // There's no need to cache this event...
        this->closing = true;
    }



    void Game::SendEvent(const GameEvent &e)
    {
        this->eventQueueLock.Lock();
        {
            this->eventQueue.Push(e);
        }
        this->eventQueueLock.Unlock();
    }


    void Game::SetEventFilter(GameEventFilter* filter)
    {
        this->eventFilter = filter;
    }

    GameEventFilter* Game::GetEventFilter()
    {
        return this->eventFilter;
    }



    GTGUI::Server & Game::GetGUI()
    {
        return this->gui;
    }

    void Game::StepGUI()
    {
        this->StepGUI(this->deltaTimeInSeconds);
    }

    void Game::StepGUI(double deltaTimeInSecondsIn)
    {
        this->gui.Step(deltaTimeInSecondsIn);
        this->gui.Paint();
    }


    void Game::EnableFullscreen()
    {
        if (this->window != nullptr)
        {
            // We use GTEngine.Display.Width and GTEngine.Display.Height in determining this.
            int width  = this->script.GetInteger("GTEngine.Display.Width");
            int height = this->script.GetInteger("GTEngine.Display.Height");

            this->window->EnableFullscreen(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
            Renderer::SetCurrentWindow(this->window);
        }
    }

    void Game::DisableFullscreen()
    {
        if (this->window != nullptr)
        {
            this->window->DisableFullscreen();
            Renderer::SetCurrentWindow(this->window);
        }
    }

    bool Game::IsFullscreen() const
    {
        if (this->window != nullptr)
        {
            return this->window->IsFullscreen();
        }

        return false;
    }


    void Game::RefreshDisplay()
    {
        this->GetWindow()->SetSize(this->GetScript().GetInteger("GTEngine.Display.Width"), this->GetScript().GetInteger("GTEngine.Display.Height"));

        if (this->GetScript().GetBoolean("GTEngine.Display.Fullscreen"))
        {
            this->EnableFullscreen();
        }
        else
        {
            this->DisableFullscreen();
        }
    }


    void Game::GetMousePosition(int &x, int &y)
    {
        if (this->window != nullptr)
        {
            this->window->GetMousePosition(x, y);
        }
        else
        {
            x = 0;
            y = 0;
        }
    }


    void Game::GetMouseOffset(float &x, float &y)
    {
        if (this->mouseCaptured)
        {
            if (this->IsMouseSmoothingEnabled())
            {
                // We don't actually do a simple average over the entire buffer. Instead each value is given a weight as it's contribution towards
                // the average amount.
                float totalX        = 0.0f;
                float totalY        = 0.0f;
                float averageFactor = 0.0f;
                float iFactor       = 1.0f;

                for (size_t i = 0; i < MouseBufferSize; ++i)
                {
                    size_t bufferIndex = (this->mousePosBufferIndex - i) % MouseBufferSize;

                    totalX += static_cast<float>(this->mousePosXBuffer[bufferIndex] * iFactor);
                    totalY += static_cast<float>(this->mousePosYBuffer[bufferIndex] * iFactor);

                    averageFactor += iFactor;
                    iFactor *= MouseSmoothFactor;
                }

                x = totalX / averageFactor;
                y = totalY / averageFactor;
            }
            else
            {
                x = static_cast<float>(this->mousePosXBuffer[this->mousePosBufferIndex]);
                y = static_cast<float>(this->mousePosYBuffer[this->mousePosBufferIndex]);
            }
        }
        else
        {
            x = 0;
            y = 0;
        }
    }

    void Game::SetMousePosition(int x, int y)
    {
        if (this->window != nullptr)
        {
            this->window->SetMousePosition(x, y);
        }
    }


    void Game::EnableMouseSmoothing()
    {
        this->isMouseSmoothingEnabled = true;
    }

    void Game::DisableMouseSmoothing()
    {
        this->isMouseSmoothingEnabled = false;
    }

    bool Game::IsMouseSmoothingEnabled() const
    {
        return this->isMouseSmoothingEnabled;
    }


    void Game::CaptureMouse()
    {
        if (!this->mouseCaptured)
        {
            this->HideCursor();
            this->mouseCaptured = true;
            this->GetMousePosition(this->mouseCapturePosX, this->mouseCapturePosY);

            // The mouse needs to be moved to the middle of the window.
            unsigned int windowWidth, windowHeight;
            this->window->GetSize(windowWidth, windowHeight);

            this->mouseCenterX = (int)windowWidth  / 2;
            this->mouseCenterY = (int)windowHeight / 2;
            this->SetMousePosition(this->mouseCenterX, this->mouseCenterY);


            this->gui.MarkMouseAsCaptured();
        }
    }

    void Game::ReleaseMouse()
    {
        if (this->mouseCaptured)
        {
            this->ShowCursor();
            this->mouseCaptured = false;

            // Mouse position needs to be restored.
            this->SetMousePosition(this->mouseCapturePosX, this->mouseCapturePosY);

            // The GUI needs to know about this.
            this->gui.SetMousePosition(this->mouseCapturePosX, this->mouseCapturePosY);
            this->gui.UnmarkMouseAsCaptured();
        }
    }

    void Game::ShowCursor()
    {
        if (!this->isCursorVisible)
        {
            this->window->ShowCursor();
            this->isCursorVisible = true;
        }
    }

    void Game::HideCursor()
    {
        if (this->isCursorVisible)
        {
            this->window->HideCursor();
            this->isCursorVisible = false;
        }
    }


    bool Game::IsKeyDown(GTLib::Key key) const
    {
        if (this->focused)  // Keys will never be down if we're not focused...
        {
            auto iKey = this->keyDownMap.Find(key);
            if (iKey != nullptr)
            {
                return iKey->value;
            }
        }

        return false;
    }

    bool Game::IsMouseButtonDown(GTLib::MouseButton button) const
    {
        if (this->focused)  // Keys will never be down if we're not focused...
        {
            auto iButton = this->mouseButtonDownMap.Find(button);
            if (iButton != nullptr)
            {
                return iButton->value;
            }
        }

        return false;
    }


    void Game::CacheMousePosition()
    {
        if (this->mouseCaptured)
        {
            int offsetX, offsetY;
            this->GetMousePosition(offsetX, offsetY);

            offsetX -= this->mouseCenterX;
            offsetY -= this->mouseCenterY;

            this->mousePosXBuffer[this->mousePosBufferIndex] = offsetX;
            this->mousePosYBuffer[this->mousePosBufferIndex] = offsetY;
            this->mousePosBufferIndex = (this->mousePosBufferIndex + 1) % MouseBufferSize;

            this->SetMousePosition(this->mouseCenterX, this->mouseCenterY);
        }
    }


    void Game::Pause()
    {
        if (!this->paused)
        {
            this->paused = true;
            m_gameStateManager.OnPause(*this);

            this->PostScriptEvent_OnPause();
        }
    }

    void Game::Resume()
    {
        if (this->paused)
        {
            this->paused = false;
            m_gameStateManager.OnResume(*this);

            this->PostScriptEvent_OnResume();
        }
    }


    void Game::OpenEditor()
    {
        //CALLGRIND_ZERO_STATS;
        //__itt_resume();
        if (m_gameStateManager.OnEditorOpening(*this))
        {
            // The main game window GUI element needs to be hidden.
            this->gameWindowGUIElement->Hide();


            // The in-game profiling GUI needs to be hidden when the editor is open.
            this->DebuggingGUI.Hide();

            // We will also enable the profiler when the editor is open, because why not?
            this->profiler.Enable();


            // The game is always paused while the editor is running.
            this->Pause();

            // The mouse must be released.
            this->captureMouseOnEditorClose = this->IsMouseCaptured();
            this->ReleaseMouse();


            if (!this->editor.IsStarted())
            {
                this->editor.Startup(this->gui);
            }

            this->editor.Open();

            m_gameStateManager.OnEditorOpen(*this);
        }
        //CALLGRIND_STOP_INSTRUMENTATION;
        //__itt_pause();
    }

    void Game::CloseEditor()
    {
        if (m_gameStateManager.OnEditorClosing(*this))
        {
            this->editor.Close();

            // The main game window GUI element needs to be shown.
            this->gameWindowGUIElement->Show();

            // We can now unpause the game.
            this->Resume();

            // If the mouse was captured before we opened the editor, we'll want to re-capture it.
            if (this->captureMouseOnEditorClose)
            {
                this->CaptureMouse();
            }


            if (this->DebuggingGUI.isShowing)
            {
                this->profiler.Enable();
                this->DebuggingGUI.Show();
            }
            else
            {
                this->profiler.Disable();
            }

            m_gameStateManager.OnEditorClose(*this);
        }
    }

    bool Game::IsEditorOpen() const
    {
        return this->editor.IsOpen();
    }


    void Game::ShowDebugging()
    {
        // If we haven't yet initialised the debugging GUI, we need to do it.
        if (!this->IsEditorOpen())
        {
            if (!DebuggingGUI.isInitialised)
            {
                this->DebuggingGUI.Initialise(this->gui);
            }

            if (this->DebuggingGUI.DebuggingMain != nullptr)
            {
                this->DebuggingGUI.DebuggingMain->Show();
                this->DebuggingGUI.isShowing = true;
            }

            this->profiler.Enable();
        }
    }

    void Game::HideDebugging()
    {
        if (!this->IsEditorOpen())
        {
            if (this->DebuggingGUI.DebuggingMain != nullptr)
            {
                this->DebuggingGUI.DebuggingMain->Hide();
                this->DebuggingGUI.isShowing = false;
            }

            this->profiler.Disable();
        }
    }

    bool Game::IsDebuggingOpen() const
    {
        return this->DebuggingGUI.isShowing;
    }


    bool Game::ExecuteScript(const char* scriptIn)
    {
        return this->script.Execute(scriptIn);
    }




    void Game::EnableScriptAutoReload()
    {
        this->isAutoScriptReloadEnabled = true;
    }

    void Game::DisableScriptAutoReload()
    {
        this->isAutoScriptReloadEnabled = false;
    }



    bool Game::PackageForDistribution(const char* outputDirectory, const char* executableName)
    {
        GTLib::Path absoluteOutputDirectory(g_EngineContext->GetExecutableDirectoryAbsolutePath());
        absoluteOutputDirectory.Append(outputDirectory);

        // We will start by creating the output directory.
        if (!easyvfs_is_existing_directory(g_EngineContext->GetVFS(), absoluteOutputDirectory.c_str()))
        {
            if (!easyvfs_mkdir(g_EngineContext->GetVFS(), absoluteOutputDirectory.c_str()))
            {
                // Failed to create the output directory.
                return false;
            }
        }

        GTEngine::GamePackager packager(absoluteOutputDirectory.c_str());


        // We will start by copying over the data directories.
        auto &absoluteDataDirectories = g_EngineContext->GetApplicationConfig().GetDataDirectories();
        {
            for (size_t iDataDirectory = 0; iDataDirectory < absoluteDataDirectories.count; ++iDataDirectory)
            {
                packager.CopyDataDirectory(absoluteDataDirectories[iDataDirectory].c_str());
            }
        }

        if (GTLib::Path::ExtensionEqual(g_EngineContext->GetExecutableAbsolutePath(), "exe"))
        {
            if (GTLib::Path::ExtensionEqual(executableName, "exe"))
            {
                packager.CopyExecutable(g_EngineContext->GetExecutableAbsolutePath(), executableName);
            }
            else
            {
                packager.CopyExecutable(g_EngineContext->GetExecutableAbsolutePath(), (GTLib::String(executableName) + ".exe").c_str());
            }
        }
        else
        {
            packager.CopyExecutable(g_EngineContext->GetExecutableAbsolutePath(), executableName);
        }

        packager.WriteConfig();

        return true;
    }



    void Game::OnFileInsert(const DataFilesWatcher::Item &item)
    {
        (void)item;
    }

    void Game::OnFileRemove(const DataFilesWatcher::Item &item)
    {
        (void)item;
    }

    void Game::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
        // If the file is an asset, we need to update everything that is using it. We do this via the asset libraries.
        if ((item.info.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            // It's not a directory.

            auto extension = GTLib::Path::Extension(item.info.absolutePath);

            if (ModelLibrary::IsExtensionSupported(extension))
            {
                ModelLibrary::Reload(item.info.absolutePath);
            }
            else if (Texture2DLibrary::IsExtensionSupported(extension))
            {
                Texture2DLibrary::Reload(item.info.absolutePath);
            }
            else if (GT::IsSupportedMaterialExtension(item.info.absolutePath))
            {
                MaterialLibrary::Reload(item.info.absolutePath);
            }
            else if (GT::IsSupportedParticleSystemExtension(item.info.absolutePath))
            {
                ParticleSystemLibrary::Reload(item.info.absolutePath);
            }
            else
            {
                // It might be a script file. We'll try reloading.
                ScriptLibrary::Reload(item.info.absolutePath);

                // If we have a script file we will reload it if applicable.
                if (this->IsScriptAutoReloadEnabled())
                {
                    if (this->script.HasFileBeenLoaded(item.info.absolutePath))
                    {
                        this->script.ExecuteFile(g_EngineContext->GetVFS(), item.info.absolutePath);
                    }
                }
            }
        }
    }


    GTLib::Key Game::GetProfilerToggleKey() const
    {
        return this->profilerToggleKey;
    }

    void Game::SetProfilerToggleKey(GTLib::Key key)
    {
        this->profilerToggleKey = key;
    }


    const GTLib::KeyCombination & Game::GetEditorToggleKeyCombination() const
    {
        return this->editorToggleKeyCombination;
    }

    void Game::SetEditorToggleKeyCombination(const GTLib::KeyCombination &newCombination)
    {
        this->editorToggleKeyCombination = newCombination;
    }




    Component* Game::CreateCustomComponent(const char*, SceneNode &) const
    {
        return nullptr;
    }



    /////////////////////////////////////
    // Game State Management

    bool Game::SerializeGameState(GTLib::Serializer &serializer)
    {
        return m_gameStateManager.Serialize(*this, serializer);
    }

    bool Game::DeserializeGameState(GTLib::Deserializer &deserializer)
    {
        return m_gameStateManager.Deserialize(*this, deserializer);
    }

    bool Game::SaveGameState(const char* destinationFilePath)
    {
        easyvfs_file* pFile = easyvfs_open(g_EngineContext->GetVFS(), destinationFilePath, EASYVFS_WRITE | EASYVFS_CREATE_DIRS, 0);
        if (pFile != nullptr)
        {
            GTLib::FileSerializer serializer(pFile);
            bool result = this->SerializeGameState(serializer);

            easyvfs_close(pFile);
            return result;
        }

        return false;
    }

    bool Game::LoadGameState(const char* sourceFilePath)
    {
        easyvfs_file* pFile = easyvfs_open(g_EngineContext->GetVFS(), sourceFilePath, EASYVFS_READ, 0);
        if (pFile != nullptr)
        {
            GTLib::FileDeserializer deserializer(pFile);
            bool result = this->DeserializeGameState(deserializer);

            easyvfs_close(pFile);
            return result;
        }

        return false;
    }

    bool Game::LoadScene(const char* sceneRelativePath)
    {
        return m_gameStateManager.LoadScene(*this, sceneRelativePath);
    }


    bool GameCommandLineProc(const char* key, const char* value, void* pUserData)
    {
        Game* pGame = reinterpret_cast<Game*>(pUserData);
        assert(pGame != nullptr);

        if (strcmp(key, "config") == 0) {
            pGame->GetScript().ExecuteFile(g_EngineContext->GetVFS(), value);
            return true;
        }

        return true;
    }


    bool Game::Startup()
    {
        // The first thing we do is load up the scripting environment. We do this first because it will contain configuration properties
        // for things later on.
        if (this->script.Startup())
        {
            // We give the game an opportunity to load configs before processing --config arguments.
            m_gameStateManager.OnLoadConfigs(*this);

            // This is where the user config scripts are loaded.
            easyutil_parse_cmdline(&g_EngineContext->GetCommandLine(), GameCommandLineProc, this);

#if 0
            const char** cmdLine_config = commandLine.GetArgument("config");
            if (cmdLine_config != nullptr)
            {
                for (int i = 0; cmdLine_config[i] != nullptr; ++i)
                {
                    this->script.ExecuteFile(cmdLine_config[i]);
                }
            }
#endif


            // Here we will set the default anistropy for textures via the texture library.
            Texture2DLibrary::SetDefaultAnisotropy(static_cast<unsigned int>(this->script.GetInteger("GTEngine.Display.Textures.Anisotropy")));


            // First we need a window. Note that we don't show it straight away.
            this->window = Renderer::CreateWindow();
            if (this->window != nullptr)
            {
                // We'll need to grab the update thread object. We grab this from the thread cache which will have been initialised
                // in GTEngine::Startup(). It's important that we have a thread here, so we need to force it (first argument = true).
                this->updateThread = ThreadCache::AcquireThread(true);


                // We'll want to set a few window properties before showing it... We want to show the window relatively early to make
                // the game feel a little bit more speedy, even though it's not really.
                this->window->SetTitle("GTEngine Game");
                this->window->SetSize(this->script.GetInteger("GTEngine.Display.Width"), this->script.GetInteger("GTEngine.Display.Height"));

                // Now we can set the window's event handler and show it.
                this->window->SetEventHandler(this->windowEventHandler);
                this->window->Show();


                // Here we initialise the GUI. We need a font server for this, so it needs to be done after initialising fonts.
                Log("Loading GUI...");
                if (!this->InitialiseGUI())
                {
                    Log("Error loading GUI.");
                }


                // Now we initialise the object that will watch the data directory.
                Log("Loading Files Watcher...");
                if (!this->InitialiseDataFilesWatcher())
                {
                    Log("Error starting up files watcher.");
                }


                // Here is where we let the game object do some startup stuff.
                if (m_gameStateManager.OnStartup(*this))
                {
                    this->script.Execute("Game.OnStartup();");
                    return true;
                }

                // OnStartup() has returned false somewhere.
                return false;
            }
            else
            {
                // We couldn't create a window, which means the renderer is not usable...
                GTEngine::PostError("Error initialising renderer.");
            }
        }
        else
        {
            GTEngine::PostError("Error initialising scripting environment.");
        }

        return false;
    }


    bool Game::InitialiseGUI()
    {
        this->gui.SetEventHandler(this->guiEventHandler);
        return true;
    }

    bool Game::InitialiseDataFilesWatcher()
    {
        auto &directories = g_EngineContext->GetApplicationConfig().GetDataDirectories();
        if (directories.count > 0)
        {
            for (size_t i = 0; i < directories.count; ++i)
            {
                this->dataFilesWatcher.AddRootDirectory(directories[i].c_str());
            }
        }

        this->dataFilesWatcher.AddEventHandler(this->dataFilesWatcherEventHandler);

        return true;
    }


    void Game::Shutdown()
    {
        // The data files watcher might be running. We better wait for it to finish.
        this->dataFilesWatcher.__Deactivate();          // <-- This will cause the background thread to die quicker if it happens to be running.
        this->dataFilesWatcher.WaitForEvents();         // <-- This will make sure the background thread has finished.


        // We first let the game know that we are shutting down. It's important that we do this before killing anything.
        m_gameStateManager.OnShutdown(*this);
        this->script.Execute("Game.OnShutdown();");     // <-- TODO: Don't use this inline style calling. Instead, properly call it through the C++ API.


        delete this->window;


        ThreadCache::UnacquireThread(this->updateThread);


        // NOTE: Got a random crash here. I think it was the destructor of the window. I recently added garbage collection for renderers, so perhaps
        //       there is a sync issue with that. My first thought is that maybe the renderer was destructing rendering resources or something... don't know for sure.
    }

    void Game::Loop()
    {
        while (!this->closing)
        {
            // If the profiler is enabled, we're going to post some events to it.
            if (this->profiler.IsEnabled())
            {
                this->profiler.OnBeginFrame();
            }



            // First we need to handle any pending window messages. We do not want to wait here (first argument).
            while (GTLib::PumpNextWindowEvent(false));


            // If we're watching the data directories, we want to check for changes now.
            if (this->IsDataFilesWatchingEnabled())
            {
                float checkInterval = this->GetDataFilesWatchInterval();

                if (GTLib::Timing::GetTimeInSeconds() - this->lastDataFilesWatchTime >= checkInterval)
                {
                    if (this->dataFilesWatcher.EventsReady())
                    {
                        this->dataFilesWatcher.DispatchEvents(false);       // <-- 'false' means to not wait.
                        this->dataFilesWatcher.CheckForChanges(true);       // <-- 'true' means to go asynchronous.
                    }

                    this->lastDataFilesWatchTime = static_cast<float>(GTLib::Timing::GetTimeInSeconds());
                }
            }


            // We want our events to be handled synchronously on the main thread.
            this->HandleEvents();


            // TODO: Remove this little section when the new GUI event handling system is in place.
            // The GUI events can be handled here. If it turns out that they're best handled on the update thread, all we need
            // to do is remove this line - GTGUI::Server::Step() will also handle any pending events. I like to handle the GUI
            // events from here because it's nice to have GUI events handled at the same time as window events, since they're
            // kind of related.
            //this->gui.HandleEvents();


            // Here we want to start the next frame. Everything between StartFrame() and EndFrame() will be executed at the same
            // time as the update thread.
            this->StartFrame();     // <-- starts any applicable threads.
            {
                this->Draw();
            }
            this->EndFrame();       // <-- blocks until all threads are finished.


            // Here we need to update the profiler.
            if (this->profiler.IsEnabled())
            {
                this->profiler.OnEndFrame();
            }
        }
    }

    void Game::StartFrame() // [Main Thread]
    {
        // The first thing we do is retrieve the delta time...
        this->deltaTimeInSeconds = GTLib::Min(this->updateTimer.Update(), 1.0);

        // We also need to increment the total running time, but only if we're not paused.
        if (!this->IsPaused())
        {
            this->totalRunninTimeInSeconds += this->deltaTimeInSeconds;
        }


        // We need to cache the current position of the mouse. We need to do this in order to get smooth mouse movement.
        this->CacheMousePosition();

        // Here is where we swap rendering command queues in preperation for the scene update, which is about to re-fill the back queue.
        this->SwapRCQueues();


        // Now we let the game know that we're starting the frame.
        m_gameStateManager.OnStartFrame(*this);

        // Now we just run the job without attempting to block (second argument).
        this->updateThread->Start(this->updateJob, false);
    }

    void Game::EndFrame() // [Main Thread]
    {
        // First we need to block until all threads have finished executing...
        this->updateThread->Wait();

        // Now we can let the game know that we've finished the frame...
        m_gameStateManager.OnEndFrame(*this);
    }

    void Game::Update() //[Update Thread]
    {
        if (this->profiler.IsEnabled())
        {
            this->profiler.OnBeginUpdate();
        }


        // If the debugging overlay is open, we need to show the debugging information.
        if (this->IsDebuggingOpen())
        {
            this->DebuggingGUI.Update(this->profiler);
        }


        // If the editor is open it also needs to be updated.
        if (this->editor.IsOpen())
        {
            this->editor.Update(this->deltaTimeInSeconds);
        }

        // The game needs to know that we're updating...
        m_gameStateManager.OnUpdate(*this, this->deltaTimeInSeconds);
        this->PostScriptEvent_OnUpdate(this->deltaTimeInSeconds);


        // We will step the GUI after updating the game. This will call rendering functions.
        this->StepGUI(this->deltaTimeInSeconds);


        if (this->profiler.IsEnabled())
        {
            this->profiler.OnEndUpdate();
        }
    }

    void Game::Draw() //[Main Thread]
    {
        if (this->profiler.IsEnabled())
        {
            this->profiler.OnBeginRendering();
        }


        // NOTE:
        //
        // We're not currently calling any OnDraw events. The problem is with the multi-threading nature of the engine. Events here are called from a different thread
        // to other events, so it's not a trivial matter of simply calling the function without any synchronization.


        //this->OnDraw();
        //this->script.Execute("Game.OnDraw();");

        Renderer::ExecuteCallCache();

        //this->OnPostDraw();
        //this->script.Execute("Game.OnPostDraw();");


        // At this point we can finally swap the buffers.
        Renderer::SwapBuffers();


        if (this->profiler.IsEnabled())
        {
            this->profiler.OnEndRendering();
        }
    }

    void Game::SwapRCQueues()           // TODO: Consider renaming this to SwapCallCaches() to match the new renderer.
    {
        Renderer::SwapCallCaches();
    }


    bool Game::IsKeyCombinationDown(const GTLib::KeyCombination &combination) const
    {
        // We ignore keys that are set to null, but we need to always return false if all of them are null.
        if (!combination.IsAnyKeySet())
        {
            return false;
        }


        bool down = true;

        down = down && (combination.systemKey1    == GTLib::Keys::Null || this->IsKeyDown(combination.systemKey1));
        down = down && (combination.systemKey2    == GTLib::Keys::Null || this->IsKeyDown(combination.systemKey2));
        down = down && (combination.printableKey1 == GTLib::Keys::Null || this->IsKeyDown(combination.printableKey1));
        down = down && (combination.printableKey2 == GTLib::Keys::Null || this->IsKeyDown(combination.printableKey2));

        return down;
    }


    void Game::HandleEvents()
    {
        GTGUI::EventContext eventContext = this->gui.BeginPostingEvents();

        GameEvent e;
        while (this->eventQueue.Next(e))
        {
            switch (e.code)
            {
            case EventCodes::Unknown: break;

            case EventCodes::OnClose:
                {
                    this->Close();
                    break;
                }

            case EventCodes::OnSize:                      this->HandleEvent_OnSize(e);                                  break;
            case EventCodes::OnMouseMove:                 this->HandleEvent_OnMouseMove(e, eventContext);               break;
            case EventCodes::OnMouseWheel:                this->HandleEvent_OnMouseWheel(e, eventContext);              break;
            case EventCodes::OnMouseButtonDown:           this->HandleEvent_OnMouseButtonDown(e, eventContext);         break;
            case EventCodes::OnMouseButtonUp:             this->HandleEvent_OnMouseButtonUp(e, eventContext);           break;
            case EventCodes::OnMouseButtonDoubleClick:    this->HandleEvent_OnMouseButtonDoubleClick(e, eventContext);  break;
            case EventCodes::OnKeyPressed:                this->HandleEvent_OnKeyPressed(e, eventContext);              break;
            case EventCodes::OnKeyReleased:               this->HandleEvent_OnKeyReleased(e, eventContext);             break;
            case EventCodes::OnKeyDown:                   this->HandleEvent_OnKeyDown(e, eventContext);                 break;
            case EventCodes::OnKeyUp:                     this->HandleEvent_OnKeyUp(e, eventContext);                   break;
            case EventCodes::OnReceiveFocus:              this->HandleEvent_OnReceiveFocus(e);                          break;
            case EventCodes::OnLoseFocus:                 this->HandleEvent_OnLoseFocus(e);                             break;

            // Any generic events are posted as an event to the game so that an application can handle it itself.
            default:
                {
                    m_gameStateManager.OnHandleEvent(*this, e);
                    break;
                }
            }
        }

        this->gui.EndPostingEvents(eventContext);
    }

    void Game::HandleEvent_OnSize(GameEvent &e)
    {
        this->gui.SetViewportSize(e.size.width, e.size.height);

        if (this->eventFilter == nullptr || this->eventFilter->OnSize(e.size.width, e.size.height))
        {
            m_gameStateManager.OnSize(*this, e.size.width, e.size.height);
            this->PostScriptEvent_OnSize(e);
        }
    }

    void Game::HandleEvent_OnMouseMove(GameEvent &e, GTGUI::EventContext eventContext)
    {
        this->mousePosX = e.mousemove.x;
        this->mousePosY = e.mousemove.y;



        // If we're captured and blocking, we don't want to post anything.
        if (this->mouseMoveLockCounter == 0)
        {
            this->gui.OnMouseMove(eventContext, e.mousemove.x, e.mousemove.y);


            // We don't post mouse events if the mouse is captured.
            if (!this->mouseCaptured)
            {
                if (this->eventFilter == nullptr || this->eventFilter->OnMouseMove(e.mousemove.x, e.mousemove.y))
                {
                    m_gameStateManager.OnMouseMove(*this, e.mousemove.x, e.mousemove.y);
                    this->PostScriptEvent_OnMouseMove(e);
                }
            }
        }
        else
        {
            --this->mouseMoveLockCounter;
        }
    }

    void Game::HandleEvent_OnMouseWheel(GameEvent &e, GTGUI::EventContext eventContext)
    {
        this->gui.OnMouseWheel(eventContext, e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);

        if (this->eventFilter == nullptr || this->eventFilter->OnMouseWheel(e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y))
        {
            m_gameStateManager.OnMouseWheel(*this, e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);
            this->PostScriptEvent_OnMouseWheel(e);
        }
    }

    void Game::HandleEvent_OnMouseButtonDown(GameEvent &e, GTGUI::EventContext eventContext)
    {
        this->mouseButtonDownMap.Add(e.mousedown.button, true);

        this->gui.OnMouseButtonDown(eventContext, static_cast<int>(e.mousedown.button));



        // If the editor is open, we'll want to post this event to that too.
        if (this->editor.IsOpen())
        {
            this->editor.OnMouseButtonDown(e.mousedown.button, e.mousedown.x, e.mousedown.y);
        }


        if (this->eventFilter == nullptr || this->eventFilter->OnMouseButtonDown(e.mousedown.button, e.mousedown.x, e.mousedown.y))
        {
            m_gameStateManager.OnMouseButtonDown(*this, e.mousedown.button, e.mousedown.x, e.mousedown.y);
            this->PostScriptEvent_OnMouseButtonDown(e);
        }
    }

    void Game::HandleEvent_OnMouseButtonUp(GameEvent &e, GTGUI::EventContext eventContext)
    {
        auto iButtonDown = this->mouseButtonDownMap.Find(e.mouseup.button);
        if (iButtonDown != nullptr)
        {
            iButtonDown->value = false;
        }


        this->gui.OnMouseButtonUp(eventContext, static_cast<int>(e.mouseup.button));


        // If the editor is open, we'll want to post this event to that too.
        if (this->editor.IsOpen())
        {
            this->editor.OnMouseButtonUp(e.mouseup.button, e.mouseup.x, e.mouseup.y);
        }


        if (this->eventFilter == nullptr || this->eventFilter->OnMouseButtonUp(e.mouseup.button, e.mouseup.x, e.mouseup.y))
        {
            m_gameStateManager.OnMouseButtonUp(*this, e.mouseup.button, e.mouseup.x, e.mouseup.y);
            this->PostScriptEvent_OnMouseButtonUp(e);
        }
    }

    void Game::HandleEvent_OnMouseButtonDoubleClick(GameEvent &e, GTGUI::EventContext eventContext)
    {
        this->gui.OnMouseButtonDoubleClick(eventContext, static_cast<int>(e.mousedoubleclick.button));


        if (this->eventFilter == nullptr || this->eventFilter->OnMouseButtonDoubleClick(e.mousedoubleclick.button, e.mousedoubleclick.x, e.mousedoubleclick.y))
        {
            m_gameStateManager.OnMouseButtonDoubleClick(*this, e.mousedoubleclick.button, e.mousedoubleclick.x, e.mousedoubleclick.y);
            this->PostScriptEvent_OnMouseButtonDoubleClick(e);
        }
    }

    void Game::HandleEvent_OnKeyPressed(GameEvent &e, GTGUI::EventContext eventContext)
    {
        this->keyDownMap.Add(e.keypressed.key, true);

        this->gui.OnKeyPressed(eventContext, e.keypressed.key);


        if (this->eventFilter == nullptr || this->eventFilter->OnKeyPressed(e.keypressed.key))
        {
            m_gameStateManager.OnKeyPress(*this, e.keypressed.key);
            this->PostScriptEvent_OnKeyPressed(e);
        }



        // Editor.
        if (this->editor.IsOpen())
        {
            this->editor.OnKeyPressed(e.keypressed.key);
        }

        if (this->editorToggleKeyCombination.IsPrintableKey(e.keypressed.key) && this->IsKeyCombinationDown(this->editorToggleKeyCombination))
        {
            if (!this->IsEditorOpen())
            {
                this->OpenEditor();
            }
            else
            {
                this->CloseEditor();
            }
        }

        // Profiler.
        if (e.keypressed.key == this->profilerToggleKey)
        {
            if (!this->IsDebuggingOpen())
            {
                this->ShowDebugging();
            }
            else
            {
                this->HideDebugging();
            }
        }
    }

    void Game::HandleEvent_OnKeyReleased(GameEvent &e, GTGUI::EventContext eventContext)
    {
        (void)eventContext;

        auto iKeyDown = this->keyDownMap.Find(e.keyreleased.key);
        if (iKeyDown != nullptr)
        {
            iKeyDown->value = false;
        }


        if (this->eventFilter == nullptr || this->eventFilter->OnKeyReleased(e.keyreleased.key))
        {
            m_gameStateManager.OnKeyRelease(*this, e.keyreleased.key);
            this->PostScriptEvent_OnKeyReleased(e);
        }


        if (this->editor.IsOpen())
        {
            this->editor.OnKeyReleased(e.keypressed.key);
        }
    }

    void Game::HandleEvent_OnKeyDown(GameEvent &e, GTGUI::EventContext eventContext)
    {
        this->gui.OnKeyDown(eventContext, e.keydown.key);


        if (this->eventFilter == nullptr || this->eventFilter->OnKeyDown(e.keydown.key))
        {
            m_gameStateManager.OnKeyDown(*this, e.keydown.key);
            this->PostScriptEvent_OnKeyDown(e);
        }
    }

    void Game::HandleEvent_OnKeyUp(GameEvent &e, GTGUI::EventContext eventContext)
    {
        this->gui.OnKeyUp(eventContext, e.keyup.key);


        if (this->eventFilter == nullptr || this->eventFilter->OnKeyUp(e.keyup.key))
        {
            m_gameStateManager.OnKeyUp(*this, e.keyup.key);
            this->PostScriptEvent_OnKeyUp(e);
        }
    }

    void Game::HandleEvent_OnReceiveFocus(GameEvent &e)
    {
        this->focused = true;

        // If we're watching data files, we're going to check and update right now. This is useful for toggling between the editor and the other application.
        if (this->IsDataFilesWatchingEnabled() || this->IsScriptAutoReloadEnabled())
        {
            this->dataFilesWatcher.CheckForChanges(false);
            this->dataFilesWatcher.DispatchEvents();
        }

        if (this->captureMouseOnReceiveFocus)
        {
            this->CaptureMouse();
            this->captureMouseOnReceiveFocus = false;
        }

        if (this->editor.IsOpen())
        {
            this->editor.OnReceiveFocus();
        }


        if (this->eventFilter == nullptr || this->eventFilter->OnReceiveFocus())
        {
            m_gameStateManager.OnReceiveFocus(*this);
            this->PostScriptEvent_OnReceiveFocus(e);
        }
    }

    void Game::HandleEvent_OnLoseFocus(GameEvent &e)
    {
        this->focused = false;

        if (this->editor.IsOpen())
        {
            this->editor.OnLoseFocus();
        }

        // We need to post key released/up events for any key that is currently down.
        for (size_t i = 0; i < this->keyDownMap.count; ++i)
        {
            auto iKey = this->keyDownMap.buffer[i];
            assert(iKey != nullptr);

            if (iKey->value)
            {
                auto eventContext = this->gui.BeginPostingEvents();
                {
                    GameEvent e2;

                    e2.code            = EventCodes::OnKeyReleased;
                    e2.keyreleased.key = iKey->key;
                    this->HandleEvent_OnKeyReleased(e2, eventContext);

                    e2.code      = EventCodes::OnKeyUp;
                    e2.keyup.key = iKey->key;
                    this->HandleEvent_OnKeyUp(e2, eventContext);
                }
                this->gui.EndPostingEvents(eventContext);

                iKey->value = false;
            }
        }

        this->DisableFullscreen();

        // We'll need to release the mouse.
        this->captureMouseOnReceiveFocus = this->IsMouseCaptured();
        this->ReleaseMouse();


        if (this->eventFilter == nullptr || this->eventFilter->OnLoseFocus())
        {
            m_gameStateManager.OnLoseFocus(*this);
            this->PostScriptEvent_OnLoseFocus(e);
        }
    }


    void Game::PostScriptEvent_OnUpdate(double deltaTimeInSecondsIn)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnUpdate");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();
                this->script.Push("deltaTimeInSeconds");
                this->script.Push(deltaTimeInSecondsIn);
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnSize(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnSize");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("width");
                this->script.Push(static_cast<int>(e.size.width));
                this->script.SetTableValue(-3);

                this->script.Push("height");
                this->script.Push(static_cast<int>(e.size.height));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnMouseMove(const GameEvent &e)
    {
        Scripting::PostEvent_OnMouseMove(this->script, e.mousemove.x, e.mousemove.y);
    }

    void Game::PostScriptEvent_OnMouseWheel(const GameEvent &e)
    {
        Scripting::PostEvent_OnMouseWheel(this->script, e.mousewheel.x, e.mousewheel.y, e.mousewheel.delta);
    }

    void Game::PostScriptEvent_OnMouseButtonDown(const GameEvent &e)
    {
        Scripting::PostEvent_OnMouseButtonDown(this->script, e.mousedown.x, e.mousedown.y, e.mousedown.button);
    }

    void Game::PostScriptEvent_OnMouseButtonUp(const GameEvent &e)
    {
        Scripting::PostEvent_OnMouseButtonUp(this->script, e.mouseup.x, e.mouseup.y, e.mouseup.button);
    }

    void Game::PostScriptEvent_OnMouseButtonDoubleClick(const GameEvent &e)
    {
        Scripting::PostEvent_OnMouseButtonDoubleClick(this->script, e.mousedoubleclick.x, e.mousedoubleclick.y, e.mousedoubleclick.button);
    }

    void Game::PostScriptEvent_OnKeyPressed(const GameEvent &e)
    {
        Scripting::PostEvent_OnKeyPressed(this->script, e.keypressed.key);
    }

    void Game::PostScriptEvent_OnKeyReleased(const GameEvent &e)
    {
        Scripting::PostEvent_OnKeyReleased(this->script, e.keyreleased.key);
    }

    void Game::PostScriptEvent_OnKeyDown(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnKeyDown");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("key");
                this->script.Push(static_cast<int>(e.keydown.key));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnKeyUp(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnKeyUp");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("key");
                this->script.Push(static_cast<int>(e.keyup.key));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnReceiveFocus(const GameEvent &)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnReceiveFocus");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.Call(0, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnLoseFocus(const GameEvent &)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnLoseFocus");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.Call(0, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnPause()
    {
        Scripting::PostEvent_OnGamePause(this->script);
    }

    void Game::PostScriptEvent_OnResume()
    {
        Scripting::PostEvent_OnGameResume(this->script);
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
