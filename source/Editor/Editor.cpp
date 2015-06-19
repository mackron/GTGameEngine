// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/Editor/EditorGUISurfaceAUXData.hpp>
#include <GTGameEngine/Editor/Controls/EditorHeaderControl.hpp>
#include <GTGameEngine/Editor/Controls/EditorBodyControl.hpp>
#include <GTGameEngine/Editor/Controls/EditorFooterControl.hpp>
#include <GTGameEngine/GameContext.hpp>

namespace GT
{
    Editor::Editor(GT::GameContext &gameContext)
        : m_gameContext(gameContext),
          m_guiRenderer(),
          m_guiFontManager(),
          m_gui(&m_guiRenderer, &m_guiFontManager),
          m_globalGUIEventHandler(),
          m_hMainWindow(NULL),
          m_windowSurfaceMap(),
          m_eventHandlers(),
          m_isOpen(false),
          m_pHeaderControl(nullptr),
          m_pBodyControl(nullptr),
          m_pFooterControl(nullptr)
    {
    }

    Editor::~Editor()
    {
    }


    bool Editor::Startup()
    {
        // We need to reuse the main game window. If we don't have at least one we'll have to return false. The main game window is
        // the window at index 0.
        unsigned int windowCount = m_gameContext.GetWindowCount();
        if (windowCount > 0)
        {
            m_hMainWindow = m_gameContext.GetWindowByIndex(0);
            if (m_hMainWindow != NULL)
            {
                // We need a GUI surface for the main game window.
                if (this->CreateWindowSurfaceAndElement(m_hMainWindow))
                {
                    // Attach the GUI event handler.
                    m_gui.AttachGlobalEventHandler(m_globalGUIEventHandler);


                    // Create the header control.
                    HGUIElement hMainWindowElement = this->GetWindowElement(m_hMainWindow);
                    if (hMainWindowElement != 0)
                    {
                        // Create the header control.
                        m_pHeaderControl = new EditorHeaderControl(m_gui);
                        m_gui.SetElementParent(m_pHeaderControl->GetRootGUIElement(), hMainWindowElement);

                        // Create the body control.
                        m_pBodyControl = new EditorBodyControl(m_gui);
                        m_gui.SetElementParent(m_pBodyControl->GetRootGUIElement(), hMainWindowElement);

                        // Create the footer control.
                        m_pFooterControl = new EditorFooterControl(m_gui);
                        m_gui.SetElementParent(m_pFooterControl->GetRootGUIElement(), hMainWindowElement);


                        return true;
                    }


                    // TESTING
                    //m_hTestElement = m_gui.CreateElement();
                    //m_gui.AttachElementToSurface(m_hTestElement, hMainSurface);
                    //m_gui.SetElementBackgroundColour(m_hTestElement, GTLib::Colour(0.25f, 0.25f, 0.25f, 1));
                    //m_gui.SetElementSizeRatio(m_hTestElement, 1.0f, 1.0f);
                    //m_gui.SetElementBorder(m_hTestElement, 8, GTLib::Colour(0.75f, 0.25f, 0.25f, 1.0f));
                    //m_gui.EnableElementChildHeightFlexing(m_hTestElement);
                    //m_gui.SetElementVerticalAlign(m_hTestElement, GT::VerticalAlign_Center);
                    //m_gui.SetElementHorizontalAlign(m_hTestElement, GT::HorizontalAlign_Left);


                    
                }
            }
        }

        return false;
    }

    void Editor::Shutdown()
    {
        // TODO: Delete all surfaces and elements.

        m_gui.DetachGlobalEventHandler(m_globalGUIEventHandler);


        // Remove all event handlers.
        m_eventHandlers.Clear();
    }


