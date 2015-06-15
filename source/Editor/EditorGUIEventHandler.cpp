// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

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


    void EditorGUIEventHandler::OnSurfaceNeedsRepaint(GT::GUIContext &context, GT::HGUISurface hSurface, const GTLib::Rect<int> &rect)
    {
        auto pAUXData = context.GetSurfaceAuxData<EditorGUISurfaceAUXData>(hSurface);
        if (pAUXData != nullptr)
        {
#if defined(GT_PLATFORM_WINDOWS)
            RECT invalidatedRect;
            invalidatedRect.left   = rect.left;
            invalidatedRect.top    = rect.top;
            invalidatedRect.right  = rect.right;
            invalidatedRect.bottom = rect.bottom;
            InvalidateRect(reinterpret_cast<HWND>(pAUXData->hWindow), &invalidatedRect, true);
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
        }
    }
}
