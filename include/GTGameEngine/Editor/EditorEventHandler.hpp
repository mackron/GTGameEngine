// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorEventHandler_hpp_
#define __GT_EditorEventHandler_hpp_

namespace GT
{
    /// Base class for handling events from the editor.
    class EditorEventHandler
    {
    public:

        /// Constructor.
        EditorEventHandler();

        /// Destructor.
        virtual ~EditorEventHandler();


        /// Called after the editor has been opened.
        virtual void OnEditorOpened();

        /// Called after the editor has been closed.
        virtual void OnEditorClosed();
    };
}

#endif
