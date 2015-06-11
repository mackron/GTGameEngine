// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorGUIEventHandler_hpp_
#define __GT_EditorGUIEventHandler_hpp_

#include <GTGameEngine/Core/GUI/GUIEventHandler.hpp>

namespace GT
{
    /// The GUI event handler used by the editor.
    class EditorGUIEventHandler : public GUIEventHandler
    {
    public:

        /// Constructor.
        EditorGUIEventHandler();

        /// Destructor.
        ~EditorGUIEventHandler();



        /// @copydoc GUIEventHandler::OnSurfaceNeedsRepaint()
        void OnSurfaceNeedsRepaint(GT::GUIContext &context, GT::HGUISurface hSurface, const GTLib::Rect<int> &rect);
    };
}

#endif
