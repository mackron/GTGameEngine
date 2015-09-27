// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_WorkbenchEditor
#define GT_WorkbenchEditor

#include "../EditorSubEditor.hpp"

#include <GTGameEngine/GUI/CommonControls/TreeView.hpp>

namespace GT
{
    class WorkbenchEditor : public EditorSubEditor
    {
    public:

        /// Constructor
        WorkbenchEditor(Editor &editor, SubEditorAllocator &allocator, const char* identifier);

        /// Destructor.
        ~WorkbenchEditor();



    private:

        //HGUIElement m_hTV;

        TreeView* m_pTV;


        TreeViewItem* m_pRootItem;

        TreeViewItem* m_pItem0;
        TreeViewItem* m_pItem0_0;

        TreeViewItem* m_pItem1;
        TreeViewItem* m_pItem1_0;
    };
}

#endif
