
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


                    /// Retrieves the local position of the given scene node as 3 return values.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose position is being retrieved.
                    int GetPosition(GTCore::Script &script);

                    /// Sets the local position of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose position is being set.
                    ///     Argument 2: The x position.
                    ///     Argument 3: The y position.
                    ///     Argument 4: The z position.
                    int SetPosition(GTCore::Script &script);

                    /// Retrieves the local XYZ rotation of the given scene node as 3 return values.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose rotation is being retrieved.
                    int GetRotationXYZ(GTCore::Script &script);

                    /// Sets the local rotation of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose rotation is being set.
                    ///     Argument 2: The x rotation.
                    ///     Argument 3: The y rotation.
                    ///     Argument 4: The z rotation.
                    int SetRotationXYZ(GTCore::Script &script);

                    /// Retrieves the local scale of the given scene node as 3 return values.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
                    int GetScale(GTCore::Script &script);

                    /// Sets the local scale of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose scale is being set.
                    ///     Argument 2: The x scale.
                    ///     Argument 3: The y scale.
                    ///     Argument 4: The z scale.
                    int SetScale(GTCore::Script &script);

                    /// Returns a table containing the component IDs (names) that are attached to the given node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose component IDs are being retrieved.
                    int GetAttachedComponentIDs(GTCore::Script &script);


                    /// Shows the scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node.
                    int Show(GTCore::Script &script);

                    /// Hides the scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node.
                    int Hide(GTCore::Script &script);

                    /// Determines whether or not the node is visible.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node.
                    int IsVisible(GTCore::Script &script);





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

                    /// Retrieves the path of the model currently set on the given model component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose model's path is being retrieved.
                    int GetModelPath(GTCore::Script &script);

                    /// Enables shadow casting.
                    int EnableShadowCasting(GTCore::Script &script);

                    /// Disables shadow casting.
                    int DisableShadowCasting(GTCore::Script &script);

                    /// Determines whether or not shadow casting is enabled.
                    int IsShadowCastingEnabled(GTCore::Script &script);
                }

                namespace PointLightComponentFFI
                {
                    /// Sets the colour of the light.
                    ///
                    /// @remarks
                    ///     Argument 1:     A pointer to the component whose colour is being set.
                    ///     Argument 2,3,4: The red, green and blue colour components, respectively.
                    int SetColour(GTCore::Script &script);

                    /// Retrieves the colour of the light as 3 floats.
                    int GetColour(GTCore::Script &script);


                    /// Enables shadow casting on the light.
                    int EnableShadowCasting(GTCore::Script &script);

                    /// Disables shadow casting on the light.
                    int DisableShadowCasting(GTCore::Script &script);

                    /// Determines whether or not shadow casting is enabled.
                    int IsShadowCastingEnabled(GTCore::Script &script);


                    /// Retrieves the constant attenuation of the light.
                    int GetConstantAttenuation(GTCore::Script &script);

                    /// Retrieves the linear attenuation of the light.
                    int GetLinearAttenuation(GTCore::Script &script);

                    /// Retrieves the quadratic attenuation of the light.
                    int GetQuadraticAttenuation(GTCore::Script &script);

                    /// Sets the attenuation of the light.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose attenuation is being set.
                    ///     Argument 2: The constant attenuation.
                    ///     Argument 3: The linear attenuation.
                    ///     Argument 4: The quadratic attenuation.
                    int SetAttenuation(GTCore::Script &script);
                }

                namespace SpotLightComponentFFI
                {
                    /// Sets the colour of the light.
                    ///
                    /// @remarks
                    ///     Argument 1:     A pointer to the component whose colour is being set.
                    ///     Argument 2,3,4: The red, green and blue colour components, respectively.
                    int SetColour(GTCore::Script &script);

                    /// Retrieves the colour of the light as 3 floats.
                    int GetColour(GTCore::Script &script);


                    /// Enables shadow casting on the light.
                    int EnableShadowCasting(GTCore::Script &script);

                    /// Disables shadow casting on the light.
                    int DisableShadowCasting(GTCore::Script &script);

                    /// Determines whether or not shadow casting is enabled.
                    int IsShadowCastingEnabled(GTCore::Script &script);


                    /// Retrieves the constant attenuation of the light.
                    int GetConstantAttenuation(GTCore::Script &script);

                    /// Retrieves the linear attenuation of the light.
                    int GetLinearAttenuation(GTCore::Script &script);

                    /// Retrieves the quadratic attenuation of the light.
                    int GetQuadraticAttenuation(GTCore::Script &script);

                    /// Sets the attenuation of the light.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose attenuation is being set.
                    ///     Argument 2: The constant attenuation.
                    ///     Argument 3: The linear attenuation.
                    ///     Argument 4: The quadratic attenuation.
                    int SetAttenuation(GTCore::Script &script);
                }

                namespace DirectionalLightComponentFFI
                {
                    /// Sets the colour of the light.
                    ///
                    /// @remarks
                    ///     Argument 1:     A pointer to the component whose colour is being set.
                    ///     Argument 2,3,4: The red, green and blue colour components, respectively.
                    int SetColour(GTCore::Script &script);

                    /// Retrieves the colour of the light as 3 floats.
                    int GetColour(GTCore::Script &script);


                    /// Enables shadow casting on the light.
                    int EnableShadowCasting(GTCore::Script &script);

                    /// Disables shadow casting on the light.
                    int DisableShadowCasting(GTCore::Script &script);

                    /// Determines whether or not shadow casting is enabled.
                    int IsShadowCastingEnabled(GTCore::Script &script);
                }

                namespace EditorMetadataComponentFFI
                {
                    /// Shows a sprite on the object while it's shown in the editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose sprite is being shown.
                    ///     Argument 2: The file path of the texture to use on the sprite.
                    ///     Argument 3: A table containing the colour to use with the sprite. Keys are r, g and b.
                    int ShowSprite(GTCore::Script &script);

                    /// Hides the sprite.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose sprite is being hidden.
                    int HideSprite(GTCore::Script &script);

                    /// Determines whether or not a sprite is being shown.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int IsShowingSprite(GTCore::Script &script);

                    /// Retrieves the path of the texture being used for the sprite.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose sprite texture is being retrieved.
                    int GetSpriteTexturePath(GTCore::Script &script);
                }
            }
        }
    }
}

#endif