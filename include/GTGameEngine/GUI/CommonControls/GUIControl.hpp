// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_GUIControl
#define GT_GUIControl

#include "../GUIContext.hpp"
#include <easy_util/easy_util.h>    // For annotations and NO_COPY()

namespace GT
{
    class GUIControl
    {
    public:

        /// Constructor
        GUIControl(GUIContext &gui, HGUIElement hParent = 0);

        /// Destructor.
        virtual ~GUIControl();


        /// Retrieves a reference to the GUI object that owns this control and was passed into the constructor.
        GUIContext & GetGUIContext() const;

        /// Retrieves a handle to the root GUI element.
        HGUIElement GetRootElement() const;



        /// Cast operator for casting this control to the root GUI element.
        operator HGUIElement() const { return this->GetRootElement(); }

        

        //////////////////////////////////////////
        // Events

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



    protected:

        /// A reference to the GUI context that was passed to the constructor.
        GUIContext &m_gui;

        /// The root GUI element. This is the parent to all child controls.
        HGUIElement m_hRootElement;



    private:

        /// The GUI event handler.
        class ControlGUIEventHandler : public GUIEventHandler
        {
        public:
            ControlGUIEventHandler(GUIControl &control) : m_control(control) {}

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

            GUIControl &m_control;


            NO_COPY(ControlGUIEventHandler)
        } m_eventHandler;
    };
}

#endif
