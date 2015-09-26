// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorControl
#define GT_EditorControl

#include "../../GUI/CommonControls/GUIControl.hpp"

namespace GT
{
    class Editor;

    /// Base class for editor controls.
    ///
    /// All event handling methods operate on the root GUI element.
    class EditorControl : public GUIControl
    {
    public:

        /// Constructor.
        ///
        /// @remarks
        ///     This will create an empty element. It is up to the host to attach the element to a parent element.
        EditorControl(Editor &editor);

        /// Destructor.
        virtual ~EditorControl();


        /// Retrieves a reference to the editor.
        ///
        /// @return A reference to the editor.
              Editor & GetEditor();
        const Editor & GetEditor() const;


    protected:

        /// A reference to the GUI context that owns the element making up the control.
        Editor &m_editor;
    };
}

#endif
