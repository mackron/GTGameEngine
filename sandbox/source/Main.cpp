
#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/GameState.hpp>
#include <GTGameEngine/Graphics/DefaultGraphicsWorld.hpp>

class SandboxGameState : public GT::GameState
{
public:

    /// Constructor.
    SandboxGameState()
        : m_pGraphicsAPI(nullptr), 
          m_pGraphicsWorld(nullptr),
          m_hMainWindow(0),
          m_isShiftKeyDown(false)
    {
    }

    /// Destructor.
    ~SandboxGameState()
    {
    }


    unsigned int GetWindowCount(GT::GameContext &gameContext) const
    {
        return 1;
    }

    GT::HWindow GetWindowByIndex(GT::GameContext &gameContext, unsigned int index)
    {
        assert(index == 0);
        return m_hMainWindow;
    }


    void Step(GT::GameContext &gameContext, double deltaTimeInSeconds)
    {
        (void)gameContext;
        (void)deltaTimeInSeconds;

        // TODO: Add support for multi-threaded rendering.
        m_pGraphicsWorld->ExecuteRenderingCommands();
    }



    bool OnStarting(GT::GameContext &gameContext, const GTLib::CommandLine &commandLine)
    {
        // Load the config file.


        // Create the game windows.
        m_hMainWindow = gameContext.CreateMainWindow(GT::WindowType::PrimaryWindow, "GTGameEngine Sandbox", 0, 0, 1280, 720);
        if (m_hMainWindow == 0)
        {
            return false;
        }

        // Show the window.
        gameContext.ShowWindow(m_hMainWindow);


        m_pGraphicsAPI = gameContext.GetEngineContext().GetBestGraphicsAPI();
        assert(m_pGraphicsAPI != nullptr);
        {
            m_pGraphicsWorld = new GT::DefaultGraphicsWorld(gameContext.GetGUI(), *m_pGraphicsAPI);
            if (m_pGraphicsWorld->Startup())
            {
                m_pGraphicsWorld->CreateRenderTargetFromWindow(reinterpret_cast<HWND>(m_hMainWindow), 0);
            }
        }


        return true;
    }


    void OnKeyPressed(GT::GameContext &gameContext, GT::HWindow hWindow, GTLib::Key key)
    {
        if (key == GTLib::Keys::Shift)
        {
            m_isShiftKeyDown = true;
        }

        if (key == GTLib::Keys::Tab && m_isShiftKeyDown)
        {
            if (gameContext.IsEditorOpen())
            {
                gameContext.CloseEditor();
            }
            else
            {
                gameContext.OpenEditor();
            }
        }
    }

    void OnKeyReleased(GT::GameContext &gameContext, GT::HWindow hWindow, GTLib::Key key)
    {
        if (key == GTLib::Keys::Shift)
        {
            m_isShiftKeyDown = false;
        }
    }



private:

    /// The graphics API.
    GT::GraphicsAPI* m_pGraphicsAPI;

    /// The default graphics world.
    GT::DefaultGraphicsWorld* m_pGraphicsWorld;


    /// The main window.
    GT::HWindow m_hMainWindow;

    /// Keeps track of whether or not the shift key is down.
    bool m_isShiftKeyDown;
};


int main(int argc, char** argv)
{
    GT::EngineContext engine;
    if (GT::Succeeded(engine.Startup(argc, argv)))
    {
        SandboxGameState gameState;    
        GT::GameContext game(engine, gameState);
        if (GT::Succeeded(game.Startup()))
        {
            int exitCode = game.Run();


            game.Shutdown();
            engine.Shutdown();

            return exitCode;
        }
        else
        {
            // Error initializing the game.
            engine.Shutdown();
            return 2;
        }
    }
    else
    {
        // Error initializing the engine.
        return 1;
    }    
}
