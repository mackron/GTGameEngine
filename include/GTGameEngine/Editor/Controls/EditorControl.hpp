// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorControl
#define GT_EditorControl

#include "../../GUI/GUIContext.hpp"

namespace GT
{
    class Editor;

    /// Base class for editor controls.
    ///
    /// All event handling methods operate on the root GUI element.
    class EditorControl
    {
    public:

        /// Constructor.
        ///
        /// @remarks
        ///     This will create an empty element. It is up to the host to attach the element to a parent element.
        EditorControl(Editor &editor);

        /// Destructor.
        virtual ~EditorControl();


        /// Retrieves a reference to the editor.
        ///
        /// @return A reference to the editor.
              Editor & GetEditor();
        const Editor & GetEditor() const;

        /// Retrieves a reference to the GUI context.
        ///
        /// @return A reference to the GUI context that was used to construct the control.
              GUIContext & GetGUI();
        const GUIContext & GetGUI() const;

        /// Retrieves a handle to the root GUI element.
        ///
        /// @return A handle to the control's root GUI element.
        HGUIElement GetRootGUIElement() const;



        /// @copydoc GUIEventHandler::OnMove()
        virtual void OnMove(int x, int y);

        /// @copydoc GUIEventHandler::OnSize()
        virtual void OnSize(unsigned int width, unsigned int height);

        /// @copydoc GUIEventHandler::OnMouseEnter()
        virtual void OnMouseEnter();

        /// @copydoc GUIEventHandler::OnMouseLeave()
        virtual void OnMouseLeave();

        /// @copydoc GUIEventHandler::OnMouseMove()
        virtual void OnMouseMove(int mousePosX, int mousePosY);

        /// @copydoc GUIEventHandler::OnMouseButtonPressed()
        virtual void OnMouseButtonPressed(int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIEventHandler::OnMouseButtonReleased()
        virtual void OnMouseButtonReleased(int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIEventHandler::OnMouseButtonDoubleClicked()
        virtual void OnMouseButtonDoubleClicked(int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIEventHandler::OnSetMouseEventCapture()
        virtual void OnSetMouseEventCapture();

        /// @copydoc GUIEventHandler::OnReleasedMouseEventCapture()
        virtual void OnReleaseMouseEventCapture();



    private:

        /// A reference to the GUI context that owns the element making up the control.
        Editor &m_editor;

        /// A handle to the GUI element. This is created in the constructor.
        HGUIElement m_hRootElement;


        /// The GUI event handler.
        class ControlGUIEventHandler : public GUIEventHandler
        {
        public:
            ControlGUIEventHandler(EditorControl &control);

        private:
            void OnSize(GUIContext &, HGUIElement, unsigned int width, unsigned int height)                           { m_control.OnSize(width, height); }
            void OnMove(GUIContext &, HGUIElement, int x, int y)                                                      { m_control.OnMove(x, y); }
            void OnMouseEnter(GUIContext &, HGUIElement)                                                              { m_control.OnMouseEnter(); }
            void OnMouseLeave(GUIContext &, HGUIElement)                                                              { m_control.OnMouseLeave(); }
            void OnMouseMove(GUIContext &, HGUIElement, int mousePosX, int mousePosY)                                 { m_control.OnMouseMove(mousePosX, mousePosY); }
            void OnMouseButtonPressed(GUIContext &, HGUIElement, int mouseButton, int mousePosX, int mousePosY)       { m_control.OnMouseButtonPressed(mouseButton, mousePosX, mousePosY); }
            void OnMouseButtonReleased(GUIContext &, HGUIElement, int mouseButton, int mousePosX, int mousePosY)      { m_control.OnMouseButtonReleased(mouseButton, mousePosX, mousePosY); }
            void OnMouseButtonDoubleClicked(GUIContext &, HGUIElement, int mouseButton, int mousePosX, int mousePosY) { m_control.OnMouseButtonDoubleClicked(mouseButton, mousePosX, mousePosY); }
            void OnSetMouseEventCapture(GUIContext &, HGUIElement)                                                    { m_control.OnSetMouseEventCapture(); }
            void OnReleaseMouseEventCapture(GUIContext &, HGUIElement)                                                { m_control.OnReleaseMouseEventCapture(); }

            EditorControl &m_control;


            ControlGUIEventHandler(const ControlGUIEventHandler &);
            ControlGUIEventHandler & operator=(const ControlGUIEventHandler &);

        } m_eventHandler;



    private:    // No copying.
        EditorControl(const EditorControl &);
        EditorControl & operator=(const EditorControl &);
    };
}

#endif
