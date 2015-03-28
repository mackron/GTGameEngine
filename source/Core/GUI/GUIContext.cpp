// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUIContext.hpp>
#include "GUIRenderer_Generic.hpp"

extern "C" {
    //uint64_t _x64_clipping_clear(void* buffer, unsigned int stride, unsigned int rectLeft, unsigned int rectTop, uint64_t rectRight, uint64_t rectBottom);
}

namespace GT
{
    GUIContext::GUIContext(GUIRenderer* renderer)
        : m_elementHandles(),
          m_surfaceHandles(),
          //m_imageManager(),
          m_fontManager(/*m_imageManager*/),
          m_defaultFont(nullptr),
          m_xBaseDPI(96), m_yBaseDPI(96), m_xDPI(m_xBaseDPI), m_yDPI(m_yBaseDPI), 
          m_renderer(renderer),
          m_ownsRenderer(false),
          m_layoutContext(),
          m_batchLockCounter(0),
          m_globalEventHandlers()
#if defined(GT_GUI_DEBUGGING)
        , m_layoutLogger()
#endif
    {
        // We need a renderer. For now we're just goint to use the generic one.
        if (renderer == nullptr)
        {
            m_renderer = new GUIRenderer_Generic;
            m_ownsRenderer = true;
        }


        // Create the default font.
        GTLib::FontInfo fi;
        fi.family       = "liberation sans";
        fi.sizeInPoints = 9 * this->GetXDPIScalingFactor();
        m_defaultFont = m_fontManager.AcquireFont(fi);
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


    ////////////////////////////////////////////////////////////////
    // Surfaces

    HGUISurface GUIContext::CreateSurface()
    {
        HGUISurface handle = m_surfaceHandles.CreateHandle();
        if (handle != 0)
        {
            m_surfaceHandles.AssociateObjectWithHandle(handle, new GUISurface(handle));
        }

        return handle;
    }

    void GUIContext::DeleteSurface(HGUISurface hSurface)
    {
        assert(m_renderer != nullptr);

        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            m_surfaceHandles.DeleteHandle(hSurface);
            delete surface;
        }
    }

    HGUISurface GUIContext::FindSurfaceByID(const char* id) const
    {
        GUISurface* foundSurface = nullptr;

        m_surfaceHandles.IterateAssociatedObjects([&](GUISurface* surface) -> bool
        {
            assert(surface != nullptr);
            {
                if (surface->id == id)
                {
                    foundSurface = surface;
                    return false;
                }
            }

            return true;
        });



        if (foundSurface != nullptr)
        {
            return foundSurface->handle;
        }
        
        // A surface with the given ID could not be found.
        return 0;
    }

