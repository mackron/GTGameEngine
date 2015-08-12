// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsWorldGUIRenderer_hpp_
#define __GT_GraphicsWorldGUIRenderer_hpp_

#include "../GUI/GUIRenderer.hpp"

namespace GT
{
    class GraphicsWorld;

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



    private:

        /// A pointer to the graphcis world to route painting commands to.
        GraphicsWorld* m_pGraphicsWorld;
    };
}

#endif