    bool Editor::Open()
    {
        if (!this->IsOpen())
        {
            // Every relevant window needs to be shown.
            if (m_hMainWindow != 0)
            {
                // If the window was already visible it needs to be redrawn.
                HGUISurface hSurface = this->GetWindowSurface(m_hMainWindow);
                if (hSurface != NULL)
                {
                    printf("Open\n");

                    unsigned int windowWidth;
                    unsigned int windowHeight;
                    if (m_gameContext.GetWindowSize(m_hMainWindow, windowWidth, windowHeight))
                    {
                        m_gui.SetSurfaceSize(hSurface, windowWidth, windowHeight);
                    }

                    m_gui.InvalidateSurfaceRect(hSurface);


                    // At this point the editor is now open, so post the event and set the flag.
                    this->PostEvent_OnEditorOpened();
                    m_isOpen = true;

                    return true;
                }
            }

            return false;
        }
        else
        {
            // The editor is already open.
            return true;
        }
    }

    void Editor::Close()
    {
        if (this->IsOpen())
        {
            // Every relevant windows needs to be hidden.
            printf("Closed\n");


            // At this point the editor is closed, so post the event and set the flag.
            this->PostEvent_OnEditorClosed();
            m_isOpen = false;
        }
    }

    bool Editor::IsOpen() const
    {
        return m_isOpen;
    }


    void Editor::Step(double deltaTimeInSeconds)
    {
        (void)deltaTimeInSeconds;
    }


    void Editor::AttachEventHandler(EditorEventHandler &eventHandler)
    {
        m_eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
        m_eventHandlers.PushBack(&eventHandler);
    }

    void Editor::DetachEventHandler(EditorEventHandler &eventHandler)
    {
        m_eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
    }


    ////////////////////////////////////
    // Inbound Events

    bool Editor::OnWantToClose()
    {
        return true;
    }

    void Editor::OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.SetSurfaceSize(hSurface, width, height);
        }
    }

    void Editor::OnWindowMoved(HWindow hWindow, int xPos, int yPos)
    {
#if 0
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
        }
#else
        (void)hWindow;
        (void)xPos;
        (void)yPos;
