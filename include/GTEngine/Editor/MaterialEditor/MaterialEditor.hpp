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

        /// Retrieves a reference to the viewport camera scene node.
              SceneNode & GetViewportCameraSceneNode()       { return this->camera; }
        const SceneNode & GetViewportCameraSceneNode() const { return this->camera; }


        ///////////////////////////////////////////////////
        // GUI Events.

        /// Called when the text script text box is modified.
        void OnScriptTextChanged();



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


    private:

        /// The event handler attached to the text area.
        struct ScriptTextBoxEventHandler : public GTGUI::ElementEventHandler
        {
            /// Constructor.
            ScriptTextBoxEventHandler(MaterialEditor &ownerEditorIn)
                : ownerEditor(ownerEditorIn)
            {
            }

            /// GTGUI::ElementEventHandler::OnTextChanged().
            void OnTextChanged(GTGUI::Element &)
            {
                ownerEditor.OnScriptTextChanged();
            }


            /// A pointer to the text editor that owns the text area.
            MaterialEditor &ownerEditor;


        private:    // No copying.
            ScriptTextBoxEventHandler(const ScriptTextBoxEventHandler &);
            ScriptTextBoxEventHandler & operator=(const ScriptTextBoxEventHandler &);
        };






        /// The scene for the preview window.
        Scene scene;

        /// The scene node acting as the camera for the preview window viewport.
        SceneNode camera;

        /// The scene node containing the model to use in the preview window.
        SceneNode modelNode;


        /// The main container element.
        GTGUI::Element* mainElement;


        /// The script text box element.
        GTGUI::Element* scriptTextBoxElement;

        /// The event handler for the script text box.
        ScriptTextBoxEventHandler scriptTextBoxEventHandler;


        /// The viewport element.
        GTGUI::Element* viewportElement;

        /// The viewport event handler.
        Editor3DViewportEventHandler viewportEventHandler;



        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The material to use with the model.
        Material* material;


        /// Keeps track of whether or not the material editor is in the middle of saving. We use this in determining whether or not the text
        /// should be set when it detects a modification to the file on disk.
        bool isSaving;

        /// Keeps track of whether or not we are handling a reload. We use this in keeping track of whether or not to mark the file as modified
        /// when the script text is changed.
        bool isReloading;


    private:    // No copying.
        MaterialEditor(const MaterialEditor &);
        MaterialEditor & operator=(const MaterialEditor &);
    };
}

#endif
