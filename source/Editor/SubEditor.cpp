
#include <GTEngine/Editor/SubEditor.hpp>
#include <GTCore/IO.hpp>
#include <GTCore/Path.hpp>

namespace GTEngine
{
    SubEditor::SubEditor(const char* path, const char* relativeTo)
        : absolutePath(), relativePath()
    {
        if (GTCore::Path::IsAbsolute(path))
        {
            this->absolutePath = path;

            assert(relativeTo != nullptr);
            assert(GTCore::Path::IsAbsolute(relativeTo));
            {
                this->relativePath = GTCore::IO::ToRelativePath(path, relativeTo);
            }
        }
        else
        {
            if (!GTCore::IO::FindAbsolutePath(path, this->absolutePath))
            {
                // If we get here, the file doesn't actually exist. We'll set the absolute path anyway, but it will be erroneous.
                this->absolutePath = GTCore::IO::ToAbsolutePath(path, GTCore::IO::GetCurrentDirectory());
            }

            this->relativePath = path;
        }
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