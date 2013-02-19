// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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


        /// Registers a scene to the given script.
        ///
        /// @param script [in] A reference to the script object to register the scene with.
        /// @param scene  [in] A reference to the scene to register.
        bool RegisterScene(GTCore::Script &script, Scene &scene);

        /// Unregisters the given scene from the given script.
        ///
        /// @param script [in] A reference to the script object to unregister the scene from.
        /// @param scene  [in] A reference to the scene to unregister.
        void UnregisterScene(GTCore::Script &script, Scene &scene);

        /// Instantiates a scene node on it's owner scene.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node being instantiated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered.
        void InstantiateSceneNode(GTCore::Script &script, SceneNode &sceneNode);

        /// Uninstantiates the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node being uninstantiated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered.
        void UninstantiateSceneNode(GTCore::Script &script, SceneNode &sceneNode);

        /// Pushes a scene node object to the top of the stack.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node whose Lua counterpart is being pushed.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        ///     @par
        ///     The scene node will need to be popped when no longer needed.
        ///     @par
        ///     This function asserts that the scene node has an OnUpdate function.
        void PushSceneNode(GTCore::Script &script, SceneNode &sceneNode);


        /// Calls the OnUpdate event on the given scene node.
        ///
        /// @param script             [in] A reference to the main script object.
        /// @param sceneNode          [in] A reference to the scene node having it's event called.
        /// @param deltaTimeInSeconds [in] The time since the last update. Used for time-based operations.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnUpdate(GTCore::Script &script, SceneNode &sceneNode, double deltaTimeInSeconds);

        /// Calls the OnStartup event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnStartup(GTCore::Script &script, SceneNode &sceneNode);

        /// Calls the OnShutdown event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnShutdown(GTCore::Script &script, SceneNode &sceneNode);



        ///////////////////////////////////////////////////////////////
        // Script Definitions

        /// Loads a scene node script, replacing the old one if it exists.
        ///
        /// @param script             [in] A reference to the script to load the scene node script into.
        /// @param scriptRelativePath [in] The relative path of the script.
        /// @param scriptString       [in] The actual script content.
        bool LoadScriptDefinition(GTCore::Script &script, const char* scriptRelativePath, const char* scriptString);

        /// Unloads a scene node script.
        ///
        /// @param script             [in] A reference to the script to unload the scene node script from.
        /// @param scriptRelativePath [in] The relative path of the script to unload.
        void UnloadScriptDefinition(GTCore::Script &script, const char* scriptRelativePath);

        /// Synchronizes the script definitions with the script library.
        ///
        /// @remarks
        ///     This will unload any no-longer-loaded definitions, load newly loaded ones and refresh existing ones.
        ///     @par
        ///     This does not update any scene nodes connected to the applicable definitions.
        void SyncScriptDefinitionsWithLibrary(GTCore::Script &script);



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

            /// Determines if the given file path is a prefab file, based on it's extension.
            int IsPrefabFile(GTCore::Script &script);


            /// Creates a prefab file from a scene node.
            ///
            /// @remarks
            ///     Argument 1: The absolute path of the file to create or replace.
            ///     Argument 2: The path to make the absolute path relative to.
            ///     Argument 3: A pointer to the scene node to create the prefrab file from.
            int CreatePrefab(GTCore::Script &script);


            /// Determines if the given string is the name of a supported scene node event handler.
            ///
            /// @remarks
            ///     Argument 1: The name of the event handler to check.
            int IsSceneNodeEventHandler(GTCore::Script &script);



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


                    /// Retrieves the ID of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose ID is being retrieved.
                    ///
                    ///     An important note. In the C++ side, the ID is 64-bits. On the Lua side, it is 32-bits.
                    int GetID(GTCore::Script &script);


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


                    /// Retrieves a pointer to the parent scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose parent is being retrieved.
                    int GetParentPtr(GTCore::Script &script);

                    /// Attaches a scene node to the given node as a child.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose having a child attached.
                    ///     Argument 2: A pointer to the child node.
                    ///     Argument 3: A boolean controlling whether or not the world transformation should remain constant. 'true' = keep the world transform.
                    int AttachChild(GTCore::Script &script);

                    /// Orphans a scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node being orphaned.
                    ///     Argument 2: A boolean controlling whether or not the world transformation should remain constant. 'true' = keep the world transform.
                    int Orphan(GTCore::Script &script);

                    /// Determines whether or not the given scene node is an ancestor of another.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the main scene node.
                    ///     Argument 2: A pointer to the ancestor scene node.
                    int IsAncestor(GTCore::Script &script);

                    /// Determines whether or not the given scene node is a descendant of another.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the main scene node.
                    ///     Argument 2: A pointer to the descendant scene node.
                    int IsDescendant(GTCore::Script &script);

                    /// Determines whether or not the given scene node is related to another.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the main scene node.
                    ///     Argument 2: A pointer to the relative scene node.
                    int IsRelative(GTCore::Script &script);


                    /// Retrieves a table containing the IDs of the child nodes.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node in question.
                    int GetChildrenIDs(GTCore::Script &script);


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


                    /// Retrieves the local position of the given scene node as 3 return values.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose position is being retrieved.
                    int GetPosition(GTCore::Script &script);

                    /// Retrieves the world position of the given scene node as 3 floats.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose world position is being retrieved.
                    int GetWorldPosition(GTCore::Script &script);

                    /// Sets the local position of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose position is being set.
                    ///     Argument 2: The x position.
                    ///     Argument 3: The y position.
                    ///     Argument 4: The z position.
                    int SetPosition(GTCore::Script &script);

                    /// Sets the world position of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose position is being set.
                    ///     Argument 2: The x position.
                    ///     Argument 3: The y position.
                    ///     Argument 4: The z position.
                    int SetWorldPosition(GTCore::Script &script);


                    /// Retrieves the local XYZ rotation of the given scene node as 3 return values.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose rotation is being retrieved.
                    int GetRotationXYZ(GTCore::Script &script);

                    /// Retrieves the world XYZ rotation of the given scene node as 3 return values.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose world rotation is being retrieved.
                    int GetWorldRotationXYZ(GTCore::Script &script);

                    /// Sets the local rotation of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose rotation is being set.
                    ///     Argument 2: The x rotation.
                    ///     Argument 3: The y rotation.
                    ///     Argument 4: The z rotation.
                    int SetRotationXYZ(GTCore::Script &script);

                    /// Sets the world rotation of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose rotation is being set.
                    ///     Argument 2: The x rotation.
                    ///     Argument 3: The y rotation.
                    ///     Argument 4: The z rotation.
                    int SetWorldRotationXYZ(GTCore::Script &script);


                    /// Retrieves the local scale of the given scene node as 3 return values.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
                    int GetScale(GTCore::Script &script);

                    /// Retrieves the world scale of the given scene node as 3 return values.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
                    int GetWorldScale(GTCore::Script &script);

                    /// Sets the local scale of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose scale is being set.
                    ///     Argument 2: The x scale.
                    ///     Argument 3: The y scale.
                    ///     Argument 4: The z scale.
                    int SetScale(GTCore::Script &script);

                    /// Sets the world scale of the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node whose scale is being set.
                    ///     Argument 2: The x scale.
                    ///     Argument 3: The y scale.
                    ///     Argument 4: The z scale.
                    int SetWorldScale(GTCore::Script &script);


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



                    /// Retrieves a pointer to the scene that this node is part of, if any.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene node.
                    int GetScenePtr(GTCore::Script &script);




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

                    /// Creates a new empty scene node by the given scene.
                    ///
                    /// @remarks
                    ///     Argument 1: The internal pointer to the scene that is having the scene node added to it.
                    ///     Return:     The internal pointer to the new scene node.
                    int CreateNewSceneNode(GTCore::Script &script);


                    /// Retrieves a table containing pointers to every scene node in the scene. The key is the scene node ID and value is the C++ pointer.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene.
                    ///     Return:     An ID/Pointer map of every scene node in the scene.
                    int GetSceneNodePtrs(GTCore::Script &script);
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


                    /// Sets a material of the model currently set on the given model component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The index of the material being set.
                    ///     Argument 3: The name of the new material.
                    int SetMaterial(GTCore::Script &script);

                    /// Retrieves the name of the material at the given index of the model currently set on the given model component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetMaterialPath(GTCore::Script &script);

                    /// Retrieves the number of materials on the model currently attached to the given model component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetMaterialCount(GTCore::Script &script);
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


                    /// Sets the inner and outer angles of the spot light.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The inner angle.
                    ///     Argument 3: The outer angle.
                    int SetAngles(GTCore::Script &script);

                    /// Retrieves the inner and outer angles of the spot light, as two floats.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetAngles(GTCore::Script &script);
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

                namespace AmbientLightComponentFFI
                {
                    /// Sets the colour of the light.
                    ///
                    /// @remarks
                    ///     Argument 1:     A pointer to the component whose colour is being set.
                    ///     Argument 2,3,4: The red, green and blue colour components, respectively.
                    int SetColour(GTCore::Script &script);

                    /// Retrieves the colour of the light as 3 floats.
                    int GetColour(GTCore::Script &script);
                }


                namespace DynamicsComponentFFI
                {
                    /// Sets the mass of the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose mass is being set.
                    ///     Argument 2: The new mass.
                    int SetMass(GTCore::Script &script);

                    /// Retrieves the mass of the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the componen whose mass is being retrieved.
                    int GetMass(GTCore::Script &script);

                    /// Determines whether or not the object is static.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int IsStatic(GTCore::Script &script);

                    /// Determines or sets whether or not the object is kinematic.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer the component.
                    ///     Argument 2: Whether or not the object should be made kinematic, or nil.
                    ///
                    ///     If the second argument is nil, this will act as a getter.
                    int IsKinematic(GTCore::Script &script);


                    /// Sets the friction.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer ot the component.
                    ///     Argument 2: The friction to apply to the value.
                    int SetFriction(GTCore::Script &script);

                    /// Retrieves the friction.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetFriction(GTCore::Script &script);

                    /// Sets the restituion (bounciness).
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The restitution.
                    int SetRestitution(GTCore::Script &script);

                    /// Retrieves the restitution (bounciness).
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetRestitution(GTCore::Script &script);

                    /// Sets the damping.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The linear damping.
                    ///     Argument 3: The angular damping.
                    int SetDamping(GTCore::Script &script);

                    /// Retrieves the linear damping.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetLinearDamping(GTCore::Script &script);

                    /// Retrieves the angular damping.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetAngularDamping(GTCore::Script &script);


                    /// Sets the collision filter.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The collision group. Only the first 16 bits are used.
                    ///     Argument 3: The collision mask. Only the first 16 bits are used.
                    int SetCollisionFilter(GTCore::Script &script);

                    /// Retrieves the collision group.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetCollisionGroup(GTCore::Script &script);

                    /// Retrieves the collision mask.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetCollisionMask(GTCore::Script &script);


                    /// Sets the linear velocity.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x velocity.
                    ///     Argument 3: The y velocity.
                    ///     Argument 4: The z velocity.
                    int SetLinearVelocity(GTCore::Script &script);

                    /// Retrieves the linear velocity as 3 floats.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     @par
                    ///     The return value will be 3 floats.
                    int GetLinearVelocity(GTCore::Script &script);

                    /// Sets the angular velocity.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x velocity.
                    ///     Argument 3: The y velocity.
                    ///     Argument 4: The z velocity.
                    int SetAngularVelocity(GTCore::Script &script);

                    /// Retrieves the linear velocity as 3 floats.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     @par
                    ///     The return value will be 3 floats.
                    int GetAngularVelocity(GTCore::Script &script);


                    /// Sets the linear factor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x factor.
                    ///     Argument 3: The y factor.
                    ///     Argument 4: The z factor.
                    int SetLinearFactor(GTCore::Script &script);

                    /// Sets the angular factor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x factor.
                    ///     Argument 3: The y factor.
                    ///     Argument 4: The z factor.
                    int SetAngularFactor(GTCore::Script &script);


                    /// Sets the gravity to use with the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x gravity.
                    ///     Argument 3: The y gravity.
                    ///     Argument 4: The z gravity.
                    int SetGravity(GTCore::Script &script);

                    /// Retrieves the gravity being used with the object, as 3 floats.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     @par
                    ///     The return value will be 3 floats.
                    int GetGravity(GTCore::Script &script);

                    /// Applies the gravity of the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int ApplyGravity(GTCore::Script &script);


                    /// Applies a central force to the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x force.
                    ///     Argument 3: The y force.
                    ///     Argument 4: The z force.
                    int ApplyCentralForce(GTCore::Script &script);

                    /// Applies a torque to the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x torque.
                    ///     Argument 3: The y torque.
                    ///     Argument 4: The z torque.
                    int ApplyTorque(GTCore::Script &script);

                    /// Applies an impulse to the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x impulse.
                    ///     Argument 3: The y impulse.
                    ///     Argument 4: The z impulse.
                    ///     Argument 5: The relative x position to apply the impulse.
                    ///     Argument 6: The relative y position to apply the impulse.
                    ///     Argument 7: The relative z position to apply the impulse.
                    int ApplyImpulse(GTCore::Script &script);

                    /// Applies a central impulse to the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x impulse.
                    ///     Argument 3: The y impulse.
                    ///     Argument 4: The z impulse.
                    int ApplyCentralImpulse(GTCore::Script &script);

                    /// Applies a torque impulse.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x impulse.
                    ///     Argument 3: The y impulse.
                    ///     Argument 4: The z impulse.
                    int ApplyTorqueImpulse(GTCore::Script &script);


                    /// Disables deactivation of the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int DisableDeactivation(GTCore::Script &script);

                    /// Enables deactivation of an object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int EnableDeactivation(GTCore::Script &script);

                    /// Determines whether or not deactivation is enabled.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int IsDeactivationEnabled(GTCore::Script &script);

                    /// Activates the object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int Activate(GTCore::Script &script);


                    /// Enables navigation mesh generation for this object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int EnableNavigationMeshGeneration(GTCore::Script &script);

                    /// Disables navigation mesh generation for this object.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int DisableNavigationMeshGeneration(GTCore::Script &script);

                    /// Determines whether or not navigation mesh generation is enabled.
                    ///
                    /// @remakrs
                    ///     Argument 1: A pointer to the component.
                    int IsNavigationMeshGenerationEnabled(GTCore::Script &script);



                    /// Adds a box collision shape to the component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The half x extent.
                    ///     Argument 3: The half y extent.
                    ///     Argument 4: The half z extent.
                    ///     Argument 5: Optional x offset.
                    ///     Argument 6: Optional y offset.
                    ///     Argument 7: Optional z offset.
                    int AddBoxCollisionShape(GTCore::Script &script);

                    /// Adds a sphere collision shape to the component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The radius.
                    ///     Argument 5: Optional x offset.
                    ///     Argument 6: Optional y offset.
                    ///     Argument 7: Optional z offset.
                    int AddSphereCollisionShape(GTCore::Script &script);

                    /// Adds an ellipsoid collision shape to the component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The x radius.
                    ///     Argument 3: The y radius.
                    ///     Argument 4: The z radius.
                    ///     Argument 5: Optional x offset.
                    ///     Argument 6: Optional y offset.
                    ///     Argument 7: Optional z offset.
                    int AddEllipsoidCollisionShape(GTCore::Script &script);

                    /// Adds a cylinder collision shape to the component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The half x extent.
                    ///     Argument 3: The half y extent.
                    ///     Argument 4: The half z extent.
                    ///     Argument 5: Optional x offset.
                    ///     Argument 6: Optional y offset.
                    ///     Argument 7: Optional z offset.
                    int AddCylinderXCollisionShape(GTCore::Script &script);
                    int AddCylinderYCollisionShape(GTCore::Script &script);
                    int AddCylinderZCollisionShape(GTCore::Script &script);

                    /// Adds a capsure collision shape to the component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The radius.
                    ///     Argument 3: The length.
                    ///     Argument 5: Optional x offset.
                    ///     Argument 6: Optional y offset.
                    ///     Argument 7: Optional z offset.
                    int AddCapsuleXCollisionShape(GTCore::Script &script);
                    int AddCapsuleYCollisionShape(GTCore::Script &script);
                    int AddCapsuleZCollisionShape(GTCore::Script &script);

                    /// Sets the collision shapes of the object to that of the convex hulls of the model in the Model component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The margin to apply to the collision shapes.
                    int SetCollisionShapesToModelConvexHulls(GTCore::Script &script);

                    /// Removes every collision shape.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int RemoveAllCollisionShapes(GTCore::Script &script);

                    /// Removes the collision shape at the given index.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer ot the component to delete.
                    ///     Argument 2: The index of the shape to delete.
                    int RemoveCollisionShapeAtIndex(GTCore::Script &script);

                    /// Retrieves the number of collision shapes.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetCollisionShapeCount(GTCore::Script &script);

                    /// Retrieves a table containing information about the collision shape of the given index.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The index of the shape whose information is being retrieved.
                    int GetCollisionShapeAtIndex(GTCore::Script &script);

                    /// Determines whether or not we are using the convex hull of a model.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int IsUsingConvexHullsFromModel(GTCore::Script &script);

                    /// Sets the offset for the collision shape at the given index.
                    ///
                    /// @remarks
                    ///     Argument 1:       A pointer to the component.
                    ///     Argument 2:       The index of the shape whose offset is being changed.
                    ///     Argument 3, 4, 5: The new offset, as 3 floats
                    int SetCollisionShapeOffset(GTCore::Script &script);

                    /// Sets the extents of the box shape at the given index.
                    ///
                    /// @remarks
                    ///     If the shape at the given index is not a box, this function will fail.
                    ///     @par
                    ///     Argument 1:       A pointer to the component.
                    ///     Argument 2:       The index of the shape.
                    ///     Argument 3, 4, 5: The new half extents.
                    int SetBoxCollisionShapeHalfExtents(GTCore::Script &script);

                    /// Sets the radius of the sphere shape at the given index.
                    ///
                    /// @remarks
                    ///     If the shape at the given index is not a sphere, this function will fail.
                    ///     @par
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The index of the shape.
                    ///     Argument 3: The new radius.
                    int SetSphereCollisionShapeRadius(GTCore::Script &script);

                    /// Sets the radius of the ellipsoid shape at the given index.
                    ///
                    /// @remarks
                    ///     If the shape at the given index is not an ellipsoid, this function will fail.
                    ///     @par
                    ///     Argument 1:       A pointer to the component.
                    ///     Argument 2:       The index of the shape.
                    ///     Argument 3, 4, 5: The new radius as 3 floats.
                    int SetEllipsoidCollisionShapeRadius(GTCore::Script &script);

                    /// Sets the extents of the cylinder shape at the given index.
                    ///
                    /// @remarks
                    ///     If the shape at the given index is not a cylinder, this function will fail.
                    ///     @par
                    ///     Argument 1:       A pointer to the component.
                    ///     Argument 2:       The index of the shape.
                    ///     Argument 3, 4, 5: The new half extents.
                    int SetCylinderCollisionShapeHalfExtents(GTCore::Script &script);

                    /// Sets the size of the capsule shape at the given index.
                    ///
                    /// @remarks
                    ///     If the shape at the given index is not an ellipsoid, this function will fail.
                    ///     @par
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The index of the shape.
                    ///     Argument 3: The radius of the shape.
                    ///     Argument 4: The height of the shape.
                    int SetCapsuleCollisionShapeSize(GTCore::Script &script);
                }


                namespace ScriptComponentFFI
                {
                    /// Adds a script file to the component.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The relative path of the script file being added.
                    int AddScript(GTCore::Script &script);

                    /// Removes a script file from the component by it's relative path.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The relative path of the script file being removed.
                    int RemoveScriptByRelativePath(GTCore::Script &script);

                    /// Removes a script file by it's index.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The index of the script being removed.
                    int RemoveScriptByIndex(GTCore::Script &script);

                    /// Reloads a script at the given index with a new file.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The index of the script being reloaded.
                    ///     Argument 3: The relative path of the new script.
                    int ReloadScript(GTCore::Script &script);

                    /// Retrieves the names of the script files the given script component uses.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int GetScriptFilePaths(GTCore::Script &script);

                    /// Retrieves an array containing the names and types of the public variables of the script definition at the given index.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The index of the script definition whose public variables are being retried.
                    ///
                    ///     The return value is an array of {name, type} types.
                    int GetPublicVariableNamesAndTypesByIndex(GTCore::Script &script);


                    /// Retrieves the names and values of every public variable.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///
                    ///     The returned table is keyed by the name. Note that this is out of order.
                    int GetPublicVariableNamesAndValues(GTCore::Script &script);

                    /// Retrieves the value of the public variable with the given name.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The name of the variable whose value is being retrieved.
                    int GetPublicVariableValue(GTCore::Script &script);

                    /// Sets the value of the public variable with the given name.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     Argument 2: The name of the variable whose value is being set.
                    ///     Argument 3: The value to assign to the variable.
                    ///
                    ///     If the value is an unsupported type, nothing will be changed.
                    int SetPublicVariableValue(GTCore::Script &script);
                }


                namespace EditorMetadataComponentFFI
                {
                    /// Marks the node as selected.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     @par
                    ///     Note that this isn't enough to fully select the node. Instead, this simply marks the node as selected.
                    int Select(GTCore::Script &script);

                    /// Marks the node as deselected.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     @par
                    ///     Note that this isn't enough to fully deselect the node. Instead, this simply marks the node as selected.
                    int Deselect(GTCore::Script &script);

                    /// Determines whether or not the node is marked as selected.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    ///     @par
                    ///     Returns a boolean.
                    int IsSelected(GTCore::Script &script);


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


                    /// Shows a direction on the object while it's shown in the editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose direction arrow is being shown.
                    int ShowDirectionArrow(GTCore::Script &script);

                    /// Hides the direction arrow.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component whose direction arrow is being hidden.
                    int HideDirectionArrow(GTCore::Script &script);

                    /// Determines whether or not the direction arrow is being shown.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the component.
                    int IsShowingDirectionArrow(GTCore::Script &script);
                }



                namespace SubEditorFFI
                {
                    /// Retrieves the absolute path of the file loaded in the given sub-editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the sub-editor.
                    int GetAbsolutePath(GTCore::Script &script);

                    /// Retrieves the relative path of the file loaded in the given sub-editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the sub-editor.
                    int GetRelativePath(GTCore::Script &script);
                }

                namespace ModelEditorFFI
                {
                    /// Retrieves the relative names of all of the materials in the model of the given model editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the model editor.
                    int GetMaterials(GTCore::Script &script);

                    /// Retrieves the settings used to build the convex hull of the model loaded in the given model editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the model editor.
                    int GetConvexHullBuildSettings(GTCore::Script &script);

                    /// Plays the animation track of the model loaded in the given model editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the model editor.
                    int PlayAnimation(GTCore::Script &script);

                    /// Stops the animation track of the model loaded in the given model editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the model editor.
                    int StopAnimation(GTCore::Script &script);

                    /// Sets a material on the model loaded in the given model editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the model editor.
                    ///     Argument 2: The index of the material being set.
                    ///     Argument 3: The relative path of the material to use.
                    int SetMaterial(GTCore::Script &script);

                    /// Shows the convex decomposition of the model.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the model editor.
                    int ShowConvexDecomposition(GTCore::Script &script);

                    /// Hides the convex decomposition of the model.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the model editor.
                    int HideConvexDecomposition(GTCore::Script &script);

                    /// Builds the convex decomposition of the model.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the model editor.
                    int BuildConvexDecomposition(GTCore::Script &script);
                }

                namespace SceneEditorFFI
                {
                    /// Retrieves the C++ pointer to the scene of the given scene editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int GetScenePtr(GTCore::Script &script);

                    /// Deselects everything in the given scene editor.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int DeselectAll(GTCore::Script &script);

                    /// Selects the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor containing the scene node.
                    ///     Argument 2: A pointer to the scene node to select.
                    ///     Argument 3: A boolean controlling whether or not to notify the editor of the change. A value of false means the editor WILL receive notification.
                    int SelectSceneNode(GTCore::Script &script);

                    /// Deselects the given scene node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor containing the scene node.
                    ///     Argument 2: A pointer to the scene node to deselect.
                    ///     Argument 3: A boolean controlling whether or not to notify the editor of the change. A value of false means the editor WILL receive notification.
                    int DeselectSceneNode(GTCore::Script &script);

                    /// Retrieves the number of nodes that are currently selected.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int GetSelectedSceneNodeCount(GTCore::Script &script);

                    /// Retrieves a pointer to the first selected node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int GetFirstSelectedSceneNodePtr(GTCore::Script &script);

                    /// Retrieves an array containing the selected scene node IDs.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int GetSelectedSceneNodeIDs(GTCore::Script &script);

                    /// Determines if the given scene node is selected.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    ///     Argument 2: The ID of the scene node.
                    int IsSceneNodeSelectedByID(GTCore::Script &script);


                    /// Attempts to select a gizmo with the mouse.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    ///
                    ///     Returns true if a gizmo was selected.
                    int TryGizmoMouseSelect(GTCore::Script &script);

                    /// Performs a mouse selection.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int DoMouseSelection(GTCore::Script &script);


                    /// Deletes all of the selected nodes.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int RemoveSelectedSceneNodes(GTCore::Script &script);

                    /// Deletes the given node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    ///     Argument 2: A pointer to the scene node to delete.
                    int RemoveSceneNode(GTCore::Script &script);

                    /// Duplicates the selected scene nodes.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int DuplicateSelectedSceneNodes(GTCore::Script &script);

                    /// Duplicates the given node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    ///     Argument 2: A pointer to the scene node to duplicate.
                    int DuplicateSceneNode(GTCore::Script &script);


                    /// Switches the gizmo to translate mode.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int SwitchGizmoToTranslateMode(GTCore::Script &script);

                    /// Switches the gizmo to rotate mode.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int SwitchGizmoToRotateMode(GTCore::Script &script);

                    /// Switches the gizmo to scale mode.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int SwitchGizmoToScaleMode(GTCore::Script &script);

                    /// Switches the gizmo to local space.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int SwitchGizmoToLocalSpace(GTCore::Script &script);

                    /// Switches the gizmo to global space.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int SwitchGizmoToGlobalSpace(GTCore::Script &script);

                    /// Toggles the gizmo space.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int ToggleGizmoSpace(GTCore::Script &script);

                    /// Determines whether or not the gizmo is in local space.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int IsGizmoInLocalSpace(GTCore::Script &script);

                    /// Determines whether or not the gizmo is in global space.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int IsGizmoInGlobalSpace(GTCore::Script &script);


                    /// Plays the game.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int StartPlaying(GTCore::Script &script);

                    /// Stops the game.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int StopPlaying(GTCore::Script &script);

                    /// Determines whether or not the game is playing.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int IsPlaying(GTCore::Script &script);


                    /// Enables physics simulation.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int EnablePhysicsSimulation(GTCore::Script &script);

                    /// Disable physics simulation.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int DisablePhysicsSimulation(GTCore::Script &script);

                    /// Determines whether or not physics simulation is enabled.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int IsPhysicsSimulationEnabled(GTCore::Script &script);


                    /// Commits a state stack frame for undo/redo operations.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int CommitStateStackFrame(GTCore::Script &script);

                    /// Performs an undo operation.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int Undo(GTCore::Script &script);

                    /// Performs a redo operation.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int Redo(GTCore::Script &script);


                    /// Instantiates a prefab, returning a pointer to the root node.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    ///     Argument 2: The relative path of the prefab file.
                    ///
                    ///     This leaves the prefab positioned at the origin. Reposition the scene node manually if needed.
                    int InstantiatePrefab(GTCore::Script &script);


                    /// Retrieves a pointer to the scene node of the given ID.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    ///     Argument 2: The ID of the scene node to retrieve.
                    int GetSceneNodePtrByID(GTCore::Script &script);


                    /// Retrieves a array of pointers to the scene nodes in the scene.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    int GetSceneNodes(GTCore::Script &script);


                    /// Positions the given scene node in front of the camera.
                    ///
                    /// @remarks
                    ///     Argument 1: A pointer to the scene editor.
                    ///     Argument 2: A pointer to the scene node.
                    int PositionSceneNodeInFrontOfCamera(GTCore::Script &script);
                }
            }
        }
    }
}

#endif