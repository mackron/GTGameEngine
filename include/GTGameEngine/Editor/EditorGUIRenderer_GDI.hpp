// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorGUIRenderer_GDI
#define GT_EditorGUIRenderer_GDI

#include "../Config.hpp"

#if defined(GT_BUILD_EDITOR) && defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GTLib/Map.hpp>
#include <GTGameEngine/GUI/GUIRenderer.hpp>
#include <GTGameEngine/GUI/GUIResourceManager.hpp>
#include <GTGameEngine/Editor/EditorGUISurfaceAUXData.hpp>
#include <GTGameEngine/Assets/AssetLibrary.hpp>

namespace GT
{
    /// GDI implementation of the GUI renderer for the editor.
    ///
    /// It is assumed that the GUI context's font manager is the default GDI font manager. This means text will be drawn directly
    /// using the GDI API.
    ///
    /// It is also assumed that all painting commands will be executed in response to the WM_PAINT message. BeginPaintSurface()
    /// will call Win32's BeginPaint() function and EndPaintSurface() will call Win32's EndPaint() function.
    class EditorGUIRenderer_GDI : public GUIRenderer, public GUIResourceManager
    {
    public:

        /// Constructor.
        EditorGUIRenderer_GDI(AssetLibrary &assetLibrary);

        /// Destructor.
        ~EditorGUIRenderer_GDI();


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



        /////////////////////////////////////////////////
        // GUIResourceManager

        /// @copydoc GUIResourceManager::LoadImage()
        HGUIImage LoadImage(const char* filePath);

        /// @copydoc GUIResourceManager::LoadImage()
        void UnloadImage(HGUIImage hImage);

        /// @copydoc GUIResourceManager::LoadImage()
        bool GetImageSize(HGUIImage hImage, unsigned int &widthOut, unsigned int &heightOut) const;

        /// @copydoc GUIResourceManager::LoadImage()
        GUIImageFormat GetImageFormat(HGUIImage hImage) const;

        /// @copydoc GUIResourceManager::LoadImage()
        const void* GetImageData(HGUIImage hImage) const;



    private:

        /// A reference to the asset library that was passed to the constructor. This is used to load image resources.
        AssetLibrary &m_assetLibrary;


        /// The PAINTSTRUCT that was created from Win32's BeginPaint() function which is called from BeginPaintSurface().
        PAINTSTRUCT m_ps;

        /// The surface that is currently being painted.
        HWND m_hCurrentWindow;

        /// The DC to draw to.
        HDC m_hDC;

        /// A pointer to the AUX data for the current surface.
        EditorGUISurfaceAUXData* m_pCurrentSurfaceAUXData;


        /// The device context for drawing alpha blended bitmaps. When drawing a bitmap, we do so with the AlphaBlend() API which requires
        /// us to make the source bitmap current on a separate DC - which is this one in our case.
        HDC m_hAlphaBlendDC;

        /// The HGUIImage -> HBITMAP mappings.
        GTLib::Map<HGUIImage, HBITMAP> m_bitmaps;


        /// The bitmap to use with DrawRawImage().
        HBITMAP m_hRawBitmap;

        /// A pointer to the buffer containing the data of the raw bitmap.
        void* m_pRawBitmapData;

        /// The width of the raw bitmap.
        unsigned int m_rawBitmapWidth;

        /// The height of the raw bitmap.
        unsigned int m_rawBitmapHeight;
    };
}

#endif

#endif
