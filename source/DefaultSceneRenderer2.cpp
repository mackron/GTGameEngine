// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer2.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    DefaultSceneRenderer2::DefaultSceneRenderer2()
        : viewportFramebuffers()
    {
    }

    DefaultSceneRenderer2::~DefaultSceneRenderer2()
    {
    }


    void DefaultSceneRenderer2::Begin(Scene &scene)
    {
        (void)scene;
    }

    void DefaultSceneRenderer2::End(Scene &scene)
    {
        (void)scene;
    }

    void DefaultSceneRenderer2::RenderViewport(Scene &scene, SceneViewport &viewport)
    {
        (void)scene;


        // 1) Make everything current.
        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);
        {
            Renderer2::SetCurrentFramebuffer(framebuffer->framebuffer);

            int drawBuffers[] = {0};
            Renderer2::SetDrawBuffers(1, drawBuffers);


            // Clear.
            Renderer2::SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
            Renderer2::SetClearDepth(1.0f);
            //Renderer2::SetClearStencil(0);
            Renderer2::Clear(BufferType_Colour | BufferType_Depth | BufferType_Stencil);
        }
    }

    void DefaultSceneRenderer2::AddViewport(SceneViewport &viewport)
    {
        // We create a framebuffer for every viewport. We map these to viewports with a simple GTCore::Map container.
        auto framebuffer = this->CreateFramebuffer(viewport.GetWidth(), viewport.GetHeight());
        assert(framebuffer != nullptr);
        {
            viewport.SetColourBuffer(framebuffer->colourOutputBuffer);
            this->viewportFramebuffers.Add(&viewport, framebuffer);
        }
    }

    void DefaultSceneRenderer2::RemoveViewport(SceneViewport &viewport)
    {
        // When a viewport is removed, the framebuffer needs to be deleted.
        viewport.SetColourBuffer(nullptr);

        auto iFramebuffer = this->viewportFramebuffers.Find(&viewport);
        assert(iFramebuffer != nullptr);
        {
            auto framebuffer = iFramebuffer->value;
            assert(framebuffer != nullptr);
            {
                this->DeleteFramebuffer(framebuffer);
                this->viewportFramebuffers.RemoveByIndex(iFramebuffer->index);
            }
        }
    }

    void DefaultSceneRenderer2::OnViewportResized(SceneViewport &viewport)
    {
        (void)viewport;
    }





    /////////////////////////////////////////////////////////
    // Private

    DefaultSceneRendererFramebuffer* DefaultSceneRenderer2::CreateFramebuffer(unsigned int width, unsigned int height)
    {
        auto newFramebuffer = new DefaultSceneRendererFramebuffer;
        newFramebuffer->framebuffer        = Renderer2::CreateFramebuffer();
        newFramebuffer->depthStencilBuffer = Renderer2::CreateTexture2D();
        newFramebuffer->colourOutputBuffer = Renderer2::CreateTexture2D();


        // Sizes and formats need to be set.
        newFramebuffer->depthStencilBuffer->SetData(width, height, GTImage::ImageFormat_Depth24_Stencil8);
        newFramebuffer->colourOutputBuffer->SetData(width, height, GTImage::ImageFormat_RGBA8);


        // Renderer needs to know about the new texture states.
        Renderer2::PushTexture2DData(*newFramebuffer->depthStencilBuffer);
        Renderer2::PushTexture2DData(*newFramebuffer->colourOutputBuffer);

        // Filters.
        Renderer2::SetTexture2DFilter(*newFramebuffer->colourOutputBuffer, TextureFilter_Nearest, TextureFilter_Nearest);
        
        // Wrap Modes.
        Renderer2::SetTexture2DWrapMode(*newFramebuffer->colourOutputBuffer, TextureWrapMode_ClampToEdge);


        // Attach to the main framebuffer.
        newFramebuffer->framebuffer->AttachDepthStencilBuffer(newFramebuffer->depthStencilBuffer);
        newFramebuffer->framebuffer->AttachColourBuffer(newFramebuffer->colourOutputBuffer, 0);

        Renderer2::PushAttachments(*newFramebuffer->framebuffer);


        return newFramebuffer;
    }

    void DefaultSceneRenderer2::DeleteFramebuffer(DefaultSceneRendererFramebuffer* framebufferToDelete)
    {
        Renderer2::DeleteTexture2D(framebufferToDelete->depthStencilBuffer);
        Renderer2::DeleteTexture2D(framebufferToDelete->colourOutputBuffer);
        
        Renderer2::DeleteFramebuffer(framebufferToDelete->framebuffer);

        delete framebufferToDelete;
    }

    DefaultSceneRendererFramebuffer* DefaultSceneRenderer2::GetViewportFramebuffer(SceneViewport &viewport)
    {
        auto iFramebuffer = this->viewportFramebuffers.Find(&viewport);
        if (iFramebuffer != nullptr)
        {
            return iFramebuffer->value;
        }

        return nullptr;
    }
}