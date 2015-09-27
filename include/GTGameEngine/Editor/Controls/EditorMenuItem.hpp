// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorMenuItem
#define GT_EditorMenuItem

#include "EditorControl.hpp"

namespace GT
{
    class EditorMenuItem : public EditorControl
    {
    public:

        typedef std::function<void ()> OnPressedProc;


        /// Constructor
        EditorMenuItem(Editor &editor, HGUIElement hParent, const char* text, const char* shortcut, bool showArrow);

        /// Destructor.
        ~EditorMenuItem();


        /// Retrieves a handle to the icon element.
        HGUIElement GetIconElement() const { return m_hIcon; }

        /// Retrieves a handle to the text element.
        HGUIElement GetTextElement() const { return m_hText; }

        /// Retrieves a handle to the shortcut element.
        HGUIElement GetShortcutElement() const { return m_hShortcut; }


        /// Sets the function to call when the item is pressed.
        ///
        /// @remarks
        ///     Note that when this is called the menu is not automatically closed. This is because the menu item has no knowledge
        ///     of the menu that owns it.
        void OnPressed(OnPressedProc proc);


    private:

        /// The element containing the icon.
        HGUIElement m_hIcon;

        /// The element containing the text.
        HGUIElement m_hText;

        /// The element containing the shortcut text.
        HGUIElement m_hShortcut;

        /// The element containing the arrow icon.
        HGUIElement m_hArrow;


        /// The function to call when the item is pressed.
        OnPressedProc m_onPressed;
    };
}

#endif
