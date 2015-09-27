// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/EditorGUIRenderer_GDI.hpp>

#if defined(GT_BUILD_EDITOR) && defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/GUI/GUIContext.hpp>
#include <GTGameEngine/GUI/GUIFontManager_GDI.hpp>
#include <GTGameEngine/Assets/ImageAsset.hpp>

#if defined(GT_COMPILER_VC)
#pragma comment(lib, "msimg32")
#endif

namespace GT
{
    EditorGUIRenderer_GDI::EditorGUIRenderer_GDI(AssetLibrary &assetLibrary)
        : GUIRenderer(),
          m_assetLibrary(assetLibrary),
          m_ps(),
          m_hCurrentWindow(0),
          m_hDC(0),
          m_pCurrentSurfaceAUXData(nullptr),
          m_hAlphaBlendDC(0),
          m_hRawBitmap(0), m_pRawBitmapData(nullptr), m_rawBitmapWidth(0), m_rawBitmapHeight(0)
    {
        ZeroMemory(&m_ps, sizeof(m_ps));
        m_hAlphaBlendDC = CreateCompatibleDC(GetDC(GetDesktopWindow()));
    }

    EditorGUIRenderer_GDI::~EditorGUIRenderer_GDI()
    {
        DeleteDC(m_hAlphaBlendDC);

        if (m_hRawBitmap != 0) {
            DeleteObject(m_hRawBitmap);
        }
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
        //this->DrawRectangle(context, rect, GT::Color(0, 0, 0, 1));
    }


    void EditorGUIRenderer_GDI::DrawRectangle(GUIContext &context, GTLib::Rect<int> rect, GT::Color color)
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


