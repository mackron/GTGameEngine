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

        auto pAUXData = context.GetSurfaceAuxData<EditorGUISurfaceAUXData>(hSurface);
        if (pAUXData != nullptr)
        {
            HDC hDC = BeginPaint(reinterpret_cast<HWND>(pAUXData->hWindow), &m_ps);
            if (hDC != NULL)
            {
                // The offscreen buffer may need to be refresehd.
                RECT windowRect;
                GetClientRect(reinterpret_cast<HWND>(pAUXData->hWindow), &windowRect);

                int windowWidth  = windowRect.right  - windowRect.left;
                int windowHeight = windowRect.bottom - windowRect.top;
                if (pAUXData->hMemDC == NULL || (windowWidth != pAUXData->memBitmapWidth || windowHeight != pAUXData->memBitmapHeight))
                {
                    if (pAUXData->hMemDC)
                    {
                        DeleteObject(reinterpret_cast<HANDLE>(pAUXData->hMemBitmap));
                        DeleteDC(reinterpret_cast<HDC>(pAUXData->hMemDC));
                    }

                    pAUXData->hMemDC          = reinterpret_cast<size_t>(CreateCompatibleDC(hDC));
                    pAUXData->hMemBitmap      = reinterpret_cast<size_t>(CreateCompatibleBitmap(hDC, windowWidth, windowHeight));
                    pAUXData->memBitmapWidth  = windowWidth;
                    pAUXData->memBitmapHeight = windowHeight;
                }

                SelectObject(reinterpret_cast<HDC>(pAUXData->hMemDC), reinterpret_cast<HANDLE>(pAUXData->hMemBitmap));

                // Set the current window.
                m_hCurrentWindow         = reinterpret_cast<HWND>(pAUXData->hWindow);
                m_hDC                    = reinterpret_cast<HDC>(pAUXData->hMemDC);
                m_pCurrentSurfaceAUXData = pAUXData;


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

        // Reset the current window.
        m_hCurrentWindow = NULL;
        m_hDC = NULL;
        m_pCurrentSurfaceAUXData = nullptr;


        // End painting.
        EndPaint(m_hCurrentWindow, &m_ps);
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

                int bufferSize = MultiByteToWideChar(CP_UTF8, 0, textRunDesc.text.c_str(), -1, nullptr, 0);
                if (bufferSize > 0)
                {
                    if (bufferSize > 64)
                    {
                        wchar_t* buffer = reinterpret_cast<wchar_t*>(malloc(sizeof(wchar_t) * bufferSize));
                        if (buffer != nullptr)
                        {
                            MultiByteToWideChar(CP_UTF8, 0, textRunDesc.text.c_str(), -1, buffer, sizeof(wchar_t)*bufferSize);

                            TextOutW(m_hDC, textRunDesc.xPos, textRunDesc.yPos, buffer, bufferSize - 1);
                            free(buffer);
                        }
                    }
                    else
                    {
                        wchar_t buffer[64];
                        bufferSize = MultiByteToWideChar(CP_UTF8, 0, textRunDesc.text.c_str(), -1, buffer, 64);
                        if (bufferSize > 0)
                        {
                            TextOutW(m_hDC, textRunDesc.xPos, textRunDesc.yPos, buffer, bufferSize - 1);
                        }
                    }
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
}

#endif