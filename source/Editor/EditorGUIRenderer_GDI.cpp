// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/EditorGUIRenderer_GDI.hpp>

#if defined(GT_BUILD_EDITOR) && defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/GUI/GUIContext.hpp>
#include <GTGameEngine/GUI/GUIFontManager_GDI.hpp>

namespace GT
{
    EditorGUIRenderer_GDI::EditorGUIRenderer_GDI()
        : GUIRenderer(),
          m_ps(),
          m_hCurrentWindow(0)
    {
        ZeroMemory(&m_ps, sizeof(m_ps));
    }

    EditorGUIRenderer_GDI::~EditorGUIRenderer_GDI()
    {
    }


    void EditorGUIRenderer_GDI::BeginPaintSurface(GUIContext &context, HGUISurface hSurface, void* pInputData)
    {
        (void)pInputData;

        auto pUserData = context.GetSurfaceUserData<EditorGUISurfaceAUXData>(hSurface);
        if (pUserData != nullptr)
        {
            HDC hDC = BeginPaint(reinterpret_cast<HWND>(pUserData->hWindow), &m_ps);
            if (hDC != NULL)
            {
                // The offscreen buffer may need to be refresehd.
                RECT windowRect;
                GetClientRect(reinterpret_cast<HWND>(pUserData->hWindow), &windowRect);

                int windowWidth  = windowRect.right  - windowRect.left;
                int windowHeight = windowRect.bottom - windowRect.top;
                if (pUserData->hMemDC == NULL || (windowWidth != pUserData->memBitmapWidth || windowHeight != pUserData->memBitmapHeight))
                {
                    if (pUserData->hMemDC)
                    {
                        DeleteObject(reinterpret_cast<HANDLE>(pUserData->hMemBitmap));
                        DeleteDC(reinterpret_cast<HDC>(pUserData->hMemDC));
                    }

                    pUserData->hMemDC          = reinterpret_cast<size_t>(CreateCompatibleDC(hDC));
                    pUserData->hMemBitmap      = reinterpret_cast<size_t>(CreateCompatibleBitmap(hDC, windowWidth, windowHeight));
                    pUserData->memBitmapWidth  = windowWidth;
                    pUserData->memBitmapHeight = windowHeight;
                }

                SelectObject(reinterpret_cast<HDC>(pUserData->hMemDC), reinterpret_cast<HANDLE>(pUserData->hMemBitmap));

                // Set the current window.
                m_hCurrentWindow         = reinterpret_cast<HWND>(pUserData->hWindow);
                m_hDC                    = reinterpret_cast<HDC>(pUserData->hMemDC);
                m_pCurrentSurfaceAUXData = pUserData;


                // No pen by default.
                SelectObject(m_hDC, GetStockObject(NULL_PEN));

                // The normal brush by default.
                SelectObject(m_hDC, GetStockObject(DC_BRUSH));

                // Don't draw the default text background.
                SetBkMode(m_hDC, TRANSPARENT);
                //SetBkColor(m_hDC, RGB(255, 0, 0));
                //SetTextAlign(m_hDC, TA_BASELINE);
            }
            else
            {
                ZeroMemory(&m_ps, sizeof(m_ps));
            }
        }
    }

    void EditorGUIRenderer_GDI::EndPaintSurface(GUIContext &context)
    {
        (void)context;


        if (m_pCurrentSurfaceAUXData != nullptr)
        {
            // Present the back buffer.
            int xPos   = m_ps.rcPaint.left;
            int yPos   = m_ps.rcPaint.top;
            int width  = m_ps.rcPaint.right - m_ps.rcPaint.left;
            int height = m_ps.rcPaint.bottom - m_ps.rcPaint.top;
            BitBlt(GetDC(reinterpret_cast<HWND>(m_pCurrentSurfaceAUXData->hWindow)), xPos, yPos, width, height, reinterpret_cast<HDC>(m_pCurrentSurfaceAUXData->hMemDC), xPos, yPos, SRCCOPY);

            // Restore the old bitmap.
            SelectObject(reinterpret_cast<HDC>(m_pCurrentSurfaceAUXData->hMemDC), reinterpret_cast<HANDLE>(m_pCurrentSurfaceAUXData->hMemBitmap));
        }


        // The clipping rectangle needs to be reset before so that future WM_PAINT messages are sent the correct drawing rectangle.
        SelectClipRgn(m_ps.hdc, NULL);
        SelectClipRgn(m_hDC, NULL);

        // End painting.
        EndPaint(m_hCurrentWindow, &m_ps);


        // Reset the current window.
        m_hCurrentWindow = NULL;
        m_hDC = NULL;
        m_pCurrentSurfaceAUXData = nullptr;
    }


