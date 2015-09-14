// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorSubEditor.hpp>

namespace GT
{
    EditorSubEditor::EditorSubEditor(Editor &editor)
        : EditorControl(editor),
          m_onChangedProc(nullptr)
    {
    }

    EditorSubEditor::~EditorSubEditor()
    {
    }


    void EditorSubEditor::OnChanged(OnChangedProc onChangedProc)
    {
        m_onChangedProc = onChangedProc;
    }

    void EditorSubEditor::OnChanged()
    {
        if (m_onChangedProc != nullptr) {
            m_onChangedProc();
        }
    }
}
