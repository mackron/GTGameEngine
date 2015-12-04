// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_GameStateManager
#define GT_GameStateManager

#include <GTGE/Core/Serializer.hpp>
#include <GTGE/Core/Deserializer.hpp>
#include <GTGE/Core/Dictionary.hpp>
#include <GTGE/GameEvent.hpp>

namespace GT
{
    class  Context;
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
        virtual void OnLoadConfigs(Context &context);

        /// Called when a game is first started.
        ///
        /// @param game        [in] A reference to the applicable game object.
        /// @param commandLine [in] The command line arguments that were passed to the game.
        ///
        /// @return True if the game is started up successfully; false otherwise.
        ///
        /// @remarks
        ///     This is called after OnLoadConfigs() and after the window has been created and shown.
        virtual bool OnStartup(Context &context);

        /// Called when the game is shutting down.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This the last event to ever be called. It is called before the engine's context is destroyed.
        virtual void OnShutdown(Context &context);


        /// Called when the game is updated.
        ///
        /// @param game               [in] A reference to the applicable game object.
        /// @param deltaTimeInSeconds [in] The time since the last frame, in seconds.
        ///
        /// @remarks
        ///     This is called from the update thread, once per frame. This is were you'll want to step the current scene.
        virtual void OnUpdate(Context &context, double deltaTimeInSeconds);

        /// Called just before the next frame has started.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This function is completely synchronised. Neither the update nor the rendering thread is running at this point. It will be
        ///     executed on the main thread.
        virtual void OnStartFrame(Context &context);

        /// Called after the current frame has finished.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This function is completely synchronised. Neither the update nor the rendering thread is running at this point. It will be
        ///     executed on the main thread.
        virtual void OnEndFrame(Context &context);


        /// Called when the game window is resized.
        ///
        /// @param game      [in] A reference to the applicable game object.
        /// @param newWidth  [in] The new width of the game window.
        /// @param newHeight [in] The new height of the game window.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnSize(Context &context, unsigned int newWidth, unsigned int newHeight);


        /// Called when the mouse is moved.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param xPos [in] The new position of the cursor on the x axis.
        /// @param yPos [in] The new position of the cursor on the y axis.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnMouseMove(Context &context, int xPos, int yPos);

        /// Called when the mouse wheel is turned.
        ///
        /// @param game  [in] A reference to the applicable game object.
        /// @param delta [in] The mouse wheel delta specifying the direction of the turn.
        /// @param xPos  [in] The position of the cursor on the x axis.
        /// @param yPos  [in] The position of the cursor on the y axis.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnMouseWheel(Context &context, int delta, int xPos, int yPos);

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
        virtual void OnMouseButtonDown(Context &context, int button, int xPos, int yPos);

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
        virtual void OnMouseButtonUp(Context &context, int button, int xPos, int yPos);

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
        virtual void OnMouseButtonDoubleClick(Context &context, int button, int xPos, int yPos);


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
        virtual void OnKeyPress(Context &context, Key key);

        /// Called when a key is released.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param key  [in] The key code representing the key that was released.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     There is no difference between this and OnKeyUp, except that OnKeyUp will be called after OnKeyReleased.
        virtual void OnKeyRelease(Context &context, Key key);

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
        virtual void OnKeyDown(Context &context, Key key);

        /// Called when a key is released.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param key  [in] The key code representing the key that was released.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     There is no difference between this and OnKeyReleased, except that OnKeyUp will be called after OnKeyReleased.
        virtual void OnKeyUp(Context &context, Key key);


        /// Called when the game window receives focus.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnReceiveFocus(Context &context);

        /// Called when the game window loses focus.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @remarks
        ///     This is called from the update thread.
        virtual void OnLoseFocus(Context &context);


        /// Called when an event unknown to the engine is handled.
        ///
        /// @param game [in] A reference to the applicable game object.
        /// @param e    [in] A reference to the event object.
        ///
        /// @remarks
        ///     This is called from the update thread.
        ///     @par
        ///     You will most likely never need to implement this function.
        virtual void OnHandleEvent(Context &context, GameEvent &e);


        /// Called when the game is paused.
        ///
        /// @param game [in] A reference to the applicable game object.
        virtual void OnPause(Context &context);

        /// Called when the game is resumed from a paused state.
        ///
        /// @param game [in] A reference to the applicable game object.
        virtual void OnResume(Context &context);


        /// Called just before the editor is opened.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @return True to allow the editor to continue opening; false to prevent the editor from opening.
        virtual bool OnEditorOpening(Context &context);

        /// Called just before the editor is closed.
        ///
        /// @param game [in] A reference to the applicable game object.
        ///
        /// @return True to allow the editor to continue closing; false to prevent the editor from closing.
        virtual bool OnEditorClosing(Context &context);

        /// Called just after the editor is opened.
        ///
        /// @param game [in] A reference to the applicable game object.
        virtual void OnEditorOpen(Context &context);

        /// Called just after the editor is closed.
        ///
        /// @param game [in] A reference to the applicable game object.
        virtual void OnEditorClose(Context &context);



        ///////////////////////////////////////
        // Methods

        /// Called when a scene needs to be loaded and made current.
        ///
        /// @param game              [in] A reference to the applicable game object.
        /// @param sceneRelativePath [in] The relative path of the scene file to load.
        ///
        /// @return True if the scene is loaded successfully; false otherwise.
        virtual bool LoadScene(Context &context, const char* sceneRelativePath);

        

        ///////////////////////////////////////
        // Serialization

        /// Serializes the game state.
        ///
        /// @param game       [in] A reference to the applicable game object.
        /// @param serializer [in] The serializer to write the data to.
        virtual bool Serialize(Context &context, Serializer &serializer);


        ///////////////////////////////////////
        // Deserialization