    void GUIContext::SetSurfaceID(HGUISurface hSurface, const char* id)
    {
        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            surface->id = id;
        }
    }

    const char* GUIContext::GetSurfaceID(HGUISurface hSurface) const
    {
        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            return surface->id.c_str();
        }

        return 0;
    }


    void GUIContext::SetSurfaceSize(HGUISurface hSurface, unsigned int newWidth, unsigned int newHeight)
    {
        assert(m_renderer != nullptr);

        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            surface->width  = newWidth;
            surface->height = newHeight;


            this->BeginBatch();

            // Top level elements may need to have their layouts invalidated.
            for (size_t iTopLevelElement = 0; iTopLevelElement < surface->topLevelElements.GetCount(); ++iTopLevelElement)
            {
                auto element = this->GetElementPtr(surface->topLevelElements[iTopLevelElement]);
                assert(element != nullptr);
                {
                    int flags = 0;
                    
                    // Width.
                    if (GUIElementStyle_Get_IsAnyWidthOfType(element->style, NumberType_Percent))
                    {
                        flags |= LayoutFlag_WidthInvalid;
                    }

                    // Height.
                    if (GUIElementStyle_Get_IsAnyHeightOfType(element->style, NumberType_Percent))
                    {
                        flags |= LayoutFlag_HeightInvalid;
                    }


                    // Position.
                    if (GUIElementStyle_Get_righthaspriority(element->style) || GUIElementStyle_Get_bottomhaspriority(element->style))
                    {
                        flags |= LayoutFlag_PositionInvalid;
                    }


                    if (flags != 0)
                    {
                        this->Layout_InvalidateElementLayout(*element, flags);
                    }
                }
            }

            this->EndBatch();
        }
    }

    void GUIContext::GetSurfaceSize(HGUISurface hSurface, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            widthOut  = surface->width;
            heightOut = surface->height;
        }
        else
        {
            widthOut  = 0U;
            heightOut = 0U;
        }
    }

    bool GUIContext::DoesSurfaceContainGUIElement(HGUISurface hSurface, HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return pElement->hSurface == hSurface;
        }
        else
        {
            return false;
        }
    }

    void GUIContext::SetSurfacePaintingMode(HGUISurface hSurface, GUIPaintingMode paintingMode)
    {
        auto pSurface = this->GetSurfacePtr(hSurface);
        if (pSurface != nullptr)
        {
            pSurface->paintingMode = paintingMode;
        }
    }



    ////////////////////////////////////////////////////////////////
    // GUI Elements

    HGUIElement GUIContext::CreateElement()
    {
        HGUIElement handle = m_elementHandles.CreateHandle();
        if (handle != 0)
        {
            auto element = new GUIElement(handle);
            assert(element != nullptr);
            {
                m_elementHandles.AssociateObjectWithHandle(handle, element);

                // Set the default styles.
                memset(&element->style, 0, sizeof(element->style));

                // Min/max size.
                //
                // NOTE: We don't want to use SetGUIElementMinWidthToDefault(), etc. because we that will trigger a layout validation which we don't really want. To work around this, we
                //       need to set it directly on the style.
                GUIElementStyle_Set_minwidth(element->style, 0U,                         NumberType_Absolute);
                GUIElementStyle_Set_maxwidth(element->style, GUIStyleNumber_MaxUnsigned, NumberType_Absolute);

                GUIElementStyle_Set_minheight(element->style, 0U,                         NumberType_Absolute);
                GUIElementStyle_Set_maxheight(element->style, GUIStyleNumber_MaxUnsigned, NumberType_Absolute);


                // Set the default font.
                this->SetElementFont(handle, "Liberation Sans", FontWeight_Medium, FontSlant_None, 9, NumberType_Points);
            }
        }
        
        return handle;
    }

    void GUIContext::DeleteElement(HGUIElement hElement)
    {
        this->BeginBatch();
        {
            auto element = this->GetElementPtr(hElement);
            if (element != nullptr)
            {
                // The area consumed by the deleted element needs to be invalidated so that it will be redrawn.
                this->Painting_InvalidateElementRect(*element);


                // We'll recursively delete the children first.
                while (element->firstChild != 0)
                {
                    this->DeleteElement(element->firstChild);
                }


                // The element needs to be isolated before deleting.
                this->DetachElementFromParentAndSiblings(*element);


                // If the element has an invalid layout, clear it from the layout manager.
                if (element->layout.layoutValidationListItem != nullptr)
                {
                    m_layoutContext.invalidElements.Remove(element->layout.layoutValidationListItem);
                }

                // Now the element can actually be deleted.
                m_elementHandles.DeleteHandle(hElement);
                delete element;
            }
        }
        this->EndBatch();
    }

    void GUIContext::SetElementID(HGUIElement hElement, const char* id)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            element->id = id;
        }
    }

    const char* GUIContext::GetElementID(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return element->id.c_str();
        }

        return nullptr;
    }

    HGUIElement GUIContext::FindElementByID(const char* elementID) const
    {
        GUIElement* foundElement = nullptr;

        m_elementHandles.IterateAssociatedObjects([&](GUIElement* element) -> bool
        {
            assert(element != nullptr);
            {
                if (element->id == elementID)
                {
                    foundElement = element;
                    return false;
                }
            }

            return true;
        });



        if (foundElement != nullptr)
        {
            return foundElement->handle;
        }
        
        // An element with the given ID could not be found.
        return 0;
    }


    HGUIElement GUIContext::GetElementParent(HGUIElement hChildElement) const
    {
        auto childElement = this->GetElementPtr(hChildElement);
        if (childElement != nullptr)
        {
            return childElement->parent;
        }

        return 0;
    }

    void GUIContext::SetElementParent(HGUIElement hChildElement, HGUIElement hParentElement)
    {
        if (hParentElement != 0)
        {
            this->AppendChildElement(hParentElement, hChildElement);
        }
        else
        {
            this->DetachElementFromParent(hChildElement);
        }
    }

    void GUIContext::DetachElementFromParent(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            this->DetachElementFromParent(*element);
        }
    }

    void GUIContext::DetachElementFromParentAndSiblings(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            this->DetachElementFromParentAndSiblings(*element);
        }
    }

    void GUIContext::AppendChildElement(HGUIElement hParentElement, HGUIElement hChildElement)
    {
        auto parentElement = this->GetElementPtr(hParentElement);
        if (parentElement != nullptr)
        {
            auto childElement = this->GetElementPtr(hChildElement);
            if (childElement != nullptr)
            {
                this->AppendChildElement(*parentElement, *childElement);
            }
        }
    }

    void GUIContext::PrependChildElement(HGUIElement hParentElement, HGUIElement hChildElement)
    {
        auto parentElement = this->GetElementPtr(hParentElement);
        if (parentElement != nullptr)
        {
            auto childElement = this->GetElementPtr(hChildElement);
            if (childElement != nullptr)
            {
                this->PrependChildElement(*parentElement, *childElement);
            }
        }
    }

    void GUIContext::AppendSiblingElement(HGUIElement hElementToAppendTo, HGUIElement hElementToAppend)
    {
        auto elementToAppendTo = this->GetElementPtr(hElementToAppendTo);
        if (elementToAppendTo != nullptr)
        {
            auto elementToAppend = this->GetElementPtr(hElementToAppend);
            if (elementToAppend != nullptr)
            {
                this->AppendSiblingElement(*elementToAppendTo, *elementToAppend);
            }
        }
    }

    void GUIContext::PrependSiblingElement(HGUIElement hElementToPrependTo, HGUIElement hElementToPrepend)
    {
        auto elementToPrependTo = this->GetElementPtr(hElementToPrependTo);
        if (elementToPrependTo != nullptr)
        {
            auto elementToPrepend = this->GetElementPtr(hElementToPrepend);
            if (elementToPrepend != nullptr)
            {
                this->PrependSiblingElement(*elementToPrependTo, *elementToPrepend);
            }
        }
    }


    HGUIElement GUIContext::GetElementNextSibling(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return element->nextSibling;
        }

        return 0;
    }

    HGUIElement GUIContext::GetElementPreviousSibling(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return element->prevSibling;
        }

        return 0;
    }


    void GUIContext::IterateElementPrevSiblings(HGUIElement hElement, std::function<void (HGUIElement)> func)
    {
        HGUIElement prevSibling = this->GetElementPreviousSibling(hElement);
        if (prevSibling != 0)
        {
            this->IterateElementPrevSiblings(prevSibling, func);
            func(prevSibling);
        }
    }

    void GUIContext::IterateElementNextSiblings(HGUIElement hElement, std::function<void (HGUIElement)> func)
    {
        // NOTE: This can be iterative which might be more efficient. Try switching if this shows up in profiling. Using recursion here
        //       for consistency with InterateGUIElementPrevSiblings().
        HGUIElement nextSibling = this->GetElementNextSibling(hElement);
        if (nextSibling != 0)
        {
            func(nextSibling);
            this->IterateElementNextSiblings(nextSibling, func);
        }
    }

    void GUIContext::IterateElementSiblingsAndSelf(HGUIElement hElement, std::function<void (HGUIElement)> func)
    {
        // Prev siblings.
        this->IterateElementPrevSiblings(hElement, func);

        // Self
        func(hElement);

        // Next siblings.
        this->IterateElementNextSiblings(hElement, func);
    }


    bool GUIContext::IsElementChild(HGUIElement hParentElement, HGUIElement hChildElement) const
    {
        auto parentElement = this->GetElementPtr(hParentElement);
        if (parentElement != nullptr)
        {
            for (HGUIElement hElement = parentElement->firstChild; hElement != 0; hElement = this->GetElementNextSibling(hElement))
            {
                if (hChildElement == hElement)
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool GUIContext::IsElementParent(HGUIElement hChildElement, HGUIElement hParentElement) const
    {
        return this->GetElementParent(hChildElement) == hParentElement;
    }

    bool GUIContext::IsElementDescendant(HGUIElement hParentElement, HGUIElement hChildElement) const
    {
        auto parentElement = this->GetElementPtr(hParentElement);
        if (parentElement != nullptr)
        {
            for (HGUIElement hElement = parentElement->firstChild; hElement != 0; hElement = this->GetElementNextSibling(hElement))
            {
                if (hChildElement == hElement)
                {
                    return true;
                }
                else
                {
                    if (this->IsElementDescendant(hElement, hChildElement))
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool GUIContext::IsElementAncestor(HGUIElement hChildElement, HGUIElement hParentElement) const
    {
        auto childElement = this->GetElementPtr(hChildElement);
        if (childElement != nullptr)
        {
            if (childElement->parent == hParentElement)
            {
                return true;
            }
            else
            {
                return this->IsElementAncestor(childElement->parent, hParentElement);
            }
        }

        return false;
    }


    void GUIContext::SetElementPosition(HGUIElement hElement, float xPos, float yPos)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            // When this API is used, we assume it's relative positioning, relative to the top/left corner of the parent.
            GUIElementStyle_Set_positioning(element->style, Positioning_Relative);
            GUIElementStyle_Set_lefthaspriority(element->style, true);
            GUIElementStyle_Set_tophaspriority( element->style, true);
            GUIElementStyle_Set_left(element->style, static_cast<int32_t>(xPos), NumberType_Points);
            GUIElementStyle_Set_top( element->style, static_cast<int32_t>(yPos), NumberType_Points);

            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementPosition(HGUIElement hElement, int xPos, int yPos)
    {
        this->SetElementPosition(hElement, static_cast<float>(xPos), static_cast<float>(yPos));
    }


    void GUIContext::SetElementLeftPosition(HGUIElement hElement, int left)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_left(element->style, left, NumberType_Points);
            GUIElementStyle_Set_lefthaspriority(element->style, true);

            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementTopPosition(HGUIElement hElement, int top)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_top(element->style, top, NumberType_Points);
            GUIElementStyle_Set_tophaspriority(element->style, true);

            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementRightPosition(HGUIElement hElement, int right)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_right(element->style, right, NumberType_Points);
            GUIElementStyle_Set_righthaspriority(element->style, true);

            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBottomPosition(HGUIElement hElement, int bottom)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_bottom(element->style, bottom, NumberType_Points);
            GUIElementStyle_Set_bottomhaspriority(element->style, true);

            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }


    void GUIContext::SetElementRelativePositionOrigin(HGUIElement hElement, PositionOrigin origin)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_positionorigin(element->style, origin);


            if (GUIElementStyle_Get_positioning(element->style) == Positioning_Relative)
            {
                this->BeginBatch();
                {
                    this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
                }
                this->EndBatch();
            }
        }
    }

    PositionOrigin GUIContext::GetElementRelativePositionOrigin(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_positionorigin(element->style);
        }

        return PositionOrigin_Inner;
    }


    void GUIContext::GetElementPosition(HGUIElement hElement, float &xPosOut, float &yPosOut) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            xPosOut = element->layout.relativePosX;
            yPosOut = element->layout.relativePosY;
        }
        else
        {
            xPosOut = 0.0f;
            yPosOut = 0.0f;
        }
    }

    void GUIContext::GetElementPosition(HGUIElement hElement, int &xPosOut, int &yPosOut) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            xPosOut = static_cast<int>(element->layout.relativePosX);
            yPosOut = static_cast<int>(element->layout.relativePosY);
        }
        else
        {
            xPosOut = 0;
            yPosOut = 0;
        }
    }

    void GUIContext::GetElementAbsolutePosition(HGUIElement hElement, float &xPosOut, float &yPosOut) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            xPosOut = element->layout.absolutePosX;
            yPosOut = element->layout.absolutePosY;
        }
        else
        {
            xPosOut = 0.0f;
            yPosOut = 0.0f;
        }
    }

    void GUIContext::GetElementAbsolutePosition(HGUIElement hElement, int &xPosOut, int &yPosOut) const
    {
        float xPosF;
        float yPosF;
        this->GetElementAbsolutePosition(hElement, xPosF, yPosF);

        xPosOut = static_cast<int>(xPosF);
        yPosOut = static_cast<int>(yPosF);
    }

    void GUIContext::SetElementSize(HGUIElement hElement, float width, float height)
    {
        this->Layout_SetElementSize(hElement, static_cast<uint32_t>(width), NumberType_Points, static_cast<uint32_t>(height), NumberType_Points);
    }
    void GUIContext::SetElementSize(HGUIElement hElement, unsigned int width, unsigned int height)
    {
        this->Layout_SetElementSize(hElement, static_cast<uint32_t>(width), NumberType_Points, static_cast<uint32_t>(height), NumberType_Points);
    }
    void GUIContext::SetElementSizeRatio(HGUIElement hElement, float widthRatio, float heightRatio)
    {
        this->Layout_SetElementSize(hElement, static_cast<uint32_t>(widthRatio * 100), NumberType_Percent, static_cast<uint32_t>(heightRatio * 100), NumberType_Percent);
    }
    void GUIContext::SetElementSizeToChildren(HGUIElement hElement)
    {
        this->Layout_SetElementSize(hElement, 0U, NumberType_Auto, 0U, NumberType_Auto);
    }


    void GUIContext::SetElementWidth(HGUIElement hElement, float width)
    {
        this->Layout_SetElementWidth(hElement, static_cast<uint32_t>(width), NumberType_Points);
    }
    void GUIContext::SetElementWidth(HGUIElement hElement, unsigned int width)
    {
        this->Layout_SetElementWidth(hElement, static_cast<uint32_t>(width), NumberType_Points);
    }
    void GUIContext::SetElementWidthRatio(HGUIElement hElement, float widthRatio)
    {
        this->Layout_SetElementWidth(hElement, static_cast<uint32_t>(widthRatio * 100), NumberType_Percent);
    }
    void GUIContext::SetElementWidthToChildren(HGUIElement hElement)
    {
        this->Layout_SetElementWidth(hElement, 0U, NumberType_Auto);
    }

    void GUIContext::SetElementMinWidth(HGUIElement hElement, float minWidth)
    {
        this->Layout_SetElementMinWidth(hElement, static_cast<uint32_t>(minWidth), NumberType_Points);
    }
    void GUIContext::SetElementMinWidth(HGUIElement hElement, unsigned int minWidth)
    {
        this->Layout_SetElementMinWidth(hElement, static_cast<uint32_t>(minWidth), NumberType_Points);
    }
    void GUIContext::SetElementMinWidthRatio(HGUIElement hElement, float minWidthRatio)
    {
        this->Layout_SetElementMinWidth(hElement, static_cast<uint32_t>(minWidthRatio * 100), NumberType_Percent);
    }
    void GUIContext::SetElementMinWidthToChildren(HGUIElement hElement)
    {
        this->Layout_SetElementMinWidth(hElement, 0U, NumberType_Auto);
    }

    void GUIContext::SetElementMaxWidth(HGUIElement hElement, float maxWidth)
    {
        this->Layout_SetElementMaxWidth(hElement, static_cast<uint32_t>(maxWidth), NumberType_Points);
    }
    void GUIContext::SetElementMaxWidth(HGUIElement hElement, unsigned int maxWidth)
    {
        this->Layout_SetElementMaxWidth(hElement, static_cast<uint32_t>(maxWidth), NumberType_Points);
    }
    void GUIContext::SetElementMaxWidthRatio(HGUIElement hElement, float maxWidthRatio)
    {
        this->Layout_SetElementMaxWidth(hElement, static_cast<uint32_t>(maxWidthRatio * 100), NumberType_Percent);
    }
    void GUIContext::SetElementMaxWidthToChildren(HGUIElement hElement)
    {
        this->Layout_SetElementMaxWidth(hElement, 0U, NumberType_Auto);
    }



    void GUIContext::SetElementHeight(HGUIElement hElement, float height)
    {
        this->Layout_SetElementHeight(hElement, static_cast<uint32_t>(height), NumberType_Points);
    }
    void GUIContext::SetElementHeight(HGUIElement hElement, unsigned int height)
    {
        this->Layout_SetElementHeight(hElement, static_cast<uint32_t>(height), NumberType_Points);
    }
    void GUIContext::SetElementHeightRatio(HGUIElement hElement, float heightRatio)
    {
        this->Layout_SetElementHeight(hElement, static_cast<uint32_t>(heightRatio * 100), NumberType_Percent);
    }
    void GUIContext::SetElementHeightToChildren(HGUIElement hElement)
    {
        this->Layout_SetElementHeight(hElement, 0U, NumberType_Auto);
    }

    void GUIContext::SetElementMinHeight(HGUIElement hElement, float minHeight)
    {
        this->Layout_SetElementMinHeight(hElement, static_cast<uint32_t>(minHeight), NumberType_Points);
    }
    void GUIContext::SetElementMinHeight(HGUIElement hElement, unsigned int minHeight)
    {
        this->Layout_SetElementMinHeight(hElement, static_cast<uint32_t>(minHeight), NumberType_Points);
    }
    void GUIContext::SetElementMinHeightRatio(HGUIElement hElement, float minHeightRatio)
    {
        this->Layout_SetElementMinHeight(hElement, static_cast<uint32_t>(minHeightRatio * 100), NumberType_Percent);
    }
    void GUIContext::SetElementMinHeightToChildren(HGUIElement hElement)
    {
        this->Layout_SetElementMinHeight(hElement, 0U, NumberType_Auto);
    }

    void GUIContext::SetElementMaxHeight(HGUIElement hElement, float maxHeight)
    {
        this->Layout_SetElementMaxHeight(hElement, static_cast<uint32_t>(maxHeight), NumberType_Points);
    }
    void GUIContext::SetElementMaxHeight(HGUIElement hElement, unsigned int maxHeight)
    {
        this->Layout_SetElementMaxHeight(hElement, static_cast<uint32_t>(maxHeight), NumberType_Points);
    }
    void GUIContext::SetElementMaxHeightRatio(HGUIElement hElement, float maxHeightRatio)
    {
        this->Layout_SetElementMaxHeight(hElement, static_cast<uint32_t>(maxHeightRatio * 100), NumberType_Percent);
    }
    void GUIContext::SetElementMaxHeightToChildren(HGUIElement hElement)
    {
        this->Layout_SetElementMaxHeight(hElement, 0U, NumberType_Auto);
    }




    float GUIContext::GetElementWidth(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return element->layout.width;
        }

        return 0;
    }

    float GUIContext::GetElementHeight(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return element->layout.height;
        }

        return 0;
    }


    void GUIContext::GetElementAbsoluteRect(HGUIElement hElement, GTLib::Rect<float> &rectOut) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            this->GetElementAbsoluteRect(*element, rectOut);
        }
    }

    void GUIContext::GetElementAbsoluteRect(HGUIElement hElement, GTLib::Rect<int> &rectOut) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            this->GetElementAbsoluteRect(*element, rectOut);
        }
    }


    void GUIContext::SetElementPositioning(HGUIElement hElement, Positioning positioning)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            // If the current positioning is automatic, we need to invalidate the position of the neighbouring auto-positioned element.
            this->BeginBatch();
            {
                if (GUIElementStyle_Get_positioning(element->style) == Positioning_Auto)
                {
                    auto neighboringAutoPositionedElement = this->Layout_GetNeighboringAutoPositionedSibling(*element);
                    if (neighboringAutoPositionedElement != nullptr)
                    {
                        this->Layout_InvalidateElementLayout(*neighboringAutoPositionedElement, LayoutFlag_PositionInvalid);
                    }
                }

                GUIElementStyle_Set_positioning(element->style, positioning);

                this->Layout_InvalidateElementLayout(*element, LayoutFlag_SizeAndPositionInvalid);
            }
            this->EndBatch();
        }
    }

    Positioning GUIContext::GetElementPositioning(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_positioning(element->style);
        }

        return Positioning_Auto;
    }




    void GUIContext::SetElementBackgroundColour(HGUIElement hElement, const GTLib::Colour &colour)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_backgroundcolor(element->style, colour);


            this->BeginBatch();
            {
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    GTLib::Colour GUIContext::GetElementBackgroundColour(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_backgroundcolor(element->style);
        }

        return GTLib::Colour(0.0f, 0.0f, 0.0f);
    }



    void GUIContext::SetElementBorderLeftWidth(HGUIElement hElement, uint32_t width)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_borderleftwidth(element->style, width, NumberType_Points);

            element->layout.borderLeftWidth = static_cast<float>(width) * this->GetXDPIScalingFactor();


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerWidthChange(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderTopWidth(HGUIElement hElement, uint32_t width)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_bordertopwidth(element->style, width, NumberType_Points);

            element->layout.borderTopWidth = static_cast<float>(width) * this->GetYDPIScalingFactor();


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerHeightChange(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderRightWidth(HGUIElement hElement, uint32_t width)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_borderrightwidth(element->style, width, NumberType_Points);

            element->layout.borderRightWidth = static_cast<float>(width) * this->GetXDPIScalingFactor();


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerWidthChange(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderBottomWidth(HGUIElement hElement, uint32_t width)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_borderbottomwidth(element->style, width, NumberType_Points);

            element->layout.borderBottomWidth = static_cast<float>(width) * this->GetYDPIScalingFactor();


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerHeightChange(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderWidth(HGUIElement hElement, uint32_t width)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_borderwidth(element->style, width, NumberType_Points);

            element->layout.borderLeftWidth   = static_cast<float>(width) * this->GetXDPIScalingFactor();
            element->layout.borderTopWidth    = static_cast<float>(width) * this->GetYDPIScalingFactor();
            element->layout.borderRightWidth  = static_cast<float>(width) * this->GetXDPIScalingFactor();
            element->layout.borderBottomWidth = static_cast<float>(width) * this->GetYDPIScalingFactor();

            
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerSizeChange(*element);
            }
            this->EndBatch();
        }
    }


    void GUIContext::SetElementBorderLeftColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_borderleftcolor(element->style, borderColor);


            this->BeginBatch();
            {
                // TODO: Only invalidate the border rectangle instead of the entire element.
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderTopColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_bordertopcolor(element->style, borderColor);


            this->BeginBatch();
            {
                // TODO: Only invalidate the border rectangle instead of the entire element.
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderRightColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_borderrightcolor(element->style, borderColor);


            this->BeginBatch();
            {
                // TODO: Only invalidate the border rectangle instead of the entire element.
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderBottomColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_borderbottomcolor(element->style, borderColor);


            this->BeginBatch();
            {
                // TODO: Only invalidate the border rectangle instead of the entire element.
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderColor(HGUIElement hElement, const GTLib::Colour &borderColor)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_bordercolor(element->style, borderColor);


            this->BeginBatch();
            {
                // TODO: Only invalidate the border rectangle instead of the entire element.
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementBorderLeft(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        this->BeginBatch();
        {
            this->SetElementBorderLeftWidth(hElement, width);
            this->SetElementBorderLeftColor(hElement, color);
        }
        this->EndBatch();
    }

    void GUIContext::SetElementBorderTop(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        this->BeginBatch();
        {
            this->SetElementBorderTopWidth(hElement, width);
            this->SetElementBorderTopColor(hElement, color);
        }
        this->EndBatch();
    }

    void GUIContext::SetElementBorderRight(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        this->BeginBatch();
        {
            this->SetElementBorderRightWidth(hElement, width);
            this->SetElementBorderRightColor(hElement, color);
        }
        this->EndBatch();
    }

    void GUIContext::SetElementBorderBottom(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        this->BeginBatch();
        {
            this->SetElementBorderBottomWidth(hElement, width);
            this->SetElementBorderBottomColor(hElement, color);
        }
        this->EndBatch();
    }

    void GUIContext::SetElementBorder(HGUIElement hElement, uint32_t width, const GTLib::Colour &color)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_border(element->style, width, NumberType_Points, color);

            element->layout.borderLeftWidth   = static_cast<float>(width) * this->GetXDPIScalingFactor();
            element->layout.borderTopWidth    = static_cast<float>(width) * this->GetYDPIScalingFactor();
            element->layout.borderRightWidth  = static_cast<float>(width) * this->GetXDPIScalingFactor();
            element->layout.borderBottomWidth = static_cast<float>(width) * this->GetYDPIScalingFactor();

            
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerSizeChange(*element);
            }
            this->EndBatch();
        }
    }



    void GUIContext::SetElementPaddingLeft(HGUIElement hElement, uint32_t padding)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_paddingleft(element->style, padding, NumberType_Points);

            element->layout.paddingLeft = static_cast<float>(padding) * this->GetXDPIScalingFactor();


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerWidthChange(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementPaddingTop(HGUIElement hElement, uint32_t padding)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_paddingtop(element->style, padding, NumberType_Points);

            element->layout.paddingTop = static_cast<float>(padding) * this->GetYDPIScalingFactor();


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerHeightChange(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementPaddingRight(HGUIElement hElement, uint32_t padding)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_paddingright(element->style, padding, NumberType_Points);

            element->layout.paddingRight = static_cast<float>(padding) * this->GetXDPIScalingFactor();


            this->EndBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerWidthChange(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementPaddingBottom(HGUIElement hElement, uint32_t padding)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_paddingbottom(element->style, padding, NumberType_Points);

            element->layout.paddingBottom = static_cast<float>(padding) * this->GetYDPIScalingFactor();


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerHeightChange(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementPadding(HGUIElement hElement, uint32_t padding)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_padding(element->style, padding, NumberType_Points);

            element->layout.paddingLeft   = static_cast<float>(padding) * this->GetXDPIScalingFactor();
            element->layout.paddingTop    = static_cast<float>(padding) * this->GetYDPIScalingFactor();
            element->layout.paddingRight  = static_cast<float>(padding) * this->GetXDPIScalingFactor();
            element->layout.paddingBottom = static_cast<float>(padding) * this->GetYDPIScalingFactor();


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnInnerSizeChange(*element);
            }
            this->EndBatch();
        }
    }



    void GUIContext::SetElementMarginLeft(HGUIElement hElement, uint32_t margin)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_marginleft(element->style, margin, NumberType_Points);

            element->layout.marginLeft = static_cast<float>(margin) * this->GetXDPIScalingFactor();


            // The margin will change this element's position.
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementMarginTop(HGUIElement hElement, uint32_t margin)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_margintop(element->style, margin, NumberType_Points);

            element->layout.marginTop = static_cast<float>(margin) * this->GetYDPIScalingFactor();


            // The margin will change this element's position.
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementMarginRight(HGUIElement hElement, uint32_t margin)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_marginright(element->style, margin, NumberType_Points);

            element->layout.marginRight = static_cast<float>(margin) * this->GetXDPIScalingFactor();


            // The margin will change this element's position.
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementMarginBottom(HGUIElement hElement, uint32_t margin)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_marginbottom(element->style, margin, NumberType_Points);

            element->layout.marginBottom = static_cast<float>(margin) * this->GetYDPIScalingFactor();


            // The margin will change this element's position.
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::SetElementMargin(HGUIElement hElement, uint32_t margin)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_margin(element->style, margin, NumberType_Points);

            element->layout.marginLeft   = static_cast<float>(margin) * this->GetXDPIScalingFactor();
            element->layout.marginTop    = static_cast<float>(margin) * this->GetYDPIScalingFactor();
            element->layout.marginRight  = static_cast<float>(margin) * this->GetXDPIScalingFactor();
            element->layout.marginBottom = static_cast<float>(margin) * this->GetYDPIScalingFactor();


            // The margin will change this element's position.
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }



    void GUIContext::SetElementChildAxis(HGUIElement hElement, ChildAxis axis)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_childaxis(element->style, axis);

            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnChildAxisChange(*element);
            }
            this->EndBatch();
        }
    }

    ChildAxis GUIContext::GetElementChildAxis(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_childaxis(element->style);
        }

        return ChildAxis_Vertical;
    }


    void GUIContext::SetElementHorizontalAlign(HGUIElement hElement, HorizontalAlign align)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_horizontalaling(element->style, align);

            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnHorizontalAlignChange(*element);
            }
            this->EndBatch();
        }
    }

    HorizontalAlign GUIContext::GetElementHorizontalAlign(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_horizontalalign(element->style);
        }

        return HorizontalAlign_Left;
    }


    void GUIContext::SetElementVerticalAlign(HGUIElement hElement, VerticalAlign align)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_verticalaling(element->style, align);

            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayoutsOnVerticalAlignChange(*element);
            }
            this->EndBatch();
        }
    }

    VerticalAlign GUIContext::GetElementVerticalAlign(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_verticalalign(element->style);
        }

        return VerticalAlign_Top;
    }



    void GUIContext::SetElementChildrenWidthBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_childrenwidthboundary(element->style, boundary);


            // All children with relative widths need updating.
            this->BeginBatch();
            {
                for (HGUIElement hChild = element->firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
                {
                    auto child = this->GetElementPtr(hChild);
                    assert(child != nullptr);
                    {
                        if (GUIElementStyle_Get_IsAnyWidthOfType(child->style, NumberType_Percent))
                        {
                            this->Layout_InvalidateElementLayout(*child, LayoutFlag_PositionInvalid | LayoutFlag_WidthInvalid);
                        }
                    }
                }
            }
            this->EndBatch();
        }
    }

    ChildrenSizeBoundary GUIContext::GetElementChildrenWidthBoundary(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_childrenwidthboundary(element->style);
        }

        return ChildrenSizeBoundary_Inner;
    }

    void GUIContext::SetElementChildrenHeightBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_childrenheightboundary(element->style, boundary);


            // All children with relative widths need updating.
            this->BeginBatch();
            {
                for (HGUIElement hChild = element->firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
                {
                    auto child = this->GetElementPtr(hChild);
                    assert(child != nullptr);
                    {
                        if (GUIElementStyle_Get_IsAnyHeightOfType(child->style, NumberType_Percent))
                        {
                            this->Layout_InvalidateElementLayout(*child, LayoutFlag_PositionInvalid | LayoutFlag_HeightInvalid);
                        }
                    }
                }
            }
            this->EndBatch();
        }
    }

    ChildrenSizeBoundary GUIContext::GetElementChildrenHeightBoundary(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_childrenheightboundary(element->style);
        }

        return ChildrenSizeBoundary_Inner;
    }

    void GUIContext::SetElementChildrenSizeBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_childrenwidthboundary( element->style, boundary);
            GUIElementStyle_Set_childrenheightboundary(element->style, boundary);


            // All children with relative widths need updating.
            this->BeginBatch();
            {
                for (HGUIElement hChild = element->firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
                {
                    auto child = this->GetElementPtr(hChild);
                    assert(child != nullptr);
                    {
                        int invalidLayoutFlags = 0;

                        if (GUIElementStyle_Get_IsAnyWidthOfType(child->style, NumberType_Percent))
                        {
                            invalidLayoutFlags |= LayoutFlag_WidthInvalid;
                        }
                        if (GUIElementStyle_Get_IsAnyWidthOfType(child->style, NumberType_Percent))
                        {
                            invalidLayoutFlags |= LayoutFlag_HeightInvalid;
                        }


                        if (invalidLayoutFlags != 0)
                        {
                            this->Layout_InvalidateElementLayout(*child, LayoutFlag_PositionInvalid | invalidLayoutFlags);
                        }
                    }
                }
            }
            this->EndBatch();
        }
    }



    void GUIContext::SetElementClippingBoundary(HGUIElement hElement, ClippingBoundary boundary)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_clippingboundary(element->style, boundary);

            this->BeginBatch();
            {
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    ClippingBoundary GUIContext::GetElementClippingBoundary(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_clippingboundary(element->style);
        }

        return ClippingBoundary_Inner;
    }


    void GUIContext::SetElementClippingMode(HGUIElement hElement, ClippingMode clippingMode)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_clippingmode(element->style, clippingMode);

            this->BeginBatch();
            {
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    ClippingMode GUIContext::GetElementClippingMode(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_clippingmode(element->style);
        }

        return ClippingMode_Auto;
    }



    void GUIContext::SetElementVisibility(HGUIElement hElement, bool visible)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_visible(element->style, visible);
        }
    }

    bool GUIContext::IsElementVisible(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return this->IsElementVisible(*element);
        }

        return false;
    }


    void GUIContext::EnableElementChildWidthFlexing(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_flexchildrenwidth(element->style, true);


            this->BeginBatch();
            {
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::DisableElementChildWidthFlexing(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_flexchildrenwidth(element->style, false);


            this->BeginBatch();
            {
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    bool GUIContext::IsElementChildWidthFlexingEnabled(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_flexchildrenwidth(element->style);
        }

        return false;
    }


    void GUIContext::EnableElementChildHeightFlexing(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_flexchildrenheight(element->style, true);


            this->BeginBatch();
            {
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    void GUIContext::DisableElementChildHeightFlexing(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_flexchildrenheight(element->style, false);


            this->BeginBatch();
            {
                this->Painting_InvalidateElementRect(*element);
            }
            this->EndBatch();
        }
    }

    bool GUIContext::IsElementChildHeightFlexingEnabled(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return GUIElementStyle_Get_flexchildrenheight(element->style);
        }

        return false;
    }


    void GUIContext::SetElementText(HGUIElement hElement, const char* text)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            element->textManager.SetText(text);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_TextInvalid);
            }
            this->EndBatch();
        }
    }

    const char* GUIContext::GetElementText(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return element->textManager.GetText();
        }

        return nullptr;
    }

    bool GUIContext::DoesElementHaveText(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            return element->textManager.HasText();
        }

        return false;
    }


    const GTLib::Font* GUIContext::SetElementFont(HGUIElement hElement, const char* family, FontWeight weight, FontSlant slant, uint32_t size, uint32_t sizeType)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            auto oldFont = this->GetElementFont(hElement);

            // 1) Update the styles.
            GUIElementStyle_Set_fontfamily(element->style, m_fontManager.EncodeFontFamily(family));
            GUIElementStyle_Set_fontweight(element->style, weight);
            GUIElementStyle_Set_fontslant(element->style, slant);
            GUIElementStyle_Set_fontsize(element->style, size, sizeType);

            // 2) Update the font based on the current style.
            auto newFont = this->UpdateElementFontFromStyle(*element);

            // 3) Update the layout, but only if the font is different.
            if (newFont != oldFont)
            {
                this->BeginBatch();
                {
                    this->Layout_InvalidateElementLayout(hElement, LayoutFlag_TextInvalid);
                }
                this->EndBatch();
            }


            return this->GetElementFont(hElement);
        }

        return nullptr;
    }

    const GTLib::Font* GUIContext::GetElementFont(HGUIElement hElement) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            if (element->textManager.GetDefaultFont() != nullptr)
            {
                return element->textManager.GetDefaultFont();
            }
            else
            {
                return m_defaultFont;
            }
        }

        return nullptr;
    }


#if 0
    bool GUIContext::SetElementSurface(HGUIElement hElement, HGUISurface hSurface)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            if (element->parent == 0)
            {
                auto surface = this->GetSurfacePtr(hSurface);
                if (surface != nullptr)
                {
                    if (!this->DoesSurfaceContainGUIElement(hSurface, hElement))
                    {
                        surface->topLevelElements.PushBack(hElement);

                        // The layout may need to be updated.
                        this->BeginBatch();
                        {
                            this->Layout_InvalidateElementLayout(*element, LayoutFlag_WidthInvalid | LayoutFlag_HeightInvalid | LayoutFlag_PositionInvalid);
                        }
                        this->EndBatch();
                    }
                }
            }
            else
            {
                return false;
            }
        }

        return false;
    }

    HGUISurface GUIContext::GetElementSurface(HGUIElement hElement) const
    {
        HGUISurface hElementSurface = 0;
        m_surfaceHandles.IterateAssociatedObjects([&](GUISurface* surface) -> bool
        {
            if (this->DoesSurfaceContainGUIElement(surface->handle, hElement))
            {
                hElementSurface = surface->handle;
                return false;   // break
            }

            return true;    // continue
        });


        return hElementSurface;
    }
