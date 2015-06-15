// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include "GUIRenderer_Generic.hpp"
#include <GTGameEngine/GUI/GUIContext.hpp>

namespace GT
{
    GUIRenderBuffer_Generic::GUIRenderBuffer_Generic(HGUIRenderBuffer handleIn, unsigned int widthIn, unsigned int heightIn)
        : handle(handleIn),
          width(widthIn), height(heightIn),
          data(nullptr),
          isDataMapped(false)
    {
        this->data = reinterpret_cast<uint32_t*>(malloc(widthIn*heightIn*4));
    }

    GUIRenderBuffer_Generic::~GUIRenderBuffer_Generic()
    {
        free(this->data);
    }




    GUIRenderer_Generic::GUIRenderer_Generic()
        : GUIRenderer(),
          m_renderBuffers(),
          m_currentRenderBuffer(nullptr)
    {
    }

    GUIRenderer_Generic::~GUIRenderer_Generic()
    {
        // Every render buffer needs to be deleted.
        m_renderBuffers.IterateAssociatedObjects([](GUIRenderBuffer_Generic* renderBuffer) -> bool
        {
            delete renderBuffer;
            return true;
        });
    }


#if 0
    HGUIRenderBuffer GUIRenderer_Generic::CreateRenderBuffer(unsigned int width, unsigned int height)
    {
        HGUIRenderBuffer handle = m_renderBuffers.CreateHandle();
        if (handle != 0)
        {
            m_renderBuffers.AssociateObjectWithHandle(handle, new GUIRenderBuffer_Generic(handle, width, height));
        }

        return handle;
    }

    void GUIRenderer_Generic::DeleteRenderBuffer(HGUIRenderBuffer hBuffer)
    {
        auto renderBuffer = this->GetRenderBufferObject(hBuffer);
        if (renderBuffer != nullptr)
        {
            // If this buffer is the current one, make the current render buffer null.
            if (m_currentRenderBuffer == renderBuffer)
            {
                this->SetCurrentRenderBuffer(0);
            }


            delete renderBuffer;
            m_renderBuffers.DeleteHandle(hBuffer);
        }
    }

    void GUIRenderer_Generic::GetRenderBufferSize(HGUIRenderBuffer hBuffer, unsigned int &widthOut, unsigned int &heightOut)
    {
        auto renderBuffer = this->GetRenderBufferObject(hBuffer);
        if (renderBuffer != nullptr)
        {
            widthOut  = renderBuffer->width;
            heightOut = renderBuffer->height;
        }
        else
        {
            widthOut  = 0;
            heightOut = 0;
        }
    }
#endif

#if 0
    void* GUIRenderer_Generic::MapRenderBuffer(HGUIRenderBuffer hBuffer)
    {
        auto renderBuffer = this->GetRenderBufferObject(hBuffer);
        if (renderBuffer != nullptr)
        {
            if (!renderBuffer->isDataMapped)
            {
                return renderBuffer->data;
            }
        }

        return nullptr;
    }

    bool GUIRenderer_Generic::UnmapRenderBuffer(HGUIRenderBuffer hBuffer, const void* data)
    {
        auto renderBuffer = this->GetRenderBufferObject(hBuffer);
        if (renderBuffer != nullptr)
        {
            if (renderBuffer->isDataMapped && renderBuffer->data == data)
            {
                renderBuffer->isDataMapped = false;
                return true;
            }
        }

        return false;
    }
#endif

#if 0
    void GUIRenderer_Generic::SetCurrentRenderBuffer(HGUIRenderBuffer hBuffer)
    {
        m_currentRenderBuffer = this->GetRenderBufferObject(hBuffer);
    }
#endif



    ////////////////////////////////////////////////
    // Drawing

    void GUIRenderer_Generic::BeginPaintSurface(GT::GUIContext &context, GT::HGUISurface hSurface, void* pInputData)
    {
        (void)context;
        (void)hSurface;
        (void)pInputData;
    }

