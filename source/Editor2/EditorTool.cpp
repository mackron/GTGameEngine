// Copyright (C) 2015 David Reid. See included LICENCE file.

#include "EditorTool.hpp"
#include <GTGE/Editor2/Editor2.hpp>
#include <easy_appkit/ak_gui.h>
#include <easy_gui/easy_gui.h>

namespace GT
{
    struct AK_EditorToolUserData
    {
        /// A pointer to the asset explorer.
        EditorTool* pEditorTool;
    };


    EditorTool::EditorTool(Editor2 &editor)
        : m_editor(editor), m_pTool(NULL)
    {
    }

    EditorTool::~EditorTool()
    {
        easygui_delete_element(m_pTool);
    }


    bool EditorTool::Init(const char* attributes)
    {
        (void)attributes;

        AK_EditorToolUserData userData;
        userData.pEditorTool = this;

        m_pTool = ak_create_tool(m_editor.GetAKApplication(), NULL, sizeof(this), &userData);
        if (m_pTool == NULL) {
            return false;
        }

        return true;
    }


    EditorTool* GetEditorTool(easygui_element* pTool)
    {
        AK_EditorToolUserData* pUserData = reinterpret_cast<AK_EditorToolUserData*>(ak_get_tool_extra_data(pTool));
        if (pUserData == NULL) {
            return NULL;
        }

        return pUserData->pEditorTool;
    }
}