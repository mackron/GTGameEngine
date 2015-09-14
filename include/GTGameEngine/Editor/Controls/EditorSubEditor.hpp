// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorSubEditor
#define GT_EditorSubEditor

#include "EditorControl.hpp"
#include <functional>

namespace GT
{
    class EditorSubEditor : public EditorControl
    {
    public:

        typedef std::function<void ()> OnChangedProc;


        /// Constructor.
        EditorSubEditor(Editor &editor);

        /// Destructor.
        virtual ~EditorSubEditor();


        /// Sets the function to call when a change is made to the resource.
        void OnChanged(OnChangedProc onChangedProc);

        /// Calls the OnChanged function.
        void OnChanged();


        /// Saves the contents to the given file.
        virtual bool SaveFile(const char* absolutePath) = 0;



    private:

        /// The function to call when the resource is modified. This is set by OnChanged()
        OnChangedProc m_onChangedProc;
    
    };
}

#endif
