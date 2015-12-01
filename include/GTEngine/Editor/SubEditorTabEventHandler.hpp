// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SubEditorTabEventHandler
#define GT_SubEditorTabEventHandler

#include <GTEngine/GUI/GUIElementEventHandler.hpp>

namespace GT
{
    class SubEditor;

    /// Class representing the event handler to attack to the tab of a sub-editor.
    class SubEditorTabEventHandler : public GTGUI::GUIElementEventHandler
    {
    public:

        /// Constructor.
        SubEditorTabEventHandler(SubEditor &owner);

        /// Destructor.
        ~SubEditorTabEventHandler();


        /// GTGUI::GUIElementEventHandler::OnMouseButtonDown().
        void OnMouseButtonDown(GTGUI::GUIElement &element, int button, int x, int y);


    private:

        /// A reference to the sub editor that owns the tab this event handler is attached to.
        SubEditor &owner;


    private:    // No copying.
        SubEditorTabEventHandler(const SubEditorTabEventHandler &);
        SubEditorTabEventHandler & operator=(const SubEditorTabEventHandler &);
    };
}

#endif
