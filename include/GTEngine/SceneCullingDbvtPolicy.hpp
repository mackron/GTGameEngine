
#ifndef __GTEngine_SceneCullingDbvtPolicy_hpp_
#define __GTEngine_SceneCullingDbvtPolicy_hpp_

#include "Physics.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    class SceneNode;
    class ModelComponent;
    class PointLightComponent;

    /// The Dbvt collision policy to use for scene node culling.
    ///
    /// Basically, this is used for finding visible scene nodes during occlusion and frustum culling. At the moment this is only used for frustum
    /// culling, but it will be modified to do occlusion culling also.
    ///
    /// This will only include scene nodes that are actually visible.
    ///
    /// This assumes the user data of the collision objects are a SceneNode object.
    struct SceneCullingDbvtPolicy : btDbvt::ICollide
    {
        /// Constructor.
        SceneCullingDbvtPolicy();

        /// Destructor.
        virtual ~SceneCullingDbvtPolicy();


        /// Virtual function that will be called for every visible scene node.
        ///
        /// @remarks
        ///     This will be called for each collision object, meaning it is possible that it will be called multiple times for a single scene node.
        //virtual void ProcessSceneNode(SceneNode &) {};

        /// Virtual function that will be called for every visible model component.
        virtual void ProcessModel(ModelComponent &) {}

        /// Virtual function that will be called for every visible point light component.
        virtual void ProcessPointLight(PointLightComponent &) {}



        /// btDbvt::ICollide::Process()
        void Process(const btDbvtNode* node, btScalar) { this->Process(node); }
        void Process(const btDbvtNode* leaf);
    };
}

#endif