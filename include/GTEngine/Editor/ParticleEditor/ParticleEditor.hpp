// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "../SubEditor.hpp"
#include "../Editor3DViewportEventHandler.hpp"
#include "../../Scene.hpp"

namespace GTEngine
{
    /// Class representing the particle system editor.
    class ParticleEditor : public SubEditor
    {
    public:

        /// Constructor.
        ParticleEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~ParticleEditor();


        /// Resets the camera.
        void ResetCamera();




        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();

        /// SubEditor::Save()
        bool Save();

        /// SubEditor::Update()
        void OnUpdate(double deltaTimeInSeconds);


        /// SubEditor::OnFileUpdate()
        void OnFileUpdate(const DataFilesWatcher::Item &item);



    private:

        /// Applies the camera rotation to the camera node.
        void ApplyCameraRotation();


    private:


        /// The scene for the preview window.
        Scene scene;

        /// The viewport that the preview scene will be drawn from.
        SceneViewport viewport;

        /// The scene node acting as the camera for the preview window viewport.
        SceneNode camera;
    };
}