// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUIContext.hpp>

namespace GT
{
    GUIContext::GUIContext(GUIRenderer* renderer, GUIFontManager* pFontManager, GUIResourceManager* pResourceManager)
        : GUIContextBase(pFontManager, pResourceManager),
          m_elementHandles(),
          m_surfaceHandles(),
          m_renderer(renderer),
          m_ownsRenderer(false)
#if defined(GT_GUI_DEBUGGING)
        , m_layoutLogger()
#endif
    {
    }

    GUIContext::~GUIContext()
    {
        // At the point of destruction we are just doing pure cleanup. We don't care about posting events or anything here.



        // Delete the elements ans surfaces last, just in case something else is referencing them, which shouldn't actually happen.
        m_elementHandles.IterateAssociatedObjects([&](GUIElement* element) -> bool
        {
            delete element;
            return true;
        });
        m_surfaceHandles.IterateAssociatedObjects([&](GUISurface* surface) -> bool
        {
            delete surface;
            return true;
        });


        // Delete the renderer last. This will clean up the render buffers.
        if (m_ownsRenderer)
        {
            delete m_renderer;
        }
    }


    /////////////////////////////////////////////////////////////////
    // Virtual Method Implementations

    GUISurface* GUIContext::CreateSurfacePtr()
    {
        HGUISurface hSurface = m_surfaceHandles.CreateHandle();
        if (hSurface != 0)
        {
            auto pSurface = new GUISurfaceWithHandle(hSurface);
            m_surfaceHandles.AssociateObjectWithHandle(hSurface, pSurface);

            return pSurface;
        }

        return nullptr;
    }

    void GUIContext::DeleteSurfacePtr(GUISurface* pSurface)
    {
        auto pSurfaceWithHandle = reinterpret_cast<GUISurfaceWithHandle*>(pSurface);
        if (pSurfaceWithHandle != nullptr)
        {
            if (pSurfaceWithHandle == this->GetSurfacePtr(pSurfaceWithHandle->handle))      // <-- This guards against cases when the handle is invalid.
            {
                m_surfaceHandles.DeleteHandle(pSurfaceWithHandle->handle);
                delete pSurfaceWithHandle;
            }
        }
    }


    GUIElement* GUIContext::CreateElementPtr()
    {
        HGUIElement hElement = m_elementHandles.CreateHandle();
        if (hElement != 0)
        {
            auto pElement = new GUIElementWithHandle(hElement);
            m_elementHandles.AssociateObjectWithHandle(hElement, pElement);

            return pElement;
        }

        return nullptr;
    }

    void GUIContext::DeleteElementPtr(GUIElement* pElement)
    {
        if (pElement != nullptr)
        {
            auto pElementWithHandle = reinterpret_cast<GUIElementWithHandle*>(pElement);
            if (pElementWithHandle != nullptr)
            {
                if (pElementWithHandle == this->GetElementPtr(pElementWithHandle->handle))
                {
                    m_elementHandles.DeleteHandle(pElementWithHandle->handle);

                    delete pElement->pCallbackEventHandlers;
                    delete pElement;
                }
            }
        }
    }


    void GUIContext::IterateSurfaces(std::function<bool (GUISurface* pSurface)> handler) const
    {
        m_surfaceHandles.IterateAssociatedObjects(handler);
    }

    void GUIContext::IterateElements(std::function<bool (GUIElement* pElement)> handler) const
    {
        m_elementHandles.IterateAssociatedObjects(handler);
    }


