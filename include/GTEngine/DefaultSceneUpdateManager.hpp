// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_DefaultSceneUpdateManager_hpp_
#define __GTEngine_DefaultSceneUpdateManager_hpp_

#include "SceneUpdateManager.hpp"
#include "SceneNode.hpp"
#include "Physics/DynamicsWorld.hpp"
#include <GTLib/Vector.hpp>

namespace GTEngine
{
    /// The default update manager for scenes.
    ///
    /// The class can often act as the base class for custom update managers.
    class DefaultSceneUpdateManager : public SceneUpdateManager
    {
    public:

        /// Constructor.
        DefaultSceneUpdateManager();

        /// Destructor.
        virtual ~DefaultSceneUpdateManager();


        /// SceneUpdateManager::NeedsUpdate().
        virtual bool NeedsUpdate(SceneNode &sceneNode) const;

        /// SceneUpdateManager::AddObject().
        virtual void AddSceneNode(SceneNode &sceneNode);

        /// SceneUpdateManager::RemoveObject().
        virtual void RemoveSceneNode(SceneNode &sceneNode);

        /// SceneUpdateManager::Step().
        virtual void Step(double deltaTimeInSeconds, SceneCullingManager &cullingManager);


    protected:

        /// Steps a scene node.
        ///
        /// @param node               [in] The scene node being updated.
        /// @param deltaTimeInSeconds [in] The delta time (time since the last step).
        ///
        /// @remarks
        ///     This is called from Step().
        virtual void StepSceneNode(SceneNode &node, double deltaTimeInSeconds, SceneCullingManager &cullingManager);



    protected:

        /// The list of every scene node that should be updated.
        GTLib::Vector<SceneNode*> sceneNodes;
    };
}

#endif
