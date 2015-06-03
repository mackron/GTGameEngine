// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUIContextBase.hpp>
#include <GTGameEngine/Core/GUI/GUIFontManager_GDI.hpp>
#include <GTGameEngine/Core/GUI/GUISimpleTextLayout.hpp>

namespace GT
{
    GUIContextBase::GUIContextBase()
        : m_pFontManager(),
          m_xBaseDPI(96), m_yBaseDPI(96),
          m_layoutContext(),
          m_batchLockCounter(0)
    {
        // TODO: Delete this line later.
        m_pFontManager = new GUIFontManager_GDI;
    }

    GUIContextBase::~GUIContextBase()
    {
        // TODO: Delete this line later.
        delete m_pFontManager;
    }

    GUISurface* GUIContextBase::CreateSurface()
    {
        return this->CreateSurfacePtr();
    }

    void GUIContextBase::DeleteSurface(GUISurface* pSurface)
    {
        if (pSurface != nullptr)
        {
            // Detach any elements that are attached to this surface.
            while (pSurface->topLevelElements.GetCount() > 0)
            {
                auto pTopLevelElement = pSurface->topLevelElements[0];
                assert(pTopLevelElement != nullptr);
                {
                    GUIContextBase::DetachElementFromSurface(pTopLevelElement);
                }
            }

            this->DeleteSurfacePtr(pSurface);
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
                        this->Layout_InvalidateElementLayout(pElement, flags);
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

    bool GUIContextBase::IterateSurfaceElements(GUISurface* pSurface, std::function<bool (GUIElement* pElement)> handler)
    {
        assert(pSurface != nullptr);


        for (size_t iTopLevelElement = 0; iTopLevelElement < pSurface->topLevelElements.count; ++iTopLevelElement)
        {
            auto pTopLevelElement = pSurface->topLevelElements[iTopLevelElement];
            assert(pTopLevelElement != nullptr);
            {
                if (!handler(pTopLevelElement))
                {
                    return false;
                }

                if (!this->IterateElementDescendants(pTopLevelElement, [&](GUIElement* pChildElement) -> bool { return handler(pChildElement); }))
                {
                    return false;
                }
            }
        }

        return true;
    }



    ////////////////////////////////////////////////////////////////
    // Elements

    GUIElement* GUIContextBase::CreateElement()
    {
        auto pElement = this->CreateElementPtr();
        if (pElement != nullptr)
        {
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

            // Text colour.
            GUIElementStyle_Set_textcolor(pElement->style, GTLib::Colour(0.0f, 0.0f, 0.0f, 1.0f));
        }

        return pElement;
    }

    void GUIContextBase::DeleteElement(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        this->BeginBatch();
        {
            // The area consumed by the deleted element needs to be invalidated so that it will be redrawn.
            this->Painting_InvalidateElementRect(pElement);


            // We'll recursively delete the children first.
            while (pElement->pFirstChild != 0)
            {
                this->DeleteElement(pElement->pFirstChild);
            }


            // The element needs to be isolated before deleting.
            this->DetachElementFromParentAndSiblings(pElement);


            // If the element has an invalid layout, clear it from the layout manager.
            if (pElement->layout.layoutValidationListItem != nullptr)
            {
                m_layoutContext.invalidElements.Remove(pElement->layout.layoutValidationListItem);
            }

            // Now the element can actually be deleted.
            this->DeleteElementPtr(pElement);
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
                if (pParent->pFirstChild == pChildElement)
                {
                    assert(pChildElement->pPrevSibling == nullptr);
                    pParent->pFirstChild = pChildElement->pNextSibling;
                }

                if (pParent->pLastChild == pChildElement)
                {
                    assert(pChildElement->pNextSibling == nullptr);
                    pParent->pLastChild = pChildElement->pPrevSibling;
                }


                pChildElement->pParent = nullptr;


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
                GUIElement* pNeigboringAutoPositionedSibling = this->Layout_GetNeighboringAutoPositionedSibling(pChildElement);
                if (pNeigboringAutoPositionedSibling != nullptr)
                {
                    this->Layout_InvalidateElementLayout(pNeigboringAutoPositionedSibling, LayoutFlag_PositionInvalid);
                }
            }


            // The siblings can only be done after detaching from the parent.

            auto pPrevSibling = pChildElement->pPrevSibling;
            auto pNextSibling = pChildElement->pNextSibling;

            if (pPrevSibling != nullptr)
            {
                pPrevSibling->pNextSibling = pNextSibling;
            }

            if (pNextSibling != nullptr)
            {
                pNextSibling->pPrevSibling = pPrevSibling;
            }

            pChildElement->pPrevSibling = nullptr;
            pChildElement->pNextSibling = nullptr;


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
            if (pParentElement->pLastChild != nullptr)
            {
                // The parent has children.
                assert(pParentElement->pFirstChild != nullptr);
                this->AppendSiblingElement(pParentElement->pLastChild, pChildElement);
            }
            else
            {
                // The parent does not have children.
                assert(pParentElement->pFirstChild == nullptr);


                // The child element needs to be detached from it's current siblings and parent.
                this->DetachElementFromParentAndSiblings(pChildElement);


                // Because the parent did not have children earlier, the new child element will be both the first and last child.
                pParentElement->pFirstChild = pChildElement;
                pParentElement->pLastChild  = pChildElement;

                // Make sure the child knows who it's new parent is.
                pChildElement->pParent = pParentElement;


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
            if (pParentElement->pSurface != pChildElement->pSurface)
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
            if (pParentElement->pFirstChild != nullptr)
            {
                // The parent has children.
                assert(pParentElement->pLastChild != nullptr);
                this->PrependSiblingElement(pParentElement->pFirstChild, pChildElement);
            }
            else
            {
                // The parent does not have children.
                assert(pParentElement->pLastChild == nullptr);


                // The child element needs to be detached from it's current siblings and parent.
                this->DetachElementFromParentAndSiblings(pChildElement);


                // Because the parent did not have children earlier, the new child element will be both the first and last child.
                pParentElement->pFirstChild = pChildElement;
                pParentElement->pLastChild  = pChildElement;

                // Make sure the child knows who it's new parent is.
                pChildElement->pParent = pParentElement;


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
            if (pParentElement->pSurface != pChildElement->pSurface)
            {
                this->SetElementSurfaceRecursive(pChildElement, pParentElement->pSurface);
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
            pElementToAppend->pParent = pElementToAppendTo->pParent;


            // The parent has been set, so now it's time to link to it's new siblings.
            GUIElement* pElementToPrependTo = pElementToAppendTo->pNextSibling;

            pElementToAppend->pPrevSibling = pElementToAppendTo;
            pElementToAppendTo->pNextSibling = pElementToAppend;
        
            if (pElementToPrependTo != nullptr)
            {
                pElementToAppend->pNextSibling = pElementToPrependTo;
                pElementToPrependTo->pPrevSibling = pElementToAppend;
            }
            else
            {
                // The new element is the last child of the parent.
                if (pElementToAppend->pParent != nullptr)
                {
                    pElementToAppend->pParent->pLastChild = pElementToAppend;
                }
            }


            // Invalidate the applicable layouts.
            if (pElementToAppend->pParent != nullptr)
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(pElementToAppend->pParent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pElementToAppend->pParent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pElementToAppend->pParent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pElementToAppend->pParent, LayoutFlag_HeightInvalid);
                }
            }

            //TODO: Only invalidate the properties that will have changed.
            this->Layout_InvalidateElementLayout(pElementToAppendTo, LayoutFlag_SizeAndPositionInvalid);
            this->Layout_InvalidateElementLayout(pElementToAppend,   LayoutFlag_SizeAndPositionInvalid);


            // If the elements are on different surfaces the appendee needs to be moved.
            if (pElementToAppendTo->pSurface != pElementToAppend->pSurface)
            {
                this->SetElementSurfaceRecursive(pElementToAppend, pElementToAppendTo->pSurface);
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
            pElementToPrepend->pParent = pElementToPrependTo->pParent;


            // The parent has been set, so now it's time to link to it's new sibling.
            GUIElement* pElementToAppendTo = pElementToPrependTo->pPrevSibling;

            pElementToPrepend->pNextSibling = pElementToPrependTo;
            pElementToPrependTo->pPrevSibling = pElementToPrepend;

            if (pElementToAppendTo != nullptr)
            {
                pElementToPrepend->pPrevSibling = pElementToAppendTo;
                pElementToAppendTo->pNextSibling = pElementToPrepend;
            }
            else
            {
                // The new element is the first child of the parent.
                if (pElementToPrepend->pParent != nullptr)
                {
                    pElementToPrepend->pParent->pFirstChild = pElementToPrepend;
                }
            }


            // Invalidate the applicable layouts.
            if (pElementToPrepend->pParent != nullptr)
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(pElementToPrepend->pParent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pElementToPrepend->pParent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pElementToPrepend->pParent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pElementToPrepend->pParent, LayoutFlag_HeightInvalid);
                }
            }

            //TODO: Only invalidate the properties that will have changed.
            this->Layout_InvalidateElementLayout(pElementToPrependTo, LayoutFlag_SizeAndPositionInvalid);
            this->Layout_InvalidateElementLayout(pElementToPrepend,   LayoutFlag_SizeAndPositionInvalid);


            // If the elements are on different surfaces the appendee needs to be moved.
            if (pElementToPrependTo->pSurface != pElementToPrepend->pSurface)
            {
                this->SetElementSurfaceRecursive(pElementToPrepend, pElementToPrependTo->pSurface);
            }
        }
        this->EndBatch();
    }

    GUIElement* GUIContextBase::GetElementNextSibling(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->pNextSibling;
    }

    GUIElement* GUIContextBase::GetElementPreviousSibling(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->pPrevSibling;
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

    bool GUIContextBase::IterateElementPrevSiblingsReverse(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        GUIElement* pPrevSibling = this->GetElementPreviousSibling(pElement);
        if (pPrevSibling != nullptr)
        {
            if (handler(pPrevSibling))
            {
                return this->IterateElementPrevSiblingsReverse(pPrevSibling, handler);
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

    bool GUIContextBase::IterateElementNextSiblingsReverse(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        GUIElement* pNextSibling = this->GetElementNextSibling(pElement);
        if (pNextSibling != nullptr)
        {
            if (this->IterateElementNextSiblingsReverse(pNextSibling, handler))
            {
                return handler(pNextSibling);
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    bool GUIContextBase::IterateElementSiblings(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const
    {
        // Prev siblings.
        if (this->IterateElementPrevSiblings(pElement, handler))
        {
            // Next siblings.
            return this->IterateElementNextSiblings(pElement, handler);
        }
        else
        {
            return false;
        }
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

        for (auto pChild = pElement->pFirstChild; pChild != nullptr; pChild = pChild->pNextSibling)
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



    void GUIContextBase::SetElementWidth(GUIElement* pElement, float width)
    {
        this->Layout_SetElementWidth(pElement, static_cast<uint32_t>(width), NumberType_Points);
    }
    void GUIContextBase::SetElementWidth(GUIElement* pElement, unsigned int width)
    {
        this->Layout_SetElementWidth(pElement, static_cast<uint32_t>(width), NumberType_Points);
    }
    void GUIContextBase::SetElementWidthRatio(GUIElement* pElement, float widthRatio)
    {
        this->Layout_SetElementWidth(pElement, static_cast<uint32_t>(widthRatio * 100), NumberType_Percent);
    }
    void GUIContextBase::SetElementWidthToChildren(GUIElement* pElement)
    {
        this->Layout_SetElementWidth(pElement, 0U, NumberType_Auto);
    }

    void GUIContextBase::SetElementMinWidth(GUIElement* pElement, float minWidth)
    {
        this->Layout_SetElementMinWidth(pElement, static_cast<uint32_t>(minWidth), NumberType_Points);
    }
    void GUIContextBase::SetElementMinWidth(GUIElement* pElement, unsigned int minWidth)
    {
        this->Layout_SetElementMinWidth(pElement, static_cast<uint32_t>(minWidth), NumberType_Points);
    }
    void GUIContextBase::SetElementMinWidthRatio(GUIElement* pElement, float minWidthRatio)
    {
        this->Layout_SetElementMinWidth(pElement, static_cast<uint32_t>(minWidthRatio * 100), NumberType_Percent);
    }
    void GUIContextBase::SetElementMinWidthToChildren(GUIElement* pElement)
    {
        this->Layout_SetElementMinWidth(pElement, 0U, NumberType_Auto);
    }

    void GUIContextBase::SetElementMaxWidth(GUIElement* pElement, float maxWidth)
    {
        this->Layout_SetElementMaxWidth(pElement, static_cast<uint32_t>(maxWidth), NumberType_Points);
    }
    void GUIContextBase::SetElementMaxWidth(GUIElement* pElement, unsigned int maxWidth)
    {
        this->Layout_SetElementMaxWidth(pElement, static_cast<uint32_t>(maxWidth), NumberType_Points);
    }
    void GUIContextBase::SetElementMaxWidthRatio(GUIElement* pElement, float maxWidthRatio)
    {
        this->Layout_SetElementMaxWidth(pElement, static_cast<uint32_t>(maxWidthRatio * 100), NumberType_Percent);
    }
    void GUIContextBase::SetElementMaxWidthToChildren(GUIElement* pElement)
    {
        this->Layout_SetElementMaxWidth(pElement, 0U, NumberType_Auto);
    }


    void GUIContextBase::SetElementHeight(GUIElement* pElement, float height)
    {
        this->Layout_SetElementHeight(pElement, static_cast<uint32_t>(height), NumberType_Points);
    }
    void GUIContextBase::SetElementHeight(GUIElement* pElement, unsigned int height)
    {
        this->Layout_SetElementHeight(pElement, static_cast<uint32_t>(height), NumberType_Points);
    }
    void GUIContextBase::SetElementHeightRatio(GUIElement* pElement, float heightRatio)
    {
        this->Layout_SetElementHeight(pElement, static_cast<uint32_t>(heightRatio * 100), NumberType_Percent);
    }
    void GUIContextBase::SetElementHeightToChildren(GUIElement* pElement)
    {
        this->Layout_SetElementHeight(pElement, 0U, NumberType_Auto);
    }

    void GUIContextBase::SetElementMinHeight(GUIElement* pElement, float minHeight)
    {
        this->Layout_SetElementMinHeight(pElement, static_cast<uint32_t>(minHeight), NumberType_Points);
    }
    void GUIContextBase::SetElementMinHeight(GUIElement* pElement, unsigned int minHeight)
    {
        this->Layout_SetElementMinHeight(pElement, static_cast<uint32_t>(minHeight), NumberType_Points);
    }
    void GUIContextBase::SetElementMinHeightRatio(GUIElement* pElement, float minHeightRatio)
    {
        this->Layout_SetElementMinHeight(pElement, static_cast<uint32_t>(minHeightRatio * 100), NumberType_Percent);
    }
    void GUIContextBase::SetElementMinHeightToChildren(GUIElement* pElement)
    {
        this->Layout_SetElementMinHeight(pElement, 0U, NumberType_Auto);
    }

    void GUIContextBase::SetElementMaxHeight(GUIElement* pElement, float maxHeight)
    {
        this->Layout_SetElementMaxHeight(pElement, static_cast<uint32_t>(maxHeight), NumberType_Points);
    }
    void GUIContextBase::SetElementMaxHeight(GUIElement* pElement, unsigned int maxHeight)
    {
        this->Layout_SetElementMaxHeight(pElement, static_cast<uint32_t>(maxHeight), NumberType_Points);
    }
    void GUIContextBase::SetElementMaxHeightRatio(GUIElement* pElement, float maxHeightRatio)
    {
        this->Layout_SetElementMaxHeight(pElement, static_cast<uint32_t>(maxHeightRatio * 100), NumberType_Percent);
    }
    void GUIContextBase::SetElementMaxHeightToChildren(GUIElement* pElement)
    {
        this->Layout_SetElementMaxHeight(pElement, 0U, NumberType_Auto);
    }


    float GUIContextBase::GetElementWidth(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->layout.width;
    }

    float GUIContextBase::GetElementHeight(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->layout.height;
    }


    void GUIContextBase::GetElementAbsoluteRect(GUIElement* pElement, GTLib::Rect<float> &rectOut) const
    {
        assert(pElement != nullptr);

        rectOut.left   = pElement->layout.absolutePosX;
        rectOut.top    = pElement->layout.absolutePosY;
        rectOut.right  = rectOut.left + pElement->layout.width;
        rectOut.bottom = rectOut.top  + pElement->layout.height;
    }

    void GUIContextBase::GetElementAbsoluteRect(GUIElement* pElement, GTLib::Rect<int> &rectOut) const
    {
        assert(pElement != nullptr);

        rectOut.left   = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX));
        rectOut.top    = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY));
        rectOut.right  = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX + pElement->layout.width));
        rectOut.bottom = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY + pElement->layout.height));
    }


    void GUIContextBase::SetElementPositioning(GUIElement* pElement, Positioning positioning)
    {
        assert(pElement != nullptr);

        // If the current positioning is automatic, we need to invalidate the position of the neighbouring auto-positioned element.
        this->BeginBatch();
        {
            if (GUIElementStyle_Get_positioning(pElement->style) == Positioning_Auto)
            {
                auto pNeighboringAutoPositionedElement = this->Layout_GetNeighboringAutoPositionedSibling(pElement);
                if (pNeighboringAutoPositionedElement != nullptr)
                {
                    this->Layout_InvalidateElementLayout(pNeighboringAutoPositionedElement, LayoutFlag_PositionInvalid);
                }
            }

            GUIElementStyle_Set_positioning(pElement->style, positioning);

            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_SizeAndPositionInvalid);
        }
        this->EndBatch();
    }

    Positioning GUIContextBase::GetElementPositioning(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_positioning(pElement->style);
    }



    void GUIContextBase::SetElementBackgroundColour(GUIElement* pElement, const GTLib::Colour &colour)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_backgroundcolor(pElement->style, colour);

        this->BeginBatch();
        {
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    GTLib::Colour GUIContextBase::GetElementBackgroundColour(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_backgroundcolor(pElement->style);
    }



    void GUIContextBase::SetElementBorderLeftWidth(GUIElement* pElement, uint32_t width)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_borderleftwidth(pElement->style, width, NumberType_Points);

        pElement->layout.borderLeftWidth = static_cast<float>(width) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerWidthChange(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderTopWidth(GUIElement* pElement, uint32_t width)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_bordertopwidth(pElement->style, width, NumberType_Points);

        pElement->layout.borderTopWidth = static_cast<float>(width) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerHeightChange(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderRightWidth(GUIElement* pElement, uint32_t width)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_borderrightwidth(pElement->style, width, NumberType_Points);

        pElement->layout.borderRightWidth = static_cast<float>(width) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerWidthChange(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderBottomWidth(GUIElement* pElement, uint32_t width)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_borderbottomwidth(pElement->style, width, NumberType_Points);

        pElement->layout.borderBottomWidth = static_cast<float>(width) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerHeightChange(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderWidth(GUIElement* pElement, uint32_t width)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_borderwidth(pElement->style, width, NumberType_Points);

        pElement->layout.borderLeftWidth   = static_cast<float>(width) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.borderTopWidth    = static_cast<float>(width) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.borderRightWidth  = static_cast<float>(width) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.borderBottomWidth = static_cast<float>(width) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));

            
        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerSizeChange(pElement);
        }
        this->EndBatch();
    }


    void GUIContextBase::SetElementBorderLeftColor(GUIElement* pElement, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_borderleftcolor(pElement->style, borderColor);


        this->BeginBatch();
        {
            // TODO: Only invalidate the border rectangle instead of the entire element.
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderTopColor(GUIElement* pElement, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_bordertopcolor(pElement->style, borderColor);


        this->BeginBatch();
        {
            // TODO: Only invalidate the border rectangle instead of the entire element.
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderRightColor(GUIElement* pElement, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_borderrightcolor(pElement->style, borderColor);


        this->BeginBatch();
        {
            // TODO: Only invalidate the border rectangle instead of the entire element.
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderBottomColor(GUIElement* pElement, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_borderbottomcolor(pElement->style, borderColor);


        this->BeginBatch();
        {
            // TODO: Only invalidate the border rectangle instead of the entire element.
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderColor(GUIElement* pElement, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_bordercolor(pElement->style, borderColor);


        this->BeginBatch();
        {
            // TODO: Only invalidate the border rectangle instead of the entire element.
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }


    void GUIContextBase::SetElementBorderLeft(GUIElement* pElement, uint32_t width, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        this->BeginBatch();
        {
            this->SetElementBorderLeftWidth(pElement, width);
            this->SetElementBorderLeftColor(pElement, borderColor);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderTop(GUIElement* pElement, uint32_t width, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        this->BeginBatch();
        {
            this->SetElementBorderTopWidth(pElement, width);
            this->SetElementBorderTopColor(pElement, borderColor);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderRight(GUIElement* pElement, uint32_t width, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        this->BeginBatch();
        {
            this->SetElementBorderRightWidth(pElement, width);
            this->SetElementBorderRightColor(pElement, borderColor);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorderBottom(GUIElement* pElement, uint32_t width, const GTLib::Colour &borderColor)
    {
        assert(pElement != nullptr);

        this->BeginBatch();
        {
            this->SetElementBorderBottomWidth(pElement, width);
            this->SetElementBorderBottomColor(pElement, borderColor);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementBorder(GUIElement* pElement, uint32_t width, const GTLib::Colour &color)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_border(pElement->style, width, NumberType_Points, color);

        pElement->layout.borderLeftWidth   = static_cast<float>(width) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.borderTopWidth    = static_cast<float>(width) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.borderRightWidth  = static_cast<float>(width) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.borderBottomWidth = static_cast<float>(width) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));

            
        this->BeginBatch();
        {
            this->Painting_InvalidateElementRect(pElement);
            this->Layout_InvalidateElementLayoutsOnInnerSizeChange(pElement);
        }
        this->EndBatch();
    }



    void GUIContextBase::SetElementPaddingLeft(GUIElement* pElement, uint32_t padding)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_paddingleft(pElement->style, padding, NumberType_Points);

        pElement->layout.paddingLeft = static_cast<float>(padding) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerWidthChange(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementPaddingTop(GUIElement* pElement, uint32_t padding)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_paddingtop(pElement->style, padding, NumberType_Points);

        pElement->layout.paddingTop = static_cast<float>(padding) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerHeightChange(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementPaddingRight(GUIElement* pElement, uint32_t padding)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_paddingright(pElement->style, padding, NumberType_Points);

        pElement->layout.paddingRight = static_cast<float>(padding) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));


        this->EndBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerWidthChange(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementPaddingBottom(GUIElement* pElement, uint32_t padding)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_paddingbottom(pElement->style, padding, NumberType_Points);

        pElement->layout.paddingBottom = static_cast<float>(padding) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerHeightChange(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementPadding(GUIElement* pElement, uint32_t padding)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_padding(pElement->style, padding, NumberType_Points);

        pElement->layout.paddingLeft   = static_cast<float>(padding) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.paddingTop    = static_cast<float>(padding) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.paddingRight  = static_cast<float>(padding) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.paddingBottom = static_cast<float>(padding) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnInnerSizeChange(pElement);
        }
        this->EndBatch();
    }



    void GUIContextBase::SetElementMarginLeft(GUIElement* pElement, uint32_t margin)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_marginleft(pElement->style, margin, NumberType_Points);

        pElement->layout.marginLeft = static_cast<float>(margin) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));


        // The margin will change this element's position.
        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementMarginTop(GUIElement* pElement, uint32_t margin)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_margintop(pElement->style, margin, NumberType_Points);

        pElement->layout.marginTop = static_cast<float>(margin) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));


        // The margin will change this element's position.
        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementMarginRight(GUIElement* pElement, uint32_t margin)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_marginright(pElement->style, margin, NumberType_Points);

        pElement->layout.marginRight = static_cast<float>(margin) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));


        // The margin will change this element's position.
        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementMarginBottom(GUIElement* pElement, uint32_t margin)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_marginbottom(pElement->style, margin, NumberType_Points);

        pElement->layout.marginBottom = static_cast<float>(margin) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));


        // The margin will change this element's position.
        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::SetElementMargin(GUIElement* pElement, uint32_t margin)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_margin(pElement->style, margin, NumberType_Points);

        pElement->layout.marginLeft   = static_cast<float>(margin) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.marginTop    = static_cast<float>(margin) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.marginRight  = static_cast<float>(margin) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
        pElement->layout.marginBottom = static_cast<float>(margin) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));


        // The margin will change this element's position.
        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid);
        }
        this->EndBatch();
    }


    void GUIContextBase::SetElementChildAxis(GUIElement* pElement, ChildAxis axis)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_childaxis(pElement->style, axis);

        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnChildAxisChange(pElement);
        }
        this->EndBatch();
    }

    ChildAxis GUIContextBase::GetElementChildAxis(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_childaxis(pElement->style);
    }


    void GUIContextBase::SetElementHorizontalAlign(GUIElement* pElement, HorizontalAlign align)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_horizontalaling(pElement->style, align);

        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnHorizontalAlignChange(pElement);
        }
        this->EndBatch();
    }

    HorizontalAlign GUIContextBase::GetElementHorizontalAlign(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_horizontalalign(pElement->style);
    }

    void GUIContextBase::SetElementVerticalAlign(GUIElement* pElement, VerticalAlign align)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_verticalaling(pElement->style, align);

        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayoutsOnVerticalAlignChange(pElement);
        }
        this->EndBatch();
    }

    VerticalAlign GUIContextBase::GetElementVerticalAlign(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_verticalalign(pElement->style);
    }



    void GUIContextBase::SetElementChildrenWidthBoundary(GUIElement* pElement, ChildrenSizeBoundary boundary)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_childrenwidthboundary(pElement->style, boundary);


        // All children with relative widths need updating.
        this->BeginBatch();
        {
            this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(pChild->style, NumberType_Percent))
                {
                    this->Layout_InvalidateElementLayout(pChild, LayoutFlag_PositionInvalid | LayoutFlag_WidthInvalid);
                }

                return true;
            });
        }
        this->EndBatch();
    }

    ChildrenSizeBoundary GUIContextBase::GetElementChildrenWidthBoundary(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_childrenwidthboundary(pElement->style);
    }

    void GUIContextBase::SetElementChildrenHeightBoundary(GUIElement* pElement, ChildrenSizeBoundary boundary)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_childrenheightboundary(pElement->style, boundary);


        // All children with relative heights need updating.
        this->BeginBatch();
        {
            this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
            {
                if (GUIElementStyle_Get_IsAnyHeightOfType(pChild->style, NumberType_Percent))
                {
                    this->Layout_InvalidateElementLayout(pChild, LayoutFlag_PositionInvalid | LayoutFlag_HeightInvalid);
                }

                return true;
            });
        }
        this->EndBatch();
    }

    ChildrenSizeBoundary GUIContextBase::GetElementChildrenHeightBoundary(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_childrenheightboundary(pElement->style);
    }

    void GUIContextBase::SetElementChildrenSizeBoundary(GUIElement* pElement, ChildrenSizeBoundary boundary)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_childrenwidthboundary( pElement->style, boundary);
        GUIElementStyle_Set_childrenheightboundary(pElement->style, boundary);


        // All children with relative sizes need updating.
        this->BeginBatch();
        {
            this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
            {
                int invalidLayoutFlags = 0;
                if (GUIElementStyle_Get_IsAnyWidthOfType(pChild->style, NumberType_Percent))
                {
                    invalidLayoutFlags |= LayoutFlag_WidthInvalid;
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pChild->style, NumberType_Percent))
                {
                    invalidLayoutFlags |= LayoutFlag_HeightInvalid;
                }


                if (invalidLayoutFlags != 0)
                {
                    this->Layout_InvalidateElementLayout(pChild, LayoutFlag_PositionInvalid | invalidLayoutFlags);
                }

                return true;
            });
        }
        this->EndBatch();
    }


    void GUIContextBase::SetElementClippingBoundary(GUIElement* pElement, ClippingBoundary boundary)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_clippingboundary(pElement->style, boundary);

        this->BeginBatch();
        {
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    ClippingBoundary GUIContextBase::GetElementClippingBoundary(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_clippingboundary(pElement->style);
    }


    void GUIContextBase::SetElementClippingMode(GUIElement* pElement, ClippingMode clippingMode)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_clippingmode(pElement->style, clippingMode);

        this->BeginBatch();
        {
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    ClippingMode GUIContextBase::GetElementClippingMode(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_clippingmode(pElement->style);
    }


    void GUIContextBase::SetElementVisibility(GUIElement* pElement, bool visible)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_visible(pElement->style, visible);

        
        // TODO: A change in visibility will change the layout of other elements. Also need a redraw.
    }

    bool GUIContextBase::IsElementVisible(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        if (!GUIElementStyle_Get_visible(pElement->style))
        {
            return false;
        }

        auto pParent = this->GetElementParent(pElement);
        if (pParent != nullptr)
        {
            return this->IsElementVisible(pParent);
        }

        return true;
    }



    void GUIContextBase::EnableElementChildWidthFlexing(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_flexchildrenwidth(pElement->style, true);


        this->BeginBatch();
        {
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::DisableElementChildWidthFlexing(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_flexchildrenwidth(pElement->style, false);


        this->BeginBatch();
        {
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    bool GUIContextBase::IsElementChildWidthFlexingEnabled(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_flexchildrenwidth(pElement->style);
    }


    void GUIContextBase::EnableElementChildHeightFlexing(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_flexchildrenheight(pElement->style, true);


        this->BeginBatch();
        {
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    void GUIContextBase::DisableElementChildHeightFlexing(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_flexchildrenheight(pElement->style, false);


        this->BeginBatch();
        {
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    bool GUIContextBase::IsElementChildHeightFlexingEnabled(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_flexchildrenheight(pElement->style);
    }



    void GUIContextBase::SetElementText(GUIElement* pElement, const char* text)
    {
        assert(pElement != nullptr);

        if (m_pFontManager != nullptr)
        {
            this->UpdateTextLayout(pElement, text);


            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(pElement, LayoutFlag_TextInvalid);
            }
            this->EndBatch();
        }
    }

    const char* GUIContextBase::GetElementText(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        if (pElement->pTextLayout != nullptr)
        {
            return pElement->pTextLayout->GetText();
        }

        return nullptr;
    }

    bool GUIContextBase::DoesElementHaveText(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        if (pElement->pTextLayout != nullptr)
        {
            return GTLib::Strings::IsNullOrEmpty(pElement->pTextLayout->GetText()) == false;
        }

        return false;
    }

    HGUIFont GUIContextBase::SetElementFont(GUIElement* pElement, const char* family, FontWeight weight, FontSlant slant, uint32_t size, uint32_t sizeType)
    {
        assert(pElement != nullptr);
        assert(m_pFontManager != nullptr);

        auto pOldFont = this->GetElementFont(pElement);

        // 1) Update the styles.
        GUIElementStyle_Set_fontfamily(pElement->style, m_pFontManager->EncodeFontFamily(family));
        GUIElementStyle_Set_fontweight(pElement->style, weight);
        GUIElementStyle_Set_fontslant(pElement->style, slant);
        GUIElementStyle_Set_fontsize(pElement->style, size, sizeType);

        // 2) Update the font based on the current style.
        auto pNewFont = this->UpdateElementFontFromStyle(pElement);

        // 3) Update the layout, but only if the font is different.
        if (pNewFont != pOldFont)
        {
            this->BeginBatch();
            {
                this->Layout_InvalidateElementLayout(pElement, LayoutFlag_TextInvalid);
            }
            this->EndBatch();
        }


        return this->GetElementFont(pElement);
    }

    HGUIFont GUIContextBase::GetElementFont(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->hFont;
    }


    void GUIContextBase::SetElementTextColor(GUIElement* pElement, const GTLib::Colour &colour)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_textcolor(pElement->style, colour);

        this->BeginBatch();
        {
            // TODO: Only invalidate the text rectangle (previous and current).
            this->Painting_InvalidateElementRect(pElement);
        }
        this->EndBatch();
    }

    GTLib::Colour GUIContextBase::GetElementTextColor(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return GUIElementStyle_Get_textcolor(pElement->style);
    }



    bool GUIContextBase::AttachElementToSurface(GUIElement* pElement, GUISurface* pSurface)
    {
        assert(pElement != nullptr);

        if (pElement->pParent == nullptr)
        {
            if (pElement->pSurface != pSurface)
            {
                // The element must be detached from the old surface first.
                if (pElement->pSurface != 0)
                {
                    this->DetachElementFromSurface(pElement);
                }


                // The element needs to be set as a top-level element on the surface.
                if (pSurface != nullptr)
                {
                    pSurface->topLevelElements.PushBack(pElement);
                }


                // Now the surface must be set recursively on the children.
                this->SetElementSurfaceRecursive(pElement, pSurface);


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

    bool GUIContextBase::DetachElementFromSurface(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        if (pElement->pParent == nullptr)
        {
            auto pSurface = this->GetElementSurface(pElement);
            if (pSurface != nullptr)
            {
                // The element needs to be removed as a top-level element on the surface.
                if (pSurface != nullptr)
                {
                    pSurface->topLevelElements.RemoveFirstOccuranceOf(pElement);
                }


                // The element must be set recursively on the children.
                this->SetElementSurfaceRecursive(pElement, nullptr);


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

    GUISurface* GUIContextBase::GetElementSurface(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        return pElement->pSurface;
    }



    ////////////////////////////////////////////////////////////////
    // Batching

    void GUIContextBase::BeginBatch()
    {
        m_batchLockCounter += 1;
    }

    void GUIContextBase::EndBatch()
    {
        if (m_batchLockCounter > 0)
        {
            m_batchLockCounter -= 1;


            // If the counter is now at zero, validate the layouts.
            if (m_batchLockCounter == 0)
            {
                // Validate the layout.
                this->Layout_ValidateElementLayouts();

                // Validate the surface graphics.
                this->Painting_PaintAndValidateSurfaceRects();
            }
        }
    }



    ////////////////////////////////////////////////////////////////
    // Painting

    void GUIContextBase::InvalidateSurfaceRect(GUISurface* pSurface, const GTLib::Rect<int> &rect)
    {
        assert(pSurface != nullptr);

        this->BeginBatch();
        {
            this->Painting_InvalidateRect(pSurface, rect);
        }
        this->EndBatch();
    }

    void GUIContextBase::PaintSurface(GUISurface* pSurface, const GTLib::Rect<int> &rect, void* pInputData)
    {
        assert(pSurface != nullptr);

        this->Renderer_BeginPaintSurface(pSurface, pInputData);
        {
            this->Renderer_Clear(rect);

            for (size_t iElement = 0; iElement < pSurface->topLevelElements.GetCount(); ++iElement)
            {
                auto pTopLevelElement = pSurface->topLevelElements[iElement];
                if (pTopLevelElement != nullptr)
                {
                    this->ClippedTraversal(pTopLevelElement, rect, [&](GUIElement* pElementToPaint, const GTLib::Rect<int> &visibleRect) -> void
                    {
                        this->Painting_PaintElement(pSurface, pElementToPaint, visibleRect);
                    });
                }
            }
        }
        this->Renderer_EndPaintSurface();
    }

    void GUIContextBase::PaintSurface(GUISurface* pSurface, void* pInputData)
    {
        assert(pSurface != nullptr);

        unsigned int surfaceWidth;
        unsigned int surfaceHeight;
        this->GetSurfaceSize(pSurface, surfaceWidth, surfaceHeight);

        GTLib::Rect<int> paintRect;
        paintRect.left   = 0;
        paintRect.top    = 0;
        paintRect.right  = static_cast<int>(surfaceWidth);
        paintRect.bottom = static_cast<int>(surfaceHeight);
        this->PaintSurface(pSurface, paintRect, pInputData);
    }




    ////////////////////////////////////////////////////////////////
    // Inbound Events

    void GUIContextBase::OnMouseMove(GUISurface* pSurface, int mousePosX, int mousePosY)
    {
        auto pNewElementUnderMouse = this->FindElementUnderPoint(pSurface, mousePosX, mousePosY);
            
        // OnMouseLeave/OnMouseEnter
        if (pSurface->pElementUnderMouse != pNewElementUnderMouse)
        {
            auto pOldElementUnderMouse = pSurface->pElementUnderMouse;
            if (pOldElementUnderMouse != nullptr)
            {
                this->PostEvent_OnMouseLeave(pOldElementUnderMouse);
            }

            if (pNewElementUnderMouse != nullptr)
            {
                this->PostEvent_OnMouseEnter(pNewElementUnderMouse);
            }


            pSurface->pElementUnderMouse = pNewElementUnderMouse;
        }


        // OnMouseMove
        //
        // If an element is capturing mouse events, it needs to be the one to receive the event.
        GUIElement* pEventReceiver = nullptr;
        if (pSurface->pElementCapturingMouseEvents != nullptr)
        {
            pEventReceiver = pSurface->pElementCapturingMouseEvents;
        }
        else
        {
            pEventReceiver = pNewElementUnderMouse;
        }

        if (pEventReceiver != nullptr)
        {
            // Need to convert the point to local coordinates.
            int relativeMousePosX;
            int relativeMousePosY;
            this->AbsoluteToRelative(pEventReceiver, mousePosX, mousePosY, relativeMousePosX, relativeMousePosY);

            this->PostEvent_OnMouseMove(pEventReceiver, relativeMousePosX, relativeMousePosY);
        }
    }


    ////////////////////////////////////////////////////////////////
    // Searching/Gathering

    GUIElement* GUIContextBase::FindElementUnderPoint(GUISurface* pSurface, int x, int y)
    {
        assert(pSurface != nullptr);

        GUIElement* pElementUnderPoint = nullptr;


        GTLib::Rect<float> rect;
        rect.left   = 0;
        rect.top    = 0;
        rect.right  = static_cast<float>(pSurface->width);
        rect.bottom = static_cast<float>(pSurface->height);

        for (size_t iElement = 0; iElement < pSurface->topLevelElements.GetCount(); ++iElement)
        {
            auto pTopLevelElement = pSurface->topLevelElements[iElement];
            assert(pTopLevelElement != nullptr);
            {
                this->ClippedTraversal(pTopLevelElement, rect, [&](GUIElement* pElement, const GTLib::Rect<int> &visibleRect) -> void
                {
                    if (visibleRect.Contains(x, y))
                    {
                        pElementUnderPoint = pElement;
                    }
                });
            }
        }


        return pElementUnderPoint;
    }



    ////////////////////////////////////////////////////////////////
    // DPI / Scaling

    unsigned int GUIContextBase::GetBaseXDPI() const
    {
        return m_xBaseDPI;
    }

    unsigned int GUIContextBase::GetBaseYDPI() const
    {
        return m_yBaseDPI;
    }

    void GUIContextBase::GetBaseDPI(unsigned int &xBaseDPIOut, unsigned int &yBaseDPIOut) const
    {
        xBaseDPIOut = m_xBaseDPI;
        yBaseDPIOut = m_yBaseDPI;
    }

    void GUIContextBase::SetBaseDPI(unsigned int xBaseDPI, unsigned int yBaseDPI)
    {
        m_xBaseDPI = xBaseDPI;
        m_yBaseDPI = yBaseDPI;

        this->UpdateAllElementsOnDPIChange();
    }


    unsigned int GUIContextBase::GetXDPI(GUISurface* pSurface) const
    {
        if (pSurface != nullptr)
        {
            return pSurface->xDPI;
        }
        else
        {
            return this->GetBaseXDPI();
        }
    }

    unsigned int GUIContextBase::GetYDPI(GUISurface* pSurface) const
    {
        if (pSurface != nullptr)
        {
            return pSurface->xDPI;
        }
        else
        {
            return this->GetBaseYDPI();
        }
    }

    void GUIContextBase::GetDPI(GUISurface* pSurface, unsigned int &xDPIOut, unsigned int &yDPIOut)
    {
        assert(pSurface != nullptr);

        xDPIOut = pSurface->xDPI;
        yDPIOut = pSurface->yDPI;
    }

    void GUIContextBase::SetDPI(GUISurface* pSurface, unsigned int xDPI, unsigned int yDPI)
    {
        assert(pSurface != nullptr);

        pSurface->xDPI = xDPI;
        pSurface->yDPI = yDPI;

        this->UpdateAllElementsOnDPIChange(pSurface);
    }


    float GUIContextBase::GetXDPIScalingFactor(GUISurface* pSurface) const
    {
        return this->GetXDPI(pSurface) / static_cast<float>(m_xBaseDPI);
    }

    float GUIContextBase::GetYDPIScalingFactor(GUISurface* pSurface) const
    {
        return this->GetYDPI(pSurface) / static_cast<float>(m_yBaseDPI);
    }



    ////////////////////////////////////////////////////////////////
    // Utilities

    void GUIContextBase::AbsoluteToRelative(GUIElement* pElement, int xIn, int yIn, int &xOut, int &yOut) const
    {
        assert(pElement != nullptr);

        xOut = static_cast<int>(xIn - pElement->layout.absolutePosX);
        yOut = static_cast<int>(yIn - pElement->layout.absolutePosY);
    }



    ////////////////////////////////////////////////////////////////
    // Miscellaneous

    GUIFontManager* GUIContextBase::GetFontManager()
    {
        return m_pFontManager;
    }



    ////////////////////////////////////////////////////////////////
    // Private

    void GUIContextBase::SetElementSurfaceRecursive(GUIElement* pElement, GUISurface* pSurface)
    {
        assert(pElement != nullptr);

        pElement->pSurface = pSurface;

        // The layout may need to be updated.
        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_WidthInvalid | LayoutFlag_HeightInvalid | LayoutFlag_PositionInvalid);
        }
        this->EndBatch();


        // Now we update the children.
        this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
        {
            this->SetElementSurfaceRecursive(pChild, pSurface);
            return true;
        });
    }


    void GUIContextBase::GetElementChildrenClippingRect(GUIElement* pElement, GTLib::Rect<float> &rectOut) const
    {
        assert(pElement != nullptr);

        this->GetElementAbsoluteRect(pElement, rectOut);

        // Pull in based on the boundary.
        switch (GUIElementStyle_Get_clippingboundary(pElement->style))
        {
        case ClippingBoundary_Outer:
            {
                // Don't pull in at all.
                break;
            }

        case ClippingBoundary_InnerBorder:
            {
                // Pull in by just the border.
                rectOut.left   += pElement->layout.borderLeftWidth;
                rectOut.top    += pElement->layout.borderTopWidth;
                rectOut.right  -= pElement->layout.borderRightWidth;
                rectOut.bottom -= pElement->layout.borderBottomWidth;

                break;
            }

        case ClippingBoundary_Inner:
        default:
            {
                // Pull in by both the padding and the border.
                rectOut.left   += pElement->layout.borderLeftWidth   + pElement->layout.paddingLeft;
                rectOut.top    += pElement->layout.borderTopWidth    + pElement->layout.paddingTop;
                rectOut.right  -= pElement->layout.borderRightWidth  + pElement->layout.paddingRight;
                rectOut.bottom -= pElement->layout.borderBottomWidth + pElement->layout.paddingBottom;

                break;
            }
        }
    }

    void GUIContextBase::GetElementChildrenClippingRect(GUIElement* pElement, GTLib::Rect<int> &rectOut) const
    {
        assert(pElement != nullptr);

        GTLib::Rect<float> rectF;
        this->GetElementChildrenClippingRect(pElement, rectF);

        rectOut.left   = static_cast<int>(GTLib::Round(rectF.left));
        rectOut.top    = static_cast<int>(GTLib::Round(rectF.top));
        rectOut.right  = static_cast<int>(GTLib::Round(rectF.right));
        rectOut.bottom = static_cast<int>(GTLib::Round(rectF.bottom));
    }


    bool GUIContextBase::IsElementClippedAgainstParent(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        ClippingMode mode = GUIElementStyle_Get_clippingmode(pElement->style);
        if (mode == ClippingMode_Auto)
        {
            if (GUIElementStyle_Get_positioning(pElement->style) != Positioning_Absolute)
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

    void GUIContextBase::ClippedTraversal(GUIElement* pElement, const GTLib::Rect<float> &clippingRect, std::function<void (GUIElement*, const GTLib::Rect<int> &)> func)
    {
        assert(pElement != nullptr);

        GTLib::Rect<float> elementRect;
        this->GetElementAbsoluteRect(pElement, elementRect);

        // If clipping is disabled on the child we don't care about constraining the visible region - the entire rectangle will be classed as visible.
        GTLib::Rect<float> elementVisibleRect;
        if (this->IsElementClippedAgainstParent(pElement))
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
            GTLib::Rect<int> elementVisibleRectI;
            elementVisibleRectI.left   = static_cast<int>(GTLib::Round(elementVisibleRect.left));
            elementVisibleRectI.top    = static_cast<int>(GTLib::Round(elementVisibleRect.top));
            elementVisibleRectI.right  = static_cast<int>(GTLib::Round(elementVisibleRect.right));
            elementVisibleRectI.bottom = static_cast<int>(GTLib::Round(elementVisibleRect.bottom));
            func(pElement, elementVisibleRectI);


            // Now we need to recursively call this method again, only this time we need to pass in a new clipping rectangle. The rectangle will be determined
            // by the child clipping mode, which controls whether or not children are clipped against padding + border, only the border, or only the outside.
            //
            // To calculate this clipping rectangle, we start with the unclamped rectangle. We then pull it in based on our clipping boundary. Finally, we clamp
            // against the original clipping rectangle.
            GTLib::Rect<float> childClippingRect;
            this->GetElementChildrenClippingRect(pElement, childClippingRect);

            // Clamp against the original clipping rectangle.
            childClippingRect.Clamp(clippingRect);


            // We now need to traverse the children, but only if they themselves are at least partially visible.
            bool doesChildClippingRectangleHaveVolume = childClippingRect.left < childClippingRect.right && childClippingRect.top < childClippingRect.bottom;
            this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
            {
                if (!this->IsElementClippedAgainstParent(pChild) || doesChildClippingRectangleHaveVolume)
                {
                    this->ClippedTraversal(pChild, childClippingRect, func);
                }

                return true;
            });
        }
    }


    HGUIFont GUIContextBase::UpdateElementFontFromStyle(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        const char* family = m_pFontManager->DecodeFontFamily(GUIElementStyle_Get_fontfamily(pElement->style));
        FontWeight  weight = GUIElementStyle_Get_fontweight(pElement->style);
        FontSlant   slant  = GUIElementStyle_Get_fontslant(pElement->style);

        uint32_t sizeType;
        uint32_t size = GUIElementStyle_Get_fontsize(pElement->style, sizeType);


        // We now have enough information to generate a font from a font info structure.
        GUIFontInfo fi;
        fi.family = family;
        fi.dpiX = this->GetXDPI(this->GetElementSurface(pElement));
        fi.dpiY = this->GetYDPI(this->GetElementSurface(pElement));
        
        if (sizeType == NumberType_Points)
        {
            fi.sizeInPoints = static_cast<float>(size) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
            fi.sizeInPixels = 0;
        }
        else
        {
            fi.sizeInPoints = 0;
            fi.sizeInPixels = static_cast<float>(size) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
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


        HGUIFont hFont = m_pFontManager->AcquireFont(fi);
        if (hFont != 0)
        {
            pElement->hFont = hFont;
            this->UpdateTextLayout(pElement, this->GetElementText(pElement));

            return hFont;
        }

        return this->GetElementFont(pElement);
    }
    
    void GUIContextBase::UpdateTextLayout(GUIElement* pElement, const char* text)
    {
        assert(pElement != nullptr);

        if (m_pFontManager != nullptr)
        {
            if (text != nullptr)
            {
                // TODO: Compare the type of the current text layout and replace with a new one if the new type is different.

                if (pElement->pTextLayout == nullptr)
                {
                    pElement->pTextLayout = new GUISimpleTextLayout(*m_pFontManager);
                }


                GUISimpleTextLayout* pTextLayout = reinterpret_cast<GUISimpleTextLayout*>(pElement->pTextLayout);
                if (pTextLayout != nullptr)
                {
                    pTextLayout->SetTextAndFont(text, this->GetElementFont(pElement));
                    pTextLayout->SetColor(this->GetElementTextColor(pElement));
                }
            }
            else
            {
                // If the element does not have any text, delete the layout object to save memory.
                delete pElement->pTextLayout;
            }
        }
    }



    ////////////////////////////////////////////////////////////////
    // DPI / Scaling

    void GUIContextBase::UpdateAllElementsOnDPIChange(GUISurface* pSurface)
    {
        assert(pSurface != nullptr);

        this->BeginBatch();
        this->IterateSurfaceElements(pSurface, [&](GUIElement* pElement) -> bool
        {
            // Font.
            this->UpdateElementFontFromStyle(pElement);

            // Borders.
            this->UpdateElementBorderSizes(pElement);

            // Margins.
            this->UpdateElementMarginSizes(pElement);

            // Padding.
            this->UpdateElementPaddingSizes(pElement);

            // Layout.
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_PositionInvalid | LayoutFlag_SizeInvalid | LayoutFlag_TextInvalid);

            return true;
        });
        this->EndBatch();
    }

    void GUIContextBase::UpdateAllElementsOnDPIChange()
    {
        this->BeginBatch();
        this->IterateSurfaces([&](GUISurface* pSurface) -> bool
        {
            this->UpdateAllElementsOnDPIChange(pSurface);
            return true;
        });
        this->EndBatch();
    }

    void GUIContextBase::UpdateElementBorderSizes(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        uint32_t leftType;
        pElement->layout.borderLeftWidth   = static_cast<float>(GUIElementStyle_Get_borderleftwidth(pElement->style, leftType));

        uint32_t topType;
        pElement->layout.borderTopWidth    = static_cast<float>(GUIElementStyle_Get_bordertopwidth(pElement->style, topType));

        uint32_t rightType;
        pElement->layout.borderRightWidth  = static_cast<float>(GUIElementStyle_Get_borderrightwidth(pElement->style, rightType));

        uint32_t bottomType;
        pElement->layout.borderBottomWidth = static_cast<float>(GUIElementStyle_Get_borderbottomwidth(pElement->style, bottomType));


        if (leftType   == NumberType_Points) { pElement->layout.borderLeftWidth   *= this->GetXDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (topType    == NumberType_Points) { pElement->layout.borderTopWidth    *= this->GetYDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (rightType  == NumberType_Points) { pElement->layout.borderRightWidth  *= this->GetXDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (bottomType == NumberType_Points) { pElement->layout.borderBottomWidth *= this->GetYDPIScalingFactor(this->GetElementSurface(pElement)); }
    }

    void GUIContextBase::UpdateElementMarginSizes(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        uint32_t leftType;
        pElement->layout.marginLeft   = static_cast<float>(GUIElementStyle_Get_marginleft(pElement->style, leftType));

        uint32_t topType;
        pElement->layout.marginTop    = static_cast<float>(GUIElementStyle_Get_margintop(pElement->style, topType));

        uint32_t rightType;
        pElement->layout.marginRight  = static_cast<float>(GUIElementStyle_Get_marginright(pElement->style, rightType));

        uint32_t bottomType;
        pElement->layout.marginBottom = static_cast<float>(GUIElementStyle_Get_marginbottom(pElement->style, bottomType));


        if (leftType   == NumberType_Points) { pElement->layout.marginLeft   *= this->GetXDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (topType    == NumberType_Points) { pElement->layout.marginTop    *= this->GetYDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (rightType  == NumberType_Points) { pElement->layout.marginRight  *= this->GetXDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (bottomType == NumberType_Points) { pElement->layout.marginBottom *= this->GetYDPIScalingFactor(this->GetElementSurface(pElement)); }
    }

    void GUIContextBase::UpdateElementPaddingSizes(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        uint32_t leftType;
        pElement->layout.paddingLeft   = static_cast<float>(GUIElementStyle_Get_paddingleft(pElement->style, leftType));

        uint32_t topType;
        pElement->layout.paddingTop    = static_cast<float>(GUIElementStyle_Get_paddingtop(pElement->style, topType));

        uint32_t rightType;
        pElement->layout.paddingRight  = static_cast<float>(GUIElementStyle_Get_paddingright(pElement->style, rightType));

        uint32_t bottomType;
        pElement->layout.paddingBottom = static_cast<float>(GUIElementStyle_Get_paddingbottom(pElement->style, bottomType));


        if (leftType   == NumberType_Points) { pElement->layout.paddingLeft   *= this->GetXDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (topType    == NumberType_Points) { pElement->layout.paddingTop    *= this->GetYDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (rightType  == NumberType_Points) { pElement->layout.paddingRight  *= this->GetXDPIScalingFactor(this->GetElementSurface(pElement)); }
        if (bottomType == NumberType_Points) { pElement->layout.paddingBottom *= this->GetYDPIScalingFactor(this->GetElementSurface(pElement)); }
    }



    //////////////////////////////////////////////////
    // Painting

    void GUIContextBase::Painting_InvalidateRect(GUISurface* pSurface, const GTLib::Rect<int> &rect)
    {
        assert(pSurface != nullptr);

        if (rect.left > static_cast<int>(pSurface->width)  || rect.right  < 0 ||
            rect.top  > static_cast<int>(pSurface->height) || rect.bottom < 0)
        {
            return;
        }

        pSurface->invalidRect.left   = GTLib::Min(pSurface->invalidRect.left,   rect.left);
        pSurface->invalidRect.top    = GTLib::Min(pSurface->invalidRect.top,    rect.top);
        pSurface->invalidRect.right  = GTLib::Max(pSurface->invalidRect.right,  rect.right);
        pSurface->invalidRect.bottom = GTLib::Max(pSurface->invalidRect.bottom, rect.bottom);

        if (pSurface->invalidRect.left   < 0)                                  { pSurface->invalidRect.left   = 0; }
        if (pSurface->invalidRect.top    < 0)                                  { pSurface->invalidRect.top    = 0; }
        if (pSurface->invalidRect.right  > static_cast<int>(pSurface->width))  { pSurface->invalidRect.right  = static_cast<int>(pSurface->width);  }
        if (pSurface->invalidRect.bottom > static_cast<int>(pSurface->height)) { pSurface->invalidRect.bottom = static_cast<int>(pSurface->height); }
    }

    void GUIContextBase::Painting_InvalidateElementRect(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        auto pSurface = this->GetElementSurface(pElement);
        if (pSurface != nullptr)
        {
            GTLib::Rect<int> rect;
            rect.left   = static_cast<int>(pElement->layout.absolutePosX);
            rect.top    = static_cast<int>(pElement->layout.absolutePosY);
            rect.right  = static_cast<int>(pElement->layout.absolutePosX + pElement->layout.width);
            rect.bottom = static_cast<int>(pElement->layout.absolutePosY + pElement->layout.height);
            this->Painting_InvalidateRect(pSurface, rect);
        }
    }

    void GUIContextBase::Painting_PaintAndValidateSurfaceRects()
    {
        this->IterateSurfaces([&](GUISurface* pSurface) -> bool
        {
            if (pSurface->invalidRect.right > pSurface->invalidRect.left && pSurface->invalidRect.bottom > pSurface->invalidRect.top)
            {
                //this->PaintSurface(pSurface, pSurface->invalidRect);
                this->PostEvent_OnSurfaceNeedsRepaint(pSurface, pSurface->invalidRect);
                
                pSurface->invalidRect.left   = 0;
                pSurface->invalidRect.top    = 0;
                pSurface->invalidRect.right  = 0;
                pSurface->invalidRect.bottom = 0;
            }

            return true;
        });
    }

    void GUIContextBase::Painting_PaintElement(GUISurface* pSurface, GUIElement* pElement, const GTLib::Rect<int> &visibleRect)
    {
        assert(pSurface != nullptr);
        assert(pElement != nullptr);

        Painting_DrawAndSetClippingRect(pSurface, visibleRect, this->GetElementBackgroundColour(pElement));


        // Text.
        if (this->DoesElementHaveText(pElement))
        {
            HGUIFont hFont = this->GetElementFont(pElement);
            if (hFont != 0)
            {
                // Text needs to be clipped against the inner rectangle (the same rectangle children are clipped against).
                GTLib::Rect<int> childClippingRect;
                this->GetElementChildrenClippingRect(pElement, childClippingRect);

                // The text clipping rectangle needs to be clamped against the input clipping rectangle.
                childClippingRect.Clamp(visibleRect);
                Renderer_SetClippingRect(childClippingRect);


                if (Renderer_CanDrawText(hFont))
                {
                    pElement->pTextLayout->SetBounds(childClippingRect.GetWidth(), childClippingRect.GetHeight());  // <-- TODO: Update the boundary at the proper time - when the size has actually changed.

                    pElement->pTextLayout->IterateVisibleTextRuns([&](const GUITextRunDesc &run) {
                        GUITextRunDesc run2(run);
                        run2.xPos += static_cast<int>(pElement->layout.absolutePosX);
                        run2.yPos += static_cast<int>(pElement->layout.absolutePosY);

                        Renderer_DrawText(run2);
                    });
                }
                else
                {
                    // Draw the text the slow, generic way.
                }


                // The clipping rect needs to be reset.
                Renderer_SetClippingRect(visibleRect);
            }
        }

        

        // Borders. These should always be drawn last so that they're always on top.
        GTLib::Rect<int> borderLeftRect;
        borderLeftRect.left     = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX));
        borderLeftRect.top      = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY));
        borderLeftRect.right    = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX + pElement->layout.borderLeftWidth));
        borderLeftRect.bottom   = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY + pElement->layout.height));

        GTLib::Rect<int> borderTopRect;
        borderTopRect.left      = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX));
        borderTopRect.top       = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY));
        borderTopRect.right     = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX + pElement->layout.width));
        borderTopRect.bottom    = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY + pElement->layout.borderTopWidth));

        GTLib::Rect<int> borderRightRect;
        borderRightRect.left    = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX + pElement->layout.width - pElement->layout.borderRightWidth));
        borderRightRect.top     = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY));
        borderRightRect.right   = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX + pElement->layout.width));
        borderRightRect.bottom  = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY + pElement->layout.height));
        
        GTLib::Rect<int> borderBottomRect;
        borderBottomRect.left   = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX));
        borderBottomRect.top    = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY + pElement->layout.height - pElement->layout.borderBottomWidth));
        borderBottomRect.right  = static_cast<int>(GTLib::Round(pElement->layout.absolutePosX + pElement->layout.width));
        borderBottomRect.bottom = static_cast<int>(GTLib::Round(pElement->layout.absolutePosY + pElement->layout.height));

        // Remove the overlap.
        borderTopRect.right    -= borderRightRect.GetWidth();
        borderRightRect.bottom -= borderBottomRect.GetHeight();
        borderBottomRect.left  += borderLeftRect.GetWidth();
        borderLeftRect.top     += borderTopRect.GetHeight();

        Renderer_DrawRectangle(borderLeftRect,   GUIElementStyle_Get_borderleftcolor(pElement->style));
        Renderer_DrawRectangle(borderTopRect,    GUIElementStyle_Get_bordertopcolor(pElement->style));
        Renderer_DrawRectangle(borderRightRect,  GUIElementStyle_Get_borderrightcolor(pElement->style));
        Renderer_DrawRectangle(borderBottomRect, GUIElementStyle_Get_borderbottomcolor(pElement->style));


        // Text.
