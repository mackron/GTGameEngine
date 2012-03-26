
#include <GTEngine/Game.hpp>
#include <GTEngine/GameUpdateJob.hpp>
#include <GTEngine/ApplicationConfig.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/FontManager.hpp>
#include <GTEngine/GUIEventHandler.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTCore/System.hpp>
#include <GTCore/CommandLine.hpp>
#include <GTCore/Math.hpp>

#include <GTEngine/ColladaLoader.hpp>
#include <GTEngine/DefaultScene.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior: elements of array 'array' will be default initialized
    #pragma warning(disable:4355)   // 'this' used in initialise list.
    #pragma warning(disable:4482)   // nonstandard extension used: enum used in qualified name
#endif

namespace GTEngine
{
    Game::Game(int argc, char** argv)
        : isInitialised(false), closing(false), eventQueue(), eventQueueLock(), window(nullptr), windowEventHandler(*this), threads(nullptr), updateThread(nullptr), updateJob(nullptr), 
          deltaTimeInSeconds(0.0), updateTimer(), fontServer(nullptr), defaultFont(nullptr),
          gui(nullptr), guiEventHandler(nullptr),
          scene(nullptr), currentRCBuffer(nullptr),
          paused(false),
          mouseCaptured(false), mouseCapturePosX(0), mouseCapturePosY(0),
          mouseCenterX(0), mouseCenterY(0),
          mousePosXBuffer(), mousePosYBuffer(), mousePosBufferIndex(0)
    {
        this->isInitialised = this->Initialise(argc, argv);
    }

    Game::~Game()
    {
        this->Uninitialise();
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
        assert(this->gui != nullptr);
        return *this->gui;
    }

    void Game::StepGUI()
    {
        this->StepGUI(this->deltaTimeInSeconds);
    }

    void Game::StepGUI(double deltaTimeInSeconds)
    {
        assert(this->gui != nullptr);
        this->gui->Step(deltaTimeInSeconds);
    }


