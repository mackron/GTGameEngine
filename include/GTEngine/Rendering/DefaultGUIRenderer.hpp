// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Renderer_DefaultGUIRenderer_hpp_
#define __GTEngine_Renderer_DefaultGUIRenderer_hpp_

#include "Renderer.hpp"
#include <GTGUI/Rendering.hpp>

namespace GTEngine
{
    /// The default renderer for GUI's. This calls functions directly from the main Renderer class.
    class DefaultGUIRenderer : public GTGUI::Renderer
    {
    public:

        /// Constructor.
        DefaultGUIRenderer();

        /// Destructor.
        virtual ~DefaultGUIRenderer();


        /// GTGUI::Renderer::Begin()
        void Begin(const GTGUI::Server &guiServer);

        /// GTGUI::Renderer::End()
        void End();

        /// GTGUI::Renderer::BeginElementOnDrawEvent()
        void BeginElementOnDrawEvent(GTGUI::Element &element);

        /// GTUI::Renderer::EndElementOnDrawEvent()
        void EndElementOnDrawEvent(GTGUI::Element &element);

        /// GTGUI::Renderer::SetScissor()
        void SetScissor(int x, int y, unsigned int width, unsigned int height);

        /// GTGUI::Renderer::SetOffset()
        virtual void SetOffset(float offsetX, float offsetY);

        /// GTGUI::Renderer::SetTexture()
        virtual void SetTexture(GTGUI::ImageHandle image);

        /// GTGUI::Renderer::EnableBlending()
        virtual void EnableBlending();

        /// GTGUI::Renderer::DisableBlending()
        virtual void DisableBlending();

        /// GTGUI::Renderer::Draw()
        void Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount);


    private:

        /// Retrieves a texture for the given image.
        //Texture2D* AcquireTexture2DFromImage(const GTLib::Image* image);

        /// A helper for enabled all of the current state.
        void RestoreCurrentState();
        
        /// Sets the current shader.
        ///
        /// @remarks
        ///     If the current shader is already set to the input shader, this will return immediately.
        void SetCurrentShader(Shader* shader);


    private:

        /// The main shader.
        Shader* shader;
        
        /// The shader to use when A8 texture formats.
        Shader* shaderA8;
        
        
        /// The default texture.
        Texture2D* defaultTexture;
        

        /// The viewport width.
        unsigned int viewportWidth;

        /// The viewport height.
        unsigned int viewportHeight;

        /// The projection matrix.
        glm::mat4 projection;


        /// The current x offset to apply to geometry.
        float currentOffsetX;

        /// The current y offset to apply to geometry.
        float currentOffsetY;
        
        /// The current shader.
        Shader* currentShader;

        /// The current texture to apply to geometry.
        Texture2D* currentTexture;

        /// Whether or not blending is currently enabled.
        bool isBlendingEnabled;
        

        /// Keeps track of whether or not shader uniforms needs to be pushed to the renderer.
        bool uniformsRequirePush;
        
    private:    // No copying.
        DefaultGUIRenderer(const DefaultGUIRenderer &);
        DefaultGUIRenderer & operator=(const DefaultGUIRenderer &);
    };
}

#endif