#endif

    bool GUIContext::AttachElementToSurface(HGUIElement hElement, HGUISurface hSurface)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->parent == 0)
            {
                if (pElement->hSurface != hSurface)
                {
                    // The element must be detached from the old surface first.
                    if (pElement->hSurface != 0)
                    {
                        this->DetachElementFromSurface(hElement);
                    }


                    // The element needs to be set as a top-level element on the surface.
                    auto pSurface = this->GetSurfacePtr(hSurface);
                    if (pSurface != nullptr)
                    {
                        pSurface->topLevelElements.PushBack(hElement);
                    }


                    // Now the surface must be set recursively on the children.
                    this->SetElementSurfaceRecursive(*pElement, hSurface);


                    return true;
                }
                else
                {
                    // The old and new surface are both the same, so leave everything as-is.
                    return true;
                }
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

    bool GUIContext::DetachElementFromSurface(HGUIElement hElement)
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            if (pElement->parent == 0)
            {
                if (pElement->hSurface != 0)
                {
                    // The element needs to be removed as a top-level element on the surface.
                    auto pSurface = this->GetSurfacePtr(pElement->hSurface);
                    if (pSurface != nullptr)
                    {
                        pSurface->topLevelElements.RemoveFirstOccuranceOf(hElement);
                    }


                    // The element must be set recursively on the children.
                    this->SetElementSurfaceRecursive(*pElement, 0);


                    return true;
                }
                else
                {
                    // The element isn't attached to a surface, so just leave everything as-is and pretend all is good.
                    return true;
                }
            }
            else
            {
                // The element is not a root level element.
                return false;
            }
        }
        else
        {
            // The element is null.
            return false;
        }
    }

    HGUISurface GUIContext::GetElementSurface(HGUIElement hElement) const
    {
        auto pElement = this->GetElementPtr(hElement);
        if (pElement != nullptr)
        {
            return pElement->hSurface;
        }

        return 0;
    }



    void GUIContext::BeginBatch()
    {
        ++m_batchLockCounter;
    }

    void GUIContext::EndBatch()
    {
        if (m_batchLockCounter > 0)
        {
            --m_batchLockCounter;


            // If the counter is now at zero, validate the layouts.
            if (m_batchLockCounter == 0)
            {
                this->Layout_ValidateElementLayouts();
                this->Painting_PaintAndValidateSurfaceRects();
            }
        }
    }



    ////////////////////////////////////////////////////////////////
    // Painting

    void GUIContext::InvalidateSurfaceRect(HGUISurface hSurface, const GTLib::Rect<int> &rect)
    {
        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            this->Painting_InvalidateRect(*surface, rect);
        }
    }

    void GUIContext::PaintSurface(HGUISurface hSurface, const GTLib::Rect<int> &rect)
    {
        assert(m_renderer != nullptr);

        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            m_renderer->BeginPaintSurface(hSurface);
            {
                m_renderer->Clear(rect);

                for (size_t iElement = 0; iElement < surface->topLevelElements.GetCount(); ++iElement)
                {
                    auto topLevelElement = this->GetElementPtr(surface->topLevelElements[iElement]);
                    if (topLevelElement != nullptr)
                    {
                        this->ClippedTraversal(*topLevelElement, rect, [&](GUIElement &elementToPaint, const GTLib::Rect<int> &visibleRect) -> void
                        {
                            this->Painting_PaintElement(*surface, elementToPaint, visibleRect);
                        });
                    }
                }


                this->PostEvent_OnPaint(hSurface, rect);
            }
            m_renderer->EndPaintSurface();
        }
    }

    void GUIContext::PaintSurface(HGUISurface hSurface)
    {
        unsigned int surfaceWidth;
        unsigned int surfaceHeight;
        this->GetSurfaceSize(hSurface, surfaceWidth, surfaceHeight);

        GTLib::Rect<int> paintRect;
        paintRect.left   = 0;
        paintRect.top    = 0;
        paintRect.right  = static_cast<int>(surfaceWidth);
        paintRect.bottom = static_cast<int>(surfaceHeight);
        this->PaintSurface(hSurface, paintRect);
    }


    /////////////////////////////////////////////////////////////////
    // Events

    void GUIContext::AttachLocalEventHandler(HGUIElement hElement, GUIEventHandler &eventHandler)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            element->eventHandlers.PushBack(&eventHandler);
        }
    }

    void GUIContext::DetachLocalEventHandler(HGUIElement hElement, GUIEventHandler &eventHandler)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            element->eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
        }
    }

    void GUIContext::AttachGlobalEventHandler(GUIEventHandler &eventHandler)
    {
        m_globalEventHandlers.PushBack(&eventHandler);
    }

    void GUIContext::DetachGlobalEventHandler(GUIEventHandler &eventHandler)
    {
        m_globalEventHandlers.RemoveFirstOccuranceOf(&eventHandler);
    }


    ////////////////////////////////////////////////////////////////
    // Inbound Events

    void GUIContext::OnMouseMove(HGUISurface hSurface, int mousePosX, int mousePosY)
    {
        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            auto hNewElementUnderMouse = this->FindElementUnderPoint(hSurface, mousePosX, mousePosY);
            
            // OnMouseLeave/OnMouseEnter
            if (surface->hElementUnderMouse != hNewElementUnderMouse)
            {
                auto oldElementUnderMouse = this->GetElementPtr(surface->hElementUnderMouse);
                if (oldElementUnderMouse != nullptr)
                {
                    this->PostEvent_OnMouseLeave(oldElementUnderMouse->handle);
                }

                auto newElementUnderMouse = this->GetElementPtr(hNewElementUnderMouse);
                if (newElementUnderMouse != nullptr)
                {
                    this->PostEvent_OnMouseEnter(newElementUnderMouse->handle);
                }


                surface->hElementUnderMouse = hNewElementUnderMouse;
            }


            // OnMouseMove
            //
            // If an element is capturing mouse events, it needs to be the one to receive the event.
            GUIElement* eventReceiver = nullptr;
            if (surface->hElementCapturingMouseEvents != 0)
            {
                auto elementCapturingMouseEvents = this->GetElementPtr(surface->hElementCapturingMouseEvents);
                if (elementCapturingMouseEvents != nullptr)
                {
                    eventReceiver = elementCapturingMouseEvents;
                }
                else
                {
                    eventReceiver = this->GetElementPtr(hNewElementUnderMouse);
                }
            }
            else
            {
                eventReceiver = this->GetElementPtr(hNewElementUnderMouse);
            }

            if (eventReceiver != nullptr)
            {
                // Need to convert the point to local coordinates.
                int relativeMousePosX;
                int relativeMousePosY;
                this->AbsoluteToRelative(eventReceiver->handle, mousePosX, mousePosY, relativeMousePosX, relativeMousePosY);

                this->PostEvent_OnMouseMove(eventReceiver->handle, relativeMousePosX, relativeMousePosY);
            }
        }
    }



    ////////////////////////////////////////////////////////////////
    // Searching/Gathering

    HGUIElement GUIContext::FindElementUnderPoint(HGUISurface hSurface, int x, int y)
    {
        HGUIElement hElementUnderPoint = 0;

        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            GTLib::Rect<float> rect;
            rect.left   = 0;
            rect.top    = 0;
            rect.right  = static_cast<float>(surface->width);
            rect.bottom = static_cast<float>(surface->height);

            for (size_t iElement = 0; iElement < surface->topLevelElements.GetCount(); ++iElement)
            {
                auto topLevelElement = this->GetElementPtr(surface->topLevelElements[iElement]);
                if (topLevelElement != nullptr)
                {
                    this->ClippedTraversal(*topLevelElement, rect, [&](GUIElement &element, const GTLib::Rect<int> &visibleRect) -> void
                    {
                        if (visibleRect.Contains(x, y))
                        {
                            hElementUnderPoint = element.handle;
                        }
                    });
                }
            }
        }

        return hElementUnderPoint;
    }



    ////////////////////////////////////////////////////////////////
    // DPI / Scaling

    unsigned int GUIContext::GetBaseXDPI() const
    {
        return m_xBaseDPI;
    }

    unsigned int GUIContext::GetBaseYDPI() const
    {
        return m_yBaseDPI;
    }

    void GUIContext::GetBaseDPI(unsigned int &xBaseDPIOut, unsigned int &yBaseDPIOut) const
    {
        xBaseDPIOut = m_xBaseDPI;
        yBaseDPIOut = m_yBaseDPI;
    }

    void GUIContext::SetBaseDPI(unsigned int xBaseDPI, unsigned int yBaseDPI)
    {
        m_xBaseDPI = xBaseDPI;
        m_yBaseDPI = yBaseDPI;

        this->UpdateAllElementsOnDPIChange();
    }


    unsigned int GUIContext::GetXDPI() const
    {
        return m_xDPI;
    }

    unsigned int GUIContext::GetYDPI() const
    {
        return m_yDPI;
    }

    void GUIContext::GetDPI(unsigned int &xDPIOut, unsigned int &yDPIOut)
    {
        xDPIOut = m_xDPI;
        yDPIOut = m_yDPI;
    }

    void GUIContext::SetDPI(unsigned int xDPI, unsigned int yDPI)
    {
        m_xDPI = xDPI;
        m_yDPI = yDPI;

        this->UpdateAllElementsOnDPIChange();
    }


    float GUIContext::GetXDPIScalingFactor() const
    {
        return static_cast<float>(m_xDPI) / static_cast<float>(m_xBaseDPI);
    }

    float GUIContext::GetYDPIScalingFactor() const
    {
        return static_cast<float>(m_yDPI) / static_cast<float>(m_yBaseDPI);
    }



    ////////////////////////////////////////////////////////////////
    // Utilities

    void GUIContext::AbsoluteToRelative(HGUIElement hElement, int xIn, int yIn, int &xOut, int &yOut) const
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            xOut = static_cast<int>(xIn - element->layout.absolutePosX);
            yOut = static_cast<int>(yIn - element->layout.absolutePosY);
        }
        else
        {
            xOut = xIn;
            yOut = yIn;
        }
    }


    ////////////////////////////////////////////////////////////////
    // Private

    GUISurface* GUIContext::GetSurfacePtr(HGUISurface hSurface) const
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

    GUIElement* GUIContext::GetElementPtr(HGUIElement hElement) const
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


    void GUIContext::SetElementSurfaceRecursive(GUIElement &element, HGUISurface hSurface)
    {
        element.hSurface = hSurface;

        // The layout may need to be updated.
        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(element, LayoutFlag_WidthInvalid | LayoutFlag_HeightInvalid | LayoutFlag_PositionInvalid);
        }
        this->EndBatch();


        // Now we update the children.
        for (HGUIElement hChild = element.firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
        {
            auto pChild = this->GetElementPtr(hChild);
            assert(pChild != nullptr);
            {
                this->SetElementSurfaceRecursive(*pChild, hSurface);
            }
        }
    }


    void GUIContext::GetElementAbsoluteRect(GUIElement &element, GTLib::Rect<float> &rectOut) const
    {
        rectOut.left   = element.layout.absolutePosX;
        rectOut.top    = element.layout.absolutePosY;
        rectOut.right  = rectOut.left + element.layout.width;
        rectOut.bottom = rectOut.top  + element.layout.height;
    }

    void GUIContext::GetElementAbsoluteRect(GUIElement &element, GTLib::Rect<int> &rectOut) const
    {
        rectOut.left   = static_cast<int>(GTLib::Round(element.layout.absolutePosX));
        rectOut.top    = static_cast<int>(GTLib::Round(element.layout.absolutePosY));
        rectOut.right  = static_cast<int>(GTLib::Round(element.layout.absolutePosX + element.layout.width));
        rectOut.bottom = static_cast<int>(GTLib::Round(element.layout.absolutePosY + element.layout.height));
    }


    void GUIContext::GetElementChildrenClippingRect(GUIElement &element, GTLib::Rect<float> &rectOut) const
    {
        this->GetElementAbsoluteRect(element, rectOut);

        // Pull in based on the boundary.
        switch (GUIElementStyle_Get_clippingboundary(element.style))
        {
        case ClippingBoundary_Outer:
            {
                // Don't pull in at all.
                break;
            }

        case ClippingBoundary_InnerBorder:
            {
                // Pull in by just the border.
                rectOut.left   += element.layout.borderLeftWidth;
                rectOut.top    += element.layout.borderTopWidth;
                rectOut.right  -= element.layout.borderRightWidth;
                rectOut.bottom -= element.layout.borderBottomWidth;

                break;
            }

        case ClippingBoundary_Inner:
        default:
            {
                // Pull in by both the padding and the border.
                rectOut.left   += element.layout.borderLeftWidth   + element.layout.paddingLeft;
                rectOut.top    += element.layout.borderTopWidth    + element.layout.paddingTop;
                rectOut.right  -= element.layout.borderRightWidth  + element.layout.paddingRight;
                rectOut.bottom -= element.layout.borderBottomWidth + element.layout.paddingBottom;

                break;
            }
        }
    }

    void GUIContext::GetElementChildrenClippingRect(GUIElement &element, GTLib::Rect<int> &rectOut) const
    {
        GTLib::Rect<float> rectF;
        this->GetElementChildrenClippingRect(element, rectF);

        rectOut.left   = static_cast<int>(GTLib::Round(rectF.left));
        rectOut.top    = static_cast<int>(GTLib::Round(rectF.top));
        rectOut.right  = static_cast<int>(GTLib::Round(rectF.right));
        rectOut.bottom = static_cast<int>(GTLib::Round(rectF.bottom));
    }


    bool GUIContext::IsElementVisible(GUIElement &element) const
    {
        if (!GUIElementStyle_Get_visible(element.style))
        {
            return false;
        }

        auto parent = this->GetElementPtr(element.parent);
        if (parent != nullptr)
        {
            return this->IsElementVisible(*parent);
        }

        return true;
    }


    bool GUIContext::IsElementClippedAgainstParent(GUIElement &element) const
    {
        ClippingMode mode = GUIElementStyle_Get_clippingmode(element.style);
        if (mode == ClippingMode_Auto)
        {
            if (GUIElementStyle_Get_positioning(element.style) != Positioning_Absolute)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            // The clipping mode is explicitly set.
            if (mode == ClippingMode_Disabled)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }

    void GUIContext::ClippedTraversal(GUIElement &element, const GTLib::Rect<float> &clippingRect, std::function<void (GUIElement &, const GTLib::Rect<int> &)> func)
    {
        GTLib::Rect<float> elementRect;
        this->GetElementAbsoluteRect(element, elementRect);

        // If clipping is disabled on the child we don't care about constraining the visible region - the entire rectangle will be classed as visible.
        GTLib::Rect<float> elementVisibleRect;
        if (this->IsElementClippedAgainstParent(element))
        {
            elementVisibleRect = GTLib::Rect<float>::Clamp(elementRect, clippingRect);
        }
        else
        {
            elementVisibleRect = elementRect;
        }



        // If the visible rectangle has some volume we can call the function and then traverse the children.
        if (elementVisibleRect.left < elementVisibleRect.right && elementVisibleRect.top < elementVisibleRect.bottom)
        {
            func(element, GTLib::Rect<int>(elementVisibleRect));


            // Now we need to recursively call this method again, only this time we need to pass in a new clipping rectangle. The rectangle will be determined
            // by the child clipping mode, which controls whether or not children are clipped against padding + border, only the border, or only the outside.
            //
            // To calculate this clipping rectangle, we start with the unclamped rectangle. We then pull it in based on our clipping boundary. Finally, we clamp
            // against the original clipping rectangle.
            GTLib::Rect<float> childClippingRect;
            this->GetElementChildrenClippingRect(element, childClippingRect);

            // Clamp against the original clipping rectangle.
            childClippingRect.Clamp(clippingRect);


            // We now need to traverse the children, but only if they themselves are not clipped.
            bool doesChildClippingRectangleHaveVolume = childClippingRect.left < childClippingRect.right && childClippingRect.top < childClippingRect.bottom;
            for (HGUIElement hChild = element.firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
            {
                auto child = this->GetElementPtr(hChild);
                assert(child != nullptr);
                {
                    if (!this->IsElementClippedAgainstParent(*child) || doesChildClippingRectangleHaveVolume)
                    {
                        this->ClippedTraversal(*child, childClippingRect, func);
                    }
                }
            }
        }
    }


    const GTLib::Font* GUIContext::UpdateElementFontFromStyle(GUIElement &element)
    {
        const char* family = m_fontManager.DecodeFontFamily(GUIElementStyle_Get_fontfamily(element.style));
        FontWeight  weight = GUIElementStyle_Get_fontweight(element.style);
        FontSlant   slant  = GUIElementStyle_Get_fontslant(element.style);

        uint32_t sizeType;
        uint32_t size = GUIElementStyle_Get_fontsize(element.style, sizeType);


        // We now have enough information to generate a font from a font info structure.
        GTLib::FontInfo fi;
        fi.family = family;
        fi.dpiX = m_xDPI;
        fi.dpiY = m_yDPI;
        
        if (sizeType == NumberType_Points)
        {
            fi.sizeInPoints = static_cast<float>(size) * this->GetXDPIScalingFactor();
            fi.sizeInPixels = 0;
        }
        else
        {
            fi.sizeInPoints = 0;
            fi.sizeInPixels = static_cast<float>(size) * this->GetYDPIScalingFactor();
        }


        switch (weight)
        {
        case FontWeight_Thin:       fi.styleFlags |= GTLib::FontStyle_Thin;       break;
        case FontWeight_ExtraLight: fi.styleFlags |= GTLib::FontStyle_ExtraLight; break;
        case FontWeight_Light:      fi.styleFlags |= GTLib::FontStyle_Light;      break;
        case FontWeight_SemiBold:   fi.styleFlags |= GTLib::FontStyle_SemiBold;   break;
        case FontWeight_Bold:       fi.styleFlags |= GTLib::FontStyle_Bold;       break;
        case FontWeight_ExtraBold:  fi.styleFlags |= GTLib::FontStyle_ExtraBold;  break;
        case FontWeight_Heavy:      fi.styleFlags |= GTLib::FontStyle_Heavy;      break;

        case FontWeight_Medium:
        default:
            {
                fi.styleFlags |= GTLib::FontStyle_Medium;
                break;
            }
        }


        switch (slant)
        {
        case FontSlant_Italic:  fi.styleFlags |= GTLib::FontStyle_Italic;  break;
        case FontSlant_Oblique: fi.styleFlags |= GTLib::FontStyle_Oblique; break;

        case FontSlant_None:
        default:
            {
                break;
            }
        }


        auto font = m_fontManager.AcquireFont(fi);
        if (font != nullptr)
        {
            element.textManager.SetDefaultFont(font);
            return font;
        }

        return this->GetElementFont(element.handle);
    }



    //////////////////////////////////////////////////
    // Direct element hierarchy operations.

    void GUIContext::AppendChildElement(GUIElement &parentElement, GUIElement &childElement)
    {
        this->BeginBatch();
        {
            // If the parent already has children, we can just append the child element to the last child.
            if (parentElement.lastChild != 0)
            {
                // The parent has children.

                assert(parentElement.firstChild != 0);
            
                auto lastChild = this->GetElementPtr(parentElement.lastChild);
                assert(lastChild != nullptr);
                {
                    this->AppendSiblingElement(*lastChild, childElement);
                }
            }
            else
            {
                // The parent does not have children.

                assert(parentElement.firstChild == 0);


                // The child element needs to be detached from it's current siblings and parent.
                this->DetachElementFromParentAndSiblings(childElement);


                // Because the parent did not have children earlier, the new child element will be both the first and last child.
                parentElement.firstChild = childElement.handle;
                parentElement.lastChild  = childElement.handle;

                // Make sure the child knows who it's new parent is.
                childElement.parent  = parentElement.handle;


                // Invalidate the applicable layouts.
                if (GUIElementStyle_Get_IsAnyWidthOfType(parentElement.style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(parentElement, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(parentElement.style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(parentElement, LayoutFlag_HeightInvalid);
                }

                //TODO: Only invalidate the properties that will have changed.
                this->Layout_InvalidateElementLayout(childElement, LayoutFlag_SizeAndPositionInvalid);
            }


            // If the child was on a different surface to the parent, it needs to be moved.
            if (parentElement.hSurface != childElement.hSurface)
            {
                this->SetElementSurfaceRecursive(childElement, parentElement.hSurface);
            }
        }
        this->EndBatch();
    }

    void GUIContext::PrependChildElement(GUIElement &parentElement, GUIElement &childElement)
    {
        this->BeginBatch();
        {
            // If the parent already has children, we can just append the child element to the last child.
            if (parentElement.firstChild != 0)
            {
                // The parent has children.

                assert(parentElement.lastChild != 0);
            
                auto firstChild = this->GetElementPtr(parentElement.firstChild);
                assert(firstChild != nullptr);
                {
                    this->PrependSiblingElement(*firstChild, childElement);
                }
            }
            else
            {
                // The parent does not have children.

                assert(parentElement.lastChild == 0);


                // The child element needs to be detached from it's current siblings and parent.
                this->DetachElementFromParentAndSiblings(childElement);


                // Because the parent did not have children earlier, the new child element will be both the first and last child.
                parentElement.firstChild = childElement.handle;
                parentElement.lastChild  = childElement.handle;

                // Make sure the child knows who it's new parent is.
                childElement.parent  = parentElement.handle;


                // Invalidate the applicable layouts.
                if (GUIElementStyle_Get_IsAnyWidthOfType(parentElement.style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(parentElement, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(parentElement.style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(parentElement, LayoutFlag_HeightInvalid);
                }

                // Invalidate the applicable layouts. TODO: Only invalidate the properties that will have changed.
                this->Layout_InvalidateElementLayout(childElement, LayoutFlag_SizeAndPositionInvalid);
            }


            // If the child was on a different surface to the parent, it needs to be moved.
            if (parentElement.hSurface != childElement.hSurface)
            {
                this->SetElementSurfaceRecursive(childElement, parentElement.hSurface);
            }
        }
        this->EndBatch();
    }

    void GUIContext::AppendSiblingElement(GUIElement &elementToAppendTo, GUIElement &elementToAppend)
    {
        this->BeginBatch();
        {
            // We want to detach the element from it's current siblings and parent first.
            this->DetachElementFromParentAndSiblings(elementToAppend);


            // The new parent needs to be that of the sibling that's being appended to.
            elementToAppend.parent = elementToAppendTo.parent;


            // The parent has been set, so now it's time to link to it's new siblings.
            HGUIElement hElementToPrependTo = elementToAppendTo.nextSibling;

            elementToAppend.prevSibling = elementToAppendTo.handle;
            elementToAppendTo.nextSibling = elementToAppend.handle;
        
            if (hElementToPrependTo != 0)
            {
                auto elementToPrependTo = this->GetElementPtr(hElementToPrependTo);
                assert(elementToPrependTo != nullptr);
                {
                    elementToAppend.nextSibling = elementToPrependTo->handle;
                    elementToPrependTo->prevSibling = elementToAppend.handle;
                }
            }
            else
            {
                // The new element is the last child of the parent.
                auto parent = this->GetElementPtr(elementToAppend.parent);
                if (parent != nullptr)
                {
                    parent->lastChild = elementToAppend.handle;
                }
            }


            // Invalidate the applicable layouts.
            auto parent = this->GetElementPtr(elementToAppend.parent);
            if (parent != nullptr)
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_HeightInvalid);
                }
            }

            //TODO: Only invalidate the properties that will have changed.
            this->Layout_InvalidateElementLayout(elementToAppendTo, LayoutFlag_SizeAndPositionInvalid);
            this->Layout_InvalidateElementLayout(elementToAppend,   LayoutFlag_SizeAndPositionInvalid);


            // If the elements are on different surfaces the appendee needs to be moved.
            if (elementToAppendTo.hSurface != elementToAppend.hSurface)
            {
                this->SetElementSurfaceRecursive(elementToAppend, elementToAppendTo.hSurface);
            }
        }
        this->EndBatch();
    }

    void GUIContext::PrependSiblingElement(GUIElement &elementToPrependTo, GUIElement &elementToPrepend)
    {
        this->BeginBatch();
        {
            // We want to detach the element from it's current siblings and parent first.
            this->DetachElementFromParentAndSiblings(elementToPrepend);


            // The new parent needs to be that of the siblings that's being prepended to.
            elementToPrepend.parent = elementToPrependTo.parent;


            // The parent has been set, so now it's time to link to it's new sibling.
            HGUIElement hElementToAppendTo = elementToPrependTo.prevSibling;

            elementToPrepend.nextSibling = elementToPrependTo.handle;
            elementToPrependTo.prevSibling = elementToPrepend.handle;

            if (hElementToAppendTo != 0)
            {
                auto elementToAppendTo = this->GetElementPtr(hElementToAppendTo);
                assert(elementToAppendTo != nullptr);
                {
                    elementToPrepend.prevSibling = elementToAppendTo->handle;
                    elementToAppendTo->nextSibling = elementToPrepend.handle;
                }
            }
            else
            {
                // The new element is the first child of the parent.
                auto parent = this->GetElementPtr(elementToPrepend.parent);
                if (parent != nullptr)
                {
                    parent->firstChild = elementToPrepend.handle;
                }
            }


            // Invalidate the applicable layouts.
            auto parent = this->GetElementPtr(elementToPrepend.parent);
            if (parent != nullptr)
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_HeightInvalid);
                }
            }

            //TODO: Only invalidate the properties that will have changed.
            this->Layout_InvalidateElementLayout(elementToPrependTo, LayoutFlag_SizeAndPositionInvalid);
            this->Layout_InvalidateElementLayout(elementToPrepend,   LayoutFlag_SizeAndPositionInvalid);


            // If the elements are on different surfaces the appendee needs to be moved.
            if (elementToPrependTo.hSurface != elementToPrepend.hSurface)
            {
                this->SetElementSurfaceRecursive(elementToPrepend, elementToPrependTo.hSurface);
            }
        }
        this->EndBatch();
    }

    void GUIContext::DetachElementFromParent(GUIElement &element)
    {
        if (element.parent != 0)
        {
            this->DetachElementFromParentAndSiblings(element);
        }
    }

    void GUIContext::DetachElementFromParentAndSiblings(GUIElement &element)
    {
        this->BeginBatch();
        {
            // The parent needs to be done first because we will need access to the current siblings (the siblings before detachment).

            auto parent = this->GetElementPtr(element.parent);
            if (parent != nullptr)
            {
                if (parent->firstChild == element.handle)
                {
                    assert(element.prevSibling == 0);
                    parent->firstChild = element.nextSibling;
                }

                if (parent->lastChild == element.handle)
                {
                    assert(element.nextSibling == 0);
                    parent->lastChild = element.prevSibling;
                }


                element.parent = 0;


                // The size of the parent may have changed.
                if (GUIElementStyle_Get_IsAnyWidthOfType(parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_HeightInvalid);
                }
            }


            // Before we detach from siblings, we need to check if we need to update the positions of auto-positioned elements. We only need to invalidate one
            // of these - the validation stage will ensure all auto-positioend siblings will be updated at the same time.
            if (GUIElementStyle_Get_positioning(element.style) == Positioning_Auto)
            {
                GUIElement* neigboringAutoPositionedSibling = this->Layout_GetNeighboringAutoPositionedSibling(element);
                if (neigboringAutoPositionedSibling != nullptr)
                {
                    this->Layout_InvalidateElementLayout(*neigboringAutoPositionedSibling, LayoutFlag_PositionInvalid);
                }
            }


            // The siblings can only be done after detaching from the parent.

            auto hPrevSibling = element.prevSibling;
            auto hNextSibling = element.nextSibling;

            if (hPrevSibling != 0)
            {
                auto prevSibling = this->GetElementPtr(hPrevSibling);
                assert(prevSibling != 0);
                {
                    prevSibling->nextSibling = hNextSibling;
                }
            }

            if (hNextSibling != 0)
            {
                auto nextSibling = this->GetElementPtr(hNextSibling);
                assert(nextSibling != 0);
                {
                    nextSibling->prevSibling = hPrevSibling;
                }
            }

            element.prevSibling = 0;
            element.nextSibling = 0;


            // Invalidate the applicable layouts.
            this->Layout_InvalidateElementLayout(element, LayoutFlag_SizeAndPositionInvalid);
        }
        this->EndBatch();
    }


    ////////////////////////////////////////////////////////////////
    // DPI / Scaling

    void GUIContext::UpdateAllElementsOnDPIChange()
    {
        // Every single element of every surface needs to have every layout property updated. In addition, every element needs to have their font updated for the new scaling.
        this->BeginBatch();
        m_elementHandles.IterateAssociatedObjects([&](GUIElement* element) -> bool
        {
            assert(element != nullptr);

            // Font.
            this->UpdateElementFontFromStyle(*element);

            // Borders.
            this->UpdateElementBorderSizes(*element);

            // Margins.
            this->UpdateElementMarginSizes(*element);

            // Padding.
            this->UpdateElementPaddingSizes(*element);

            // Layout.
            this->Layout_InvalidateElementLayout(*element, LayoutFlag_PositionInvalid | LayoutFlag_SizeInvalid | LayoutFlag_TextInvalid);

            return true;
        });
        this->EndBatch();
    }

    void GUIContext::UpdateElementBorderSizes(GUIElement &element)
    {
        uint32_t leftType;
        element.layout.borderLeftWidth   = static_cast<float>(GUIElementStyle_Get_borderleftwidth(element.style, leftType));

        uint32_t topType;
        element.layout.borderTopWidth    = static_cast<float>(GUIElementStyle_Get_bordertopwidth(element.style, topType));

        uint32_t rightType;
        element.layout.borderRightWidth  = static_cast<float>(GUIElementStyle_Get_borderrightwidth(element.style, rightType));

        uint32_t bottomType;
        element.layout.borderBottomWidth = static_cast<float>(GUIElementStyle_Get_borderbottomwidth(element.style, bottomType));


        if (leftType   == NumberType_Points) { element.layout.borderLeftWidth   *= this->GetXDPIScalingFactor(); }
        if (topType    == NumberType_Points) { element.layout.borderTopWidth    *= this->GetYDPIScalingFactor(); }
        if (rightType  == NumberType_Points) { element.layout.borderRightWidth  *= this->GetXDPIScalingFactor(); }
        if (bottomType == NumberType_Points) { element.layout.borderBottomWidth *= this->GetYDPIScalingFactor(); }
    }

    void GUIContext::UpdateElementMarginSizes(GUIElement &element)
    {
        uint32_t leftType;
        element.layout.marginLeft   = static_cast<float>(GUIElementStyle_Get_marginleft(element.style, leftType));

        uint32_t topType;
        element.layout.marginTop    = static_cast<float>(GUIElementStyle_Get_margintop(element.style, topType));

        uint32_t rightType;
        element.layout.marginRight  = static_cast<float>(GUIElementStyle_Get_marginright(element.style, rightType));

        uint32_t bottomType;
        element.layout.marginBottom = static_cast<float>(GUIElementStyle_Get_marginbottom(element.style, bottomType));


        if (leftType   == NumberType_Points) { element.layout.marginLeft   *= this->GetXDPIScalingFactor(); }
        if (topType    == NumberType_Points) { element.layout.marginTop    *= this->GetYDPIScalingFactor(); }
        if (rightType  == NumberType_Points) { element.layout.marginRight  *= this->GetXDPIScalingFactor(); }
        if (bottomType == NumberType_Points) { element.layout.marginBottom *= this->GetYDPIScalingFactor(); }
    }

    void GUIContext::UpdateElementPaddingSizes(GUIElement &element)
    {
        uint32_t leftType;
        element.layout.paddingLeft   = static_cast<float>(GUIElementStyle_Get_paddingleft(element.style, leftType));

        uint32_t topType;
        element.layout.paddingTop    = static_cast<float>(GUIElementStyle_Get_paddingtop(element.style, topType));

        uint32_t rightType;
        element.layout.paddingRight  = static_cast<float>(GUIElementStyle_Get_paddingright(element.style, rightType));

        uint32_t bottomType;
        element.layout.paddingBottom = static_cast<float>(GUIElementStyle_Get_paddingbottom(element.style, bottomType));


        if (leftType   == NumberType_Points) { element.layout.paddingLeft   *= this->GetXDPIScalingFactor(); }
        if (topType    == NumberType_Points) { element.layout.paddingTop    *= this->GetYDPIScalingFactor(); }
        if (rightType  == NumberType_Points) { element.layout.paddingRight  *= this->GetXDPIScalingFactor(); }
        if (bottomType == NumberType_Points) { element.layout.paddingBottom *= this->GetYDPIScalingFactor(); }
    }


    //////////////////////////////////////////////////
    // Painting

    void GUIContext::Painting_InvalidateRect(GUISurface &surface, const GTLib::Rect<int> &rect)
    {
        if (rect.left > static_cast<int>(surface.width)  || rect.right  < 0 ||
            rect.top  > static_cast<int>(surface.height) || rect.bottom < 0)
        {
            return;
        }

        surface.invalidRect.left   = GTLib::Min(surface.invalidRect.left,   rect.left);
        surface.invalidRect.top    = GTLib::Min(surface.invalidRect.top,    rect.top);
        surface.invalidRect.right  = GTLib::Max(surface.invalidRect.right,  rect.right);
        surface.invalidRect.bottom = GTLib::Max(surface.invalidRect.bottom, rect.bottom);

        if (surface.invalidRect.left   < 0)                                { surface.invalidRect.left   = 0; }
        if (surface.invalidRect.top    < 0)                                { surface.invalidRect.top    = 0; }
        if (surface.invalidRect.right  > static_cast<int>(surface.width))  { surface.invalidRect.right  = static_cast<int>(surface.width);  }
        if (surface.invalidRect.bottom > static_cast<int>(surface.height)) { surface.invalidRect.bottom = static_cast<int>(surface.height); }
    }

    void GUIContext::Painting_InvalidateRect(HGUISurface hSurface, const GTLib::Rect<int> &rect)
    {
        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            this->Painting_InvalidateRect(*surface, rect);
        }
    }

    void GUIContext::Painting_InvalidateElementRect(GUIElement &element)
    {
        auto surface = this->GetSurfacePtr(this->GetElementSurface(element.handle));
        if (surface != nullptr)
        {
            GTLib::Rect<int> rect;
            rect.left   = static_cast<int>(element.layout.absolutePosX);
            rect.top    = static_cast<int>(element.layout.absolutePosY);
            rect.right  = static_cast<int>(element.layout.absolutePosX + element.layout.width);
            rect.bottom = static_cast<int>(element.layout.absolutePosY + element.layout.height);
            this->Painting_InvalidateRect(*surface, rect);
        }
    }

    void GUIContext::Painting_PaintAndValidateSurfaceRects()
    {
        m_surfaceHandles.IterateAssociatedObjects([&](GUISurface* surface) -> bool
        {
            if (surface->paintingMode != GUIPaintingMode::Deferred)
            {
                if (surface->invalidRect.right > surface->invalidRect.left && surface->invalidRect.bottom > surface->invalidRect.top)
                {
                    this->PaintSurface(surface->handle, surface->invalidRect);
                
                    surface->invalidRect.left   = 0;
                    surface->invalidRect.top    = 0;
                    surface->invalidRect.right  = 0;
                    surface->invalidRect.bottom = 0;
                }
            }

            return true;
        });
    }

    void GUIContext::Painting_PaintElement(GUISurface &surface, GUIElement &element, const GTLib::Rect<int> &visibleRect)
    {
       // renderer.SetClippingRect(visibleRect);
        //Painting_SetClippingRect(surface, visibleRect);

        // Background.
        //GTLib::Rect<int> backgroundRect;
        //backgroundRect.left   = static_cast<int>(element.layout.absolutePosX);
        //backgroundRect.top    = static_cast<int>(element.layout.absolutePosY);
        //backgroundRect.right  = static_cast<int>(element.layout.absolutePosX + element.layout.width);
        //backgroundRect.bottom = static_cast<int>(element.layout.absolutePosY + element.layout.height);
        //renderer.DrawRectangle(visibleRect, this->GetGUIElementBackgroundColour(element.handle));

        //Painting_DrawRectangle(surface, visibleRect, this->GetGUIElementBackgroundColour(element.handle));

        Painting_DrawAndSetClippingRect(surface, visibleRect, this->GetElementBackgroundColour(element.handle));

        // Borders.
        //
        // TODO: There is overlap in the borders that will affect transparency. Borders should not overlap in the corners.

        GTLib::Rect<int> borderLeftRect;
        borderLeftRect.left   = static_cast<int>(element.layout.absolutePosX);
        borderLeftRect.top    = static_cast<int>(element.layout.absolutePosY);
        borderLeftRect.right  = static_cast<int>(element.layout.absolutePosX + element.layout.borderLeftWidth);
        borderLeftRect.bottom = static_cast<int>(element.layout.absolutePosY + element.layout.height);
        m_renderer->DrawRectangle(borderLeftRect, GUIElementStyle_Get_borderleftcolor(element.style));

        GTLib::Rect<int> borderTopRect;
        borderTopRect.left   = static_cast<int>(element.layout.absolutePosX);
        borderTopRect.top    = static_cast<int>(element.layout.absolutePosY);
        borderTopRect.right  = static_cast<int>(element.layout.absolutePosX + element.layout.width);
        borderTopRect.bottom = static_cast<int>(element.layout.absolutePosY + element.layout.borderTopWidth);
        m_renderer->DrawRectangle(borderTopRect, GUIElementStyle_Get_bordertopcolor(element.style));

        GTLib::Rect<int> borderRightRect;
        borderRightRect.left   = static_cast<int>(element.layout.absolutePosX + element.layout.width - element.layout.borderRightWidth);
        borderRightRect.top    = static_cast<int>(element.layout.absolutePosY);
        borderRightRect.right  = static_cast<int>(element.layout.absolutePosX + element.layout.width);
        borderRightRect.bottom = static_cast<int>(element.layout.absolutePosY + element.layout.height);
        m_renderer->DrawRectangle(borderRightRect, GUIElementStyle_Get_borderrightcolor(element.style));

        GTLib::Rect<int> borderBottomRect;
        borderBottomRect.left   = static_cast<int>(element.layout.absolutePosX);
        borderBottomRect.top    = static_cast<int>(element.layout.absolutePosY + element.layout.height - element.layout.borderBottomWidth);
        borderBottomRect.right  = static_cast<int>(element.layout.absolutePosX + element.layout.width);
        borderBottomRect.bottom = static_cast<int>(element.layout.absolutePosY + element.layout.height);
        m_renderer->DrawRectangle(borderBottomRect, GUIElementStyle_Get_borderbottomcolor(element.style));


        // Text.
#if 0
        if (this->DoesElementHaveText(element.handle))
        {
            // We need to iterate over each text run 

            auto font = this->GetElementFont(element.handle);
            if (font != nullptr)
            {
                // Text needs to be clipped against the inner rectangle (the same rectangle children are clipped against).
                GTLib::Rect<int> childClippingRect;
                this->GetElementChildrenClippingRect(element, childClippingRect);

                // The text clipping rectangle needs to be clamped against the input clipping rectangle.
                childClippingRect.Clamp(visibleRect);

                // TODO: Optimize this so that per-character clipping is performed instead of per-pixel.
                m_renderer->SetClippingRect(childClippingRect);
                //Painting_SetClippingRect(surface, childClippingRect);


                unsigned int tabSizeInPixels = 16;      // <-- Temp. Change this based on the size of the space character.

                font->GetServer().GetFontEngine().MeasureString(font->GetFontHandle(), this->GetElementText(element.handle), tabSizeInPixels, [&](const GTLib::FontEngine &fontEngine, char32_t character, GTLib::GlyphHandle glyph, const GTLib::Rect<int> &rect, GTLib::GlyphMetrics &, int &, int &) -> bool
                {
                    if (!GTLib::Strings::IsWhitespace(character) && glyph != 0)
                    {
                        GTLib::Rect<float> uvCoords;
                        GTLib::GlyphMapHandle glyphMap = fontEngine.GetGlyphMap(glyph, uvCoords);
                        if (glyphMap != 0)
                        {
                            // We need to map the glyph map image data so we can source it's data correctly. We know this will be in A8 format.
                            uint8_t*  srcData = reinterpret_cast<uint8_t* >(fontEngine.MapGlyphMapData(glyphMap));
                            uint32_t* dstData = reinterpret_cast<uint32_t*>(this->MapSurfaceRenderBuffer(surface.handle));

                            if (srcData != nullptr && dstData != nullptr)
                            {
                                float textPositionX = element.layout.absolutePosX + element.layout.borderLeftWidth + element.layout.paddingLeft;
                                float textPositionY = element.layout.absolutePosY + element.layout.borderTopWidth  + element.layout.paddingTop;

                                unsigned int glyphMapWidth;
                                unsigned int glyphMapHeight;
                                fontEngine.GetGlyphMapSize(glyphMap, glyphMapWidth, glyphMapHeight);

                                unsigned int srcLeft   = static_cast<unsigned int>(uvCoords.left   * glyphMapWidth);
                                unsigned int srcRight  = static_cast<unsigned int>(uvCoords.right  * glyphMapWidth);
                                unsigned int srcTop    = static_cast<unsigned int>(uvCoords.top    * glyphMapHeight);
                                unsigned int srcBottom = static_cast<unsigned int>(uvCoords.bottom * glyphMapHeight);

                                assert(srcLeft   < glyphMapWidth);
                                assert(srcRight  < glyphMapWidth);
                                assert(srcTop    < glyphMapHeight);
                                assert(srcBottom < glyphMapHeight);

                                assert(srcLeft < srcRight);
                                assert(srcTop  < srcBottom);

                                unsigned int srcWidth  = srcRight - srcLeft;
                                unsigned int srcHeight = srcBottom - srcTop;

                                unsigned int dstWidth  = surface.width;
                                unsigned int dstHeight = surface.height;

                                for (unsigned int y = 0; y < srcHeight; ++y)
                                {
                                    unsigned int srcOffset = (y + srcTop) * glyphMapWidth;
                                    unsigned int dstOffset = (y + static_cast<int>(roundf(textPositionY)) + rect.top) * dstWidth;

                                    // TODO: Make sure we don't draw past the boundary of the buffer.

                                    for (unsigned int x = 0; x < srcWidth; ++x)
                                    {
                                        unsigned int iSrc = srcOffset + x + srcLeft;
                                        unsigned int iDst = dstOffset + x + static_cast<int>(roundf(textPositionX)) + rect.left;

                                        if ((dstData[iDst] & 0x01))    // <-- Check clipping.
                                        {
                                            uint32_t src = (srcData[iSrc] | 0xFFFFFF00);
                                            uint32_t dst = (dstData[iDst] & 0xFFFFFF00) | ((dstData[iDst] & 0x000000FE) >> 1);

                                            uint32_t srcR = (src & 0xFF000000) >> 24;
                                            uint32_t srcG = (src & 0x00FF0000) >> 16;
                                            uint32_t srcB = (src & 0x0000FF00) >> 8;
                                            uint32_t srcA = (src & 0x000000FF) >> 0;

                                            uint32_t dstR = (dst & 0xFF000000) >> 24;
                                            uint32_t dstG = (dst & 0x00FF0000) >> 16;
                                            uint32_t dstB = (dst & 0x0000FF00) >> 8;
                                            uint32_t dstA = (dst & 0x000000FF) >> 0;

                                            float alpha = srcA / 255.0f;
                                            uint32_t resR = static_cast<uint32_t>(((dstR * (1.0f-alpha)) + (srcR * alpha))) << 24;
                                            uint32_t resG = static_cast<uint32_t>(((dstG * (1.0f-alpha)) + (srcG * alpha))) << 16;
                                            uint32_t resB = static_cast<uint32_t>(((dstB * (1.0f-alpha)) + (srcB * alpha))) << 8;
                                            uint32_t resA = static_cast<uint32_t>(((dstA * (1.0f-alpha)) + (srcA * alpha))) << 0;

                                            dstData[iDst] = resR | resG | resB | ((resA >> 1) << 1) | (dstData[iDst] & 0x01);
                                        }
                                    }
                                }
                            }

                            this->UnmapSurfaceRenderBuffer(surface.handle, dstData);
                        }
                    }

                    return true;
                });
            }
        }
#endif
    }

    void GUIContext::Painting_DrawAndSetClippingRect(GUISurface &surface, const GTLib::Rect<int> &rect, const GTLib::Colour &color)
    {
        (void)surface;

        assert(m_renderer != nullptr);

#if 1
        m_renderer->SetClippingRect(rect);
        m_renderer->DrawRectangle(rect, color);

        //this->Painting_SetClippingRect(surface, rect);
        //this->Painting_DrawRectangle(surface, rect, color);
#else
        auto pixelBuffer = reinterpret_cast<uint32_t*>(this->MapSurfacePixelBuffer(surface.handle));
        if (pixelBuffer != nullptr)
        {
            // If the entire rectangle is completely outside the surface, just ignore it.
            if (rect.left   > static_cast<int>(surface.m_width))  return;
            if (rect.top    > static_cast<int>(surface.m_height)) return;
            if (rect.right  < 0) return;
            if (rect.bottom < 0) return;

            // The rectangle needs to be clamped within the surface's area.
            GTLib::Rect<unsigned int> clampedRect;
            clampedRect.left   = (rect.left   < 0)                                  ? 0                : rect.left;
            clampedRect.top    = (rect.top    < 0)                                  ? 0                : rect.top;
            clampedRect.right  = (rect.right  > static_cast<int>(surface.m_width))  ? surface.m_width  : rect.right;
            clampedRect.bottom = (rect.bottom > static_cast<int>(surface.m_height)) ? surface.m_height : rect.bottom;


            // Clear the areas outside the clipping rectangle to 0.
            for (unsigned int y = m_paintingContext.clippingRect.top; y < m_paintingContext.clippingRect.bottom; ++y)
            {
#if 0
                unsigned int offset = y * surface.m_width;
                for (unsigned int x = m_paintingContext.clippingRect.left; x < m_paintingContext.clippingRect.right; ++x)
                {
                    pixelBuffer[offset + x] = pixelBuffer[offset + x] & 0xFFFFFFFE;
                }
#else
                uint32_t* row32 = pixelBuffer + (y * surface.m_width) + m_paintingContext.clippingRect.left;
                uint64_t* row64 = reinterpret_cast<uint64_t*>(row32);

                const unsigned int rectWidth = m_paintingContext.clippingRect.GetWidth();
                const unsigned int rectWidthHalf = rectWidth >> 1;
                for (unsigned int x = 0; x < rectWidthHalf; ++x)
                {
                    row64[x] = row64[x] & 0xFFFFFFFEFFFFFFFE;
                }

                if (rectWidthHalf % 2)
                {
                    row32[rectWidth - 1] = row32[rectWidth - 1] & 0xFFFFFFFE;
                }
#endif
            }


            uint32_t rgb8 =
                (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
                (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
                (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8);

            for (unsigned int y = clampedRect.top; y < clampedRect.bottom; ++y)
            {
                unsigned int offset = y * surface.m_width;
                for (unsigned int x = clampedRect.left; x < clampedRect.right; ++x)
                {
                    pixelBuffer[offset + x] = rgb8 | 0x01;
                }
            }


            // Set the clipping rectangle on the context so we can know the region that is clipped.
            m_paintingContext.clippingRect = clampedRect;

            this->UnmapSurfacePixelBuffer(surface.handle);
        }
#endif
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
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            // The list of event handlers that have been processed. Do not dereference any of these pointers as they may become invalid during iteration.
            GTLib::Vector<GUIEventHandler*> processedEventHandlers;


            size_t iEventHandler = 0;
            while (iEventHandler < element->eventHandlers.GetCount())
            {
                auto eventHandler = element->eventHandlers[iEventHandler];
                assert(eventHandler != nullptr);
                {
                    if (func(*eventHandler))
                    {
                        // If the handle is no longer valid, break.
                        if (this->GetElementPtr(hElement) != element)
                        {
                            return;
                        }


                        // Let the algorithm know that this event handler has been processed.
                        processedEventHandlers.PushBack(eventHandler);


                        // If an event handler has been removed, we need to roll the index back to that of the event handler that was last processed.
                        if (eventHandler == element->eventHandlers[iEventHandler])
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

                                if (processedEventHandlers.ExistsReverse(element->eventHandlers[iEventHandler]))
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
            auto eventHandler = m_globalEventHandlers[iEventHandler];
            assert(eventHandler != nullptr);
            {
                if (func(*eventHandler))
                {
                    // Let the algorithm know that this event handler has been processed.
                    processedEventHandlers.PushBack(eventHandler);


                    // If an event handler has been removed, we need to roll the index back to that of the event handler that was last processed.
                    if (eventHandler == m_globalEventHandlers[iEventHandler])
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

    void GUIContext::PostEvent_OnSize(HGUIElement hElement, unsigned int width, unsigned int height)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnSize(*this, hElement, width, height);
                return true;
            });

            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (element == this->GetElementPtr(hElement))
                {
                    eventHandler.OnSize(*this, hElement, width, height);
                    return true;
                }
                
                return false;
            });
        }
    }

    void GUIContext::PostEvent_OnMove(HGUIElement hElement, int x, int y)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMove(*this, hElement, x, y);
                return true;
            });

            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (element == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMove(*this, hElement, x, y);
                    return true;
                }
                
                return false;
            });
        }
    }

    void GUIContext::PostEvent_OnMouseEnter(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseEnter(*this, hElement);
                return true;
            });

            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (element == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseEnter(*this, hElement);
                    return true;
                }
                
                return false;
            });
        }
    }

    void GUIContext::PostEvent_OnMouseLeave(HGUIElement hElement)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseLeave(*this, hElement);
                return true;
            });

            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (element == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseLeave(*this, hElement);
                    return true;
                }
                
                return false;
            });
        }
    }

    void GUIContext::PostEvent_OnMouseMove(HGUIElement hElement, int mousePosX, int mousePosY)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            // Local
            this->IterateLocalEventHandlers(hElement, [&](GUIEventHandler &eventHandler) -> bool
            {
                eventHandler.OnMouseMove(*this, hElement, mousePosX, mousePosY);
                return true;
            });

            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (element == this->GetElementPtr(hElement))
                {
                    eventHandler.OnMouseMove(*this, hElement, mousePosX, mousePosY);
                    return true;
                }
                
                return false;
            });
        }
    }

    void GUIContext::PostEvent_OnPaint(HGUISurface hSurface, const GTLib::Rect<int> &rect)
    {
        auto surface = this->GetSurfacePtr(hSurface);
        if (surface != nullptr)
        {
            // Global
            this->IterateGlobalEventHandlers([&](GUIEventHandler &eventHandler) -> bool
            {
                if (surface == this->GetSurfacePtr(hSurface))
                {
                    eventHandler.OnPaint(*this, hSurface, rect);
                    return true;
                }

                return false;
            });
        }
    }



    //////////////////////////////////////////////////
    // Layout

    void GUIContext::Layout_SetElementSize(HGUIElement hElement, uint32_t width, uint32_t widthType, uint32_t height, uint32_t heightType)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_width( element->style, width,  widthType);
            GUIElementStyle_Set_height(element->style, height, heightType);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_SizeInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::Layout_SetElementWidth(HGUIElement hElement, uint32_t width, uint32_t widthType)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_width(element->style, width,  widthType);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_WidthInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::Layout_SetElementMinWidth(HGUIElement hElement, uint32_t minWidth, uint32_t widthType)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_minwidth(element->style, minWidth, widthType);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_WidthInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::Layout_SetElementMaxWidth(HGUIElement hElement, uint32_t maxWidth, uint32_t widthType)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_maxwidth(element->style, maxWidth, widthType);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_WidthInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::Layout_SetElementHeight(HGUIElement hElement, uint32_t height, uint32_t heightType)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_height(element->style, static_cast<uint32_t>(height), heightType);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_HeightInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::Layout_SetElementMinHeight(HGUIElement hElement, uint32_t minHeight, uint32_t heightType)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_minheight(element->style, minHeight, heightType);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_HeightInvalid);
            }
            this->EndBatch();
        }
    }

    void GUIContext::Layout_SetElementMaxHeight(HGUIElement hElement, uint32_t maxHeight, uint32_t heightType)
    {
        auto element = this->GetElementPtr(hElement);
        if (element != nullptr)
        {
            GUIElementStyle_Set_maxheight(element->style, maxHeight, heightType);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(*element, LayoutFlag_HeightInvalid);
            }
            this->EndBatch();
        }
    }



    void GUIContext::Layout_InvalidateElementLayout(GUIElement &element, unsigned int flags)
    {
        if (flags != 0)
        {
            if (element.layout.flags == 0)
            {
                assert(element.layout.layoutValidationListItem == nullptr);
                element.layout.layoutValidationListItem = m_layoutContext.invalidElements.Append(&element);

                // The element is amount to have it's layout changed which means we will want to invalidate the previous rectangle. The new
                // rectangle will be invalidated after validating the layout.
                this->Painting_InvalidateElementRect(element);
            }
        
            element.layout.flags |= flags;
        }
    }


    void GUIContext::Layout_InvalidateElementLayoutsOnInnerWidthChange(GUIElement &element)
    {
        int invalidLayoutFlags = 0;

        uint32_t widthType;
        uint32_t minWidthType;
        uint32_t maxWidthType;
        GUIElementStyle_Get_allwidthtypes(element.style, widthType, minWidthType, maxWidthType);
        if (widthType == NumberType_Auto || minWidthType == NumberType_Auto || maxWidthType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }

        if (invalidLayoutFlags != 0)
        {
            this->Layout_InvalidateElementLayout(element, invalidLayoutFlags);
        }



        // The positions and size of children may have changed.
        int childrenInvalidLayoutFlags = LayoutFlag_PositionInvalid;
        if (GUIElementStyle_Get_childrenwidthboundary(element.style) != ChildrenSizeBoundary_Outer)
        {
            childrenInvalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }

        for (HGUIElement hChild = element.firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
        {
            auto child = this->GetElementPtr(hChild);
            assert(child != nullptr);
            {
                this->Layout_InvalidateElementLayout(*child, childrenInvalidLayoutFlags);
            }
        }
    }

    void GUIContext::Layout_InvalidateElementLayoutsOnInnerHeightChange(GUIElement &element)
    {
        int invalidLayoutFlags = 0;

        uint32_t heightType;
        uint32_t minHeightType;
        uint32_t maxHeightType;
        GUIElementStyle_Get_allheighttypes(element.style, heightType, minHeightType, maxHeightType);
        if (heightType == NumberType_Auto || minHeightType == NumberType_Auto || maxHeightType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        if (invalidLayoutFlags != 0)
        {
            this->Layout_InvalidateElementLayout(element, invalidLayoutFlags);
        }



        // The positions of children may have changed.
        int childrenInvalidLayoutFlags = LayoutFlag_PositionInvalid;
        if (GUIElementStyle_Get_childrenheightboundary(element.style) != ChildrenSizeBoundary_Outer)
        {
            childrenInvalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        for (HGUIElement hChild = element.firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
        {
            auto child = this->GetElementPtr(hChild);
            assert(child != nullptr);
            {
                this->Layout_InvalidateElementLayout(*child, childrenInvalidLayoutFlags);
            }
        }
    }

    void GUIContext::Layout_InvalidateElementLayoutsOnInnerSizeChange(GUIElement &element)
    {
        int invalidLayoutFlags = 0;

        uint32_t widthType;
        uint32_t minWidthType;
        uint32_t maxWidthType;
        GUIElementStyle_Get_allwidthtypes(element.style, widthType, minWidthType, maxWidthType);
        if (widthType == NumberType_Auto || minWidthType == NumberType_Auto || maxWidthType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }

        uint32_t heightType;
        uint32_t minHeightType;
        uint32_t maxHeightType;
        GUIElementStyle_Get_allheighttypes(element.style, heightType, minHeightType, maxHeightType);
        if (heightType == NumberType_Auto || minHeightType == NumberType_Auto || maxHeightType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        if (invalidLayoutFlags != 0)
        {
            this->Layout_InvalidateElementLayout(element, invalidLayoutFlags);
        }



        // The positions of children may have changed.
        int childrenInvalidLayoutFlags = LayoutFlag_PositionInvalid;
        if (GUIElementStyle_Get_childrenwidthboundary(element.style) != ChildrenSizeBoundary_Outer)
        {
            childrenInvalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }
        if (GUIElementStyle_Get_childrenheightboundary(element.style) != ChildrenSizeBoundary_Outer)
        {
            childrenInvalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        for (HGUIElement hChild = element.firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
        {
            auto child = this->GetElementPtr(hChild);
            assert(child != nullptr);
            {
                this->Layout_InvalidateElementLayout(*child, childrenInvalidLayoutFlags);
            }
        }
    }


    void GUIContext::Layout_InvalidateElementLayoutsOnChildAxisChange(GUIElement &element)
    {
        int invalidLayoutFlags = 0;

        uint32_t widthType;
        uint32_t minWidthType;
        uint32_t maxWidthType;
        GUIElementStyle_Get_allwidthtypes(element.style, widthType, minWidthType, maxWidthType);
        if (widthType == NumberType_Auto || minWidthType == NumberType_Auto || maxWidthType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }

        uint32_t heightType;
        uint32_t minHeightType;
        uint32_t maxHeightType;
        GUIElementStyle_Get_allheighttypes(element.style, heightType, minHeightType, maxHeightType);
        if (heightType == NumberType_Auto || minHeightType == NumberType_Auto || maxHeightType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        


        // The positions of children may have changed.
        int childrenInvalidLayoutFlags = LayoutFlag_PositionInvalid;
        for (HGUIElement hChild = element.firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
        {
            auto child = this->GetElementPtr(hChild);
            assert(child != nullptr);
            {
                this->Layout_InvalidateElementLayout(*child, childrenInvalidLayoutFlags);
            }
        }


        if (invalidLayoutFlags != 0)
        {
            this->Layout_InvalidateElementLayout(element, invalidLayoutFlags);
        }
    }


    void GUIContext::Layout_InvalidateElementLayoutsOnHorizontalAlignChange(GUIElement &element)
    {
        // Auto-positioned elements need to be repositioned. We can do this by just invalidating the first auto-positioned element.
        auto firstAutoPositionedChild = this->Layout_GetFirstAutoPositionedChild(element);
        if (firstAutoPositionedChild != nullptr)
        {
            this->Layout_InvalidateElementLayout(*firstAutoPositionedChild, LayoutFlag_PositionInvalid);
        }
    }

    void GUIContext::Layout_InvalidateElementLayoutsOnVerticalAlignChange(GUIElement &element)
    {
        // Auto-positioned elements need to be repositioned. We can do this by just invalidating the first auto-positioned element.
        auto firstAutoPositionedChild = this->Layout_GetFirstAutoPositionedChild(element);
        if (firstAutoPositionedChild != nullptr)
        {
            this->Layout_InvalidateElementLayout(*firstAutoPositionedChild, LayoutFlag_PositionInvalid);
        }
    }



    void GUIContext::Layout_ValidateElementLayouts()
    {
        if (m_batchLockCounter == 0)
        {
#if defined(GT_GUI_DEBUGGING)
            m_layoutLogger.Print("\n\nValidateGUIElementLayouts() begin\n");
#endif

            while (m_layoutContext.invalidElements.root != nullptr)
            {
                auto iElement = m_layoutContext.invalidElements.root;
                assert(iElement != nullptr);
                {
                    auto element = iElement->value;
                    assert(element != nullptr);
                    {
                        this->Layout_ValidateElementLayout(*element);
                    }
                }
            }


            ///////////////////////////////////////////////////
            // Post-process stage.

            // Update absolute layouts.
            this->Layout_UpdateElementAbsolutePositions();

            // Post events.
            this->Layout_InvalidateRectsAndPostEventsToElements();

#if defined(GT_GUI_DEBUGGING)
            m_layoutLogger.Print("ValidateGUIElementLayouts() end\n");
#endif
        }
    }

    void GUIContext::Layout_ValidateElementLayout(GUIElement &element)
    {
#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.BeginLine();
        m_layoutLogger.Print(GTLib::String::CreateFormatted("Validating Element: %d\n", element.handle).c_str());
#endif


        if ((element.layout.flags & LayoutFlag_WidthInvalid) != 0)
        {
            this->Layout_ValidateElementWidth(element);
        }

        if ((element.layout.flags & LayoutFlag_HeightInvalid) != 0)
        {
            this->Layout_ValidateElementHeight(element);
        }

        if ((element.layout.flags & (LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid)) != 0)
        {
            this->Layout_ValidateElementPosition(element);
        }

        if ((element.layout.flags & LayoutFlag_TextInvalid) != 0)
        {
            this->Layout_ValidateElementText(element);
        }



        // The list item in the layout context needs to be removed.
        if (element.layout.flags == 0 && element.layout.layoutValidationListItem != nullptr)
        {
            m_layoutContext.invalidElements.Remove(element.layout.layoutValidationListItem);
            element.layout.layoutValidationListItem = nullptr;
        }


#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.EndLine();
#endif
    }

    void GUIContext::Layout_ValidateElementWidth(GUIElement &element)
    {
#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.BeginLine();
        m_layoutLogger.Print(GTLib::String::CreateFormatted("width: %d\n", element.handle).c_str());
#endif
        float oldOuterWidth = GUIContext::Layout_GetElementOuterWidth(element);
        float newOuterWidth = this->Layout_UpdateElementWidth(element);

        if (oldOuterWidth != newOuterWidth)
        {
            this->Layout_MarkElementSizeAsChanged(element);


            //////////////////////////////////////////////////////////////////////////
            // Dependencies
            auto parent = this->GetElementPtr(element.parent);
            
            uint32_t parentWidthType = 0;
            if (parent != nullptr)
            {
                GUIElementStyle_Get_width(parent->style, parentWidthType);
            }


            // Widths
            //
            // Any %-sized children will have changed.
            for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
            {
                auto childElement = this->GetElementPtr(hChildElement);
                assert(childElement != nullptr);
                {
                    uint32_t childWidthType;
                    uint32_t childMinWidthType;
                    uint32_t childMaxWidthType;
                    GUIElementStyle_Get_allwidthtypes(childElement->style, childWidthType, childMinWidthType, childMaxWidthType);

                    if (childWidthType == NumberType_Percent || childMinWidthType == NumberType_Percent || childMaxWidthType == NumberType_Percent)
                    {
                        this->Layout_InvalidateElementLayout(*childElement, LayoutFlag_WidthInvalid);
                    }
                }
            }

            // Flexed siblings will need to have their widths updated.
            if (parent != nullptr && GUIElementStyle_Get_flexchildrenwidth(parent->style) && GUIElementStyle_Get_childaxis(parent->style) == ChildAxis_Horizontal/* && !element.style.width->InPercent()*/)       // <-- TODO: Check why we originally had this InPercent() condition and if it's still needed. The issue is that size validation will be needed when the margin/padding changes.
            {
                for (HGUIElement hSiblingElement = parent->firstChild; hSiblingElement != 0; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                {
                    auto siblingElement = this->GetElementPtr(hSiblingElement);
                    assert(siblingElement != nullptr);
                    {
                        if (!GUIContext::Layout_IsElementWidthInvalid(*siblingElement))
                        {
                            this->Layout_InvalidateElementLayout(*siblingElement, LayoutFlag_WidthInvalid);
                        }
                    }
                }
            }

            // If the parent is auto-sized, it will also have it's width changed.
            if (parent != nullptr && parentWidthType == NumberType_Auto)
            {
                uint32_t widthType;
                GUIElementStyle_Get_width(element.style, widthType);

                if (widthType != NumberType_Percent)
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_WidthInvalid);
                }
            }



            // Positions
            //
            // The position of this element will have changed if we're not aligned left.
            if ((parent != nullptr && GUIElementStyle_Get_horizontalalign(parent->style) != HorizontalAlign_Left) || (GUIElementStyle_Get_positioning(element.style) == Positioning_Relative && GUIElementStyle_Get_righthaspriority(element.style)))
            {
                this->Layout_InvalidateElementLayout(element, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
            }

            // Children might have positions changed depending on alignment.
            if (GUIElementStyle_Get_horizontalalign(element.style) != HorizontalAlign_Left)
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        this->Layout_InvalidateElementLayout(*childElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                    }
                }
            }
            else
            {
                // TODO: What is the point in this branch? Only repositioning relative and absolute children and only if the alignment is left? Why? What scenario requires this?

                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_positioning(childElement->style) != Positioning_Auto)
                        {
                            this->Layout_InvalidateElementLayout(*childElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                        }
                    }
                }
            }

            // Siblings might have positions changed depending on the parent's alignment.
            if (parent != nullptr)
            {
                switch (GUIElementStyle_Get_horizontalalign(parent->style))
                {
                case HorizontalAlign_Right:
                    {
                        // All auto-positioned siblings before this element.
                        for (HGUIElement hSiblingElement = parent->firstChild; hSiblingElement != element.handle; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                        {
                            auto siblingElement = this->GetElementPtr(hSiblingElement);
                            assert(siblingElement != nullptr);
                            {
                                if (GUIElementStyle_Get_positioning(siblingElement->style) == Positioning_Auto)
                                {
                                    this->Layout_InvalidateElementLayout(*siblingElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                                }
                            }
                        }

                        break;
                    }

                case HorizontalAlign_Center:
                    {
                        // All auto-positioned siblings, including this element.
                        for (HGUIElement hSiblingElement = parent->firstChild; hSiblingElement != 0; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                        {
                            auto siblingElement = this->GetElementPtr(hSiblingElement);
                            assert(siblingElement != nullptr);
                            {
                                if (GUIElementStyle_Get_positioning(siblingElement->style) == Positioning_Auto)
                                {
                                    this->Layout_InvalidateElementLayout(*siblingElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                                }
                            }
                        }

                        break;
                    }

                case HorizontalAlign_Left:
                default:
                    {
                        // All auto-positioned siblings after this element.
                        for (HGUIElement hSiblingElement = element.handle; hSiblingElement != 0; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                        {
                            auto siblingElement = this->GetElementPtr(hSiblingElement);
                            assert(siblingElement != nullptr);
                            {
                                if (GUIElementStyle_Get_positioning(siblingElement->style) == Positioning_Auto)
                                {
                                    this->Layout_InvalidateElementLayout(*siblingElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                                }
                            }
                        }

                        break;
                    }
                }
            }
        }


        // Now we need to remove ourselves from the invalid list.
        this->Layout_MarkElementWidthAsValid(element);

#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.EndLine();
#endif
    }

    void GUIContext::Layout_ValidateElementHeight(GUIElement &element)
    {
#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.BeginLine();
        m_layoutLogger.Print(GTLib::String::CreateFormatted("height: %d\n", element.handle).c_str());
#endif
        float oldOuterHeight = GUIContext::Layout_GetElementOuterHeight(element);
        float newOuterHeight = this->Layout_UpdateElementHeight(element);

        if (oldOuterHeight != newOuterHeight)
        {
            this->Layout_MarkElementSizeAsChanged(element);


            //////////////////////////////////////////////////////////////////////////
            // Dependencies
            auto parent = this->GetElementPtr(element.parent);

            uint32_t parentHeightType = 0;
            if (parent != nullptr)
            {
                GUIElementStyle_Get_height(parent->style, parentHeightType);
            }


            // Widths
            //
            // Any %-sized children will have changed.
            for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
            {
                auto childElement = this->GetElementPtr(hChildElement);
                assert(childElement != nullptr);
                {
                    uint32_t childHeightType;
                    uint32_t childMinHeightType;
                    uint32_t childMaxHeightType;
                    GUIElementStyle_Get_allheighttypes(childElement->style, childHeightType, childMinHeightType, childMaxHeightType);

                    if (childHeightType == NumberType_Percent || childMinHeightType == NumberType_Percent || childMaxHeightType == NumberType_Percent)
                    {
                        this->Layout_InvalidateElementLayout(*childElement, LayoutFlag_HeightInvalid);
                    }
                }
            }

            // Flexed siblings will need to have their heights updated.
            if (parent != nullptr && GUIElementStyle_Get_flexchildrenheight(parent->style) && GUIElementStyle_Get_childaxis(parent->style) == ChildAxis_Vertical/* && !element.style.height->InPercent()*/)       // <-- TODO: Check why we originally had this InPercent() condition and if it's still needed. The issue is that size validation will be needed when the margin/padding changes.
            {
                for (HGUIElement hSiblingElement = parent->firstChild; hSiblingElement != 0; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                {
                    auto siblingElement = this->GetElementPtr(hSiblingElement);
                    assert(siblingElement != nullptr);
                    {
                        if (!GUIContext::Layout_IsElementHeightInvalid(*siblingElement))
                        {
                            this->Layout_InvalidateElementLayout(*siblingElement, LayoutFlag_HeightInvalid);
                        }
                    }
                }
            }

            // If the parent is auto-sized, it will also have it's height changed.
            if (parent != nullptr && parentHeightType == NumberType_Auto)
            {
                uint32_t heightType;
                GUIElementStyle_Get_height(element.style, heightType);

                if (heightType != NumberType_Percent)
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_HeightInvalid);
                }
            }



            // Positions
            //
            // The position of this element will have changed if we're not aligned top.
            if ((parent != nullptr && GUIElementStyle_Get_verticalalign(parent->style) != VerticalAlign_Top) || (GUIElementStyle_Get_positioning(element.style) == Positioning_Relative && GUIElementStyle_Get_bottomhaspriority(element.style)))
            {
                this->Layout_InvalidateElementLayout(element, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
            }

            // Children might have positions changed depending on alignment.
            if (GUIElementStyle_Get_verticalalign(element.style) != VerticalAlign_Top)
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        this->Layout_InvalidateElementLayout(*childElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                    }
                }
            }
            else
            {
                // TODO: What is the point in this branch? Only repositioning relative and absolute children and only if the alignment is top? Why? What scenario requires this?

                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_positioning(childElement->style) != Positioning_Auto)
                        {
                            this->Layout_InvalidateElementLayout(*childElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                        }
                    }
                }
            }

            // Siblings might have positions changed depending on the parent's alignment.
            if (parent != nullptr)
            {
                switch (GUIElementStyle_Get_verticalalign(parent->style))
                {
                case VerticalAlign_Bottom:
                    {
                        // All auto-positioned siblings before this element.
                        for (HGUIElement hSiblingElement = parent->firstChild; hSiblingElement != element.handle; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                        {
                            auto siblingElement = this->GetElementPtr(hSiblingElement);
                            assert(siblingElement != nullptr);
                            {
                                if (GUIElementStyle_Get_positioning(siblingElement->style) == Positioning_Auto)
                                {
                                    this->Layout_InvalidateElementLayout(*siblingElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                                }
                            }
                        }

                        break;
                    }

                case VerticalAlign_Center:
                    {
                        // All auto-positioned siblings, including this element.
                        for (HGUIElement hSiblingElement = parent->firstChild; hSiblingElement != 0; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                        {
                            auto siblingElement = this->GetElementPtr(hSiblingElement);
                            assert(siblingElement != nullptr);
                            {
                                if (GUIElementStyle_Get_positioning(siblingElement->style) == Positioning_Auto)
                                {
                                    this->Layout_InvalidateElementLayout(*siblingElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                                }
                            }
                        }

                        break;
                    }

                case VerticalAlign_Top:
                default:
                    {
                        // All auto-positioned siblings after this element.
                        for (HGUIElement hSiblingElement = element.handle; hSiblingElement != 0; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                        {
                            auto siblingElement = this->GetElementPtr(hSiblingElement);
                            assert(siblingElement != nullptr);
                            {
                                if (GUIElementStyle_Get_positioning(siblingElement->style) == Positioning_Auto)
                                {
                                    this->Layout_InvalidateElementLayout(*siblingElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                                }
                            }
                        }

                        break;
                    }
                }
            }
        }


        // Now we need to remove ourselves from the invalid list.
        this->Layout_MarkElementHeightAsValid(element);


#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.EndLine();
#endif
    }

    void GUIContext::Layout_ValidateElementPosition(GUIElement &element)
    {
#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.BeginLine();
        m_layoutLogger.Print(GTLib::String::CreateFormatted("position: %d\n", element.handle).c_str());
#endif

        // We validate the position differently depending on it's positioning type (auto, relative or absolute).
        auto oldRelativeX = element.layout.relativePosX;
        auto oldRelativeY = element.layout.relativePosY;

        switch (GUIElementStyle_Get_positioning(element.style))
        {
        case Positioning_Absolute:
            {
                this->Layout_ValidateElementPosition_Absolute(element);
                break;
            }

        case Positioning_Relative:
            {
                this->Layout_ValidateElementPosition_Relative(element);
                break;
            }

        case Positioning_Auto:
        default:
            {
                this->Layout_ValidateElementPosition_Auto(element);
                break;
            }
        }


        if (oldRelativeX != element.layout.relativePosX || oldRelativeY != element.layout.relativePosY)
        {
            this->Layout_MarkElementPositionAsChanged(element);
        }



        // Now we need to remove ourselves from the invalid list.
        this->Layout_MarkElementPositionAsValid(element);

#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.EndLine();
#endif
    }

    void GUIContext::Layout_ValidateElementPosition_Absolute(GUIElement &element)
    {
        assert(GUIElementStyle_Get_positioning(element.style) == Positioning_Absolute);
        {
            GUISurface* surface = nullptr;


            // X
            if (GUIElementStyle_Get_righthaspriority(element.style))
            {
                // Right

                uint32_t rightType;
                int32_t  right = GUIElementStyle_Get_right(element.style, rightType);

                auto parent = this->GetElementPtr(element.parent);
                if (parent != nullptr)
                {
                    element.layout.relativePosX = parent->layout.width - element.layout.width;
                }
                else
                {
                    if (surface == nullptr) { surface = this->GetSurfacePtr(this->GetElementSurface(element.handle)); }
                    if (surface != nullptr)
                    {
                        element.layout.relativePosX = surface->width - element.layout.width;
                    }
                    else
                    {
                        element.layout.relativePosX = 0.0f;
                    }
                }


                if (rightType == NumberType_Percent)
                {
                    if (surface == nullptr) { surface = this->GetSurfacePtr(this->GetElementSurface(element.handle)); }
                    if (surface != nullptr)
                    {
                        element.layout.relativePosX += surface->width * (right / 100.0f);
                    }
                }
                else
                {
                    if (rightType == NumberType_Points)
                    {
                        element.layout.relativePosX += static_cast<float>(right) * this->GetXDPIScalingFactor();
                    }
                    else
                    {
                        element.layout.relativePosX += static_cast<float>(right);
                    }
                }
            }
            else
            {
                // Left

                uint32_t leftType;
                int32_t  left = GUIElementStyle_Get_left(element.style, leftType);

                if (leftType == NumberType_Percent)
                {
                    if (surface == nullptr) { surface = this->GetSurfacePtr(this->GetElementSurface(element.handle)); }
                    if (surface != nullptr)
                    {
                        element.layout.relativePosX = surface->width * (left / 100.0f);
                    }
                }
                else
                {
                    if (leftType == NumberType_Points)
                    {
                        element.layout.relativePosX = static_cast<float>(left) * this->GetXDPIScalingFactor();
                    }
                    else
                    {
                        element.layout.relativePosX = static_cast<float>(left);
                    }
                }
            }



            // Y
            if (GUIElementStyle_Get_bottomhaspriority(element.style))
            {
                // Bottom

                uint32_t bottomType;
                int32_t  bottom = GUIElementStyle_Get_bottom(element.style, bottomType);

                auto parent = this->GetElementPtr(element.parent);
                if (parent != nullptr)
                {
                    element.layout.relativePosY = parent->layout.height - element.layout.height;
                }
                else
                {
                    if (surface == nullptr) { surface = this->GetSurfacePtr(this->GetElementSurface(element.handle)); }
                    if (surface != nullptr)
                    {
                        element.layout.relativePosY = surface->height - element.layout.height;
                    }
                    else
                    {
                        element.layout.relativePosY = 0.0f;
                    }
                }


                if (bottomType == NumberType_Percent)
                {
                    if (surface == nullptr) { surface = this->GetSurfacePtr(this->GetElementSurface(element.handle)); }
                    if (surface != nullptr)
                    {
                        element.layout.relativePosY += surface->width * (bottom / 100.0f);
                    }
                }
                else
                {
                    if (bottomType == NumberType_Points)
                    {
                        element.layout.relativePosY += static_cast<float>(bottom) * this->GetYDPIScalingFactor();
                    }
                    else
                    {
                        element.layout.relativePosY += static_cast<float>(bottom);
                    }
                }
            }
            else
            {
                // Top

                uint32_t topType;
                int32_t  top = GUIElementStyle_Get_left(element.style, topType);

                if (topType == NumberType_Percent)
                {
                    if (surface == nullptr) { surface = this->GetSurfacePtr(this->GetElementSurface(element.handle)); }
                    if (surface != nullptr)
                    {
                        element.layout.relativePosY = surface->height * (top / 100.0f);
                    }
                }
                else
                {
                    if (topType == NumberType_Points)
                    {
                        element.layout.relativePosY = static_cast<float>(top) * this->GetYDPIScalingFactor();
                    }
                    else
                    {
                        element.layout.relativePosY = static_cast<float>(top);
                    }
                }
            }
        }
    }
    
    void GUIContext::Layout_ValidateElementPosition_Relative(GUIElement &element)
    {
        assert(GUIElementStyle_Get_positioning(element.style) == Positioning_Relative);
        {
            // The position is based on the parent without any consideration for siblings. If the element does not have a parent, we will position it against the surface.
            //
            // The position can be calculated in two stages. The first is to calculate the orign. The second is to calculate an offset. They are then added together to
            // form the final position.

            bool leftHasPriority = GUIElementStyle_Get_lefthaspriority(element.style);
            bool topHasPriority  = GUIElementStyle_Get_tophaspriority(element.style);


            uint32_t positionTypeX = 0;
            uint32_t positionTypeY = 0;
            float positionX = 0.0f;
            float positionY = 0.0f;

            float containerPaddingX     = 0.0f;
            float containerPaddingY     = 0.0f;
            float containerBorderWidthX = 0.0f;
            float containerBorderWidthY = 0.0f;

            float originX = 0.0f;
            float originY = 0.0f;
            

            auto parent = this->GetElementPtr(element.parent);
            if (parent != nullptr)
            {
                if (leftHasPriority)
                {
                    containerPaddingX     = parent->layout.paddingLeft;
                    containerBorderWidthX = parent->layout.borderLeftWidth;
                }
                else
                {
                    containerPaddingX     = -parent->layout.paddingRight;
                    containerBorderWidthX = -parent->layout.borderRightWidth;

                    originX = parent->layout.width;
                }


                if (topHasPriority)
                {
                    containerPaddingY     = parent->layout.paddingTop;
                    containerBorderWidthY = parent->layout.borderTopWidth;
                }
                else
                {
                    containerPaddingY     = -parent->layout.paddingBottom;
                    containerBorderWidthY = -parent->layout.borderBottomWidth;

                    originY = parent->layout.height;
                }
            }
            else
            {
                // Position based on the size of the surface.
                auto surface = this->GetSurfacePtr(this->GetElementSurface(element.handle));
                if (surface != nullptr)
                {
                    if (!leftHasPriority)
                    {
                        // Right has priority.
                        originX = static_cast<float>(surface->width);
                    }

                    if (!topHasPriority)
                    {
                        // Bottom has priority.
                        originY = static_cast<float>(surface->height);
                    }
                }
            }


            // The origin needs to be moved based on the position-origin style attribute.
            switch (GUIElementStyle_Get_positionorigin(element.style))
            {
            case PositionOrigin_Inner:
                {
                    // Pull inside the border and padding.
                    originX += containerBorderWidthX + containerPaddingX;
                    originY += containerBorderWidthY + containerPaddingY;

                    break;
                }

            case PositionOrigin_InnerBorder:
                {
                    // Pull inside the border, but not the padding.
                    originX += containerBorderWidthX;
                    originY += containerBorderWidthY;

                    break;
                }

            case PositionOrigin_Outer:
            default:
                {
                    // Don't want to adjust anything in this case.
                    break;
                }
            }




            // Offset.
            float offsetX = 0.0f;
            float offsetY = 0.0f;

            if (leftHasPriority)
            {
                positionX = static_cast<float>(GUIElementStyle_Get_left(element.style, positionTypeX));
            }
            else
            {
                positionX = -static_cast<float>(GUIElementStyle_Get_right(element.style, positionTypeX));
                offsetX  -= element.layout.width;
            }

            if (topHasPriority)
            {
                positionY = static_cast<float>(GUIElementStyle_Get_top(element.style, positionTypeY));
            }
            else
            {
                positionY = -static_cast<float>(GUIElementStyle_Get_bottom(element.style, positionTypeY));
                offsetY  -= element.layout.height;
            }


            if (positionTypeX == NumberType_Percent)
            {
                offsetX += Layout_GetElementWidthForRelativeSizing(*parent) * (positionX / 100.0f);
            }
            else if (positionTypeX == NumberType_Points)
            {
                offsetX += positionX * this->GetXDPIScalingFactor();
            }
            else
            {
                offsetX += positionX;
            }

            if (positionTypeY == NumberType_Percent)
            {
                offsetY += Layout_GetElementHeightForRelativeSizing(*parent) * (positionY / 100.0f);
            }
            else if (positionTypeY == NumberType_Points)
            {
                offsetY += positionY * this->GetYDPIScalingFactor();
            }
            else
            {
                offsetY += positionY;
            }



            element.layout.relativePosX = originX + offsetX;
            element.layout.relativePosY = originY + offsetY;
        }
    }

    void GUIContext::Layout_ValidateElementPosition_Auto(GUIElement &element)
    {
        assert(GUIElementStyle_Get_positioning(element.style) == Positioning_Auto);
        {
            auto parent = this->GetElementPtr(element.parent);

            ChildAxis       childAxis       = ChildAxis_Vertical;
            HorizontalAlign horizontalAlign = HorizontalAlign_Left;
            VerticalAlign   verticalAlign   = VerticalAlign_Top;

            float containerWidth             = 0.0f;
            float containerHeight            = 0.0f;
            float containerPaddingLeft       = 0.0f;
            float containerPaddingTop        = 0.0f;
            float containerPaddingRight      = 0.0f;
            float containerPaddingBottom     = 0.0f;
            float containerBorderLeftWidth   = 0.0f;
            float containerBorderTopWidth    = 0.0f;
            float containerBorderRightWidth  = 0.0f;
            float containerBorderBottomWidth = 0.0f;
            float containerInnerOffsetX      = 0.0f;
            float containerInnerOffsetY      = 0.0f;

            if (parent != nullptr)
            {
                childAxis       = GUIElementStyle_Get_childaxis(      parent->style);
                horizontalAlign = GUIElementStyle_Get_horizontalalign(parent->style);
                verticalAlign   = GUIElementStyle_Get_verticalalign(  parent->style);

                containerWidth  = parent->layout.width;
                containerHeight = parent->layout.height;

                containerPaddingLeft   = parent->layout.paddingLeft;
                containerPaddingTop    = parent->layout.paddingTop;
                containerPaddingRight  = parent->layout.paddingRight;
                containerPaddingBottom = parent->layout.paddingBottom;

                containerBorderLeftWidth   = parent->layout.borderLeftWidth;
                containerBorderTopWidth    = parent->layout.borderTopWidth;
                containerBorderRightWidth  = parent->layout.borderRightWidth;
                containerBorderBottomWidth = parent->layout.borderBottomWidth;

                containerInnerOffsetX = static_cast<float>(GUIElementStyle_Get_inneroffsetx(parent->style));
                containerInnerOffsetY = static_cast<float>(GUIElementStyle_Get_inneroffsety(parent->style));
            }
            else
            {
                auto surface = this->GetSurfacePtr(this->GetElementSurface(element.handle));
                if (surface != nullptr)
                {
                    containerWidth  = static_cast<float>(surface->width);
                    containerHeight = static_cast<float>(surface->height);
                }
            }


            // When we validate an auto-positioned element, we're actually going to validate it and all of it's auto-positioned siblings in one batch. We do this in two passes. The first
            // pass calculates the dimensions of the siblings. The second pass calculates the actual positions.

            // Pass #1 - Calculate the sibling dimensions.
            float siblingsWidth  = 0.0f;
            float siblingsHeight = 0.0f;

            this->IterateElementSiblingsAndSelf(element.handle, [&](HGUIElement hSibling) -> void {
                auto sibling = this->GetElementPtr(hSibling);
                assert(sibling != nullptr);
                {
                    if (GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                    {
                        // If the size of the element is invalid, we want to validate them first. We do this because validating the size will likely trigger
                        // an invalidation of the position, which will cause redundant position recalcuations. Instead, we simply just validate now.
                        if (GUIContext::Layout_IsElementWidthInvalid(*sibling))
                        {
                            this->Layout_ValidateElementWidth(*sibling);
                        }
                        if (GUIContext::Layout_IsElementHeightInvalid(*sibling))
                        {
                            this->Layout_ValidateElementHeight(*sibling);
                        }


                        if (childAxis == ChildAxis_Vertical)
                        {
                            // Child axis = vertical.
                            siblingsWidth   = GTLib::Max(siblingsWidth, GUIContext::Layout_GetElementOuterWidth(*sibling));
                            siblingsHeight += GUIContext::Layout_GetElementOuterHeight(*sibling);
                        }
                        else
                        {
                            // Child axis = horizontal.
                            siblingsWidth += GUIContext::Layout_GetElementOuterWidth(*sibling);
                            siblingsHeight = GTLib::Max(siblingsHeight, GUIContext::Layout_GetElementOuterHeight(*sibling));
                        }
                    }
                }
            });

            
            float offsetX = 0.0f;
            float offsetY = 0.0f;

            switch (horizontalAlign)
            {
            case HorizontalAlign_Right:
                {
                    offsetX = containerWidth - containerBorderRightWidth - containerPaddingRight - siblingsWidth;
                    break;
                }

            case HorizontalAlign_Center:
                {
                    offsetX = (containerWidth - siblingsWidth) / 2.0f;
                    break;
                }

            case HorizontalAlign_Left:
            default:
                {
                    offsetX = containerBorderLeftWidth + containerPaddingLeft;
                    break;
                }
            }


            switch (verticalAlign)
            {
            case VerticalAlign_Bottom:
                {
                    offsetY = containerHeight - containerBorderBottomWidth - containerPaddingBottom - siblingsHeight;
                    break;
                }

            case VerticalAlign_Center:
                {
                    offsetY = (containerHeight - siblingsHeight) / 2.0f;
                    break;
                }

            case VerticalAlign_Top:
            default:
                {
                    offsetY = containerBorderTopWidth + containerPaddingTop;
                    break;
                }
            }


            offsetX += containerInnerOffsetX;
            offsetY += containerInnerOffsetY;




            // Pass #2 - Set the position.
            float localOffsetX = 0;
            float localOffsetY = 0;

            this->IterateElementSiblingsAndSelf(element.handle, [&](HGUIElement hSibling) -> void {
                auto sibling = this->GetElementPtr(hSibling);
                assert(sibling != nullptr);
                {
                    if (GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                    {
                        // If the siblings layout is not invalid, we will need to invalidate the rectangle region of the element's previous layout rectangle. If the element's
                        // layout is already invalidated, this will have already been done.
                        if (sibling->layout.flags == 0)
                        {
                            this->Painting_InvalidateElementRect(*sibling);
                        }



                        float prevRelativeX = sibling->layout.relativePosX;
                        float prevRelativeY = sibling->layout.relativePosY;

                        if (childAxis == ChildAxis_Vertical)
                        {
                            // Child axis = vertical.
                            sibling->layout.relativePosY = offsetY + localOffsetY + sibling->layout.marginTop;

                            if (horizontalAlign == HorizontalAlign_Left)
                            {
                                sibling->layout.relativePosX = offsetX + localOffsetX + sibling->layout.marginLeft;
                            }
                            else
                            {
                                if (horizontalAlign == HorizontalAlign_Right)
                                {
                                    sibling->layout.relativePosX = containerWidth - containerBorderRightWidth - containerPaddingRight - sibling->layout.marginRight - sibling->layout.width;
                                }
                                else
                                {
                                    sibling->layout.relativePosX = (containerWidth - GUIContext::Layout_GetElementOuterWidth(*sibling)) * 0.5f + sibling->layout.marginLeft;
                                }
                            }
                        }
                        else
                        {
                            // Child axis = horizontal.
                            sibling->layout.relativePosX = offsetX + localOffsetX + sibling->layout.marginLeft;

                            if (verticalAlign == VerticalAlign_Top)
                            {
                                sibling->layout.relativePosY = offsetY + localOffsetY + sibling->layout.marginTop;
                            }
                            else
                            {
                                if (verticalAlign == VerticalAlign_Bottom)
                                {
                                    sibling->layout.relativePosY = containerHeight - containerBorderBottomWidth - containerPaddingBottom - sibling->layout.marginBottom - sibling->layout.height;
                                }
                                else
                                {
                                    sibling->layout.relativePosY = (containerHeight - GUIContext::Layout_GetElementOuterHeight(*sibling)) * 0.5f + sibling->layout.marginTop;
                                }
                            }
                        }




                        // We adjust the local offset based on the axis we're running along.
                        if (GUIElementStyle_Get_visible(sibling->style))
                        {
                            if (childAxis == ChildAxis_Vertical)
                            {
                                localOffsetY += GUIContext::Layout_GetElementOuterHeight(*sibling);
                            }
                            else
                            {
                                localOffsetX += GUIContext::Layout_GetElementOuterWidth(*sibling);
                            }
                        }


                        if (prevRelativeX != sibling->layout.relativePosX || prevRelativeY != sibling->layout.relativePosY)
                        {
                            this->Layout_MarkElementPositionAsChanged(*sibling);
                        }

                        // We now need to mark the position as valid.
                        this->Layout_MarkElementPositionAsValid(*sibling);
                    }
                }
            });


            if (parent != nullptr)
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(*parent, LayoutFlag_HeightInvalid);
                }
            }
        }
    }

    void GUIContext::Layout_ValidateElementText(GUIElement &element)
    {
        // TODO: Update the layout of the text container.



        // If the size is auto sized, it needs to be invalidated.
        if (GUIElementStyle_Get_IsAnyHeightOfType(element.style, NumberType_Auto))
        {
            this->Layout_InvalidateElementLayout(element, LayoutFlag_WidthInvalid);
        }
        if (GUIElementStyle_Get_IsAnyHeightOfType(element.style, NumberType_Auto))
        {
            this->Layout_InvalidateElementLayout(element, LayoutFlag_HeightInvalid);
        }


        // Now we need to remove ourselves from the invalid list.
        this->Layout_MarkElementTextAsValid(element);
    }



    void GUIContext::Layout_MarkElementWidthAsValid(GUIElement &element)
    {
        element.layout.flags &= ~LayoutFlag_WidthInvalid;
        if (element.layout.flags == 0)
        {
            m_layoutContext.invalidElements.Remove(element.layout.layoutValidationListItem);
            element.layout.layoutValidationListItem = nullptr;
        }
    }

    void GUIContext::Layout_MarkElementHeightAsValid(GUIElement &element)
    {
        element.layout.flags &= ~LayoutFlag_HeightInvalid;
        if (element.layout.flags == 0)
        {
            m_layoutContext.invalidElements.Remove(element.layout.layoutValidationListItem);
            element.layout.layoutValidationListItem = nullptr;
        }
    }

    void GUIContext::Layout_MarkElementPositionAsValid(GUIElement &element)
    {
        element.layout.flags &= ~LayoutFlag_RelativeXPositionInvalid;
        element.layout.flags &= ~LayoutFlag_RelativeYPositionInvalid;
        if (element.layout.flags == 0)
        {
            m_layoutContext.invalidElements.Remove(element.layout.layoutValidationListItem);
            element.layout.layoutValidationListItem = nullptr;
        }
    }

    void GUIContext::Layout_MarkElementTextAsValid(GUIElement &element)
    {
        element.layout.flags &= ~LayoutFlag_TextInvalid;
        if (element.layout.flags == 0)
        {
            m_layoutContext.invalidElements.Remove(element.layout.layoutValidationListItem);
            element.layout.layoutValidationListItem = nullptr;
        }
    }



    float GUIContext::Layout_UpdateElementWidth(GUIElement &element)
    {
        uint32_t styleWidthType;
        uint32_t styleWidth = GUIElementStyle_Get_width(element.style, styleWidthType);

        uint32_t styleMinWidthType;
        uint32_t styleMinWidth = GUIElementStyle_Get_minwidth(element.style, styleMinWidthType);

        uint32_t styleMaxWidthType;
        uint32_t styleMaxWidth = GUIElementStyle_Get_maxwidth(element.style, styleMaxWidthType);


        float newWidth = this->Layout_CalculateElementWidth(element, styleWidth, styleWidthType, true);             // <-- 'true' means we want to use flexing, if appropriate.

        float minWidth = 0;
        float maxWidth = newWidth;

        //if (styleMinWidthType != NumberType_Auto)
        {
            minWidth = this->Layout_CalculateElementWidth(element, styleMinWidth, styleMinWidthType, false);        // <-- 'false' means we don't want to consider flexing.
        }
        //if (styleMaxWidthType != NumberType_Auto)
        {
            maxWidth = this->Layout_CalculateElementWidth(element, styleMaxWidth, styleMaxWidthType, false);        // <-- 'false' means we don't want to consider flexing.
        }

        
        element.layout.unclampedWidth = newWidth;
        element.layout.width          = GTLib::Clamp(newWidth, minWidth, maxWidth);

        return GUIContext::Layout_GetElementOuterWidth(element);
    }

    float GUIContext::Layout_UpdateElementHeight(GUIElement &element)
    {
        uint32_t styleHeightType;
        uint32_t styleHeight = GUIElementStyle_Get_height(element.style, styleHeightType);

        uint32_t styleMinHeightType;
        uint32_t styleMinHeight = GUIElementStyle_Get_minheight(element.style, styleMinHeightType);

        uint32_t styleMaxHeightType;
        uint32_t styleMaxHeight = GUIElementStyle_Get_maxheight(element.style, styleMaxHeightType);


        float newHeight = this->Layout_CalculateElementHeight(element, styleHeight, styleHeightType, true);             // <-- 'true' means we want to use flexing, if appropriate.

        float minHeight = 0;
        float maxHeight = newHeight;

        //if (styleMinHeightType != NumberType_Auto)
        {
            minHeight = this->Layout_CalculateElementHeight(element, styleMinHeight, styleMinHeightType, false);        // <-- 'false' means we don't want to consider flexing.
        }
        //if (styleMaxHeightType != NumberType_Auto)
        {
            maxHeight = this->Layout_CalculateElementHeight(element, styleMaxHeight, styleMaxHeightType, false);        // <-- 'false' means we don't want to consider flexing.
        }

        
        element.layout.unclampedHeight = newHeight;
        element.layout.height          = GTLib::Clamp(newHeight, minHeight, maxHeight);

        return GUIContext::Layout_GetElementOuterHeight(element);
    }


    // It is possible to have elements sized such that they flex with the size of it's parent. If we want to perform this calculation, calculateFlexed will be
    // set to true.
    //
    // When calculating the size of a %-sized element, we use the following simple formula:
    //   s = p * r
    //
    // Where:
    //   s is the calculated size
    //   p is the size of the parent
    //   r is the size of the child relative to the size of the parent
    //
    // The complexity comes in the calculation of r, which is different depending on whether or not flexing is used.
    //
    // If the child is not flexed, r is simply the size ratio of the child without any changes. On the other hand, if the child is flexed, the size is
    // distributed between every flexed child within the available flexing area.
    //
    // The available flexing area is the width of the parent, minus and fixed sized elements (including margins). It is calculated using the parent as:
    //   f = 0
    //   for each child do
    //       if child is auto-positioned and visible then
    //          f += child margin
    //          f += child padding
    //
    //          if child size is fixed then
    //              f += child size
    //          end
    //       end if
    //   end loop
    //   a = parentWidth - f;
    //
    // We ignore relative and absolute positioned children since they are usually a special case. Invisible elements should not contribute to the calculation
    // either so they are also ignored. The margin and padding of children are always fixed (for now), so they are always included regardless of whether or
    // not the child has a flexible size.
    //
    // Once the available space has been determined, every element whose size is relative will be distributed into that space based on their width value. As
    // an example, if there are two elements, one with a size of 10% and the other with a size of 20%, the elements will be distributed such that the 10%
    // element is a third of the size of the 20% element (10% / (10% + 20%) = 0.333...)

    float GUIContext::Layout_CalculateElementWidth(GUIElement &element, uint32_t width, uint32_t widthType, bool calculateFlexed)
    {
        float result = 0.0f;

        switch (widthType)
        {
        case NumberType_Absolute:
        case NumberType_Pixels:
            {
                // We just set directly.
                result = static_cast<float>(width);
                break;
            }

        case NumberType_Points:
            {
                // Same as pixels/absolute, only now it's DPI aware.
                result = static_cast<float>(width) * this->GetXDPIScalingFactor();
                break;
            }

        case NumberType_Percent:     // Depends on the parent and non-% siblings.
            {
                float sizeRatio  = width / 100.0f;

                auto parent = this->GetElementPtr(element.parent);
                if (parent)
                {
                    float parentSize = this->Layout_GetElementWidthForRelativeSizing(*parent);

                    if (calculateFlexed && GUIElementStyle_Get_flexchildrenwidth(parent->style) && GUIElementStyle_Get_positioning(element.style) == Positioning_Auto)
                    {
                        if (GUIElementStyle_Get_childaxis(parent->style) == ChildAxis_Horizontal)
                        {
                            float totalPercent = 0.0f;
                            float totalFixed   = 0.0f;

                            // This loop will include this element.
                            for (HGUIElement hSiblingElement = parent->firstChild; hSiblingElement != 0; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                            {
                                auto siblingElement = this->GetElementPtr(hSiblingElement);
                                assert(siblingElement != nullptr);
                                {
                                    if (GUIElementStyle_Get_positioning(siblingElement->style) == Positioning_Auto && GUIElementStyle_Get_visible(siblingElement->style))
                                    {
                                        totalFixed += GUIContext::Layout_GetElementHorizontalMargin( *siblingElement);
                                        totalFixed += GUIContext::Layout_GetElementHorizontalPadding(*siblingElement);


                                        uint32_t siblingWidthType;
                                        uint32_t siblingWidth = GUIElementStyle_Get_width(siblingElement->style, siblingWidthType);

                                        if (siblingWidthType == NumberType_Percent)
                                        {
                                            totalPercent += static_cast<float>(siblingWidth);
                                        }
                                        else
                                        {
                                            totalFixed += siblingElement->layout.width;
                                        }
                                    }
                                }
                            }

                            parentSize -= totalFixed;
                            if (totalPercent > 0.0f)
                            {
                                sizeRatio = width / totalPercent;
                            }
                            else
                            {
                                sizeRatio = 0.0f;
                            }
                        }
                        else
                        {
                            sizeRatio = 1.0f;
                        }
                    }


                    result = parentSize * sizeRatio;
                }
                else
                {
                    // The element does not have a parent, so we will size based on the surface.
                    auto surface = this->GetSurfacePtr(this->GetElementSurface(element.handle));
                    if (surface != nullptr)
                    {
                        result = surface->width * sizeRatio;
                    }
                }

                break;
            }

        case NumberType_Auto:       // Depends on the children.
            {
                result = GUIContext::Layout_GetElementHorizontalBorderSize(element) + GUIContext::Layout_GetElementHorizontalPadding(element);

                // The width will be the width of the children plus padding.
                if (element.firstChild != 0)
                {
                    result += this->Layout_CalculateElementChildrenWidthForAutoSize(element);
                }
                else if (this->DoesElementHaveText(element.handle))
                {
                    result += element.textManager.GetTextWidth();
                }

                break;
            }
        }


        return result;
    }

    float GUIContext::Layout_CalculateElementHeight(GUIElement &element, uint32_t height, uint32_t heightType, bool calculateFlexed)
    {
        float result = 0.0f;

        switch (heightType)
        {
        case NumberType_Absolute:
        case NumberType_Pixels:
            {
                // We just set directly.
                result = static_cast<float>(height);
                break;
            }

        case NumberType_Points:
            {
                // Same as pixels/absolute, only now it's DPI aware.
                result = static_cast<float>(height) * this->GetYDPIScalingFactor();
                break;
            }

        case NumberType_Percent:     // Depends on the parent and non-% siblings.
            {
                float sizeRatio  = height / 100.0f;

                auto parent = this->GetElementPtr(element.parent);
                if (parent)
                {
                    float parentSize = this->Layout_GetElementHeightForRelativeSizing(*parent);

                    if (calculateFlexed && GUIElementStyle_Get_flexchildrenheight(parent->style) && GUIElementStyle_Get_positioning(element.style) == Positioning_Auto)
                    {
                        if (GUIElementStyle_Get_childaxis(parent->style) == ChildAxis_Vertical)
                        {
                            float totalPercent = 0.0f;
                            float totalFixed   = 0.0f;

                            // This loop will include this element.
                            for (HGUIElement hSiblingElement = parent->firstChild; hSiblingElement != 0; hSiblingElement = this->GetElementNextSibling(hSiblingElement))
                            {
                                auto siblingElement = this->GetElementPtr(hSiblingElement);
                                assert(siblingElement != nullptr);
                                {
                                    if (GUIElementStyle_Get_positioning(siblingElement->style) == Positioning_Auto && GUIElementStyle_Get_visible(siblingElement->style))
                                    {
                                        totalFixed += GUIContext::Layout_GetElementVerticalMargin( *siblingElement);
                                        totalFixed += GUIContext::Layout_GetElementVerticalPadding(*siblingElement);


                                        uint32_t siblingHeightType;
                                        uint32_t siblingHeight = GUIElementStyle_Get_height(siblingElement->style, siblingHeightType);

                                        if (siblingHeightType == NumberType_Percent)
                                        {
                                            totalPercent += static_cast<float>(siblingHeight);
                                        }
                                        else
                                        {
                                            totalFixed += siblingElement->layout.height;
                                        }
                                    }
                                }
                            }

                            parentSize -= totalFixed;
                            if (totalPercent > 0.0f)
                            {
                                sizeRatio = height / totalPercent;
                            }
                            else
                            {
                                sizeRatio = 0.0f;
                            }
                        }
                        else
                        {
                            sizeRatio = 1.0f;
                        }
                    }


                    result = parentSize * sizeRatio;
                }
                else
                {
                    // The element does not have a parent, so we will size based on the surface.
                    auto surface = this->GetSurfacePtr(this->GetElementSurface(element.handle));
                    if (surface != nullptr)
                    {
                        result = surface->height * sizeRatio;
                    }
                }

                break;
            }

        case NumberType_Auto:       // Depends on the children.
            {
                result = GUIContext::Layout_GetElementVerticalBorderSize(element) + GUIContext::Layout_GetElementVerticalPadding(element);

                // The height will be the height of the children plus padding.
                if (element.firstChild != 0)
                {
                    result += this->Layout_CalculateElementChildrenHeightForAutoSize(element);
                }
                else if (this->DoesElementHaveText(element.handle))
                {
                    result += element.textManager.GetTextHeight();
                }

                break;
            }
        }


        return result;
    }




    float GUIContext::Layout_CalculateElementChildrenWidthForAutoSize(GUIElement &element) const
    {
        float result = 0.0f;

        ChildAxis childAxis = GUIElementStyle_Get_childaxis(element.style);
        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(childElement->style))
                        {
                            if (GUIElementStyle_Get_positioning(childElement->style) == Positioning_Auto)
                            {
                                result += this->Layout_GetElementHorizontalMargin(*childElement);       // <-- Always include the margin.

                                uint32_t widthType;
                                uint32_t minWidthType;
                                uint32_t maxWidthType;
                                GUIElementStyle_Get_allwidthtypes(childElement->style, widthType, minWidthType, maxWidthType);

                                if (widthType == NumberType_Auto && (minWidthType == NumberType_Percent || maxWidthType == NumberType_Percent))
                                {
                                    result += childElement->layout.unclampedWidth;
                                }
                                else
                                {
                                    result += childElement->layout.width;
                                }
                            }
                        }
                    }
                }

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(childElement->style))
                        {
                            if (GUIElementStyle_Get_positioning(childElement->style) == Positioning_Auto)
                            {
                                float childWidth = this->Layout_GetElementHorizontalMargin(*childElement);       // <-- Always include the margin.

                                uint32_t widthType;
                                uint32_t minWidthType;
                                uint32_t maxWidthType;
                                GUIElementStyle_Get_allwidthtypes(childElement->style, widthType, minWidthType, maxWidthType);

                                if (widthType == NumberType_Auto && (minWidthType == NumberType_Percent || maxWidthType == NumberType_Percent))
                                {
                                    childWidth += childElement->layout.unclampedWidth;
                                }
                                else
                                {
                                    childWidth += childElement->layout.width;
                                }

                                result = GTLib::Max(result, childWidth);
                            }
                        }
                    }
                }

                break;
            }
        }

        return result;
    }

    float GUIContext::Layout_CalculateElementChildrenHeightForAutoSize(GUIElement &element) const
    {
        float result = 0.0f;

        ChildAxis childAxis = GUIElementStyle_Get_childaxis(element.style);
        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(childElement->style))
                        {
                            if (GUIElementStyle_Get_positioning(childElement->style) == Positioning_Auto)
                            {
                                float childHeight = this->Layout_GetElementVerticalMargin(*childElement);       // <-- Always include the margin.

                                uint32_t heightType;
                                uint32_t minHeightType;
                                uint32_t maxHeightType;
                                GUIElementStyle_Get_allheighttypes(childElement->style, heightType, minHeightType, maxHeightType);

                                if (heightType == NumberType_Auto && (minHeightType == NumberType_Percent || maxHeightType == NumberType_Percent))
                                {
                                    childHeight += childElement->layout.unclampedHeight;
                                }
                                else
                                {
                                    childHeight += childElement->layout.height;
                                }

                                result = GTLib::Max(result, childHeight);
                            }
                        }
                    }
                }

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(childElement->style))
                        {
                            if (GUIElementStyle_Get_positioning(childElement->style) == Positioning_Auto)
                            {
                                result += this->Layout_GetElementVerticalMargin(*childElement);       // <-- Always include the margin.

                                uint32_t heightType;
                                uint32_t minHeightType;
                                uint32_t maxHeightType;
                                GUIElementStyle_Get_allheighttypes(childElement->style, heightType, minHeightType, maxHeightType);

                                if (heightType == NumberType_Auto && (minHeightType == NumberType_Percent || maxHeightType == NumberType_Percent))
                                {
                                    result += childElement->layout.unclampedHeight;
                                }
                                else
                                {
                                    result += childElement->layout.height;
                                }
                            }
                        }
                    }
                }

                break;
            }
        }

        return result;
    }


    float GUIContext::Layout_CalculateElementChildrenWidth(GUIElement &element)
    {
        float result = 0.0f;

        ChildAxis childAxis = GUIElementStyle_Get_childaxis(element.style);
        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(childElement->style))
                        {
                            if (GUIElementStyle_Get_positioning(childElement->style) == Positioning_Auto)
                            {
                                result += GUIContext::Layout_GetElementOuterWidth(*childElement);
                            }
                        }
                    }
                }

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(childElement->style))
                        {
                            if (GUIElementStyle_Get_positioning(childElement->style) == Positioning_Auto)
                            {
                                result = GTLib::Max(result, GUIContext::Layout_GetElementOuterWidth(*childElement));
                            }
                        }
                    }
                }

                break;
            }
        }

        return result;
    }

    float GUIContext::Layout_CalculateElementChildrenHeight(GUIElement &element)
    {
        float result = 0.0f;

        ChildAxis childAxis = GUIElementStyle_Get_childaxis(element.style);
        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(childElement->style))
                        {
                            if (GUIElementStyle_Get_positioning(childElement->style) == Positioning_Auto)
                            {
                                result = GTLib::Max(result, GUIContext::Layout_GetElementOuterHeight(*childElement));
                            }
                        }
                    }
                }

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                for (HGUIElement hChildElement = element.firstChild; hChildElement != 0; hChildElement = this->GetElementNextSibling(hChildElement))
                {
                    auto childElement = this->GetElementPtr(hChildElement);
                    assert(childElement != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(childElement->style))
                        {
                            if (GUIElementStyle_Get_positioning(childElement->style) == Positioning_Auto)
                            {
                                result += GUIContext::Layout_GetElementOuterHeight(*childElement);
                            }
                        }
                    }
                }

                break;
            }
        }

        return result;
    }


    float GUIContext::Layout_CalculateTotalVisibleAutoPositionedSiblingsWidth(GUIElement &element)
    {
        float result = 0.0f;

        ChildAxis childAxis = ChildAxis_Vertical;
        
        auto parent = this->GetElementPtr(element.parent);
        if (parent != nullptr)
        {
            childAxis = GUIElementStyle_Get_childaxis(parent->style);
        }


        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                if (GUIElementStyle_Get_visible(element.style) && GUIElementStyle_Get_positioning(element.style) == Positioning_Auto)
                {
                    result = GUIContext::Layout_GetElementOuterWidth(element);
                }

                // Prev
                for (HGUIElement hSibling = element.prevSibling; hSibling != 0; hSibling = this->GetElementPreviousSibling(hSibling))
                {
                    auto sibling = this->GetElementPtr(hSibling);
                    assert(sibling != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(sibling->style) && GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                        {
                            result += GUIContext::Layout_GetElementOuterWidth(*sibling);
                        }
                    }
                }

                // Next
                for (HGUIElement hSibling = element.nextSibling; hSibling != 0; hSibling = this->GetElementNextSibling(hSibling))
                {
                    auto sibling = this->GetElementPtr(hSibling);
                    assert(sibling != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(sibling->style) && GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                        {
                            result += GUIContext::Layout_GetElementOuterWidth(*sibling);
                        }
                    }
                }

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                if (GUIElementStyle_Get_visible(element.style) && GUIElementStyle_Get_positioning(element.style) == Positioning_Auto)
                {
                    result = GUIContext::Layout_GetElementOuterWidth(element);
                }


                // Prev
                for (HGUIElement hSibling = element.prevSibling; hSibling != 0; hSibling = this->GetElementPreviousSibling(hSibling))
                {
                    auto sibling = this->GetElementPtr(hSibling);
                    assert(sibling != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(sibling->style) && GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                        {
                            result = GTLib::Max(result, GUIContext::Layout_GetElementOuterWidth(*sibling));
                        }
                    }
                }

                // Next
                for (HGUIElement hSibling = element.nextSibling; hSibling != 0; hSibling = this->GetElementNextSibling(hSibling))
                {
                    auto sibling = this->GetElementPtr(hSibling);
                    assert(sibling != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(sibling->style) && GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                        {
                            result = GTLib::Max(result, GUIContext::Layout_GetElementOuterWidth(*sibling));
                        }
                    }
                }

                break;
            }
        }

        return result;
    }

    float GUIContext::Layout_CalculateTotalVisibleAutoPositionedSiblingsHeight(GUIElement &element)
    {
        float result = 0.0f;

        ChildAxis childAxis = ChildAxis_Vertical;
        
        auto parent = this->GetElementPtr(element.parent);
        if (parent != nullptr)
        {
            childAxis = GUIElementStyle_Get_childaxis(parent->style);
        }


        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                if (GUIElementStyle_Get_visible(element.style) && GUIElementStyle_Get_positioning(element.style) == Positioning_Auto)
                {
                    result = GUIContext::Layout_GetElementOuterHeight(element);
                }

                // Prev
                for (HGUIElement hSibling = element.prevSibling; hSibling != 0; hSibling = this->GetElementPreviousSibling(hSibling))
                {
                    auto sibling = this->GetElementPtr(hSibling);
                    assert(sibling != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(sibling->style) && GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                        {
                            result = GTLib::Max(result, GUIContext::Layout_GetElementOuterHeight(*sibling));
                        }
                    }
                }

                // Next
                for (HGUIElement hSibling = element.nextSibling; hSibling != 0; hSibling = this->GetElementNextSibling(hSibling))
                {
                    auto sibling = this->GetElementPtr(hSibling);
                    assert(sibling != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(sibling->style) && GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                        {
                            result = GTLib::Max(result, GUIContext::Layout_GetElementOuterHeight(*sibling));
                        }
                    }
                }

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                if (GUIElementStyle_Get_visible(element.style) && GUIElementStyle_Get_positioning(element.style) == Positioning_Auto)
                {
                    result = GUIContext::Layout_GetElementOuterHeight(element);
                }


                // Prev
                for (HGUIElement hSibling = element.prevSibling; hSibling != 0; hSibling = this->GetElementPreviousSibling(hSibling))
                {
                    auto sibling = this->GetElementPtr(hSibling);
                    assert(sibling != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(sibling->style) && GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                        {
                            result += GUIContext::Layout_GetElementOuterHeight(*sibling);
                        }
                    }
                }

                // Next
                for (HGUIElement hSibling = element.nextSibling; hSibling != 0; hSibling = this->GetElementNextSibling(hSibling))
                {
                    auto sibling = this->GetElementPtr(hSibling);
                    assert(sibling != nullptr);
                    {
                        if (GUIElementStyle_Get_visible(sibling->style) && GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                        {
                            result += GUIContext::Layout_GetElementOuterHeight(*sibling);
                        }
                    }
                }
 

                break;
            }
        }

        return result;
    }


    GUIElement* GUIContext::Layout_GetNeighboringAutoPositionedSibling(GUIElement &element)
    {
        for (HGUIElement hSibling = element.nextSibling; hSibling != 0; hSibling = this->GetElementNextSibling(hSibling))
        {
            auto sibling = this->GetElementPtr(hSibling);
            if (sibling != nullptr)
            {
                if (GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                {
                    return sibling;
                }
            }
        }

        for (HGUIElement hSibling = element.prevSibling; hSibling != 0; hSibling = this->GetElementPreviousSibling(hSibling))
        {
            auto sibling = this->GetElementPtr(hSibling);
            if (sibling != nullptr)
            {
                if (GUIElementStyle_Get_positioning(sibling->style) == Positioning_Auto)
                {
                    return sibling;
                }
            }
        }

        return nullptr;
    }

    GUIElement* GUIContext::Layout_GetFirstAutoPositionedChild(GUIElement &element)
    {
        for (HGUIElement hChild = element.firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
        {
            auto child = this->GetElementPtr(hChild);
            if (child != nullptr)
            {
                if (GUIElementStyle_Get_positioning(child->style) == Positioning_Auto)
                {
                    return child;
                }
            }
        }

        return nullptr;
    }



    void GUIContext::Layout_MarkElementPositionAsChanged(GUIElement &element)
    {
        // If the flags specifying the validated layout properties is non-zero, we know it is already in our validated elements list and thus
        // doesn't need to be added again.
        if (element.layout.layoutChangeFlags == 0)
        {
            m_layoutContext.validatedElements.PushBack(&element);
        }


        // When the relative position of this element changes, the absolute position of both this element and it's descendents will need
        // to be recalculated. We're not going to do that yet, however we will need to get setup for it.
        //
        // The absolute position of elements is calculated recursively - when an element has it's position updated, it will update the
        // position of it's children, and so on.
        if (!this->Layout_HasElementAbsolutePositionChanged(element))      // <-- The is recursive.
        {
            // If a child to this element is already in this list it needs to be removed.
            for (size_t iChildElement = 0; iChildElement < m_layoutContext.elementsWithInvalidAbsolutePositions.GetCount(); )
            {
                auto childElement = m_layoutContext.elementsWithInvalidAbsolutePositions[iChildElement];
                assert(childElement != nullptr);
                {
                    if (this->IsElementAncestor(childElement->handle, element.handle))
                    {
                        m_layoutContext.elementsWithInvalidAbsolutePositions.Remove(iChildElement);
                    }
                    else
                    {
                        ++iChildElement;
                    }
                }
            }


            m_layoutContext.elementsWithInvalidAbsolutePositions.PushBack(&element);
        }


        element.layout.layoutChangeFlags |= LayoutFlag_PositionChanged;
    }

    void GUIContext::Layout_MarkElementSizeAsChanged(GUIElement &element)
    {
        // If the flags specifying the validated layout properties is non-zero, we know it is already in our validated elements list and thus
        // doesn't need to be added again.
        if (element.layout.layoutChangeFlags == 0)
        {
            m_layoutContext.validatedElements.PushBack(&element);
        }

        element.layout.layoutChangeFlags |= LayoutFlag_SizeChanged;
    }


    bool GUIContext::Layout_HasElementAbsolutePositionChanged(GUIElement &element)
    {
        if ((element.layout.layoutChangeFlags & LayoutFlag_PositionChanged) != 0)
        {
            return true;
        }


        // If any ancestor has an invalid absolute position, then so will this one.
        auto parent = this->GetElementPtr(element.parent);
        if (parent != nullptr)
        {
            return this->Layout_HasElementAbsolutePositionChanged(*parent);
        }


        return false;
    }


    void GUIContext::Layout_UpdateElementAbsolutePositions()
    {
        for (size_t iElement = 0; iElement < m_layoutContext.elementsWithInvalidAbsolutePositions.GetCount(); ++iElement)
        {
            auto element = m_layoutContext.elementsWithInvalidAbsolutePositions[iElement];
            assert(element != nullptr);
            {
                this->Layout_UpdateElementAbsolutePosition(*element);
            }
        }


        m_layoutContext.elementsWithInvalidAbsolutePositions.Clear();
    }

    void GUIContext::Layout_UpdateElementAbsolutePosition(GUIElement &element)
    {
        element.layout.absolutePosX = element.layout.relativePosX;
        element.layout.absolutePosY = element.layout.relativePosY;

        auto parent = this->GetElementPtr(element.parent);
        if (parent != nullptr)
        {
            if (GUIElementStyle_Get_positioning(element.style) != GT::Positioning_Absolute)
            {
                element.layout.absolutePosX += parent->layout.absolutePosX;
                element.layout.absolutePosY += parent->layout.absolutePosY;
            }
        }


        // Children will need to be updated.
        for (HGUIElement hChild = element.firstChild; hChild != 0; hChild = this->GetElementNextSibling(hChild))
        {
            auto child = this->GetElementPtr(hChild);
            assert(child != nullptr);
            {
                this->Layout_UpdateElementAbsolutePosition(*child);
            }
        }
    }

    void GUIContext::Layout_InvalidateRectsAndPostEventsToElements()
    {
        for (size_t iElement = 0; iElement < m_layoutContext.validatedElements.GetCount(); ++iElement)
        {
            auto element = m_layoutContext.validatedElements[iElement];
            assert(element != nullptr);
            {
                // Invalidate the rectangle.
                this->Painting_InvalidateElementRect(*element);


                // Post events.
                //
                // There is a chance when events are posted that the element will be deleted. We need to make sure we don't attempted to dereference the element pointer
                // after posting events.
                HGUIElement hElement = element->handle;
                unsigned int width  = static_cast<unsigned int>(element->layout.width);
                unsigned int height = static_cast<unsigned int>(element->layout.height);
                int xPos = static_cast<int>(element->layout.relativePosX);
                int yPos = static_cast<int>(element->layout.relativePosY);
                uint16_t layoutChangeFlags = element->layout.layoutChangeFlags;


                // We need to clear the change flags on the element so we know they are now valid. We do this before posting the events so that we don't dereference the
                // element object after posting the events since it is possible for the host application to delete it.
                element->layout.layoutChangeFlags &= ~(LayoutFlag_PositionChanged | LayoutFlag_SizeChanged);


                if ((layoutChangeFlags & LayoutFlag_SizeChanged) && this->GetElementPtr(hElement) == element)
                {
                    this->PostEvent_OnSize(element->handle, width, height);
                }
                if ((layoutChangeFlags & LayoutFlag_PositionChanged) && this->GetElementPtr(hElement) == element)
                {
                    this->PostEvent_OnMove(element->handle, xPos, yPos);
                }
            }
        }

        m_layoutContext.validatedElements.Clear();
    }



    ////////////////////////////
    // Static Layout Helpers

    float GUIContext::Layout_GetElementHorizontalPadding(GUIElement &element)
    {
        return element.layout.paddingLeft + element.layout.paddingRight;
    }

    float GUIContext::Layout_GetElementVerticalPadding(GUIElement &element)
    {
        return element.layout.paddingTop + element.layout.paddingBottom;
    }

    float GUIContext::Layout_GetElementHorizontalMargin(GUIElement &element)
    {
        return element.layout.marginLeft + element.layout.marginRight;
    }

    float GUIContext::Layout_GetElementVerticalMargin(GUIElement &element)
    {
        return element.layout.marginTop + element.layout.marginBottom;
    }

    float GUIContext::Layout_GetElementOuterWidth(GUIElement &element)
    {
        return element.layout.width + GUIContext::Layout_GetElementHorizontalMargin(element);
    }

    float GUIContext::Layout_GetElementHorizontalBorderSize(GUIElement &element)
    {
        return element.layout.borderLeftWidth + element.layout.borderRightWidth;
    }

    float GUIContext::Layout_GetElementVerticalBorderSize(GUIElement &element)
    {
        return element.layout.borderTopWidth + element.layout.borderBottomWidth;
    }

    float GUIContext::Layout_GetElementOuterHeight(GUIElement &element)
    {
        return element.layout.height + GUIContext::Layout_GetElementVerticalMargin(element);
    }

    float GUIContext::Layout_GetElementInnerWidth(GUIElement &element)
    {
        return element.layout.width - GUIContext::Layout_GetElementHorizontalPadding(element) - GUIContext::Layout_GetElementHorizontalBorderSize(element);
    }

    float GUIContext::Layout_GetElementInnerHeight(GUIElement &element)
    {
        return element.layout.height - GUIContext::Layout_GetElementVerticalPadding(element) - GUIContext::Layout_GetElementVerticalBorderSize(element);
    }

    float GUIContext::Layout_GetElementInnerBorderWidth(GUIElement &element)
    {
        return element.layout.width - GUIContext::Layout_GetElementHorizontalBorderSize(element);
    }

    float GUIContext::Layout_GetElementInnerBorderHeight(GUIElement &element)
    {
        return element.layout.height - GUIContext::Layout_GetElementVerticalBorderSize(element);
    }


    float GUIContext::Layout_GetElementWidthForRelativeSizing(GUIElement &element)
    {
        switch (GUIElementStyle_Get_childrenwidthboundary(element.style))
        {
        case ChildrenSizeBoundary_Outer:             // The outside of the borders.
            {
                return element.layout.width;
            }

        case ChildrenSizeBoundary_InnerBorder:       // The inside of the borders, but outside the padding.
            {
                return GUIContext::Layout_GetElementInnerBorderWidth(element);
            }

        case ChildrenSizeBoundary_Inner:             // The inside of the borders and padding (default)
        default:
            {
                return GUIContext::Layout_GetElementInnerWidth(element);
            }
        }
    }

    float GUIContext::Layout_GetElementHeightForRelativeSizing(GUIElement &element)
    {
        switch (GUIElementStyle_Get_childrenheightboundary(element.style))
        {
        case ChildrenSizeBoundary_Outer:             // The outside of the borders.
            {
                return element.layout.height;
            }

        case ChildrenSizeBoundary_InnerBorder:       // The inside of the borders, but outside the padding.
            {
                return GUIContext::Layout_GetElementInnerBorderHeight(element);
            }

        case ChildrenSizeBoundary_Inner:             // The inside of the borders and padding (default)
        default:
            {
                return GUIContext::Layout_GetElementInnerHeight(element);
            }
        }
    }

    bool GUIContext::Layout_IsElementWidthInvalid(GUIElement &element)
    {
        return (element.layout.flags & LayoutFlag_WidthInvalid) != 0;
    }

    bool GUIContext::Layout_IsElementHeightInvalid(GUIElement &element)
    {
        return (element.layout.flags & LayoutFlag_HeightInvalid) != 0;
    }

    bool GUIContext::Layout_IsElementPositionInvalid(GUIElement &element)
    {
        return (element.layout.flags & (LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid)) != 0;
    }
}