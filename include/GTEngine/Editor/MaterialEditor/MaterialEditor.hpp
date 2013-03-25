// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_MaterialEditor_hpp_
#define __GTEngine_MaterialEditor_hpp_

#include "../SubEditor.hpp"

namespace GTEngine
{
    /// The material editor.
    class MaterialEditor : public SubEditor
    {
    public:

        /// Constructor.
        MaterialEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~MaterialEditor();



    private:

    };
}

#endif