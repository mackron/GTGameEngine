// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_SceneNode_hpp_
#define __GTEngine_Scripting_SceneNode_hpp_

#include "../GameScript.hpp"
#include "../SceneNode.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the scene node library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        bool LoadSceneNodeLibrary(GTLib::Script &script);



        /// Instantiates a scene node on it's owner scene.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node being instantiated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered.
        void InstantiateSceneNode(GTLib::Script &script, SceneNode &sceneNode);

        /// Uninstantiates the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node being uninstantiated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered.
        void UninstantiateSceneNode(GTLib::Script &script, SceneNode &sceneNode);

        /// Updates the public variables of the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node whose public variables are being updated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered and the given scene node has a script component.
        void UpdatePublicVariables(GTLib::Script &script, SceneNode &sceneNode);


        /// Registers the given component with the Lua representation of the given scene node.
        ///
        /// @param script      [in] A reference to the script object.
        /// @param sceneNode   [in] A reference to the scene node whose Lua representation is being updated.
        /// @param componentID [in] The ID of the component that is being registered.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered and the given scene node actually has the component.
        ///     @par
        ///     This does not change the state of any variables.
        void RegisterComponent(GTLib::Script &script, SceneNode &sceneNode, const char* componentID);

        /// Unregisters the given component from the Lua representation of the given scene node.
        ///
        /// @param script      [in] A reference to the script objec.t
        /// @param sceneNode   [in] A reference to the scene node whose Lua representation is being updated.
        /// @param componentID [in] The ID of the component that is being unregistered.
        void UnregisterComponent(GTLib::Script &script, SceneNode &sceneNode, const char* componentID);


        /// Pushes a scene node object to the top of the stack.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node whose Lua counterpart is being pushed.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        ///     @par
        ///     The scene node will need to be popped when no longer needed.
        void PushSceneNode(GTLib::Script &script, SceneNode &sceneNode);


        /// Serializes the scene node based off the OnSerialize events.
        ///
        /// @param script     [in] A reference to the main script object.
        /// @param sceneNode  [in] A reference to the scene node that is being serialized.
        /// @param serializer [in] A reference to the serializer to write the data to.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void DoOnSerialize(GTLib::Script &script, SceneNode &sceneNode, GTLib::Serializer &serializer);

        /// Deserializes the scene node based off the OnDeserialize events and the data supplied by the given deserializer.
        ///
        /// @param script      [in] A reference to the main script object.
        /// @param sceneNode   [in] A reference to the scene node that is being deserialized.
        /// @param deserialier [in] A reference to the deserializer to retrieve the data from.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        ///     @par
        ///     This should be called based on the data written by DoOnSerialize().
        void DoOnDeserialize(GTLib::Script &script, SceneNode &sceneNode, GTLib::Deserializer &deserializer);


        /// Calls the OnUpdate event on the given scene node.
        ///
        /// @param script             [in] A reference to the main script object.
        /// @param sceneNode          [in] A reference to the scene node having it's event called.
        /// @param deltaTimeInSeconds [in] The time since the last update. Used for time-based operations.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnUpdate(GTLib::Script &script, SceneNode &sceneNode, double deltaTimeInSeconds);

        /// Calls the OnStartup event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnStartup(GTLib::Script &script, SceneNode &sceneNode);

        /// Calls the OnShutdown event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnShutdown(GTLib::Script &script, SceneNode &sceneNode);

        /// Calls the OnShow event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnShow(GTLib::Script &script, SceneNode &sceneNode);

        /// Calls the OnHide event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnHide(GTLib::Script &script, SceneNode &sceneNode);

        /// Calls the OnSceneNodeEnter event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnSceneNodeEnter(GTLib::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

        /// Calls the OnEnterSceneNode event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnEnterSceneNode(GTLib::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

        /// Calls the OnSceneNodeLeave event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnSceneNodeLeave(GTLib::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

        /// Calls the OnLeaveSceneNode event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnLeaveSceneNode(GTLib::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

        /// Calls the OnSerializeGlobalData event on the given scene node.
        ///
        /// @param script     [in] A reference to the main script object.
        /// @param sceneNode  [in] A reference to the scene node that's having it's event called.
        /// @param serializer [in] A reference to the serialize to write the data to.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnSerializeGlobalData(GTLib::Script &script, SceneNode &sceneNode, GTLib::Serializer &serializer);

        /// Calls the OnDeserializeGlobalData event on the given scene node.
        ///
        /// @param script       [in] A reference to the main script object.
        /// @param sceneNode    [in] A reference to the scene node that's having it's event called.
        /// @param deserializer [in] A reference to the deserializer to read the data from.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnDeserializeGlobalData(GTLib::Script &script, SceneNode &sceneNode, GTLib::Deserializer &deserializer);


        namespace SceneNodeFFI
        {
            /// Creates a new empty scene node, returning a pointer to it.
            int Create(GTLib::Script &script);

            /// Deletes a scene node that was created with CreateSceneNode(GTLib::Script &).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node returned by CreateSceneNode(GTLib::Script &).
            int Delete(GTLib::Script &script);


            /// Retrieves the ID of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose ID is being retrieved.
            ///
            ///     An important note. In the C++ side, the ID is 64-bits. On the Lua side, it is 32-bits.
            int GetID(GTLib::Script &script);


            /// Retrieves the name of the given component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose name is being retrieved.
            int GetName(GTLib::Script &script);

            /// Sets the name of the given component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose name is being set.
            ///     Argument 2: The new name for the scene node.
            int SetName(GTLib::Script &script);


            /// Retrieves a pointer to the parent scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose parent is being retrieved.
            int GetParentPtr(GTLib::Script &script);

            /// Attaches a scene node to the given node as a child.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose having a child attached.
            ///     Argument 2: A pointer to the child node.
            ///     Argument 3: A boolean controlling whether or not the world transformation should remain constant. 'true' = keep the world transform.
            int AttachChild(GTLib::Script &script);

            /// Orphans a scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node being orphaned.
            ///     Argument 2: A boolean controlling whether or not the world transformation should remain constant. 'true' = keep the world transform.
            int Orphan(GTLib::Script &script);

            /// Determines whether or not the given scene node is an ancestor of another.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the main scene node.
            ///     Argument 2: A pointer to the ancestor scene node.
            int IsAncestor(GTLib::Script &script);

            /// Determines whether or not the given scene node is a descendant of another.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the main scene node.
            ///     Argument 2: A pointer to the descendant scene node.
            int IsDescendant(GTLib::Script &script);

            /// Determines whether or not the given scene node is related to another.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the main scene node.
            ///     Argument 2: A pointer to the relative scene node.
            int IsRelative(GTLib::Script &script);


            /// Retrieves a table containing the IDs of the child nodes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node in question.
            int GetChildrenIDs(GTLib::Script &script);

            /// Retrieves the ID of the first child scene node with the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose child is being retrieved.
            int GetChildIDByName(GTLib::Script &script);

            /// Retrieves the ID of the first descendant scene node with the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose descendant is being retrieved.
            int GetDescendantIDByName(GTLib::Script &script);


            /// Adds a new component to the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose having the component added to it.
            ///     Argument 2: The name of the component to add, as a string.
            ///     @par
            ///     Returns a table representing the new component.
            int AddComponent(GTLib::Script &script);

            /// Removes a component from the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose having the component removed.
            ///     Argument 2: The name of the componet to remove, as a string.
            int RemoveComponent(GTLib::Script &script);

            /// Retrieves a component from the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose having a component retrieved.
            ///     Argument 2: The name of the component to retrieve, as a string.
            ///     @par
            ///     This will instantiate a new table every time it is called. If the given scene node does not have a component of the given name, this will return nil.
            int GetComponent(GTLib::Script &script);

            /// Returns a table containing the component IDs (names) that are attached to the given node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose component IDs are being retrieved.
            int GetAttachedComponentIDs(GTLib::Script &script);


            /// Disables position inheritance.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int DisablePositionInheritance(GTLib::Script &script);

            /// Enables position inheritance.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int EnablePositionInheritance(GTLib::Script &script);

            /// Determines whether or not position inheritance is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Return: True if position inheritance is enabled; false otherwise.
            int IsPositionInheritanceEnabled(GTLib::Script &script);


            /// Disables orientation inheritance.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int DisableOrientationInheritance(GTLib::Script &script);

            /// Enables orientation inheritance.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int EnableOrientationInheritance(GTLib::Script &script);

            /// Determines whether or not orientation inheritance is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Return: True if orientation inheritance is enabled; false otherwise.
            int IsOrientationInheritanceEnabled(GTLib::Script &script);


            /// Disables scale inheritance.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int DisableScaleInheritance(GTLib::Script &script);

            /// Enables scale inheritance.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int EnableScaleInheritance(GTLib::Script &script);

            /// Determines whether or not scale inheritance is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Return: True if scale inheritance is enabled; false otherwise.
            int IsScaleInheritanceEnabled(GTLib::Script &script);



            /// Retrieves the local position of the given scene node as 3 return values.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose position is being retrieved.
            ///     Return:     A math.vec3() containing the position, relative to the parent.
            int GetPosition(GTLib::Script &script);

            /// Retrieves the world position of the given scene node as 3 floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose world position is being retrieved.
            ///     Return:     A math.vec3() containing the world position.
            int GetWorldPosition(GTLib::Script &script);

            /// Sets the local position of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose position is being set.
            ///     Argument 2: The x position or a math.vec3() containing the position.
            ///     Argument 3: The y position.
            ///     Argument 4: The z position.
            int SetPosition(GTLib::Script &script);

            /// Sets the world position of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose position is being set.
            ///     Argument 2: The x position or a math.vec3() containing the position.
            ///     Argument 3: The y position.
            ///     Argument 4: The z position.
            int SetWorldPosition(GTLib::Script &script);


            /// Retrieves the local orientation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose orientation is being retrieved.
            ///     Return:     A math.quat containing the local orientation.
            int GetOrientation(GTLib::Script &script);

            /// Retrieves the world orientation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose orientation is being retrieved.
            ///     Return:     A math.quat containing the world orientation.
            int GetWorldOrientation(GTLib::Script &script);

            /// Sets the local orientation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The math.quat containing the orientation.
            int SetOrientation(GTLib::Script &script);

            /// Sets the world orientation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The math.quat containing the orientation.
            int SetWorldOrientation(GTLib::Script &script);


            /// Retrieves the local Euler angles representing the rotation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose rotation is being retrieved.
            ///     Return:     A math.vec3 containing the Euler angles in degrees.
            int GetEulerRotation(GTLib::Script &script);

            /// Retrieves the world Euler angles representing the rotation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose rotation is being retrieved.
            ///     Return:     A math.vec3 containing the Euler angles in degrees.
            int GetWorldEulerRotation(GTLib::Script &script);

            /// Sets the local rotation of the given scene nodes as Euler angles.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The x rotation (pitch) or a math.vec3 containing the pitch, yaw and roll.
            ///     Argument 3: The y rotation (yaw).
            ///     Argument 4: The z rotation (roll).
            int SetEulerRotation(GTLib::Script &script);

            /// Sets the world rotation of the given scene nodes as Euler angles.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The x rotation (pitch) or a math.vec3 containing the pitch, yaw and roll.
            ///     Argument 3: The y rotation (yaw).
            ///     Argument 4: The z rotation (roll).
            int SetWorldEulerRotation(GTLib::Script &script);


            /// Retrieves the local scale of the given scene node as 3 return values.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
            ///     Return:     A math.vec3 containing the scale.
            int GetScale(GTLib::Script &script);

            /// Retrieves the world scale of the given scene node as 3 return values.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
            ///     Return:     A math.vec3 containing the scale.
            int GetWorldScale(GTLib::Script &script);

            /// Sets the local scale of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose scale is being set.
            ///     Argument 2: The x scale or a math.vec3 containing the scale.
            ///     Argument 3: The y scale.
            ///     Argument 4: The z scale.
            int SetScale(GTLib::Script &script);

            /// Sets the world scale of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose scale is being set.
            ///     Argument 2: The x scale or a math.vec3 containing the scale.
            ///     Argument 3: The y scale.
            ///     Argument 4: The z scale.
            int SetWorldScale(GTLib::Script &script);


            /// Translates the given scene node along it's local axis.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The amount of translation to apply as a math.vec3.
            int Translate(GTLib::Script &script);

            /// Rotates the given scene node around it's local axis.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The rotation angle.
            ///     Argument 3: The rotation axis as a math.vec3.
            int Rotate(GTLib::Script &script);

            /// Scales the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The amount to scale as a math.vec3.
            int Scale(GTLib::Script &script);


            /// Translates the given scene node along the world axis.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The amount of translation to apply as a math.vec3.
            int TranslateWorld(GTLib::Script &script);

            /// Rotates the scene node about the world axis.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The rotation angle.
            ///     Argument 3: The rotation axis.
            int RotateWorld(GTLib::Script &script);

            /// Rotates the scene node around the world axis, around the given pivot point.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The rotation angle.
            ///     Argument 3: The rotation axis.
            ///     Argument 4: The pivot point.
            int RotateWorldAroundPivot(GTLib::Script &script);


            /// Interpolates the position of the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The destination position.
            ///     Argument 3: The interpolation delta (typically between 0 and 1).
            int InterpolatePosition(GTLib::Script &script);

            /// Interpolates the orientation of the scene node.
            ///
            /// @remarks
            ///     This does not perform a shortest-path interpolation. Use Slerp() for that.
            ///
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The destination orientation.
            ///     Argument 3: The interpolation delta (typically between 0 and 1).
            int InterpolateOrientation(GTLib::Script &script);

            /// Interpolates the scale of the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The destination scale.
            ///     Argument 3: The interpolation delta (typically between 0 and 1).
            int InterpolateScale(GTLib::Script &script);

            /// Performs a shortest-path interpolation of the orientation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The destination orientation.
            ///     Argument 3: The delta (typically between 0 and 1).
            int Slerp(GTLib::Script &script);


            /// Clamps the position of the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The min position.
            ///     Argument 3: The max position.
            int ClampPosition(GTLib::Script &script);

            /// Clamps the world position of the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The min world position.
            ///     Argument 3: The max world position.
            int ClampWorldPosition(GTLib::Script &script);

            /// Clamps the scale of the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The min scale.
            ///     Argument 3: The max scale.
            int ClampScale(GTLib::Script &script);

            /// Clamps the world scale of the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The min world scale.
            ///     Argument 3: The max world scale.
            int ClampWorldScale(GTLib::Script &script);


            /// Looks at the given point in the world.
            ///
            /// @remarks
            ///     The up vector is expected to be normalized.
            ///
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: The target to look at.
            ///     Argument 3: The up vector.
            int LookAt(GTLib::Script &script);

            /// Looks at the given scene node.
            ///
            /// @remarks
            ///     The up vector is expected to be normalized.
            ///
            ///     Argument 1: A pointer to the scene node to orientate.
            ///     Argument 2: A pointer to the scene node to look at.
            ///     Argument 3: The up vector.
            int LookAtSceneNode(GTLib::Script &script);

            /// Orientates the scene to look in the given direction.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            ///     Argument 2: A normalized math.vec3 representing the direction to look in.
            ///     Argument 3: The up vector.
            int LookInDirection(GTLib::Script &script);


            /// Retrieves a normalized vector pointing in the direction of the given scene nodes local forward direction (-Z).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int Forward(GTLib::Script &script);

            /// Retrieves a normalized vector pointing in the direction of the given scene nodes local up direction (+Y).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int Up(GTLib::Script &script);

            /// Retrieves a normalized vector pointing in the direction of the given scene nodes local right direction (+X).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int Right(GTLib::Script &script);

            /// Retrieves a normalized vector pointing in the direction of the given scene nodes world forward direction (-Z).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int WorldForward(GTLib::Script &script);

            /// Retrieves a normalized vector pointing in the direction of the given scene nodes world up direction (+Y).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int WorldUp(GTLib::Script &script);

            /// Retrieves a normalized vector pointing in the direction of the given scene nodes world right direction (+X).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int WorldRight(GTLib::Script &script);



            /// Shows the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int Show(GTLib::Script &script);

            /// Hides the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int Hide(GTLib::Script &script);

            /// Determines whether or not the node is visible.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int IsVisible(GTLib::Script &script);



            /// Retrieves a pointer to the scene that this node is part of, if any.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int GetScenePtr(GTLib::Script &script);




            ////////////////////////////////////////////////////////////////
            // Functions below are just helpers.

            /// A helper function for pushing a new instantiation of script-side component object.
            ///
            /// @param script    [in] The script where the result will be pushed to.
            /// @param component [in] A pointer to the C++ component.
            void PushComponent(GTLib::Script &script, Component* component);
        }
    }
}

#endif