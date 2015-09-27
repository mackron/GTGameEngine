// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/WorkbenchEditor/WorkbenchEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    WorkbenchEditor::WorkbenchEditor(Editor &editor, SubEditorAllocator &allocator, const char* identifier)
        : EditorSubEditor(editor, allocator, identifier)
    {
        m_gui.SetElementSizeRatio(this->GetRootElement(), 1, 1);
        m_gui.SetElementBorder(m_hRootElement, 1, m_editor.GetTheme().borderDim);
    }

    WorkbenchEditor::~WorkbenchEditor()
    {
    }
}
