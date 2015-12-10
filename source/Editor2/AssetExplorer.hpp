// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_AssetExplorer
#define GT_AssetExplorer

#include "EditorTool.hpp"

namespace GT
{
    class Editor2;

    /// The asset explorer tool.
    ///
    /// This is implemented as an easy_appkit tool.
    class AssetExplorer : public EditorTool
    {
    public:

        /// Constructor.
        AssetExplorer(Editor2 &editor);

        /// Destructor.
        ~AssetExplorer();


        /// @copydoc EditorTool::Init()
        bool Init(const char* attributes);


        /// Retrieves a pointer to the internal tree-view control.
        easygui_element* GetTreeView();


    private:

        /// The tree view control.
        easygui_element* m_pTV;
    };
}

#endif
