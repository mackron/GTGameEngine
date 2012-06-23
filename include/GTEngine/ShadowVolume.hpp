
#ifndef __GTEngine_ShadowVolume_hpp_
#define __GTEngine_ShadowVolume_hpp_

#include "Rendering/VertexArray.hpp"
#include "Math.hpp"
#include "Model.hpp"

namespace GTEngine
{
    /// Class representing the shadow volume of a model.
    ///
    /// Shadow volume generation is based on the work in the paper Robust, Geometry-Independent Shadow Volumes, by Graham Aldridge and Eric Woods.
    class ShadowVolume
    {
    public:

        /// Constructor.
        ShadowVolume();

        /// Destructor.
        ~ShadowVolume();


        /// Builds the shadow volume from a light, model and clipping planes.
        ///
        /// @param lightPosition [in] The position of the light in world space.
        /// @param lightRadius   [in] The approximate radius of the light. Vertices will not be projected further than this.
        /// @param model         [in] A reference to the model that is having a shadow volume generated.
        /// @param nearPlane     [in] The near plane to clamp against.
        /// @param farPlane      [in] The far plane to clamp against.
        ///
        /// @remarks
        ///     The shadow volume vertices will be clamped first against the light's radius, and then again against the near/far planes.
        void BuildFromPointLight(const glm::vec3 &lightPosition, float lightRadius, const Model &model, const Math::Plane &nearPlane, const Math::Plane &farPlane);


    private:

        /// The vertex array containing the vertex data. This is in P3 format.
        VertexArray vertexArray;
    };
}

#endif