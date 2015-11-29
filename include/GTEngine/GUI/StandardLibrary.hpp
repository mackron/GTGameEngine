// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_StandardLibrary
#define GT_GUI_StandardLibrary

#include "Server.hpp"

namespace GTGUI
{
    /// Static class for working with the standard library.
    class StandardLibrary
    {
    public:

        /// Loads the entire standard library into the server.
        ///
        /// @param server [in] A reference to the server to load the standard library into.
        ///
        /// @return True if the standard library is loaded successfully.
        static bool Load(Server &server);



    private:

        /// Loads the miscellaneous styles and scripts, such as the default background colours.
        static bool LoadMisc(Server &server);

        /// Loads the button control.
        static bool LoadButton(Server &server);

        /// Loads the text box control.
        static bool LoadTextBox(Server &server);

        /// Loads the check box control.
        static bool LoadCheckBox(Server &server);

        /// Loads the labeled spinner control.
        static bool LoadLabeledSpinner(Server &server);

        /// Loads the tree-view control.
        static bool LoadTreeView(Server &server);

        /// Loads the scrollbar controls.
        static bool LoadScrollbars(Server &server);

        /// Loads the tooltips controls.
        static bool LoadTooltips(Server &server);


        /// Loads and registers the standard images onto the server.
        static bool LoadImages(Server &server);
    };
}

#endif
