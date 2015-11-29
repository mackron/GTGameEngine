// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SceneUpdateManager
#define GT_SceneUpdateManager

#include "SceneNode.hpp"
#include "SceneCullingManager.hpp"

namespace GT
{
    /// Base class used for managing the updating of scene objects.
    ///
    /// The update manager basically handles the update phase of a scene. This does not include physics.
    ///
    /// AddObject() and RemoveObject() will be called for every single object that is added or removed from the scene. It is up to the manager
    /// to decide whether or not the object actually needs updating.
    ///
    /// Step() will be called whenever the scene is stepped.
    class SceneUpdateManager
    {
    public:

        /// Constructor.
        SceneUpdateManager()
            : isScriptEventsBlocked(false)
        {
        }

        /// Destructor.
        virtual ~SceneUpdateManager()
        {
        }


        /// Determines whether or not a scene node needs updating.
        ///
        /// @param sceneNode [in] The scene node to check.
        ///
        /// @remarks
        ///     This method is called during a preprocess step for determining whether or not the object should be added with AddObject(). If this
        ///     method returns false, AddObject() will not be called on the object. Otherwise, it will. This method makes it simpler for implementations
        ///     of AddObject().
        virtual bool NeedsUpdate(SceneNode &sceneNode) const = 0;

        /// Called when a scene node is added.
        ///
        /// @param sceneNode [in] A reference to the scene node being added to the manager.
        virtual void AddSceneNode(SceneNode &sceneNode) = 0;

        /// Called when a scene node is removed.
        ///
        /// @param sceneNode [in] A reference to the scene node being removed.
        ///
        /// @remarks
        ///     It is possible that this method can be called on an object that was not added with AddObject(). This case must be handled.
        virtual void RemoveSceneNode(SceneNode &sceneNode) = 0;
    

        /// Performs the update step.
        ///
        /// @param deltaTimeInSeconds [in] The delta time in seconds (time between updates).
        /// @param cullingManager     [in] A reference to the culling manager so the update manager can update culling information when needed.
        ///
        /// @remarks
        ///     This should step everything that needs stepping, not including physics.
        virtual void Step(double deltaTimeInSeconds, SceneCullingManager &cullingManager) = 0;



        /////////////////////////////////////////////
        // Non-virtual.

        /// Lets the manager know that events should not be posted to scripts.
        void BlockScriptEvents()
        {
            this->isScriptEventsBlocked = true;
        }

        /// Lets the manager know that events are allowed to be posted to scripts.
        void UnblockScriptEvents()
        {
            this->isScriptEventsBlocked = false;
        }

        /// Determines whether or not events should be blocked from being posted to scripts.
        bool IsScriptEventsBlocks() const
        {
            return this->isScriptEventsBlocked;
        }



    private:

        /// Keeps track of whether or not script events are blocked.
        bool isScriptEventsBlocked;
    };
}

#endif
