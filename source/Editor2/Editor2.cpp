// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "../../include/GTGE/Editor2/Editor2.hpp"
#include "../../include/GTGE/Context.hpp"
#include "../Editor2/AssetExplorer.hpp"
#include <string.h>
#include <assert.h>

namespace GT
{
    struct AK_EditorUserData
    {
        /// A pointer to the editor.
        Editor2* pEditor;
    };


    /// The callback function for retrieving the default layout config.
    static const char* Editor_OnDefaultLayout(ak_application* pApplication)
    {
        AK_EditorUserData* pUserData = reinterpret_cast<AK_EditorUserData*>(ak_get_application_extra_data(pApplication));
        assert(pUserData != NULL);

        Editor2* pEditor = pUserData->pEditor;
        assert(pEditor != NULL);

        return pEditor->GetDefaultConfig();
    }

    
    /// The callback function for creating a tool.
    static easygui_element* Editor_OnCreateTool(ak_application* pApplication, ak_window* pWindow, const char* type, const char* attributes)
    {
        AK_EditorUserData* pUserData = reinterpret_cast<AK_EditorUserData*>(ak_get_application_extra_data(pApplication));
        assert(pUserData != NULL);

        Editor2* pEditor = pUserData->pEditor;
        assert(pEditor != NULL);

        EditorTool* pTool = NULL;
        if (strcmp(type, "AssetExplorer") == 0) {
            pTool = new AssetExplorer(*pEditor);
        }


        if (pTool == NULL) {
            return NULL;
        }

        if (!pTool->Init(attributes))
        {
            delete pTool;
            return NULL;
        }

        return pTool->GetAKTool();
    }

    /// The callback function for deleting a tool.
    static bool Editor_OnDeleteTool(ak_application* pApplication, easygui_element* pTool, bool force)
    {
        EditorTool* pEditorTool = GetEditorTool(pTool);
        if (pEditorTool == NULL) {
            return true;
        }

        delete pEditorTool;
        return true;
    }



    Editor2::Editor2(Context &context)
        : m_context(context),
          m_pApplication(NULL),
          m_pSymbolFont(NULL), m_symbolFontMetrics()
    {
    }

    Editor2::~Editor2()
    {
        this->Shutdown();
    }


    int Editor2::StartupAndRun()
    {
        AK_EditorUserData userData;
        userData.pEditor = this;

        m_pApplication = ak_create_application("GTGE/Editor", sizeof(userData), &userData);
        if (m_pApplication != NULL)
        {
            m_pSymbolFont = easygui_create_font(ak_get_application_gui(m_pApplication), "Segoe UI Symbol", 14, easygui_font_weight_normal, easygui_font_slant_none, 0);
            easygui_get_font_metrics(m_pSymbolFont, 1, 1, &m_symbolFontMetrics);

            // Set the callback that the application object will call when the default layout script is required.
            ak_set_on_default_config(m_pApplication, Editor_OnDefaultLayout);
            ak_set_on_create_tool(m_pApplication, Editor_OnCreateTool);
            ak_set_on_delete_tool(m_pApplication, Editor_OnDeleteTool);

            return ak_run_application(m_pApplication);
        }

        return -1;
    }

    void Editor2::Shutdown()
    {
        if (m_pApplication != NULL)
        {
            ak_delete_application(m_pApplication);
            m_pApplication = NULL;
        }
    }


    ak_application* Editor2::GetAKApplication()
    {
        return m_pApplication;
    }

    easygui_context* Editor2::GetGUI()
    {
        return ak_get_application_gui(m_pApplication);
    }

    easyvfs_context* Editor2::GetVFS()
    {
        return m_context.GetVFS();
    }

    ak_theme* Editor2::GetAKTheme()
    {
        return ak_get_application_theme(m_pApplication);
    }



    ///////////////////////////////////////////////////////////
    //
    // The methods below should only be used internally.
    //
    ///////////////////////////////////////////////////////////

    const char* Editor2::GetDefaultConfig()
    {
        return
            "Layout \"Default\"\n"
            "Window application 0 0 1280, 720 true \"GTGE Editor\" MainWindow\n"
            "  "
            "/Window\n"
            "/Layout";
    }
}
