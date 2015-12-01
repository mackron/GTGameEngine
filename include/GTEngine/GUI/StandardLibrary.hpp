// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIStandardLibrary
#define GT_GUIStandardLibrary

#include "Server.hpp"

namespace GTGUI
{
    /// Static class for working with the standard library.
    class GUIStandardLibrary
    {
    public:

        /// Loads the entire standard library into the server.
        ///
        /// @param server [in] A reference to the server to load the standard library into.
        ///
        /// @return True if the standard library is loaded successfully.
        static bool Load(GUIServer &server);



    private:

        /// Loads the miscellaneous styles and scripts, such as the default background colours.
        static bool LoadMisc(GUIServer &server);

        /// Loads the button control.
        static bool LoadButton(GUIServer &server);

        /// Loads the text box control.
        static bool LoadTextBox(GUIServer &server);

        /// Loads the check box control.
        static bool LoadCheckBox(GUIServer &server);

        /// Loads the labeled spinner control.
        static bool LoadLabeledSpinner(GUIServer &server);

        /// Loads the tree-view control.
        static bool LoadTreeView(GUIServer &server);

        /// Loads the scrollbar controls.
        static bool LoadScrollbars(GUIServer &server);

        /// Loads the tooltips controls.
        static bool LoadTooltips(GUIServer &server);


        /// Loads and registers the standard images onto the server.
        static bool LoadImages(GUIServer &server);
    };
}

#endif
