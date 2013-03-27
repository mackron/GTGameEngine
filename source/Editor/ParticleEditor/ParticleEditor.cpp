// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/ParticleEditor/ParticleEditor.hpp>

namespace GTEngine
{
    ParticleEditor::ParticleEditor(Editor &editor, const char* absolutePath, const char* relativePath)
        : SubEditor(editor, absolutePath, relativePath)
    {
    }

    ParticleEditor::~ParticleEditor()
    {
    }


    void ParticleEditor::ResetCamera()
    {
    }


    ///////////////////////////////////////////////////
    // Virtual Methods.

    void ParticleEditor::Show()
    {
    }

    void ParticleEditor::Hide()
    {
    }

    bool ParticleEditor::Save()
    {
        return true;
    }

    void ParticleEditor::OnUpdate(double deltaTimeInSeconds)
    {
    }

    void ParticleEditor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
    }




    ///////////////////////////////////////////////////
    // Private.

    void ParticleEditor::ApplyCameraRotation()
    {
    }
}