// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Context.hpp>
#include <GTGE/Rendering/Renderer.hpp>
#include <GTGE/Scripting.hpp>
#include <GTGE/IO.hpp>
#include <GTGE/GamePackager.hpp>
#include <GTGE/Editor2/Editor2.hpp>
#include <GTGE/Core/System.hpp>
#include <GTGE/Core/Strings/Tokenizer.hpp>
#include <GTGE/Core/String.hpp>
#include <GTGE/Core/Keyboard.hpp>
#include <GTGE/Core/WindowManagement.hpp>
#include <dr_libs/dr_path.h>
#include <dr_libs/dr_vfs.h>

namespace GT
{
    typedef struct
    {
        /// The absolute path of the executable's directory.
        char absoluteExeDirPath[DRVFS_MAX_PATH];

        /// The relative executable path.
        char absoluteExePath[DRVFS_MAX_PATH];

        /// The relative path of the log file.
        char relativeLogPath[DRVFS_MAX_PATH];

    } CommandLineData;


    static bool parse_cmdline_proc(const char* key, const char* value, void* pUserData)
    {
        CommandLineData* pData = reinterpret_cast<CommandLineData*>(pUserData);
        assert(pData != NULL);

        if (strcmp(key, "[path]") == 0)
        {
            char exeDirectoryPath[DRVFS_MAX_PATH];
            drpath_copy_base_path(value, exeDirectoryPath, sizeof(exeDirectoryPath));

            _chdir(exeDirectoryPath);
            _getcwd(pData->absoluteExeDirPath, sizeof(pData->absoluteExeDirPath));

            drpath_copy_and_append(pData->absoluteExePath, sizeof(pData->absoluteExePath), pData->absoluteExeDirPath, drpath_file_name(value));

            return true;
        }

        if (strcmp(key, "logfile") == 0)
        {
            strcpy_s(pData->relativeLogPath, sizeof(pData->relativeLogPath), value);
            return true;
        }

        return true;
    }


    typedef struct
    {
        /// A pointer to the main context.
        Context* pContext;

        /// A pointer to the file to read the config data from.
        drvfs_file* pFile;

    } AppConfigData;

    static unsigned int app_config_read(void* pUserData, void* pDataOut, unsigned int bytesToRead)
    {
        AppConfigData* pData = reinterpret_cast<AppConfigData*>(pUserData);
        assert(pData != NULL);

        unsigned int bytesRead;
        if (drvfs_read(pData->pFile, pDataOut, bytesToRead, &bytesRead)) {
            return bytesRead;
        }

        return 0;
    }

    static void app_config_pair(void* pUserData, const char* key, const char* value)
    {
        AppConfigData* pData = reinterpret_cast<AppConfigData*>(pUserData);
        assert(pData != NULL);

        printf("key/value: [%s] = [%s]\n", key, value);

        if (strcmp(key, "BaseDirectory") == 0)
        {
            pData->pContext->AddBaseDirectoryRelativeToExe(value);
            return;
        }
    }

    static void app_config_error(void* pUserData, const char* message, unsigned int line)
    {
        AppConfigData* pData = reinterpret_cast<AppConfigData*>(pUserData);
        assert(pData != NULL);

        pData->pContext->LogErrorf("config.cfg:%d %s", line, message);
    }




    Context::Context(GameStateManager &gameStateManager)
        : m_cmdline(),
          m_executableDirectoryAbsolutePath(),
          m_pVFS(nullptr),
          m_pLogFile(nullptr),
          m_pAudioContext(nullptr), m_pAudioPlaybackDevice(nullptr), m_soundWorld(*this),
          m_assetLibrary(),
          m_scriptLibrary(*this), m_particleSystemLibrary(*this), m_prefabLibrary(*this), m_modelLibrary(*this), m_materialLibrary(*this), m_shaderLibrary(*this), m_vertexArrayLibrary(*this), m_textureLibrary(*this),
          m_gameStateManager(gameStateManager),
          isInitialised(false), closing(false),
          eventQueue(), eventQueueLock(NULL),
          eventFilter(nullptr),
          window(nullptr), windowEventHandler(*this),
          script(*this),
          deltaTimeInSeconds(0.0), totalRunninTimeInSeconds(0.0), updateTimer(),
          guiImageManager(), gui(&script, &guiImageManager), guiEventHandler(*this), guiRenderer(*this), gameWindowGUIElement(nullptr),
          paused(false), focused(true),
          isCursorVisible(true),
          isAutoScriptReloadEnabled(false),
          keyDownMap(), mouseButtonDownMap(),
          editor(*this),
          isMouseSmoothingEnabled(false),
          mouseCaptured(false), captureMouseOnReceiveFocus(false), captureMouseOnEditorClose(false), mouseCapturePosX(0), mouseCapturePosY(0),
          mouseCenterX(0), mouseCenterY(0),
          mousePosXBuffer(), mousePosYBuffer(), mousePosBufferIndex(0),
          mousePosX(0), mousePosY(0), mouseMoveLockCounter(0),
          profilerToggleKey(Keys::F11),
          editorToggleKeyCombination(Keys::Shift, Keys::Tab)
#ifdef GT_BUILD_EDITOR
        , m_pEditor(nullptr)
#endif
    {
    }