    void GUIRenderer_Generic::EndPaintSurface(GT::GUIContext &context)
    {
        (void)context;
    }

    void GUIRenderer_Generic::Clear(GT::GUIContext &context)
    {
        (void)context;

        if (m_currentRenderBuffer != nullptr)
        {
            m_currentClippingRect.left   = 0;
            m_currentClippingRect.top    = 0;
            m_currentClippingRect.right  = m_currentRenderBuffer->width;
            m_currentClippingRect.bottom = m_currentRenderBuffer->height;

            memset(m_currentRenderBuffer->data, 0, m_currentRenderBuffer->width * m_currentRenderBuffer->height * 4);
        }
    }

    void GUIRenderer_Generic::Clear(GT::GUIContext &context, const GTLib::Rect<int> &rect)
    {
        (void)context;

        if (m_currentRenderBuffer != nullptr)
        {
            // TODO: Optimize this so that it's done as a single operation.

            this->SetClippingRect(context, rect);

            for (unsigned int y = m_currentClippingRect.top; y < m_currentClippingRect.bottom; ++y)
            {
                unsigned int offset = y * m_currentRenderBuffer->width;
                for (unsigned int x = m_currentClippingRect.left; x < m_currentClippingRect.right; ++x)
                {
                    m_currentRenderBuffer->data[offset + x] = 0x01;
                }
            }
        }
    }

