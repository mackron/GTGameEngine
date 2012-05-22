
#ifndef __GTEngine_SceneCullingDbvtPolicy_hpp_
#define __GTEngine_SceneCullingDbvtPolicy_hpp_

#include "Physics.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    class SceneNode;
    class ModelComponent;
    class PointLightComponent;
    class SpotLightComponent;

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


        /// Virtual function that will be called for every visible model component.
        virtual void ProcessModel(ModelComponent &) {}

        /// Virtual function that will be called for every visible point light component.
        virtual void ProcessPointLight(PointLightComponent &) {}

        /// Virtual function taht will be called for every visible spot light component.
        virtual void ProcessSpotLight(SpotLightComponent &) {}



        /// btDbvt::ICollide::Process()
        void Process(const btDbvtNode* node, btScalar) { this->Process(node); }
        void Process(const btDbvtNode* leaf);
    };
}

#endif