    void GUIContext::PostEvent_OnSize(GUIElement* pElement, unsigned int width, unsigned int height)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnSize(*this, hElement, width, height);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnSize;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](width, height);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnSize(*this, hElement, width, height);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnSize;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, width, height);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnMove(GUIElement* pElement, int x, int y)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMove(*this, hElement, x, y);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnMove;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](x, y);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMove(*this, hElement, x, y);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnMove;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, x, y);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnSizeAndOnMove(GUIElement* pElement, unsigned int width, unsigned height, int x, int y)
    {
        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;

        auto pElementAtStart = pElement;
        if (pElementAtStart != nullptr)
        {
            this->PostEvent_OnSize(pElement, width, height);

            if (this->GetElementPtr(hElement) == pElementAtStart)       // <-- This check is to guard against cases when the element was deleted during OnSize().
            {
                this->PostEvent_OnMove(pElement, x, y);
            }
        }
    }

    void GUIContext::PostEvent_OnMouseEnter(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseEnter(*this, hElement);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnMouseEnter;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i]();
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseEnter(*this, hElement);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnMouseEnter;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnMouseLeave(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseLeave(*this, hElement);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnMouseLeave;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i]();
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseLeave(*this, hElement);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnMouseLeave;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnMouseMove(GUIElement* pElement, int mousePosX, int mousePosY)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseMove(*this, hElement, mousePosX, mousePosY);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnMouseMove;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](mousePosX, mousePosY);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseMove(*this, hElement, mousePosX, mousePosY);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnMouseMove;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, mousePosX, mousePosY);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnMouseButtonPressed(GUIElement* pElement, int mouseButton, int mousePosX, int mousePosY)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseButtonPressed(*this, hElement, mouseButton, mousePosX, mousePosY);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnMouseButtonPressed;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](mouseButton, mousePosX, mousePosY);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseButtonPressed(*this, hElement, mouseButton, mousePosX, mousePosY);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnMouseButtonPressed;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, mouseButton, mousePosX, mousePosY);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnMouseButtonReleased(GUIElement* pElement, int mouseButton, int mousePosX, int mousePosY)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseButtonReleased(*this, hElement, mouseButton, mousePosX, mousePosY);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnMouseButtonReleased;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](mouseButton, mousePosX, mousePosY);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseButtonReleased(*this, hElement, mouseButton, mousePosX, mousePosY);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnMouseButtonReleased;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, mouseButton, mousePosX, mousePosY);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnMouseButtonDoubleClicked(GUIElement* pElement, int mouseButton, int mousePosX, int mousePosY)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseButtonDoubleClicked(*this, hElement, mouseButton, mousePosX, mousePosY);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnMouseButtonDoubleClicked;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](mouseButton, mousePosX, mousePosY);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseButtonDoubleClicked(*this, hElement, mouseButton, mousePosX, mousePosY);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnMouseButtonDoubleClicked;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, mouseButton, mousePosX, mousePosY);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnMouseWheel(GUIElement* pElement, int delta, int mousePosX, int mousePosY)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseWheel(*this, hElement, delta, mousePosX, mousePosY);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnMouseWheel;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](delta, mousePosX, mousePosY);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseWheel(*this, hElement, delta, mousePosX, mousePosY);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnMouseWheel;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, delta, mousePosX, mousePosY);
                }
                else
                {
                    break;
                }
            }
        }
    }


    void GUIContext::PostEvent_OnSetMouseEventCapture(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnSetMouseEventCapture(*this, hElement);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnSetMouseEventCapture;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i]();
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnSetMouseEventCapture(*this, hElement);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnSetMouseEventCapture;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnReleaseMouseEventCapture(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnReleaseMouseEventCapture(*this, hElement);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnReleaseMouseEventCapture;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i]();
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnReleaseMouseEventCapture(*this, hElement);
                    return true;
                }

                return false;
            });


            auto handlers = m_callbackGlobalEventHandlers.OnReleaseMouseEventCapture;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement);
                }
                else
                {
                    break;
                }
            }
        }
    }


    void GUIContext::PostEvent_OnKeyPressed(GUIElement* pElement, GTLib::Key key)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnKeyPressed(*this, hElement, key);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnKeyPressed;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](key);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnKeyPressed(*this, hElement, key);
                    return true;
                }

                return false;
            });


            auto handlers = m_callbackGlobalEventHandlers.OnKeyPressed;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, key);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnKeyPressedAutoRepeat(GUIElement* pElement, GTLib::Key key)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnKeyPressedAutoRepeat(*this, hElement, key);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnKeyPressedAutoRepeat;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](key);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnKeyPressedAutoRepeat(*this, hElement, key);
                    return true;
                }

                return false;
            });


            auto handlers = m_callbackGlobalEventHandlers.OnKeyPressedAutoRepeat;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, key);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnKeyReleased(GUIElement* pElement, GTLib::Key key)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnKeyReleased(*this, hElement, key);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnKeyReleased;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](key);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnKeyReleased(*this, hElement, key);
                    return true;
                }

                return false;
            });


            auto handlers = m_callbackGlobalEventHandlers.OnKeyReleased;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, key);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnPrintableKeyDown(GUIElement* pElement, char32_t character)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnPrintableKeyDown(*this, hElement, character);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnPrintableKeyDown;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i](character);
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnPrintableKeyDown(*this, hElement, character);
                    return true;
                }

                return false;
            });


            auto handlers = m_callbackGlobalEventHandlers.OnPrintableKeyDown;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement, character);
                }
                else
                {
                    break;
                }
            }
        }
    }


    void GUIContext::PostEvent_OnTextChanged(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnTextChanged(*this, hElement);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnTextChanged;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i]();
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnTextChanged(*this, hElement);
                    return true;
                }

                return false;
            });


            auto handlers = m_callbackGlobalEventHandlers.OnTextChanged;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement);
                }
                else
                {
                    break;
                }
            }
        }
    }


    void GUIContext::PostEvent_OnPaint(GUIElement* pElement)
    {
        if (pElement != nullptr && pElement->pCallbackEventHandlers != nullptr)
        {
            for (size_t i = 0; i < pElement->pCallbackEventHandlers->OnPaint.GetCount(); ++i)
            {
                pElement->pCallbackEventHandlers->OnPaint[i](m_renderer);
            }
        }
    }


    void GUIContext::PostEvent_OnReceiveKeyboardFocus(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnReceiveKeyboardFocus(*this, hElement);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnReceiveKeyboardFocus;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i]();
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnReceiveKeyboardFocus(*this, hElement);
                    return true;
                }

                return false;
            });


            auto handlers = m_callbackGlobalEventHandlers.OnReceiveKeyboardFocus;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnLoseKeyboardFocus(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        HGUIElement hElement = reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        assert(hElement != 0);
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnLoseKeyboardFocus(*this, hElement);
                return true;
            });

            if (this->GetElementPtr(hElement) == pElement && pElement->pCallbackEventHandlers)
            {
                auto handlers = pElement->pCallbackEventHandlers->OnLoseKeyboardFocus;
                for (size_t i = 0; i < handlers.GetCount(); ++i)
                {
                    if (pElement == this->GetElementPtr(hElement))
                    {
                        handlers[i]();
                    }
                    else
                    {
                        break;
                    }
                }
            }


            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    eventHandler.OnLoseKeyboardFocus(*this, hElement);
                    return true;
                }

                return false;
            });


            auto handlers = m_callbackGlobalEventHandlers.OnLoseKeyboardFocus;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pElement == this->GetElementPtr(hElement))
                {
                    handlers[i](hElement);
                }
                else
                {
                    break;
                }
            }
        }
    }


    void GUIContext::PostEvent_OnSurfaceNeedsRepaint(GUISurface* pSurface, const GTLib::Rect<int> &rect)
    {
        assert(pSurface != nullptr);

        HGUIElement hSurface = reinterpret_cast<GUISurfaceWithHandle*>(pSurface)->handle;
        assert(hSurface != 0);
        {
            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (pSurface == this->GetSurfacePtr(hSurface))
                {
                    eventHandler.OnSurfaceNeedsRepaint(*this, hSurface, rect);
                    return true;
                }

                return false;
            });

            auto handlers = m_callbackGlobalEventHandlers.OnSurfaceNeedsRepaint;
            for (size_t i = 0; i < handlers.GetCount(); ++i)
            {
                if (pSurface == this->GetSurfacePtr(hSurface))
                {
                    handlers[i](hSurface, rect);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void GUIContext::PostEvent_OnCursorNeedsToChange(GUISystemCursor cursor)
    {
        // Global
        this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
        {
            eventHandler.OnCursorNeedsToChange(*this, cursor);
            return true;
        });

        auto handlers = m_callbackGlobalEventHandlers.OnCursorNeedsToChange;
        for (size_t i = 0; i < handlers.GetCount(); ++i)
        {
            handlers[i](cursor);
        }
    }


    void GUIContext::Renderer_BeginPaintSurface(GUISurface* pSurface, void* pInputData)
    {
        assert(m_renderer != nullptr);
        m_renderer->BeginPaintSurface(*this, reinterpret_cast<GUISurfaceWithHandle*>(pSurface)->handle, pInputData);
    }

    void GUIContext::Renderer_EndPaintSurface()
    {
        assert(m_renderer != nullptr);
        m_renderer->EndPaintSurface(*this);
    }

    void GUIContext::Renderer_Clear(const GTLib::Rect<int> &rect)
    {
        assert(m_renderer != nullptr);
        m_renderer->Clear(*this, rect);
    }

    void GUIContext::Renderer_DrawRectangle(GTLib::Rect<int> rect, GTLib::Colour color)
    {
        assert(m_renderer != nullptr);

        if (color.a > 0)
        {
            if (rect.left < rect.right && rect.top < rect.bottom)
            {
                m_renderer->DrawRectangle(*this, rect, color);
            }
        }
    }

    void GUIContext::Renderer_SetClippingRect(GTLib::Rect<int> clippingRect)
    {
        assert(m_renderer != nullptr);
        m_renderer->SetClippingRect(*this, clippingRect);
    }

    bool GUIContext::Renderer_CanDrawText(HGUIFont hFont)
    {
        assert(m_renderer != nullptr);
        return m_renderer->CanDrawText(*this, hFont);
    }

    void GUIContext::Renderer_DrawText(const GUITextRunDesc &textRunDesc)
    {
        assert(m_renderer != nullptr);
        m_renderer->DrawText(*this, textRunDesc);
    }

    void GUIContext::Renderer_DrawRawImage(int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent)
    {
        assert(m_renderer != nullptr);
        m_renderer->DrawRawImage(*this, xPos, yPos, width, height, pImageData, isTransparent);
    }

    void GUIContext::Renderer_InitializeImage(HGUIImage hImage, unsigned int width, unsigned int height, GUIImageFormat format, const void* pData)
    {
        assert(m_renderer != nullptr);
        m_renderer->InitializeImage(*this, hImage, width, height, format, pData);
    }

    void GUIContext::Renderer_UninitializeImage(HGUIImage hImage)
    {
        assert(m_renderer != nullptr);
        m_renderer->UninitializeImage(*this, hImage);
    }

    void GUIContext::Renderer_DrawTexturedRectangle(GTLib::Rect<int> rect, HGUIImage hImage, GTLib::Colour color, unsigned int subImageOffsetX, unsigned int subImageOffsetY, unsigned int subImageWidth, unsigned int subImageHeight)
    {
        assert(m_renderer != nullptr);

        if (color.a > 0)
        {
            if (rect.left < rect.right && rect.top < rect.bottom)
            {
                m_renderer->DrawTexturedRectangle(*this, rect, hImage, color, subImageOffsetX, subImageOffsetY, subImageWidth, subImageHeight);
            }
        }
    }




    ////////////////////////////////////////////////////////////////
    // Surfaces

    HGUISurface GUIContext::CreateSurface()
    {
        auto pSurface = GUIContextBase::CreateSurface();
        if (pSurface != nullptr)
        {
            return reinterpret_cast<GUISurfaceWithHandle*>(pSurface)->handle;
        }
        else
        {
            return 0;
        }
    }

    void GUIContext::DeleteSurface(HGUISurface hSurface)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::DeleteSurface(pSurface);
        }
    }

    HGUISurface GUIContext::FindSurfaceByID(const char* id) const
    {
        auto pSurface = GUIContextBase::FindSurfaceByID(id);
        if (pSurface != nullptr)
        {
            return reinterpret_cast<GUISurfaceWithHandle*>(pSurface)->handle;
        }
        else
        {
            return 0;
        }
    }

    void GUIContext::SetSurfaceID(HGUISurface hSurface, const char* id)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::SetSurfaceID(pSurface, id);
        }
    }

    const char* GUIContext::GetSurfaceID(HGUISurface hSurface) const
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            return GUIContextBase::GetSurfaceID(pSurface);
        }
        else
        {
            return nullptr;
        }
    }


    void GUIContext::SetSurfaceSize(HGUISurface hSurface, unsigned int newWidth, unsigned int newHeight)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::SetSurfaceSize(pSurface, newWidth, newHeight);
        }
    }

    void GUIContext::GetSurfaceSize(HGUISurface hSurface, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::GetSurfaceSize(pSurface, widthOut, heightOut);
        }
        else
        {
            widthOut  = 0U;
            heightOut = 0U;
        }
    }

    bool GUIContext::DoesSurfaceContainGUIElement(HGUISurface hSurface, HGUIElement hElement) const
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            auto pElement = this->GetElementPtr(hElement);
            if (pElement != nullptr)
            {
                return GUIContextBase::DoesSurfaceContainGUIElement(pSurface, pElement);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    HGUISurface GUIContext::GetSurfaceUnderMouse() const
    {
        auto pSurface = GUIContextBase::GetSurfaceUnderMouse();
        if (pSurface != nullptr)
        {
            return reinterpret_cast<GUISurfaceWithHandle*>(pSurface)->handle;
        }

        return 0;
    }



    ////////////////////////////////////////////////////////////////
    // GUI Elements

    HGUIElement GUIContext::CreateElement()
    {
        auto pElement = GUIContextBase::CreateElement();
        if (pElement != nullptr)
        {
            return reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        }
        else
        {
            return 0;
        }
    }

    void GUIContext::DeleteElement(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DeleteElement(pElement);
        }
    }

    void GUIContext::SetElementID(HGUIElement hElement, const char* id)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementID(pElement, id);
        }
    }

    const char* GUIContext::GetElementID(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementID(pElement);
        }
        else
        {
            return nullptr;
        }
    }

    HGUIElement GUIContext::FindElementByID(const char* elementID) const
    {
        auto pElement = GUIContextBase::FindElementByID(elementID);
        if (pElement != nullptr)
        {
            return reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        }
        else
        {
            return 0;
        }
    }


    HGUIElement GUIContext::GetElementParent(HGUIElement hChildElement) const
    {
        auto pChildElement = this->GetElementPtr(hChildElement);
        if (pChildElement != nullptr)
        {
            auto pParentElement = GUIContextBase::GetElementParent(pChildElement);
            if (pParentElement != nullptr)
            {
                return reinterpret_cast<GUIElementWithHandle*>(pParentElement)->handle;
            }
        }

        return 0;
    }

    void GUIContext::SetElementParent(HGUIElement hChildElement, HGUIElement hParentElement)
    {
        auto pChildElement = this->GetElementPtr(hChildElement);
        if (pChildElement != nullptr)
        {
            auto pParentElement = this->GetElementPtr(hParentElement);
            if (pParentElement != nullptr)
            {
                GUIContextBase::SetElementParent(pChildElement, pParentElement);
            }
        }
    }

    void GUIContext::DetachElementFromParent(HGUIElement hChildElement)
    {
        auto pChildElement = this->GetElementPtr(hChildElement);
        if (pChildElement != nullptr)
        {
            GUIContextBase::DetachElementFromParent(pChildElement);
        }
    }

    void GUIContext::DetachElementFromParentAndSiblings(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DetachElementFromParentAndSiblings(pElement);
        }
    }

    void GUIContext::AppendChildElement(HGUIElement hParentElement, HGUIElement hChildElement)
    {
        auto pParentElement = this->GetElementPtr(hParentElement);
        if (pParentElement != nullptr)
        {
            auto pChildElement = this->GetElementPtr(hChildElement);
            if (pChildElement != nullptr)
            {
                GUIContextBase::AppendChildElement(pParentElement, pChildElement);
            }
        }
    }

    void GUIContext::PrependChildElement(HGUIElement hParentElement, HGUIElement hChildElement)
    {
        auto pParentElement = this->GetElementPtr(hParentElement);
        if (pParentElement != nullptr)
        {
            auto pChildElement = this->GetElementPtr(hChildElement);
            if (pChildElement != nullptr)
            {
                GUIContextBase::PrependChildElement(pParentElement, pChildElement);
            }
        }
    }

    void GUIContext::AppendSiblingElement(HGUIElement hElementToAppendTo, HGUIElement hElementToAppend)
    {
        auto pElementToAppendTo = this->GetElementPtr(hElementToAppendTo);
        if (pElementToAppendTo != nullptr)
        {
            auto pElementToAppend = this->GetElementPtr(hElementToAppend);
            if (pElementToAppend != nullptr)
            {
                GUIContextBase::AppendSiblingElement(pElementToAppendTo, pElementToAppend);
            }
        }
    }

    void GUIContext::PrependSiblingElement(HGUIElement hElementToPrependTo, HGUIElement hElementToPrepend)
    {
        auto pElementToPrependTo = this->GetElementPtr(hElementToPrependTo);
        if (pElementToPrependTo != nullptr)
        {
            auto pElementToPrepend = this->GetElementPtr(hElementToPrepend);
            if (pElementToPrepend != nullptr)
            {
                GUIContextBase::PrependSiblingElement(pElementToPrependTo, pElementToPrepend);
            }
        }
    }


    HGUIElement GUIContext::GetElementNextSibling(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            auto pNextSibling = GUIContextBase::GetElementNextSibling(pElement);
            if (pNextSibling != nullptr)
            {
                return reinterpret_cast<GUIElementWithHandle*>(pNextSibling)->handle;
            }
        }

        return 0;
    }

    HGUIElement GUIContext::GetElementPreviousSibling(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            auto pPrevSibling = GUIContextBase::GetElementPreviousSibling(pElement);
            if (pPrevSibling != nullptr)
            {
                return reinterpret_cast<GUIElementWithHandle*>(pPrevSibling)->handle;
            }
        }

        return 0;
    }


    bool GUIContext::IterateElementPrevSiblings(HGUIElement hElement, std::function<bool (HGUIElement)> handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IterateElementPrevSiblings(pElement, [&](GUIElement* pSibling) -> bool
            {
                return handler(reinterpret_cast<GUIElementWithHandle*>(pSibling)->handle);
            });
        }

        return true;
    }

    bool GUIContext::IterateElementPrevSiblingsReverse(HGUIElement hElement, std::function<bool (HGUIElement)> handler) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IterateElementPrevSiblingsReverse(pElement, [&](GUIElement* pSibling) -> bool
            {
                return handler(reinterpret_cast<GUIElementWithHandle*>(pSibling)->handle);
            });
        }

        return true;
    }

    bool GUIContext::IterateElementNextSiblings(HGUIElement hElement, std::function<bool (HGUIElement)> handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IterateElementNextSiblings(pElement, [&](GUIElement* pSibling) -> bool
            {
                return handler(reinterpret_cast<GUIElementWithHandle*>(pSibling)->handle);
            });
        }

        return true;
    }

    bool GUIContext::IterateElementNextSiblingsReverse(HGUIElement hElement, std::function<bool (HGUIElement)> handler) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IterateElementNextSiblingsReverse(pElement, [&](GUIElement* pSibling) -> bool
            {
                return handler(reinterpret_cast<GUIElementWithHandle*>(pSibling)->handle);
            });
        }

        return true;
    }

    bool GUIContext::IterateElementSiblings(HGUIElement hElement, std::function<bool (HGUIElement)> handler) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IterateElementSiblings(pElement, [&](GUIElement* pSibling) -> bool
            {
                return handler(reinterpret_cast<GUIElementWithHandle*>(pSibling)->handle);
            });
        }

        return true;
    }

    bool GUIContext::IterateElementSiblingsAndSelf(HGUIElement hElement, std::function<bool (HGUIElement)> handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
            {
                return handler(reinterpret_cast<GUIElementWithHandle*>(pSibling)->handle);
            });
        }

        return true;
    }


    bool GUIContext::IsElementChild(HGUIElement hParentElement, HGUIElement hChildElement) const
    {
        auto pParentElement = this->GetElementPtr(hParentElement);
        if (pParentElement != nullptr)
        {
            auto pChildElement = this->GetElementPtr(hChildElement);
            if (pChildElement != nullptr)
            {
                return GUIContextBase::IsElementChild(pParentElement, pChildElement);
            }
        }

        return false;
    }

    bool GUIContext::IsElementParent(HGUIElement hChildElement, HGUIElement hParentElement) const
    {
        auto pChildElement = this->GetElementPtr(hChildElement);
        if (pChildElement != nullptr)
        {
            auto pParentElement = this->GetElementPtr(hParentElement);
            if (pParentElement != nullptr)
            {
                return GUIContextBase::IsElementParent(pChildElement, pParentElement);
            }
        }

        return false;
    }

    bool GUIContext::IsElementDescendant(HGUIElement hParentElement, HGUIElement hChildElement) const
    {
        auto pParentElement = this->GetElementPtr(hParentElement);
        if (pParentElement != nullptr)
        {
            auto pChildElement = this->GetElementPtr(hChildElement);
            if (pChildElement != nullptr)
            {
                return GUIContextBase::IsElementDescendant(pParentElement, pChildElement);
            }
        }

        return false;
    }

    bool GUIContext::IsElementAncestor(HGUIElement hChildElement, HGUIElement hParentElement) const
    {
        auto pChildElement = this->GetElementPtr(hChildElement);
        if (pChildElement != nullptr)
        {
            auto pParentElement = this->GetElementPtr(hParentElement);
            if (pParentElement != nullptr)
            {
                return GUIContextBase::IsElementAncestor(pChildElement, pParentElement);
            }
        }

        return false;
    }


    void GUIContext::SetElementPosition(HGUIElement hElement, float xPos, float yPos)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementPosition(pElement, xPos, yPos);
        }
    }

    void GUIContext::SetElementPosition(HGUIElement hElement, int xPos, int yPos)
    {
        this->SetElementPosition(hElement, static_cast<float>(xPos), static_cast<float>(yPos));
    }


    void GUIContext::SetElementLeftPosition(HGUIElement hElement, int left)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementLeftPosition(pElement, left);
        }
    }

    void GUIContext::SetElementTopPosition(HGUIElement hElement, int top)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementTopPosition(pElement, top);
        }
    }

    void GUIContext::SetElementRightPosition(HGUIElement hElement, int right)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementRightPosition(pElement, right);
        }
    }

    void GUIContext::SetElementBottomPosition(HGUIElement hElement, int bottom)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBottomPosition(pElement, bottom);
        }
    }


    void GUIContext::SetElementRelativePositionOrigin(HGUIElement hElement, PositionOrigin origin)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementRelativePositionOrigin(pElement, origin);
        }
    }

    PositionOrigin GUIContext::GetElementRelativePositionOrigin(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementRelativePositionOrigin(pElement);
        }
        else
        {
            return PositionOrigin_Inner;
        }
    }


    void GUIContext::GetElementPosition(HGUIElement hElement, float &xPosOut, float &yPosOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementPosition(pElement, xPosOut, yPosOut);
        }
        else
        {
            xPosOut = 0.0f;
            yPosOut = 0.0f;
        }
    }

    void GUIContext::GetElementPosition(HGUIElement hElement, int &xPosOut, int &yPosOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementPosition(pElement, xPosOut, yPosOut);
        }
        else
        {
            xPosOut = 0;
            yPosOut = 0;
        }
    }

    void GUIContext::GetElementAbsolutePosition(HGUIElement hElement, float &xPosOut, float &yPosOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementAbsolutePosition(pElement, xPosOut, yPosOut);
        }
        else
        {
            xPosOut = 0.0f;
            yPosOut = 0.0f;
        }
    }

    void GUIContext::GetElementAbsolutePosition(HGUIElement hElement, int &xPosOut, int &yPosOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementAbsolutePosition(pElement, xPosOut, yPosOut);
        }
        else
        {
            xPosOut = 0;
            yPosOut = 0;
        }
    }

    void GUIContext::SetElementSize(HGUIElement hElement, float width, float height)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementSize(pElement, width, height);
        }
    }
    void GUIContext::SetElementSize(HGUIElement hElement, unsigned int width, unsigned int height)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementSize(pElement, width, height);
        }
    }
    void GUIContext::SetElementSizeRatio(HGUIElement hElement, float widthRatio, float heightRatio)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementSizeRatio(pElement, widthRatio, heightRatio);
        }
    }
    void GUIContext::SetElementSizeToChildren(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementSizeToChildren(pElement);
        }
    }


    void GUIContext::SetElementWidth(HGUIElement hElement, float width)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementWidth(pElement, width);
        }
    }
    void GUIContext::SetElementWidth(HGUIElement hElement, unsigned int width)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementWidth(pElement, width);
        }
    }
    void GUIContext::SetElementWidthRatio(HGUIElement hElement, float widthRatio)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementWidthRatio(pElement, widthRatio);
        }
    }
    void GUIContext::SetElementWidthToChildren(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementWidthToChildren(pElement);
        }
    }

    void GUIContext::SetElementMinWidth(HGUIElement hElement, float minWidth)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMinWidth(pElement, minWidth);
        }
    }
    void GUIContext::SetElementMinWidth(HGUIElement hElement, unsigned int minWidth)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMinWidth(pElement, minWidth);
        }
    }
    void GUIContext::SetElementMinWidthRatio(HGUIElement hElement, float minWidthRatio)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMinWidthRatio(pElement, minWidthRatio);
        }
    }
    void GUIContext::SetElementMinWidthToChildren(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMinWidthToChildren(pElement);
        }
    }

    void GUIContext::SetElementMaxWidth(HGUIElement hElement, float maxWidth)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMaxWidth(pElement, maxWidth);
        }
    }
    void GUIContext::SetElementMaxWidth(HGUIElement hElement, unsigned int maxWidth)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMaxWidth(pElement, maxWidth);
        }
    }
    void GUIContext::SetElementMaxWidthRatio(HGUIElement hElement, float maxWidthRatio)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMaxWidthRatio(pElement, maxWidthRatio);
        }
    }
    void GUIContext::SetElementMaxWidthToChildren(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMaxWidthToChildren(pElement);
        }
    }



    void GUIContext::SetElementHeight(HGUIElement hElement, float height)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementHeight(pElement, height);
        }
    }
    void GUIContext::SetElementHeight(HGUIElement hElement, unsigned int height)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementHeight(pElement, height);
        }
    }
    void GUIContext::SetElementHeightRatio(HGUIElement hElement, float heightRatio)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementHeightRatio(pElement, heightRatio);
        }
    }
    void GUIContext::SetElementHeightToChildren(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementHeightToChildren(pElement);
        }
    }

    void GUIContext::SetElementMinHeight(HGUIElement hElement, float minHeight)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMinHeight(pElement, minHeight);
        }
    }
    void GUIContext::SetElementMinHeight(HGUIElement hElement, unsigned int minHeight)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMinHeight(pElement, minHeight);
        }
    }
    void GUIContext::SetElementMinHeightRatio(HGUIElement hElement, float minHeightRatio)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMinHeightRatio(pElement, minHeightRatio);
        }
    }
    void GUIContext::SetElementMinHeightToChildren(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMinHeightToChildren(pElement);
        }
    }

    void GUIContext::SetElementMaxHeight(HGUIElement hElement, float maxHeight)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMaxHeight(pElement, maxHeight);
        }
    }
    void GUIContext::SetElementMaxHeight(HGUIElement hElement, unsigned int maxHeight)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMaxHeight(pElement, maxHeight);
        }
    }
    void GUIContext::SetElementMaxHeightRatio(HGUIElement hElement, float maxHeightRatio)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMaxHeightRatio(pElement, maxHeightRatio);
        }
    }
    void GUIContext::SetElementMaxHeightToChildren(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMaxHeightToChildren(pElement);
        }
    }




    float GUIContext::GetElementWidth(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementWidth(pElement);
        }

        return 0;
    }
    float GUIContext::GetElementWidthInPoints(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementWidthInPoints(pElement);
        }

        return 0;
    }

    float GUIContext::GetElementHeight(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementHeight(pElement);
        }

        return 0;
    }
    float GUIContext::GetElementHeightInPoints(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementHeightInPoints(pElement);
        }

        return 0;
    }


    void GUIContext::GetElementAbsoluteRect(HGUIElement hElement, GTLib::Rect<float> &rectOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementAbsoluteRect(pElement, rectOut);
        }
    }
    void GUIContext::GetElementAbsoluteRect(HGUIElement hElement, GTLib::Rect<int> &rectOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementAbsoluteRect(pElement, rectOut);
        }
    }

    void GUIContext::GetElementAbsoluteInnerRect(HGUIElement hElement, GTLib::Rect<float> &rectOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementAbsoluteInnerRect(pElement, rectOut);
        }
    }
    void GUIContext::GetElementAbsoluteInnerRect(HGUIElement hElement, GTLib::Rect<int> &rectOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementAbsoluteInnerRect(pElement, rectOut);
        }
    }

    void GUIContext::GetElementAbsoluteInnerBorderRect(HGUIElement hElement, GTLib::Rect<float> &rectOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementAbsoluteInnerBorderRect(pElement, rectOut);
        }
    }
    void GUIContext::GetElementAbsoluteInnerBorderRect(HGUIElement hElement, GTLib::Rect<int> &rectOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::GetElementAbsoluteInnerBorderRect(pElement, rectOut);
        }
    }


    void GUIContext::SetElementPositioning(HGUIElement hElement, Positioning positioning)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementPositioning(pElement, positioning);
        }
    }

    Positioning GUIContext::GetElementPositioning(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementPositioning(pElement);
        }

        return Positioning_Auto;
    }




    void GUIContext::SetElementBackgroundColor(HGUIElement hElement, const GTLib::Colour &color)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundColor(pElement, color);
        }
    }

    GTLib::Colour GUIContext::GetElementBackgroundColor(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundColor(pElement);
        }

        return GTLib::Colour(0.0f, 0.0f, 0.0f);
    }

    void GUIContext::SetElementBackgroundColorBoundary(HGUIElement hElement, BackgroundBoundary boundary)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundColorBoundary(pElement, boundary);
        }
    }

    BackgroundBoundary GUIContext::GetElementBackgroundColorBoundary(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundColorBoundary(pElement);
        }

        return BackgroundBoundary_InnerBorder;
    }


    bool GUIContext::SetElementBackgroundImage(HGUIElement hElement, const char* imageFilePath, unsigned int subImageOffsetX, unsigned int subImageOffsetY, unsigned int subImageWidth, unsigned int subImageHeight)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::SetElementBackgroundImage(pElement, imageFilePath, subImageOffsetX, subImageOffsetY, subImageWidth, subImageHeight);
        }

        return false;
    }

    HGUIImage GUIContext::GetElementBackgroundImage(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundImage(pElement);
        }

        return NULL;
    }


    void GUIContext::SetElementBackgroundImageColor(HGUIElement hElement, const GTLib::Colour &color)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundImageColor(pElement, color);
        }
    }

    GTLib::Colour GUIContext::GetElementBackgroundImageColor(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundImageColor(pElement);
        }

        return GTLib::Colour(1, 1, 1, 1);
    }


    void GUIContext::SetElementBackgroundSubImageOffset(HGUIElement hElement, unsigned int subImageOffsetX, unsigned int subImageOffsetY)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundSubImageOffset(pElement, subImageOffsetX, subImageOffsetY);
        }
    }

    bool GUIContext::GetElementBackgroundSubImageOffset(HGUIElement hElement, unsigned int &offsetXOut, unsigned int &offsetYOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundSubImageOffset(pElement, offsetXOut, offsetYOut);
        }

        return false;
    }


    void GUIContext::SetElementBackgroundSubImageSize(HGUIElement hElement, unsigned int subImageWidth, unsigned int subImageHeight)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundSubImageSize(pElement, subImageWidth, subImageHeight);
        }
    }

    bool GUIContext::GetElementBackgroundSubImageSize(HGUIElement hElement, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundSubImageSize(pElement, widthOut, heightOut);
        }

        return false;
    }


    void GUIContext::SetElementBackgroundImageBoundary(HGUIElement hElement, BackgroundBoundary boundary)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundImageBoundary(pElement, boundary);
        }
    }

    BackgroundBoundary GUIContext::GetElementBackgroundImageBoundary(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundImageBoundary(pElement);
        }

        return BackgroundBoundary_InnerBorder;
    }


    void GUIContext::SetElementBackgroundImageAlign(HGUIElement hElement, BackgroundAlign alignX, BackgroundAlign alignY)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundImageAlign(pElement, alignX, alignY);
        }
    }

    bool GUIContext::GetElementBackgroundImageAlign(HGUIElement hElement, BackgroundAlign &alignXOut, BackgroundAlign &alignYOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundImageAlign(pElement, alignXOut, alignYOut);
        }

        return false;
    }

    void GUIContext::SetElementBackgroundImageRepeat(HGUIElement hElement, BackgroundRepeat repeatX, BackgroundRepeat repeatY)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundImageRepeat(pElement, repeatX, repeatY);
        }
    }

    bool GUIContext::GetElementBackgroundImageRepeat(HGUIElement hElement, BackgroundRepeat &repeatXOut, BackgroundRepeat &repeatYOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundImageRepeat(pElement, repeatXOut, repeatYOut);
        }

        return false;
    }

    void GUIContext::SetElementBackgroundImageScale(HGUIElement hElement, float scaleX, float scaleY)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBackgroundImageScale(pElement, scaleX, scaleY);
        }
    }

    bool GUIContext::GetElementBackgroundImageScale(HGUIElement hElement, float &scaleXOut, float &scaleYOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementBackgroundImageScale(pElement, scaleXOut, scaleYOut);
        }

        return false;
    }



    void GUIContext::SetElementBorderLeftWidth(HGUIElement hElement, uint32_t width)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderLeftWidth(pElement, width);
        }
    }

    void GUIContext::SetElementBorderTopWidth(HGUIElement hElement, uint32_t width)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderTopWidth(pElement, width);
        }
    }

    void GUIContext::SetElementBorderRightWidth(HGUIElement hElement, uint32_t width)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderRightWidth(pElement, width);
        }
    }

    void GUIContext::SetElementBorderBottomWidth(HGUIElement hElement, uint32_t width)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderBottomWidth(pElement, width);
        }
    }

    void GUIContext::SetElementBorderWidth(HGUIElement hElement, uint32_t width)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderWidth(pElement, width);
        }
    }


    void GUIContext::SetElementBorderLeftColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderLeftColor(pElement, borderColor);
        }
    }

    void GUIContext::SetElementBorderTopColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderTopColor(pElement, borderColor);
        }
    }

    void GUIContext::SetElementBorderRightColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderRightColor(pElement, borderColor);
        }
    }

    void GUIContext::SetElementBorderBottomColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderBottomColor(pElement, borderColor);
        }
    }

    void GUIContext::SetElementBorderColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderColor(pElement, borderColor);
        }
    }

    void GUIContext::SetElementBorderLeft(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderLeft(pElement, width, color);
        }
    }

    void GUIContext::SetElementBorderTop(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderTop(pElement, width, color);
        }
    }

    void GUIContext::SetElementBorderRight(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderRight(pElement, width, color);
        }
    }

    void GUIContext::SetElementBorderBottom(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderBottom(pElement, width, color);
        }
    }

    void GUIContext::SetElementBorder(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorder(pElement, width, color);
        }
    }


    void GUIContext::SetElementBorderLeftMaskInPixels(HGUIElement hElement, uint32_t maskOffset, uint32_t maskLength, const GTLib::Colour &maskColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderLeftMaskInPixels(pElement, maskOffset, maskLength, maskColor);
        }
    }

    void GUIContext::SetElementBorderTopMaskInPixels(HGUIElement hElement, uint32_t maskOffset, uint32_t maskLength, const GTLib::Colour &maskColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderTopMaskInPixels(pElement, maskOffset, maskLength, maskColor);
        }
    }

    void GUIContext::SetElementBorderRightMaskInPixels(HGUIElement hElement, uint32_t maskOffset, uint32_t maskLength, const GTLib::Colour &maskColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderRightMaskInPixels(pElement, maskOffset, maskLength, maskColor);
        }
    }

    void GUIContext::SetElementBorderBottomMaskInPixels(HGUIElement hElement, uint32_t maskOffset, uint32_t maskLength, const GTLib::Colour &maskColor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementBorderBottomMaskInPixels(pElement, maskOffset, maskLength, maskColor);
        }
    }



    void GUIContext::SetElementPaddingLeft(HGUIElement hElement, uint32_t padding)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementPaddingLeft(pElement, padding);
        }
    }

    void GUIContext::SetElementPaddingTop(HGUIElement hElement, uint32_t padding)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementPaddingTop(pElement, padding);
        }
    }

    void GUIContext::SetElementPaddingRight(HGUIElement hElement, uint32_t padding)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementPaddingRight(pElement, padding);
        }
    }

    void GUIContext::SetElementPaddingBottom(HGUIElement hElement, uint32_t padding)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementPaddingBottom(pElement, padding);
        }
    }

    void GUIContext::SetElementPadding(HGUIElement hElement, uint32_t padding)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementPadding(pElement, padding);
        }
    }


    uint32_t GUIContext::GetElementPaddingLeft(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementPaddingLeft(pElement);
        }

        return 0;
    }

    uint32_t GUIContext::GetElementPaddingTop(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementPaddingTop(pElement);
        }

        return 0;
    }

    uint32_t GUIContext::GetElementPaddingRight(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementPaddingRight(pElement);
        }

        return 0;
    }

    uint32_t GUIContext::GetElementPaddingBottom(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementPaddingBottom(pElement);
        }

        return 0;
    }



    void GUIContext::SetElementMarginLeft(HGUIElement hElement, uint32_t margin)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMarginLeft(pElement, margin);
        }
    }

    void GUIContext::SetElementMarginTop(HGUIElement hElement, uint32_t margin)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMarginTop(pElement, margin);
        }
    }

    void GUIContext::SetElementMarginRight(HGUIElement hElement, uint32_t margin)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMarginRight(pElement, margin);
        }
    }

    void GUIContext::SetElementMarginBottom(HGUIElement hElement, uint32_t margin)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMarginBottom(pElement, margin);
        }
    }

    void GUIContext::SetElementMargin(HGUIElement hElement, uint32_t margin)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementMargin(pElement, margin);
        }
    }



    void GUIContext::SetElementChildAxis(HGUIElement hElement, ChildAxis axis)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementChildAxis(pElement, axis);
        }
    }

    ChildAxis GUIContext::GetElementChildAxis(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementChildAxis(pElement);
        }

        return ChildAxis_Vertical;
    }


    void GUIContext::SetElementHorizontalAlign(HGUIElement hElement, HorizontalAlign align)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementHorizontalAlign(pElement, align);
        }
    }

    HorizontalAlign GUIContext::GetElementHorizontalAlign(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementHorizontalAlign(pElement);
        }

        return HorizontalAlign_Left;
    }


    void GUIContext::SetElementVerticalAlign(HGUIElement hElement, VerticalAlign align)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementVerticalAlign(pElement, align);
        }
    }

    VerticalAlign GUIContext::GetElementVerticalAlign(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementVerticalAlign(pElement);
        }

        return VerticalAlign_Top;
    }



    void GUIContext::SetElementChildrenWidthBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementChildrenWidthBoundary(pElement, boundary);
        }
    }

    ChildrenSizeBoundary GUIContext::GetElementChildrenWidthBoundary(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementChildrenWidthBoundary(pElement);
        }

        return ChildrenSizeBoundary_Inner;
    }

    void GUIContext::SetElementChildrenHeightBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementChildrenHeightBoundary(pElement, boundary);
        }
    }

    ChildrenSizeBoundary GUIContext::GetElementChildrenHeightBoundary(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementChildrenHeightBoundary(pElement);
        }

        return ChildrenSizeBoundary_Inner;
    }

    void GUIContext::SetElementChildrenSizeBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementChildrenSizeBoundary(pElement, boundary);
        }
    }



    void GUIContext::SetElementClippingBoundary(HGUIElement hElement, ClippingBoundary boundary)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementClippingBoundary(pElement, boundary);
        }
    }

    ClippingBoundary GUIContext::GetElementClippingBoundary(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementClippingBoundary(pElement);
        }

        return ClippingBoundary_Inner;
    }


    void GUIContext::SetElementClippingMode(HGUIElement hElement, ClippingMode clippingMode)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementClippingMode(pElement, clippingMode);
        }
    }

    ClippingMode GUIContext::GetElementClippingMode(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementClippingMode(pElement);
        }

        return ClippingMode_Auto;
    }



    void GUIContext::SetElementVisibility(HGUIElement hElement, bool visible)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementVisibility(pElement, visible);
        }
    }

    bool GUIContext::IsElementVisible(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IsElementVisible(pElement);
        }

        return false;
    }


    void GUIContext::EnableElementChildWidthFlexing(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::EnableElementChildWidthFlexing(pElement);
        }
    }

    void GUIContext::DisableElementChildWidthFlexing(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DisableElementChildWidthFlexing(pElement);
        }
    }

    bool GUIContext::IsElementChildWidthFlexingEnabled(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IsElementChildWidthFlexingEnabled(pElement);
        }

        return false;
    }


    void GUIContext::EnableElementChildHeightFlexing(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::EnableElementChildHeightFlexing(pElement);
        }
    }

    void GUIContext::DisableElementChildHeightFlexing(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DisableElementChildHeightFlexing(pElement);
        }
    }

    bool GUIContext::IsElementChildHeightFlexingEnabled(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IsElementChildHeightFlexingEnabled(pElement);
        }

        return false;
    }


    void GUIContext::SetElementInnerOffsetX(HGUIElement hElement, float innerOffsetX)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementInnerOffsetX(pElement, innerOffsetX);
        }
    }

    void GUIContext::SetElementInnerOffsetY(HGUIElement hElement, float innerOffsetY)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementInnerOffsetY(pElement, innerOffsetY);
        }
    }

    void GUIContext::SetElementInnerOffset(HGUIElement hElement, float innerOffsetX, float innerOffsetY)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementInnerOffset(pElement, innerOffsetX, innerOffsetY);
        }
    }

    float GUIContext::GetElementInnerOffsetX(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementInnerOffsetX(pElement);
        }

        return 0.0f;
    }

    float GUIContext::GetElementInnerOffsetY(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementInnerOffsetY(pElement);
        }

        return 0.0f;
    }


    void GUIContext::SetElementText(HGUIElement hElement, const char* text)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementText(pElement, text);
        }
    }

    const char* GUIContext::GetElementText(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementText(pElement);
        }

        return nullptr;
    }

    bool GUIContext::DoesElementHaveText(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::DoesElementHaveText(pElement);
        }

        return false;
    }

    void GUIContext::InsertTextAtCursor(HGUIElement hElement, const char* text)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::InsertTextAtCursor(pElement, text);
        }
    }

    void GUIContext::DeleteSelectedText(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DeleteSelectedText(pElement);
        }
    }

    GTLib::String GUIContext::GetSelectedText(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetSelectedText(pElement);
        }

        return "";
    }

    void GUIContext::SelectAllText(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SelectAllText(pElement);
        }
    }

    bool GUIContext::UndoTextEdit(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::UndoTextEdit(pElement);
        }

        return false;
    }

    bool GUIContext::RedoTextEdit(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::RedoTextEdit(pElement);
        }

        return false;
    }

    unsigned int GUIContext::GetUndoPointsRemainingCount(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetUndoPointsRemainingCount(pElement);
        }

        return 0;
    }

    unsigned int GUIContext::GetRedoPointsRemainingCount(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetRedoPointsRemainingCount(pElement);
        }

        return 0;
    }

    bool GUIContext::PrepareUndoRedoPoint(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::PrepareUndoRedoPoint(pElement);
        }

        return false;
    }

    bool GUIContext::CreateUndoRedoPoint(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::CreateUndoRedoPoint(pElement);
        }

        return false;
    }


    HGUIFont GUIContext::SetElementFont(HGUIElement hElement, const char* family, FontWeight weight, FontSlant slant, uint32_t size, uint32_t sizeType)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::SetElementFont(pElement, family, weight, slant, size, sizeType);
        }

        return 0;
    }

    HGUIFont GUIContext::GetElementFont(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementFont(pElement);
        }

        return 0;
    }


    void GUIContext::SetElementTextColor(HGUIElement hElement, const GTLib::Colour &color)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementTextColor(pElement, color);
        }
    }

    GTLib::Colour GUIContext::GetElementTextColor(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementTextColor(pElement);
        }

        return GTLib::Colour(0.0f, 0.0f, 0.0f, 1.0f);
    }


    void GUIContext::EnableEditableText(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::EnableEditableText(pElement);
        }
    }

    void GUIContext::DisableEditableText(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DisableEditableText(pElement);
        }
    }

    bool GUIContext::IsEditableTextEnabled(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IsEditableTextEnabled(pElement);
        }

        return false;
    }

    void GUIContext::EnableMultiLineText(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::EnableMultiLineText(pElement);
        }
    }

    void GUIContext::DisableMultiLineText(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DisableMultiLineText(pElement);
        }
    }

    bool GUIContext::IsMultiLineTextEnabled(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IsMultiLineTextEnabled(pElement);
        }

        return false;
    }


    void GUIContext::EnableFocusOnMouseClick(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::EnableFocusOnMouseClick(pElement);
        }
    }

    void GUIContext::DisableFocusOnMouseClick(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DisableFocusOnMouseClick(pElement);
        }
    }

    bool GUIContext::IsFocusOnMouseClickEnabled(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IsFocusOnMouseClickEnabled(pElement);
        }

        return false;
    }


    void GUIContext::GiveElementKeyboardFocus(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GiveElementKeyboardFocus(pElement);
        }
    }

    void GUIContext::ReleaseKeyboardFocus()
    {
        GUIContextBase::ReleaseKeyboardFocus();
    }

    HGUIElement GUIContext::GetElementWithKeyboardFocus() const
    {
        auto pElement = reinterpret_cast<GUIElementWithHandle*>(GUIContextBase::GetElementWithKeyboardFocus());
        if (pElement != nullptr)
        {
            return pElement->handle;
        }

        return NULL;
    }



    void GUIContext::SetElementCursor(HGUIElement hElement, GUISystemCursor cursor)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetElementCursor(pElement, cursor);
        }
    }

    GUISystemCursor GUIContext::GetElementCursor(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::GetElementCursor(pElement);
        }

        return GUISystemCursor::Default;
    }


    bool GUIContext::AttachElementToSurface(HGUIElement hElement, HGUISurface hSurface)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            auto pSurface = this->GetSurfacePtr(hSurface);
            if (pSurface != nullptr)
            {
                return GUIContextBase::AttachElementToSurface(pElement, pSurface);
            }
        }

        return false;
    }

    bool GUIContext::DetachElementFromSurface(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::DetachElementFromSurface(pElement);
        }

        return false;
    }

    HGUISurface GUIContext::GetElementSurface(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            auto pSurface = GUIContextBase::GetElementSurface(pElement);
            if (pSurface != nullptr)
            {
                return reinterpret_cast<GUISurfaceWithHandle*>(pSurface)->handle;
            }
        }

        return 0;
    }

    HGUIElement GUIContext::GetElementUnderMouse() const
    {
        auto pElement = GUIContextBase::GetElementUnderMouse();
        if (pElement != nullptr)
        {
            return reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        }

        return 0;
    }

    bool GUIContext::IsElementUnderMouse(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IsElementUnderMouse(pElement);
        }

        return false;
    }


    void GUIContext::EnableCursorPassThrough(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::EnableCursorPassThrough(pElement);
        }
    }

    void GUIContext::DisableCursorPassThrough(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::DisableCursorPassThrough(pElement);
        }
    }

    bool GUIContext::IsCursorPassThroughEnabled(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::IsCursorPassThroughEnabled(pElement);
        }

        return false;
    }



    ////////////////////////////////////////////////////////////////
    // Batching

    void GUIContext::BeginBatch()
    {
        GUIContextBase::BeginBatch();
    }

    void GUIContext::EndBatch()
    {
        GUIContextBase::EndBatch();
    }



    ////////////////////////////////////////////////////////////////
    // Time Stepping

    void GUIContext::Step(double deltaTimeInSeconds)
    {
        GUIContextBase::Step(deltaTimeInSeconds);
    }



    ////////////////////////////////////////////////////////////////
    // Painting

    void GUIContext::InvalidateSurfaceRect(HGUISurface hSurface, const GTLib::Rect<int> &rect)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::InvalidateSurfaceRect(pSurface, rect);
        }
    }

    void GUIContext::InvalidateSurfaceRect(HGUISurface hSurface)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            unsigned int surfaceWidth;
            unsigned int surfaceHeight;
            GUIContextBase::GetSurfaceSize(pSurface, surfaceWidth, surfaceHeight);

            GTLib::Rect<int> rect;
            rect.left   = 0;
            rect.top    = 0;
            rect.right  = static_cast<int>(surfaceWidth);
            rect.bottom = static_cast<int>(surfaceHeight);
            GUIContextBase::InvalidateSurfaceRect(pSurface, rect);
        }
    }

    void GUIContext::PaintSurface(HGUISurface hSurface, const GTLib::Rect<int> &rect, void* pInputData)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::PaintSurface(pSurface, rect, pInputData);
        }
    }

    void GUIContext::PaintSurface(HGUISurface hSurface, void* pInputData)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::PaintSurface(pSurface, pInputData);
        }
    }


    /////////////////////////////////////////////////////////////////
    // Events

    // TODO: In the Attach*() methods, how should duplicate event handlers be handled? Currently it is not checked whether or not
    //       an event handler is already attached.
    //
    //       An event handler should never be attached more than once. It should be removed and then added to the end of the list.

    void GUIContext::AttachLocalEventHandler(HGUIElement hElement, GUIEventHandler &eventHandler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            pElement->eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
            pElement->eventHandlers.PushBack(&eventHandler);
        }
    }

    void GUIContext::DetachLocalEventHandler(HGUIElement hElement, GUIEventHandler &eventHandler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            pElement->eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
        }
    }

    void GUIContext::AttachGlobalEventHandler(GUIEventHandler &eventHandler)
    {
        m_globalEventHandlers.RemoveFirstOccuranceOf(&eventHandler);
        m_globalEventHandlers.PushBack(&eventHandler);
    }

    void GUIContext::DetachGlobalEventHandler(GUIEventHandler &eventHandler)
    {
        m_globalEventHandlers.RemoveFirstOccuranceOf(&eventHandler);
    }



    void GUIContext::OnElementMove(HGUIElement hElement, LocalOnMoveProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnMove.PushBack(handler);
        }
    }

    void GUIContext::OnElementSize(HGUIElement hElement, LocalOnSizeProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnSize.PushBack(handler);
        }
    }

    void GUIContext::OnElementMouseEnter(HGUIElement hElement, LocalOnMouseEnterProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnMouseEnter.PushBack(handler);
        }
    }

    void GUIContext::OnElementMouseLeave(HGUIElement hElement, LocalOnMouseLeaveProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnMouseLeave.PushBack(handler);
        }
    }

    void GUIContext::OnElementMouseMove(HGUIElement hElement, LocalOnMouseMoveProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnMouseMove.PushBack(handler);
        }
    }

    void GUIContext::OnElementMouseButtonPressed(HGUIElement hElement, LocalOnMouseButtonPressedProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnMouseButtonPressed.PushBack(handler);
        }
    }

    void GUIContext::OnElementMouseButtonReleased(HGUIElement hElement, LocalOnMouseButtonReleasedProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnMouseButtonReleased.PushBack(handler);
        }
    }

    void GUIContext::OnElementMouseButtonDoubleClicked(HGUIElement hElement, LocalOnMouseButtonDoubleClickedProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnMouseButtonDoubleClicked.PushBack(handler);
        }
    }

    void GUIContext::OnElementMouseWheel(HGUIElement hElement, LocalOnMouseWheelProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnMouseWheel.PushBack(handler);
        }
    }

    void GUIContext::OnElementKeyPressed(HGUIElement hElement, LocalOnKeyPressedProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnKeyPressed.PushBack(handler);
        }
    }

    void GUIContext::OnElementKeyPressedAutoRepeat(HGUIElement hElement, LocalOnKeyPressedAutoRepeatProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnKeyPressedAutoRepeat.PushBack(handler);
        }
    }

    void GUIContext::OnElementKeyReleased(HGUIElement hElement, LocalOnKeyReleasedProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnKeyReleased.PushBack(handler);
        }
    }

    void GUIContext::OnElementPrintableKeyDown(HGUIElement hElement, LocalOnPrintableKeyDownProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnPrintableKeyDown.PushBack(handler);
        }
    }

    void GUIContext::OnElementSetMouseEventCapture(HGUIElement hElement, LocalOnSetMouseEventCaptureProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnSetMouseEventCapture.PushBack(handler);
        }
    }

    void GUIContext::OnElementReleaseMouseEventCapture(HGUIElement hElement, LocalOnReleaseMouseEventCaptureProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnReleaseMouseEventCapture.PushBack(handler);
        }
    }

    void GUIContext::OnElementTextChanged(HGUIElement hElement, LocalOnTextChangedProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnTextChanged.PushBack(handler);
        }
    }

    void GUIContext::OnElementPaint(HGUIElement hElement, LocalOnPaintProc handler)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->pCallbackEventHandlers == nullptr)
            {
                pElement->pCallbackEventHandlers = new LocalCallbackEventHandlers;
            }

            pElement->pCallbackEventHandlers->OnPaint.PushBack(handler);
        }
    }


    void GUIContext::OnGlobalElementMove(GlobalOnMoveProc handler)
    {
        m_callbackGlobalEventHandlers.OnMove.PushBack(handler);
    }

    void GUIContext::OnGlobalElementSize(GlobalOnSizeProc handler)
    {
        m_callbackGlobalEventHandlers.OnSize.PushBack(handler);
    }

    void GUIContext::OnGlobalElementMouseEnter(GlobalOnMouseEnterProc handler)
    {
        m_callbackGlobalEventHandlers.OnMouseEnter.PushBack(handler);
    }

    void GUIContext::OnGlobalElementMouseLeave(GlobalOnMouseLeaveProc handler)
    {
        m_callbackGlobalEventHandlers.OnMouseLeave.PushBack(handler);
    }

    void GUIContext::OnGlobalElementMouseMove(GlobalOnMouseMoveProc handler)
    {
        m_callbackGlobalEventHandlers.OnMouseMove.PushBack(handler);
    }

    void GUIContext::OnGlobalElementMouseButtonPressed(GlobalOnMouseButtonPressedProc handler)
    {
        m_callbackGlobalEventHandlers.OnMouseButtonPressed.PushBack(handler);
    }

    void GUIContext::OnGlobalElementMouseButtonReleased(GlobalOnMouseButtonReleasedProc handler)
    {
        m_callbackGlobalEventHandlers.OnMouseButtonReleased.PushBack(handler);
    }

    void GUIContext::OnGlobalElementMouseButtonDoubleClicked(GlobalOnMouseButtonDoubleClickedProc handler)
    {
        m_callbackGlobalEventHandlers.OnMouseButtonDoubleClicked.PushBack(handler);
    }

    void GUIContext::OnGlobalElementMouseWheel(GlobalOnMouseWheelProc handler)
    {
        m_callbackGlobalEventHandlers.OnMouseWheel.PushBack(handler);
    }

    void GUIContext::OnGlobalElementKeyPressed(GlobalOnKeyPressedProc handler)
    {
        m_callbackGlobalEventHandlers.OnKeyPressed.PushBack(handler);
    }

    void GUIContext::OnGlobalElementKeyPressedAutoRepeat(GlobalOnKeyPressedAutoRepeatProc handler)
    {
        m_callbackGlobalEventHandlers.OnKeyPressedAutoRepeat.PushBack(handler);
    }

    void GUIContext::OnGlobalElementKeyReleased(GlobalOnKeyReleasedProc handler)
    {
        m_callbackGlobalEventHandlers.OnKeyReleased.PushBack(handler);
    }

    void GUIContext::OnGlobalElementPrintableKeyDown(GlobalOnPrintableKeyDownProc handler)
    {
        m_callbackGlobalEventHandlers.OnPrintableKeyDown.PushBack(handler);
    }

    void GUIContext::OnGlobalElementSetMouseEventCapture(GlobalOnSetMouseEventCaptureProc handler)
    {
        m_callbackGlobalEventHandlers.OnSetMouseEventCapture.PushBack(handler);
    }

    void GUIContext::OnGlobalElementReleaseMouseEventCapture(GlobalOnReleaseMouseEventCaptureProc handler)
    {
        m_callbackGlobalEventHandlers.OnReleaseMouseEventCapture.PushBack(handler);
    }

    void GUIContext::OnGlobalElementTextChanged(GlobalOnTextChangedProc handler)
    {
        m_callbackGlobalEventHandlers.OnTextChanged.PushBack(handler);
    }

    void GUIContext::OnGlobalElementSurfaceNeedsRepaint(GlobalOnSurfaceNeedsRepaintProc handler)
    {
        m_callbackGlobalEventHandlers.OnSurfaceNeedsRepaint.PushBack(handler);
    }

    void GUIContext::OnGlobalCursorNeedsToChange(GlobalOnCursorNeedsToChangeProc handler)
    {
        m_callbackGlobalEventHandlers.OnCursorNeedsToChange.PushBack(handler);
    }




    ////////////////////////////////////////////////////////////////
    // Inbound Events

    void GUIContext::OnMouseMove(HGUISurface hSurface, int mousePosX, int mousePosY)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::OnMouseMove(pSurface, mousePosX, mousePosY);
        }
    }

    void GUIContext::OnMouseEnter(HGUISurface hSurface)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::OnMouseEnter(pSurface);
        }
    }

    void GUIContext::OnMouseLeave(HGUISurface hSurface)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::OnMouseLeave(pSurface);
        }
    }

    void GUIContext::OnMouseButtonPressed(HGUISurface hSurface, int mouseButton, int mousePosX, int mousePosY)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::OnMouseButtonPressed(pSurface, mouseButton, mousePosX, mousePosY);
        }
    }

    void GUIContext::OnMouseButtonReleased(HGUISurface hSurface, int mouseButton, int mousePosX, int mousePosY)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::OnMouseButtonReleased(pSurface, mouseButton, mousePosX, mousePosY);
        }
    }

    void GUIContext::OnMouseButtonDoubleClicked(HGUISurface hSurface, int mouseButton, int mousePosX, int mousePosY)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::OnMouseButtonDoubleClicked(pSurface, mouseButton, mousePosX, mousePosY);
        }
    }

    void GUIContext::OnMouseWheel(HGUISurface hSurface, int delta, int mousePosX, int mousePosY)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::OnMouseWheel(pSurface, delta, mousePosX, mousePosY);
        }
    }

    void GUIContext::OnKeyPressed(GTLib::Key key)
    {
        GUIContextBase::OnKeyPressed(key);
    }

    void GUIContext::OnKeyPressedAutoRepeat(GTLib::Key key)
    {
        GUIContextBase::OnKeyPressedAutoRepeat(key);
    }

    void GUIContext::OnKeyReleased(GTLib::Key key)
    {
        GUIContextBase::OnKeyReleased(key);
    }

    void GUIContext::OnPrintableKeyDown(char32_t character)
    {
        GUIContextBase::OnPrintableKeyDown(character);
    }



    ////////////////////////////////////////////////////////////////
    // Searching/Gathering

    HGUIElement GUIContext::FindElementUnderPoint(HGUISurface hSurface, int x, int y)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            auto pElementUnderPoint = GUIContextBase::FindElementUnderPoint(pSurface, x, y);
            if (pElementUnderPoint != nullptr)
            {
                return reinterpret_cast<GUIElementWithHandle*>(pElementUnderPoint)->handle;
            }
        }

        return 0;
    }



    ////////////////////////////////////////////////////////////////
    // DPI / Scaling

    unsigned int GUIContext::GetBaseXDPI() const
    {
        return GUIContextBase::GetBaseXDPI();
    }

    unsigned int GUIContext::GetBaseYDPI() const
    {
        return GUIContextBase::GetBaseYDPI();
    }

    void GUIContext::GetBaseDPI(unsigned int &xBaseDPIOut, unsigned int &yBaseDPIOut) const
    {
        return GUIContextBase::GetBaseDPI(xBaseDPIOut, yBaseDPIOut);
    }

    void GUIContext::SetBaseDPI(unsigned int xBaseDPI, unsigned int yBaseDPI)
    {
        GUIContextBase::SetBaseDPI(xBaseDPI, yBaseDPI);
    }


    unsigned int GUIContext::GetXDPI(HGUISurface hSurface) const
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            return GUIContextBase::GetXDPI(pSurface);
        }

        return this->GetBaseXDPI();
    }

    unsigned int GUIContext::GetYDPI(HGUISurface hSurface) const
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            return GUIContextBase::GetYDPI(pSurface);
        }

        return this->GetBaseYDPI();
    }

    void GUIContext::GetDPI(HGUISurface hSurface, unsigned int &xDPIOut, unsigned int &yDPIOut)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            return GUIContextBase::GetDPI(pSurface, xDPIOut, yDPIOut);
        }
        else
        {
            xDPIOut = this->GetBaseXDPI();
            yDPIOut = this->GetBaseYDPI();
        }
    }

    void GUIContext::SetDPI(HGUISurface hSurface, unsigned int xDPI, unsigned int yDPI)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            GUIContextBase::SetDPI(pSurface, xDPI, yDPI);
        }
    }


    float GUIContext::GetXDPIScalingFactor(HGUISurface hSurface) const
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            return GUIContextBase::GetXDPIScalingFactor(pSurface);
        }
        else
        {
            return 1.0f;
        }
    }

    float GUIContext::GetYDPIScalingFactor(HGUISurface hSurface) const
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            return GUIContextBase::GetYDPIScalingFactor(pSurface);
        }
        else
        {
            return 1.0f;
        }
    }


    float GUIContext::PixelsToPointsX(HGUIElement hElement, float x)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return x / GUIContextBase::GetXDPIScalingFactor(GUIContextBase::GetElementSurface(pElement));
        }

        return x;
    }
    float GUIContext::PixelsToPointsX(HGUIElement hElement, int x)
    {
        return PixelsToPointsX(hElement, static_cast<float>(x));
    }

    float GUIContext::PixelsToPointsY(HGUIElement hElement, float y)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return y / GUIContextBase::GetXDPIScalingFactor(GUIContextBase::GetElementSurface(pElement));
        }

        return y;
    }
    float GUIContext::PixelsToPointsY(HGUIElement hElement, int y)
    {
        return PixelsToPointsY(hElement, static_cast<float>(y));
    }



    ////////////////////////////////////////////////////////////////
    // Utilities

    void GUIContext::AbsoluteToRelative(HGUIElement hElement, int xIn, int yIn, int &xOut, int &yOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::AbsoluteToRelative(pElement, xIn, yIn, xOut, yOut);
        }
        else
        {
            xOut = xIn;
            yOut = yIn;
        }
    }

    void GUIContext::RelativeToAbsolute(HGUIElement hElement, int xIn, int yIn, int &xOut, int &yOut) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return GUIContextBase::RelativeToAbsolute(pElement, xIn, yIn, xOut, yOut);
        }
        else
        {
            xOut = xIn;
            yOut = yIn;
        }
    }


    ////////////////////////////////////////////////////////////////
    // Miscellaneous

    GUIFontManager* GUIContext::GetFontManager()
    {
        return GUIContextBase::GetFontManager();
    }

    void GUIContext::SetMouseEventCapture(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            GUIContextBase::SetMouseEventCapture(pElement);
        }
    }

    HGUIElement GUIContext::GetMouseEventCapture() const
    {
        auto pElement = GUIContextBase::GetMouseEventCapture();
        if (pElement != nullptr)
        {
            return reinterpret_cast<GUIElementWithHandle*>(pElement)->handle;
        }

        return 0;
    }

    void GUIContext::ReleaseMouseEventCapture()
    {
        GUIContextBase::ReleaseMouseEventCapture();
    }


    void GUIContext::ShowTextCursor(HGUIElement hOwnerElement, int relativePosX, int relativePosY)
    {
        auto pOwnerElement = this->GetElementPtr(hOwnerElement);
        if (pOwnerElement != nullptr)
        {
            GUIContextBase::ShowTextCursor(pOwnerElement, relativePosX, relativePosY);
        }
    }

    void GUIContext::HideTextCursor()
    {
        GUIContextBase::HideTextCursor();
    }

    bool GUIContext::IsTextCursorVisible() const
    {
        return GUIContextBase::IsTextCursorVisible();
    }

    void GUIContext::SetTextCursorBlinkTime(float blinkTimeInSeconds)
    {
        return GUIContextBase::SetTextCursorBlinkTime(blinkTimeInSeconds);
    }

    HGUIElement GUIContext::GetTextCursorOwnerElement() const
    {
        auto pOwnerElement = GUIContextBase::GetTextCursorOwnerElement();
        if (pOwnerElement != nullptr)
        {
            return reinterpret_cast<GUIElementWithHandle*>(pOwnerElement)->handle;
        }

        return NULL;
    }

    void GUIContext::GetTextCursorRelativePosition(int &relativePosXOut, int &relativePosYOut) const
    {
        GUIContextBase::GetTextCursorRelativePosition(relativePosXOut, relativePosYOut);
    }



    ////////////////////////////////////////////////////////////////
    // Private

    GUISurfaceWithHandle* GUIContext::GetSurfacePtr(HGUISurface hSurface) const
    {
        auto surface = m_surfaceHandles.GetAssociatedObject(hSurface);
        if (surface != nullptr)
        {
            if (hSurface == surface->handle)
            {
                return surface;
            }
            else
            {
                // Invalid handle (uniqueness IDs do not match).
                return nullptr;
            }
        }
        else
        {
            // Invalid handle (deleted or null).
            return nullptr;
        }
    }

    GUIElementWithHandle* GUIContext::GetElementPtr(HGUIElement hElement) const
    {
        auto element = m_elementHandles.GetAssociatedObject(hElement);
        if (element != nullptr)
        {
            if (hElement == element->handle)
            {
                return element;
            }
            else
            {
                // Invalid handle (uniqueness IDs do not match).
                return nullptr;
            }
        }
        else
        {
            // Invalid handle (deleted or null).
            return nullptr;
        }
    }


    //////////////////////////////////////////////////
    // Event Posting
    //
    // Posting an event is not as simple as it appears on the surface. The issue is that once an event is posted, the host application
    // can do anything it wants, including delete the element itself, remove an event handler, attach a new event handler, etc.
    //
    // To combat this, we need to check for the following:
    //   - Is the element handle still valid?
    //   - Was an event handler attached?
    //   - Was an event handler detached?
    //
    // There are a few assumptions we can make when doing these checks.
    //   - When an element is deleted, the handle will become invalid, so GetElementPtr() will return null.
    //   - When an event handler is attached, it will be always be attached to the end of the list, which means it will be handled correctly as part of the normal iteration.
    //   - When an event handler is detached, it will only matter if the detached event handler came before the current one.

    void GUIContext::IterateLocalEventHandlers(HGUIElement hElement, std::function<bool (GUIEventHandler &eventHandler)> func)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            // The list of event handlers that have been processed. Do not dereference any of these pointers as they may become invalid during iteration.
            GTLib::Vector<GUIEventHandler*> processedEventHandlers;


            size_t iEventHandler = 0;
            while (iEventHandler < pElement->eventHandlers.GetCount())
            {
                auto eventHandler = pElement->eventHandlers[iEventHandler];
                assert(eventHandler != nullptr);
                {
                    if (func(*eventHandler))
                    {
                        // If the handle is no longer valid, break.
                        if (this->GetElementPtr(hElement) != pElement)
                        {
                            return;
                        }


                        // Let the algorithm know that this event handler has been processed.
                        processedEventHandlers.PushBack(eventHandler);


                        // If an event handler has been removed, we need to roll the index back to that of the event handler that was last processed.
                        if (eventHandler == pElement->eventHandlers[iEventHandler])
                        {
                            // No significant event handler was removed. Just move forward like normal.
                            iEventHandler += 1;
                        }
                        else
                        {
                            // An event handler has been removed, so need to ensure we adjust the index appropriately. All we're doing is rolling back
                            // to the index just past the last processed event handler.
                            while (iEventHandler > 0)
                            {
                                iEventHandler -= 1;

                                if (processedEventHandlers.ExistsReverse(pElement->eventHandlers[iEventHandler]))
                                {
                                    iEventHandler += 1;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        // The iteration function returned false which means we want to break.
                        return;
                    }
                }
            }
        }
    }

    void GUIContext::IterateGlobalEventHandlers(std::function<bool (GUIEventHandler &eventHandler)> func)
    {
        // The list of event handlers that have been processed. Do not dereference any of these pointers as they may become invalid during iteration.
        GTLib::Vector<GUIEventHandler*> processedEventHandlers;


        size_t iEventHandler = 0;
        while (iEventHandler < m_globalEventHandlers.GetCount())
        {
            auto pEventHandler = m_globalEventHandlers[iEventHandler];
            assert(pEventHandler != nullptr);
            {
                if (func(*pEventHandler))
                {
                    // Let the algorithm know that this event handler has been processed.
                    processedEventHandlers.PushBack(pEventHandler);


                    // If an event handler has been removed, we need to roll the index back to that of the event handler that was last processed.
                    if (pEventHandler == m_globalEventHandlers[iEventHandler])
                    {
                        // No significant event handler was removed. Just move forward like normal.
                        iEventHandler += 1;
                    }
                    else
                    {
                        // An event handler has been removed, so need to ensure we adjust the index appropriately. All we're doing is rolling back
                        // to the index just past the last processed event handler.
                        while (iEventHandler > 0)
                        {
                            iEventHandler -= 1;

                            if (processedEventHandlers.ExistsReverse(m_globalEventHandlers[iEventHandler]))
                            {
                                iEventHandler += 1;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    // The iteration function returned false which means we want to break.
                    return;
                }
            }
        }
    }
}
