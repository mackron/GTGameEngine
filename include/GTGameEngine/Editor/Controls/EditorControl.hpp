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
        virtual void OnMove(GUIContext &context, HGUIElement hElement, unsigned int width, unsigned int height);

        /// @copydoc GUIEventHandler::OnSize()
        virtual void OnSize(GUIContext &context, HGUIElement hElement, int x, int y);

        /// @copydoc GUIEventHandler::OnMouseEnter()
        virtual void OnMouseEnter(GUIContext &context, HGUIElement hElement);

        /// @copydoc GUIEventHandler::OnMouseLeave()
        virtual void OnMouseLeave(GUIContext &context, HGUIElement hElement);

        /// @copydoc GUIEventHandler::OnMouseMove()
        virtual void OnMouseMove(GUIContext &context, HGUIElement hElement, int mousePosX, int mousePosY);

        /// @copydoc GUIEventHandler::OnMouseButtonPressed()
        virtual void OnMouseButtonPressed(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIEventHandler::OnMouseButtonReleased()
        virtual void OnMouseButtonReleased(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIEventHandler::OnMouseButtonDoubleClicked()
        virtual void OnMouseButtonDoubleClicked(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIEventHandler::OnSetMouseEventCapture()
        virtual void OnSetMouseEventCapture(GUIContext &context, HGUIElement hElement);

        /// @copydoc GUIEventHandler::OnReleasedMouseEventCapture()
        virtual void OnReleaseMouseEventCapture(GUIContext &context, HGUIElement hElement);



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
            void OnSize(GUIContext &context, HGUIElement hElement, unsigned int width, unsigned int height)                           { m_control.OnSize(context, hElement, width, height); }
            void OnMove(GUIContext &context, HGUIElement hElement, int x, int y)                                                      { m_control.OnMove(context, hElement, x, y); }
            void OnMouseEnter(GUIContext &context, HGUIElement hElement)                                                              { m_control.OnMouseEnter(context, hElement); }
            void OnMouseLeave(GUIContext &context, HGUIElement hElement)                                                              { m_control.OnMouseLeave(context, hElement); }
            void OnMouseMove(GUIContext &context, HGUIElement hElement, int mousePosX, int mousePosY)                                 { m_control.OnMouseMove(context, hElement, mousePosX, mousePosY); }
            void OnMouseButtonPressed(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY)       { m_control.OnMouseButtonPressed(context, hElement, mouseButton, mousePosX, mousePosY); }
            void OnMouseButtonReleased(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY)      { m_control.OnMouseButtonReleased(context, hElement, mouseButton, mousePosX, mousePosY); }
            void OnMouseButtonDoubleClicked(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY) { m_control.OnMouseButtonDoubleClicked(context, hElement, mouseButton, mousePosX, mousePosY); }
            void OnSetMouseEventCapture(GUIContext &context, HGUIElement hElement)                                                    { m_control.OnSetMouseEventCapture(context, hElement); }
            void OnReleaseMouseEventCapture(GUIContext &context, HGUIElement hElement)                                                { m_control.OnReleaseMouseEventCapture(context, hElement); }

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
