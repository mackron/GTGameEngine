// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GraphicsWorldGUIRenderer
#define GT_GraphicsWorldGUIRenderer

#include "../GUI/GUIRenderer.hpp"
#include "GraphicsWorld.hpp"
#include <GTLib/Map.hpp>

namespace GT
{
    class GraphicsWorldGUIRenderer : public GUIRenderer
    {
    public:

        /// Constructor.
        GraphicsWorldGUIRenderer();


        /// Sets the graphics world that should receive the painting commands.
        ///
        /// @remarks
        ///     This should be set once, at the beginning before any rendering commands are submitted.
        void SetGraphicsWorld(GraphicsWorld* pGraphicsWorld);



        /// @copydoc GUIRenderer::BeginPaintSurface()
        void BeginPaintSurface(GUIContext &context, HGUISurface hSurface, void* pInputData);

        /// @copydoc GUIRenderer::EndPaintSurface()
        void EndPaintSurface(GUIContext &context);


        /// @copydoc GUIRenderer::Clear()
        void Clear(GUIContext &context);

        /// @copydoc GUIRenderer::Clear(GUIContext &, const GTLib::Rect<int> &)
        void Clear(GUIContext &context, const GTLib::Rect<int> &rect);

        /// @copydoc GUIRenderer::DrawRectangle()
        void DrawRectangle(GUIContext &context, GTLib::Rect<int> rect, GTLib::Colour colour);

        /// @copydoc GUIRenderer::SetClippingRect()
        void SetClippingRect(GUIContext &context, GTLib::Rect<int> rect);


        /// @copydoc GUIRenderer::CanDrawText()
        bool CanDrawText(GUIContext &context, HGUIFont hFont);

        /// @copydoc GUIRenderer::DrawText()
        void DrawText(GUIContext &context, const GUITextRunDesc &textRunDesc);


        /// @copydoc GUIRenderer::DrawRawImage()
        void DrawRawImage(GT::GUIContext &context, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent);


        /// @copydoc GUIRenderer::InitializeImage()
        void InitializeImage(GT::GUIContext &context, HGUIImage hImage, unsigned int width, unsigned int height, GUIImageFormat format, const void* pData);

        /// @copydoc GUIRenderer::UninitializeImage()
        void UninitializeImage(GT::GUIContext &context, HGUIImage hImage);

        /// @copydoc GUIRenderer::DrawTexturedRectangle()
        void DrawTexturedRectangle(GT::GUIContext &context, GTLib::Rect<int> rect, HGUIImage hImage, GTLib::Colour colour, unsigned int subImageOffsetX, unsigned int subImageOffsetY, unsigned int subImageWidth, unsigned int subImageHeight);



    private:

        /// Helper function for retrieving the texture resource from the given image.
        HGraphicsResource GetTextureResource(HGUIImage hImage);

        /// Converts a GUI image for enum value to the engine's graphics world equivalent.
        TextureFormat GUIImageFormatToGraphicsTextureFormat(GUIImageFormat imageFormat);


    private:

        /// A pointer to the graphcis world to route painting commands to.
        GraphicsWorld* m_pGraphicsWorld;
    };
}

#endif
