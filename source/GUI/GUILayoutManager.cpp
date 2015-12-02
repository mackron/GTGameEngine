// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/GUI/GUILayoutManager.hpp>
#include <GTGE/GUI/GUIServer.hpp>

namespace GT
{
    static const uint32_t WidthInvalidated    = (1 << 0);
    static const uint32_t HeightInvalidated   = (1 << 1);
    static const uint32_t PositionInvalidated = (1 << 2);
    static const uint32_t TextInvalidated     = (1 << 3);
    
    
    GUILayoutManager::GUILayoutManager()
        : invalidElements(), validatedElements(), topLevelValidatedElements()
    {
    }

    GUILayoutManager::~GUILayoutManager()
    {
    }

    void GUILayoutManager::InvalidateWidth(GUIElement &element)
    {
        if (!(element.layout.flags & WidthInvalidated))
        {
            if (element.layout.flags == 0)
            {
                element.layout.layoutManagerListItem = this->invalidElements.Append(&element);
            }
            
            element.layout.flags |= WidthInvalidated;
        }
    }

    void GUILayoutManager::InvalidateHeight(GUIElement &element)
    {
        if (!(element.layout.flags & HeightInvalidated))
        {
            if (element.layout.flags == 0)
            {
                element.layout.layoutManagerListItem = this->invalidElements.Append(&element);
            }
            
            element.layout.flags |= HeightInvalidated;
        }
    }

    void GUILayoutManager::InvalidatePosition(GUIElement &element)
    {
        if (!(element.layout.flags & PositionInvalidated))
        {
            if (element.layout.flags == 0)
            {
                element.layout.layoutManagerListItem = this->invalidElements.Append(&element);
            }
            
            element.layout.flags |= PositionInvalidated;
        }
    }

    void GUILayoutManager::InvalidateText(GUIElement &element)
    {
        if (!(element.layout.flags & TextInvalidated))
        {
            if (element.layout.flags == 0)
            {
                element.layout.layoutManagerListItem = this->invalidElements.Append(&element);
            }
            
            element.layout.flags |= TextInvalidated;
        }
    }


    void GUILayoutManager::Validate()
    {
        while (this->invalidElements.root != nullptr)
        {
            auto iElement = this->invalidElements.root;
            assert(iElement != nullptr);
            {
                auto element = iElement->value;
                assert(element != nullptr);
                {
                    if ((element->layout.flags & WidthInvalidated))
                    {
                        this->ValidateWidth(*element);
                    }
                    
                    if ((element->layout.flags & HeightInvalidated))
                    {
                        this->ValidateHeight(*element);
                    }
                    
                    if ((element->layout.flags & PositionInvalidated))
                    {
                        this->ValidatePosition(*element);
                    }
                    
                    if ((element->layout.flags & TextInvalidated))
                    {
                        this->ValidateText(*element);
                    }
                }
            }
        }

        this->PostProcess();
    }


    void GUILayoutManager::RemoveElement(GUIElement &element)
    {
        if (element.layout.layoutManagerListItem != nullptr)
        {
            this->invalidElements.Remove(element.layout.layoutManagerListItem);
            
            element.layout.flags                 = 0;
            element.layout.layoutManagerListItem = nullptr;
        }
        
        
        this->validatedElements.Remove(&element);
        this->topLevelValidatedElements.RemoveFirstOccuranceOf(&element);
        
        
        // Post conditions.
        assert(element.layout.flags                 == 0);
        assert(element.layout.layoutManagerListItem == nullptr);
    }


