// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/EditorGUIRenderer_GDI.hpp>

#if defined(GT_BUILD_EDITOR) && defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/GUI/GUIContext.hpp>
#include <GTGameEngine/GUI/GUIFontManager_GDI.hpp>
#include <GTGameEngine/Editor/EditorGUISurfaceAUXData.hpp>

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
                // No pen by default.
                SelectObject(m_ps.hdc, GetStockObject(NULL_PEN));

                // The normal brush by default.
                SelectObject(m_ps.hdc, GetStockObject(DC_BRUSH));

                // Don't draw the default text background.
                SetBkMode(m_ps.hdc, TRANSPARENT);
                //SetBkColor(m_ps.hdc, RGB(255, 0, 0));


                // Set the current window.
                m_hCurrentWindow = reinterpret_cast<HWND>(pAUXData->hWindow);
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

        // The clipping rectangle needs to be reset before so that future WM_PAINT messages are sent the correct drawing rectangle.
        SelectClipRgn(m_ps.hdc, NULL);

        // Reset the current window.
        m_hCurrentWindow = NULL;


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
        this->DrawRectangle(context, rect, GTLib::Colour(0, 0, 0, 1));
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
                SetDCBrushColor(m_ps.hdc, RGB(color.r*255, color.g*255, color.b*255));

                // Now draw the rectangle. The documentation for this says that the width and height is 1 pixel less when the pen is null. Therefore we will
                // increase the width and height by 1 since we have got the pen set to null.
                Rectangle(m_ps.hdc, rect.left, rect.top, rect.right + 1, rect.bottom + 1);
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

        SelectClipRgn(m_ps.hdc, NULL);
        IntersectClipRect(m_ps.hdc, rect.left, rect.top, rect.right, rect.bottom);
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
                HGDIOBJ  hPrevFont      = SelectObject(m_ps.hdc, hFontWin32);
                COLORREF hPrevTextColor = SetTextColor(m_ps.hdc, RGB(textRunDesc.color.r*255, textRunDesc.color.g*255, textRunDesc.color.b*255));

                TextOutA(m_ps.hdc, textRunDesc.xPos, textRunDesc.yPos, textRunDesc.text.c_str(), static_cast<int>(textRunDesc.text.GetCharacterCount()));

                SelectObject(m_ps.hdc, hPrevFont);
                SetTextColor(m_ps.hdc, hPrevTextColor);
            }
        }
    }
}

#endif