        // If the width or height of the image data is larger than that of the existing bitmap, the bitmap will need to be recreated.
        if (width > m_rawBitmapWidth || height > m_rawBitmapHeight)
        {
            // The old HBITMAP needs to be deleted.
            if (m_hRawBitmap != 0) {
                DeleteObject(m_hRawBitmap);
            }

            BITMAPINFO bmi;
            ZeroMemory(&bmi, sizeof(bmi));
            bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
            bmi.bmiHeader.biWidth       = width;
            bmi.bmiHeader.biHeight      = height;
            bmi.bmiHeader.biPlanes      = 1;
            bmi.bmiHeader.biBitCount    = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            m_hRawBitmap = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, &m_pRawBitmapData, NULL, 0);
            if (m_hRawBitmap != 0) {
                m_rawBitmapWidth  = width;
                m_rawBitmapHeight = height;
            } else {
                m_pRawBitmapData  = nullptr;
                m_rawBitmapWidth  = 0;
                m_rawBitmapHeight = 0;
            }
        }


        
        if (m_hRawBitmap != 0)
        {
            // Image data is assumed to be RGBA.
            for (unsigned int iRow = 0; iRow < height; ++iRow)
            {
                const unsigned int iRowSrc = height - (iRow + 1);
                const unsigned int iRowDst = iRow;

                for (unsigned int iCol = 0; iCol < width; ++iCol)
                {
                    uint32_t  srcTexel = reinterpret_cast<const uint32_t*>(pImageData      )[(iRowSrc * width)            + iCol];
                    uint32_t &dstTexel = reinterpret_cast<      uint32_t*>(m_pRawBitmapData)[(iRowDst * m_rawBitmapWidth) + iCol];

                    uint32_t srcTexelA = (srcTexel & 0xFF000000) >> 24;
                    uint32_t srcTexelB = (srcTexel & 0x00FF0000) >> 16;
                    uint32_t srcTexelG = (srcTexel & 0x0000FF00) >> 8;
                    uint32_t srcTexelR = (srcTexel & 0x000000FF) >> 0;

                    if (isTransparent)
                    {
                        srcTexelB = srcTexelB * srcTexelA / 0xFF;
                        srcTexelG = srcTexelG * srcTexelA / 0xFF;
                        srcTexelR = srcTexelR * srcTexelA / 0xFF;
                    }

                    dstTexel = (srcTexelR << 16) | (srcTexelG << 8) | (srcTexelB << 0) | (srcTexelA << 24);
                }
            }

            GdiFlush();


            HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(m_hAlphaBlendDC, m_hRawBitmap));

            if (isTransparent) {
                BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                AlphaBlend(m_hDC, xPos, yPos, width, height, m_hAlphaBlendDC, 0, 0, width, height, blend);
            } else {
                BitBlt(m_hDC, xPos, yPos, width, height, m_hAlphaBlendDC, 0, m_rawBitmapHeight - height, SRCCOPY);
            }

            SelectObject(m_hAlphaBlendDC, hOldBitmap);
        }
    }


    void EditorGUIRenderer_GDI::InitializeImage(GT::GUIContext &context, HGUIImage hImage, unsigned int width, unsigned int height, GUIImageFormat format, const void* pData)
    {
        (void)context;


        if (format == GUIImageFormat::None) {
            return;
        }


        void* pBitmapWin32Data = nullptr;

        WORD bitcount = 32;
        if (format == GUIImageFormat::A8) {
            bitcount = 8;
        }

        BITMAPINFO bmi;
        ZeroMemory(&bmi, sizeof(bmi));
        bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
        bmi.bmiHeader.biWidth       = width;
        bmi.bmiHeader.biHeight      = height;
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = bitcount;
        bmi.bmiHeader.biCompression = BI_RGB;
        HBITMAP hBitmapWin32 = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, &pBitmapWin32Data, NULL, 0);
        if (hBitmapWin32 != 0)
        {
            switch (format)
            {
            case GUIImageFormat::RGBA8:
                {
                    for (unsigned int iRow = 0; iRow < height; ++iRow)
                    {
                        const unsigned int iRowSrc = height - (iRow + 1);
                        const unsigned int iRowDst = iRow;

                        for (unsigned int iCol = 0; iCol < width; ++iCol)
                        {
                            uint32_t  srcTexel = reinterpret_cast<const uint32_t*>(pData           )[(iRowSrc * width) + iCol];
                            uint32_t &dstTexel = reinterpret_cast<      uint32_t*>(pBitmapWin32Data)[(iRowDst * width) + iCol];

                            uint32_t srcTexelA = (srcTexel & 0xFF000000) >> 24;
                            uint32_t srcTexelB = (srcTexel & 0x00FF0000) >> 16;
                            uint32_t srcTexelG = (srcTexel & 0x0000FF00) >> 8;
                            uint32_t srcTexelR = (srcTexel & 0x000000FF) >> 0;

                            srcTexelB = srcTexelB * srcTexelA / 0xFF;
                            srcTexelG = srcTexelG * srcTexelA / 0xFF;
                            srcTexelR = srcTexelR * srcTexelA / 0xFF;

                            dstTexel = (srcTexelR << 16) | (srcTexelG << 8) | (srcTexelB << 0) | (srcTexelA << 24);
                        }
                    }

                    break;
                }

            case GUIImageFormat::A8:
                {
                    for (unsigned int iRow = 0; iRow < height; ++iRow)
                    {
                        for (unsigned int iCol = 0; iCol < width; ++iCol)
                        {
                            uint8_t  srcTexel = reinterpret_cast<const uint8_t*>(pData           )[(iRow * width) + iCol];
                            uint8_t &dstTexel = reinterpret_cast<      uint8_t*>(pBitmapWin32Data)[(iRow * width) + iCol];

                            dstTexel = srcTexel;
                        }
                    }

                    break;
                }

            case GUIImageFormat::None:
            default:
                {
                    break;
                }
            }

            m_bitmaps.Add(hImage, hBitmapWin32);

            GdiFlush();
        }
    }

    void EditorGUIRenderer_GDI::UninitializeImage(GT::GUIContext &context, HGUIImage hImage)
    {
        (void)context;

        auto iBitmapWin32 = m_bitmaps.Find(hImage);
        if (iBitmapWin32 != nullptr)
        {
            DeleteObject(iBitmapWin32->value);
            m_bitmaps.RemoveByIndex(iBitmapWin32->index);
        }
    }

    void EditorGUIRenderer_GDI::DrawTexturedRectangle(GT::GUIContext &context, GTLib::Rect<int> rect, HGUIImage hImage, GT::Color colour, unsigned int subImageOffsetX, unsigned int subImageOffsetY, unsigned int subImageWidth, unsigned int subImageHeight)
    {
        // NOTE: Colour is not implemented yet.

        (void)context;
        (void)colour;

        auto iBitmapWin32 = m_bitmaps.Find(hImage);
        if (iBitmapWin32 != nullptr)
        {
            HBITMAP hNewBitmap = iBitmapWin32->value;
            HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(m_hAlphaBlendDC, hNewBitmap));

            BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            AlphaBlend(m_hDC, rect.left, rect.top, rect.GetWidth(), rect.GetHeight(), m_hAlphaBlendDC, int(subImageOffsetX), int(subImageOffsetY), int(subImageWidth), int(subImageHeight), blend);

            SelectObject(m_hAlphaBlendDC, hOldBitmap);
        }
    }





    /////////////////////////////////////////////////
    // GUIResourceManager

    HGUIImage EditorGUIRenderer_GDI::LoadImage(const char* filePath)
    {
        Asset* pImageAsset = m_assetLibrary.Load(filePath);
        if (pImageAsset != nullptr) {
            if (pImageAsset->GetClass() == AssetClass_Image) {
                return reinterpret_cast<HGUIImage>(pImageAsset);
            }
        }

        return NULL;
    }

    void EditorGUIRenderer_GDI::UnloadImage(HGUIImage hImage)
    {
        m_assetLibrary.Unload(reinterpret_cast<Asset*>(hImage));
    }

    bool EditorGUIRenderer_GDI::GetImageSize(HGUIImage hImage, unsigned int &widthOut, unsigned int &heightOut) const
    {
        ImageAsset* pImageAsset = reinterpret_cast<ImageAsset*>(hImage);
        if (pImageAsset != nullptr) {
            widthOut  = pImageAsset->GetImageWidth();
            heightOut = pImageAsset->GetImageHeight();

            return true;
        }

        return false;
    }

    GUIImageFormat EditorGUIRenderer_GDI::GetImageFormat(HGUIImage hImage) const
    {
        ImageAsset* pImageAsset = reinterpret_cast<ImageAsset*>(hImage);
        if (pImageAsset != nullptr) {
            switch (pImageAsset->GetImageFormat())
            {
            case TextureFormat_RGBA8: return GUIImageFormat::RGBA8;
            case TextureFormat_R8:    return GUIImageFormat::A8;

            default: break;
            }
        }

        return GUIImageFormat::Unknown;
    }

    const void* EditorGUIRenderer_GDI::GetImageData(HGUIImage hImage) const
    {
        ImageAsset* pImageAsset = reinterpret_cast<ImageAsset*>(hImage);
        if (pImageAsset != nullptr) {
            return pImageAsset->GetImageData();
        }

        return nullptr;
    }
}

#endif