    void GUILayoutManager::ValidateWidth(GUIElement &element)
    {
        auto oldWidth = element.GetOuterWidth();
        auto newWidth = GUILayoutManager::UpdateWidth(element);

        if (oldWidth != newWidth)
        {
            this->MarkSizeAsChanged(element);


            //////////////////////////////////////////////////////////////////////////
            // Dependencies

            // Widths
            //
            // Any %-sized children will have changed.
            for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
            {
                if (iChild->style.width->InPercent() || iChild->style.minWidth->InPercent() || iChild->style.maxWidth->InPercent())
                {
                    this->ValidateWidth(*iChild);
                }
            }

            // Flexed siblings will need to have their width's updated.
            if (element.parent != nullptr && element.parent->style.flexChildWidth->value && element.parent->style.childPlane->value == GUIPlane_Horizontal && !element.style.width->InPercent())
            {
                for (auto iSibling = element.parent->firstChild; iSibling != nullptr; iSibling = iSibling->nextSibling)
                {
                    if (iSibling != &element && iSibling->style.width->InPercent())
                    {
                        if (!this->IsWidthInvalid(*iSibling))
                        {
                            this->ValidateWidth(*iSibling);
                        }
                    }
                }
            }

            // If the parent is auto-sized, it will also have it's width changed.
            if (element.parent != nullptr && element.parent->style.width->Automatic())
            {
                if (!element.style.width->InPercent())
                {
                    this->ValidateWidth(*element.parent);
                }
            }



            // Positions
            //
            // The position of this element will have changed if we're not aligned left.
            if ((element.parent != nullptr && element.parent->style.horizontalAlign->value != GUIAlign_Left) || (element.style.positioning->value == GUIPositioning_Relative && element.style.rightHasPriority))
            {
                this->InvalidatePosition(element);
            }

            // Children might have positions changed depending on alignment.
            if (element.style.horizontalAlign->value != GUIAlign_Left)
            {
                for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
                {
                    this->InvalidatePosition(*iChild);
                }
            }
            else
            {
                // Relative elements need to be repositioned in this case.
                for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
                {
                    if (iChild->style.positioning->value != GUIPositioning_Auto)
                    {
                        this->InvalidatePosition(*iChild);
                    }
                }
            }

            // Siblings might have positions changed depending on the parent's alignment.
            if (element.parent != nullptr)
            {
                if (element.parent->style.horizontalAlign->value == GUIAlign_Left)
                {
                    for (auto iSibling = &element; iSibling != nullptr; iSibling = iSibling->nextSibling)
                    {
                        if (iSibling->style.positioning->value == GUIPositioning_Auto)
                        {
                            this->InvalidatePosition(*iSibling);
                        }
                    }
                }
                else if (element.parent->style.horizontalAlign->value == GUIAlign_Center)
                {
                    for (auto iSibling = element.parent->firstChild; iSibling != nullptr; iSibling = iSibling->nextSibling)
                    {
                        if (iSibling->style.positioning->value == GUIPositioning_Auto)
                        {
                            this->InvalidatePosition(*iSibling);
                        }
                    }
                }
                else if (element.parent->style.horizontalAlign->value == GUIAlign_Right)
                {
                    for (auto iSibling = element.parent->firstChild; iSibling != &element; iSibling = iSibling->nextSibling)
                    {
                        if (iSibling->style.positioning->value == GUIPositioning_Auto)
                        {
                            this->InvalidatePosition(*iSibling);
                        }
                    }
                }
            }

            // Text
            if (element.HasText() || element.HasEditableText())
            {
                this->ValidateText(element, false);
            }


            element.InvalidateBorderRenderingData();
            element.InvalidateBackgroundRenderingData();
            element.InvalidateShadowRenderingData();
        }



        // Now we need to remove ourselves from the invalid list.
        element.layout.flags &= ~WidthInvalidated;
        if (element.layout.flags == 0)
        {
            this->invalidElements.Remove(element.layout.layoutManagerListItem);
            element.layout.layoutManagerListItem = nullptr;
        }
    }

    void GUILayoutManager::ValidateHeight(GUIElement &element)
    {
        auto oldHeight = element.GetOuterHeight();
        auto newHeight = GUILayoutManager::UpdateHeight(element);

        if (oldHeight != newHeight)
        {
            this->MarkSizeAsChanged(element);


            //////////////////////////////////////////////////////////////////////////
            // Dependencies

            // Any %-sized children will have changed.
            for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
            {
                if (iChild->style.height->InPercent() || iChild->style.minHeight->InPercent() || iChild->style.maxHeight->InPercent())
                {
                    this->ValidateHeight(*iChild);
                }
            }

            // Flexed siblings will need to have their heights's updated.
            if (element.parent != nullptr && element.parent->style.flexChildHeight->value && element.parent->style.childPlane->value == GUIPlane_Vertical && !element.style.height->InPercent())
            {
                for (auto iSibling = element.parent->firstChild; iSibling != nullptr; iSibling = iSibling->nextSibling)
                {
                    if (iSibling != &element && iSibling->style.height->InPercent())
                    {
                        if (!this->IsHeightInvalid(*iSibling))       // <-- TODO: This is supposed to be an optimization. Needs profiling.
                        {
                            this->ValidateHeight(*iSibling);
                        }
                    }
                }
            }

            // If the parent is auto-sized, it will also have it's height changed.
            if (element.parent != nullptr && element.parent->style.height->Automatic())
            {
                if (!element.style.height->InPercent())
                {
                    this->ValidateHeight(*element.parent);
                }
            }


            // Positions
            //
            // The position of this element will have changed if we're not aligned top.
            if ((element.parent != nullptr && element.parent->style.verticalAlign->value != GUIAlign_Top) || (element.style.positioning->value == GUIPositioning_Relative && element.style.bottomHasPriority))
            {
                this->InvalidatePosition(element);
            }

            // Children might have positions changed depending on alignment.
            if (element.style.verticalAlign->value != GUIAlign_Top)
            {
                for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
                {
                    this->InvalidatePosition(*iChild);
                }
            }
            else
            {
                // Relative elements need to be repositioned in this case.
                for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
                {
                    if (iChild->style.positioning->value != GUIPositioning_Auto)
                    {
                        this->InvalidatePosition(*iChild);
                    }
                }
            }

            // Siblings might have positions changed depending on the parent's alignment.
            if (element.parent != nullptr)
            {
                if (element.parent->style.verticalAlign->value == GUIAlign_Top)
                {
                    for (auto iSibling = &element; iSibling != nullptr; iSibling = iSibling->nextSibling)
                    {
                        if (iSibling->style.positioning->value == GUIPositioning_Auto)
                        {
                            this->InvalidatePosition(*iSibling);
                        }
                    }
                }
                else if (element.parent->style.verticalAlign->value == GUIAlign_Center)
                {
                    for (auto iSibling = element.parent->firstChild; iSibling != nullptr; iSibling = iSibling->nextSibling)
                    {
                        if (iSibling->style.positioning->value == GUIPositioning_Auto)
                        {
                            this->InvalidatePosition(*iSibling);
                        }
                    }
                }
                else if (element.parent->style.verticalAlign->value == GUIAlign_Bottom)
                {
                    for (auto iSibling = element.parent->firstChild; iSibling != &element; iSibling = iSibling->nextSibling)
                    {
                        if (iSibling->style.positioning->value == GUIPositioning_Auto)
                        {
                            this->InvalidatePosition(*iSibling);
                        }
                    }
                }
            }


            // Text/Font
            if (element.style.fontSize->InPercent())
            {
                element.UpdateFontFromStyle();
                this->ValidateText(element, false);
            }
            else
            {
                if (element.HasText() || element.HasEditableText())
                {
                    this->ValidateText(element, false);
                }
            }


            element.InvalidateBorderRenderingData();
            element.InvalidateBackgroundRenderingData();
            element.InvalidateShadowRenderingData();
        }



        // Now we need to remove ourselves from the invalid list.
        element.layout.flags &= ~HeightInvalidated;        
        if (element.layout.flags == 0)
        {
            this->invalidElements.Remove(element.layout.layoutManagerListItem);
            element.layout.layoutManagerListItem = nullptr;
        }
    }


