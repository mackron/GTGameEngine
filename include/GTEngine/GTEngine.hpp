
#ifndef __GTEngine_hpp_
#define __GTEngine_hpp_

#include "Game.hpp"

namespace GTEngine
{
    extern Game* GlobalGame;


    /// Starts up the engine.
    ///
    /// @param argc [in] The argument count.
    /// @param argv [in] The argument strings.
    ///
    /// @return True if the engine is initialised successfully; false otherwise.
    bool _PreStartup(int argc, char** argv);


    /// Starts up the engine, returning an instance of the given game class (T).
    ///
    /// @param argc [in] The argument count from main().
    /// @param argv [in] The argument strings from main().
    ///
    /// @return A pointer to the main game object.
    ///
    /// @remarks
    ///     The 'T' template argument is the type of the game object to create. This will use the default constructor. Thus, the game
    ///     object MUST use a default constructor.
    template <typename T>
    T* Startup(int argc, char** argv)
    {
        if (GlobalGame == nullptr)
        {
            if (_PreStartup(argc, argv))
            {
                GlobalGame = new T;
                if (!GlobalGame->Startup(argc, argv))
                {
                    delete GlobalGame;
                    GlobalGame = nullptr;
                }
            }
        }

        return static_cast<T*>(GlobalGame);
    }

    /// Shuts down the engine, deleting the given game object returned by Startup().
    ///
    /// @param game [in] The game object returned by Startup().
    void Shutdown(Game* game);

    


    /// Retrieves the directory containing the executable.
    ///
    /// @return The directory containing the executable.
    const char* GetExecutableDirectory();


    // TODO: Is this a good spot for this? Not sure where to put it...
    /// Creates an instantiation of a component based on it's name.
    ///
    /// @param componentName [in] The name of the comonent to instantiate.
    /// @param hostSceneNode [in] The scene node to associate with the component.
    ///
    /// @return A pointer to the new component if successful, null otherwise.
    ///
    /// @remarks
    ///     The component will be created with 'new'. Delete it with 'delete'
    ///     @par
    ///     If 'name' does not correspond to a component defined by the engine, this will call Game::CreateCustomComponent().
    Component* CreateComponentByName(const char* componentName, SceneNode &hostSceneNode);
}

#endif
