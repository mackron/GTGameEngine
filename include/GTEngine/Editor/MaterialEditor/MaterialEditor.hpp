// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_MaterialEditor_hpp_
#define __GTEngine_MaterialEditor_hpp_

#include "../SubEditor.hpp"
#include "../Editor3DViewportEventHandler.hpp"
#include "../../Scene.hpp"

namespace GTEngine
{
    /// The material editor.
    class MaterialEditor : public SubEditor
    {
    public:

        /// Constructor.
        MaterialEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~MaterialEditor();


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

        /// The scene node containing the model to use in the preview window.
        SceneNode modelNode;


        /// The main container element.
        GTGUI::Element* mainElement;

        /// The viewport element.
        GTGUI::Element* viewportElement;

        /// The viewport event handler.
        Editor3DViewportEventHandler viewportEventHandler;


        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The material to use with the model.
        Material* material;
    };
}

#endif