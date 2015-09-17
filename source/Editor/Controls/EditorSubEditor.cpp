// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorSubEditor.hpp>

#define EASYUTIL_ONLY_MSVC_COMPAT
#include <easy_util/easy_util.h>

namespace GT
{
    EditorSubEditor::EditorSubEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePathOrIdentifier)
        : EditorControl(editor),
          m_allocator(allocator),
          m_absolutePathOrIdentifier(),
          m_pTab(nullptr),
          m_onChangedProc(nullptr)
    {
        strcpy_s(m_absolutePathOrIdentifier, GT_MAX_PATH, absolutePathOrIdentifier);
    }

    EditorSubEditor::~EditorSubEditor()
    {
    }


    SubEditorAllocator & EditorSubEditor::GetAllocator() const
    {
        return m_allocator;
    }

    const char* EditorSubEditor::GetAbsolutePathOrIdentifier() const
    {
        return m_absolutePathOrIdentifier;
    }


    void EditorSubEditor::SetTab(EditorTab* pTab)
    {
        m_pTab = pTab;
    }

    EditorTab* EditorSubEditor::GetTab() const
    {
        return m_pTab;
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
