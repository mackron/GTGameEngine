
#ifndef __GTEngine_DefaultSceneUpdateManager_hpp_
#define __GTEngine_DefaultSceneUpdateManager_hpp_

#include "SceneUpdateManager.hpp"
#include "SceneNode.hpp"
#include "Physics/DynamicsWorld.hpp"
#include <GTCore/Vector.hpp>

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
        virtual bool NeedsUpdate(SceneObject &object) const;

        /// SceneUpdateManager::AddObject().
        virtual void AddObject(SceneObject &object);

        /// SceneUpdateManager::RemoveObject().
        virtual void RemoveObject(SceneObject &object);

        /// SceneUpdateManager::Step().
        virtual void Step(double deltaTimeInSeconds);


    protected:

        /// Steps a scene node.
        ///
        /// @param node               [in] The scene node being updated.
        /// @param deltaTimeInSeconds [in] The delta time (time since the last step).
        ///
        /// @remarks
        ///     This is called from Step().
        virtual void StepSceneNode(SceneNode &node, double deltaTimeInSeconds);



    protected:

        /// The list of every object that should be updated.
        GTCore::Vector<SceneObject*> objects;
    };
}

#endif