#if 0
        if (this->DoesElementHaveText(pElement->handle))
        {
            // We need to iterate over each text run 

            auto font = this->GetElementFont(pElement->handle);
            if (font != nullptr)
            {
                // Text needs to be clipped against the inner rectangle (the same rectangle children are clipped against).
                GTLib::Rect<int> childClippingRect;
                this->GetElementChildrenClippingRect(pElement, childClippingRect);

                // The text clipping rectangle needs to be clamped against the input clipping rectangle.
                childClippingRect.Clamp(visibleRect);

                // TODO: Optimize this so that per-character clipping is performed instead of per-pixel.
                m_renderer->SetClippingRect(childClippingRect);
                //Painting_SetClippingRect(surface, childClippingRect);


                unsigned int tabSizeInPixels = 16;      // <-- Temp. Change this based on the size of the space character.

                font->GetServer().GetFontEngine().MeasureString(font->GetFontHandle(), this->GetElementText(pElement->handle), tabSizeInPixels, [&](const GTLib::FontEngine &fontEngine, char32_t character, GTLib::GlyphHandle glyph, const GTLib::Rect<int> &rect, GTLib::GlyphMetrics &, int &, int &) -> bool
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
                                float textPositionX = pElement->layout.absolutePosX + pElement->layout.borderLeftWidth + pElement->layout.paddingLeft;
                                float textPositionY = pElement->layout.absolutePosY + pElement->layout.borderTopWidth  + pElement->layout.paddingTop;

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

    void GUIContextBase::Painting_DrawAndSetClippingRect(GUISurface* pSurface, const GTLib::Rect<int> &rect, const GTLib::Colour &color)
    {
        (void)pSurface;

        Renderer_SetClippingRect(rect);
        Renderer_DrawRectangle(rect, color);
    }



    //////////////////////////////////////////////////
    // Layout

    void GUIContextBase::Layout_SetElementSize(GUIElement* pElement, uint32_t width, uint32_t widthType, uint32_t height, uint32_t heightType)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_width( pElement->style, width,  widthType);
        GUIElementStyle_Set_height(pElement->style, height, heightType);


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_SizeInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::Layout_SetElementWidth(GUIElement* pElement, uint32_t width, uint32_t widthType)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_width(pElement->style, width,  widthType);


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_WidthInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::Layout_SetElementMinWidth(GUIElement* pElement, uint32_t minWidth, uint32_t widthType)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_minwidth(pElement->style, minWidth, widthType);


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_WidthInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::Layout_SetElementMaxWidth(GUIElement* pElement, uint32_t maxWidth, uint32_t widthType)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_maxwidth(pElement->style, maxWidth, widthType);


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_WidthInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::Layout_SetElementHeight(GUIElement* pElement, uint32_t height, uint32_t heightType)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_height(pElement->style, static_cast<uint32_t>(height), heightType);


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_HeightInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::Layout_SetElementMinHeight(GUIElement* pElement, uint32_t minHeight, uint32_t heightType)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_minheight(pElement->style, minHeight, heightType);


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_HeightInvalid);
        }
        this->EndBatch();
    }

    void GUIContextBase::Layout_SetElementMaxHeight(GUIElement* pElement, uint32_t maxHeight, uint32_t heightType)
    {
        assert(pElement != nullptr);

        GUIElementStyle_Set_maxheight(pElement->style, maxHeight, heightType);


        this->BeginBatch();
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_HeightInvalid);
        }
        this->EndBatch();
    }



    void GUIContextBase::Layout_InvalidateElementLayout(GUIElement* pElement, unsigned int flags)
    {
        assert(pElement != nullptr);

        if (flags != 0)
        {
            if (pElement->layout.flags == 0)
            {
                assert(pElement->layout.layoutValidationListItem == nullptr);
                pElement->layout.layoutValidationListItem = m_layoutContext.invalidElements.Append(pElement);

                // The pElement is amount to have it's layout changed which means we will want to invalidate the previous rectangle. The new
                // rectangle will be invalidated after validating the layout.
                this->Painting_InvalidateElementRect(pElement);
            }
        
            pElement->layout.flags |= flags;
        }
    }


    void GUIContextBase::Layout_InvalidateElementLayoutsOnInnerWidthChange(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        int invalidLayoutFlags = 0;

        uint32_t widthType;
        uint32_t minWidthType;
        uint32_t maxWidthType;
        GUIElementStyle_Get_allwidthtypes(pElement->style, widthType, minWidthType, maxWidthType);
        if (widthType == NumberType_Auto || minWidthType == NumberType_Auto || maxWidthType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }

        if (invalidLayoutFlags != 0)
        {
            this->Layout_InvalidateElementLayout(pElement, invalidLayoutFlags);
        }



        // The positions and size of children may have changed.
        int childrenInvalidLayoutFlags = LayoutFlag_PositionInvalid;
        if (GUIElementStyle_Get_childrenwidthboundary(pElement->style) != ChildrenSizeBoundary_Outer)
        {
            childrenInvalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }

        this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
        {
            this->Layout_InvalidateElementLayout(pChild, childrenInvalidLayoutFlags);
            return true;
        });
    }

    void GUIContextBase::Layout_InvalidateElementLayoutsOnInnerHeightChange(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        int invalidLayoutFlags = 0;

        uint32_t heightType;
        uint32_t minHeightType;
        uint32_t maxHeightType;
        GUIElementStyle_Get_allheighttypes(pElement->style, heightType, minHeightType, maxHeightType);
        if (heightType == NumberType_Auto || minHeightType == NumberType_Auto || maxHeightType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        if (invalidLayoutFlags != 0)
        {
            this->Layout_InvalidateElementLayout(pElement, invalidLayoutFlags);
        }



        // The positions of children may have changed.
        int childrenInvalidLayoutFlags = LayoutFlag_PositionInvalid;
        if (GUIElementStyle_Get_childrenheightboundary(pElement->style) != ChildrenSizeBoundary_Outer)
        {
            childrenInvalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
        {
            this->Layout_InvalidateElementLayout(pChild, childrenInvalidLayoutFlags);
            return true;
        });
    }

    void GUIContextBase::Layout_InvalidateElementLayoutsOnInnerSizeChange(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        int invalidLayoutFlags = 0;

        uint32_t widthType;
        uint32_t minWidthType;
        uint32_t maxWidthType;
        GUIElementStyle_Get_allwidthtypes(pElement->style, widthType, minWidthType, maxWidthType);
        if (widthType == NumberType_Auto || minWidthType == NumberType_Auto || maxWidthType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }

        uint32_t heightType;
        uint32_t minHeightType;
        uint32_t maxHeightType;
        GUIElementStyle_Get_allheighttypes(pElement->style, heightType, minHeightType, maxHeightType);
        if (heightType == NumberType_Auto || minHeightType == NumberType_Auto || maxHeightType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        if (invalidLayoutFlags != 0)
        {
            this->Layout_InvalidateElementLayout(pElement, invalidLayoutFlags);
        }



        // The positions of children may have changed.
        int childrenInvalidLayoutFlags = LayoutFlag_PositionInvalid;
        if (GUIElementStyle_Get_childrenwidthboundary(pElement->style) != ChildrenSizeBoundary_Outer)
        {
            childrenInvalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }
        if (GUIElementStyle_Get_childrenheightboundary(pElement->style) != ChildrenSizeBoundary_Outer)
        {
            childrenInvalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
        {
            this->Layout_InvalidateElementLayout(pChild, childrenInvalidLayoutFlags);
            return true;
        });
    }


    void GUIContextBase::Layout_InvalidateElementLayoutsOnChildAxisChange(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        int invalidLayoutFlags = 0;

        uint32_t widthType;
        uint32_t minWidthType;
        uint32_t maxWidthType;
        GUIElementStyle_Get_allwidthtypes(pElement->style, widthType, minWidthType, maxWidthType);
        if (widthType == NumberType_Auto || minWidthType == NumberType_Auto || maxWidthType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_WidthInvalid;
        }

        uint32_t heightType;
        uint32_t minHeightType;
        uint32_t maxHeightType;
        GUIElementStyle_Get_allheighttypes(pElement->style, heightType, minHeightType, maxHeightType);
        if (heightType == NumberType_Auto || minHeightType == NumberType_Auto || maxHeightType == NumberType_Auto)
        {
            invalidLayoutFlags |= LayoutFlag_HeightInvalid;
        }

        


        // The positions of children may have changed.
        int childrenInvalidLayoutFlags = LayoutFlag_PositionInvalid;

        this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
        {
            this->Layout_InvalidateElementLayout(pChild, childrenInvalidLayoutFlags);
            return true;
        });


        if (invalidLayoutFlags != 0)
        {
            this->Layout_InvalidateElementLayout(pElement, invalidLayoutFlags);
        }
    }


    void GUIContextBase::Layout_InvalidateElementLayoutsOnHorizontalAlignChange(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        // Auto-positioned pElements need to be repositioned. We can do this by just invalidating the first auto-positioned pElement->
        auto pFirstAutoPositionedChild = this->Layout_GetFirstAutoPositionedChild(pElement);
        if (pFirstAutoPositionedChild != nullptr)
        {
            this->Layout_InvalidateElementLayout(pFirstAutoPositionedChild, LayoutFlag_PositionInvalid);
        }
    }

    void GUIContextBase::Layout_InvalidateElementLayoutsOnVerticalAlignChange(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        // Auto-positioned pElements need to be repositioned. We can do this by just invalidating the first auto-positioned pElement->
        auto pFirstAutoPositionedChild = this->Layout_GetFirstAutoPositionedChild(pElement);
        if (pFirstAutoPositionedChild != nullptr)
        {
            this->Layout_InvalidateElementLayout(pFirstAutoPositionedChild, LayoutFlag_PositionInvalid);
        }
    }



    void GUIContextBase::Layout_ValidateElementLayouts()
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
                    auto pElement = iElement->value;
                    assert(pElement != nullptr);
                    {
                        this->Layout_ValidateElementLayout(pElement);
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

    void GUIContextBase::Layout_ValidateElementLayout(GUIElement* pElement)
    {
        assert(pElement != nullptr);

#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.BeginLine();
        m_layoutLogger.Print(GTLib::String::CreateFormatted("Validating Element: %d\n", pElement->handle).c_str());
#endif


        if ((pElement->layout.flags & LayoutFlag_WidthInvalid) != 0)
        {
            this->Layout_ValidateElementWidth(pElement);
        }

        if ((pElement->layout.flags & LayoutFlag_HeightInvalid) != 0)
        {
            this->Layout_ValidateElementHeight(pElement);
        }

        if ((pElement->layout.flags & (LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid)) != 0)
        {
            this->Layout_ValidateElementPosition(pElement);
        }

        if ((pElement->layout.flags & LayoutFlag_TextInvalid) != 0)
        {
            this->Layout_ValidateElementText(pElement);
        }



        // The list item in the layout context needs to be removed.
        if (pElement->layout.flags == 0 && pElement->layout.layoutValidationListItem != nullptr)
        {
            m_layoutContext.invalidElements.Remove(pElement->layout.layoutValidationListItem);
            pElement->layout.layoutValidationListItem = nullptr;
        }


#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.EndLine();
#endif
    }

    void GUIContextBase::Layout_ValidateElementWidth(GUIElement* pElement)
    {
        assert(pElement != nullptr);

#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.BeginLine();
        m_layoutLogger.Print(GTLib::String::CreateFormatted("width: %d\n", pElement->handle).c_str());
#endif
        float oldOuterWidth = GUIContextBase::Layout_GetElementOuterWidth(pElement);
        float newOuterWidth = this->Layout_UpdateElementWidth(pElement);

        if (oldOuterWidth != newOuterWidth)
        {
            this->Layout_MarkElementSizeAsChanged(pElement);


            //////////////////////////////////////////////////////////////////////////
            // Dependencies
            auto pParent = this->GetElementParent(pElement);
            
            uint32_t parentWidthType = 0;
            if (pParent != nullptr)
            {
                GUIElementStyle_Get_width(pParent->style, parentWidthType);
            }


            // Widths
            //
            // Any %-sized children will have changed.
            this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
            {
                uint32_t childWidthType;
                uint32_t childMinWidthType;
                uint32_t childMaxWidthType;
                GUIElementStyle_Get_allwidthtypes(pChild->style, childWidthType, childMinWidthType, childMaxWidthType);

                if (childWidthType == NumberType_Percent || childMinWidthType == NumberType_Percent || childMaxWidthType == NumberType_Percent)
                {
                    this->Layout_InvalidateElementLayout(pChild, LayoutFlag_WidthInvalid);
                }

                return true;
            });


            // Flexed siblings will need to have their widths updated.
            if (pParent != nullptr && GUIElementStyle_Get_flexchildrenwidth(pParent->style) && GUIElementStyle_Get_childaxis(pParent->style) == ChildAxis_Horizontal)
            {
                this->IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
                {
                    if (!GUIContextBase::Layout_IsElementWidthInvalid(pSibling))
                    {
                        this->Layout_InvalidateElementLayout(pSibling, LayoutFlag_WidthInvalid);
                    }

                    return true;
                });
            }

            // If the parent is auto-sized, it will also have it's width changed.
            if (pParent != nullptr && parentWidthType == NumberType_Auto)
            {
                uint32_t widthType;
                GUIElementStyle_Get_width(pElement->style, widthType);

                if (widthType != NumberType_Percent)
                {
                    this->Layout_InvalidateElementLayout(pParent, LayoutFlag_WidthInvalid);
                }
            }



            // Positions
            //
            // The position of this element will have changed if we're not aligned left.
            if ((pParent != nullptr && GUIElementStyle_Get_horizontalalign(pParent->style) != HorizontalAlign_Left) || (GUIElementStyle_Get_positioning(pElement->style) == Positioning_Relative && GUIElementStyle_Get_righthaspriority(pElement->style)))
            {
                this->Layout_InvalidateElementLayout(pElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
            }

            // Children might have positions changed depending on alignment.
            if (GUIElementStyle_Get_horizontalalign(pElement->style) != HorizontalAlign_Left)
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    this->Layout_InvalidateElementLayout(pChild, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                    return true;
                });
            }
#if 0
            else
            {
                // TODO: What is the point in this branch? Only repositioning relative and absolute children and only if the alignment is left? Why? What scenario requires this?

                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_positioning(pChild->style) != Positioning_Auto)
                    {
                        this->Layout_InvalidateElementLayout(pChild, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                    }

                    return true;
                });
            }
