// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_ParticleEditor_hpp_
#define __GTEngine_ParticleEditor_hpp_

#include "../SubEditor.hpp"
#include "../Editor3DViewportEventHandler.hpp"
#include "../EditorGrid.hpp"
#include "../EditorAxisArrows.hpp"
#include "../../ParticleSystem.hpp"
#include "../../Scene.hpp"

namespace GT
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

        /// Retrieves a reference tothe camera scene node.
              SceneNode & GetCameraSceneNode()       { return this->camera; }
        const SceneNode & GetCameraSceneNode() const { return this->camera; }


        /// Retrieves a reference to the particle system definition being editted.
        ParticleSystemDefinition & GetParticleSystemDefinition();

        /// Refreshes the viewport so that it shows the current state of the particle system being editted.
        ///
        /// @remarks
        ///     This should be called whenever the particle definition has been modified.
        void RefreshViewport();


        /// Sets the orientation of the preview particle system.
        void SetOrientation(const glm::quat &orientation);


        /// Shows the grid.
        void ShowGrid();

        /// Hides the grid.
        void HideGrid();

        /// Determines whether or not the grid is showing.
        bool IsShowingGrid() const;


        /// Shows the axis arrows.
        void ShowAxisArrows();

        /// Hides the axis arrows.
        void HideAxisArrows();

        /// Determines whether or not the axis arrows is showing.
        bool IsShowingAxisArrows() const;
        


        ///////////////////////////////////////////////////
        // GUI Events.
        //
        // These events are received in response to certain GUI events.

        /// Called when the main viewport is resized.
        void OnViewportSize();



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::GetMainElement()
              GTGUI::Element* GetMainElement()       { return this->mainElement; }
        const GTGUI::Element* GetMainElement() const { return this->mainElement; }

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



    private:

        /// The particle system definition that is being editted. This is not instantiated by the particle system library.
        ParticleSystemDefinition particleSystemDefinition;

        /// The particle system to use in the preview window.
        ParticleSystem particleSystem;



        /// The scene for the preview window.
        Scene scene;

        /// The scene node acting as the camera for the preview window viewport.
        SceneNode camera;

        /// The scene node containing the particle system.
        SceneNode particleNode;


        /// The main container element.
        GTGUI::Element* mainElement;


        /// The viewport element.
        GTGUI::Element* viewportElement;

        /// The viewport event handler to we can detect when it is resized.
        struct ViewportEventHandler : public Editor3DViewportEventHandler
        {
            /// Constructor.
            ViewportEventHandler(ParticleEditor &ownerIn, Game &game, SceneViewport &viewport)
                : Editor3DViewportEventHandler(game, viewport), owner(ownerIn)
            {
            }


            /// Called after the element has been resized.
            void OnSize(GTGUI::Element &element)
            {
                Editor3DViewportEventHandler::OnSize(element);
                owner.OnViewportSize();
            }


            /// The owner of the viewport.
            ParticleEditor &owner;

        }viewportEventHandler;


        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The grid.
        EditorGrid grid;

        /// The axis arrows.
        EditorAxisArrows axisArrows;


        /// Keeps track of whether or not the editor is in the middle of saving. We use this in determining whether or not the settings should be
        /// set when it detects a modification to the file on disk.
        bool isSaving;

        /// Keeps track of whether or not we are handling a reload. We use this in keeping track of whether or not to mark the file as modified
        /// when the settings are changed.
        bool isReloading;


        /// Keeps track of whether or not the grid is visible.
        bool isShowingGrid;

        /// Keeps track of whether or not the axis arrows are visible.
        bool isShowingAxisArrows;


    private:    // No copying.
        ParticleEditor(const ParticleEditor &);
        ParticleEditor & operator=(const ParticleEditor &);
    };
}

#endif