        /// Deserializes the game state.
        ///
        /// @param game       [in] A reference to the applicable game object.
        /// @param deserializer [in] The deserializer to write the data to.
        virtual bool Deserialize(Context &context, Deserializer &deserializer);
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

        /// Retrieves the data structure that can be used to restore the scene to it's appropriate state.
        ///
        /// @param relativeFilePath [in] The relative file path of the scene whose data is being retrieved.
        ///
        /// @return A pointer to the SceneStateStackRestoreCommands structure that contains the difference between the base scene and it's current state.
        ///
        /// @remarks
        ///     If no difference data is stored for the given scene, the null will be returned.
        virtual SceneStateStackRestoreCommands* GetSceneData(const char* relativeFilePath);


        ///////////////////////////////////////
        // Serialization

        /// GameStateManager::Serialize()
        virtual bool Serialize(Context &context, Serializer &serializer);

        /// Serializes the header information.
        ///
        /// @param game       [in] A reference to the applicable game object.
        /// @param serializer [in] The serializer to write the data to.
        ///
        /// @return The version of the data.
        ///
        /// @remarks
        ///     The return value will be passed to DeserializeHeaderData().
        virtual uint32_t SerializeHeaderData(Context &context, Serializer &serializer);

        /// Serializes the global data.
        ///
        /// @param game       [in] A reference to the applicable game object.
        /// @param serializer [in] The serializer to write the data to.
        ///
        /// @return The version of the data.
        ///
        /// @remarks
        ///     The return value will be passed to DeserializeGlobalData().
        virtual uint32_t SerializeGlobalData(Context &context, Serializer &serializer);



        ///////////////////////////////////////
        // Deserialization

        /// GameStateManager::Deserialize()
        virtual bool Deserialize(Context &context, Deserializer &deserializer);

        /// Deserializes the header information.
        ///
        /// @param game       [in] A reference to the applicable game object.
        /// @param deserializer [in] The deserializer to read the data from.
        virtual bool DeserializeHeaderData(Context &context, Deserializer &deserializer, uint32_t version);

        /// Deserializes the global game data.
        ///
        /// @param game       [in] A reference to the applicable game object.
        /// @param deserializer [in] The deserializer to read the data from.
        virtual bool DeserializeGlobalData(Context &context, Deserializer &deserializer, uint32_t version);


    private:

        /// Maps a scene's relative file path to it's serialized data.
        Dictionary<SceneStateStackRestoreCommands*> m_sceneData;
    };




    /// The default game state manager.
    ///
    /// This can be used as the game state manager for most games. If more flexibility is required, you may want to
    /// consider rolling out a custom game state manager implementation.
    class DefaultGameStateManager : public GenericGameStateManager
    {
    public:

        /// Constructor.
        DefaultGameStateManager();

        /// Destructor.
        virtual ~DefaultGameStateManager();


        ////////////////////////////////////////
        // Events

        /// GameStateManager::OnStartup().
        virtual bool OnStartup(Context &context);

        /// GameStateManager::OnShutdown().
        virtual void OnShutdown(Context &context);

        /// GameStateManager::OnUpdate().
        virtual void OnUpdate(Context &context, double deltaTimeInSeconds);

        /// GameStateManager::OnStartFrame().
        virtual void OnStartFrame(Context &context);

        /// GameStateManager::OnSize().
        virtual void OnSize(Context &context, unsigned int newWidth, unsigned int newHeight);

        /// GameStateManager::OnEditorClosing().
        virtual bool OnEditorClosing(Context &context);



        /// GameStateManager::LoadScene().
        virtual bool LoadScene(Context &context, const char* sceneRelativePath);



        ///////////////////////////////////////
        // Serialization

        /// GenericGameStateManager::Serialize()
        virtual bool Serialize(Context &context, Serializer &serializer);

        /// GenericGameStateManager::SerializeHeaderData()
        virtual uint32_t SerializeHeaderData(Context &context, Serializer &serializer);

        /// GenericGameStateManager::SerializeGlobalData()
        virtual uint32_t SerializeGlobalData(Context &context, Serializer &serializer);


        ///////////////////////////////////////
        // Deserialization

        /// GenericGameStateManager::Deserialize()
        virtual bool Deserialize(Context &context, Deserializer &deserializer);

        /// GenericGameStateManager::DeserializeHeaderData()
        virtual bool DeserializeHeaderData(Context &context, Deserializer &deserializer, uint32_t version);

        /// GenericGameStateManager::DeserializeGlobalData()
        virtual bool DeserializeGlobalData(Context &context, Deserializer &deserializer, uint32_t version);



    private:

        /// Determines whether or not the game state is deserializing.
        bool IsDeserializing() const
        {
            return m_isDeserializing;
        }


    private:

        /// A pointer to the current scene. We use a pointer because we'll be dynamically creating and deleting scenes.
        Scene* m_currentScene;

        /// The relative path of the current scene. We need this so we can update the game state manager when a scene changes.
        String m_currentSceneRelativePath;

        /// A pointer to the next scene that's just about to be loaded. When a scene is loaded, we don't want to switch scenes
        /// immediately because there is a chance it will be in the middle of updating. Instead, we want to delay the loading
        /// until the next frame, in which case we'll do the switch.
        Scene* m_nextScene;

        /// The relative path of the next scene that's just about to be loaded.
        String m_nextSceneRelativePath;

        /// The aspect ratio to use. If this is 0, the aspect ratio will be re-calculated dynamically when the window is resized.
        float m_aspectRatio;

        /// Whether or not the game state is in the process of deserializing. We use this in determining whether or not LoadScene()
        /// should update the scene state - we don't want to bother doing it if we're deserializing.
        bool m_isDeserializing;
    };
}

#endif
