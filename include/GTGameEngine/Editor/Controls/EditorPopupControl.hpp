// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorPopupControl_hpp_
#define __GT_EditorPopupControl_hpp_

#include "EditorControl.hpp"
#include "../../WindowManager.hpp"

namespace GT
{
    /// Base class for popup controls.
    class EditorPopupControl : public EditorControl
    {
    public:

        /// Constructor.
        ///
        /// @param windowManager [in] A reference to window manager that will create and delete the popup window.
        /// @param hParentWindow [in] A handle to the popup window's parent.
        ///
        /// @remarks
        ///     This will create the popup window, but does not display it. Use GetWindow() to determine if the window was created successfully.
        ///     @par
        ///     This will also create a GUI surface that should be drawn to the window.
        EditorPopupControl(Editor &editor, HWindow hParentWindow);

        /// Destructor.
        virtual ~EditorPopupControl();


        /// Retrieves a reference to the popup window.
        HWindow GetWindow() const;


    private:

        /// A handle to the window.
        HWindow m_hWindow;
    };
}

#endif // !__GT_EditorPopupControl_hpp_
