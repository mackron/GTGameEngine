// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GT_Engine_GameStateManager_hpp_
#define __GT_Engine_GameStateManager_hpp_

#include <GTLib/Serializer.hpp>
#include <GTLib/Deserializer.hpp>
#include <GTLib/Dictionary.hpp>
#include <GTLib/CommandLine.hpp>
#include <GTEngine/GameEvent.hpp>

namespace GTEngine
{
    class  Game;
    class  Scene;
    struct SceneStateStackRestoreCommands;


    /// Base class for managing the state of a game.
    ///
    /// Every game is different, so it may need to be managed slightly different. The main purpose behind this class is saving and
    /// loading the state of a game - i.e. doing save games.
    ///
    /// The game state manager must be able to serialize and deserialize the game such that necessary information can be saved to
    /// a file, and the game can be loaded in an accurate enough state to do a save game.
    ///
    /// A game state manager needs to be persistent across the entire lifetime of the game
    class GameStateManager
    {
    public:

        /// Destructor.
        virtual ~GameStateManager();


        ///////////////////////////////////////
        // Events

        /// Called right at the start of initialization when the config scripts are being loaded.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This is called right at the start of initialisation just before the --config arguments are processed. This is the
        ///     first event to be called, before OnStartup() and before anything else is initialised.
        ///     @par
        ///     This is the best place to load configs. However, since this function is called before everything is initialised,
        ///     the configs should only use simple functionality like simple variable assignment. Complex scripts should be run
        ///     in OnStartup() or later.
        virtual void OnLoadConfigs(Game &game);

        /// Called when a game is first started.
        ///
        /// @param game        [in] A reference to the applicable game object.
        /// @param commandLine [in] The command line arguments that were passed to the game.
        ///
        /// @return True if the game is started up successfully; false otherwise.
        ///
        /// @remarks
        ///     This is called after OnLoadConfigs() and after the window has been created and shown.
        virtual bool OnStartup(Game &game, const GTLib::CommandLine &commandLine);

        /// Called when the game is shutting down.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This the last event to ever be called. It is called before the engine's context is destroyed.
        virtual void OnShutdown(Game &game);


        /// Called when the game is updated.
        ///
        /// @param game               [in] A reference to the applicable game object.
        /// @param deltaTimeInSeconds [in] The time since the last frame, in seconds.
        ///
        /// @remarks
        ///     This is called from the update thread, once per frame. This is were you'll want to step the current scene.
        virtual void OnUpdate(Game &game, double deltaTimeInSeconds);

        /// Called just before the next frame has started.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This function is completely synchronised. Neither the update nor the rendering thread is running at this point. It will be
        ///     executed on the main thread.
        virtual void OnStartFrame(Game &game);

        /// Called after the current frame has finished.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This function is completely synchronised. Neither the update nor the rendering thread is running at this point. It will be
        ///     executed on the main thread.
        virtual void OnEndFrame(Game &game);


        /// Called when the game window is resized.
        ///
        /// @param game      [in] A reference to the applicable game object.
        /// @param newWidth  [in] The new width of the game window.
        /// @param newHeight [in] The new height of the game window.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnSize(Game &game, unsigned int newWidth, unsigned int newHeight);


        /// Called when the mouse is moved.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param xPos [in] The new position of the cursor on the x axis.
        /// @param yPos [in] The new position of the cursor on the y axis.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnMouseMove(Game &game, int xPos, int yPos);

        /// Called when the mouse wheel is turned.
        ///
        /// @param game  [in] A reference to the applicable game object.
        /// @param delta [in] The mouse wheel delta specifying the direction of the turn.
        /// @param xPos  [in] The position of the cursor on the x axis.
        /// @param yPos  [in] The position of the cursor on the y axis.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnMouseWheel(Game &game, int delta, int xPos, int yPos);

        /// Called when a mouse button is pressed.
        ///
        /// @param game   [in] A reference to the applicable game object.
        /// @param button [in] A number representing the button that was pressed.
        /// @param xPos   [in] The position of the cursor on the x axis.
        /// @param yPos   [in] The position of the cursor on the y axis.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     Standard button indices are:
        ///       - Left = 1
        ///       - Right = 2
        ///       - Middle = 3
        virtual void OnMouseButtonDown(Game &game, int button, int xPos, int yPos);

        /// Called when a mouse button is raised.
        ///
        /// @param game   [in] A reference to the applicable game object.
        /// @param button [in] A number representing the button that was raised.
        /// @param xPos   [in] The position of the cursor on the x axis.
        /// @param yPos   [in] The position of the cursor on the y axis.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     Standard button indices are:
        ///       - Left = 1
        ///       - Right = 2
        ///       - Middle = 3
        virtual void OnMouseButtonUp(Game &game, int button, int xPos, int yPos);

        /// Called when a mouse button is double-clicked.
        ///
        /// @param game   [in] A reference to the applicable game object.
        /// @param button [in] A number representing the button that was double-clicked.
        /// @param xPos   [in] The position of the cursor on the x axis.
        /// @param yPos   [in] The position of the cursor on the y axis.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     Standard button indices are:
        ///       - Left = 1
        ///       - Right = 2
        ///       - Middle = 3
        virtual void OnMouseButtonDoubleClick(Game &game, int button, int xPos, int yPos);


