
#include <GTEngine/DefaultSceneRenderer.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

// In this file, DSR = DefaultSceneRenderer.

namespace GTEngine
{
    /// The callback used during the material pass.
    class DefaultSceneRenderer_MaterialPassCallback : public SceneCullingManager::VisibleCallback
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_MaterialPassCallback(DefaultSceneRenderer &rendererIn)
            : SceneCullingManager::VisibleCallback(), renderer(rendererIn)
        {
        }

        /// Destructor.
        ~DefaultSceneRenderer_MaterialPassCallback()
        {
        }


        /// SceneCullingManager::ProcessObjectModel().
        void ProcessObjectModel(SceneObject &object)
        {
            (void)object;
        }


        /// SceneCullingManager::ProcessObjectAmbientLight().
        void ProcessObjectAmbientLight(SceneObject &object)
        {
            this->renderer.__AmbientLight(object);
        }

        /// SceneCullingManager::ProcessObjectDirectionalLight().
        void ProcessObjectDirectionalLight(SceneObject &object)
        {
            this->renderer.__DirectionalLight(object);
        }

        /// SceneCullingManager::ProcessObjectPointLight().
        void ProcessObjectPointLight(SceneObject &object)
        {
            this->renderer.__PointLight(object);
        }

        /// SceneCullingManager::ProcessObjectSpotLight().
        void ProcessObjectSpotLight(SceneObject &object)
        {
            this->renderer.__SpotLight(object);
        }



    private:

        /// The renderer that owns this callback.
        DefaultSceneRenderer &renderer;


    private:    // No copying.
        DefaultSceneRenderer_MaterialPassCallback(const DefaultSceneRenderer_MaterialPassCallback &);
        DefaultSceneRenderer_MaterialPassCallback & operator=(const DefaultSceneRenderer_MaterialPassCallback &);
    };
}


namespace GTEngine
{
    DefaultSceneRenderer::DefaultSceneRenderer()
        : ambientLights(), directionalLights(), pointLights(), spotLights(),
          viewportFramebuffers(),  
          rcBegin()
    {
    }

    DefaultSceneRenderer::~DefaultSceneRenderer()
    {
    }


    void DefaultSceneRenderer::Begin(Scene &scene)
    {
        (void)scene;

        this->ambientLights.Clear();
        this->directionalLights.Clear();
        this->pointLights.Clear();
        this->spotLights.Clear();
    }

    void DefaultSceneRenderer::End(Scene &scene)
    {
        (void)scene;
    }


    void DefaultSceneRenderer::RenderViewport(Scene &scene, SceneViewport &viewport)
    {
        (void)scene;

        // We render layer-by-layer, starting at the top and working our way down.
        auto &layers = viewport.GetLayerCameraMap();
        if (layers.count > 0)
        {
            // The first thing we do is set the framebuffer to the current viewport. This also where the framebuffer will be resized if required.
            auto &rcBegin = this->rcBegin[Renderer::BackIndex].Acquire();
            //rcBegin.framebuffer = 
            Renderer::BackRCQueue->Append(rcBegin);



            for (size_t i = layers.count; i > 0; --i)
            {
                // We're going to start with the material pass. This pass will acquire the ambient lights that we'll use for the ambient sub-pass for lighting.
                DefaultSceneRenderer_MaterialPassCallback materialPassCallback(*this);
                scene.QueryVisibleObjects(viewport.GetMVPMatrix(static_cast<size_t>(i)), materialPassCallback);


                // With the material pass done, we now need to do lighting. The first part of the lighting is any lights that do not cast shadows.
            }


            // At this point the viewport will be finished and we can finish up.
        }
    }


    void DefaultSceneRenderer::AddViewport(SceneViewport &viewport)
    {
        this->viewportFramebuffers.Add(&viewport, new DefaultSceneRenderer::Framebuffer);
    }

    void DefaultSceneRenderer::RemoveViewport(SceneViewport &viewport)
    {
        this->viewportFramebuffers.Remove(&viewport);
    }

    void DefaultSceneRenderer::OnViewportResized(SceneViewport &viewport)
    {
        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);

        framebuffer->Resize(viewport.GetWidth(), viewport.GetHeight());
    }



    void DefaultSceneRenderer::__AmbientLight(const SceneObject &object)
    {
        this->ambientLights.PushBack(&object);
    }

    void DefaultSceneRenderer::__DirectionalLight(const SceneObject &object)
    {
        this->directionalLights.PushBack(&object);
    }

    void DefaultSceneRenderer::__PointLight(const SceneObject &object)
    {
        this->pointLights.PushBack(&object);
    }

    void DefaultSceneRenderer::__SpotLight(const SceneObject &object)
    {
        this->spotLights.PushBack(&object);
    }



    ///////////////////////////////////////////////
    // Private

    DefaultSceneRenderer::Framebuffer* DefaultSceneRenderer::GetViewportFramebuffer(SceneViewport &viewport)
    {
        auto iFramebuffer = this->viewportFramebuffers.Find(&viewport);
        assert(iFramebuffer != nullptr);

        return iFramebuffer->value;
    }
}