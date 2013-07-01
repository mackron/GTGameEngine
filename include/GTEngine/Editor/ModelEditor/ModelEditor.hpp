// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ModelEditor_hpp_
#define __GTEngine_ModelEditor_hpp_

#include "../SubEditor.hpp"
#include "../Editor3DViewportEventHandler.hpp"
#include "../EditorGrid.hpp"
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


        /// Retrieves a reference to the viewport camera scene node.
              SceneNode & GetViewportCameraSceneNode()       { return this->camera; }
        const SceneNode & GetViewportCameraSceneNode() const { return this->camera; }


        /// Retrieves a reference to the model definition being modified.
        ModelDefinition & GetModelDefinition();


        /// Retrieves the AABB of the model.
        ///
        /// @param aabbMin [in] A reference to the variable that will receive the lower bounds.
        /// @param aabbMax [in] A reference to the variable that will receive the upper bounds.
        void GetModelAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax);


        /// Refreshes the viewport so that it shows the current state of the model being editted.
        ///
        /// @remarks
        ///     This should be called whenever the model definition has been modified.
        void RefreshViewport();


        /// Retrieves the relative paths of the materials attached to the model.
        ///
        /// @param materialsOut [out] A reference to the vector that will receive the material paths.
        void GetMaterials(GTCore::Vector<GTCore::String> &materialsOut);


        /// Adds a new animation segment to the model.
        ///
        /// @param name       [in] The name of the segment.
        /// @param startIndex [in] The index of the first frame.
        /// @param endIndex   [in] The index of the last frame.
        void AddNewAnimationSegment(const char* name, size_t startIndex, size_t endIndex);

        /// Removes an animation segment by it's index.
        ///
        /// @param index [in] The index of the animation segment to remove.
        void RemoveAnimationSegmentByIndex(size_t index);

        /// Sets the name of the given animation segment.
        ///
        /// @param index   [in] The index of the animation segment whose name is being set.
        /// @param newName [in] The new name of the animation segment.
        void SetAnimationSegmentName(size_t index, const char* newName);

        /// Retrieves the name of the given animation segment.
        ///
        /// @param index [in] The index of the animation segment whose name is being retrieved.
        const char* GetAnimationSegmentName(size_t index) const;

        /// Sets the start and end frame indices of the given animation segment.
        ///
        /// @param segmentIndex [in] The index of the animation segment whose frame indices are being set.
        /// @param startIndex   [in] The index of the first frame.
        /// @param endIndex     [in] The index of the last frame.
        void SetAnimationSegmentFrameIndices(size_t segmentIndex, size_t startIndex, size_t endIndex);

        /// Retrieves the start and end frame indices of the given animation segment.
        ///
        /// @param segmentIndex [in] The index of the animation segment whose frame indices are being retrieved.
        /// @param startIndex   [in] A reference to the variable that will receive the index of the first frame.
        /// @param endIndex     [in] A reference to the variable that will receive the index of the last frame.
        void GetAnimationSegmentFrameIndices(size_t segmentIndex, size_t &startIndex, size_t &endIndex) const;


        /// Plays an animation segment by it's index.
        ///
        /// @param segmentIndex [in] The index of the segment to play.
        ///
        /// @remarks
        ///     The animation will loop.
        void PlayAnimationSegmentByIndex(size_t segmentIndex);

        /// Plays the model's animation track.
        void PlayAnimation();

        /// Resumes the model's animation if it's currently paused.
        void ResumeAnimation();

        /// Stops the model's animation track.
        void StopAnimation();

        /// Pauses the model's animation.
        void PauseAnimation();

        /// Determines whether or not the model's animation is playing.
        bool IsAnimationPlaying() const;

        /// Determines whether or not the model's animation is paused.
        bool IsAnimationPaused() const;


        /// Sets a material on the model.
        ///
        /// @param index        [in] The index of the material being set.
        /// @param relativePath [in] The relative path of the material being set.
        bool SetMaterial(size_t index, const char* relativePath);


        /// Retrieves the convex hull build settings.
        ///
        /// @param settingsOut [out] A reference to the object that will receive the settings.
        void GetConvexHullBuildSettings(ConvexHullBuildSettings &settingsOut);

        /// Shows the current model's convex decomposition.
        void ShowConvexDecomposition();

        /// Hides the current model's convex decomposition.
        void HideConvexDecomposition();

        /// Builds the convex decomposition of the current model.
        void BuildConvexDecomposition(ConvexHullBuildSettings &settings);


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

        /// Deletes the convex hulls for the currently loaded model.
        void DeleteConvexHulls();

        /// Refreshes the model editor.
        void Refresh();

        /// Reloads the model.
        void Reload();

        /// Updates the animation playback controls.
        void UpdateAnimationPlaybackControls();


    private:

        /// The model definition that is being editted.
        ModelDefinition modelDefinition;

        /// The model to show in the viewport.
        Model model;


        /// The scene containing the model.
        Scene scene;

        /// The scene node acting as the camera for the viewport.
        SceneNode camera;


        /// The scene node containing the model.
        SceneNode modelNode;

        /// The parent node for convex hulls. Hidden by default.
        GTEngine::SceneNode convexHullParentNode;

        /// The list of scene nodes containing the models for the visual representation of the convex hulls.
        GTCore::Vector<GTEngine::SceneNode*> convexHullNodes;


        /// The main container element.
        GTGUI::Element* mainElement;

        /// The viewport element.
        GTGUI::Element* viewportElement;

        /// The timeline element.
        GTGUI::Element* timelineElement;


        /// The viewport event handler.
        Editor3DViewportEventHandler viewportEventHandler;

        


        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The grid.
        EditorGrid grid;


        /// The random number generator for convex hull colours, and whatever else we may need.
        GTCore::Random random;


        /// The index of the segment that is currently being played. Set to -1 when nothing is playing.
        int currentlyPlayingSegmentIndex;

        /// The index of the sequence that is currently being played. Set to -1 when nothing is playing.
        int currentlyPlayingSequenceIndex;


        /// Keeps track of whether or not the editor is in the middle of saving. We use this in determining whether or not the settings should be
        /// set when it detects a modification to the file on disk.
        bool isSaving;

        /// Keeps track of whether or not we are handling a reload. We use this in keeping track of whether or not to mark the file as modified
        /// when the settings are changed.
        bool isReloading;


    private:    // No copying.
        ModelEditor(const ModelEditor &);
        ModelEditor & operator=(const ModelEditor &);
    };
}

#endif
