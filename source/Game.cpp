
#include <GTEngine/Game.hpp>
#include <GTEngine/FontManager.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/GarbageCollector.hpp>
#include <GTEngine/ThreadCache.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTCore/System.hpp>
#include <GTCore/Strings/Tokenizer.hpp>
#include <GTCore/String.hpp>
#include <GTCore/CommandLine.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior: elements of array 'array' will be default initialized
    #pragma warning(disable:4355)   // 'this' used in initialise list.
    #pragma warning(disable:4482)   // nonstandard extension used: enum used in qualified name
#endif

namespace GTEngine
{
    extern bool IsGameObjectCreated;

    Game::Game(int argc, char** argv)
        : isInitialised(false), closing(false), eventQueue(), eventQueueLock(),
          window(nullptr), windowEventHandler(*this),
          script(*this),
          updateThread(nullptr), updateJob(*this), 
          totalRunninTimeInSeconds(0.0), deltaTimeInSeconds(0.0), updateTimer(),
          fontServer("var/fonts.cache"), defaultFont(nullptr),
          gui(&script, &fontServer), guiEventHandler(*this),
          paused(false), focused(true),
          keyDownMap(), mouseButtonDownMap(),
          editor(*this),
          mouseCaptured(false), mouseCapturePosX(0), mouseCapturePosY(0),
          mouseCenterX(0), mouseCenterY(0),
          mousePosXBuffer(), mousePosYBuffer(), mousePosBufferIndex(0)
    {
        if (!IsGameObjectCreated)
        {
            this->isInitialised = this->Initialise(argc, argv);
            if (this->isInitialised)
            {
                IsGameObjectCreated = true;
            }
        }
        else
        {
            PostError("A Game object has already been created. Only a single Game object can be created for each application.");
        }
    }

    Game::~Game()
    {
        this->Uninitialise();
        IsGameObjectCreated = false;
    }

    int Game::Run()
    {
        // If the game is not initialised, we can't continue.
        if (this->IsInitialised())
        {
            // All we do now is enter the game loop... Once this returns, we're finished with the game and we
            // can clean up.
            this->Loop();

            // If we made it here, it means the game was run and closed normally.
            return 0;
        }

        // If we made it here, there was an error initialising.
        return 1;
    }