    void GUILayoutManager::ValidatePosition(GUIElement &element, bool invalidateSiblings)
    {
        // We validate the position differently depending on it's positioning type (auto, relative or absolute).
        auto oldX = element.x;
        auto oldY = element.y;

        if (element.style.positioning->value == GUIPositioning_Auto)
        {
            this->ValidatePosition_Auto(element, invalidateSiblings);
        }
        else if (element.style.positioning->value == GUIPositioning_Relative)
        {
            this->ValidatePosition_Relative(element);
        }
        else if (element.style.positioning->value == GUIPositioning_Absolute)
        {
            this->ValidatePosition_Absolute(element);
        }


        if (oldX != element.x || oldY != element.y)
        {
            this->MarkPositionAsChanged(element);
        }



        element.layout.flags &= ~PositionInvalidated;        
        if (element.layout.flags == 0)
        {
            this->invalidElements.Remove(element.layout.layoutManagerListItem);
            element.layout.layoutManagerListItem = nullptr;
        }
    }

    void GUILayoutManager::ValidatePosition_Auto(GUIElement &element, bool validateSiblings)
    {
        // In order for the position of this element to be correct, we need to validate the previous siblings. If we don't do this, it means the element
        // will not be positioned correctly.
        if (validateSiblings)
        {
            if (element.parent != nullptr)
            {
                for (auto iSibling = element.parent->firstChild; iSibling != nullptr; iSibling = iSibling->nextSibling)
                {
                    if (iSibling->style.positioning->value == GUIPositioning_Auto && this->IsPositionInvalid(*iSibling))
                    {
                        this->ValidatePosition(*iSibling, false);      // <-- 'false' means to not invalidate siblings. Important!
                    }
                }

                // The loop above will validate the original input element. Thus, we can terminate here.
                return;
            }
        }

        

        // The position is based on the previous auto-positioned sibling. We call this the provoking sibling. We retrieve this first.
        auto provokingSibling = this->GetProvokingSibling(element);

        // The positions depend on a few things, the main one being the plane the siblings are running along. This is defined by the parent. Sometimes there will
        // be cases where the element may have been orphaned, but the layout command is still active. In this case the parent will be set to null. We simply use
        // a conditional branch to filter this.
        if (element.parent != nullptr)
        {
            if (element.parent->style.childPlane->value == GUIPlane_Horizontal) // Horizontal Plane
            {
                // X
                if (provokingSibling != nullptr)
                {
                    element.x = provokingSibling->x + provokingSibling->width + provokingSibling->GetRightMargin();
                }
                else
                {
                    // If we don't have a provoking sibling, it means this is the first one. In this case, we need to ensure the initial position is correct. This depends
                    // on the alignment. If we don't get this initial positioning correct, all other siblings will be incorrectly positioned.
                    if (element.parent->style.horizontalAlign->value == GUIAlign_Right)
                    {
                        element.x = element.parent->width - element.parent->GetRightPadding() - element.parent->GetChildrenWidth();
                    }
                    else if (element.parent->style.horizontalAlign->value == GUIAlign_Center)
                    {
                        element.x = (element.parent->width - element.parent->GetChildrenWidth()) / 2;
                    }
                    else    // Left by default.
                    {
                        element.x = element.parent->GetLeftPadding();
                    }
                }

                element.x += element.GetLeftMargin();


                // Y
                if (element.parent->style.verticalAlign->value == GUIAlign_Bottom)
                {
                    element.y = element.parent->height - element.parent->GetBottomPadding() - element.height - element.GetBottomMargin();
                }
                else if (element.parent->style.verticalAlign->value == GUIAlign_Center)
                {
                    element.y = (element.parent->height - element.GetOuterHeight()) / 2;
                }
                else        // Top by default.
                {
                    element.y = element.parent->GetTopPadding() + element.GetTopMargin();
                }
            }
            else    // Vertical Plane
            {
                // X
                if (element.parent->style.horizontalAlign->value == GUIAlign_Right)
                {
                    element.x = element.parent->width - element.parent->GetRightPadding() - element.width - element.GetRightMargin();
                }
                else if (element.parent->style.horizontalAlign->value == GUIAlign_Center)
                {
                    element.x = (element.parent->width - element.GetOuterWidth()) / 2;
                }
                else        // Left by default.
                {
                    element.x = element.parent->GetLeftPadding() + element.GetLeftMargin();
                }

                // Y
                if (provokingSibling != nullptr)
                {
                    element.y = provokingSibling->y + provokingSibling->height + provokingSibling->GetBottomMargin();
                }
                else
                {
                    // If we don't have a provoking sibling, it means this is the first one. In this case, we need to ensure the initial position is correct. This depends
                    // on the alignment. If we don't get this initial positioning correct, all other siblings will not be positioned correctly.
                    if (element.parent->style.verticalAlign->value == GUIAlign_Bottom)
                    {
                        element.y = element.parent->height - element.parent->GetBottomPadding() - element.parent->GetChildrenHeight();
                    }
                    else if (element.parent->style.verticalAlign->value == GUIAlign_Center)
                    {
                        element.y = (element.parent->height - element.parent->GetChildrenHeight()) / 2;
                    }
                    else    // Top by default.
                    {
                        element.y = element.parent->GetTopPadding();
                    }
                }

                element.y += element.GetTopMargin();
            }

            // Remember, we have previously checked that the parent is not null.
            if (provokingSibling == nullptr)
            {
                element.x += static_cast<int>(element.parent->style.innerOffsetX->value);
                element.y += static_cast<int>(element.parent->style.innerOffsetY->value);
            }
        }
    }

