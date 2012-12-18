
#include <GTEngine/Game.hpp>
#include <GTEngine/FontManager.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/GarbageCollector.hpp>
#include <GTEngine/ThreadCache.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/ApplicationConfig.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTCore/System.hpp>
#include <GTCore/Strings/Tokenizer.hpp>
#include <GTCore/String.hpp>
#include <GTCore/CommandLine.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/Keyboard.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior: elements of array 'array' will be default initialized
    #pragma warning(disable:4355)   // 'this' used in initialise list.
    #pragma warning(disable:4482)   // nonstandard extension used: enum used in qualified name
#endif

namespace GTEngine
{
    Game::Game()
        : isInitialised(false), closing(false), eventQueue(), eventQueueLock(),
          window(nullptr), windowEventHandler(*this),
          script(*this),
          updateThread(nullptr), updateJob(*this),
          deltaTimeInSeconds(0.0), totalRunninTimeInSeconds(0.0), updateTimer(),
          fontServer("var/fonts.cache"), defaultFont(nullptr),
          gui(&script, &fontServer), guiEventHandler(*this),
          paused(false), focused(true),
          isCursorVisible(true),
          isAutoScriptReloadEnabled(false),
          keyDownMap(), mouseButtonDownMap(),
          editor(*this),
          profiler(),
          DebuggingGUI(*this),
          mouseCaptured(false), mouseCapturePosX(0), mouseCapturePosY(0),
          mouseCenterX(0), mouseCenterY(0),
          mousePosXBuffer(), mousePosYBuffer(), mousePosBufferIndex(0),
          dataFilesWatcher(), lastDataFilesWatchTime(0.0f), isDataFilesWatchingEnabled(false),
          currentGameState(nullptr), previousGameState(nullptr),
          dataFilesWatcherEventHandler(*this),
          profilerToggleKey(GTCore::Keys::F11),
          editorToggleKeyCombination(GTCore::Keys::Shift, GTCore::Keys::Tab)
    {
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
            this->eventQueue.Push(e);
        this->eventQueueLock.Unlock();
    }


    GTGUI::Server & Game::GetGUI()
    {
        return this->gui;
    }

    void Game::StepGUI()
    {
        this->StepGUI(this->deltaTimeInSeconds);
    }

    void Game::StepGUI(double deltaTimeInSeconds)
    {
        this->gui.Step(deltaTimeInSeconds);
    }


    void Game::EnableFullscreen()
    {
        if (this->window != nullptr)
        {
            // We use Display.Width and Display.Height in determining this.
            int width  = this->script.GetInteger("Display.Width");
            int height = this->script.GetInteger("Display.Height");

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
        this->GetWindow()->SetSize(this->GetScript().GetInteger("Display.Width"), this->GetScript().GetInteger("Display.Height"));

        if (this->GetScript().GetBoolean("Display.Fullscreen"))
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


    void Game::GetSmoothedMouseOffset(float &x, float &y)
    {
        if (this->mouseCaptured)
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

                averageFactor += 1.0; //iFactor;
                iFactor *= MouseSmoothFactor;
            }

            x = totalX / averageFactor;
            y = totalY / averageFactor;
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
        }
    }

    void Game::ReleaseMouse()
    {
        if (this->mouseCaptured)
        {
            this->ShowCursor();
            this->mouseCaptured = false;

            this->SetMousePosition(this->mouseCapturePosX, this->mouseCapturePosY);
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


    bool Game::IsKeyDown(GTCore::Key key) const
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

    bool Game::IsMouseButtonDown(GTCore::MouseButton button) const
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
            this->OnPause();

            if (this->currentGameState != nullptr)
            {
                this->currentGameState->OnPause();
            }
        }
    }

    void Game::Resume()
    {
        if (this->paused)
        {
            this->paused = false;
            this->OnResume();

            if (this->currentGameState != nullptr)
            {
                this->currentGameState->OnResume();
            }
        }
    }


