// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUIContextBase.hpp>

namespace GT
{
    GUIContextBase::GUIContextBase()
        : m_layoutContext()
    {
    }

    GUIContextBase::~GUIContextBase()
    {
    }

    GUISurface* GUIContextBase::CreateSurface()
    {
        return new GUISurface(0);
    }

    void GUIContextBase::DeleteSurface(GUISurface* pSurface)
    {
        if (pSurface != nullptr)
        {
            // NOTE: What about elements that are tied to this surface?
            delete pSurface;
        }
    }

    GUISurface* GUIContextBase::FindSurfaceByID(const char* id) const
    {
        GUISurface* pFoundSurface = nullptr;

        this->IterateSurfaces([&](GUISurface* pSurface) -> bool
        {
            assert(pSurface != nullptr);
            {
                if (pSurface->id == id)
                {
                    pFoundSurface = pSurface;
                    return false;
                }
            }

            return true;
        });


        return pFoundSurface;
    }

    void GUIContextBase::SetSurfaceID(GUISurface* pSurface, const char* id)
    {
        assert(pSurface != nullptr);

        pSurface->id = id;
    }

    const char* GUIContextBase::GetSurfaceID(GUISurface* pSurface) const
    {
        assert(pSurface != nullptr);

        return pSurface->id.c_str();
    }

    void GUIContextBase::SetSurfaceSize(GUISurface* pSurface, unsigned int newWidth, unsigned int newHeight)
    {
        assert(pSurface != nullptr);

        if (pSurface != nullptr)
        {
            pSurface->width  = newWidth;
            pSurface->height = newHeight;


            this->BeginBatch();

            // Top level elements may need to have their layouts invalidated.
            for (size_t iTopLevelElement = 0; iTopLevelElement < pSurface->topLevelElements.GetCount(); ++iTopLevelElement)
            {
                auto pElement = pSurface->topLevelElements[iTopLevelElement];
                assert(pElement != nullptr);
                {
                    int flags = 0;
                    
                    // Width.
                    if (GUIElementStyle_Get_IsAnyWidthOfType(pElement->style, NumberType_Percent))
                    {
                        flags |= LayoutFlag_WidthInvalid;
                    }

                    // Height.
                    if (GUIElementStyle_Get_IsAnyHeightOfType(pElement->style, NumberType_Percent))
                    {
                        flags |= LayoutFlag_HeightInvalid;
                    }


                    // Position.
                    if (GUIElementStyle_Get_righthaspriority(pElement->style) || GUIElementStyle_Get_bottomhaspriority(pElement->style))
                    {
                        flags |= LayoutFlag_PositionInvalid;
                    }


                    if (flags != 0)
                    {
                        this->Layout_InvalidateElementLayout(*pElement, flags);
                    }
                }
            }

            this->EndBatch();
        }
    }

    void GUIContextBase::GetSurfaceSize(GUISurface* pSurface, unsigned int &widthOut, unsigned int &heightOut) const
    {
        assert(pSurface != nullptr);

        widthOut  = pSurface->width;
        heightOut = pSurface->height;
    }

    bool GUIContextBase::DoesSurfaceContainGUIElement(GUISurface* pSurface, GUIElement* pElement) const
    {
        return this->GetElementSurface(pElement) == pSurface;
    }

    void GUIContextBase::SetSurfacePaintingMode(GUISurface* pSurface, GUIPaintingMode paintingMode)
    {
        assert(pSurface != nullptr);

        pSurface->paintingMode = paintingMode;
    }




    ////////////////////////////////////////////////////////////////
    // Elements

    GUIElement* GUIContextBase::CreateElement()
    {
        auto pElement = new GUIElement(0);


        // Set the default styles.
        memset(&pElement->style, 0, sizeof(pElement->style));

        // Min/max size.
        //
        // NOTE: We don't want to use SetGUIElementMinWidthToDefault(), etc. because we that will trigger a layout validation which we don't really want. To work around this, we
        //       need to set it directly on the style.
        GUIElementStyle_Set_minwidth(pElement->style, 0U,                         NumberType_Absolute);
        GUIElementStyle_Set_maxwidth(pElement->style, GUIStyleNumber_MaxUnsigned, NumberType_Absolute);

        GUIElementStyle_Set_minheight(pElement->style, 0U,                         NumberType_Absolute);
        GUIElementStyle_Set_maxheight(pElement->style, GUIStyleNumber_MaxUnsigned, NumberType_Absolute);


        // Set the default font.
        this->SetElementFont(pElement, "Liberation Sans", FontWeight_Medium, FontSlant_None, 9, NumberType_Points);
    }

