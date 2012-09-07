
#ifndef __GTEngine_DefaultSceneRenderer_hpp_
#define __GTEngine_DefaultSceneRenderer_hpp_

#include "SceneRenderer.hpp"
#include "Rendering/Framebuffer.hpp"

class Scene;

namespace GTEngine
{
    /// The default scene renderer.
    class DefaultSceneRenderer : public SceneRenderer
    {
    public:

        /// Constructor.
        DefaultSceneRenderer();

        /// Destructor.
        virtual ~DefaultSceneRenderer();


        /// SceneRenderer::Begin().
        virtual void Begin(Scene &scene);

        /// SceneRenderer::End().
        virtual void End(Scene &scene);


        /// SceneRenderer::BeginViewport().
        virtual void BeginViewport(Scene &scene, SceneViewport &viewport);

        /// SceneRenderer::EndViewport().
        virtual void EndViewport(Scene &scene, SceneViewport &viewport);



        /// SceneRenderer::Geometry().
        virtual void Geometry(const VertexArray &vertexArray, const Material &material, const glm::vec3 &transform);

        /// SceneRenderer::AmbientLight().
        virtual void AmbientLight(const glm::vec3 &colour);

        /// SceneRenderer::DirectionalLight().
        virtual void DirectionalLight(const glm::vec3 &colour, const glm::vec3 direction);

        /// SceneRenderer::PointLight().
        virtual void PointLight(const glm::vec3 &colour, const glm::vec3 &position, float constantAttenuation, float linearAttenuation, float quadraticAttenuation);

        /// SceneRenderer::SpotLight().
        virtual void SpotLight(const glm::vec3 &colour, const glm::vec3 &position, const glm::vec3 &direction, float innerRadius, float outerRadius, float constantAttenuation, float linearAttenuation, float quadraticAttenuation);



    private:

        /// Class representing a framebuffer for use by this renderer. There will usually be a framebuffer for every viewport.
        class Framebuffer : public GTEngine::Framebuffer
        {
        public:




        private:


        };
    };
}

#endif