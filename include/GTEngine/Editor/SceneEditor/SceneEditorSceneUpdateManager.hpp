// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SceneEditorSceneUpdateManager
#define GT_SceneEditorSceneUpdateManager

#include "../../DefaultSceneUpdateManager.hpp"

namespace GT
{
    /// Special update manager for scenes loaded into the scene editor.
    class SceneEditorSceneUpdateManager : public DefaultSceneUpdateManager
    {
    public:

        /// Constructor.
        SceneEditorSceneUpdateManager(const SceneNode &cameraNode);

        /// Destructor.
        ~SceneEditorSceneUpdateManager();


        /// Enables physics simulation.
        void Enable();

        /// Disable physics simulation.
        void Disable();

        /// Determines whether or not the physics simulation is enabled.
        bool IsEnabled() const { return this->isEnabled; }


        /// Enables particles simulation.
        void EnableParticles();

        /// Disables particles simulation.
        void DisableParticles();

        /// Determines whether or not the particles simulation is enabled.
        bool IsParticlesEnabled() const { return this->isParticlesEnabled; }


        /// Pauses scene updates.
        void PauseSceneUpdates();

        /// Resumes scene updates.
        void ResumeSceneUpdates();

        /// Determines whether or not scene updates are enabled.
        bool IsSceneUpdatesPaused() const { return this->isSceneUpdatesPaused; }


        /// DefaultSceneUpdateManager::Step()
        void StepSceneNode(SceneNode &node, double deltaTimeInSeconds, SceneCullingManager &cullingManager);


    private:

        /// The camera node. We will use this for orientating sprites.
        const SceneNode &cameraNode;

        /// Keeps track of whether or not updating is enabled.
        bool isEnabled;

        /// Keeps track of whether or not particles are being updated while editting.
        bool isParticlesEnabled;

        /// Keeps track of whether or not scene updating is paused.
        bool isSceneUpdatesPaused;


    private:    // No copying.
        SceneEditorSceneUpdateManager(const SceneEditorSceneUpdateManager &);
        SceneEditorSceneUpdateManager & operator=(const SceneEditorSceneUpdateManager &);
    };
}

#endif