// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/MaterialEditor/MaterialEditor.hpp>

namespace GTEngine
{
    MaterialEditor::MaterialEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath)
    {
    }

    MaterialEditor::~MaterialEditor()
    {
    }
}