    void GUILayoutManager::ValidatePosition_Relative(GUIElement &element)
    {
        // The position is based on the parent without any consideration for siblings. Sometimes there will be cases where the element may have been orphaned, but
        // the layout command is still active. In this case the parent will be set to null. We simply use a conditional branch to filter this.
        if (element.parent != nullptr)
        {
            // X
            if (element.style.rightHasPriority)          // right
            {
                if (element.style.width->Automatic())
                {
                    if (element.HasText())
                    {
                        element.x = element.parent->width - element.textManager.GetTextWidth() - element.GetHorizontalPadding();
                    }
                    else
                    {
                        element.x = element.parent->width - element.GetChildrenWidth() - element.GetHorizontalPadding();
                    }
                }
                else
                {
                    element.x = element.parent->width - element.width;
                }


                if (element.style.right->InPercent())
                {
                    element.x += static_cast<int>(element.GetParentRelativeWidth() * (element.style.right->value / 100.0f));
                }
                else
                {
                    element.x += static_cast<int>(element.style.right->value);
                }


                if (element.style.positionOrigin->value == PositionOrigin_InnerBorder)
                {
                    element.x -= static_cast<int>(element.parent->style.borderRightWidth->value);
                }
                else if (element.style.positionOrigin->value == PositionOrigin_Inner)
                {
                    element.x -= element.parent->GetRightPadding();
                }
            }
            else                                                // left
            {
                if (element.style.left->InPercent())
                {
                    element.x = static_cast<int>(element.GetParentRelativeWidth() * (element.style.left->value / 100.0f));
                }
                else
                {
                    element.x = static_cast<int>(element.style.left->value);
                }


                if (element.style.positionOrigin->value == PositionOrigin_InnerBorder)
                {
                    element.x += static_cast<int>(element.parent->style.borderLeftWidth->value);
                }
                else if (element.style.positionOrigin->value == PositionOrigin_Inner)
                {
                    element.x += element.parent->GetLeftPadding();
                }
            }

            // Y
            if (element.style.bottomHasPriority)         // bottom
            {
                if (element.style.height->Automatic())
                {
                    if (element.HasText())
                    {
                        element.y = element.parent->height - element.textManager.GetTextHeight() - element.GetVerticalPadding();
                    }
                    else
                    {
                        element.y = element.parent->height - element.GetChildrenHeight() - element.GetVerticalPadding();
                    }
                }
                else
                {
                    element.y = element.parent->height - element.height;
                }


                if (element.style.bottom->InPercent())
                {
                    element.y += static_cast<int>(element.GetParentRelativeHeight() * (element.style.bottom->value / 100.0f));
                }
                else
                {
                    element.y += static_cast<int>(element.style.bottom->value);
                }


                if (element.style.positionOrigin->value == PositionOrigin_InnerBorder)
                {
                    element.y -= static_cast<int>(element.parent->style.borderBottomWidth->value);
                }
                else if (element.style.positionOrigin->value == PositionOrigin_Inner)
                {
                    element.y -= element.parent->GetBottomPadding();
                }
            }
            else                                                // top
            {
                if (element.style.top->InPercent())
                {
                    element.y = static_cast<int>(element.GetParentRelativeHeight() * (element.style.top->value / 100.0f));
                }
                else
                {
                    element.y = static_cast<int>(element.style.top->value);
                }


                if (element.style.positionOrigin->value == PositionOrigin_InnerBorder)
                {
                    element.y += static_cast<int>(element.parent->style.borderTopWidth->value);
                }
                else if (element.style.positionOrigin->value == PositionOrigin_Inner)
                {
                    element.y += element.parent->GetTopPadding();
                }
            }
        }
    }

