// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneState_hpp_
#define __GT_SceneState_hpp_

#include <cstdint>

namespace GT
{
    class SceneNode;
    class SceneEventHandler;

    /// Base class for scene states.
    class SceneState
    {
    public:

        /// Constructor.
        SceneState();

        /// Destructor.
        virtual ~SceneState();


        /// Creates a scene node with the given ID, but does not insert it into the scene.
        ///
        /// @param id [in] The ID to assign to the scene node.
        virtual SceneNode* CreateSceneNode(uint64_t id) = 0;

        /// Deletes the scene node with the given ID.
        ///
        /// @param id [in] The ID of the scene node to delete.
        virtual void DeleteSceneNode(uint64_t id) = 0;

        /// Deletes the given scene node.
        ///
        /// @param pSceneNode [in] A pointer to the scene node to delete.
        virtual void DeleteSceneNode(SceneNode* pSceneNode);


        /// Adds the given scene node to the scene.
        ///
        /// @param sceneNode [in] The scene node to add to the scene.
        ///
        /// @remarks
        ///     If the scene node's ID is zero or a scene node of the same ID already exists, this will fail.
        virtual bool InsertSceneNode(SceneNode* pSceneNode) = 0;

        /// Removes the given scene node.
        ///
        /// @param sceneNode [in] The scene node to remove from the scene.
        virtual void RemoveSceneNode(SceneNode* pSceneNode) = 0;


        /// Retrieves a pointer to the scene node with the given ID.
        ///
        /// @param sceneNodeID [in] The ID of the scene node to retrieve.
        virtual SceneNode* GetSceneNodeByID(uint64_t sceneNodeID) = 0;


        /// Sets the event handler.
        ///
        /// @param pEventHandler [in] A pointer to the new event handler.
        virtual void SetEventHandler(SceneEventHandler* pEventHandler) = 0;

        /// Retrieves a pointer to the event handler.
        ///
        /// @return A pointer to the event handler.
        virtual SceneEventHandler* GetEventHandler() = 0;



        /// Steps the scene state.
        ///
        /// @param deltaTimeInSeconds [in] The delta time in seconds.
        virtual void Step(double deltaTimeInSeconds) = 0;
    };
}

#endif
