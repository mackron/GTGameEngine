// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Renderer_DefaultGUIRenderer
#define GT_Renderer_DefaultGUIRenderer

#include "Renderer.hpp"
#include <GTEngine/GUI/Rendering/GUIRenderer.hpp>

namespace GT
{
    /// The default renderer for GUI's. This calls functions directly from the main Renderer class.
    class DefaultGUIRenderer : public GUIRenderer
    {
    public:

        /// Constructor.
        DefaultGUIRenderer();

        /// Destructor.
        virtual ~DefaultGUIRenderer();


        /// Renderer::Begin()
        void Begin(const GUIServer &guiServer);

        /// Renderer::End()
        void End();

        /// Renderer::BeginElementOnDrawEvent()
        void BeginElementOnDrawEvent(GUIElement &element);

        /// GTUI::Renderer::EndElementOnDrawEvent()
        void EndElementOnDrawEvent(GUIElement &element);

        /// Renderer::SetScissor()
        void SetScissor(int x, int y, unsigned int width, unsigned int height);

        /// Renderer::SetOffset()
        virtual void SetOffset(float offsetX, float offsetY);

        /// Renderer::SetTexture()
        virtual void SetTexture(GUIImageHandle image);

        /// Renderer::EnableBlending()
        virtual void EnableBlending();

        /// Renderer::DisableBlending()
        virtual void DisableBlending();

        /// Renderer::Draw()
        void Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount);


    private:

        /// Retrieves a texture for the given image.
        //Texture2D* AcquireTexture2DFromImage(const Image* image);

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
