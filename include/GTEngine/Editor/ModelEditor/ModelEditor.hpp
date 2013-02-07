// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ModelEditor_hpp_
#define __GTEngine_ModelEditor_hpp_

#include "../SubEditor.hpp"
#include "../Editor3DViewportEventHandler.hpp"
#include "../../Scene.hpp"
#include <GTCore/Random.hpp>

namespace GTEngine
{
    /// Class representing an image editor.
    class ModelEditor : public SubEditor
    {
    public:

        /// Constructor.
        ModelEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~ModelEditor();


        /// Resets the camera to the default position.
        void ResetCamera();

        /// Sets the rotation of the camera.
        void SetCameraRotation(float xRotation, float yRotation);


        /// Retrieves the relative paths of the materials attached to the model.
        ///
        /// @param materialsOut [out] A reference to the vector that will receive the material paths.
        void GetMaterials(GTCore::Vector<GTCore::String> &materialsOut);

        /// Retrieves the convex hull build settings.
        ///
        /// @param settingsOut [out] A reference to the object that will receive the settings.
        void GetConvexHullBuildSettings(ConvexHullBuildSettings &settingsOut);

        /// Plays the model's animation track.
        void PlayAnimation();

        /// Stops the model's animation track.
        void StopAnimation();

        /// Sets a material on the model.
        ///
        /// @param index        [in] The index of the material being set.
        /// @param relativePath [in] The relative path of the material being set.
        bool SetMaterial(size_t index, const char* relativePath);

        /// Shows the current model's convex decomposition.
        void ShowConvexDecomposition();

        /// Hides the current model's convex decomposition.
        void HideConvexDecomposition();

        /// Builds the convex decomposition of the current model.
        void BuildConvexDecomposition(ConvexHullBuildSettings &settings);


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

        /// Deletes the convex hulls for the currently loaded model.
        void DeleteConvexHulls();

        /// Refreshes the model editor.
        void Refresh();


    private:

        /// The scene containing the model.
        Scene scene;

        /// The viewport that the scene will be drawn from.
        SceneViewport viewport;

        /// The scene node acting as the camera for the viewport.
        SceneNode camera;


        /// The scene node containing the model.
        SceneNode modelNode;

        /// The parent node for convex hulls. Hidden by default.
        GTEngine::SceneNode convexHullParentNode;

        /// The list of scene nodes containing the models for the visual representation of the convex hulls.
        GTCore::Vector<GTEngine::SceneNode*> convexHullNodes;


        /// The main container element.s
        GTGUI::Element* mainElement;

        /// The viewport element.
        GTGUI::Element* viewportElement;

        /// The viewport event handler.
        Editor3DViewportEventHandler viewportEventHandler;


        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The random number generator for convex hull colours, and whatever else we may need.
        GTCore::Random random;


    private:    // No copying.
        ModelEditor(const ModelEditor &);
        ModelEditor & operator=(const ModelEditor &);
    };
}

#endif