    void EditorGUIRenderer_GDI::Clear(GUIContext &context)
    {
        RECT windowRect;
        GetWindowRect(m_hCurrentWindow, &windowRect);

        this->Clear(context, GTLib::Rect<int>(windowRect.left, windowRect.top, windowRect.right, windowRect.bottom));
    }

    void EditorGUIRenderer_GDI::Clear(GUIContext &context, const GTLib::Rect<int> &rect)
    {
        this->SetClippingRect(context, rect);

        // There is no need to clear to black with GDI.
        //this->DrawRectangle(context, rect, GTLib::Colour(0, 0, 0, 1));
    }


    void EditorGUIRenderer_GDI::DrawRectangle(GUIContext &context, GTLib::Rect<int> rect, GTLib::Colour color)
    {
        (void)context;

        if (color.a > 0.0f)
        {
            if (color.a == 1.0f)
            {
                // Opaque.

                // Set the color.
                SetDCBrushColor(m_hDC, RGB(color.r*255, color.g*255, color.b*255));

                // Now draw the rectangle. The documentation for this says that the width and height is 1 pixel less when the pen is null. Therefore we will
                // increase the width and height by 1 since we have got the pen set to null.
                Rectangle(m_hDC, rect.left, rect.top, rect.right + 1, rect.bottom + 1);
            }
            else
            {
                // Transparent.
            }
        }
    }

    void EditorGUIRenderer_GDI::SetClippingRect(GUIContext &context, GTLib::Rect<int> rect)
    {
        (void)context;

        SelectClipRgn(m_hDC, NULL);
        IntersectClipRect(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
    }

    bool EditorGUIRenderer_GDI::CanDrawText(GUIContext &context, HGUIFont hFont)
    {
        (void)context;
        (void)hFont;

        return true;
    }

    void EditorGUIRenderer_GDI::DrawText(GUIContext &context, const GUITextRunDesc &textRunDesc)
    {
        // TODO: Consider making this so that the previous font is not reselected. Can do this globally in EndPaintSurface()

        auto pFontManagerGDI = reinterpret_cast<GUIFontManager_GDI*>(context.GetFontManager());
        if (pFontManagerGDI != nullptr)
        {
            HFONT hFontWin32 = pFontManagerGDI->GetWin32FontHandle(textRunDesc.hFont);
            if (hFontWin32 != 0)
            {
                HGDIOBJ  hPrevFont      = SelectObject(m_hDC, hFontWin32);
                COLORREF hPrevTextColor = SetTextColor(m_hDC, RGB(textRunDesc.color.r*255, textRunDesc.color.g*255, textRunDesc.color.b*255));

                wchar_t textW[GT_MAX_TEXT_RUN_SIZE_IN_BYTES];
                int bufferSize = MultiByteToWideChar(CP_UTF8, 0, textRunDesc.text, -1, textW, GT_MAX_TEXT_RUN_SIZE_IN_BYTES - 1);
                if (bufferSize > 0)
                {
                    TextOutW(m_hDC, textRunDesc.xPos, textRunDesc.yPos, textW, bufferSize - 1);
                }


                SelectObject(m_hDC, hPrevFont);
                SetTextColor(m_hDC, hPrevTextColor);
            }
        }
    }


    void EditorGUIRenderer_GDI::DrawRawImage(GUIContext &context, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent)
    {
        (void)context;
        (void)xPos;
        (void)yPos;
        (void)width;
        (void)height;
        (void)pImageData;
        (void)isTransparent;

        // TODO: Implement.
    }


    void EditorGUIRenderer_GDI::InitializeImage(GT::GUIContext &context, HGUIImage hImage, unsigned int width, unsigned int height, GUIImageFormat format, const void* pData)
    {
        // TODO: Implement.
        (void)context;
        (void)hImage;
        (void)width;
        (void)height;
        (void)format;
        (void)pData;
    }

    void EditorGUIRenderer_GDI::UninitializeImage(GT::GUIContext &context, HGUIImage hImage)
    {
        // TODO: Implement.
        (void)context;
        (void)hImage;
    }
}

#endif