    Context::~Context()
    {
    }



    bool GameCommandLineProc(const char* key, const char* value, void* pUserData)
    {
        Context* pContext = reinterpret_cast<Context*>(pUserData);
        assert(pContext != nullptr);

        if (strcmp(key, "config") == 0) {
            pContext->GetScript().ExecuteFile(pContext->GetVFS(), value);
            return true;
        }

        return true;
    }


    bool Context::Startup(drutil_cmdline &cmdline)
    {
        // Make the application DPI aware right at the start.
        win32_make_dpi_aware();

        m_cmdline = cmdline;


        // We need to initialize the virtual file system early so we can do things like create logs and cache files.
        m_pVFS = drvfs_create_context();


        // Parse the command line.
        CommandLineData cmdlineData;
        strcpy_s(cmdlineData.relativeLogPath, sizeof(cmdlineData.relativeLogPath), "var/logs/engine.txt");
        drutil_parse_cmdline(&m_cmdline, parse_cmdline_proc, &cmdlineData);


        // Make sure the executable's absolute path is clean for future things.
        drpath_clean(cmdlineData.absoluteExeDirPath, m_executableDirectoryAbsolutePath, sizeof(m_executableDirectoryAbsolutePath));
        drpath_clean(cmdlineData.absoluteExePath, m_executableAbsolutePath, sizeof(m_executableDirectoryAbsolutePath));

        // The directory containing the executable needs to be the lowest-priority base path.
        drvfs_add_base_directory(m_pVFS, m_executableDirectoryAbsolutePath);
            


        // We will need to open the log file as soon as possible, but it needs to be done after ensuring the current directory is set to that of the executable.
        char logpath[DRVFS_MAX_PATH];
        drpath_copy_and_append(logpath, sizeof(logpath), m_executableDirectoryAbsolutePath, cmdlineData.relativeLogPath);

        m_pLogFile = drvfs_open(m_pVFS, logpath, DRVFS_WRITE, 0);
        if (m_pLogFile == NULL) {
            printf("WARNING: Failed to open log file.\n");
        }




        // After moving into the application directory, we need to load up the config file and move into the data directory. From
        // there we can read the user configs and setup the log file.
        //
        // This is different from a user configuration (which are located in the 'configs' folder). The application configuration
        // usually remains constant. It defines things like directories. We won't return false if we fail to open, in which case
        // the game will use defaults.
        AppConfigData cfg;
        cfg.pContext = this;
        cfg.pFile    = drvfs_open(m_pVFS, "config.cfg", DRVFS_READ, 0);
        if (cfg.pFile != NULL) {
            drutil_parse_key_value_pairs(app_config_read, app_config_pair, app_config_error, &cfg);
            drvfs_close(cfg.pFile);
        }
        



        //// Asset Library ////
        if (!m_assetLibrary.Startup(m_pVFS))
        {
            this->LogError("Failed to initialize asset library.");
        }




        //////////////////////////////////////////
        // Audio System
            
        m_pAudioContext = draudio_create_context();
        if (m_pAudioContext == NULL) {
            this->LogError("Failed to create audio system.");
        }

        // TEMP: Print the playback devices.
        unsigned int playbackDeviceCount = draudio_get_output_device_count(m_pAudioContext);
        for (unsigned int iDevice = 0; iDevice < playbackDeviceCount; ++iDevice)
        {
            draudio_device_info info;
            if (draudio_get_output_device_info(m_pAudioContext, iDevice, &info))
            {
                this->Logf("Playback Device (%d) - %s", iDevice, info.description);
            }
        }

        if (playbackDeviceCount > 0) {
            m_pAudioPlaybackDevice = draudio_create_output_device(m_pAudioContext, 0);
        }

        m_soundWorld.Startup();




        // Before we can do any windowing operations we will need to initialise the window management module of GTLib.
        StartupWindowManager();


        // With the log file created, we can startup all of our other sub-systems.
        this->Logf("Starting Rendering Sub-System...");
        if (Renderer::Startup())
        {
            this->Logf("Renderer Caps:");
            this->Logf("    Max Color Attachments: %d", Renderer::GetMaxColourAttachments());
            this->Logf("    Max Draw Buffers:      %d", Renderer::GetMaxDrawBuffers());
            this->Logf("    Max Texture Units:     %d", Renderer::GetMaxTextureUnits());

            this->Logf("Loading Shaders...");
            m_shaderLibrary.LoadFromDirectory("engine/shaders/glsl");
            m_shaderLibrary.LoadFromDirectory("shaders/glsl");
        }
        else
        {
            return false;
        }


        // With sub-systems started up, we can startup our resource libraries.
        this->Logf("Initializing Texture Library...");
        m_textureLibrary.Startup();

        this->Logf("Initializing Vertex Array Library...");
        m_vertexArrayLibrary.Startup();

        this->Logf("Initializing Material Library...");
        m_materialLibrary.Startup();

        this->Logf("Initializing Model Library...");
        m_modelLibrary.Startup();

        this->Logf("Initializing Prefab Library...");
        m_prefabLibrary.Startup();

        this->Logf("Initializing Particle System Library...");
        m_particleSystemLibrary.Startup();

        this->Logf("Initializing Script Library...");
        m_scriptLibrary.Startup();




        // The first thing we do is load up the scripting environment. We do this first because it will contain configuration properties
        // for things later on.
        if (this->script.Startup())
        {
            this->gui.Startup();
            this->guiRenderer.Startup();


            this->eventQueueLock = drutil_create_mutex();

            // The main game window GUI element needs to be created. It is just a 100% x 100% invisible element off the root element.
            this->gui.Load("<div id='MainGameWindow' style='width:100%; height:100%' />");
            this->gameWindowGUIElement = this->gui.GetElementByID("MainGameWindow");

            this->gui.SetRenderer(this->guiRenderer);






            // We give the game an opportunity to load configs before processing --config arguments.
            m_gameStateManager.OnLoadConfigs(*this);

            // This is where the user config scripts are loaded.
            drutil_parse_cmdline(&this->GetCommandLine(), GameCommandLineProc, this);


            // Here we will set the default anistropy for textures via the texture library.
            m_textureLibrary.SetDefaultAnisotropy(static_cast<unsigned int>(this->script.GetInteger("GTEngine.Display.Textures.Anisotropy")));


            // First we need a window. Note that we don't show it straight away - it'll be shown at the start of Run().
            this->window = Renderer::CreateWindow();
            if (this->window != nullptr)
            {
                // We'll want to set a few window properties before showing it... We want to show the window relatively early to make
                // the game feel a little bit more speedy, even though it's not really.
                this->window->SetTitle("GTEngine Game");
                this->window->SetSize(this->script.GetInteger("GTEngine.Display.Width"), this->script.GetInteger("GTEngine.Display.Height"));

                // Now we can set the window's event handler and show it.
                this->window->SetEventHandler(this->windowEventHandler);
                


                // Here we initialise the GUI. We need a font server for this, so it needs to be done after initialising fonts.
                this->Logf("Loading GUI...");
                if (!this->InitialiseGUI())
                {
                    this->Logf("Error loading GUI.");
                }


                // Here is where we let the game object do some startup stuff.
                if (m_gameStateManager.OnStartup(*this))
                {
                    this->script.Execute("Game.OnStartup();");
                }
                else
                {
                    // OnStartup() has returned false somewhere.
                    return false;
                }
            }
            else
            {
                // We couldn't create a window, which means the renderer is not usable...
                this->LogErrorf("Error initialising renderer.");
                return false;
            }
        }
        else
        {
            this->LogErrorf("Error initialising scripting environment.");
            return false;
        }



        return true;
    }

