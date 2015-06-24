// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorControl_hpp_
#define __GT_EditorControl_hpp_

#include "../../GUI/GUIContext.hpp"

namespace GT
{
    class Editor;

    /// Base class for editor controls.
    class EditorControl
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

        /// Retrieves a reference to the GUI context.
        ///
        /// @return A reference to the GUI context that was used to construct the control.
              GUIContext & GetGUI();
        const GUIContext & GetGUI() const;

        /// Retrieves a handle to the root GUI element.
        ///
        /// @return A handle to the control's root GUI element.
        HGUIElement GetRootGUIElement() const;



    private:

        /// A reference to the GUI context that owns the element making up the control.
        Editor &m_editor;

        /// A handle to the GUI element. This is created in the constructor.
        HGUIElement m_hRootElement;



    private:    // No copying.
        EditorControl(const EditorControl &);
        EditorControl & operator=(const EditorControl &);
    };
}

#endif
