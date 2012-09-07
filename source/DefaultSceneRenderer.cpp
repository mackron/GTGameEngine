
#include <GTEngine/DefaultSceneRenderer.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    DefaultSceneRenderer::DefaultSceneRenderer()
    {
    }

    DefaultSceneRenderer::~DefaultSceneRenderer()
    {
    }


    void DefaultSceneRenderer::Begin(Scene &scene)
    {
        (void)scene;
    }

    void DefaultSceneRenderer::End(Scene &scene)
    {
        (void)scene;
    }


    void DefaultSceneRenderer::BeginViewport(Scene &scene, SceneViewport &viewport)
    {
        (void)scene;

        auto &layers = viewport.GetLayerCameraMap();
        if (layers.count > 0)
        {
            for (size_t i = 0; i < layers.count; ++i)
            {
                glm::mat4 mvp = viewport.GetMVPMatrix();
            }
        }

        

        //scene.QueryVisibleObjects(mvp, callback);
    }

    void DefaultSceneRenderer::EndViewport(Scene &scene, SceneViewport &viewport)
    {
        (void)scene;
        (void)viewport;
    }



    void Geometry(const VertexArray &vertexArray, const Material &material, const glm::vec3 &transform)
    {
        (void)vertexArray;
        (void)material;
        (void)transform;
    }

    void AmbientLight(const glm::vec3 &colour)
    {
        (void)colour;
    }

    void DirectionalLight(const glm::vec3 &colour, const glm::vec3 direction)
    {
        (void)colour;
        (void)direction;
    }

    void PointLight(const glm::vec3 &colour, const glm::vec3 &position, float constantAttenuation, float linearAttenuation, float quadraticAttenuation)
    {
        (void)colour;
        (void)position;
        (void)constantAttenuation;
        (void)linearAttenuation;
        (void)quadraticAttenuation;
    }

    void SpotLight(const glm::vec3 &colour, const glm::vec3 &position, const glm::vec3 &direction, float innerRadius, float outerRadius, float constantAttenuation, float linearAttenuation, float quadraticAttenuation)
    {
        (void)colour;
        (void)position;
        (void)direction;
        (void)innerRadius;
        (void)outerRadius;
        (void)constantAttenuation;
        (void)linearAttenuation;
        (void)quadraticAttenuation;
    }
}