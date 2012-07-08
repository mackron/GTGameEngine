
#ifndef __GTEngine_SceneUpdateManager_hpp_
#define __GTEngine_SceneUpdateManager_hpp_

#include "SceneObject.hpp"

namespace GTEngine
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

        /// Destructor.
        virtual ~SceneUpdateManager() {}


        /// Determines whether or not an objects needs updating.
        ///
        /// @param object [in] The object to check.
        ///
        /// @remarks
        ///     This method is called during a preprocess step for determining whether or not the object should be added with AddObject(). If this
        ///     method returns false, AddObject() will not be called on the object. Otherwise, it will. This method makes it simpler for implementations
        ///     of AddObject().
        virtual bool NeedsUpdate(SceneObject &object) const = 0;

        /// Called when an object is added.
        ///
        /// @param object [in] A reference to the object being added to the manager.
        virtual void AddObject(SceneObject &object) = 0;

        /// Called when an object is removed.
        ///
        /// @param object [in] A reference to the object being removed.
        ///
        /// @remarks
        ///     It is possible that this method can be called on an object that was not added with AddObject(). This case must be handled.
        virtual void RemoveObject(SceneObject &object) = 0;
    

        /// Performs the update step.
        ///
        /// @param deltaTimeInSeconds [in] The delta time in seconds (time between updates).
        ///
        /// @remarks
        ///     This should step everything that needs stepping, not including physics.
        virtual void Step(double deltaTimeInSeconds) = 0;
    };
}

#endif