    void GUIContextBase::DeleteElement(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        this->BeginBatch();
        {
            // The area consumed by the deleted element needs to be invalidated so that it will be redrawn.
            this->Painting_InvalidateElementRect(pElement);


            // We'll recursively delete the children first.
            while (pElement->firstChild != 0)
            {
                this->DeleteElement(pElement->firstChild);
            }


            // The element needs to be isolated before deleting.
            this->DetachElementFromParentAndSiblings(pElement);


            // If the element has an invalid layout, clear it from the layout manager.
            if (pElement->layout.layoutValidationListItem != nullptr)
            {
                m_layoutContext.invalidElements.Remove(pElement->layout.layoutValidationListItem);
            }

            // Now the element can actually be deleted.
            delete pElement;
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementID(GUIElement* pElement, const char* id)
    {
        assert(pElement != nullptr);

        pElement->id = id;
    }

    const char* GUIContextBase::GetElementID(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->id.c_str();
    }

    GUIElement* GUIContextBase::FindElementByID(const char* elementID) const
    {
        GUIElement* pFoundElement = nullptr;

        this->IterateElements([&](GUIElement* pElement) -> bool
        {
            assert(pElement != nullptr);
            {
                if (pElement->id == elementID)
                {
                    pFoundElement = pElement;
                    return false;
                }
            }

            return true;
        });


        return pFoundElement;
    }


    GUIElement* GUIContextBase::GetElementParent(GUIElement* pChildElement) const
    {
        assert(pChildElement != nullptr);

        return pChildElement->pParent;
    }

    void GUIContextBase::SetElementParent(GUIElement* pChildElement, GUIElement* pParentElement)
    {
        assert(pChildElement != nullptr);

        if (pParentElement != 0)
        {
            this->AppendChildElement(pParentElement, pChildElement);
        }
        else
        {
            this->DetachElementFromParent(pChildElement);
        }
    }

    void GUIContextBase::DetachElementFromParent(GUIElement* pChildElement)
    {
        assert(pChildElement != nullptr);

        if (pChildElement->pParent != nullptr)
        {
            this->DetachElementFromParentAndSiblings(pChildElement);
        }
    }

    void GUIContextBase::DetachElementFromParentAndSiblings(GUIElement* pChildElement)
    {
        this->BeginBatch();
        {
            // The parent needs to be done first because we will need access to the current siblings (the siblings before detachment).

            auto pParent = pChildElement->pParent;
            if (pParent != nullptr)
            {
                if (pParent->firstChild == pChildElement)
                {
                    assert(pChildElement->prevSibling == nullptr);
                    pParent->firstChild = pChildElement->nextSibling;
                }

                if (pParent->lastChild == pChildElement)
                {
                    assert(pChildElement->nextSibling == nullptr);
                    pParent->lastChild = pChildElement->prevSibling;
                }


                pChildElement->parent = 0;


                // The size of the parent may have changed.
                if (GUIElementStyle_Get_IsAnyWidthOfType(pParent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pParent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pParent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pParent, LayoutFlag_HeightInvalid);
                }
            }


            // Before we detach from siblings, we need to check if we need to update the positions of auto-positioned elements. We only need to invalidate one
            // of these - the validation stage will ensure all auto-positioend siblings will be updated at the same time.
            if (GUIElementStyle_Get_positioning(pChildElement->style) == Positioning_Auto)
            {
                GUIElement* neigboringAutoPositionedSibling = this->Layout_GetNeighboringAutoPositionedSibling(pChildElement);
                if (neigboringAutoPositionedSibling != nullptr)
                {
                    this->Layout_InvalidateElementLayout(*neigboringAutoPositionedSibling, LayoutFlag_PositionInvalid);
                }
            }


            // The siblings can only be done after detaching from the parent.

            auto pPrevSibling = pChildElement->prevSibling;
            auto pNextSibling = pChildElement->nextSibling;

            if (pPrevSibling != nullptr)
            {
                pPrevSibling->nextSibling = pNextSibling;
            }

            if (pNextSibling != nullptr)
            {
                pNextSibling->prevSibling = pPrevSibling;
            }

            pChildElement->prevSibling = nullptr;
            pChildElement->nextSibling = nullptr;


            // Invalidate the applicable layouts.
            this->Layout_InvalidateElementLayout(pChildElement, LayoutFlag_SizeAndPositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::AppendChildElement(GUIElement* pParentElement, GUIElement* pChildElement)
    {
        assert(pParentElement != nullptr);
        assert(pChildElement  != nullptr);


        this->BeginBatch();
        {
            // If the parent already has children, we can just append the child element to the last child.
            if (pParentElement->lastChild != nullptr)
            {
                // The parent has children.
                assert(pParentElement->firstChild != nullptr);
                this->AppendSiblingElement(pParentElement->lastChild, pChildElement);
            }
            else
            {
                // The parent does not have children.
                assert(pParentElement->firstChild == nullptr);


                // The child element needs to be detached from it's current siblings and parent.
                this->DetachElementFromParentAndSiblings(pChildElement);


                // Because the parent did not have children earlier, the new child element will be both the first and last child.
                pParentElement->firstChild = pChildElement;
                pParentElement->lastChild  = pChildElement;

                // Make sure the child knows who it's new parent is.
                pChildElement->parent = pParentElement;


                // Invalidate the applicable layouts.
                if (GUIElementStyle_Get_IsAnyWidthOfType(pParentElement->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pParentElement, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pParentElement->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pParentElement, LayoutFlag_HeightInvalid);
                }

                //TODO: Only invalidate the properties that will have changed.
                this->Layout_InvalidateElementLayout(pChildElement, LayoutFlag_SizeAndPositionInvalid);
            }


            // If the child was on a different surface to the parent, it needs to be moved.
            if (pParentElement->hSurface != pChildElement->hSurface)
            {
                this->SetElementSurfaceRecursive(pChildElement, pParentElement->pSurface);
            }
        }
        this->EndBatch();
    }

    void GUIContextBase::PrependChildElement(GUIElement* pParentElement, GUIElement* pChildElement)
    {
        assert(pParentElement != nullptr);
        assert(pChildElement  != nullptr);


        this->BeginBatch();
        {
            // If the parent already has children, we can just append the child element to the last child.
            if (pParentElement->firstChild != 0)
            {
                // The parent has children.
                assert(pParentElement->lastChild != nullptr);
                this->PrependSiblingElement(pParentElement->firstChild, pChildElement);
            }
            else
            {
                // The parent does not have children.
                assert(pParentElement->lastChild == nullptr);


                // The child element needs to be detached from it's current siblings and parent.
                this->DetachElementFromParentAndSiblings(pChildElement);


                // Because the parent did not have children earlier, the new child element will be both the first and last child.
                pParentElement->firstChild = pChildElement;
                pParentElement->lastChild  = pChildElement;

                // Make sure the child knows who it's new parent is.
                pChildElement->parent = pParentElement;


                // Invalidate the applicable layouts.
                if (GUIElementStyle_Get_IsAnyWidthOfType(pParentElement->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pParentElement, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pParentElement->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pParentElement, LayoutFlag_HeightInvalid);
                }

                // Invalidate the applicable layouts. TODO: Only invalidate the properties that will have changed.
                this->Layout_InvalidateElementLayout(pChildElement, LayoutFlag_SizeAndPositionInvalid);
            }


            // If the child was on a different surface to the parent, it needs to be moved.
            if (pParentElement->hSurface != pChildElement->hSurface)
            {
                this->SetElementSurfaceRecursive(pChildElement, pParentElement->hSurface);
            }
        }
        this->EndBatch();
    }

    void GUIContextBase::AppendSiblingElement(GUIElement* pElementToAppendTo, GUIElement* pElementToAppend)
    {
        assert(pElementToAppendTo != nullptr);
        assert(pElementToAppend   != nullptr);


        this->BeginBatch();
        {
            // We want to detach the element from it's current siblings and parent first.
            this->DetachElementFromParentAndSiblings(pElementToAppend);


            // The new parent needs to be that of the sibling that's being appended to.
            pElementToAppend->parent = pElementToAppendTo->parent;


            // The parent has been set, so now it's time to link to it's new siblings.
            GUIElement* pElementToPrependTo = pElementToAppendTo->nextSibling;

            pElementToAppend->prevSibling = pElementToAppendTo;
            pElementToAppendTo->nextSibling = pElementToAppend;
        
            if (pElementToPrependTo != nullptr)
            {
                pElementToAppend->nextSibling = pElementToPrependTo;
                pElementToPrependTo->prevSibling = pElementToAppend;
            }
            else
            {
                // The new element is the last child of the parent.
                if (pElementToAppend->parent != nullptr)
                {
                    pElementToAppend->parent->lastChild = pElementToAppend;
                }
            }


            // Invalidate the applicable layouts.
            if (pElementToAppend->parent != nullptr)
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(pElementToAppend->parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pElementToAppend->parent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pElementToAppend->parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pElementToAppend->parent, LayoutFlag_HeightInvalid);
                }
            }

            //TODO: Only invalidate the properties that will have changed.
            this->Layout_InvalidateElementLayout(pElementToAppendTo, LayoutFlag_SizeAndPositionInvalid);
            this->Layout_InvalidateElementLayout(pElementToAppend,   LayoutFlag_SizeAndPositionInvalid);


            // If the elements are on different surfaces the appendee needs to be moved.
            if (pElementToAppendTo->hSurface != pElementToAppend->hSurface)
            {
                this->SetElementSurfaceRecursive(pElementToAppend, pElementToAppendTo->hSurface);
            }
        }
        this->EndBatch();
    }

    void GUIContextBase::PrependSiblingElement(GUIElement* pElementToPrependTo, GUIElement* pElementToPrepend)
    {
        assert(pElementToPrependTo != nullptr);
        assert(pElementToPrepend   != nullptr);


        this->BeginBatch();
        {
            // We want to detach the element from it's current siblings and parent first.
            this->DetachElementFromParentAndSiblings(pElementToPrepend);


            // The new parent needs to be that of the siblings that's being prepended to.
            pElementToPrepend->parent = pElementToPrependTo->parent;


            // The parent has been set, so now it's time to link to it's new sibling.
            GUIElement* pElementToAppendTo = pElementToPrependTo->prevSibling;

            pElementToPrepend->nextSibling = pElementToPrependTo;
            pElementToPrependTo->prevSibling = pElementToPrepend;

            if (pElementToAppendTo != nullptr)
            {
                pElementToPrepend->prevSibling = pElementToAppendTo;
                pElementToAppendTo->nextSibling = pElementToPrepend;
            }
            else
            {
                // The new element is the first child of the parent.
                if (pElementToPrepend->parent != nullptr)
                {
                    pElementToPrepend->parent->firstChild = elementToPrepend.handle;
                }
            }


            // Invalidate the applicable layouts.
            if (pElementToPrepend->parent != nullptr)
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(pElementToPrepend->parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pElementToPrepend->parent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pElementToPrepend->parent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pElementToPrepend->parent, LayoutFlag_HeightInvalid);
                }
            }

            //TODO: Only invalidate the properties that will have changed.
            this->Layout_InvalidateElementLayout(pElementToPrependTo, LayoutFlag_SizeAndPositionInvalid);
            this->Layout_InvalidateElementLayout(pElementToPrepend,   LayoutFlag_SizeAndPositionInvalid);


            // If the elements are on different surfaces the appendee needs to be moved.
            if (pElementToPrependTo->hSurface != pElementToPrepend->hSurface)
            {
                this->SetElementSurfaceRecursive(pElementToPrepend, pElementToPrependTo->hSurface);
            }
        }
        this->EndBatch();
    }

    GUIElement* GUIContextBase::GetElementNextSibling(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->nextSibling;
    }

    GUIElement* GUIContextBase::GetElementPreviousSibling(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->prevSibling;
    }


    bool GUIContextBase::IterateElementPrevSiblings(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        GUIElement* pPrevSibling = this->GetElementPreviousSibling(pElement);
        if (pPrevSibling != nullptr)
        {
            if (this->IterateElementPrevSiblings(pPrevSibling, handler))
            {
                return handler(pPrevSibling);
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    bool GUIContextBase::IterateElementNextSiblings(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        // NOTE: This can be iterative which might be more efficient. Try switching if this shows up in profiling. Using recursion here
        //       for consistency with InterateGUIElementPrevSiblings().
        GUIElement* pNextSibling = this->GetElementNextSibling(pElement);
        if (pNextSibling != nullptr)
        {
            if (handler(pNextSibling))
            {
                return this->IterateElementNextSiblings(pNextSibling, handler);
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    bool GUIContextBase::IterateElementSiblingsAndSelf(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        // Prev siblings.
        if (this->IterateElementPrevSiblings(pElement, handler))
        {
            // Self
            if (handler(pElement))
            {
                // Next siblings.
                return this->IterateElementNextSiblings(pElement, handler);
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

    bool GUIContextBase::IterateElementChildren(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        assert(pElement != nullptr);

        for (auto pChild = pElement->firstChild; pChild != nullptr; pChild = pChild->nextSibling)
        {
            if (!handler(pChild))
            {
                return false;
            }
        }

        return true;
    }

    bool GUIContextBase::IterateElementDescendants(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        assert(pElement != nullptr);

        return this->IterateElementChildren(pElement, [&](GUIElement* pChildElement) -> bool 
        {
            if (handler(pChildElement))
            {
                return this->IterateElementDescendants(pChildElement, [&](GUIElement* pChildElementInner) -> bool { return handler(pChildElementInner); });
            }
            else
            {
                return false;
            }
        });
    }

    bool GUIContextBase::IterateElementAncestors(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        assert(pElement != nullptr);

        auto pParent = this->GetElementParent(pElement);
        if (pParent != nullptr)
        {
            if (!handler(pParent))
            {
                return false;
            }
        }

        return true;
    }


    bool GUIContextBase::IsElementChild(GUIElement* pParentElement, GUIElement* pChildElement) const
    {
        assert(pParentElement != nullptr);

        bool result = false;
        this->IterateElementChildren(pParentElement, [&](GUIElement* pNextChildElement) -> bool 
        {
            if (pNextChildElement == pChildElement)
            {
                result = true;
                return false;
            }

            return true;
        });

        return result;
    }

    bool GUIContextBase::IsElementParent(GUIElement* pChildElement, GUIElement* pParentElement) const
    {
        assert(pChildElement != nullptr);

        return this->GetElementParent(pChildElement) == pParentElement;
    }

    bool GUIContextBase::IsElementDescendant(GUIElement* pParentElement, GUIElement* pDescendantElement) const
    {
        assert(pParentElement != nullptr);

        bool result = false;
        this->IterateElementDescendants(pParentElement, [&](GUIElement* pNextDescendantElement) -> bool 
        {
            if (pDescendantElement == pNextDescendantElement)
            {
                result = true;
                return false;
            }

            return true;
        });

        return result;
    }

    bool GUIContextBase::IsElementAncestor(GUIElement* pChildElement, GUIElement* pAncestorElement) const
    {
        assert(pChildElement != nullptr);

        bool result = false;
        this->IterateElementAncestors(pChildElement, [&](GUIElement* pNextAncestorElement) -> bool 
        {
            if (pAncestorElement == pNextAncestorElement)
            {
                result = true;
                return false;
            }

            return true;
        });

        return result;
    }


    void GUIContextBase::SetElementPosition(GUIElement* pElement, float xPos, float yPos)
    {
        assert(pElement != nullptr);

        // When this API is used, we assume it's relative positioning, relative to the top/left corner of the parent.
        GUIElementStyle_Set_positioning(pElement->style, Positioning_Relative);
        GUIElementStyle_Set_lefthaspriority(pElement->style, true);
        GUIElementStyle_Set_tophaspriority( pElement->style, true);
        GUIElementStyle_Set_left(pElement->style, static_cast<int32_t>(xPos), NumberType_Points);
        GUIElementStyle_Set_top( pElement->style, static_cast<int32_t>(yPos), NumberType_Points);

        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementPosition(GUIElement* pElement, int xPos, int yPos)
    {
        this->SetElementPosition(pElement, static_cast<float>(xPos), static_cast<float>(yPos));
    }

    void GUIContextBase::SetElementLeftPosition(GUIElement* pElement, int left)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_left(pElement->style, left, NumberType_Points);
        GUIElementStyle_Set_lefthaspriority(pElement->style, true);

        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementTopPosition(GUIElement* pElement, int top)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_top(pElement->style, top, NumberType_Points);
        GUIElementStyle_Set_tophaspriority(pElement->style, true);

        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementRightPosition(GUIElement* pElement, int right)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_right(pElement->style, right, NumberType_Points);
        GUIElementStyle_Set_righthaspriority(pElement->style, true);

        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBottomPosition(GUIElement* pElement, int bottom)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_bottom(pElement->style, bottom, NumberType_Points);
        GUIElementStyle_Set_bottomhaspriority(pElement->style, true);

        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }


    void GUIContextBase::SetElementRelativePositionOrigin(GUIElement* pElement, PositionOrigin origin)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_positionorigin(pElement->style, origin);

        if (GUIElementStyle_Get_positioning(pElement->style) == Positioning_Relative)
        {
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
            }
            this->EndBatch();
        }
    }

    PositionOrigin GUIContextBase::GetElementRelativePositionOrigin(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_positionorigin(pElement->style);
    }


    void GUIContextBase::GetElementPosition(GUIElement* pElement, float &xPosOut, float &yPosOut) const
    {
        assert(pElement != nullptr);

        xPosOut = pElement->layout.relativePosX;
        yPosOut = pElement->layout.relativePosY;
    }

    void GUIContextBase::GetElementPosition(GUIElement* pElement, int &xPosOut, int &yPosOut) const
    {
        assert(pElement != nullptr);

        xPosOut = static_cast<int>(pElement->layout.relativePosX);
        yPosOut = static_cast<int>(pElement->layout.relativePosY);
    }

    void GUIContextBase::GetElementAbsolutePosition(GUIElement* pElement, float &xPosOut, float &yPosOut) const
    {
        assert(pElement != nullptr);

        xPosOut = pElement->layout.absolutePosX;
        yPosOut = pElement->layout.absolutePosY;
    }

    void GUIContextBase::GetElementAbsolutePosition(GUIElement* pElement, int &xPosOut, int &yPosOut) const
    {
        assert(pElement != nullptr);

        xPosOut = static_cast<int>(pElement->layout.absolutePosX);
        yPosOut = static_cast<int>(pElement->layout.absolutePosY);
    }

    void GUIContextBase::SetElementSize(GUIElement* pElement, float width, float height)
    {
        this->Layout_SetElementSize(pElement, static_cast<uint32_t>(width), NumberType_Points, static_cast<uint32_t>(height), NumberType_Points);
    }
    void GUIContextBase::SetElementSize(GUIElement* pElement, unsigned int width, unsigned int height)
    {
        this->Layout_SetElementSize(pElement, static_cast<uint32_t>(width), NumberType_Points, static_cast<uint32_t>(height), NumberType_Points);
    }
    void GUIContextBase::SetElementSizeRatio(GUIElement* pElement, float widthRatio, float heightRatio)
    {
        this->Layout_SetElementSize(pElement, static_cast<uint32_t>(widthRatio * 100), NumberType_Percent, static_cast<uint32_t>(heightRatio * 100), NumberType_Percent);
    }
    void GUIContextBase::SetElementSizeToChildren(GUIElement* pElement)
    {
        this->Layout_SetElementSize(pElement, 0U, NumberType_Auto, 0U, NumberType_Auto);
    }
}