    bool Context::Startup(int argc, char** argv)
    {
        drutil_cmdline cmdline;
        if (drutil_init_cmdline(&cmdline, argc, argv))
        {
            return this->Startup(cmdline);
        }

        return false;
    }

    bool Context::Startup(const char* cmdlineWin32)
    {
        drutil_cmdline cmdline;
        if (drutil_init_cmdline_win32(&cmdline, cmdlineWin32))
        {
            return this->Startup(cmdline);
        }

        return false;
    }

    bool Context::Startup()
    {
        drutil_cmdline cmdline;
        memset(&cmdline, 0, sizeof(cmdline));

        return this->Startup(cmdline);
    }


    void Context::Shutdown()
    {
        // We first let the game know that we are shutting down. It's important that we do this before killing anything.
        m_gameStateManager.OnShutdown(*this);
        this->script.Execute("Game.OnShutdown();");     // <-- TODO: Don't use this inline style calling. Instead, properly call it through the C++ API.

        delete this->window;



        //////////////////////////////////////////
        // Audio System

        m_soundWorld.Shutdown();

        draudio_delete_output_device(m_pAudioPlaybackDevice);
        m_pAudioPlaybackDevice = nullptr;

        draudio_delete_context(m_pAudioContext);
        m_pAudioContext = nullptr;



        drutil_delete_mutex(this->eventQueueLock);




        // We kill our libraries before the major sub-systems.
        m_scriptLibrary.Shutdown();
        m_particleSystemLibrary.Shutdown();
        m_prefabLibrary.Shutdown();

        m_modelLibrary.Shutdown();
        m_materialLibrary.Shutdown();
        m_shaderLibrary.Shutdown();
        m_vertexArrayLibrary.Shutdown();
        m_textureLibrary.Shutdown();     
        

        // We shutdown major sub-systems before logging. This allows us to log shutdown info.
        Renderer::Shutdown();


        // GTLib's window management module.
        ShutdownWindowManager();
    }



