// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTLib/Config.hpp>
#include <GTGameEngine/Editor/EditorGUIEventHandler.hpp>
#include <GTGameEngine/Editor/EditorGUISurfaceAUXData.hpp>
#include <GTGameEngine/GUI/GUIContext.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

namespace GT
{
    EditorGUIEventHandler::EditorGUIEventHandler()
    {
    }

    EditorGUIEventHandler::~EditorGUIEventHandler()
    {
    }


    void EditorGUIEventHandler::OnSetMouseEventCapture(GT::GUIContext &context, GT::HGUIElement hElement)
    {
        auto pUserData = context.GetSurfaceUserData<EditorGUISurfaceAUXData>(context.GetElementSurface(hElement));
        if (pUserData != nullptr)
        {
#if defined(GT_PLATFORM_WINDOWS)
            SetCapture(reinterpret_cast<HWND>(pUserData->hWindow));
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
        }
    }

    void EditorGUIEventHandler::OnReleaseMouseEventCapture(GT::GUIContext &context, GT::HGUIElement hElement)
    {
        auto pUserData = context.GetSurfaceUserData<EditorGUISurfaceAUXData>(context.GetElementSurface(hElement));
        if (pUserData != nullptr)
        {
#if defined(GT_PLATFORM_WINDOWS)
            ReleaseCapture();
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
        }
    }


    void EditorGUIEventHandler::OnSurfaceNeedsRepaint(GT::GUIContext &context, GT::HGUISurface hSurface, const GTLib::Rect<int> &rect)
    {
        auto pUserData = context.GetSurfaceUserData<EditorGUISurfaceAUXData>(hSurface);
        if (pUserData != nullptr)
        {
#if defined(GT_PLATFORM_WINDOWS)
            RECT invalidatedRect;
            invalidatedRect.left   = rect.left;
            invalidatedRect.top    = rect.top;
            invalidatedRect.right  = rect.right;
            invalidatedRect.bottom = rect.bottom;
            InvalidateRect(reinterpret_cast<HWND>(pUserData->hWindow), &invalidatedRect, FALSE);
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
        }
    }
}
