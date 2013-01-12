
#ifndef __GTEngine_SubEditorTabEventHandler_hpp_
#define __GTEngine_SubEditorTabEventHandler_hpp_

#include <GTGUI/ElementEventHandler.hpp>

namespace GTEngine
{
    class SubEditor;

    /// Class representing the event handler to attack to the tab of a sub-editor.
    class SubEditorTabEventHandler : public GTGUI::ElementEventHandler
    {
    public:

        /// Constructor.
        SubEditorTabEventHandler(SubEditor &owner);

        /// Destructor.
        ~SubEditorTabEventHandler();


        /// GTGUI::ElementEventHandler::OnMMBDown().
        void OnMMBDown(GTGUI::Element &element, int x, int y);


    private:

        /// A reference to the sub editor that owns the tab this event handler is attached to.
        SubEditor &owner;


    private:    // No copying.
        SubEditorTabEventHandler(const SubEditorTabEventHandler &);
        SubEditorTabEventHandler & operator=(const SubEditorTabEventHandler &);
    };
}

#endif