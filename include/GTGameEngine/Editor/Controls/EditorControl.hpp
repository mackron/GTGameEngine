// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorControl_hpp_
#define __GT_EditorControl_hpp_

#include "../../GUI/GUIContext.hpp"

namespace GT
{
    /// Base class for editor controls.
    class EditorControl
    {
    public:

        /// Constructor.
        ///
        /// @remarks
        ///     This will create an empty element. It is up to the host to attach the element to a parent element.
        EditorControl(GUIContext &gui);

        /// Destructor.
        virtual ~EditorControl();


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
        GUIContext &m_gui;

        /// A handle to the GUI element. This is created in the constructor.
        HGUIElement m_hRootElement;



    private:    // No copying.
        EditorControl(const EditorControl &);
        EditorControl & operator=(const EditorControl &);
    };
}

#endif