    void GUILayoutManager::ValidatePosition_Absolute(GUIElement &element)
    {
        // X
        if (element.style.rightHasPriority)          // right
        {
            element.x = element.parent->width - element.width;

            if (element.style.right->InPercent())
            {
                element.x += static_cast<int>(element.server.GetViewportWidth() * (element.style.right->value / 100.0f));
            }
            else
            {
                element.x += static_cast<int>(element.style.right->value);
            }
        }
        else                                                // left
        {
            if (element.style.left->InPercent())
            {
                element.x = static_cast<int>(element.server.GetViewportWidth() * (element.style.left->value / 100.0f));
            }
            else
            {
                element.x = static_cast<int>(element.style.left->value);
            }
        }

        // Y
        if (element.style.bottomHasPriority)
        {
            element.y = element.parent->height - element.height;

            if (element.style.bottom->InPercent())
            {
                element.y += static_cast<int>(element.server.GetViewportHeight() * (element.style.bottom->value / 100.0f));
            }
            else
            {
                element.y += static_cast<int>(element.style.bottom->value);
            }
        }
        else                                                // top
        {
            if (element.style.top->InPercent())
            {
                element.y = static_cast<int>(element.server.GetViewportHeight() * (element.style.top->value / 100.0f));
            }
            else
            {
                element.y = static_cast<int>(element.style.top->value);
            }
        }
    }


    void GUILayoutManager::ValidateText(GUIElement &element, bool validateDependants)
    {
        element.UpdateTextManagerLayout();
        element.InvalidateTextRenderingData();

        if (validateDependants)
        {
            if (element.style.width->Automatic())
            {
                this->InvalidateWidth(element);
            }

            if (element.style.height->Automatic() && !element.style.fontSize->InPercent())
            {
                this->InvalidateHeight(element);
            }
        }


        element.layout.flags &= ~TextInvalidated;        
        if (element.layout.flags == 0)
        {
            this->invalidElements.Remove(element.layout.layoutManagerListItem);
            element.layout.layoutManagerListItem = nullptr;
        }
    }


    void GUILayoutManager::MarkSizeAsChanged(GUIElement &element)
    {
        uint32_t flags = ValidatedElement::SizeChanged;

        auto existingElement = this->validatedElements.FindByValue(element);
        if (existingElement != nullptr)
        {
            existingElement->value.flags |= flags;
        }
        else
        {
            this->validatedElements.Insert(ValidatedElement(element, flags));
        }


        this->TryMarkAsTopLevelElement(element);
    }

    void GUILayoutManager::MarkPositionAsChanged(GUIElement &element)
    {
        uint32_t flags = ValidatedElement::PositionChanged;

        auto existingElement = this->validatedElements.FindByValue(element);
        if (existingElement != nullptr)
        {
            existingElement->value.flags |= flags;
        }
        else
        {
            this->validatedElements.Insert(ValidatedElement(element, flags));
        }


        this->TryMarkAsTopLevelElement(element);
    }

