// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGE/Editor2/Editor2.hpp>
#include <string.h>
#include <assert.h>

namespace GT
{
    /// The callback function for retrieving the default layout config.
    static const char* Editor_OnDefaultLayout(ak_application* pApplication)
    {
        Editor2** ppEditor = reinterpret_cast<Editor2**>(ak_get_application_extra_data(pApplication));
        assert(ppEditor != NULL);

        Editor2* pEditor = *ppEditor;
        assert(pEditor != NULL);

        return pEditor->GetDefaultConfig();
    }



    Editor2::Editor2(Context &context)
        : m_context(context),
          m_pApplication(NULL)
    {
    }

    Editor2::~Editor2()
    {
        this->Shutdown();
    }


    int Editor2::StartupAndRun()
    {
        m_pApplication = ak_create_application("GTGE/Editor", sizeof(this), this);
        if (m_pApplication != NULL)
        {
            // Set the callback that the application object will call when the default layout script is required.
            ak_set_on_default_config(m_pApplication, Editor_OnDefaultLayout);

            return ak_run_application(m_pApplication);
        }

        return -1;
    }

    void Editor2::Shutdown()
    {
        if (m_pApplication != nullptr)
        {
            ak_delete_application(m_pApplication);
            m_pApplication = NULL;
        }
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
            "PrimaryWindow 0 0 1280, 720 true\n"
            "  "
            "/PrimaryWindow\n"
            "/Layout";
    }
}