        /// Called when a key is initially pressed.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param key  [in] The key code representing the key that was pressed.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     The difference between this and OnKeyDown is that OnKeyDown will handle auto-repeat, whereas OnKeyPressed will only
        ///     consider the initial press. OnKeyDown will also be called on the initial press, in which case it will be called after
        ///     OnKeyPressed.
        virtual void OnKeyPress(Game &game, GTLib::Key key);

        /// Called when a key is released.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param key  [in] The key code representing the key that was released.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     There is no difference between this and OnKeyUp, except that OnKeyUp will be called after OnKeyReleased.
        virtual void OnKeyRelease(Game &game, GTLib::Key key);

        /// Called when a key is pressed or auto-repeated.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param key  [in] The key code representing the key that was pressed.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     The difference between this and OnKeyPressed is that OnKeyPressed does not handle auto-repeat, whereas OnKeyDown
        ///     does. Both OnKeyDown and OnKeyPressed will be called on the initial press.
        virtual void OnKeyDown(Game &game, GTLib::Key key);

        /// Called when a key is released.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param key  [in] The key code representing the key that was released.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     There is no difference between this and OnKeyReleased, except that OnKeyUp will be called after OnKeyReleased.
        virtual void OnKeyUp(Game &game, GTLib::Key key);


        /// Called when the game window receives focus.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnReceiveFocus(Game &game);

        /// Called when the game window loses focus.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnLoseFocus(Game &game);


        /// Called when an event unknown to the engine is handled.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param e    [in] A reference to the event object.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     You will most likely never need to implement this function.
        virtual void OnHandleEvent(Game &game, GameEvent &e);


        /// Called when the game is paused.
        ///
        /// @param game [in] A reference to the applicable game object.
        virtual void OnPause(Game &game);

        /// Called when the game is resumed from a paused state.
        ///
        /// @param game [in] A reference to the applicable game object.
        virtual void OnResume(Game &game);


        /// Called just before the editor is opened.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @return True to allow the editor to continue opening; false to prevent the editor from opening.
        virtual bool OnEditorOpening(Game &game);

        /// Called just before the editor is closed.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @return True to allow the editor to continue closing; false to prevent the editor from closing.
        virtual bool OnEditorClosing(Game &game);

        /// Called just after the editor is opened.
        ///
        /// @param game [in] A reference to the applicable game object.
        virtual void OnEditorOpen(Game &game);

        /// Called just after the editor is closed.
        ///
        /// @param game [in] A reference to the applicable game object.
        virtual void OnEditorClose(Game &game);



        ///////////////////////////////////////
        // Methods

        /// Called when a scene needs to be loaded and made current.
        ///
        /// @param game              [in] A reference to the applicable game object.
        /// @param sceneRelativePath [in] The relative path of the scene file to load.
        ///
        /// @return True if the scene is loaded successfully; false otherwise.
        virtual bool LoadScene(Game &game, const char* sceneRelativePath);

        

        ///////////////////////////////////////
        // Serialization

        /// Serializes the game state.
        ///
        /// @param serializer [in] The serializer to write the data to.
        virtual bool Serialize(GTLib::Serializer &serializer);


        ///////////////////////////////////////
        // Deserialization

        /// Deserializes the game state.
        ///
        /// @param deserializer [in] The deserializer to write the data to.
        virtual bool Deserialize(GTLib::Deserializer &deserializer);
    };



    /// A generic implementation of the game state manager that can be used as the basis for most types of games.
    ///
    /// This class has three general groups of data:
    ///     - Header information, such as the name of the scene the player is currently on.
    ///     - Scene data
    ///     - Global game data
    ///
    /// The header information is not customizable. It is intended for the internal working of the manager.
    ///
    /// The scene data is simply, for ever scene, the difference between the base scene (as defined by the scene file) and the point in which the
    /// data was last updated.
    ///
    /// The global game data is for things that don't belong to any individual scene, but is rather a more game wide set of data. An example would
    /// be the player's inventory. It is saved after the scene data so that the game has a chance to change the scene in response to reading global
    /// data.
    ///
    /// When saving scenes, all that's saved is the difference between the base scene and the current point in time. The state of scenes need to
    /// be updated at certain times:
    ///     - When the game is saved
    ///     - When the game changes scenes
    class GenericGameStateManager : public GameStateManager
    {
    public:

        /// Constructor.
        GenericGameStateManager();

        /// Destructor.
        virtual ~GenericGameStateManager();



        /// Updates the serialized data of the given scene and maps it to the specified relative file path.
        ///
        /// @param relativeFilePath [in] The relative file path of the scene.
        /// @param scene            [in] The scene that will be serialized.
        ///
        /// @remarks
        ///     The scene's file path is only used to map the scene's state to a file. It doesn't actually write to or load from the file.
        virtual bool UpdateSceneState(const char* relativeFilePath, Scene &scene);



        ///////////////////////////////////////
        // Serialization

        /// GameStateManager::Serialize()
        virtual bool Serialize(GTLib::Serializer &serializer);



        ///////////////////////////////////////
        // Deserialization

        /// GameStateManager::Deserialize()
        virtual bool Deserialize(GTLib::Deserializer &deserializer);



    private:

        /// Maps a scene's relative file path to it's serialized data.
        GTLib::Dictionary<SceneStateStackRestoreCommands*> m_sceneData;
    };
}

#endif
