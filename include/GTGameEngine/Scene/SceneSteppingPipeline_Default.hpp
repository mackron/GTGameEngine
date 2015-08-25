// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneSteppingPipeline_Default
#define GT_SceneSteppingPipeline_Default

#include <cstdint>
#include <GTGameEngine/Scene/SceneSteppingPipeline.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    class SceneCallback;

    /// The default implementation of the scene stepping pipeline.
    class SceneSteppingPipeline_Default : public SceneSteppingPipeline
    {
    public:

        /// Constructor.
        SceneSteppingPipeline_Default(SceneCallback &callback);


        /// @copydoc SceneSteppingPipeline::InsertSceneNode()
        void InsertSceneNode(SceneNode* pSceneNode);

        /// @copydoc SceneSteppingPipeline::RemoveSceneNode()
        void RemoveSceneNode(SceneNode* pSceneNode);


        /// @copydoc SceneSteppingPipeline::Step()
        void Step(double deltaTimeInSeconds);

        /// @copydoc SceneSteppingPipeline::IsStepping()
        bool IsStepping() const;



    private:

        /// Performs the main update.
        void Update(double deltaTimeInSeconds);

        /// Steps the physics simulation.
        void StepPhysics(double deltaTimeInSeconds);

        /// Peforms the post-update process. This is where every scene node will be have their transforms marked as unchanged.
        void PostUpdate(double deltaTimeInSeconds);

        /// Posts all of the cached events.
        void PostEvents();

        /// Cleans up all of the objects that have been marked for deletion.
        void Cleanup();


    private:

        /// A reference to the callback that was passed to the constructor. This pipeline will route a lot of the workd
        /// to this callback. Indeed, the callback is actually where most of the hard work is done, while the pipeline
        /// will help with more of the trickier and mundane stuff.
        SceneCallback &m_callback;

        /// State flags.
        uint32_t m_stateFlags;


        /// The list of scene nodes requiring an update. When doing Update(), only these scene nodes are iterated.
        GTLib::Vector<SceneNode*> m_sceneNodesRequiringUpdate;

        /// The list of scene nodes requiring a post-update. When doing PostUpdate(), only these scene nodes are iterated.
        GTLib::Vector<SceneNode*> m_sceneNodesRequiringPostUpdate;

        /// The list of dynamic non-static scene nodes. Scenes nodes that are not in this list are assumed to have a constant transformation.
        GTLib::Vector<SceneNode*> m_dynamicSceneNodes;

        /// The list of dynamic physics objects.
        GTLib::Vector<SceneNode*> m_dynamicPhysicsSceneNodes;



        ///////////////////////////////////
        // Flags

        static const uint32_t StateFlag_IsStepping = (1 << 0);
    };
}

#endif
