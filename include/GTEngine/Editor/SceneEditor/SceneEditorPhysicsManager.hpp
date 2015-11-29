// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SceneEditorPhysicsManager_hpp_
#define __GTEngine_SceneEditorPhysicsManager_hpp_

#include "../../DefaultScenePhysicsManager.hpp"

namespace GT
{
    class SceneEditorPhysicsManager : public DefaultScenePhysicsManager
    {
    public:

        /// Constructor.
        SceneEditorPhysicsManager();

        /// Destructor.
        ~SceneEditorPhysicsManager();


        /// Enables physics simulation.
        void EnableSimulation();

        /// Disable physics simulation.
        void DisableSimulation();

        /// Determines whether or not the physics simulation is enabled.
        bool IsSimulationEnabled() const { return this->isEnabled; }


        /// DefaultScenePhysicsManager::Step()
        void Step(double deltaTimeInSeconds);



    private:

        /// Keeps track of whether or not the physics simulation is enabled.
        bool isEnabled;
    };
}

#endif
