
#include <GTEngine/Editor/SubEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTCore/IO.hpp>
#include <GTCore/Path.hpp>

namespace GTEngine
{
    SubEditor::SubEditor(Editor &ownerEditorIn, const char* absolutePathIn, const char* relativePathIn)
        : ownerEditor(ownerEditorIn), absolutePath(absolutePathIn), relativePath(relativePathIn)
    {
    }

    SubEditor::~SubEditor()
    {
    }


    const char* SubEditor::GetAbsolutePath() const
    {
        return this->absolutePath.c_str();
    }

    const char* SubEditor::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }





    ///////////////////////////////////////////////////
    // Virtual Methods.

    void SubEditor::Show()
    {
    }

    void SubEditor::Hide()
    {
    }

    void SubEditor::Save()
    {
    }
}