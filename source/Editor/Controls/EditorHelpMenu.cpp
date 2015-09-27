// Copyright (C) 2015 David Reid. See included LICENCE file.

// TODO: Get the platform-specific stuff out of here.

#include <GTGameEngine/Config.hpp>
#include <GTGameEngine/Editor/Controls/EditorHelpMenu.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#endif

namespace GT
{
    EditorHelpMenu::EditorHelpMenu(Editor &editor, HWindow hParentWindow)
        : EditorMenu(editor, hParentWindow)
    {
        EditorMenuItem* pDocumentation = this->AppendMenuItem("Documentation...", "");
        pDocumentation->OnPressed([&]() {
#if defined(GT_PLATFORM_WINDOWS)
            ShellExecuteA(NULL, "open", "https://mackron.github.com/GTGameEngine/documentation", NULL, NULL, SW_SHOWNORMAL);
#endif
        });


        this->AppendSeparator();
        this->AppendMenuItem("About...", "");
    }

    EditorHelpMenu::~EditorHelpMenu()
    {
    }
}