    void GUILayoutManager::TryMarkAsTopLevelElement(GUIElement &element)
    {
        // If an ancestor is already in the list, don't do anything.
        bool doesAncestorExist = false;
        for (size_t i = 0; i < this->topLevelValidatedElements.count; ++i)
        {
            auto topLevelElement = this->topLevelValidatedElements[i];
            assert(topLevelElement != nullptr);
            {
                if (element.IsAncestor(*topLevelElement) || topLevelElement == &element)
                {
                    doesAncestorExist = true;
                    break;
                }
            }
        }

        if (!doesAncestorExist || element.style.positioning->value == GUIPositioning_Absolute)
        {
            // We need to add the element to the list. Before we do, though, we want to make sure anything that's already in the list that
            // is a descendant of the element is removed.
            for (size_t i = 0; i < this->topLevelValidatedElements.count;)
            {
                auto topLevelElement = this->topLevelValidatedElements[i];
                assert(topLevelElement != nullptr);
                {
                    if (topLevelElement->IsAncestor(element))
                    {
                        this->topLevelValidatedElements.Remove(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
            }


            // Add the element to the list.
            this->topLevelValidatedElements.PushBack(&element);
        }
    }


    bool GUILayoutManager::IsWidthInvalid(GUIElement &element) const
    {
        return (element.layout.flags & WidthInvalidated) != 0;
    }

    bool GUILayoutManager::IsHeightInvalid(GUIElement &element) const
    {
        return (element.layout.flags & HeightInvalidated) != 0;
    }

    bool GUILayoutManager::IsPositionInvalid(GUIElement &element) const
    {
        return (element.layout.flags & PositionInvalidated) != 0;
    }


    const GUIElement* GUILayoutManager::GetProvokingSibling(GUIElement &element) const
    {
        for (auto i = element.prevSibling; i != nullptr; i = i->prevSibling)
        {
            if (i->style.positioning->value == GUIPositioning_Auto && i->IsVisible())
            {
                return i;
            }
        }

        return nullptr;
    }


    void GUILayoutManager::MarkElementAsUnclipped(GUIElement &element) const
    {
        if (element.isClippedByParent)
        {
            element.isClippedByParent = false;

            assert(element.parent != nullptr);
            {
                element.parent->clippedChildCount -= 1;
            }
        }
    }

    void GUILayoutManager::MarkElementAsClipped(GUIElement &element) const
    {
        if (element.parent != nullptr)
        {
            if (!element.isClippedByParent)
            {
                element.isClippedByParent = true;
                element.parent->clippedChildCount += 1;
            }
        }
    }

    void GUILayoutManager::CheckAndMarkElementAsClipped(GUIElement &element) const
    {
        if (element.parent != nullptr)
        {
            // We only need to clip if the element is not fully contained by the parent.
            bool isClipped = false;

            if (element.style.positioning->value != GUIPositioning_Absolute)       // <-- Absolutely positioned elements are never clipped.
            {
                if (element.style.positioning->value != GUIPositioning_Relative)   // <-- Currently, relatively positioned elements are never clipped.
                {
                    if (element.layout.absoluteX < element.parent->layout.clippingRectInner.left || element.layout.absoluteX + element.width  > element.parent->layout.clippingRectInner.right  ||
                        element.layout.absoluteY < element.parent->layout.clippingRectInner.top  || element.layout.absoluteY + element.height > element.parent->layout.clippingRectInner.bottom ||
                        element.style.backgroundImage->isset)
                    {
                        isClipped = true;
                    }
                }
            }


            if (isClipped)
            {
                this->MarkElementAsClipped(element);
            }
            else
            {
                this->MarkElementAsUnclipped(element);
            }
        }
    }


    void GUILayoutManager::UpdateAbsoluteLayoutProperties(GUIElement &element) const
    {
        if (element.parent != nullptr)
        {
            if (element.style.positioning->value != GUIPositioning_Absolute)
            {
                element.layout.absoluteX = element.parent->layout.absoluteX + element.x;
                element.layout.absoluteY = element.parent->layout.absoluteY + element.y;
            }
            else
            {
                element.layout.absoluteX = element.x;
                element.layout.absoluteY = element.y;
            }
        }

        element.layout.clippingRect.left        = element.layout.absoluteX;
        element.layout.clippingRect.top         = element.layout.absoluteY;
        element.layout.clippingRect.right       = element.layout.clippingRect.left + element.width;
        element.layout.clippingRect.bottom      = element.layout.clippingRect.top  + element.height;

        element.layout.clippingRectInner.left   = element.layout.clippingRect.left   + element.GetLeftPadding();
        element.layout.clippingRectInner.top    = element.layout.clippingRect.top    + element.GetTopPadding();
        element.layout.clippingRectInner.right  = element.layout.clippingRect.right  - element.GetRightPadding();
        element.layout.clippingRectInner.bottom = element.layout.clippingRect.bottom - element.GetBottomPadding();

        if (element.parent != nullptr)
        {
            if (element.style.positioning->value != GUIPositioning_Absolute && element.style.positioning->value != GUIPositioning_Relative)
            {
                element.layout.clippingRect.Clamp(element.parent->layout.clippingRectInner);
                element.layout.clippingRectInner.Clamp(element.layout.clippingRect);
            }

            this->CheckAndMarkElementAsClipped(element);
        }


        // Now we need to iterate over children.
        for (auto child = element.firstChild; child != nullptr; child = child->nextSibling)
        {
            if (child->style.positioning->value != GUIPositioning_Absolute)
            {
                this->UpdateAbsoluteLayoutProperties(*child);
            }
        }
    }


    void GUILayoutManager::PostProcess()
    {
        // We need to iterate over each top level element and recursively update the absolute layout properties of them and their children.
        for (size_t i = 0; i < this->topLevelValidatedElements.count; ++i)
        {
            auto element = this->topLevelValidatedElements[i];
            assert(element != nullptr);
            {
                this->UpdateAbsoluteLayoutProperties(*element);      // <-- Called recursively on children.
            }
        }
        


        // Post Events.
        while (this->validatedElements.root != nullptr)
        {
            auto &iElement = this->validatedElements.root->value;
            {
                auto element = iElement.element;
                auto flags   = iElement.flags;

                assert(element != nullptr);
                {
                    // Post events.
                    if ((flags & ValidatedElement::SizeChanged))
                    {
                        element->GetServer().OnSize(element);
                    }

                    if ((flags & ValidatedElement::PositionChanged))
                    {
                        element->GetServer().OnMove(element);
                    }
                }
            }

            this->validatedElements.RemoveRoot();
        }


        // We need to clear the top level elements.
        this->topLevelValidatedElements.Clear();
    }


    ////////////////////////////////////////////////////////////
    // Static Helpers.
    
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunreachable-code"
#endif

    int GUILayoutManager::EvaluateWidth(const GUIElement &element, const GUIStyleAttribute_Number &width, bool calculateFlexed)
    {
        int result = 0;

        switch (width.format)
        {
        // Treat pixels and points the same in the case of the width (and height).
        case GUIStyleNumberFormat_Pixels:
        case GUIStyleNumberFormat_Points:
            {
                // We just set directly.
                result = static_cast<int>(width.value);
                break;
            }
            
        case GUIStyleNumberFormat_Percent:     // Depends on the parent and non-% siblings.
            {
                // Based on the size of the parent. This will also depend on the relative-width-mode style attribute.
                if (element.parent)
                {
                    float relativeWidth = static_cast<float>(element.GetParentRelativeWidth());
                    float relativeRatio = width.value / 100.0f;

                    if (calculateFlexed && element.parent->style.flexChildWidth->value && element.style.positioning->value == GUIPositioning_Auto)
                    {
                        if (element.parent->style.childPlane->value == GUIPlane_Horizontal)
                        {
                            float totalPercent            = 0.0f;
                            float fixedSiblingsTotalWidth = 0.0f;

                            // This loop will include this element, which actually works nicely.
                            for (auto i = element.parent->firstChild; i != nullptr; i = i->nextSibling)
                            {
                                if (i->style.positioning->value == GUIPositioning_Auto && i->style.visible->value)
                                {
                                    if (i->style.width->InPercent())
                                    {
                                        totalPercent            += i->style.width->value;
                                        fixedSiblingsTotalWidth += i->GetHorizontalMargin();
                                    }
                                    else
                                    {
                                        fixedSiblingsTotalWidth += i->GetOuterWidth();
                                    }
                                }
                            }

                            relativeWidth -= fixedSiblingsTotalWidth;
                        
                            if (totalPercent > 0.0f)
                            {
                                relativeRatio = width.value / totalPercent;
                            }
                            else
                            {
                                relativeRatio = 0.0f;
                            }
                        }
                        else
                        {
                            relativeRatio = 1.0f;
                        }
                    }

                    result = static_cast<int>(relativeWidth * relativeRatio);
                }
                else
                {
                    result = 0;
                }
                
                break;
            }
            

        case GUIStyleNumberFormat_Automatic:   // Depends on the children.
            {
                result = element.GetHorizontalPadding();

                // The width will be the width of the children plus padding.
                if (element.firstChild != nullptr)
                {
                    result += GUILayoutManager::GetChildrenWidth(element);
                }
                else if (element.HasText())
                {
                    result += element.textManager.GetTextWidth();
                }

                break;
            }
            
        // Should never get here.
        default: break;
        }


        return result;
    }

    int GUILayoutManager::EvaluateHeight(const GUIElement &element, const GUIStyleAttribute_Number &height, bool calculateFlexed)
    {
        int result = 0;

        switch (height.format)
        {
        // Treat pixels and points the same in the case of the height (and height).
        case GUIStyleNumberFormat_Pixels:
        case GUIStyleNumberFormat_Points:
            {
                // We just set directly.
                result = static_cast<int>(height.value);
                break;
            }
            
        case GUIStyleNumberFormat_Percent:     // Depends on the parent.
            {
                // Based on the size of the parent. This will also depend on the relative-height-mode style attribute.
                if (element.parent)
                {
                    float relativeHeight = static_cast<float>(element.GetParentRelativeHeight());
                    float relativeRatio  = height.value / 100.0f;

                    if (calculateFlexed && element.parent->style.flexChildHeight->value && element.style.positioning->value == GUIPositioning_Auto)
                    {
                        if (element.parent->style.childPlane->value == GUIPlane_Vertical)
                        {
                            float totalPercent             = 0;
                            float fixedSiblingsTotalHeight = 0;

                            // This loop will include this element, which actually works nicely.
                            for (auto i = element.parent->firstChild; i != nullptr; i = i->nextSibling)
                            {
                                if (i->style.positioning->value == GUIPositioning_Auto && i->style.visible->value)
                                {
                                    if (i->style.height->InPercent())
                                    {
                                        totalPercent             += i->style.height->value;
                                        fixedSiblingsTotalHeight += i->GetVerticalMargin();
                                    }
                                    else
                                    {
                                        fixedSiblingsTotalHeight += i->GetOuterHeight();
                                    }
                                }
                            }

                            relativeHeight -= fixedSiblingsTotalHeight;

                            if (totalPercent > 0.0f)
                            {
                                relativeRatio = height.value / totalPercent;
                            }
                            else
                            {
                                relativeRatio = 0.0f;
                            }
                        }
                        else
                        {
                            relativeRatio = 1.0f;
                        }
                    }

                    result = static_cast<int>(relativeHeight * relativeRatio);
                }
                else
                {
                    result = 0;
                }
                
                break;
            }
            

        case GUIStyleNumberFormat_Automatic:   // Depends on the children.
            {
                result = element.GetVerticalPadding();

                // The height will be the height of the children plus padding.
                if (element.firstChild != nullptr)
                {
                    result += GUILayoutManager::GetChildrenHeight(element);
                }
                else if (element.HasText() || element.style.editableText->value == true)
                {
                    result += element.textManager.GetTextHeight();
                }

                break;
            }
            
        // Should never get here.
        default: break;
        }

        return result;
    }

#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif


    int GUILayoutManager::UpdateWidth(GUIElement &element)
    {
        // Start with margin and padding before the width because these are prerequisites for width evaluation.
        element.layout.marginLeft   = static_cast<unsigned short>(element.style.marginLeft->value);
        element.layout.marginRight  = static_cast<unsigned short>(element.style.marginRight->value);
        element.layout.paddingLeft  = static_cast<unsigned short>(element.style.paddingLeft->value  + element.style.borderLeftWidth->value);
        element.layout.paddingRight = static_cast<unsigned short>(element.style.paddingRight->value + element.style.borderRightWidth->value);


        auto newWidth = GUILayoutManager::EvaluateWidth(element, *element.style.width);

        int minWidth = 0;
        int maxWidth = newWidth;
        
        if (!element.style.minWidth->Automatic())
        {
            minWidth = GUILayoutManager::EvaluateWidth(element, *element.style.minWidth, false);
        }
        if (!element.style.maxWidth->Automatic())
        {
            maxWidth = GUILayoutManager::EvaluateWidth(element, *element.style.maxWidth, false);
        }

        
        element.layout.unconstrainedWidth = newWidth;
        element.width = GT::Clamp(newWidth, minWidth, maxWidth);

        return element.GetOuterWidth();
    }

    int GUILayoutManager::UpdateHeight(GUIElement &element)
    {
        // Start with margin and padding before the height because these are prerequisites for height evaluation.
        element.layout.marginTop     = static_cast<unsigned short>(element.style.marginTop->value);
        element.layout.marginBottom  = static_cast<unsigned short>(element.style.marginBottom->value);
        element.layout.paddingTop    = static_cast<unsigned short>(element.style.paddingTop->value    + element.style.borderTopWidth->value);
        element.layout.paddingBottom = static_cast<unsigned short>(element.style.paddingBottom->value + element.style.borderBottomWidth->value);


        auto newHeight = GUILayoutManager::EvaluateHeight(element, *element.style.height);

        int minHeight = 0;
        int maxHeight = newHeight;
        
        if (!element.style.minHeight->Automatic())
        {
            minHeight = GUILayoutManager::EvaluateHeight(element, *element.style.minHeight, false);
        }
        if (!element.style.maxHeight->Automatic())
        {
            maxHeight = GUILayoutManager::EvaluateHeight(element, *element.style.maxHeight, false);
        }

        
        element.layout.unconstrainedHeight = newHeight;
        element.height = GT::Clamp(newHeight, minHeight, maxHeight);

        return element.GetOuterHeight();
    }

    int GUILayoutManager::GetChildrenWidth(const GUIElement &element)
    {
        int result = 0;

        if (element.style.childPlane->value == GUIPlane_Horizontal)
        {
            for (auto i = element.firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    if (i->style.width->Automatic() && (i->style.minWidth->InPercent() || i->style.maxWidth->InPercent()))
                    {
                        result += (i->layout.unconstrainedWidth + i->GetHorizontalMargin());
                    }
                    else
                    {
                        result += (i->width + i->GetHorizontalMargin());
                    }
                }
            }
        }
        else // vertical
        {
            for (auto i = element.firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    int iWidth;
                    if (i->style.width->Automatic() && (i->style.minWidth->InPercent() || i->style.maxWidth->InPercent()))
                    {
                        iWidth = (i->layout.unconstrainedWidth + i->GetHorizontalMargin());
                    }
                    else
                    {
                        iWidth = (i->width + i->GetHorizontalMargin());
                    }

                    result = GT::Max(result, iWidth);
                }
            }
        }

        return result;
    }

    int GUILayoutManager::GetChildrenHeight(const GUIElement &element)
    {
        int result = 0;

        if (element.style.childPlane->value == GUIPlane_Vertical)
        {
            for (auto i = element.firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    if (i->style.height->Automatic() && (i->style.minHeight->InPercent() || i->style.maxHeight->InPercent()))
                    {
                        result += (i->layout.unconstrainedHeight + i->GetVerticalMargin());
                    }
                    else
                    {
                        result += (i->height + i->GetVerticalMargin());
                    }
                }
            }
        }
        else // horizontal
        {
            for (auto i = element.firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    int iHeight;
                    if (i->style.height->Automatic() && (i->style.minHeight->InPercent() || i->style.maxHeight->InPercent()))
                    {
                        iHeight = (i->layout.unconstrainedHeight + i->GetVerticalMargin());
                    }
                    else
                    {
                        iHeight = (i->height + i->GetVerticalMargin());
                    }

                    result = GT::Max(result, iHeight);
                }
            }
        }

        return result;
    }
}