#endif

            // Siblings might have positions changed depending on the parent's alignment.
            if (pParent != nullptr)
            {
                switch (GUIElementStyle_Get_horizontalalign(pParent->style))
                {
                case HorizontalAlign_Right:
                    {
                        // All auto-positioned siblings before this element.
                        this->IterateElementPrevSiblings(pElement, [&](GUIElement* pSibling) -> bool
                        {
                            if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                            {
                                this->Layout_InvalidateElementLayout(pSibling, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                            }

                            return true;
                        });

                        break;
                    }

                case HorizontalAlign_Center:
                    {
                        // All auto-positioned siblings, including this element.
                        this->IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
                        {
                            if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                            {
                                this->Layout_InvalidateElementLayout(pSibling, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                            }

                            return true;
                        });

                        break;
                    }

                case HorizontalAlign_Left:
                default:
                    {
                        // All auto-positioned siblings after this element.
                        this->IterateElementNextSiblings(pElement, [&](GUIElement* pSibling) -> bool
                        {
                            if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                            {
                                this->Layout_InvalidateElementLayout(pSibling, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                            }

                            return true;
                        });

                        break;
                    }
                }
            }
        }


        // Now we need to remove ourselves from the invalid list.
        this->Layout_MarkElementWidthAsValid(pElement);

#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.EndLine();
#endif
    }

    void GUIContextBase::Layout_ValidateElementHeight(GUIElement* pElement)
    {
        assert(pElement != nullptr);

#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.BeginLine();
        m_layoutLogger.Print(GTLib::String::CreateFormatted("height: %d\n", pElement->handle).c_str());
#endif
        float oldOuterHeight = GUIContextBase::Layout_GetElementOuterHeight(pElement);
        float newOuterHeight = this->Layout_UpdateElementHeight(pElement);

        if (oldOuterHeight != newOuterHeight)
        {
            this->Layout_MarkElementSizeAsChanged(pElement);


            //////////////////////////////////////////////////////////////////////////
            // Dependencies
            auto pParent = this->GetElementParent(pElement);

            uint32_t parentHeightType = 0;
            if (pParent != nullptr)
            {
                GUIElementStyle_Get_height(pParent->style, parentHeightType);
            }


            // Widths
            //
            // Any %-sized children will have changed.
            this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
            {
                uint32_t childHeightType;
                uint32_t childMinHeightType;
                uint32_t childMaxHeightType;
                GUIElementStyle_Get_allheighttypes(pChild->style, childHeightType, childMinHeightType, childMaxHeightType);

                if (childHeightType == NumberType_Percent || childMinHeightType == NumberType_Percent || childMaxHeightType == NumberType_Percent)
                {
                    this->Layout_InvalidateElementLayout(pChild, LayoutFlag_HeightInvalid);
                }

                return true;
            });

            // Flexed siblings will need to have their heights updated.
            if (pParent != nullptr && GUIElementStyle_Get_flexchildrenheight(pParent->style) && GUIElementStyle_Get_childaxis(pParent->style) == ChildAxis_Vertical)
            {
                this->IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
                {
                    if (!GUIContextBase::Layout_IsElementHeightInvalid(pSibling))
                    {
                        this->Layout_InvalidateElementLayout(pSibling, LayoutFlag_HeightInvalid);
                    }

                    return true;
                });
            }

            // If the parent is auto-sized, it will also have it's height changed.
            if (pParent != nullptr && parentHeightType == NumberType_Auto)
            {
                uint32_t heightType;
                GUIElementStyle_Get_height(pElement->style, heightType);

                if (heightType != NumberType_Percent)
                {
                    this->Layout_InvalidateElementLayout(pParent, LayoutFlag_HeightInvalid);
                }
            }



            // Positions
            //
            // The position of this pElement will have changed if we're not aligned top.
            if ((pParent != nullptr && GUIElementStyle_Get_verticalalign(pParent->style) != VerticalAlign_Top) || (GUIElementStyle_Get_positioning(pElement->style) == Positioning_Relative && GUIElementStyle_Get_bottomhaspriority(pElement->style)))
            {
                this->Layout_InvalidateElementLayout(pElement, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
            }

            // Children might have positions changed depending on alignment.
            if (GUIElementStyle_Get_verticalalign(pElement->style) != VerticalAlign_Top)
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    this->Layout_InvalidateElementLayout(pChild, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                    return true;
                });
            }