    ////////////////////////////////////////////////////
    // File System Management

    const char* Context::GetExecutableAbsolutePath() const
    {
        return m_executableAbsolutePath;
    }

    const char* Context::GetExecutableDirectoryAbsolutePath() const
    {
        return m_executableDirectoryAbsolutePath;
    }

    void Context::AddBaseDirectoryRelativeToExe(const char* relativePath)
    {
        char absolutePath[DRVFS_MAX_PATH];
        drpath_to_absolute(relativePath, this->GetExecutableDirectoryAbsolutePath(), absolutePath, sizeof(absolutePath));

        drvfs_insert_base_directory(m_pVFS, absolutePath, drvfs_get_base_directory_count(m_pVFS) - 1);
    }



    ////////////////////////////////////////////////////
    // Messages

    void Context::Log(const char* message)
    {
        // Write to the log file.
        if (m_pLogFile != NULL)
        {
            char dateTime[64];
            drutil_datetime_short(drutil_now(), dateTime, sizeof(dateTime));

            drvfs_write_string(m_pLogFile, "[");
            drvfs_write_string(m_pLogFile, dateTime);
            drvfs_write_string(m_pLogFile, "]");
            drvfs_write_line  (m_pLogFile, message);
            drvfs_flush(m_pLogFile);
        }

        // Post to the terminal.
        printf("%s\n", message);
    }

    void Context::Logf(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        {
            char msg[4096];
            vsnprintf(msg, sizeof(msg), format, args);
        
            this->Log(msg);
        }
        va_end(args);
    }

    void Context::LogMessage(const char* message)
    {
        this->Logf("[MESSAGE] %s", message);
    }

    void Context::LogMessagef(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        {
            char msg[4096];
            vsnprintf(msg, sizeof(msg), format, args);
        
            this->LogMessage(msg);
        }
        va_end(args);
    }

    void Context::LogWarning(const char* message)
    {
        this->Logf("[ERROR] %s", message);
    }

    void Context::LogWarningf(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        {
            char msg[4096];
            vsnprintf(msg, sizeof(msg), format, args);
        
            this->LogWarning(msg);
        }
        va_end(args);
    }

    void Context::LogError(const char* message)
    {
        this->Logf("[ERROR] %s", message);
    }

    void Context::LogErrorf(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        {
            char msg[4096];
            vsnprintf(msg, sizeof(msg), format, args);
        
            this->LogError(msg);
        }
        va_end(args);
    }



    ////////////////////////////////////////////////////
    // Audio

