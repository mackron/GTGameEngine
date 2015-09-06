// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultGameState
#define GT_DefaultGameState

#include "GameState.hpp"
#include "EngineContext.hpp"
#include "GameContext.hpp"

namespace GT
{
    /// Helper class for game states.
    ///
    /// This can be used as the basis for most games. This will create the engine and game context in Startup().
    class DefaultGameState : public GameState
    {
    public:

        /// Constructor.
        DefaultGameState();

        /// Destructor.
        ~DefaultGameState();


        /// Starts up the game.
        bool Startup(int argc, char** argv);

        /// Shuts down the game.
        void Shutdown();

        /// Begins the game loop and returns the exit code.
        ///
        /// @remarks
        ///     Upon returning, this does NOT shut down the game with Shutdown() - that must be done manually.
        int Run();


        /// Retrieves a pointer to the engine context.
        EngineContext* GetEngineContext() { return m_pEngineContext; }

        /// Retrieves a reference to the game context.
        GameContext* GetGameContext() { return m_pGameContext; }



        ///////////////////////////////////////////
        // Virtual Methods

        /// Called when the game is initializing.
        ///
        /// @remarks
        ///     This is where the game should do things like load the initial scene and whatnot.
        ///     @par
        ///     Return false if startup fails.
        virtual bool OnStartup(const GTLib::CommandLine &commandLine);

        /// Called when the game is closing and needs to be uninitialized.
        virtual void OnShutdown();

        /// Called when the game is being stepped/updated.
        ///
        /// @param dt [in] The in seconds since the previous frame.
        virtual void OnStep(double dt);



        ///////////////////////////////////////////
        // Inherited from GT::GameState

        void Step(GT::GameContext &gameContext, double deltaTimeInSeconds);
        bool OnStarting(GT::GameContext &gameContext, const GTLib::CommandLine &commandLine);
        void OnClosing(GT::GameContext &gameContext);
    #if 0
        unsigned int GetWindowCount(GT::GameContext &gameContext) const;
        GT::HWindow GetWindowByIndex(GT::GameContext &gameContext, unsigned int index);
        void OnPause(GT::GameContext &gameContext);
        void OnResume(GT::GameContext &gameContext);
        void OnMouseMove(GT::GameContext &gameContext, GT::HWindow hWindow, int xPos, int yPos);
        void OnMouseEnter(GT::GameContext &gameContext, GT::HWindow hWindow);
        void OnMouseLeave(GT::GameContext &gameContext, GT::HWindow hWindow);
        void OnMouseButtonPressed(GT::GameContext &gameContext, GT::HWindow hWindow, int button, int xPos, int yPos);
        void OnMouseButtonReleased(GT::GameContext &gameContext, GT::HWindow hWindow, int button, int xPos, int yPos);
        void OnMouseButtonDoubleClicked(GT::GameContext &gameContext, GT::HWindow hWindow, int button, int xPos, int yPos);
        void OnMouseWheel(GT::GameContext &gameContext, GT::HWindow hWindow, int delta, int xPos, int yPos);
        void OnKeyPressed(GT::GameContext &gameContext, GT::HWindow hWindow, GTLib::Key key);
        void OnKeyReleased(GT::GameContext &gameContext, GT::HWindow hWindow, GTLib::Key key);
        void OnWindowResized(GT::GameContext &gameContext, GT::HWindow hWindow, unsigned int width, unsigned int height);
    #endif


    private:

        /// A pointer to the engine context. This is created in Startup()
        EngineContext* m_pEngineContext;

        /// A pointer to the game context. This is created in Startup()
        GameContext* m_pGameContext;


    private:    // No copying.
        DefaultGameState(const DefaultGameState &);
        DefaultGameState & operator=(const DefaultGameState &);
    };
}

#endif