#if 0
            else
            {
                // TODO: What is the point in this branch? Only repositioning relative and absolute children and only if the alignment is top? Why? What scenario requires this?

                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_positioning(pChild->style) != Positioning_Auto)
                    {
                        this->Layout_InvalidateElementLayout(pChild, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                    }
                });
            }
#endif

            // Siblings might have positions changed depending on the parent's alignment.
            if (pParent != nullptr)
            {
                switch (GUIElementStyle_Get_verticalalign(pParent->style))
                {
                case VerticalAlign_Bottom:
                    {
                        // All auto-positioned siblings before this element.
                        this->IterateElementPrevSiblings(pElement, [&](GUIElement* pSibling) -> bool
                        {
                            if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                            {
                                this->Layout_InvalidateElementLayout(pSibling, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                            }

                            return true;
                        });

                        break;
                    }

                case VerticalAlign_Center:
                    {
                        // All auto-positioned siblings, including this element.
                        this->IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
                        {
                            if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                            {
                                this->Layout_InvalidateElementLayout(pSibling, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                            }

                            return true;
                        });

                        break;
                    }

                case VerticalAlign_Top:
                default:
                    {
                        // All auto-positioned siblings after this element.
                        this->IterateElementNextSiblings(pElement, [&](GUIElement* pSibling) -> bool
                        {
                            if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                            {
                                this->Layout_InvalidateElementLayout(pSibling, LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid);
                            }

                            return true;
                        });

                        break;
                    }
                }
            }
        }


        // Now we need to remove ourselves from the invalid list.
        this->Layout_MarkElementHeightAsValid(pElement);


