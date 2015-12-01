// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Editor_PackagingToolEditor
#define GT_Editor_PackagingToolEditor

#include "../SubEditor.hpp"

namespace GT
{
    class PackagingToolEditor : public SubEditor
    {
    public:

        /// Constructor.
        PackagingToolEditor(Editor &ownerEditor, const char* path);

        /// Destructor.
        ~PackagingToolEditor();



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::GetMainElement()
              GTGUI::GUIElement* GetMainElement()       { return this->mainElement; }
        const GTGUI::GUIElement* GetMainElement() const { return this->mainElement; }

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();



    private:

        /// The main GUI element.
        GTGUI::GUIElement* mainElement;
        
        
    private:    // No copying.
        PackagingToolEditor(const PackagingToolEditor &);
        PackagingToolEditor & operator=(const PackagingToolEditor &);
    };
}

#endif