    void Game::SetScene(Scene *scene)
    {
        this->scene = scene;

        // We need to make sure the scene is rendering to the correct RC buffer.
        if (this->scene != nullptr)
        {
            this->scene->SetCurrentRCBuffer(this->currentRCBuffer);
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

                totalX += (float)this->mousePosXBuffer[bufferIndex] * iFactor;
                totalY += (float)this->mousePosYBuffer[bufferIndex] * iFactor;

                averageFactor += 1.0f;//iFactor;
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
        GTWindow::HideMouse();
        this->mouseCaptured = true;
        this->GetMousePosition(this->mouseCapturePosX, this->mouseCapturePosY);

        // The mouse needs to be moved to the middle of the window.
        unsigned int windowWidth, windowHeight;
        this->window->GetSize(windowWidth, windowHeight);

        this->mouseCenterX = (int)windowWidth  / 2;
        this->mouseCenterY = (int)windowHeight / 2;
        this->SetMousePosition(this->mouseCenterX, this->mouseCenterY);
    }

    void Game::ReleaseMouse()
    {
        GTWindow::ShowMouse();
        this->mouseCaptured = false;

        this->SetMousePosition(this->mouseCapturePosX, this->mouseCapturePosY);
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

    void Game::SetCurrentRCBuffer(RenderCommandBuffer *rcBuffer)
    {
        if (rcBuffer == nullptr)
        {
            rcBuffer = Renderer::BackBuffer;
        }

        this->currentRCBuffer = rcBuffer;

        // We tie the game and scene RC buffer target...
        if (this->scene != nullptr)
        {
            this->scene->SetCurrentRCBuffer(this->currentRCBuffer);
        }
    }

    RenderCommandBuffer * Game::GetCurrentRCBuffer()
    {
        return this->currentRCBuffer;
    }

    void Game::Pause()
    {
        if (!this->paused)
        {
            this->paused = true;
            
            if (this->scene != nullptr)
            {
                this->scene->Pause();
            }

            this->OnPause();
        }
    }

    void Game::Resume()
    {
        if (this->paused)
        {
            this->paused = false;

            if (this->scene != nullptr)
            {
                this->scene->Resume();
            }

            this->OnResume();
        }
    }


    void Game::OnUpdate()
    {
    }

    void Game::OnPostUpdate()
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

    void Game::OnMouseButtonDown(GTWindow::MouseButton, int, int)
    {
    }

    void Game::OnMouseButtonUp(GTWindow::MouseButton, int, int)
    {
    }

    void Game::OnMouseButtonDoubleClick(GTWindow::MouseButton, int, int)
    {
    }

    void Game::OnKeyPressed(GTWindow::Key)
    {
    }

    void Game::OnKeyReleased(GTWindow::Key)
    {
    }

    void Game::OnKeyDown(GTWindow::Key)
    {
    }

    void Game::OnKeyUp(GTWindow::Key)
    {
    }

    void Game::OnSwapRCBuffers()
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


    bool Game::Initialise(int argc, char **argv)
    {
        (void)argc;
        (void)argv;

        // First we need a window. Note that we don't show it straight away.
        this->window = Renderer::CreateGameWindow();
        if (this->window != nullptr)
        {
            // We need threads. We create a thread for each core. Those, plus the main thread, should work nicely since the main
            // thread will be running mostly on the GPU, and then there will be additional threads for each logical CPU. Should
            // probably do some proper testing, though...
            this->threads      = new GTCore::Thread[GTCore::System::GetCPUCount()];
            this->updateThread = &this->threads[0];

            // Now the job that will be doing the game update.
            this->updateJob = new GameUpdateJob(*this);

            // The current rendering command buffer will be the renderer's back buffer.
            this->currentRCBuffer = Renderer::BackBuffer;


            // We'll want to set a few window properties before showing it... We want to show the window relatively early to make
            // the game feel a little bit more speedy, even though it's not really.
            this->window->SetTitle("GTEngine Game");
            this->window->SetSize(UserConfig::GetInteger("Display.Width"), UserConfig::GetInteger("Display.Height"));

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

    bool Game::InitialiseFonts()
    {
        // We're currently in the Data directory as definied by the application configuration. In this directory
        // should be a fonts directory. We'll load that up into the font cache.
        this->fontServer = new GTType::FontServer("fonts/fonts.cache");
        if (!this->fontServer->LoadDirectory("fonts"))
        {
            // We failed to load the "fonts" directory, so we'll try just loading the system fonts. If this fails,
            // we have to return false. On Windows, the system fonts are all fonts in the "Windows/Fonts" directory.
            if (!this->fontServer->LoadSystemFonts())
            {
                return false;
            }
        }

        // We need to attach an event handler.
        this->fontServer->AttachEventHandler(&GlobalFontEventHandler);


        GTType::FontInfo fi;
        fi.family         = "DejaVu Sans Mono";
        fi.sizeInPoints   = 9;
        this->defaultFont = &this->fontServer->AcquireFont(fi);

        return true;
    }

    bool Game::InitialiseGUI()
    {
        this->guiEventHandler = new GUIEventHandler(*this);

        this->gui = new GTGUI::Server(this->fontServer);
        this->gui->SetEventHandler(*this->guiEventHandler);

        // We want to register some FFI stuff so games don't need to do it themselves...
        GTCore::Script &script = this->gui->GetScriptServer().GetScript();

        script.Push(this);
        script.SetGlobal("__GamePtr");

        script.PushNewTable();
        script.SetGlobal("Game");
        script.GetGlobal("Game");
            script.Push("Close");
            script.PushClosure(Game::GUIFFI::Close, 0);
            script.SetTableValue(-3);

            script.Push("Pause");
            script.PushClosure(Game::GUIFFI::Pause, 0);
            script.SetTableValue(-3);

            script.Push("Resume");
            script.PushClosure(Game::GUIFFI::Resume, 0);
            script.SetTableValue(-3);
        script.Pop(1);  // Game

        return true;
    }

    void Game::Uninitialise()
    {
        this->fontServer->UnacquireFont(*this->defaultFont);
        
        delete this->fontServer;
        delete this->gui;
        delete this->guiEventHandler;
        delete this->window;
    }

    void Game::Loop()
    {
        while (!this->closing)
        {
            // First we need to handle any pending window messages. We do not want to wait here (first argument).
            while (GTWindow::PumpNextWindowEvent(false));

            // We want our events to be handled synchronously on the main thread.
            this->HandleEvents();
            
            // The GUI events can be handled here. If it turns out that they're best handled on the update thread, all we need
            // to do is remove this line - GTGUI::Server::Step() will also handle any pending events. I like to handle the GUI
            // events from here because it's nice to have GUI events handled at the same time as window events, since they're
            // kind of related.
            this->gui->HandleEvents();


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

        // We need to cache the current position of the mouse. We need to do this in order to get smooth mouse movement.
        this->CacheMousePosition();

        // Here is where we swap rendering command buffers'. We post an OnSwapRCBuffers() event from here so that a game can do
        // it's own buffer swaps if required.
        this->SwapRCBuffers();

        // Now we let the game know that we're starting the frame.
        this->OnStartFrame();

        // Now we just run the job without attempting to block (second argument).
        this->updateThread->Start(*this->updateJob, false);
    }

    void Game::EndFrame() // [Main Thread]
    {
        // First we need to block until all threads have finished executing...
        this->updateThread->Wait();

        // Now we can let the game know that we've finished the frame...
        this->OnEndFrame();
    }

    void Game::Update() //[Update Thread]
    {
        // The game needs to know that we're updating.
        this->OnUpdate();

        // Here we just update the game's current scene. Scene implementations should render everything to it's
        // current RC buffer, which will be set when the game's RC buffer is set.
        if (this->scene != nullptr)
        {
            this->scene->Update(this->deltaTimeInSeconds);
        }

        // The game now needs to know we've just finished updating the scene.
        this->OnPostUpdate();
    }

    void Game::Draw() //[Main Thread]
    {
        this->OnDraw();
        Renderer::ExecuteFrontBuffer();

        // We draw the GUI on top of everything else...
        Renderer::DrawGUI(this->GetGUI());

        this->OnPostDraw();
        
        Renderer::SwapBuffers();
    }

    void Game::SwapRCBuffers()
    {
        // First we swap our own objects. With the renderer, if the game's current render command buffer is the renderer's back buffer,
        // we need to set it to the renderer's front buffer (which is about to become it's back buffer).
        if (this->currentRCBuffer == Renderer::BackBuffer)
        {
            this->SetCurrentRCBuffer(Renderer::FrontBuffer);
        }

        // Now the renderer...
        Renderer::SwapRCBuffers();

        // Now the GUI...
        this->gui->SwapRCBuffers();

        // Now we call the event and allow the game to do it's own buffer swaps if it so wishes.
        this->OnSwapRCBuffers();
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
        this->gui->SetViewportSize(e.size.width, e.size.height);
        this->OnSize(e.size.width, e.size.height);
    }

    void Game::HandleEvent_OnMouseMove(GameEvent &e)
    {
        // We don't post mouse events if the mouse is captured.
        if (!this->mouseCaptured)
        {
            this->gui->OnMouseMove(e.mousemove.x, e.mousemove.y);
            this->OnMouseMove(e.mousemove.x, e.mousemove.y);
        }
    }

    void Game::HandleEvent_OnMouseWheel(GameEvent &e)
    {
        this->OnMouseWheel(e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);
    }

    void Game::HandleEvent_OnMouseButtonDown(GameEvent &e)
    {
        if (e.mousedown.button == GTWindow::MouseButton::Left)
        {
            this->gui->OnLMBDown();
        }
        else if (e.mousedown.button == GTWindow::MouseButton::Right)
        {
            this->gui->OnRMBDown();
        }
        else if (e.mousedown.button == GTWindow::MouseButton::Middle)
        {
            this->gui->OnMMBDown();
        }

        this->OnMouseButtonDown(e.mousedown.button, e.mousedown.x, e.mousedown.y);
    }

    void Game::HandleEvent_OnMouseButtonUp(GameEvent &e)
    {
        if (e.mouseup.button == GTWindow::MouseButton::Left)
        {
            this->gui->OnLMBUp();
        }
        else if (e.mouseup.button == GTWindow::MouseButton::Right)
        {
            this->gui->OnRMBUp();
        }
        else if (e.mouseup.button == GTWindow::MouseButton::Middle)
        {
            this->gui->OnMMBUp();
        }

        this->OnMouseButtonUp(e.mouseup.button, e.mouseup.x, e.mouseup.y);
    }

    void Game::HandleEvent_OnMouseButtonDoubleClick(GameEvent &e)
    {
        if (e.mousedoubleclick.button == GTWindow::MouseButton::Left)
        {
            this->gui->OnLMBDoubleClick();
        }
        else if (e.mousedoubleclick.button == GTWindow::MouseButton::Right)
        {
            this->gui->OnLMBDoubleClick();
        }
        else if (e.mousedoubleclick.button == GTWindow::MouseButton::Middle)
        {
            this->gui->OnLMBDoubleClick();
        }

        this->OnMouseButtonDoubleClick(e.mousedoubleclick.button, e.mousedoubleclick.x, e.mousedoubleclick.y);
    }

    void Game::HandleEvent_OnKeyPressed(GameEvent &e)
    {
        this->OnKeyPressed(e.keypressed.key);
    }

    void Game::HandleEvent_OnKeyReleased(GameEvent &e)
    {
        this->OnKeyReleased(e.keyreleased.key);
    }

    void Game::HandleEvent_OnKeyDown(GameEvent &e)
    {
        this->gui->OnKeyDown(e.keydown.key);
        this->OnKeyDown(e.keydown.key);
    }

    void Game::HandleEvent_OnKeyUp(GameEvent &e)
    {
        this->gui->OnKeyUp(e.keyup.key);
        this->OnKeyUp(e.keyup.key);
    }
}


// GUI FFI
namespace GTEngine
{
    Game & Game::GUIFFI::GetGameObject(GTCore::Script &script)
    {
        script.GetGlobal("__GamePtr");
        auto game = static_cast<Game *>(script.ToPointer(-1));
        script.Pop(1);

        assert(game != nullptr);

        return *game;
    }


    int Game::GUIFFI::Close(GTCore::Script &script)
    {
        auto &game = Game::GUIFFI::GetGameObject(script);

        game.Close();
        return 0;
    }

    int Game::GUIFFI::Pause(GTCore::Script &script)
    {
        auto& game = Game::GUIFFI::GetGameObject(script);

        game.Pause();
        return 0;
    }

    int Game::GUIFFI::Resume(GTCore::Script &script)
    {
        auto& game = Game::GUIFFI::GetGameObject(script);

        game.Resume();
        return 0;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif