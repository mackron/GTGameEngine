// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Scripting_SceneNode
#define GT_Scripting_SceneNode

#undef GetNextSibling
#undef GetPrevSibling
#undef GetFirstChild
#undef GetClassName

namespace GT
{
    class Script;
    class SceneNode;
    class Component;
    class Serializer;
    class Deserializer;


    /// Loads the scene node library.
    ///
    /// @param script [in] A reference to the script to load the library into.
    ///
    /// @return True if the library is loaded successfully; false otherwise.
    bool LoadSceneNodeLibrary(GT::Script &script);



    /// Instantiates a scene node on it's owner scene.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node being instantiated.
    ///
    /// @remarks
    ///     This is only performed if the owner scene is registered.
    void InstantiateSceneNode(GT::Script &script, SceneNode &sceneNode);

    /// Uninstantiates the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node being uninstantiated.
    ///
    /// @remarks
    ///     This is only performed if the owner scene is registered.
    void UninstantiateSceneNode(GT::Script &script, SceneNode &sceneNode);

    /// Updates the public variables of the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node whose public variables are being updated.
    ///
    /// @remarks
    ///     This is only performed if the owner scene is registered and the given scene node has a script component.
    void UpdatePublicVariables(GT::Script &script, SceneNode &sceneNode);


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
    void RegisterComponent(GT::Script &script, SceneNode &sceneNode, const char* componentID);

    /// Unregisters the given component from the Lua representation of the given scene node.
    ///
    /// @param script      [in] A reference to the script objec.t
    /// @param sceneNode   [in] A reference to the scene node whose Lua representation is being updated.
    /// @param componentID [in] The ID of the component that is being unregistered.
    void UnregisterComponent(GT::Script &script, SceneNode &sceneNode, const char* componentID);


    /// Pushes a scene node object to the top of the stack.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node whose Lua counterpart is being pushed.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    ///     @par
    ///     The scene node will need to be popped when no longer needed.
    void PushSceneNode(GT::Script &script, SceneNode &sceneNode);


    /// Serializes the scene node based off the OnSerialize events.
    ///
    /// @param script     [in] A reference to the main script object.
    /// @param sceneNode  [in] A reference to the scene node that is being serialized.
    /// @param serializer [in] A reference to the serializer to write the data to.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void DoOnSerialize(GT::Script &script, SceneNode &sceneNode, Serializer &serializer);

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
    void DoOnDeserialize(GT::Script &script, SceneNode &sceneNode, Deserializer &deserializer);


    /// Calls the OnUpdate event on the given scene node.
    ///
    /// @param script             [in] A reference to the main script object.
    /// @param sceneNode          [in] A reference to the scene node having it's event called.
    /// @param deltaTimeInSeconds [in] The time since the last update. Used for time-based operations.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnUpdate(GT::Script &script, SceneNode &sceneNode, double deltaTimeInSeconds);

    /// Calls the OnStartup event on the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node having it's event called.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnStartup(GT::Script &script, SceneNode &sceneNode);

    /// Calls the OnShutdown event on the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node having it's event called.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnShutdown(GT::Script &script, SceneNode &sceneNode);

    /// Calls the OnShow event on the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node having it's event called.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnShow(GT::Script &script, SceneNode &sceneNode);

    /// Calls the OnHide event on the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node having it's event called.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnHide(GT::Script &script, SceneNode &sceneNode);

    /// Calls the OnSceneNodeEnter event on the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node having it's event called.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnSceneNodeEnter(GT::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

    /// Calls the OnEnterSceneNode event on the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node having it's event called.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnEnterSceneNode(GT::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

    /// Calls the OnSceneNodeLeave event on the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node having it's event called.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnSceneNodeLeave(GT::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

    /// Calls the OnLeaveSceneNode event on the given scene node.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param sceneNode [in] A reference to the scene node having it's event called.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnLeaveSceneNode(GT::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

    /// Calls the OnSerializeGlobalData event on the given scene node.
    ///
    /// @param script     [in] A reference to the main script object.
    /// @param sceneNode  [in] A reference to the scene node that's having it's event called.
    /// @param serializer [in] A reference to the serialize to write the data to.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnSerializeGlobalData(GT::Script &script, SceneNode &sceneNode, Serializer &serializer);

    /// Calls the OnDeserializeGlobalData event on the given scene node.
    ///
    /// @param script       [in] A reference to the main script object.
    /// @param sceneNode    [in] A reference to the scene node that's having it's event called.
    /// @param deserializer [in] A reference to the deserializer to read the data from.
    ///
    /// @remarks
    ///     The scene node must be instantiated on a registered scene for this to work.
    void PostSceneNodeEvent_OnDeserializeGlobalData(GT::Script &script, SceneNode &sceneNode, Deserializer &deserializer);


    namespace SceneNodeFFI
    {
        /// Creates a new empty scene node, returning a pointer to it.
        int Create(GT::Script &script);

        /// Deletes a scene node that was created with CreateSceneNode(GT::Script &).
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node returned by CreateSceneNode(GT::Script &).
        int Delete(GT::Script &script);


        /// Retrieves the ID of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose ID is being retrieved.
        ///
        ///     An important note. In the C++ side, the ID is 64-bits. On the Lua side, it is 32-bits.
        int GetID(GT::Script &script);


        /// Retrieves the name of the given component.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose name is being retrieved.
        int GetName(GT::Script &script);

        /// Sets the name of the given component.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose name is being set.
        ///     Argument 2: The new name for the scene node.
        int SetName(GT::Script &script);


        /// Retrieves a pointer to the parent scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose parent is being retrieved.
        int GetParentPtr(GT::Script &script);

        /// Attaches a scene node to the given node as a child.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose having a child attached.
        ///     Argument 2: A pointer to the child node.
        ///     Argument 3: A boolean controlling whether or not the world transformation should remain constant. 'true' = keep the world transform.
        int AttachChild(GT::Script &script);

        /// Orphans a scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node being orphaned.
        ///     Argument 2: A boolean controlling whether or not the world transformation should remain constant. 'true' = keep the world transform.
        int Orphan(GT::Script &script);

        /// Determines whether or not the given scene node is an ancestor of another.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the main scene node.
        ///     Argument 2: A pointer to the ancestor scene node.
        int IsAncestor(GT::Script &script);

        /// Determines whether or not the given scene node is a descendant of another.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the main scene node.
        ///     Argument 2: A pointer to the descendant scene node.
        int IsDescendant(GT::Script &script);

        /// Determines whether or not the given scene node is related to another.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the main scene node.
        ///     Argument 2: A pointer to the relative scene node.
        int IsRelative(GT::Script &script);


        /// Retrieves a table containing the IDs of the child nodes.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node in question.
        int GetChildrenIDs(GT::Script &script);

        /// Retrieves the ID of the first child scene node with the given name.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose child is being retrieved.
        int GetChildIDByName(GT::Script &script);

        /// Retrieves the ID of the first descendant scene node with the given name.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose descendant is being retrieved.
        int GetDescendantIDByName(GT::Script &script);


        /// Adds a new component to the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose having the component added to it.
        ///     Argument 2: The name of the component to add, as a string.
        ///     @par
        ///     Returns a table representing the new component.
        int AddComponent(GT::Script &script);

        /// Removes a component from the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose having the component removed.
        ///     Argument 2: The name of the componet to remove, as a string.
        int RemoveComponent(GT::Script &script);

        /// Retrieves a component from the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose having a component retrieved.
        ///     Argument 2: The name of the component to retrieve, as a string.
        ///     @par
        ///     This will instantiate a new table every time it is called. If the given scene node does not have a component of the given name, this will return nil.
        int GetComponent(GT::Script &script);

        /// Returns a table containing the component IDs (names) that are attached to the given node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose component IDs are being retrieved.
        int GetAttachedComponentIDs(GT::Script &script);


        /// Disables position inheritance.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int DisablePositionInheritance(GT::Script &script);

        /// Enables position inheritance.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int EnablePositionInheritance(GT::Script &script);

        /// Determines whether or not position inheritance is enabled.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Return: True if position inheritance is enabled; false otherwise.
        int IsPositionInheritanceEnabled(GT::Script &script);


        /// Disables orientation inheritance.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int DisableOrientationInheritance(GT::Script &script);

        /// Enables orientation inheritance.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int EnableOrientationInheritance(GT::Script &script);

        /// Determines whether or not orientation inheritance is enabled.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Return: True if orientation inheritance is enabled; false otherwise.
        int IsOrientationInheritanceEnabled(GT::Script &script);


        /// Disables scale inheritance.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int DisableScaleInheritance(GT::Script &script);

        /// Enables scale inheritance.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int EnableScaleInheritance(GT::Script &script);

        /// Determines whether or not scale inheritance is enabled.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Return: True if scale inheritance is enabled; false otherwise.
        int IsScaleInheritanceEnabled(GT::Script &script);



        /// Retrieves the local position of the given scene node as 3 return values.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose position is being retrieved.
        ///     Return:     A math.vec3() containing the position, relative to the parent.
        int GetPosition(GT::Script &script);

        /// Retrieves the world position of the given scene node as 3 floats.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose world position is being retrieved.
        ///     Return:     A math.vec3() containing the world position.
        int GetWorldPosition(GT::Script &script);

        /// Sets the local position of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose position is being set.
        ///     Argument 2: The x position or a math.vec3() containing the position.
        ///     Argument 3: The y position.
        ///     Argument 4: The z position.
        int SetPosition(GT::Script &script);

        /// Sets the world position of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose position is being set.
        ///     Argument 2: The x position or a math.vec3() containing the position.
        ///     Argument 3: The y position.
        ///     Argument 4: The z position.
        int SetWorldPosition(GT::Script &script);


        /// Retrieves the local orientation of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose orientation is being retrieved.
        ///     Return:     A math.quat containing the local orientation.
        int GetOrientation(GT::Script &script);

        /// Retrieves the world orientation of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose orientation is being retrieved.
        ///     Return:     A math.quat containing the world orientation.
        int GetWorldOrientation(GT::Script &script);

        /// Sets the local orientation of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The math.quat containing the orientation.
        int SetOrientation(GT::Script &script);

        /// Sets the world orientation of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The math.quat containing the orientation.
        int SetWorldOrientation(GT::Script &script);


        /// Retrieves the local Euler angles representing the rotation of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose rotation is being retrieved.
        ///     Return:     A math.vec3 containing the Euler angles in degrees.
        int GetEulerRotation(GT::Script &script);

        /// Retrieves the world Euler angles representing the rotation of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose rotation is being retrieved.
        ///     Return:     A math.vec3 containing the Euler angles in degrees.
        int GetWorldEulerRotation(GT::Script &script);

        /// Sets the local rotation of the given scene nodes as Euler angles.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The x rotation (pitch) or a math.vec3 containing the pitch, yaw and roll.
        ///     Argument 3: The y rotation (yaw).
        ///     Argument 4: The z rotation (roll).
        int SetEulerRotation(GT::Script &script);

        /// Sets the world rotation of the given scene nodes as Euler angles.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The x rotation (pitch) or a math.vec3 containing the pitch, yaw and roll.
        ///     Argument 3: The y rotation (yaw).
        ///     Argument 4: The z rotation (roll).
        int SetWorldEulerRotation(GT::Script &script);


        /// Retrieves the local scale of the given scene node as 3 return values.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
        ///     Return:     A math.vec3 containing the scale.
        int GetScale(GT::Script &script);

        /// Retrieves the world scale of the given scene node as 3 return values.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
        ///     Return:     A math.vec3 containing the scale.
        int GetWorldScale(GT::Script &script);

        /// Sets the local scale of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose scale is being set.
        ///     Argument 2: The x scale or a math.vec3 containing the scale.
        ///     Argument 3: The y scale.
        ///     Argument 4: The z scale.
        int SetScale(GT::Script &script);

        /// Sets the world scale of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node whose scale is being set.
        ///     Argument 2: The x scale or a math.vec3 containing the scale.
        ///     Argument 3: The y scale.
        ///     Argument 4: The z scale.
        int SetWorldScale(GT::Script &script);


        /// Translates the given scene node along it's local axis.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The amount of translation to apply as a math.vec3.
        int Translate(GT::Script &script);

        /// Rotates the given scene node around it's local axis.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The rotation angle.
        ///     Argument 3: The rotation axis as a math.vec3.
        int Rotate(GT::Script &script);

        /// Scales the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The amount to scale as a math.vec3.
        int Scale(GT::Script &script);


