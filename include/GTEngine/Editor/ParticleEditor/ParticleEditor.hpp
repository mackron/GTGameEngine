// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleEditor_hpp_
#define __GTEngine_ParticleEditor_hpp_

#include "../SubEditor.hpp"
#include "../Editor3DViewportEventHandler.hpp"
#include "../EditorGrid.hpp"
#include "../../ParticleSystem.hpp"
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


        /// Retrieves a reference to the particle system definition being editted.
        ParticleSystemDefinition & GetParticleSystemDefinition();

        /// Refreshes the viewport so that it shows the current state of the particle system being editted.
        ///
        /// @remarks
        ///     This should be called whenever the particle definition has been modified.
        void RefreshViewport();



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::GetMainElement()
              GTGUI::Element* GetMainElement()        { return this->mainElement; }
        const GTGUI::Element* GetMainElement( ) const { return this->mainElement; }

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

        /// The particle system definition that is being editted. This is not instantiated by the particle system library.
        ParticleSystemDefinition particleSystemDefinition;

        /// The particle system to use in the preview window.
        ParticleSystem particleSystem;



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


        /// The grid.
        EditorGrid grid;


        /// Keeps track of whether or not the editor is in the middle of saving. We use this in determining whether or not the settings should be
        /// set when it detects a modification to the file on disk.
        bool isSaving;

        /// Keeps track of whether or not we are handling a reload. We use this in keeping track of whether or not to mark the file as modified
        /// when the settings are changed.
        bool isReloading;


    private:    // No copying.
        ParticleEditor(const ParticleEditor &);
        ParticleEditor & operator=(const ParticleEditor &);
    };
}

#endif
