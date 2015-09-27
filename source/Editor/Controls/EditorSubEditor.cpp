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
          m_onChangedProc(nullptr), m_onUnchangedProc(nullptr)
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


    void EditorSubEditor::OnUnchanged(OnUnchangedProc onUnchangedProc)
    {
        m_onUnchangedProc = onUnchangedProc;
    }

    void EditorSubEditor::OnUnchanged()
    {
        if (m_onUnchangedProc != nullptr) {
            m_onUnchangedProc();
        }
    }


    bool EditorSubEditor::Save()
    {
        return false;
    }

    void EditorSubEditor::OnActivate()
    {
    }

    void EditorSubEditor::OnDeactivate()
    {
    }

    void EditorSubEditor::Cut()
    {
    }

    void EditorSubEditor::Copy()
    {
    }

    void EditorSubEditor::Paste()
    {
    }

    void EditorSubEditor::Undo()
    {
    }

    void EditorSubEditor::Redo()
    {
    }

    void EditorSubEditor::SelectAll()
    {
    }
}
