// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleEditor_hpp_
#define __GTEngine_ParticleEditor_hpp_

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

        /// The scene node containing the particle system.
        SceneNode particleNode;


        /// The main container element.
        GTGUI::Element* mainElement;


        /// The viewport element.
        GTGUI::Element* viewportElement;

        /// The viewport event handler.
        Editor3DViewportEventHandler viewportEventHandler;


        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// Keeps track of whether or not the editor is in the middle of saving. We use this in determining whether or not the settings should be
        /// set when it detects a modification to the file on disk.
        bool isSaving;

        /// Keeps track of whether or not we are handling a reload. We use this in keeping track of whether or not to mark the file as modified
        /// when the settings are changed.
        bool isReloading;
    };
}

#endif
