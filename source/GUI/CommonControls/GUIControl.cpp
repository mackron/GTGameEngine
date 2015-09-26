// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/CommonControls/GUIControl.hpp>

namespace GT
{
    GUIControl::GUIControl(GUIContext &gui, HGUIElement hParent)
        : m_gui(gui), m_hRootElement(0), m_eventHandler(*this)
    {
        m_hRootElement = gui.CreateElement();
        if (hParent != 0) {
            gui.SetElementParent(m_hRootElement, hParent);
        }


        // Attach the event handler.
        gui.AttachLocalEventHandler(m_hRootElement, m_eventHandler);
    }

    GUIControl::~GUIControl()
    {
        m_gui.DeleteElement(m_hRootElement);
    }


    GUIContext & GUIControl::GetGUIContext() const
    {
        return m_gui;
    }

    HGUIElement GUIControl::GetRootElement() const
    {
        return m_hRootElement;
    }


    /////////////////////////////////////////
    // Events

    void GUIControl::OnMove(int, int)
    {
    }
    void GUIControl::OnSize(unsigned int, unsigned int)
    {
    }
    void GUIControl::OnMouseEnter()
    {
    }
    void GUIControl::OnMouseLeave()
    {
    }
    void GUIControl::OnMouseMove(int, int)
    {
    }
    void GUIControl::OnMouseButtonPressed(int, int, int)
    {
    }
    void GUIControl::OnMouseButtonReleased(int, int, int)
    {
    }
    void GUIControl::OnMouseButtonDoubleClicked(int, int, int)
    {
    }
    void GUIControl::OnSetMouseEventCapture()
    {
    }
    void GUIControl::OnReleaseMouseEventCapture()
    {
    }
}
