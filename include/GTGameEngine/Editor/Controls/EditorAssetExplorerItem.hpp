// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorAssetExplorerItem_hpp_
#define __GT_EditorAssetExplorerItem_hpp_

#include "EditorControl.hpp"

namespace GT
{
    // Class representing an item in the asset explorer.
    class EditorAssetExplorerItem : public EditorControl
    {
    public:

        EditorAssetExplorerItem(Editor &editor, bool isFolder);


        /// Determines whether or not the item is a folder.
        bool IsFolder() const;



    private:

        // Whether or not the item is a folder.
        bool m_isFolder;
    };
}

#endif
