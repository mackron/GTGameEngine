// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Renderer_DefaultGUIRenderer_hpp_
#define __GTEngine_Renderer_DefaultGUIRenderer_hpp_

#include "Renderer2.hpp"
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

        /// GTGUI::Renderer::SetScissor()
        void SetScissor(int x, int y, unsigned int width, unsigned int height);

        /// GTGUI::Renderer::Draw()
        void Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, float offsetX, float offsetY, const GTImage::Image* image, bool enableOpacity);


    private:

        /// Retrieves a texture for the given image.
        Texture2D* AcquireTexture2DFromImage(const GTImage::Image* image);


    private:

        /// The main shader.
        Shader* shader;

        /// The viewport width.
        unsigned int viewportWidth;

        /// The viewport height.
        unsigned int viewportHeight;

        /// The projection matrix.
        glm::mat4 projection;

        /// The list of textures currently loaded by the GUI.
        GTCore::Map<const GTImage::Image*, Texture2D*> textures;

        /// The main vertex array.
        VertexArray* vertexArray;
    };
}

#endif