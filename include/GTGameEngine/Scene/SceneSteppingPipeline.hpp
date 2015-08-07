// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneSteppingPipeline_hpp_
#define __GT_SceneSteppingPipeline_hpp_

namespace GT
{
    class Scene;
    class SceneNode;

    /// Base class for scene stepping pipelines.
    class SceneSteppingPipeline
    {
    public:

        /// Inserts a scene node.
        virtual void InsertSceneNode(SceneNode* pSceneNode) = 0;

        /// Removes a scene node.
        virtual void RemoveSceneNode(SceneNode* pSceneNode) = 0;


        /// Performs the step.
        virtual void Step(double deltaTimeInSeconds) = 0;

        /// Determines whether or not the pipeline is currently stepping.
        virtual bool IsStepping() const = 0;
    };
}

#endif
