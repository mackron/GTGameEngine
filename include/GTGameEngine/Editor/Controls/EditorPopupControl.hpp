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


        /// Shows the window.
        void Show();

        /// Hides the window.
        void Hide();

        /// Sets the position of the popup control window relative to the parent.
        ///
        /// @param xPos [in] The position of the window on the x axis.
        /// @param yPos [in] The position of the window on the y axis.
        void SetPosition(int xPos, int yPos);

        /// Sets the size of the popup control window.
        ///
        /// @param width  [in] The new width of the control.
        /// @param height [in] The new height of the control.
        void SetSize(unsigned int width, unsigned int height);


    private:

        /// A handle to the window.
        HWindow m_hWindow;
    };
}

#endif // !__GT_EditorPopupControl_hpp_