    draudio_context* Context::GetAudioContext()
    {
        return m_pAudioContext;
    }
        
    draudio_device* Context::GetAudioPlaybackDevice()
    {
        return m_pAudioPlaybackDevice;
    }

    GT::SoundWorld & Context::GetSoundWorld()
    {
        return m_soundWorld;
    }



    ////////////////////////////////////////////////////
    // Assets

    AssetLibrary & Context::GetAssetLibrary()
    {
        return m_assetLibrary;
    }








    /////////////////////////////////////////////////
    // FROM GAME
    /////////////////////////////////////////////////

    int Context::Run()
    {
#ifdef GT_BUILD_EDITOR
        // We need to check if "--editor" was passed to the command line, and if so, open the editor.
        if (this->IsEditorOnCommandLine())
        {
            m_pEditor = new Editor2(*this);
            int result = m_pEditor->StartupAndRun();

            delete m_pEditor;
            m_pEditor = nullptr;

            return result;
        }
#endif

        // If we make it here it means we are not running the editor and are running the game like normal. We need to just show the
        // window and the enter into the main loop.
        this->window->Show();

        while (!this->closing)
        {
            // First we need to handle any pending window messages. We do not want to wait here (first argument).
            while (PumpNextWindowEvent(false));


            // We want our events to be handled synchronously on the main thread.
            this->HandleEvents();


            // TODO: Remove this little section when the new GUI event handling system is in place.
            // The GUI events can be handled here. If it turns out that they're best handled on the update thread, all we need
            // to do is remove this line - GUIServer::Step() will also handle any pending events. I like to handle the GUI
            // events from here because it's nice to have GUI events handled at the same time as window events, since they're
            // kind of related.
            //this->gui.HandleEvents();


            // Here we want to start the next frame.
            this->DoFrame();
        }

        // If we made it here, it means the game was run and closed normally.
        return 0;
    }

    void Context::Close()
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



    void Context::SendEvent(const GameEvent &e)
    {
        drutil_lock_mutex(this->eventQueueLock);
        {
            this->eventQueue.Push(e);
        }
        drutil_unlock_mutex(this->eventQueueLock);
    }


    void Context::SetEventFilter(GameEventFilter* filter)
    {
        this->eventFilter = filter;
    }

    GameEventFilter* Context::GetEventFilter()
    {
        return this->eventFilter;
    }



    GUIServer & Context::GetGUI()
    {
        return this->gui;
    }

    void Context::StepGUI()
    {
        this->StepGUI(this->deltaTimeInSeconds);
    }

    void Context::StepGUI(double deltaTimeInSecondsIn)
    {
        this->gui.Step(deltaTimeInSecondsIn);
        this->gui.Paint();
    }


    void Context::EnableFullscreen()
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

    void Context::DisableFullscreen()
    {
        if (this->window != nullptr)
        {
            this->window->DisableFullscreen();
            Renderer::SetCurrentWindow(this->window);
        }
    }

    bool Context::IsFullscreen() const
    {
        if (this->window != nullptr)
        {
            return this->window->IsFullscreen();
        }

        return false;
    }


    void Context::RefreshDisplay()
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


    void Context::GetMousePosition(int &x, int &y)
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


    void Context::GetMouseOffset(float &x, float &y)
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

    void Context::SetMousePosition(int x, int y)
    {
        if (this->window != nullptr)
        {
            this->window->SetMousePosition(x, y);
        }
    }


    void Context::EnableMouseSmoothing()
    {
        this->isMouseSmoothingEnabled = true;
    }

    void Context::DisableMouseSmoothing()
    {
        this->isMouseSmoothingEnabled = false;
    }

    bool Context::IsMouseSmoothingEnabled() const
    {
        return this->isMouseSmoothingEnabled;
    }


    void Context::CaptureMouse()
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

    void Context::ReleaseMouse()
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

    void Context::ShowCursor()
    {
        if (!this->isCursorVisible)
        {
            this->window->ShowCursor();
            this->isCursorVisible = true;
        }
    }

    void Context::HideCursor()
    {
        if (this->isCursorVisible)
        {
            this->window->HideCursor();
            this->isCursorVisible = false;
        }
    }


    bool Context::IsKeyDown(Key key) const
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

    bool Context::IsMouseButtonDown(MouseButton button) const
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


    void Context::CacheMousePosition()
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