        /// Translates the given scene node along the world axis.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The amount of translation to apply as a math.vec3.
        int TranslateWorld(GT::Script &script);

        /// Rotates the scene node about the world axis.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The rotation angle.
        ///     Argument 3: The rotation axis.
        int RotateWorld(GT::Script &script);

        /// Rotates the scene node around the world axis, around the given pivot point.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The rotation angle.
        ///     Argument 3: The rotation axis.
        ///     Argument 4: The pivot point.
        int RotateWorldAroundPivot(GT::Script &script);


        /// Interpolates the position of the scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The destination position.
        ///     Argument 3: The interpolation delta (typically between 0 and 1).
        int InterpolatePosition(GT::Script &script);

        /// Interpolates the orientation of the scene node.
        ///
        /// @remarks
        ///     This does not perform a shortest-path interpolation. Use Slerp() for that.
        ///
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The destination orientation.
        ///     Argument 3: The interpolation delta (typically between 0 and 1).
        int InterpolateOrientation(GT::Script &script);

        /// Interpolates the scale of the scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The destination scale.
        ///     Argument 3: The interpolation delta (typically between 0 and 1).
        int InterpolateScale(GT::Script &script);

        /// Performs a shortest-path interpolation of the orientation of the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The destination orientation.
        ///     Argument 3: The delta (typically between 0 and 1).
        int Slerp(GT::Script &script);


        /// Clamps the position of the scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The min position.
        ///     Argument 3: The max position.
        int ClampPosition(GT::Script &script);

        /// Clamps the world position of the scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The min world position.
        ///     Argument 3: The max world position.
        int ClampWorldPosition(GT::Script &script);

        /// Clamps the scale of the scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The min scale.
        ///     Argument 3: The max scale.
        int ClampScale(GT::Script &script);

        /// Clamps the world scale of the scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The min world scale.
        ///     Argument 3: The max world scale.
        int ClampWorldScale(GT::Script &script);


        /// Looks at the given point in the world.
        ///
        /// @remarks
        ///     The up vector is expected to be normalized.
        ///
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: The target to look at.
        ///     Argument 3: The up vector.
        int LookAt(GT::Script &script);

        /// Looks at the given scene node.
        ///
        /// @remarks
        ///     The up vector is expected to be normalized.
        ///
        ///     Argument 1: A pointer to the scene node to orientate.
        ///     Argument 2: A pointer to the scene node to look at.
        ///     Argument 3: The up vector.
        int LookAtSceneNode(GT::Script &script);

        /// Orientates the scene to look in the given direction.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        ///     Argument 2: A normalized math.vec3 representing the direction to look in.
        ///     Argument 3: The up vector.
        int LookInDirection(GT::Script &script);


        /// Retrieves a normalized vector pointing in the direction of the given scene nodes local forward direction (-Z).
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int Forward(GT::Script &script);

        /// Retrieves a normalized vector pointing in the direction of the given scene nodes local up direction (+Y).
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int Up(GT::Script &script);

        /// Retrieves a normalized vector pointing in the direction of the given scene nodes local right direction (+X).
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int Right(GT::Script &script);

        /// Retrieves a normalized vector pointing in the direction of the given scene nodes world forward direction (-Z).
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int WorldForward(GT::Script &script);

        /// Retrieves a normalized vector pointing in the direction of the given scene nodes world up direction (+Y).
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int WorldUp(GT::Script &script);

        /// Retrieves a normalized vector pointing in the direction of the given scene nodes world right direction (+X).
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int WorldRight(GT::Script &script);



        /// Shows the scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int Show(GT::Script &script);

        /// Hides the scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int Hide(GT::Script &script);

        /// Determines whether or not the node is visible.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int IsVisible(GT::Script &script);



        /// Retrieves a pointer to the scene that this node is part of, if any.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene node.
        int GetScenePtr(GT::Script &script);




        ////////////////////////////////////////////////////////////////
        // Functions below are just helpers.

        /// A helper function for pushing a new instantiation of script-side component object.
        ///
        /// @param script    [in] The script where the result will be pushed to.
        /// @param component [in] A pointer to the C++ component.
        void PushComponent(GT::Script &script, Component* component);
    }
}

#endif