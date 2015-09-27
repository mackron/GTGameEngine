// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorFileMenu.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/GameContext.hpp>

namespace GT
{
    EditorFileMenu::EditorFileMenu(Editor &editor, HWindow hParentWindow)
        : EditorMenu(editor, hParentWindow)
    {
        EditorMenuItem* pSave = this->AppendMenuItem("Save", "Ctrl+S");
        pSave->OnPressed([&]() {
            m_editor.SaveFocusedFile();
            this->Close();
        });

        EditorMenuItem* pSaveAll = this->AppendMenuItem("Save All", "Ctrl+Shift+S");
        pSaveAll->OnPressed([&]() {
            m_editor.SaveAllOpenFiles();
            this->Close();
        });
            
        this->AppendSeparator();
            
        EditorMenuItem* pExit = this->AppendMenuItem("Exit", "Alt+F4");
        pExit->OnPressed([&]() {
            m_editor.GetGameContext().Close();
        });


        this->RefreshMenuItemAlignments();
    }

    EditorFileMenu::~EditorFileMenu()
    {
    }
}
