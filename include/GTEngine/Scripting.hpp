
#ifndef __GTEngine_Scripting_hpp_
#define __GTEngine_Scripting_hpp_

#include "GameScript.hpp"
#include "Scene.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the GTEngine script library into the given script.
        ///
        /// @param script [in] The script to load the GTEngine script library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        ///
        /// @remarks
        ///     This only loads the content in the GTEngine namespace/table. This will NOT load any game-specific functionality.
        bool LoadGTEngineScriptLibrary(GTCore::Script &script);



        namespace FFI
        {
            /// Retrieves the executable directory.
            int GetExecutableDirectory(GTCore::Script &script);

            /// Determines if the given file path is a model file, based on it's extension.
            int IsModelFile(GTCore::Script &script);

            /// Determines if the given file path is an image file, based on it's extension.
            int IsImageFile(GTCore::Script &script);

            /// Determines if the given file path is a sound file, based on it's extension.
            int IsSoundFile(GTCore::Script &script);

            /// Determines if the given file path is a scene file, based on it's extension.
            int IsSceneFile(GTCore::Script &script);



            namespace RendererFFI
            {
                /// Enables v-sync on the renderer.
                int EnableVSync(GTCore::Script &script);

                /// Disables v-sync on the renderer.
                int DisableVSync(GTCore::Script &script);
            }


            namespace AudioFFI
            {
                /// Plays a sound.
                ///
                /// @remarks
                ///     Argument 1: The file name of the sound to play.
                int Play(GTCore::Script &script);
            }


            namespace SystemFFI
            {
                namespace SceneNodeFFI
                {
                    /// Creates a new empty scene node, returning a pointer to it.
                    int Create(GTCore::Script &script);

                    /// Deletes a scene node that was created with CreateSceneNode(GTCore::Script &).
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node returned by CreateSceneNode(GTCore::Script &).
                    int Delete(GTCore::Script &script);



                    /// Retrieves the name of the given component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose name is being retrieved.
                    int GetName(GTCore::Script &script);

                    /// Sets the name of the given component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose name is being set.
                    ///     Argument 2: The new name for the scene node.
                    int SetName(GTCore::Script &script);

                    /// Adds a new component to the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose having the component added to it.
                    ///     Argument 2: The name of the component to add, as a string.
                    ///     @par
                    ///     Returns a table representing the new component.
                    int AddComponent(GTCore::Script &script);

                    /// Removes a component from the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose having the component removed.
                    ///     Argument 2: The name of the componet to remove, as a string.
                    int RemoveComponent(GTCore::Script &script);

                    /// Retrieves a component from the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose having a component retrieved.
                    ///     Argument 2: The name of the component to retrieve, as a string.
                    ///     @par
                    ///     This will instantiate a new table every time it is called. If the given scene node does not have a component of the given name, this will return nil.
                    int GetComponent(GTCore::Script &script);


                    /// Refreshes the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node being refreshed.
                    int Refresh(GTCore::Script &script);



                    ////////////////////////////////////////////////////////////////
                    // Functions below are just helpers.

                    /// A helper function for pushing a new instantiation of script-side component object.
                    ///
                    /// @param script             [in] The script where the result will be pushed to.
                    /// @param componentClassName [in] The class name of the component being pushed.
                    /// @param component          [in] A pointer to the C++ component.
                    void PushComponent(GTCore::Script &script, const char* componentClassName, Component* component);
                }


                namespace SceneFFI
                {
                    /// Adds a scene node to the given scene.
                    ///
                    /// @remarks
                    ///     Argument 1: The internal pointer to the scene that is having the scene node added to it.
                    ///     Argument 2: The internal pointer to the scene node being added to the scene.
                    int AddSceneNode(GTCore::Script &script);

                    /// Removes a scene node from the given scene.
                    ///
                    /// @remarks
                    ///     Argument 1: The internal pointer to the scene that is having the scene node removed from it.
                    ///     Argument 2: The internal pointer to the scene node being removed from the scene.
                    int RemoveSceneNode(GTCore::Script &script);
                }


                namespace ModelComponentFFI
                {
                    /// Sets the model for the given component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose model is being set.
                    ///     Argument 2: The file path of the model being set.
                    int SetModel(GTCore::Script &script);
                }
            }
        }
    }
}

#endif