    void Game::Close()
    {
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
        }
    }

    void Game::DisableFullscreen()
    {
        if (this->window != nullptr)
        {
            this->window->DisableFullscreen();
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
            GTCore::HideMouse();
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
            GTCore::ShowMouse();
            this->mouseCaptured = false;

            this->SetMousePosition(this->mouseCapturePosX, this->mouseCapturePosY);
        }
    }

    void Game::ShowCursor()
    {
        GTCore::ShowMouse();
    }

    void Game::HideCursor()
    {
        GTCore::HideMouse();
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

            //printf("%d %d\n", offsetX, offsetY);

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
        }
    }

    void Game::Resume()
    {
        if (this->paused)
        {
            this->paused = false;
            this->OnResume();
        }
    }


    void Game::OpenEditor()
    {
        if (this->OnEditorOpening())
        {
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
        if (!DebuggingGUI.isInitialised)
        {
            this->DebuggingGUI.Initialise(this->gui);
        }

        if (this->DebuggingGUI.DebuggingMain != nullptr)
        {
            this->DebuggingGUI.DebuggingMain->Show();
            this->DebuggingGUI.isShowing = true;
        }
    }

    void Game::HideDebugging()
    {
        if (this->DebuggingGUI.DebuggingMain != nullptr)
        {
            this->DebuggingGUI.DebuggingMain->Hide();
            this->DebuggingGUI.isShowing = false;
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



    void Game::OnUpdate()
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

    void Game::OnSwapRCQueues()
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


    bool Game::Initialise(int argc, char **argv)
    {
        // We'll need to grab the command line because the first thing we're going to do is load any user scripts into the scripting environment.
        GTCore::CommandLine cmdLine(argc, argv);

        // This is where the user config scripts are loaded.
        const char** cmdLine_config = cmdLine.GetArgument("config");
        if (cmdLine_config != nullptr)
        {
            for (int i = 0; cmdLine_config[i] != nullptr; ++i)
            {
                this->script.ExecuteFile(cmdLine_config[i]);
            }
        }


        // We will initialise the scripting environment before setting up anything else. This will allow the scripting environment to be
        // accessed as early as possible.
        Log("Initializing Scripting Environment...");
        if (!this->InitialiseScripting())
        {
            Log("Error initializing scripting environment.");
        }


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


            return true;
        }
        else
        {
            // We couldn't create a window, which means the renderer is not usable...
            GTEngine::PostError("Error initialising renderer.");
        }

        return false;
    }

    bool Game::InitialiseScripting()
    {
        bool result = true;

        // We first want to ensure we have the tables setup.
        result = result && this->script.Execute
        (
            "Game   = {};\n"
            "Editor = {};\n"
            "Engine = {};\n"
            "Engine.ModelEditor    = {};\n"
            "Engine.MaterialEditor = {};\n"
            "Engine.SceneEditor    = {};\n"
            "Engine.Sandbox        = {};\n"
        );

        // Here is where we setup the foreign function interface. We do this before setting up the standard scripting library.
        result = result && this->InitialiseScriptingFFI();

        // Here we initialise the engine's standard scripting library for making scripting stuff a little easier.
        result = result && this->InitialiseScriptingSTDLibrary();

        return result;
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

        // We need to attach an event handler.
        this->fontServer.AttachEventHandler(&GlobalFontEventHandler);


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

    void Game::Uninitialise()
    {
        if (this->defaultFont != nullptr)
        {
            this->fontServer.UnacquireFont(*this->defaultFont);
        }
        
        delete this->window;

        ThreadCache::UnacquireThread(this->updateThread);
        //delete this->updateJob;
    }

    void Game::Loop()
    {
        while (!this->closing)
        {
            // First we need to handle any pending window messages. We do not want to wait here (first argument).
            while (GTCore::PumpNextWindowEvent(false));

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
        // If the editor is open it also needs to be updated.
        if (this->editor.IsOpen())
        {
            this->editor.Update(this->GetDeltaTimeInSeconds());
        }

        // If the debugging overlay is open, we need to show the debugging information.
        if (this->IsDebuggingOpen())
        {
            this->DebuggingGUI.Step();
        }

        // The game needs to know that we're updating.
        this->OnUpdate();
    }

    void Game::Draw() //[Main Thread]
    {
        this->OnDraw();
        Renderer::ExecuteFrontRCQueue();
        this->OnPostDraw();

        // We draw the GUI on top of everything else...
        Renderer::SetFramebuffer(nullptr);
        Renderer::DrawGUI(this->gui);


        Renderer::SwapBuffers();
    }

    void Game::SwapRCQueues()
    {
        // Now the renderer...
        Renderer::SwapRCQueues();

        // Now the GUI...
        this->gui.SwapRCQueues();


        // And the editor...
        if (this->editor.IsOpen())
        {
            this->editor.SwapRCQueues();
        }

        // Now we call the event and allow the game to do it's own buffer swaps if it so wishes.
        this->OnSwapRCQueues();
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
    }

    void Game::HandleEvent_OnMouseMove(GameEvent &e)
    {
        // We don't post mouse events if the mouse is captured.
        if (!this->mouseCaptured)
        {
            this->gui.OnMouseMove(e.mousemove.x, e.mousemove.y);
            this->OnMouseMove(e.mousemove.x, e.mousemove.y);
        }
    }

    void Game::HandleEvent_OnMouseWheel(GameEvent &e)
    {
        this->gui.OnMouseWheel(e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);
        this->OnMouseWheel(e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);
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
    }

    void Game::HandleEvent_OnKeyPressed(GameEvent &e)
    {
        this->keyDownMap.Add(e.keypressed.key, true);

        this->OnKeyPressed(e.keypressed.key);
    }

    void Game::HandleEvent_OnKeyReleased(GameEvent &e)
    {
        auto iKeyDown = this->keyDownMap.Find(e.keyreleased.key);
        if (iKeyDown != nullptr)
        {
            iKeyDown->value = false;
        }

        this->OnKeyReleased(e.keyreleased.key);
    }

    void Game::HandleEvent_OnKeyDown(GameEvent &e)
    {
        this->gui.OnKeyDown(e.keydown.key);
        this->OnKeyDown(e.keydown.key);
    }

    void Game::HandleEvent_OnKeyUp(GameEvent &e)
    {
        this->gui.OnKeyUp(e.keyup.key);
        this->OnKeyUp(e.keyup.key);
    }

    void Game::HandleEvent_OnReceiveFocus(GameEvent &)
    {
        this->focused = true;

        this->OnReceiveFocus();
    }

    void Game::HandleEvent_OnLoseFocus(GameEvent &)
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

        this->OnLoseFocus();
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////
// Below is everyhing to do with setting up the scripting environment.

namespace GTEngine
{
    ///////////////////////////////////////////////////////////////
    // FFI

    Game & Game::FFI::GetGameObject(GTCore::Script &script)
    {
        script.GetGlobal("__GamePtr");
        auto game = static_cast<Game*>(script.ToPointer(-1));
        script.Pop(1);

        assert(game != nullptr);

        return *game;
    }



    ////////////////////////////////////////////////////////////////
    // Engine FFI

    int FFI_Game_Close(GTCore::Script &script)
    {
        auto &game = Game::FFI::GetGameObject(script);

        game.Close();
        return 0;
    }

    int FFI_Game_Pause(GTCore::Script &script)
    {
        auto& game = Game::FFI::GetGameObject(script);

        game.Pause();
        return 0;
    }

    int FFI_Game_Resume(GTCore::Script &script)
    {
        auto& game = Game::FFI::GetGameObject(script);

        game.Resume();
        return 0;
    }

    int FFI_Game_EnableFullscreen(GTCore::Script &script)
    {
        auto& game = Game::FFI::GetGameObject(script);

        game.EnableFullscreen();
        return 0;
    }

    int FFI_Game_DisableFullscreen(GTCore::Script &script)
    {
        auto& game = Game::FFI::GetGameObject(script);

        game.DisableFullscreen();
        return 0;
    }


    


    ////////////////////////////////////////////////////////////////
    // Engine FFI

    int FFI_Engine_ExecuteScript(GTCore::Script &script)
    {
        auto& game = Game::FFI::GetGameObject(script);

        script.Push(game.ExecuteScript(script.ToString(1)));
        return 1;
    }

    int FFI_Engine_GetLastScriptError(GTCore::Script &script)
    {
        auto& game = Game::FFI::GetGameObject(script);

        script.Push(game.GetScript().GetLastError());
        return 1;
    }

    int FFI_Engine_ShowDebug(GTCore::Script &script)
    {
        auto &game = Game::FFI::GetGameObject(script);

        game.ShowDebugging();
        return 0;
    }

    int FFI_Engine_HideDebug(GTCore::Script &script)
    {
        auto &game = Game::FFI::GetGameObject(script);

        game.HideDebugging();
        return 0;
    }

    int FFI_Engine_OpenEditor(GTCore::Script &script)
    {
        auto &game = Game::FFI::GetGameObject(script);

        game.OpenEditor();
        return 0;
    }

    int FFI_Engine_CloseEditor(GTCore::Script &script)
    {
        auto &game = Game::FFI::GetGameObject(script);

        game.CloseEditor();
        return 0;
    }



    ////////////////////////////////////////////////////////////////
    // Initialisation.

    bool Game::InitialiseScriptingFFI()
    {
        // We assert that the tables have already been created.

        this->script.Push(this);
        this->script.SetGlobal("__GamePtr");

        this->script.GetGlobal("Game");
        {
            this->script.Push("Close");
            this->script.PushClosure(FFI_Game_Close, 0);
            this->script.SetTableValue(-3);

            this->script.Push("Pause");
            this->script.PushClosure(FFI_Game_Pause, 0);
            this->script.SetTableValue(-3);

            this->script.Push("Resume");
            this->script.PushClosure(FFI_Game_Resume, 0);
            this->script.SetTableValue(-3);


            this->script.Push("EnableFullscreen");
            this->script.PushClosure(FFI_Game_EnableFullscreen, 0);
            this->script.SetTableValue(-3);

            this->script.Push("DisableFullscreen");
            this->script.PushClosure(FFI_Game_DisableFullscreen, 0);
            this->script.SetTableValue(-3);
        }
        this->script.Pop(1);    // Game


        this->script.GetGlobal("Engine");
        {
            this->script.Push("ExecuteScript");
            this->script.PushClosure(FFI_Engine_ExecuteScript, 0);
            this->script.SetTableValue(-3);

            this->script.Push("GetLastScriptError");
            this->script.PushClosure(FFI_Engine_GetLastScriptError, 0);
            this->script.SetTableValue(-3);


            this->script.Push("ShowDebug");
            this->script.PushClosure(FFI_Engine_ShowDebug, 0);
            this->script.SetTableValue(-3);

            this->script.Push("HideDebug");
            this->script.PushClosure(FFI_Engine_HideDebug, 0);
            this->script.SetTableValue(-3);

            this->script.Push("OpenEditor");
            this->script.PushClosure(FFI_Engine_OpenEditor, 0);
            this->script.SetTableValue(-3);

            this->script.Push("CloseEditor");
            this->script.PushClosure(FFI_Engine_CloseEditor, 0);
            this->script.SetTableValue(-3);
        }
        this->script.Pop(1);    // Engine


        this->script.GetGlobal("Editor");
        {
            this->script.Push("ModelEditor");
            this->script.GetTableValue(-2);
            if (this->script.IsTable(-1))
            {
            }
            this->script.Pop(1);


            this->script.Push("MaterialEditor");
            this->script.GetTableValue(-2);
            if (this->script.IsTable(-1))
            {
            }
            this->script.Pop(1);


            this->script.Push("SceneEditor");
            this->script.GetTableValue(-2);
            if (this->script.IsTable(-1))
            {
            }
            this->script.Pop(1);


            this->script.Push("Sandbox");
            this->script.GetTableValue(-2);
            if (this->script.IsTable(-1))
            {
            }
            this->script.Pop(1);
        }
        this->script.Pop(1);    // Editor

        return true;
    }

    bool Game::InitialiseScriptingSTDLibrary()
    {
        return true;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