#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.EndLine();
#endif
    }

    void GUIContextBase::Layout_ValidateElementPosition(GUIElement* pElement)
    {
        assert(pElement != nullptr);

#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.BeginLine();
        m_layoutLogger.Print(GTLib::String::CreateFormatted("position: %d\n", pElement->handle).c_str());
#endif

        // We validate the position differently depending on it's positioning type (auto, relative or absolute).
        auto oldRelativeX = pElement->layout.relativePosX;
        auto oldRelativeY = pElement->layout.relativePosY;

        switch (GUIElementStyle_Get_positioning(pElement->style))
        {
        case Positioning_Absolute:
            {
                this->Layout_ValidateElementPosition_Absolute(pElement);
                break;
            }

        case Positioning_Relative:
            {
                this->Layout_ValidateElementPosition_Relative(pElement);
                break;
            }

        case Positioning_Auto:
        default:
            {
                this->Layout_ValidateElementPosition_Auto(pElement);
                break;
            }
        }


        if (oldRelativeX != pElement->layout.relativePosX || oldRelativeY != pElement->layout.relativePosY)
        {
            this->Layout_MarkElementPositionAsChanged(pElement);
        }



        // Now we need to remove ourselves from the invalid list.
        this->Layout_MarkElementPositionAsValid(pElement);

#if defined(GT_GUI_DEBUGGING)
        m_layoutLogger.EndLine();
#endif
    }

    void GUIContextBase::Layout_ValidateElementPosition_Absolute(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        assert(GUIElementStyle_Get_positioning(pElement->style) == Positioning_Absolute);
        {
            // X
            if (GUIElementStyle_Get_righthaspriority(pElement->style))
            {
                // Right

                uint32_t rightType;
                int32_t  right = GUIElementStyle_Get_right(pElement->style, rightType);

                auto pParent = this->GetElementParent(pElement);
                if (pParent != nullptr)
                {
                    pElement->layout.relativePosX = pParent->layout.width - pElement->layout.width;
                }
                else
                {
                    auto pSurface = this->GetElementSurface(pElement);
                    if (pSurface != nullptr)
                    {
                        pElement->layout.relativePosX = pSurface->width - pElement->layout.width;
                    }
                    else
                    {
                        pElement->layout.relativePosX = 0.0f;
                    }
                }


                if (rightType == NumberType_Percent)
                {
                    auto pSurface = this->GetElementSurface(pElement);
                    if (pSurface != nullptr)
                    {
                        pElement->layout.relativePosX += pSurface->width * (right / 100.0f);
                    }
                    else
                    {
                        // Undefined for absolute positioning.
                    }
                }
                else
                {
                    if (rightType == NumberType_Points)
                    {
                        pElement->layout.relativePosX += static_cast<float>(right) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
                    }
                    else
                    {
                        pElement->layout.relativePosX += static_cast<float>(right);
                    }
                }
            }
            else
            {
                // Left

                uint32_t leftType;
                int32_t  left = GUIElementStyle_Get_left(pElement->style, leftType);

                if (leftType == NumberType_Percent)
                {
                    auto pSurface = this->GetElementSurface(pElement);
                    if (pSurface != nullptr)
                    {
                        pElement->layout.relativePosX = pSurface->width * (left / 100.0f);
                    }
                    else
                    {
                        // Undefined for absolute positioning.
                    }
                }
                else
                {
                    if (leftType == NumberType_Points)
                    {
                        pElement->layout.relativePosX = static_cast<float>(left) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
                    }
                    else
                    {
                        pElement->layout.relativePosX = static_cast<float>(left);
                    }
                }
            }



            // Y
            if (GUIElementStyle_Get_bottomhaspriority(pElement->style))
            {
                // Bottom

                uint32_t bottomType;
                int32_t  bottom = GUIElementStyle_Get_bottom(pElement->style, bottomType);

                auto pParent = this->GetElementParent(pElement);
                if (pParent != nullptr)
                {
                    pElement->layout.relativePosY = pParent->layout.height - pElement->layout.height;
                }
                else
                {
                    auto pSurface = this->GetElementSurface(pElement);
                    if (pSurface != nullptr)
                    {
                        pElement->layout.relativePosY = pSurface->height - pElement->layout.height;
                    }
                    else
                    {
                        pElement->layout.relativePosY = 0.0f;
                    }
                }


                if (bottomType == NumberType_Percent)
                {
                    auto pSurface = this->GetElementSurface(pElement);
                    if (pSurface != nullptr)
                    {
                        pElement->layout.relativePosY += pSurface->width * (bottom / 100.0f);
                    }
                    else
                    {
                        // Undefined for absolute positioning.
                    }
                }
                else
                {
                    if (bottomType == NumberType_Points)
                    {
                        pElement->layout.relativePosY += static_cast<float>(bottom) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
                    }
                    else
                    {
                        pElement->layout.relativePosY += static_cast<float>(bottom);
                    }
                }
            }
            else
            {
                // Top

                uint32_t topType;
                int32_t  top = GUIElementStyle_Get_left(pElement->style, topType);

                if (topType == NumberType_Percent)
                {
                    auto pSurface = this->GetElementSurface(pElement);
                    if (pSurface != nullptr)
                    {
                        pElement->layout.relativePosY = pSurface->height * (top / 100.0f);
                    }
                    else
                    {
                        // Undefined for absolute positioning.
                    }
                }
                else
                {
                    if (topType == NumberType_Points)
                    {
                        pElement->layout.relativePosY = static_cast<float>(top) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
                    }
                    else
                    {
                        pElement->layout.relativePosY = static_cast<float>(top);
                    }
                }
            }
        }
    }
    
    void GUIContextBase::Layout_ValidateElementPosition_Relative(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        assert(GUIElementStyle_Get_positioning(pElement->style) == Positioning_Relative);
        {
            // The position is based on the parent without any consideration for siblings. If the pElement does not have a parent, we will position it against the surface.
            //
            // The position can be calculated in two stages. The first is to calculate the orign. The second is to calculate an offset. They are then added together to
            // form the final position.

            bool leftHasPriority = GUIElementStyle_Get_lefthaspriority(pElement->style);
            bool topHasPriority  = GUIElementStyle_Get_tophaspriority(pElement->style);


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
            

            auto pParent = this->GetElementParent(pElement);
            if (pParent != nullptr)
            {
                if (leftHasPriority)
                {
                    containerPaddingX     = pParent->layout.paddingLeft;
                    containerBorderWidthX = pParent->layout.borderLeftWidth;
                }
                else
                {
                    containerPaddingX     = -pParent->layout.paddingRight;
                    containerBorderWidthX = -pParent->layout.borderRightWidth;

                    originX = pParent->layout.width;
                }


                if (topHasPriority)
                {
                    containerPaddingY     = pParent->layout.paddingTop;
                    containerBorderWidthY = pParent->layout.borderTopWidth;
                }
                else
                {
                    containerPaddingY     = -pParent->layout.paddingBottom;
                    containerBorderWidthY = -pParent->layout.borderBottomWidth;

                    originY = pParent->layout.height;
                }
            }
            else
            {
                // Position based on the size of the surface.
                auto pSurface = this->GetElementSurface(pElement);
                if (pSurface != nullptr)
                {
                    if (!leftHasPriority)
                    {
                        // Right has priority.
                        originX = static_cast<float>(pSurface->width);
                    }

                    if (!topHasPriority)
                    {
                        // Bottom has priority.
                        originY = static_cast<float>(pSurface->height);
                    }
                }
            }


            // The origin needs to be moved based on the position-origin style attribute.
            switch (GUIElementStyle_Get_positionorigin(pElement->style))
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
                positionX = static_cast<float>(GUIElementStyle_Get_left(pElement->style, positionTypeX));
            }
            else
            {
                positionX = -static_cast<float>(GUIElementStyle_Get_right(pElement->style, positionTypeX));
                offsetX  -= pElement->layout.width;
            }

            if (topHasPriority)
            {
                positionY = static_cast<float>(GUIElementStyle_Get_top(pElement->style, positionTypeY));
            }
            else
            {
                positionY = -static_cast<float>(GUIElementStyle_Get_bottom(pElement->style, positionTypeY));
                offsetY  -= pElement->layout.height;
            }


            if (positionTypeX == NumberType_Percent)
            {
                offsetX += Layout_GetElementWidthForRelativeSizing(pParent) * (positionX / 100.0f);
            }
            else if (positionTypeX == NumberType_Points)
            {
                offsetX += positionX * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
            }
            else
            {
                offsetX += positionX;
            }

            if (positionTypeY == NumberType_Percent)
            {
                offsetY += Layout_GetElementHeightForRelativeSizing(pParent) * (positionY / 100.0f);
            }
            else if (positionTypeY == NumberType_Points)
            {
                offsetY += positionY * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
            }
            else
            {
                offsetY += positionY;
            }



            pElement->layout.relativePosX = originX + offsetX;
            pElement->layout.relativePosY = originY + offsetY;
        }
    }

    void GUIContextBase::Layout_ValidateElementPosition_Auto(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        assert(GUIElementStyle_Get_positioning(pElement->style) == Positioning_Auto);
        {
            auto pParent = this->GetElementParent(pElement);

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

            if (pParent != nullptr)
            {
                childAxis       = GUIElementStyle_Get_childaxis(      pParent->style);
                horizontalAlign = GUIElementStyle_Get_horizontalalign(pParent->style);
                verticalAlign   = GUIElementStyle_Get_verticalalign(  pParent->style);

                containerWidth  = pParent->layout.width;
                containerHeight = pParent->layout.height;

                containerPaddingLeft   = pParent->layout.paddingLeft;
                containerPaddingTop    = pParent->layout.paddingTop;
                containerPaddingRight  = pParent->layout.paddingRight;
                containerPaddingBottom = pParent->layout.paddingBottom;

                containerBorderLeftWidth   = pParent->layout.borderLeftWidth;
                containerBorderTopWidth    = pParent->layout.borderTopWidth;
                containerBorderRightWidth  = pParent->layout.borderRightWidth;
                containerBorderBottomWidth = pParent->layout.borderBottomWidth;

                containerInnerOffsetX = static_cast<float>(GUIElementStyle_Get_inneroffsetx(pParent->style));
                containerInnerOffsetY = static_cast<float>(GUIElementStyle_Get_inneroffsety(pParent->style));
            }
            else
            {
                auto pSurface = this->GetElementSurface(pElement);
                if (pSurface != nullptr)
                {
                    containerWidth  = static_cast<float>(pSurface->width);
                    containerHeight = static_cast<float>(pSurface->height);
                }
            }


            // When we validate an auto-positioned pElement, we're actually going to validate it and all of it's auto-positioned siblings in one batch. We do this in two passes. The first
            // pass calculates the dimensions of the siblings. The second pass calculates the actual positions.

            // Pass #1 - Calculate the sibling dimensions.
            float siblingsWidth  = 0.0f;
            float siblingsHeight = 0.0f;

            this->IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
            {
                assert(pSibling != nullptr);

                if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                {
                    // If the size of the pElement is invalid, we want to validate them first. We do this because validating the size will likely trigger
                    // an invalidation of the position, which will cause redundant position recalcuations. Instead, we simply just validate now.
                    if (GUIContextBase::Layout_IsElementWidthInvalid(pSibling))
                    {
                        this->Layout_ValidateElementWidth(pSibling);
                    }
                    if (GUIContextBase::Layout_IsElementHeightInvalid(pSibling))
                    {
                        this->Layout_ValidateElementHeight(pSibling);
                    }


                    if (childAxis == ChildAxis_Vertical)
                    {
                        // Child axis = vertical.
                        siblingsWidth   = GTLib::Max(siblingsWidth, GUIContextBase::Layout_GetElementOuterWidth(pSibling));
                        siblingsHeight += GUIContextBase::Layout_GetElementOuterHeight(pSibling);
                    }
                    else
                    {
                        // Child axis = horizontal.
                        siblingsWidth += GUIContextBase::Layout_GetElementOuterWidth(pSibling);
                        siblingsHeight = GTLib::Max(siblingsHeight, GUIContextBase::Layout_GetElementOuterHeight(pSibling));
                    }
                }

                return true;
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

            this->IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
            {
                assert(pSibling != nullptr);

                if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                {
                    // If the siblings layout is not invalid, we will need to invalidate the rectangle region of the pElement's previous layout rectangle. If the pElement's
                    // layout is already invalidated, this will have already been done.
                    if (pSibling->layout.flags == 0)
                    {
                        this->Painting_InvalidateElementRect(pSibling);
                    }



                    float prevRelativeX = pSibling->layout.relativePosX;
                    float prevRelativeY = pSibling->layout.relativePosY;

                    if (childAxis == ChildAxis_Vertical)
                    {
                        // Child axis = vertical.
                        pSibling->layout.relativePosY = offsetY + localOffsetY + pSibling->layout.marginTop;

                        if (horizontalAlign == HorizontalAlign_Left)
                        {
                            pSibling->layout.relativePosX = offsetX + localOffsetX + pSibling->layout.marginLeft;
                        }
                        else
                        {
                            if (horizontalAlign == HorizontalAlign_Right)
                            {
                                pSibling->layout.relativePosX = containerWidth - containerBorderRightWidth - containerPaddingRight - pSibling->layout.marginRight - pSibling->layout.width;
                            }
                            else
                            {
                                pSibling->layout.relativePosX = (containerWidth - GUIContextBase::Layout_GetElementOuterWidth(pSibling)) * 0.5f + pSibling->layout.marginLeft;
                            }
                        }
                    }
                    else
                    {
                        // Child axis = horizontal.
                        pSibling->layout.relativePosX = offsetX + localOffsetX + pSibling->layout.marginLeft;

                        if (verticalAlign == VerticalAlign_Top)
                        {
                            pSibling->layout.relativePosY = offsetY + localOffsetY + pSibling->layout.marginTop;
                        }
                        else
                        {
                            if (verticalAlign == VerticalAlign_Bottom)
                            {
                                pSibling->layout.relativePosY = containerHeight - containerBorderBottomWidth - containerPaddingBottom - pSibling->layout.marginBottom - pSibling->layout.height;
                            }
                            else
                            {
                                pSibling->layout.relativePosY = (containerHeight - GUIContextBase::Layout_GetElementOuterHeight(pSibling)) * 0.5f + pSibling->layout.marginTop;
                            }
                        }
                    }




                    // We adjust the local offset based on the axis we're running along.
                    if (GUIElementStyle_Get_visible(pSibling->style))
                    {
                        if (childAxis == ChildAxis_Vertical)
                        {
                            localOffsetY += GUIContextBase::Layout_GetElementOuterHeight(pSibling);
                        }
                        else
                        {
                            localOffsetX += GUIContextBase::Layout_GetElementOuterWidth(pSibling);
                        }
                    }


                    if (prevRelativeX != pSibling->layout.relativePosX || prevRelativeY != pSibling->layout.relativePosY)
                    {
                        this->Layout_MarkElementPositionAsChanged(pSibling);
                    }

                    // We now need to mark the position as valid.
                    this->Layout_MarkElementPositionAsValid(pSibling);
                }

                return true;
            });


            if (pParent != nullptr)
            {
                if (GUIElementStyle_Get_IsAnyWidthOfType(pParent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pParent, LayoutFlag_WidthInvalid);
                }
                if (GUIElementStyle_Get_IsAnyHeightOfType(pParent->style, NumberType_Auto))
                {
                    this->Layout_InvalidateElementLayout(pParent, LayoutFlag_HeightInvalid);
                }
            }
        }
    }

    void GUIContextBase::Layout_ValidateElementText(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        // TODO: Update the layout of the text container.



        // If the size is auto sized, it needs to be invalidated.
        if (GUIElementStyle_Get_IsAnyHeightOfType(pElement->style, NumberType_Auto))
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_WidthInvalid);
        }
        if (GUIElementStyle_Get_IsAnyHeightOfType(pElement->style, NumberType_Auto))
        {
            this->Layout_InvalidateElementLayout(pElement, LayoutFlag_HeightInvalid);
        }


        // Now we need to remove ourselves from the invalid list.
        this->Layout_MarkElementTextAsValid(pElement);
    }



    void GUIContextBase::Layout_MarkElementWidthAsValid(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        pElement->layout.flags &= ~LayoutFlag_WidthInvalid;
        if (pElement->layout.flags == 0)
        {
            m_layoutContext.invalidElements.Remove(pElement->layout.layoutValidationListItem);
            pElement->layout.layoutValidationListItem = nullptr;
        }
    }

    void GUIContextBase::Layout_MarkElementHeightAsValid(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        pElement->layout.flags &= ~LayoutFlag_HeightInvalid;
        if (pElement->layout.flags == 0)
        {
            m_layoutContext.invalidElements.Remove(pElement->layout.layoutValidationListItem);
            pElement->layout.layoutValidationListItem = nullptr;
        }
    }

    void GUIContextBase::Layout_MarkElementPositionAsValid(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        pElement->layout.flags &= ~LayoutFlag_RelativeXPositionInvalid;
        pElement->layout.flags &= ~LayoutFlag_RelativeYPositionInvalid;
        if (pElement->layout.flags == 0)
        {
            m_layoutContext.invalidElements.Remove(pElement->layout.layoutValidationListItem);
            pElement->layout.layoutValidationListItem = nullptr;
        }
    }

    void GUIContextBase::Layout_MarkElementTextAsValid(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        pElement->layout.flags &= ~LayoutFlag_TextInvalid;
        if (pElement->layout.flags == 0)
        {
            m_layoutContext.invalidElements.Remove(pElement->layout.layoutValidationListItem);
            pElement->layout.layoutValidationListItem = nullptr;
        }
    }



    float GUIContextBase::Layout_UpdateElementWidth(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        uint32_t styleWidthType;
        uint32_t styleWidth = GUIElementStyle_Get_width(pElement->style, styleWidthType);

        uint32_t styleMinWidthType;
        uint32_t styleMinWidth = GUIElementStyle_Get_minwidth(pElement->style, styleMinWidthType);

        uint32_t styleMaxWidthType;
        uint32_t styleMaxWidth = GUIElementStyle_Get_maxwidth(pElement->style, styleMaxWidthType);


        float newWidth = this->Layout_CalculateElementWidth(pElement, styleWidth, styleWidthType, true);             // <-- 'true' means we want to use flexing, if appropriate.

        float minWidth = 0;
        float maxWidth = newWidth;

        //if (styleMinWidthType != NumberType_Auto)
        {
            minWidth = this->Layout_CalculateElementWidth(pElement, styleMinWidth, styleMinWidthType, false);        // <-- 'false' means we don't want to consider flexing.
        }
        //if (styleMaxWidthType != NumberType_Auto)
        {
            maxWidth = this->Layout_CalculateElementWidth(pElement, styleMaxWidth, styleMaxWidthType, false);        // <-- 'false' means we don't want to consider flexing.
        }

        
        pElement->layout.unclampedWidth = newWidth;
        pElement->layout.width          = GTLib::Clamp(newWidth, minWidth, maxWidth);

        return GUIContextBase::Layout_GetElementOuterWidth(pElement);
    }

    float GUIContextBase::Layout_UpdateElementHeight(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        uint32_t styleHeightType;
        uint32_t styleHeight = GUIElementStyle_Get_height(pElement->style, styleHeightType);

        uint32_t styleMinHeightType;
        uint32_t styleMinHeight = GUIElementStyle_Get_minheight(pElement->style, styleMinHeightType);

        uint32_t styleMaxHeightType;
        uint32_t styleMaxHeight = GUIElementStyle_Get_maxheight(pElement->style, styleMaxHeightType);


        float newHeight = this->Layout_CalculateElementHeight(pElement, styleHeight, styleHeightType, true);             // <-- 'true' means we want to use flexing, if appropriate.

        float minHeight = 0;
        float maxHeight = newHeight;

        //if (styleMinHeightType != NumberType_Auto)
        {
            minHeight = this->Layout_CalculateElementHeight(pElement, styleMinHeight, styleMinHeightType, false);        // <-- 'false' means we don't want to consider flexing.
        }
        //if (styleMaxHeightType != NumberType_Auto)
        {
            maxHeight = this->Layout_CalculateElementHeight(pElement, styleMaxHeight, styleMaxHeightType, false);        // <-- 'false' means we don't want to consider flexing.
        }

        
        pElement->layout.unclampedHeight = newHeight;
        pElement->layout.height          = GTLib::Clamp(newHeight, minHeight, maxHeight);

        return GUIContextBase::Layout_GetElementOuterHeight(pElement);
    }


    // It is possible to have pElements sized such that they flex with the size of it's parent. If we want to perform this calculation, calculateFlexed will be
    // set to true.
    //
    // When calculating the size of a %-sized pElement, we use the following simple formula:
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
    // The available flexing area is the width of the parent, minus and fixed sized pElements (including margins). It is calculated using the parent as:
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
    // We ignore relative and absolute positioned children since they are usually a special case. Invisible pElements should not contribute to the calculation
    // either so they are also ignored. The margin and padding of children are always fixed (for now), so they are always included regardless of whether or
    // not the child has a flexible size.
    //
    // Once the available space has been determined, every pElement whose size is relative will be distributed into that space based on their width value. As
    // an example, if there are two pElements, one with a size of 10% and the other with a size of 20%, the pElements will be distributed such that the 10%
    // pElement is a third of the size of the 20% pElement (10% / (10% + 20%) = 0.333...)

    float GUIContextBase::Layout_CalculateElementWidth(GUIElement* pElement, uint32_t width, uint32_t widthType, bool calculateFlexed)
    {
        assert(pElement != nullptr);

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
                result = static_cast<float>(width) * this->GetXDPIScalingFactor(this->GetElementSurface(pElement));
                break;
            }

        case NumberType_Percent:     // Depends on the parent and non-% siblings.
            {
                float sizeRatio  = width / 100.0f;

                auto pParent = this->GetElementParent(pElement);
                if (pParent)
                {
                    float parentSize = this->Layout_GetElementWidthForRelativeSizing(pParent);

                    if (calculateFlexed && GUIElementStyle_Get_flexchildrenwidth(pParent->style) && GUIElementStyle_Get_positioning(pElement->style) == Positioning_Auto)
                    {
                        if (GUIElementStyle_Get_childaxis(pParent->style) == ChildAxis_Horizontal)
                        {
                            float totalPercent = 0.0f;
                            float totalFixed   = 0.0f;

                            this->IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
                            {
                                assert(pSibling != nullptr);

                                if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto && GUIElementStyle_Get_visible(pSibling->style))
                                {
                                    totalFixed += GUIContextBase::Layout_GetElementHorizontalMargin( pSibling);
                                    totalFixed += GUIContextBase::Layout_GetElementHorizontalPadding(pSibling);


                                    uint32_t siblingWidthType;
                                    uint32_t siblingWidth = GUIElementStyle_Get_width(pSibling->style, siblingWidthType);

                                    if (siblingWidthType == NumberType_Percent)
                                    {
                                        totalPercent += static_cast<float>(siblingWidth);
                                    }
                                    else
                                    {
                                        totalFixed += pSibling->layout.width;
                                    }
                                }

                                return true;
                            });


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
                    // The pElement does not have a parent, so we will size based on the surface.
                    auto pSurface = this->GetElementSurface(pElement);
                    if (pSurface != nullptr)
                    {
                        result = pSurface->width * sizeRatio;
                    }
                }

                break;
            }

        case NumberType_Auto:       // Depends on the children.
            {
                result = GUIContextBase::Layout_GetElementHorizontalBorderSize(pElement) + GUIContextBase::Layout_GetElementHorizontalPadding(pElement);

                // The width will be the width of the children plus padding.
                if (pElement->pFirstChild != nullptr)
                {
                    result += this->Layout_CalculateElementChildrenWidthForAutoSize(pElement);
                }
                else if (this->DoesElementHaveText(pElement))
                {
                    assert(pElement->pTextLayout != nullptr);
                    result += pElement->pTextLayout->GetTextWidth();

                    //result += pElement->textManager.GetTextWidth();
                }

                break;
            }
        }


        return result;
    }

    float GUIContextBase::Layout_CalculateElementHeight(GUIElement* pElement, uint32_t height, uint32_t heightType, bool calculateFlexed)
    {
        assert(pElement != nullptr);

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
                result = static_cast<float>(height) * this->GetYDPIScalingFactor(this->GetElementSurface(pElement));
                break;
            }

        case NumberType_Percent:     // Depends on the parent and non-% siblings.
            {
                float sizeRatio  = height / 100.0f;

                auto pParent = this->GetElementParent(pElement);
                if (pParent)
                {
                    float parentSize = this->Layout_GetElementHeightForRelativeSizing(pParent);

                    if (calculateFlexed && GUIElementStyle_Get_flexchildrenheight(pParent->style) && GUIElementStyle_Get_positioning(pElement->style) == Positioning_Auto)
                    {
                        if (GUIElementStyle_Get_childaxis(pParent->style) == ChildAxis_Vertical)
                        {
                            float totalPercent = 0.0f;
                            float totalFixed   = 0.0f;

                            this->IterateElementSiblingsAndSelf(pElement, [&](GUIElement* pSibling) -> bool
                            {
                                assert(pSibling != nullptr);

                                if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto && GUIElementStyle_Get_visible(pSibling->style))
                                {
                                    totalFixed += GUIContextBase::Layout_GetElementVerticalMargin( pSibling);
                                    totalFixed += GUIContextBase::Layout_GetElementVerticalPadding(pSibling);


                                    uint32_t siblingHeightType;
                                    uint32_t siblingHeight = GUIElementStyle_Get_height(pSibling->style, siblingHeightType);

                                    if (siblingHeightType == NumberType_Percent)
                                    {
                                        totalPercent += static_cast<float>(siblingHeight);
                                    }
                                    else
                                    {
                                        totalFixed += pSibling->layout.height;
                                    }
                                }

                                return true;
                            });

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
                    // The pElement does not have a parent, so we will size based on the surface.
                    auto pSurface = this->GetElementSurface(pElement);
                    if (pSurface != nullptr)
                    {
                        result = pSurface->height * sizeRatio;
                    }
                }

                break;
            }

        case NumberType_Auto:       // Depends on the children.
            {
                result = GUIContextBase::Layout_GetElementVerticalBorderSize(pElement) + GUIContextBase::Layout_GetElementVerticalPadding(pElement);

                // The height will be the height of the children plus padding.
                if (pElement->pFirstChild != nullptr)
                {
                    result += this->Layout_CalculateElementChildrenHeightForAutoSize(pElement);
                }
                else if (this->DoesElementHaveText(pElement))
                {
                    assert(pElement->pTextLayout != nullptr);
                    result += pElement->pTextLayout->GetTextHeight();

                    //result += pElement->textManager.GetTextHeight();
                }

                break;
            }
        }


        return result;
    }




    float GUIContextBase::Layout_CalculateElementChildrenWidthForAutoSize(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        float result = 0.0f;

        ChildAxis childAxis = GUIElementStyle_Get_childaxis(pElement->style);
        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_visible(pChild->style))
                    {
                        if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
                        {
                            result += this->Layout_GetElementHorizontalMargin(pChild);       // <-- Always include the margin.

                            uint32_t widthType;
                            uint32_t minWidthType;
                            uint32_t maxWidthType;
                            GUIElementStyle_Get_allwidthtypes(pChild->style, widthType, minWidthType, maxWidthType);

                            if (widthType == NumberType_Auto && (minWidthType == NumberType_Percent || maxWidthType == NumberType_Percent))
                            {
                                result += pChild->layout.unclampedWidth;
                            }
                            else
                            {
                                result += pChild->layout.width;
                            }
                        }
                    }

                    return true;
                });

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_visible(pChild->style))
                    {
                        if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
                        {
                            float childWidth = this->Layout_GetElementHorizontalMargin(pChild);       // <-- Always include the margin.

                            uint32_t widthType;
                            uint32_t minWidthType;
                            uint32_t maxWidthType;
                            GUIElementStyle_Get_allwidthtypes(pChild->style, widthType, minWidthType, maxWidthType);

                            if (widthType == NumberType_Auto && (minWidthType == NumberType_Percent || maxWidthType == NumberType_Percent))
                            {
                                childWidth += pChild->layout.unclampedWidth;
                            }
                            else
                            {
                                childWidth += pChild->layout.width;
                            }

                            result = GTLib::Max(result, childWidth);
                        }
                    }

                    return true;
                });

                break;
            }
        }

        return result;
    }

    float GUIContextBase::Layout_CalculateElementChildrenHeightForAutoSize(GUIElement* pElement) const
    {
        assert(pElement != nullptr);

        float result = 0.0f;

        ChildAxis childAxis = GUIElementStyle_Get_childaxis(pElement->style);
        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_visible(pChild->style))
                    {
                        if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
                        {
                            float childHeight = this->Layout_GetElementVerticalMargin(pChild);       // <-- Always include the margin.

                            uint32_t heightType;
                            uint32_t minHeightType;
                            uint32_t maxHeightType;
                            GUIElementStyle_Get_allheighttypes(pChild->style, heightType, minHeightType, maxHeightType);

                            if (heightType == NumberType_Auto && (minHeightType == NumberType_Percent || maxHeightType == NumberType_Percent))
                            {
                                childHeight += pChild->layout.unclampedHeight;
                            }
                            else
                            {
                                childHeight += pChild->layout.height;
                            }

                            result = GTLib::Max(result, childHeight);
                        }
                    }

                    return true;
                });

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_visible(pChild->style))
                    {
                        if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
                        {
                            result += this->Layout_GetElementVerticalMargin(pChild);       // <-- Always include the margin.

                            uint32_t heightType;
                            uint32_t minHeightType;
                            uint32_t maxHeightType;
                            GUIElementStyle_Get_allheighttypes(pChild->style, heightType, minHeightType, maxHeightType);

                            if (heightType == NumberType_Auto && (minHeightType == NumberType_Percent || maxHeightType == NumberType_Percent))
                            {
                                result += pChild->layout.unclampedHeight;
                            }
                            else
                            {
                                result += pChild->layout.height;
                            }
                        }
                    }

                    return true;
                });

                break;
            }
        }

        return result;
    }


    float GUIContextBase::Layout_CalculateElementChildrenWidth(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        float result = 0.0f;

        ChildAxis childAxis = GUIElementStyle_Get_childaxis(pElement->style);
        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_visible(pChild->style))
                    {
                        if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
                        {
                            result += GUIContextBase::Layout_GetElementOuterWidth(pChild);
                        }
                    }

                    return true;
                });

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_visible(pChild->style))
                    {
                        if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
                        {
                            result = GTLib::Max(result, GUIContextBase::Layout_GetElementOuterWidth(pChild));
                        }
                    }

                    return true;
                });

                break;
            }
        }

        return result;
    }

    float GUIContextBase::Layout_CalculateElementChildrenHeight(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        float result = 0.0f;

        ChildAxis childAxis = GUIElementStyle_Get_childaxis(pElement->style);
        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_visible(pChild->style))
                    {
                        if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
                        {
                            result = GTLib::Max(result, GUIContextBase::Layout_GetElementOuterHeight(pChild));
                        }
                    }

                    return true;
                });

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
                {
                    if (GUIElementStyle_Get_visible(pChild->style))
                    {
                        if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
                        {
                            result += GUIContextBase::Layout_GetElementOuterHeight(pChild);
                        }
                    }

                    return true;
                });

                break;
            }
        }

        return result;
    }


    float GUIContextBase::Layout_CalculateTotalVisibleAutoPositionedSiblingsWidth(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        float result = 0.0f;

        ChildAxis childAxis = ChildAxis_Vertical;
        
        auto pParent = this->GetElementParent(pElement);
        if (pParent != nullptr)
        {
            childAxis = GUIElementStyle_Get_childaxis(pParent->style);
        }


        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                if (GUIElementStyle_Get_visible(pElement->style) && GUIElementStyle_Get_positioning(pElement->style) == Positioning_Auto)
                {
                    result = GUIContextBase::Layout_GetElementOuterWidth(pElement);
                }

                this->IterateElementSiblingsAndNotSelf(pElement, [&](GUIElement* pSibling) -> bool
                {
                    if (GUIElementStyle_Get_visible(pSibling->style) && GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                    {
                        result += GUIContextBase::Layout_GetElementOuterWidth(pSibling);
                    }

                    return true;
                });

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                if (GUIElementStyle_Get_visible(pElement->style) && GUIElementStyle_Get_positioning(pElement->style) == Positioning_Auto)
                {
                    result = GUIContextBase::Layout_GetElementOuterWidth(pElement);
                }

                this->IterateElementSiblingsAndNotSelf(pElement, [&](GUIElement* pSibling) -> bool
                {
                    if (GUIElementStyle_Get_visible(pSibling->style) && GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                    {
                        result = GTLib::Max(result, GUIContextBase::Layout_GetElementOuterWidth(pSibling));
                    }

                    return true;
                });

                break;
            }
        }

        return result;
    }

    float GUIContextBase::Layout_CalculateTotalVisibleAutoPositionedSiblingsHeight(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        float result = 0.0f;

        ChildAxis childAxis = ChildAxis_Vertical;
        
        auto pParent = this->GetElementParent(pElement);
        if (pParent != nullptr)
        {
            childAxis = GUIElementStyle_Get_childaxis(pParent->style);
        }


        switch (childAxis)
        {
        case ChildAxis_Horizontal:
            {
                if (GUIElementStyle_Get_visible(pElement->style) && GUIElementStyle_Get_positioning(pElement->style) == Positioning_Auto)
                {
                    result = GUIContextBase::Layout_GetElementOuterHeight(pElement);
                }

                this->IterateElementSiblingsAndNotSelf(pElement, [&](GUIElement* pSibling) -> bool
                {
                    if (GUIElementStyle_Get_visible(pElement->style) && GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                    {
                        result = GTLib::Max(result, GUIContextBase::Layout_GetElementOuterHeight(pSibling));
                    }

                    return true;
                });

                break;
            }


        case ChildAxis_Vertical:
        default:
            {
                if (GUIElementStyle_Get_visible(pElement->style) && GUIElementStyle_Get_positioning(pElement->style) == Positioning_Auto)
                {
                    result = GUIContextBase::Layout_GetElementOuterHeight(pElement);
                }

                this->IterateElementSiblingsAndNotSelf(pElement, [&](GUIElement* pSibling) -> bool
                {
                    if (GUIElementStyle_Get_visible(pSibling->style) && GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                    {
                        result += GUIContextBase::Layout_GetElementOuterHeight(pSibling);
                    }

                    return true;
                });

                break;
            }
        }

        return result;
    }


    GUIElement* GUIContextBase::Layout_GetNeighboringAutoPositionedSibling(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        GUIElement* pResult = nullptr;


        this->IterateElementNextSiblings(pElement, [&](GUIElement* pSibling) -> bool
        {
            if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
            {
                pResult = pSibling;
                return false;
            }

            return true;
        });

        if (pResult == nullptr)
        {
            this->IterateElementPrevSiblingsReverse(pElement, [&](GUIElement* pSibling) -> bool
            {
                if (GUIElementStyle_Get_positioning(pSibling->style) == Positioning_Auto)
                {
                    pResult = pSibling;
                    return false;
                }

                return true;
            });
        }
        

        return pResult;
    }

    GUIElement* GUIContextBase::Layout_GetFirstAutoPositionedChild(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        GUIElement* pResult = nullptr;
        this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
        {
            if (GUIElementStyle_Get_positioning(pChild->style) == Positioning_Auto)
            {
                pResult = pElement;
                return false;
            }

            return true;
        });

        return pResult;
    }



    void GUIContextBase::Layout_MarkElementPositionAsChanged(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        // If the flags specifying the validated layout properties is non-zero, we know it is already in our validated pElements list and thus
        // doesn't need to be added again.
        if (pElement->layout.layoutChangeFlags == 0)
        {
            m_layoutContext.validatedElements.PushBack(pElement);
        }


        // When the relative position of this pElement changes, the absolute position of both this pElement and it's descendents will need
        // to be recalculated. We're not going to do that yet, however we will need to get setup for it.
        //
        // The absolute position of pElements is calculated recursively - when an pElement has it's position updated, it will update the
        // position of it's children, and so on.
        if (!this->Layout_HasElementAbsolutePositionChanged(pElement))      // <-- The is recursive.
        {
            // If a child to this pElement is already in this list it needs to be removed.
            for (size_t iChildElement = 0; iChildElement < m_layoutContext.elementsWithInvalidAbsolutePositions.GetCount(); )
            {
                auto pChildElement = m_layoutContext.elementsWithInvalidAbsolutePositions[iChildElement];
                assert(pChildElement != nullptr);
                {
                    if (this->IsElementAncestor(pChildElement, pElement))
                    {
                        m_layoutContext.elementsWithInvalidAbsolutePositions.Remove(iChildElement);
                    }
                    else
                    {
                        ++iChildElement;
                    }
                }
            }


            m_layoutContext.elementsWithInvalidAbsolutePositions.PushBack(pElement);
        }


        pElement->layout.layoutChangeFlags |= LayoutFlag_PositionChanged;
    }

    void GUIContextBase::Layout_MarkElementSizeAsChanged(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        // If the flags specifying the validated layout properties is non-zero, we know it is already in our validated pElements list and thus
        // doesn't need to be added again.
        if (pElement->layout.layoutChangeFlags == 0)
        {
            m_layoutContext.validatedElements.PushBack(pElement);
        }

        pElement->layout.layoutChangeFlags |= LayoutFlag_SizeChanged;
    }


    bool GUIContextBase::Layout_HasElementAbsolutePositionChanged(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        if ((pElement->layout.layoutChangeFlags & LayoutFlag_PositionChanged) != 0)
        {
            return true;
        }


        // If any ancestor has an invalid absolute position, then so will this one.
        auto pParent = this->GetElementParent(pElement);
        if (pParent != nullptr)
        {
            return this->Layout_HasElementAbsolutePositionChanged(pParent);
        }


        return false;
    }


    void GUIContextBase::Layout_UpdateElementAbsolutePositions()
    {
        for (size_t iElement = 0; iElement < m_layoutContext.elementsWithInvalidAbsolutePositions.GetCount(); ++iElement)
        {
            auto pElement = m_layoutContext.elementsWithInvalidAbsolutePositions[iElement];
            assert(pElement != nullptr);
            {
                this->Layout_UpdateElementAbsolutePosition(pElement);
            }
        }


        m_layoutContext.elementsWithInvalidAbsolutePositions.Clear();
    }

    void GUIContextBase::Layout_UpdateElementAbsolutePosition(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        pElement->layout.absolutePosX = pElement->layout.relativePosX;
        pElement->layout.absolutePosY = pElement->layout.relativePosY;

        auto pParent = this->GetElementParent(pElement);
        if (pParent != nullptr)
        {
            if (GUIElementStyle_Get_positioning(pElement->style) != GT::Positioning_Absolute)
            {
                pElement->layout.absolutePosX += pParent->layout.absolutePosX;
                pElement->layout.absolutePosY += pParent->layout.absolutePosY;
            }
        }


        // Children will need to be updated.
        this->IterateElementChildren(pElement, [&](GUIElement* pChild) -> bool
        {
            this->Layout_UpdateElementAbsolutePosition(pChild);
            return true;
        });
    }

    void GUIContextBase::Layout_InvalidateRectsAndPostEventsToElements()
    {
        for (size_t iElement = 0; iElement < m_layoutContext.validatedElements.GetCount(); ++iElement)
        {
            auto pElement = m_layoutContext.validatedElements[iElement];
            assert(pElement != nullptr);
            {
                // Invalidate the rectangle.
                this->Painting_InvalidateElementRect(pElement);


                // Post events.
                //
                // There is a chance when events are posted that the pElement will be deleted. We need to make sure we don't attempted to dereference the pElement pointer
                // after posting events.
                unsigned int width  = static_cast<unsigned int>(pElement->layout.width);
                unsigned int height = static_cast<unsigned int>(pElement->layout.height);
                int xPos = static_cast<int>(pElement->layout.relativePosX);
                int yPos = static_cast<int>(pElement->layout.relativePosY);
                uint16_t layoutChangeFlags = pElement->layout.layoutChangeFlags;


                // We need to clear the change flags on the pElement so we know they are now valid. We do this before posting the events so that we don't dereference the
                // pElement object after posting the events since it is possible for the host application to delete it.
                pElement->layout.layoutChangeFlags &= ~(LayoutFlag_PositionChanged | LayoutFlag_SizeChanged);


                switch ((layoutChangeFlags & (LayoutFlag_SizeChanged | LayoutFlag_PositionChanged)))
                {
                case LayoutFlag_SizeChanged | LayoutFlag_PositionChanged:
                    {
                        this->PostEvent_OnSizeAndOnMove(pElement, width, height, xPos, yPos);
                        break;
                    }

                case LayoutFlag_SizeChanged:
                    {
                        this->PostEvent_OnSize(pElement, width, height);
                        break;
                    }

                case LayoutFlag_PositionChanged:
                    {
                        this->PostEvent_OnMove(pElement, xPos, yPos);
                        break;
                    }

                default:
                    {
                        break;
                    }
                }
            }
        }

        m_layoutContext.validatedElements.Clear();
    }



    ////////////////////////////
    // Static Layout Helpers

    float GUIContextBase::Layout_GetElementHorizontalPadding(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.paddingLeft + pElement->layout.paddingRight;
    }

    float GUIContextBase::Layout_GetElementVerticalPadding(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.paddingTop + pElement->layout.paddingBottom;
    }

    float GUIContextBase::Layout_GetElementHorizontalMargin(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.marginLeft + pElement->layout.marginRight;
    }

    float GUIContextBase::Layout_GetElementVerticalMargin(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.marginTop + pElement->layout.marginBottom;
    }

    float GUIContextBase::Layout_GetElementOuterWidth(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.width + GUIContextBase::Layout_GetElementHorizontalMargin(pElement);
    }

    float GUIContextBase::Layout_GetElementHorizontalBorderSize(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.borderLeftWidth + pElement->layout.borderRightWidth;
    }

    float GUIContextBase::Layout_GetElementVerticalBorderSize(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.borderTopWidth + pElement->layout.borderBottomWidth;
    }

    float GUIContextBase::Layout_GetElementOuterHeight(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.height + GUIContextBase::Layout_GetElementVerticalMargin(pElement);
    }

    float GUIContextBase::Layout_GetElementInnerWidth(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.width - GUIContextBase::Layout_GetElementHorizontalPadding(pElement) - GUIContextBase::Layout_GetElementHorizontalBorderSize(pElement);
    }

    float GUIContextBase::Layout_GetElementInnerHeight(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.height - GUIContextBase::Layout_GetElementVerticalPadding(pElement) - GUIContextBase::Layout_GetElementVerticalBorderSize(pElement);
    }

    float GUIContextBase::Layout_GetElementInnerBorderWidth(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.width - GUIContextBase::Layout_GetElementHorizontalBorderSize(pElement);
    }

    float GUIContextBase::Layout_GetElementInnerBorderHeight(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return pElement->layout.height - GUIContextBase::Layout_GetElementVerticalBorderSize(pElement);
    }


    float GUIContextBase::Layout_GetElementWidthForRelativeSizing(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        switch (GUIElementStyle_Get_childrenwidthboundary(pElement->style))
        {
        case ChildrenSizeBoundary_Outer:             // The outside of the borders.
            {
                return pElement->layout.width;
            }

        case ChildrenSizeBoundary_InnerBorder:       // The inside of the borders, but outside the padding.
            {
                return GUIContextBase::Layout_GetElementInnerBorderWidth(pElement);
            }

        case ChildrenSizeBoundary_Inner:             // The inside of the borders and padding (default)
        default:
            {
                return GUIContextBase::Layout_GetElementInnerWidth(pElement);
            }
        }
    }

    float GUIContextBase::Layout_GetElementHeightForRelativeSizing(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        switch (GUIElementStyle_Get_childrenheightboundary(pElement->style))
        {
        case ChildrenSizeBoundary_Outer:             // The outside of the borders.
            {
                return pElement->layout.height;
            }

        case ChildrenSizeBoundary_InnerBorder:       // The inside of the borders, but outside the padding.
            {
                return GUIContextBase::Layout_GetElementInnerBorderHeight(pElement);
            }

        case ChildrenSizeBoundary_Inner:             // The inside of the borders and padding (default)
        default:
            {
                return GUIContextBase::Layout_GetElementInnerHeight(pElement);
            }
        }
    }

    bool GUIContextBase::Layout_IsElementWidthInvalid(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return (pElement->layout.flags & LayoutFlag_WidthInvalid) != 0;
    }

    bool GUIContextBase::Layout_IsElementHeightInvalid(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return (pElement->layout.flags & LayoutFlag_HeightInvalid) != 0;
    }

    bool GUIContextBase::Layout_IsElementPositionInvalid(GUIElement* pElement)
    {
        assert(pElement != nullptr);

        return (pElement->layout.flags & (LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid)) != 0;
    }
}