#endif
    }

    void Editor::OnMouseMove(HWindow hWindow, int xPos, int yPos)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.OnMouseMove(hSurface, xPos, yPos);
        }
    }

    void Editor::OnMouseEnter(HWindow hWindow)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.OnMouseEnter(hSurface);
        }
    }

    void Editor::OnMouseLeave(HWindow hWindow)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.OnMouseLeave(hSurface);
        }
    }

    void Editor::OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            //m_gui.OnMouseButtonPressed(hSurface, button, xPos, yPos);
        }
    }

    void Editor::OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            //m_gui.OnMouseButtonReleased(hSurface, button, xPos, yPos);
        }
    }

    void Editor::OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            //m_gui.OnMouseButtonDoubleClicked(hSurface, button, xPos, yPos);
        }
    }

    void Editor::OnKeyPressed(HWindow hWindow, GTLib::Key key)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            if (key == GTLib::Keys::Tab)
            {
                if (m_gameContext.IsKeyDown(GTLib::Keys::Shift))
                {
                    if (this->IsOpen())
                    {
                        this->Close();
                    }
                    else
                    {
                        this->Open();
                    }
                }
            }

            //m_gui.OnKeyPressed(key);
        }
    }

    void Editor::OnKeyReleased(HWindow hWindow, GTLib::Key key)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            //m_gui.OnKeyReleased(key);
        }
    }

    void Editor::OnPrintableKeyDown(HWindow hWindow, char32_t character)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            //m_gui.OnPrintableKeyDown(character);
        }
    }

    void Editor::OnPaintWindow(HWindow hWindow, const GTLib::Rect<int> &rect)
    {
        HGUISurface hSurface = this->GetWindowSurface(hWindow);
        if (hSurface != 0)
        {
            m_gui.PaintSurface(hSurface, rect, nullptr);
        }
    }



    //////////////////////////////////////////////
    // Private

    bool Editor::GetWindowSurfaceAndElement(HWindow hWindow, WindowGUISurfaceAndElement &surfaceAndElementOut) const
    {
        auto iSurface = m_windowSurfaceMap.Find(hWindow);
        if (iSurface != nullptr)
        {
            surfaceAndElementOut = iSurface->value;
            return false;
        }
        else
        {
            return false;
        }
    }

    bool Editor::CreateWindowSurfaceAndElement(HWindow hWindow, WindowGUISurfaceAndElement &surfaceAndElementOut)
    {
        if (hWindow != NULL)
        {
            // If the window already has a surface and element, return immediately.
            if (this->GetWindowSurfaceAndElement(hWindow, surfaceAndElementOut))
            {
                return true;
            }


            // If we get here the surface does not already exist.
            unsigned int windowWidth;
            unsigned int windowHeight;
            if (m_gameContext.GetWindowSize(hWindow, windowWidth, windowHeight))
            {
                HGUISurface hSurface = m_gui.CreateSurface();
                if (hSurface != NULL)
                {
                    m_gui.SetSurfaceSize(hSurface, windowWidth, windowHeight);
                    m_gui.SetSurfaceAuxData(hSurface, new EditorGUISurfaceAUXData(hWindow));

                    HGUIElement hElement = m_gui.CreateElement();
                    if (hElement != NULL)
                    {
                        m_gui.AttachElementToSurface(hElement, hSurface);
                        m_gui.SetElementSizeRatio(hElement, 1.0f, 1.0f);
                        m_gui.SetElementBackgroundColour(hElement, GTLib::Colour(0.25f, 0.25f, 0.25f, 1));
                        m_gui.EnableElementChildWidthFlexing(hElement);
                        m_gui.EnableElementChildHeightFlexing(hElement);
                        
                        //m_gui.SetElementBorder(hElement, 8, GTLib::Colour(0.75f, 0.25f, 0.25f, 1.0f));
                        


                        surfaceAndElementOut.hSurface = hSurface;
                        surfaceAndElementOut.hElement = hElement;

                        // The window needs to be mapped to the surface.
                        m_windowSurfaceMap.Add(hWindow, surfaceAndElementOut);

                        return true;
                    }
                }
            }
        }
        
        return false;
    }

    bool Editor::CreateWindowSurfaceAndElement(HWindow hWindow)
    {
        WindowGUISurfaceAndElement unused;
        return this->CreateWindowSurfaceAndElement(hWindow, unused);
    }


    void Editor::DeleteWindowSurfaceAndElement(HWindow hWindow)
    {
        WindowGUISurfaceAndElement surfaceAndElement;
        if (this->GetWindowSurfaceAndElement(hWindow, surfaceAndElement))
        {
            // TODO: Delete the elements that are attached to the surface.

            delete m_gui.GetSurfaceAuxData<EditorGUISurfaceAUXData>(surfaceAndElement.hSurface);
            m_gui.DeleteSurface(surfaceAndElement.hSurface);
            m_gui.DeleteElement(surfaceAndElement.hElement);


            // Remove the window/surface mapping.
            m_windowSurfaceMap.RemoveByKey(hWindow);
        }
    }


    HGUISurface Editor::GetWindowSurface(HWindow hWindow) const
    {
        auto iSurface = m_windowSurfaceMap.Find(hWindow);
        if (iSurface != nullptr)
        {
            return iSurface->value.hSurface;
        }
        else
        {
            return NULL;
        }
    }

    HGUISurface Editor::GetWindowElement(HWindow hWindow) const
    {
        auto iSurface = m_windowSurfaceMap.Find(hWindow);
        if (iSurface != nullptr)
        {
            return iSurface->value.hElement;
        }
        else
        {
            return NULL;
        }
    }


    void Editor::PostEvent_OnEditorOpened()
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnEditorOpened();
            }
        }
    }

    void Editor::PostEvent_OnEditorClosed()
    {
        for (size_t iEventHandler = 0; iEventHandler < m_eventHandlers.GetCount(); ++iEventHandler)
        {
            auto pEventHandler = m_eventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                pEventHandler->OnEditorClosed();
            }
        }
    }
}