    void Game::OpenEditor()
    {
        if (this->OnEditorOpening())
        {
            // The in-game profiling GUI needs to be hidden when the editor is open.
            this->DebuggingGUI.Hide();

            // We will also enable the profiler when the editor is open, because why not?
            this->profiler.Enable();


            this->Pause();  // The game is always paused while the editor is running.

            if (!this->editor.IsStarted())
            {
                this->editor.Startup(this->gui);
            }

            this->editor.Open();

            this->OnEditorOpen();
        }
    }

    void Game::CloseEditor()
    {
        if (this->OnEditorClosing())
        {
            this->editor.Close();

            // We can now unpause the game.
            this->Resume();

            if (this->DebuggingGUI.isShowing)
            {
                this->profiler.Enable();
                this->DebuggingGUI.Show();
            }
            else
            {
                this->profiler.Disable();
            }

            this->OnEditorClose();
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


    bool Game::ExecuteScript(const char* script)
    {
        return this->script.Execute(script);
    }


    void Game::ActivateGameState(GameState &newGameState)
    {
        if (this->currentGameState != &newGameState)
        {
            this->DeactivateCurrentGameState();

            this->currentGameState = &newGameState;
            this->currentGameState->Activate();
        }
    }

    void Game::ActivatePreviousGameState()
    {
        if (this->previousGameState != nullptr)
        {
            this->ActivateGameState(*this->previousGameState);
        }
        else
        {
            this->DeactivateCurrentGameState();
        }
    }

    void Game::DeactivateCurrentGameState()
    {
        this->previousGameState = this->currentGameState;
        if (this->previousGameState != nullptr)
        {
            this->previousGameState->Deactivate();
        }

        this->currentGameState = nullptr;
    }



    void Game::EnableScriptAutoReload()
    {
        this->isAutoScriptReloadEnabled = true;
    }

    void Game::DisableScriptAutoReload()
    {
        this->isAutoScriptReloadEnabled = false;
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
        if (!item.info.isDirectory)
        {
            auto extension = GTCore::Path::Extension(item.info.path.c_str());

            if (ModelLibrary::IsExtensionSupported(extension))
            {
                ModelLibrary::ReloadModel(item.info.path.c_str());

                // We will tell the editor that the definition has changed. This will allow it to update any state information or whatnot.
                this->editor.OnModelDefinitionChanged(item.info.absolutePath.c_str());
            }
            else if (Texture2DLibrary::IsExtensionSupported(extension))
            {
                Texture2DLibrary::Reload(item.info.path.c_str());
            }
            else
            {
                // If we have a script file we will reload it if applicable.
                if (this->IsScriptAutoReloadEnabled())
                {
                    if (this->script.HasFileBeenLoaded(item.info.path.c_str()))
                    {
                        this->script.ExecuteFile(item.info.path.c_str());
                    }
                }
            }
        }
    }


    GTCore::Key Game::GetProfilerToggleKey() const
    {
        return this->profilerToggleKey;
    }

    void Game::SetProfilerToggleKey(GTCore::Key key)
    {
        this->profilerToggleKey = key;
    }


    const GTCore::KeyCombination & Game::GetEditorToggleKeyCombination() const
    {
        return this->editorToggleKeyCombination;
    }

    void Game::SetEditorToggleKeyCombination(const GTCore::KeyCombination &newCombination)
    {
        this->editorToggleKeyCombination = newCombination;
    }



    void Game::OnLoadConfigs()
    {
    }

    bool Game::OnStartup(int, char**)
    {
        return true;
    }

    void Game::OnShutdown()
    {
    }

    void Game::OnUpdate(double)
    {
    }

    void Game::OnDraw()
    {
    }

    void Game::OnPostDraw()
    {
    }

    void Game::OnStartFrame()
    {
    }

    void Game::OnEndFrame()
    {
    }

    void Game::OnSize(unsigned int, unsigned int)
    {
    }

    void Game::OnMouseMove(int, int)
    {
    }

    void Game::OnMouseWheel(int, int, int)
    {
    }

    void Game::OnMouseButtonDown(GTCore::MouseButton, int, int)
    {
    }

    void Game::OnMouseButtonUp(GTCore::MouseButton, int, int)
    {
    }

    void Game::OnMouseButtonDoubleClick(GTCore::MouseButton, int, int)
    {
    }

    void Game::OnKeyPressed(GTCore::Key)
    {
    }

    void Game::OnKeyReleased(GTCore::Key)
    {
    }

    void Game::OnKeyDown(GTCore::Key)
    {
    }

    void Game::OnKeyUp(GTCore::Key)
    {
    }

    void Game::OnReceiveFocus()
    {
    }

    void Game::OnLoseFocus()
    {
    }

    void Game::OnHandleEvent(GameEvent &)
    {
    }

    void Game::OnPause()
    {
    }

    void Game::OnResume()
    {
    }


    bool Game::OnEditorOpening()
    {
        return true;
    }

    bool Game::OnEditorClosing()
    {
        return true;
    }

    void Game::OnEditorOpen()
    {
    }

    void Game::OnEditorClose()
    {
    }


    bool Game::Startup(int argc, char **argv)
    {
        // We'll need to grab the command line because the first thing we're going to do is load any user scripts into the scripting environment.
        GTCore::CommandLine cmdLine(argc, argv);

        // The first thing we do is load up the scripting environment. We do this first because it will contain configuration properties
        // for things later on.
        if (this->script.Startup())
        {
            // We give the game an opportunity to load configs before processing --config arguments.
            this->OnLoadConfigs();

            // This is where the user config scripts are loaded.
            const char** cmdLine_config = cmdLine.GetArgument("config");
            if (cmdLine_config != nullptr)
            {
                for (int i = 0; cmdLine_config[i] != nullptr; ++i)
                {
                    this->script.ExecuteFile(cmdLine_config[i]);
                }
            }

            // Here we will set the default anistropy for textures via the texture library.
            Texture2DLibrary::SetDefaultAnisotropy(static_cast<unsigned int>(this->script.GetInteger("Display.Textures.Anisotropy")));


            // First we need a window. Note that we don't show it straight away.
            this->window = Renderer::CreateGameWindow();
            if (this->window != nullptr)
            {
                // We'll need to grab the update thread object. We grab this from the thread cache which will have been initialised
                // in GTEngine::Startup(). It's important that we have a thread here, so we need to force it (first argument = true).
                this->updateThread = ThreadCache::AcquireThread(true);


                // We'll want to set a few window properties before showing it... We want to show the window relatively early to make
                // the game feel a little bit more speedy, even though it's not really.
                this->window->SetTitle("GTEngine Game");
                this->window->SetSize(this->script.GetInteger("Display.Width"), this->script.GetInteger("Display.Height"));

                // Now we can set the window's event handler and show it.
                this->window->SetEventHandler(this->windowEventHandler);
                this->window->Show();


                // Here we'll initialise the font cache. We purposly do it after moving into the Data directory.
                Log("Loading Fonts...");
                if (!this->InitialiseFonts())
                {
                    Log("Error loading fonts.");
                }


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


                // We're going to have keyboard auto-repeat disabled by default. This should make things like player controlling a bit more intuitive.
                GTCore::Keyboard::DisableAutoRepeat();


                // Here is where we let the game object do some startup stuff.
                if (this->OnStartup(argc, argv))
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


    bool Game::InitialiseFonts()
    {
        // We're currently in the Data directory as definied by the application configuration. In this directory
        // should be a fonts directory. We'll load that up into the font cache.
        if (!this->fontServer.LoadDirectory("engine/fonts"))
        {
            // We failed to load the "fonts" directory, so we'll try just loading the system fonts. If this fails,
            // we have to return false. On Windows, the system fonts are all fonts in the "Windows/Fonts" directory.
            if (!this->fontServer.LoadSystemFonts())
            {
                return false;
            }
        }


        GTType::FontInfo fi;
        fi.family         = "Liberation Sans";
        fi.sizeInPoints   = 10;
        this->defaultFont = &this->fontServer.AcquireFont(fi);

        return true;
    }

    bool Game::InitialiseGUI()
    {
        this->gui.SetEventHandler(this->guiEventHandler);
        return true;
    }

    bool Game::InitialiseDataFilesWatcher()
    {
        auto &directories = ApplicationConfig::GetDataDirectories();
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
        this->OnShutdown();
        this->script.Execute("Game.OnShutdown();");

        if (this->defaultFont != nullptr)
        {
            this->fontServer.UnacquireFont(*this->defaultFont);
        }

        delete this->window;

        ThreadCache::UnacquireThread(this->updateThread);
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
            while (GTCore::PumpNextWindowEvent(false));


            // If we're watching the data directories, we want to check for changes now.
            if (this->IsDataFilesWatchingEnabled())
            {
                float checkInterval = this->GetDataFilesWatchInterval();

                if (GTCore::Timing::GetTimeInSeconds() - this->lastDataFilesWatchTime >= checkInterval)
                {
                    if (this->dataFilesWatcher.EventsReady())
                    {
                        this->dataFilesWatcher.DispatchEvents(false);       // <-- 'false' means to not wait.
                        this->dataFilesWatcher.CheckForChanges(true);       // <-- 'true' means to go asynchronous.
                    }

                    this->lastDataFilesWatchTime = static_cast<float>(GTCore::Timing::GetTimeInSeconds());
                }
            }


            // We want our events to be handled synchronously on the main thread.
            this->HandleEvents();

            // The GUI events can be handled here. If it turns out that they're best handled on the update thread, all we need
            // to do is remove this line - GTGUI::Server::Step() will also handle any pending events. I like to handle the GUI
            // events from here because it's nice to have GUI events handled at the same time as window events, since they're
            // kind of related.
            this->gui.HandleEvents();


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
        this->deltaTimeInSeconds = GTCore::Min(this->updateTimer.Update(), 1.0);

        // We also need to increment the total running time, but only if we're not paused.
        if (!this->IsPaused())
        {
            this->totalRunninTimeInSeconds += this->deltaTimeInSeconds;
        }


        // We need to cache the current position of the mouse. We need to do this in order to get smooth mouse movement.
        this->CacheMousePosition();

        // Here is where we swap RC queues. We post an OnSwapRCQueues() event from here so that a game can do
        // it's own buffer swaps if required.
        this->SwapRCQueues();


        // Now we let the game know that we're starting the frame.
        this->OnStartFrame();

        // Now we just run the job without attempting to block (second argument).
        this->updateThread->Start(this->updateJob, false);
    }

    void Game::EndFrame() // [Main Thread]
    {
        // We need to collect garbage.
        GarbageCollector::CollectTexture2Ds();
        GarbageCollector::CollectShaders();
        GarbageCollector::CollectFramebuffers();
        GarbageCollector::CollectVertexArrays();

        // First we need to block until all threads have finished executing...
        this->updateThread->Wait();

        // Now we can let the game know that we've finished the frame...
        this->OnEndFrame();
    }

    void Game::Update() //[Update Thread]
    {
        if (this->profiler.IsEnabled())
        {
            this->profiler.OnBeginUpdate();
        }


        double deltaTimeInSeconds = this->GetDeltaTimeInSeconds();

        // If the debugging overlay is open, we need to show the debugging information.
        if (this->IsDebuggingOpen())
        {
            this->DebuggingGUI.Update(this->profiler);
        }


        // If the editor is open it also needs to be updated.
        if (this->editor.IsOpen())
        {
            this->editor.Update(deltaTimeInSeconds);
        }

        // The game needs to know that we're updating...
        this->OnUpdate(deltaTimeInSeconds);
        this->PostScriptEvent_OnUpdate(deltaTimeInSeconds);
        
        
        // ... and the game state.
        if (this->currentGameState != nullptr)
        {
            this->currentGameState->OnUpdate(deltaTimeInSeconds);
        }


        // We will step the GUI after updating the game.
        this->StepGUI(deltaTimeInSeconds);


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
        // We're not currently calling any scripting events on the rendering thread because of a few multithreading issues with the scripting environment. Need to
        // look deeper into what's causing this. Initial guess is that the Lua implementation isn't completely thread safe, but not looked into it. For performance,
        // can't use a mutex here.


        this->OnDraw();
        //this->script.Execute("Game.OnDraw();");

        Renderer::ExecuteFrontRCQueue();

        this->OnPostDraw();
        //this->script.Execute("Game.OnPostDraw();");


        // We draw the GUI on top of everything else...
        Renderer::SetFramebuffer(nullptr);
        Renderer::DrawGUI(this->gui);


        Renderer::SwapBuffers();


        if (this->profiler.IsEnabled())
        {
            this->profiler.OnEndRendering();
        }
    }

    void Game::SwapRCQueues()
    {
        // First the renderer...
        Renderer::SwapRCQueues();

        // Now the GUI...
        this->gui.SwapRCQueues();
    }


    bool Game::IsKeyCombinationDown(const GTCore::KeyCombination &combination) const
    {
        // We ignore keys that are set to null, but we need to always return false if all of them are null.
        if (!combination.IsAnyKeySet())
        {
            return false;
        }


        bool down = true;

        down = down && (combination.systemKey1    == GTCore::Keys::Null || this->IsKeyDown(combination.systemKey1));
        down = down && (combination.systemKey2    == GTCore::Keys::Null || this->IsKeyDown(combination.systemKey2));
        down = down && (combination.printableKey1 == GTCore::Keys::Null || this->IsKeyDown(combination.printableKey1));
        down = down && (combination.printableKey2 == GTCore::Keys::Null || this->IsKeyDown(combination.printableKey2));

        return down;
    }


    void Game::HandleEvents()
    {
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

            case EventCodes::OnSize:                      this->HandleEvent_OnSize(e);                    break;
            case EventCodes::OnMouseMove:                 this->HandleEvent_OnMouseMove(e);               break;
            case EventCodes::OnMouseWheel:                this->HandleEvent_OnMouseWheel(e);              break;
            case EventCodes::OnMouseButtonDown:           this->HandleEvent_OnMouseButtonDown(e);         break;
            case EventCodes::OnMouseButtonUp:             this->HandleEvent_OnMouseButtonUp(e);           break;
            case EventCodes::OnMouseButtonDoubleClick:    this->HandleEvent_OnMouseButtonDoubleClick(e);  break;
            case EventCodes::OnKeyPressed:                this->HandleEvent_OnKeyPressed(e);              break;
            case EventCodes::OnKeyReleased:               this->HandleEvent_OnKeyReleased(e);             break;
            case EventCodes::OnKeyDown:                   this->HandleEvent_OnKeyDown(e);                 break;
            case EventCodes::OnKeyUp:                     this->HandleEvent_OnKeyUp(e);                   break;
            case EventCodes::OnReceiveFocus:              this->HandleEvent_OnReceiveFocus(e);            break;
            case EventCodes::OnLoseFocus:                 this->HandleEvent_OnLoseFocus(e);               break;

            // Any generic events are posted as an event to the game so that an application can handle it itself.
            default:
                {
                    this->OnHandleEvent(e);
                    break;
                }
            }
        }
    }

    void Game::HandleEvent_OnSize(GameEvent &e)
    {
        this->gui.SetViewportSize(e.size.width, e.size.height);

        this->OnSize(e.size.width, e.size.height);
        this->PostScriptEvent_OnSize(e);

        if (this->currentGameState != nullptr)
        {
            this->currentGameState->OnSize(e.size.width, e.size.height);
        }
    }

    void Game::HandleEvent_OnMouseMove(GameEvent &e)
    {
        // We don't post mouse events if the mouse is captured.
        if (!this->mouseCaptured)
        {
            this->gui.OnMouseMove(e.mousemove.x, e.mousemove.y);

            this->OnMouseMove(e.mousemove.x, e.mousemove.y);
            this->PostScriptEvent_OnMouseMove(e);

            if (this->currentGameState != nullptr)
            {
                this->currentGameState->OnMouseMove(e.mousemove.x, e.mousemove.y);
            }
        }
    }

    void Game::HandleEvent_OnMouseWheel(GameEvent &e)
    {
        this->gui.OnMouseWheel(e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);

        this->OnMouseWheel(e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);
        this->PostScriptEvent_OnMouseWheel(e);

        if (this->currentGameState != nullptr)
        {
            this->currentGameState->OnMouseWheel(e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);
        }
    }

    void Game::HandleEvent_OnMouseButtonDown(GameEvent &e)
    {
        this->mouseButtonDownMap.Add(e.mousedown.button, true);

        if (e.mousedown.button == GTCore::MouseButton_Left)
        {
            this->gui.OnLMBDown();
        }
        else if (e.mousedown.button == GTCore::MouseButton_Right)
        {
            this->gui.OnRMBDown();
        }
        else if (e.mousedown.button == GTCore::MouseButton_Middle)
        {
            this->gui.OnMMBDown();
        }

        this->OnMouseButtonDown(e.mousedown.button, e.mousedown.x, e.mousedown.y);
        this->PostScriptEvent_OnMouseButtonDown(e);

        if (this->currentGameState != nullptr)
        {
            this->currentGameState->OnMouseButtonDown(e.mousedown.button, e.mousedown.x, e.mousedown.y);
        }
    }

    void Game::HandleEvent_OnMouseButtonUp(GameEvent &e)
    {
        auto iButtonDown = this->mouseButtonDownMap.Find(e.mouseup.button);
        if (iButtonDown != nullptr)
        {
            iButtonDown->value = false;
        }

        if (e.mouseup.button == GTCore::MouseButton_Left)
        {
            this->gui.OnLMBUp();
        }
        else if (e.mouseup.button == GTCore::MouseButton_Right)
        {
            this->gui.OnRMBUp();
        }
        else if (e.mouseup.button == GTCore::MouseButton_Middle)
        {
            this->gui.OnMMBUp();
        }

        this->OnMouseButtonUp(e.mouseup.button, e.mouseup.x, e.mouseup.y);
        this->PostScriptEvent_OnMouseButtonUp(e);

        if (this->currentGameState != nullptr)
        {
            this->currentGameState->OnMouseButtonUp(e.mouseup.button, e.mouseup.x, e.mouseup.y);
        }
    }

    void Game::HandleEvent_OnMouseButtonDoubleClick(GameEvent &e)
    {
        if (e.mousedoubleclick.button == GTCore::MouseButton_Left)
        {
            this->gui.OnLMBDoubleClick();
        }
        else if (e.mousedoubleclick.button == GTCore::MouseButton_Right)
        {
            this->gui.OnLMBDoubleClick();
        }
        else if (e.mousedoubleclick.button == GTCore::MouseButton_Middle)
        {
            this->gui.OnLMBDoubleClick();
        }

        this->OnMouseButtonDoubleClick(e.mousedoubleclick.button, e.mousedoubleclick.x, e.mousedoubleclick.y);
        this->PostScriptEvent_OnMouseButtonDoubleClick(e);

        if (this->currentGameState != nullptr)
        {
            this->currentGameState->OnMouseButtonDoubleClick(e.mousedoubleclick.button, e.mousedoubleclick.x, e.mousedoubleclick.y);
        }
    }

    void Game::HandleEvent_OnKeyPressed(GameEvent &e)
    {
        this->keyDownMap.Add(e.keypressed.key, true);

        this->gui.OnKeyPressed(e.keypressed.key);

        this->OnKeyPressed(e.keypressed.key);
        this->PostScriptEvent_OnKeyPressed(e);

        if (this->currentGameState != nullptr)
        {
            this->currentGameState->OnKeyPressed(e.keypressed.key);
        }


        // Editor.
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

    void Game::HandleEvent_OnKeyReleased(GameEvent &e)
    {
        auto iKeyDown = this->keyDownMap.Find(e.keyreleased.key);
        if (iKeyDown != nullptr)
        {
            iKeyDown->value = false;
        }

        this->OnKeyReleased(e.keyreleased.key);
        this->PostScriptEvent_OnKeyReleased(e);

        if (this->currentGameState != nullptr)
        {
            this->currentGameState->OnKeyReleased(e.keyreleased.key);
        }
    }

    void Game::HandleEvent_OnKeyDown(GameEvent &e)
    {
        this->gui.OnKeyDown(e.keydown.key);

        this->OnKeyDown(e.keydown.key);
        this->PostScriptEvent_OnKeyDown(e);
    }

    void Game::HandleEvent_OnKeyUp(GameEvent &e)
    {
        this->gui.OnKeyUp(e.keyup.key);

        this->OnKeyUp(e.keyup.key);
        this->PostScriptEvent_OnKeyUp(e);
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

        this->OnReceiveFocus();
        this->PostScriptEvent_OnReceiveFocus(e);
    }

    void Game::HandleEvent_OnLoseFocus(GameEvent &e)
    {
        this->focused = false;

        // We need to post key released/up events for any key that is currently down.
        for (size_t i = 0; i < this->keyDownMap.count; ++i)
        {
            auto iKey = this->keyDownMap.buffer[i];
            assert(iKey != nullptr);

            if (iKey->value)
            {
                this->OnKeyReleased(iKey->key);
                this->OnKeyUp(iKey->key);

                iKey->value = false;
            }
        }

        this->DisableFullscreen();

        this->OnLoseFocus();
        this->PostScriptEvent_OnLoseFocus(e);
    }


    void Game::PostScriptEvent_OnUpdate(double deltaTimeInSeconds)
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
                this->script.Push(deltaTimeInSeconds);
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
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnMouseMove");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("x");
                this->script.Push(static_cast<int>(e.mousemove.x));
                this->script.SetTableValue(-3);

                this->script.Push("y");
                this->script.Push(static_cast<int>(e.mousemove.y));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnMouseWheel(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnMouseWheel");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("x");
                this->script.Push(static_cast<int>(e.mousewheel.x));
                this->script.SetTableValue(-3);

                this->script.Push("y");
                this->script.Push(static_cast<int>(e.mousewheel.y));
                this->script.SetTableValue(-3);

                this->script.Push("delta");
                this->script.Push(static_cast<int>(e.mousewheel.delta));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnMouseButtonDown(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnMouseButtonDown");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("x");
                this->script.Push(static_cast<int>(e.mousedown.x));
                this->script.SetTableValue(-3);

                this->script.Push("y");
                this->script.Push(static_cast<int>(e.mousedown.y));
                this->script.SetTableValue(-3);

                this->script.Push("button");
                this->script.Push(static_cast<int>(e.mousedown.button));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnMouseButtonUp(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnMouseButtonUp");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("x");
                this->script.Push(static_cast<int>(e.mouseup.x));
                this->script.SetTableValue(-3);

                this->script.Push("y");
                this->script.Push(static_cast<int>(e.mouseup.y));
                this->script.SetTableValue(-3);

                this->script.Push("button");
                this->script.Push(static_cast<int>(e.mouseup.button));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnMouseButtonDoubleClick(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnMouseButtonDoubleClick");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("x");
                this->script.Push(static_cast<int>(e.mousedoubleclick.x));
                this->script.SetTableValue(-3);

                this->script.Push("y");
                this->script.Push(static_cast<int>(e.mousedoubleclick.y));
                this->script.SetTableValue(-3);

                this->script.Push("button");
                this->script.Push(static_cast<int>(e.mousedoubleclick.button));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnKeyPressed(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnKeyPressed");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("key");
                this->script.Push(static_cast<int>(e.keypressed.key));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
    }

    void Game::PostScriptEvent_OnKeyReleased(const GameEvent &e)
    {
        this->script.GetGlobal("Game");
        assert(this->script.IsTable(-1));
        {
            this->script.Push("OnKeyReleased");
            this->script.GetTableValue(-2);
            assert(this->script.IsFunction(-1));
            {
                this->script.PushNewTable();

                this->script.Push("key");
                this->script.Push(static_cast<int>(e.keyreleased.key));
                this->script.SetTableValue(-3);

                this->script.Call(1, 0);
            }
        }
        this->script.Pop(1);
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
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
