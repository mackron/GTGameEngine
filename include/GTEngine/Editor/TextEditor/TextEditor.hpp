// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_TextEditor
#define GT_TextEditor

#include "../SubEditor.hpp"

namespace GT
{
    /// Class representing a text editor.
    class TextEditor : public SubEditor
    {
    public:

        /// Constructor.
        TextEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~TextEditor();


        /// Determines whether or not the file being edited is a script file.
        bool IsScriptFile() const;


        /// Compiles the script file and updates the error output.
        ///
        /// @remarks
        ///     This won't do anything if the file is not a script file.
        void CompileAndUpdateErrorOutput();

        /// Called by the compilation script error handler when there is an error.
        ///
        /// @param message [in] The error message.
        void OnScriptSyntaxError(GT::Script &script, int lineNumber, const char* message);


        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::GetMainElement()
              GTGUI::GUIElement* GetMainElement()       { return this->mainElement; }
        const GTGUI::GUIElement* GetMainElement() const { return this->mainElement; }

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();

        /// SubEditor::Save()
        bool Save();



    private:

        /// Adds an error to the error list.
        ///
        /// @param lineNumber [in] The line number the error occurs on.
        /// @param message    [in] The error message.
        void AddItemToErrorList(int lineNumber, const char* message);

        /// Clears the error list.
        void ClearErrorList();


    private:

        /// The main text box element.
        GTGUI::GUIElement* mainElement;

        /// The text area element. This is a child of textBox.
        GTGUI::GUIElement* textArea;

        /// The panel element.
        GTGUI::GUIElement* panelElement;

        /// The error list element.
        GTGUI::GUIElement* errorListElement;


        /// The event handler attached to the text area.
        struct TextAreaEventHandler : public GTGUI::GUIElementEventHandler
        {
            /// Constructor.
            TextAreaEventHandler(TextEditor* ownerTextEditor);

            /// GTGUI::GUIElementEventHandler::OnTextChanged().
            void OnTextChanged(GTGUI::GUIElement &element);


            /// A pointer to the text editor that owns the text area.
            TextEditor* ownerTextEditor;


        private:    // No copying.
            TextAreaEventHandler(const TextAreaEventHandler &);
            TextAreaEventHandler & operator=(const TextAreaEventHandler &);

        }*textAreaEventHandler;


        /// The script compilation error handler that will be attached to the compilation script.
        struct ScriptCompilationErrorHandler : public GT::ScriptErrorHandler
        {
            /// Constructor.
            ScriptCompilationErrorHandler(TextEditor &ownerTextEditorIn)
                : ownerTextEditor(ownerTextEditorIn)
            {
            }


            /// ScriptErrorHandler::OnError()
            void OnError(GT::Script &, const char* message)
            {
                (void)message;
                //printf("%s\n", message);
            }

            /// ScriptErrorHandler::OnSyntaxError()
            void OnSyntaxError(GT::Script &script, int lineNumber, const char* message)
            {
                this->ownerTextEditor.OnScriptSyntaxError(script, lineNumber, message);
            }

            /// ScriptErrorHandler::OnRuntimeError()
            void OnRuntimeError(GT::Script &script, const char* sourceName, int lineNumber, const char* message, const Vector<GT::ScriptCallstackItem> &callstack)
            {
                (void)sourceName;
                (void)callstack;
                this->ownerTextEditor.OnScriptSyntaxError(script, lineNumber, message);
            }


            /// A reference to the text editor that owns this error handler.
            TextEditor &ownerTextEditor;

        public:     // No copying
            ScriptCompilationErrorHandler(const ScriptCompilationErrorHandler &);
            ScriptCompilationErrorHandler & operator=(const ScriptCompilationErrorHandler &);

        }compilationErrorHandler;


        /// When compiling a script, we need to use an actual script object. In order to avoid as many false-positive errors as
        /// possible, we need to register the engine's scripting library onto that script. To do this, we're going to create a
        /// Game object which will act as a proxy. We will not call Game::Startup().
        Game* proxyGame;

        /// The script to use for doing the compilation check. This will have the standard library registered to it.
        GT::Script* compilationScript;



        /// Keeps track of whether or not we are editting a script file.
        bool isScriptFile;


    private:    // No copying.
        TextEditor(const TextEditor &);
        TextEditor & operator=(const TextEditor &);
    };
}

#endif
