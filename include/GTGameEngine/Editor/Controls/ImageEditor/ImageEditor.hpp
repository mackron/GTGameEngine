// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ImageEditor
#define GT_ImageEditor

#include "../EditorSubEditor.hpp"
#include "../../../Assets/ImageAsset.hpp"

namespace GT
{
    class ImageEditor : public EditorSubEditor
    {
    public:

        /// Constructor.
        ImageEditor(Editor &editor, const char* absolutePath, ImageAsset* pAsset);

        /// Destructor.
        virtual ~ImageEditor();



    private:

        /// A pointer to the image asset.
        ImageAsset* m_pAsset;
    };
}

#endif