    void GUIRenderer_Generic::DrawRectangle(GT::GUIContext &context, GTLib::Rect<int> rect, GTLib::Colour color)
    {
        (void)context;

        if (m_currentRenderBuffer != nullptr)
        {
            // NOTE: There's a lot of optimization opportunaties here. Currently, this is just a naive implementation, but we will probably try something a bit
            //       more efficient later on.


            // If the entire rectangle is completely outside the surface, just ignore it.
            if (rect.left   > static_cast<int>(m_currentRenderBuffer->width))  return;
            if (rect.top    > static_cast<int>(m_currentRenderBuffer->height)) return;
            if (rect.right  < 0) return;
            if (rect.bottom < 0) return;


            // The rectangle needs to be clamped within the surface's area.
            GTLib::Rect<unsigned int> clampedRect;
            clampedRect.left   = (rect.left   < 0)                                               ? 0                             : rect.left;
            clampedRect.top    = (rect.top    < 0)                                               ? 0                             : rect.top;
            clampedRect.right  = (rect.right  > static_cast<int>(m_currentRenderBuffer->width))  ? m_currentRenderBuffer->width  : rect.right;
            clampedRect.bottom = (rect.bottom > static_cast<int>(m_currentRenderBuffer->height)) ? m_currentRenderBuffer->height : rect.bottom;


            uint32_t rgb8 =
                (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
                (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
                (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8);


            for (unsigned int y = clampedRect.top; y < clampedRect.bottom; ++y)
            {
                unsigned int offset = y * m_currentRenderBuffer->width;
                for (unsigned int x = clampedRect.left; x < clampedRect.right; ++x)
                {
                    uint32_t dst = m_currentRenderBuffer->data[offset + x];
                    if ((dst & 0x01))    // <-- Check clipping.
                    {
                        m_currentRenderBuffer->data[offset + x] = (m_currentRenderBuffer->data[offset + x] & 0x000000FF) | rgb8;
                    }
                }
            }
        }
    }

    void GUIRenderer_Generic::SetClippingRect(GT::GUIContext &context, GTLib::Rect<int> clippingRect)
    {
        (void)context;

        if (m_currentRenderBuffer != nullptr)
        {
            auto pixelBuffer = m_currentRenderBuffer->data;
            if (pixelBuffer != nullptr)
            {
                // If the entire rectangle is completely outside the surface, just ignore it.
                if (clippingRect.left   > static_cast<int>(m_currentRenderBuffer->width))  return;
                if (clippingRect.top    > static_cast<int>(m_currentRenderBuffer->height)) return;
                if (clippingRect.right  < 0) return;
                if (clippingRect.bottom < 0) return;

                // The rectangle needs to be clamped within the surface's area.
                GTLib::Rect<unsigned int> clampedRect;
                clampedRect.left   = (clippingRect.left   < 0)                                               ? 0                             : clippingRect.left;
                clampedRect.top    = (clippingRect.top    < 0)                                               ? 0                             : clippingRect.top;
                clampedRect.right  = (clippingRect.right  > static_cast<int>(m_currentRenderBuffer->width))  ? m_currentRenderBuffer->width  : clippingRect.right;
                clampedRect.bottom = (clippingRect.bottom > static_cast<int>(m_currentRenderBuffer->height)) ? m_currentRenderBuffer->height : clippingRect.bottom;


                // All we need to do is clear the old clipping region.
                for (unsigned int y = m_currentClippingRect.top; y < m_currentClippingRect.bottom; ++y)
                {
    #if 0
                    unsigned int offset = y * m_currentRenderBuffer->width;
                    for (unsigned int x = m_currentClippingRect.left; x < m_currentClippingRect.right; ++x)
                    {
                        pixelBuffer[offset + x] = pixelBuffer[offset + x] & 0xFFFFFFFE;
                    }
    #else
                    uint32_t* row32 = pixelBuffer + (y * m_currentRenderBuffer->width) + m_currentClippingRect.left;
                    uint64_t* row64 = reinterpret_cast<uint64_t*>(row32);

                    const unsigned int rectWidth = m_currentClippingRect.GetWidth();
                    const unsigned int rectWidthHalf = rectWidth >> 1;
                    for (unsigned int x = 0; x < rectWidthHalf; ++x)
                    {
                        row64[x] = row64[x] & 0xFFFFFFFEFFFFFFFE;
                    }

                    if (rectWidth % 2)
                    {
                        row32[rectWidth - 1] = row32[rectWidth - 1] & 0xFFFFFFFE;
                    }
    #endif
                }

                if (clampedRect.GetWidth() > 0 && clampedRect.GetHeight() > 0)
                {
                    for (unsigned int y = clampedRect.top; y < clampedRect.bottom; ++y)
                    {
    #if 0
                        unsigned int offset = y * m_currentRenderBuffer->width;
                        for (unsigned int x = clampedRect.left; x < clampedRect.right; ++x)
                        {
                            pixelBuffer[offset + x] |= 0x01;
                        }
    #else
                        uint32_t* row32 = pixelBuffer + (y * m_currentRenderBuffer->width) + clampedRect.left;
                        uint64_t* row64 = reinterpret_cast<uint64_t*>(row32);

                        const unsigned int rectWidth = clampedRect.GetWidth();
                        const unsigned int rectWidthHalf = rectWidth >> 1;
                        for (unsigned int x = 0; x < rectWidthHalf; ++x)
                        {
                            row64[x] |= 0x0100000001;
                        }

                        if (rectWidth % 2)
                        {
                            row32[rectWidth - 1] |= 0x01;
                        }
    #endif
                    }
                }
                else
                {
                    clampedRect = GTLib::Rect<unsigned int>();
                }


                // Set the clipping rectangle on the context so we can know the region that is clipped.
                m_currentClippingRect = clampedRect;
            }
        }
    }



    ////////////////////////////////////////////////
    // Private

    GUIRenderBuffer_Generic* GUIRenderer_Generic::GetRenderBufferObject(HGUIRenderBuffer hBuffer)
    {
        auto renderBuffer = m_renderBuffers.GetAssociatedObject(hBuffer);
        if (renderBuffer != nullptr)
        {
            if (hBuffer == renderBuffer->handle)
            {
                return renderBuffer;
            }
            else
            {
                // Invalid handle (uniqueness IDs do not match).
                return nullptr;
            }
        }
        else
        {
            // Invalid handle (deleted or null).
            return nullptr;
        }
    }
}