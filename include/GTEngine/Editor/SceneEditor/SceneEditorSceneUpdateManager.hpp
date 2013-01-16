
#ifndef __GTEngine_SceneEditorSceneUpdateManager_hpp_
#define __GTEngine_SceneEditorSceneUpdateManager_hpp_

#include "../../DefaultSceneUpdateManager.hpp"

namespace GTEngine
{
    /// Special update manager for scenes loaded into the scene editor.
    class SceneEditorSceneUpdateManager : public DefaultSceneUpdateManager
    {
    public:

        /// Constructor.
        SceneEditorSceneUpdateManager(const SceneNode &cameraNode);

        /// Destructor.
        ~SceneEditorSceneUpdateManager();


        /// DefaultSceneUpdateManager::Step()
        void StepSceneNode(SceneNode &node, double deltaTimeInSeconds);


    private:

        /// The camera node. We will use this for orientating sprites.
        const SceneNode &cameraNode;


    private:    // No copying.
        SceneEditorSceneUpdateManager(const SceneEditorSceneUpdateManager &);
        SceneEditorSceneUpdateManager & operator=(const SceneEditorSceneUpdateManager &);
    };
}

#endif