
#ifndef __GTEngine_SceneRenderer_hpp_
#define __GTEngine_SceneRenderer_hpp_

#include "SceneViewport.hpp"

namespace GTEngine
{
    /// Class responsible for the rendering a scene.
    class SceneRenderer
    {
    public:

        /// Constructor.
        SceneRenderer() {}

        /// Destructor.
        virtual ~SceneRenderer() {}


        /// Called when rendering has started for the next frame.
        ///
        /// @remarks
        ///     This will be called once for every frame.
        virtual void Begin(Scene &scene) = 0;

        /// Called when rendering has ended.
        ///
        /// @remarks
        ///     This will be called once for every frame, after rendering has finished.
        virtual void End(Scene &scene) = 0;


        /// Called when rendering has started for the given viewport.
        ///
        /// @remarks viewport [in] The viewport that has started rendering.
        ///
        /// @remarks
        ///     This will be called once for every viewport.
        virtual void BeginViewport(Scene &scene, SceneViewport &viewport) = 0;

        /// Called when rendering has ended for the given viewport
        ///
        /// @param viewport [in] The viewport that has finished rendering.
        virtual void EndViewport(Scene &scene, SceneViewport &viewport) = 0;


        /// Called when a chunk of geometry needs to be drawn.
        ///
        /// @param geometry  [in] The vertex array containing the geometry to draw.
        /// @param material  [in] The material to draw the geometry with.
        /// @param transform [in] The transform to apply to the geometry.
        virtual void Geometry(const VertexArray &vertexArray, const Material &material, const glm::vec3 &transform) = 0;

        /// Notifies the renderer that an ambient light needs to be used in the scene.
        ///
        /// @param colour [in] The colour of the ambient light.
        virtual void AmbientLight(const glm::vec3 &colour) = 0;

        /// Notifies the renderer that a directional light needs to be used in the scene.
        ///
        /// @param colour    [in] The colour of the light.
        /// @param direction [in] The direction of the light.
        virtual void DirectionalLight(const glm::vec3 &colour, const glm::vec3 direction) = 0;

        /// Notifies the renderer than a point light needs to be used in the scene.
        ///
        /// @param colour               [in] The colour of the light.
        /// @param position             [in] The position of the light.
        /// @param constantAttenuation  [in] The constant attenuation of the light.
        /// @param linearAttenuation    [in] The linear attenuation of the light.
        /// @param quadraticAttenuation [in] The quadratic attenuation of the light.
        virtual void PointLight(const glm::vec3 &colour, const glm::vec3 &position, float constantAttenuation, float linearAttenuation, float quadraticAttenuation) = 0;

        /// Notifies the renderer that a spot light needs to be used in the scene.
        ///
        /// @param colour               [in] The colour of the light.
        /// @param position             [in] The position of the light.
        /// @param direction            [in] The direction of the light.
        /// @param innerRadius          [in] The inner radius of the light.
        /// @param outerRadius          [in] The outer radius of the light.
        /// @param constantAttenuation  [in] The constant attenuation of the light.
        /// @param linearAttenuation    [in] The linear attenuation of the light.
        /// @param quadraticAttenuation [in] The quadratic attenuation of the light.
        virtual void SpotLight(const glm::vec3 &colour, const glm::vec3 &position, const glm::vec3 &direction, float innerRadius, float outerRadius, float constantAttenuation, float linearAttenuation, float quadraticAttenuation) = 0;
    };
}

#endif