    void Context::Pause()
    {
        if (!this->paused)
        {
            this->paused = true;
            m_gameStateManager.OnPause(*this);

            this->PostScriptEvent_OnPause();
        }
    }

    void Context::Resume()
    {
        if (this->paused)
        {
            this->paused = false;
            m_gameStateManager.OnResume(*this);

            this->PostScriptEvent_OnResume();
        }
    }


    void Context::OpenEditor()
    {
        if (m_gameStateManager.OnEditorOpening(*this))
        {
            // The main game window GUI element needs to be hidden.
            this->gameWindowGUIElement->Hide();


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
    }

    void Context::CloseEditor()
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

            m_gameStateManager.OnEditorClose(*this);
        }
    }

    bool Context::IsEditorOpen() const
    {
        return this->editor.IsOpen();
    }


    bool Context::ExecuteScript(const char* scriptIn)
    {
        return this->script.Execute(scriptIn);
    }




    void Context::EnableScriptAutoReload()
    {
        this->isAutoScriptReloadEnabled = true;
    }

    void Context::DisableScriptAutoReload()
    {
        this->isAutoScriptReloadEnabled = false;
    }



    bool Context::PackageForDistribution(const char* outputDirectory, const char* executableName)
    {
        char absoluteOutputDirectory[DRVFS_MAX_PATH];
        drpath_copy_and_append(absoluteOutputDirectory, sizeof(absoluteOutputDirectory), this->GetExecutableDirectoryAbsolutePath(), outputDirectory);

        // We will start by creating the output directory.
        if (!drvfs_is_existing_directory(this->GetVFS(), absoluteOutputDirectory))
        {
            if (!drvfs_create_directory(this->GetVFS(), absoluteOutputDirectory))
            {
                // Failed to create the output directory.
                return false;
            }
        }

        GamePackager packager(absoluteOutputDirectory);


        // We will start by copying over the data directories, not including the executable directory.
        assert(drvfs_get_base_directory_count(this->GetVFS()) > 0);
        for (unsigned int iBaseDir = 0; iBaseDir < drvfs_get_base_directory_count(this->GetVFS()) - 1; ++iBaseDir)     // -1 because we want to ignore the executable directory.
        {
            packager.CopyDataDirectory(drvfs_get_base_directory_by_index(this->GetVFS(), iBaseDir));
        }


        if (drpath_extension_equal(this->GetExecutableAbsolutePath(), "exe"))
        {
            if (drpath_extension_equal(executableName, "exe"))
            {
                packager.CopyExecutable(this->GetExecutableAbsolutePath(), executableName);
            }
            else
            {
                packager.CopyExecutable(this->GetExecutableAbsolutePath(), (String(executableName) + ".exe").c_str());
            }
        }
        else
        {
            packager.CopyExecutable(this->GetExecutableAbsolutePath(), executableName);
        }

        packager.WriteConfig();

        return true;
    }




    Key Context::GetProfilerToggleKey() const
    {
        return this->profilerToggleKey;
    }

    void Context::SetProfilerToggleKey(Key key)
    {
        this->profilerToggleKey = key;
    }


    const KeyCombination & Context::GetEditorToggleKeyCombination() const
    {
        return this->editorToggleKeyCombination;
    }

    void Context::SetEditorToggleKeyCombination(const KeyCombination &newCombination)
    {
        this->editorToggleKeyCombination = newCombination;
    }




    Component* Context::CreateCustomComponent(const char*, SceneNode &) const
    {
        return nullptr;
    }



    /////////////////////////////////////
    // Game State Management

    bool Context::SerializeGameState(Serializer &serializer)
    {
        return m_gameStateManager.Serialize(*this, serializer);
    }

    bool Context::DeserializeGameState(Deserializer &deserializer)
    {
        return m_gameStateManager.Deserialize(*this, deserializer);
    }

    bool Context::SaveGameState(const char* destinationFilePath)
    {
        drvfs_file* pFile = drvfs_open(this->GetVFS(), destinationFilePath, DRVFS_WRITE | DRVFS_CREATE_DIRS, 0);
        if (pFile != nullptr)
        {
            FileSerializer serializer(pFile);
            bool result = this->SerializeGameState(serializer);

            drvfs_close(pFile);
            return result;
        }

        return false;
    }

    bool Context::LoadGameState(const char* sourceFilePath)
    {
        drvfs_file* pFile = drvfs_open(this->GetVFS(), sourceFilePath, DRVFS_READ, 0);
        if (pFile != nullptr)
        {
            FileDeserializer deserializer(pFile);
            bool result = this->DeserializeGameState(deserializer);

            drvfs_close(pFile);
            return result;
        }

        return false;
    }

    bool Context::LoadScene(const char* sceneRelativePath)
    {
        return m_gameStateManager.LoadScene(*this, sceneRelativePath);
    }





    /////////////////////////////////////////////
    // Private

    static bool IsEditorOnCommandLineProc(const char* key, const char* value, void* pUserData)
    {
        (void)value;

        if (strcmp(key, "editor") == 0)
        {
            *((bool*)pUserData) = true;
            return false;
        }

        return true;
    }

    bool Context::IsEditorOnCommandLine()
    {
        bool onCmdLine = false;
        drutil_parse_cmdline(&m_cmdline, IsEditorOnCommandLineProc, &onCmdLine);

        return onCmdLine;
    }

    bool Context::InitialiseGUI()
    {
        this->gui.SetEventHandler(this->guiEventHandler);
        return true;
    }

    void Context::DoFrame()
    {
        // The first thing we do is retrieve the delta time...
        this->deltaTimeInSeconds = Min(this->updateTimer.Update(), 1.0);

        // We also need to increment the total running time, but only if we're not paused.
        if (!this->IsPaused())
        {
            this->totalRunninTimeInSeconds += this->deltaTimeInSeconds;
        }


        // We need to cache the current position of the mouse. We need to do this in order to get smooth mouse movement.
        this->CacheMousePosition();


        // Now we let the game know that we're starting the frame.
        m_gameStateManager.OnStartFrame(*this);


        // Now we just run the job without attempting to block (second argument).
        this->Update();

        // Draw the frame after updating.
        this->Draw();


        // Now we can let the game know that we've finished the frame...
        m_gameStateManager.OnEndFrame(*this);
    }

    void Context::Update()
    {
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
    }

    void Context::Draw()
    {
        // NOTE:
        //
        // We're not currently calling any OnDraw events. The problem is with the multi-threading nature of the engine. Events here are called from a different thread
        // to other events, so it's not a trivial matter of simply calling the function without any synchronization.

        // At this point we can finally swap the buffers.
        Renderer::SwapBuffers();
    }


    bool Context::IsKeyCombinationDown(const KeyCombination &combination) const
    {
        // We ignore keys that are set to null, but we need to always return false if all of them are null.
        if (!combination.IsAnyKeySet())
        {
            return false;
        }


        bool down = true;

        down = down && (combination.systemKey1    == Keys::Null || this->IsKeyDown(combination.systemKey1));
        down = down && (combination.systemKey2    == Keys::Null || this->IsKeyDown(combination.systemKey2));
        down = down && (combination.printableKey1 == Keys::Null || this->IsKeyDown(combination.printableKey1));
        down = down && (combination.printableKey2 == Keys::Null || this->IsKeyDown(combination.printableKey2));

        return down;
    }


    void Context::HandleEvents()
    {
        GUIEventContext eventContext = this->gui.BeginPostingEvents();

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

    void Context::HandleEvent_OnSize(GameEvent &e)
    {
        this->gui.SetViewportSize(e.size.width, e.size.height);

        if (this->eventFilter == nullptr || this->eventFilter->OnSize(e.size.width, e.size.height))
        {
            m_gameStateManager.OnSize(*this, e.size.width, e.size.height);
            this->PostScriptEvent_OnSize(e);
        }
    }

    void Context::HandleEvent_OnMouseMove(GameEvent &e, GUIEventContext eventContext)
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

    void Context::HandleEvent_OnMouseWheel(GameEvent &e, GUIEventContext eventContext)
    {
        this->gui.OnMouseWheel(eventContext, e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);

        if (this->eventFilter == nullptr || this->eventFilter->OnMouseWheel(e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y))
        {
            m_gameStateManager.OnMouseWheel(*this, e.mousewheel.delta, e.mousewheel.x, e.mousewheel.y);
            this->PostScriptEvent_OnMouseWheel(e);
        }
    }

    void Context::HandleEvent_OnMouseButtonDown(GameEvent &e, GUIEventContext eventContext)
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

    void Context::HandleEvent_OnMouseButtonUp(GameEvent &e, GUIEventContext eventContext)
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

    void Context::HandleEvent_OnMouseButtonDoubleClick(GameEvent &e, GUIEventContext eventContext)
    {
        this->gui.OnMouseButtonDoubleClick(eventContext, static_cast<int>(e.mousedoubleclick.button));


        if (this->eventFilter == nullptr || this->eventFilter->OnMouseButtonDoubleClick(e.mousedoubleclick.button, e.mousedoubleclick.x, e.mousedoubleclick.y))
        {
            m_gameStateManager.OnMouseButtonDoubleClick(*this, e.mousedoubleclick.button, e.mousedoubleclick.x, e.mousedoubleclick.y);
            this->PostScriptEvent_OnMouseButtonDoubleClick(e);
        }
    }

    void Context::HandleEvent_OnKeyPressed(GameEvent &e, GUIEventContext eventContext)
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
    }

    void Context::HandleEvent_OnKeyReleased(GameEvent &e, GUIEventContext eventContext)
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

    void Context::HandleEvent_OnKeyDown(GameEvent &e, GUIEventContext eventContext)
    {
        this->gui.OnKeyDown(eventContext, e.keydown.key);


        if (this->eventFilter == nullptr || this->eventFilter->OnKeyDown(e.keydown.key))
        {
            m_gameStateManager.OnKeyDown(*this, e.keydown.key);
            this->PostScriptEvent_OnKeyDown(e);
        }
    }

    void Context::HandleEvent_OnKeyUp(GameEvent &e, GUIEventContext eventContext)
    {
        this->gui.OnKeyUp(eventContext, e.keyup.key);


        if (this->eventFilter == nullptr || this->eventFilter->OnKeyUp(e.keyup.key))
        {
            m_gameStateManager.OnKeyUp(*this, e.keyup.key);
            this->PostScriptEvent_OnKeyUp(e);
        }
    }

    void Context::HandleEvent_OnReceiveFocus(GameEvent &e)
    {
        this->focused = true;

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

    void Context::HandleEvent_OnLoseFocus(GameEvent &e)
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


    void Context::PostScriptEvent_OnUpdate(double deltaTimeInSecondsIn)
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

    void Context::PostScriptEvent_OnSize(const GameEvent &e)
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

    void Context::PostScriptEvent_OnMouseMove(const GameEvent &e)
    {
        GT::PostEvent_OnMouseMove(this->script, e.mousemove.x, e.mousemove.y);
    }

    void Context::PostScriptEvent_OnMouseWheel(const GameEvent &e)
    {
        GT::PostEvent_OnMouseWheel(this->script, e.mousewheel.x, e.mousewheel.y, e.mousewheel.delta);
    }

    void Context::PostScriptEvent_OnMouseButtonDown(const GameEvent &e)
    {
        GT::PostEvent_OnMouseButtonDown(this->script, e.mousedown.x, e.mousedown.y, e.mousedown.button);
    }

    void Context::PostScriptEvent_OnMouseButtonUp(const GameEvent &e)
    {
        GT::PostEvent_OnMouseButtonUp(this->script, e.mouseup.x, e.mouseup.y, e.mouseup.button);
    }

    void Context::PostScriptEvent_OnMouseButtonDoubleClick(const GameEvent &e)
    {
        GT::PostEvent_OnMouseButtonDoubleClick(this->script, e.mousedoubleclick.x, e.mousedoubleclick.y, e.mousedoubleclick.button);
    }

    void Context::PostScriptEvent_OnKeyPressed(const GameEvent &e)
    {
        GT::PostEvent_OnKeyPressed(this->script, e.keypressed.key);
    }

    void Context::PostScriptEvent_OnKeyReleased(const GameEvent &e)
    {
        GT::PostEvent_OnKeyReleased(this->script, e.keyreleased.key);
    }

    void Context::PostScriptEvent_OnKeyDown(const GameEvent &e)
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

    void Context::PostScriptEvent_OnKeyUp(const GameEvent &e)
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

    void Context::PostScriptEvent_OnReceiveFocus(const GameEvent &)
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

    void Context::PostScriptEvent_OnLoseFocus(const GameEvent &)
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

    void Context::PostScriptEvent_OnPause()
    {
        GT::PostEvent_OnGamePause(this->script);
    }

    void Context::PostScriptEvent_OnResume()
    {
        GT::PostEvent_OnGameResume(this->script);
    }
}
