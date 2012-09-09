
// This file contains everything relating to resource management for the renderer.

#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Rendering/RenderCommand.hpp>
#include <GTEngine/Rendering/RCCache.hpp>
#include <GTEngine/Logging.hpp>

#include <gtgl/gtgl.h>


// Render commands.
namespace GTEngine
{
    /////////////////////////////////////////////////////////////
    // Texture2Ds

    struct Texture2D_GL20
    {
        Texture2D_GL20()
            : object(0)
        {
        }

        Texture2D_GL20(GLuint object)
            : object(object)
        {
        }

        GLuint object;
    };

    struct RCOnTexture2DCreated : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(texture != nullptr);
            glGenTextures(1, &texture->object);
        }

        Texture2D_GL20* texture;
    };

}


namespace GTEngine
{
    /// The RC queues for resource events.
    static RCQueue ResourceRCQueues[2];

    static RCCache<RCOnTexture2DCreated> RCCache_OnTexture2DCreated[2];



    void Renderer::OnTexture2DCreated(Texture2D &texture)
    {
        if (!(texture.IsFloatingPointFormat() && !Renderer::SupportFloatTextures()))
        {
            auto rendererData = new Texture2D_GL20;
            texture.SetRendererData(rendererData);

            auto &rc = RCCache_OnTexture2DCreated[Renderer::BackIndex].Acquire();
            rc.texture = rendererData;

            ResourceRCQueues[Renderer::BackIndex].Append(rc);
        }
        else
        {
            Log("Error Creating Texture2D: Floating-point textures are not supported.");
        }
    }

    void Renderer::OnTexture2DDeleted(Texture2D &texture)
    {
        (void)texture;
    }




    void Renderer::ExecuteFrontResourceRCQueue()
    {
        ResourceRCQueues[!Renderer::BackIndex].Execute();
        ResourceRCQueues[!Renderer::BackIndex].Clear();

        RCCache_OnTexture2DCreated[!Renderer::BackIndex].Reset();
    }
}