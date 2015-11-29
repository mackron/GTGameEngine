// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/Element.hpp>
#include <GTEngine/GUI/Server.hpp>
#include <GTEngine/Core/Strings/Create.hpp>
#include <GTEngine/Core/Strings/Tokenizer.hpp>
#include <GTEngine/Core/ToString.hpp>
#include <GTEngine/Core/Math.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GTGUI
{
    Element::Element(const char* id, Server &server)
        : id(GTLib::Strings::Create(id)), server(server), parent(nullptr), firstChild(nullptr), lastChild(nullptr), prevSibling(nullptr), nextSibling(nullptr),
          style(*this), m_primaryStyleClass(nullptr),
          eventHandlers(),
          x(0), y(0), width(0), height(0),
          layout(),
          clippedChildCount(0),
          textManager(nullptr),
          textManagerEventHandler(*this),
          renderingData(server.GetGarbageCollector()),
          isTextRenderingDataValid(false), isShadowRenderingDataValid(false), isBorderRenderingDataValid(false), isBackgroundRenderingDataValid(false),
          isHandlingOnDraw(false),
          isClippedByParent(false),
          bst()
    {
        this->textManager.SetEventHandler(this->textManagerEventHandler);
        
        this->bst.hashedID = GTLib::Hash(id);
    }
    
    Element::~Element()
    {
        // Since no other elements will be using this element's primary style class, it can be deleted.
        this->DeletePrimaryStyleClass();


        // We need to ensure the font is unacquired.
        //this->server.UnacquireFont(this);
        this->server.UnacquireFont(this->GetFont());

        // Important we do this last because some stuff might need to reference it during this destructor.
        GTLib::Strings::Delete(this->id);
    }
    
    void Element::AppendChild(Element &child)
    {
        // A child can only have one parent - it needs to be orphaned from it's existing parent, if it has one.
        if (child.parent)
        {
            auto prevParent = child.parent;
            prevParent->RemoveChild(child);
        }

        // If we don't have children, this will be the first one...
        if (this->firstChild == nullptr)
        {
            this->firstChild = &child;
        }

        // If we already have children, this child needs to be appended to the end of the children list.
        if (this->lastChild != nullptr)
        {
            auto secondLastChild = this->lastChild;

            secondLastChild->nextSibling = &child;
            child.prevSibling = secondLastChild;
        }


        // The new item is always the last one.
        child.nextSibling = nullptr;
        this->lastChild = &child;


        // Can't forget to let the child know who it's new parent is.
        child.parent = this;


        // The child needs to have it's opacity updated.
        child.UpdateAbsoluteOpacity();


        // Our layout needs to be updated.
        this->server.OnChildAttached(*this);
    }

    void Element::PrependChild(Element &child)
    {
        // A child can only have one parent - it needs to be orphaned from it's existing parent, if it has one.
        if (child.parent != nullptr)
        {
            auto prevParent = child.parent;
            prevParent->RemoveChild(child);
        }


        // If we don't have children, this will be the first one...
        if (this->lastChild == nullptr)
        {
            this->lastChild = &child;
        }

        // If we already have children, this child needs to be prepended to the start of the children list.
        if (this->firstChild != nullptr)
        {
            auto secondChild = this->firstChild;

            secondChild->prevSibling = &child;
            child.nextSibling        = secondChild;
        }

        // The new item is always the first one.
        child.prevSibling = nullptr;
        this->firstChild  = &child;


        // Can't forget to let the child know who it's new parent is.
        child.parent = this;


        // The child needs to have it's opacity updated.
        child.UpdateAbsoluteOpacity();


        // Our layout needs to be updated.
        this->server.OnChildAttached(*this);
    }

    
    void Element::RemoveChild(Element &child, bool invalidateParentLayout, bool updateScript)
    {
        if (child.parent == this)       // Don't do anything if this element is not the child's parent.
        {
            if (child.prevSibling)
            {
                child.prevSibling->nextSibling = child.nextSibling;
            }
            else
            {
                // The child is the root.
                this->firstChild = child.nextSibling;
            }
            
            if (child.nextSibling)
            {
                child.nextSibling->prevSibling = child.prevSibling;
            }
            else
            {
                // The child is the end.
                this->lastChild = child.prevSibling;
            }
            
            // The child needs to be orphaned.
            child.parent      = nullptr;
            child.prevSibling = nullptr;
            child.nextSibling = nullptr;


            // The child needs to be marked as unclipped.
            if (child.isClippedByParent)
            {
                this->clippedChildCount -= 1;
            }


            // The child needs to have it's opacity updated.
            child.UpdateAbsoluteOpacity();


            // The element's previous parent needs to have it's layout updated since it now has different content. Note how we don't
            // bother updating the child element since while it does not have a parent, it will never be displayed or interacted with.
            // Instead, the child will have it's layout updated when it is re-added to another element.
            if (invalidateParentLayout)
            {
                this->server.OnChildDetached(*this);
            }


            // We need to let the script know that the child has been detached, if applicable.
            if (updateScript)
            {
                this->server.GetScriptServer().RemoveChild(*this, child);
            }
        }
    }

    void Element::RemoveAllChildren()
    {
        while (this->firstChild != nullptr)
        {
            this->RemoveChild(*this->firstChild, false, false);
        }

        this->server.OnChildDetached(*this);
        this->server.GetScriptServer().RemoveAllChildren(*this);
    }

    void Element::DeleteAllChildren()
    {
        while (this->firstChild != nullptr)
        {
            this->server.DeleteElement(this->firstChild, true, false);
        }

        this->server.OnChildDetached(*this);
    }

    bool Element::IsAncestor(Element &ancestor)
    {
        if (this->parent != nullptr)
        {
            if (this->parent == &ancestor)
            {
                return true;
            }

            // If we've made it here, we need to check the parent.
            return this->parent->IsAncestor(ancestor);
        }

        return false;
    }

    bool Element::IsChild(Element &child)
    {
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            if (i == &child)
            {
                return true;
            }

            if (i->IsChild(child))
            {
                return true;
            }
        }

        return false;
    }
    
    void Element::AttachStyleClass(StyleClass &styleClass, bool refresh)
    {
        // This method is actually just a helper/convenience method. In order to avoid cyclic includes, we need to keep track
        // of every include that has already been included.
        GTLib::Vector<StyleClass*> alreadyAttached;
        this->AttachStyleClass(styleClass, alreadyAttached, refresh);
    }

    void Element::AttachStyleClass(const char* styleClassName, bool refresh)
    {
        this->AttachStyleClass(this->server.GetStyleServer().GetStyleClass(styleClassName), refresh);
    }

    
    void Element::DetachStyleClass(StyleClass &styleClass, bool refresh)
    {
        // This method works just like AttachStyleClass(), only we detach instead of attach.
        GTLib::Vector<StyleClass*> alreadyDetached;
        this->DetachStyleClass(styleClass, alreadyDetached, refresh);
    }

    void Element::DetachStyleClass(const char* styleClassName, bool refresh)
    {
        this->DetachStyleClass(this->server.GetStyleServer().GetStyleClass(styleClassName), refresh);
    }


    StyleClass* Element::GetPrimaryStyleClass() const
    {
        if (m_primaryStyleClass == nullptr)
        {
            GTLib::Strings::List<char> className;
            className.Append("#");
            className.Append(this->id);

            m_primaryStyleClass = this->server.GetStyleServer().GetStyleClass(className.c_str());
        }

        return m_primaryStyleClass;
    }

    void Element::DeletePrimaryStyleClass()
    {
        auto primaryStyleClass = this->GetPrimaryStyleClass();
        if (primaryStyleClass != nullptr)
        {
            this->GetServer().GetStyleServer().DeleteStyleClass(*primaryStyleClass, false);      // Last arguments means to not refresh the style stack. Setting this to true will crash.
            m_primaryStyleClass = nullptr;
        }
    }

    void Element::SetStyleAttribute(const char* name, const char* value)
    {
        auto sc = this->GetPrimaryStyleClass();
        if (sc != nullptr)
        {
            sc->SetAttribute(name, value);
        }
    }

    void Element::AttachEventHandler(ElementEventHandler &eventHandler)
    {
        if (this->eventHandlers.Find(&eventHandler) == nullptr)
        {
            this->eventHandlers.Append(&eventHandler);

            if (eventHandler.ImplementsOnDraw())
            {
                this->isHandlingOnDraw = true;
            }
        }
    }

    void Element::DetachEventHandler(ElementEventHandler &eventHandler)
    {
        this->eventHandlers.Remove(this->eventHandlers.Find(&eventHandler));

        // We need to check if we are still handling OnDraw.
        if (this->isHandlingOnDraw == true)
        {
            this->isHandlingOnDraw = false;
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                if (i->value->ImplementsOnDraw())
                {
                    this->isHandlingOnDraw = true;
                }
            }
        }
    }

    void Element::DetachAllEventHandlers()
    {
        this->eventHandlers.Clear();
        this->isHandlingOnDraw = false;
    }

    bool Element::IsVisible() const
    {
        if (!this->style.visible->value)
        {
            return false;
        }

        // If the parent is not visible, neither is this one.
        if (this->parent != nullptr)
        {
            return this->parent->IsVisible();
        }

        return true;
    }

    bool Element::DoesSizeAffectParent() const
    {
        // Size won't affect the parent if this element is marked as invisible. Do NOT use IsVisible() here because
        // that will do a hierarchial traversal, which we don't want.

        if (this->style.visible->value)
        {
            // Relative and absolute elements do not affect the parent. We may add a style to control this, but we don't need it yet.
            return this->style.positioning->value == Positioning_Auto;
        }

        return false;
    }
    
    
    int Element::GetRelativePosX() const
    {
        return this->x;
    }
    
    int Element::GetRelativePosY() const
    {
        return this->y;
    }
    
    void Element::GetRelativePosition(int &relativePosXOut, int &relativePosYOut) const
    {
        relativePosXOut = this->x;
        relativePosYOut = this->y;
    }
    
    
    int Element::GetAbsolutePosX() const
    {
        return this->layout.absoluteX;
    }
    
    int Element::GetAbsolutePosY() const
    {
        return this->layout.absoluteY;
    }
    
    void Element::GetAbsolutePosition(int &absolutePosXOut, int &absolutePosYOut) const
    {
        absolutePosXOut = this->layout.absoluteX;
        absolutePosYOut = this->layout.absoluteY;
    }
    

    int Element::GetWidth() const
    {
        return this->width;
    }

    int Element::GetHeight() const
    {
        return this->height;
    }
    
    int Element::GetInnerWidth() const
    {
        int padding = this->GetHorizontalPadding();
        if (this->width > padding)
        {
            return this->width - padding;
        }
        
        return 0;
    }
    int Element::GetInnerHeight() const
    {
        int padding = this->GetVerticalPadding();
        if (this->height > padding)
        {
            return this->height - padding;
        }

        return 0;
    }

    int Element::GetOuterWidth() const
    {
        return this->width + this->GetHorizontalMargin();
    }
    int Element::GetOuterHeight() const
    {
        return this->height + this->GetVerticalMargin();
    }


    int Element::GetChildrenWidth() const
    {
        int result = 0;

        if (this->style.childPlane->value == Plane_Horizontal)
        {
            for (Element *i = this->firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    result += i->GetOuterWidth();
                }
            }
        }
        else // vertical
        {
            for (Element *i = this->firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    result = GTLib::Max(result, i->GetOuterWidth());
                }
            }
        }

        return result;
    }

    int Element::GetChildrenHeight() const
    {
        int result = 0;

        if (this->style.childPlane->value == Plane_Vertical)
        {
            for (Element *i = this->firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    result += i->GetOuterHeight();
                }
            }
        }
        else // horizontal
        {
            for (Element *i = this->firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    result = GTLib::Max(result, i->GetOuterHeight());
                }
            }
        }

        return result;
    }


    int Element::GetLeftMargin() const
    {
        return static_cast<int>(this->layout.marginLeft);
    }
    int Element::GetRightMargin() const
    {
        return static_cast<int>(this->layout.marginRight);
    }
    int Element::GetTopMargin() const
    {
        return static_cast<int>(this->layout.marginTop);
    }
    int Element::GetBottomMargin() const
    {
        return static_cast<int>(this->layout.marginBottom);
    }

    int Element::GetHorizontalMargin() const
    {
        return this->GetLeftMargin() + this->GetRightMargin();
    }
    int Element::GetVerticalMargin() const
    {
        return this->GetTopMargin() + this->GetBottomMargin();
    }


    int Element::GetLeftPadding() const
    {
        return static_cast<int>(this->layout.paddingLeft);
    }
    int Element::GetRightPadding() const
    {
        return static_cast<int>(this->layout.paddingRight);
    }
    int Element::GetTopPadding() const
    {
        return static_cast<int>(this->layout.paddingTop);
    }
    int Element::GetBottomPadding() const
    {
        return static_cast<int>(this->layout.paddingBottom);
    }

    int Element::GetHorizontalPadding() const
    {
        return this->GetLeftPadding() + this->GetRightPadding();
    }
    int Element::GetVerticalPadding() const
    {
        return this->GetTopPadding() + this->GetBottomPadding();
    }

    int Element::GetInnerLeftEdge() const
    {
        return static_cast<int>(this->style.paddingLeft->value + this->style.borderLeftWidth->value);
    }
    int Element::GetInnerRightEdge() const
    {
        return this->width - static_cast<int>(this->style.paddingRight->value + this->style.borderRightWidth->value);
    }
    int Element::GetInnerTopEdge() const
    {
        return static_cast<int>(this->style.paddingTop->value + this->style.borderTopWidth->value);
    }
    int Element::GetInnerBottomEdge() const
    {
        return this->height - static_cast<int>(this->style.paddingBottom->value + this->style.borderBottomWidth->value);
    }

    int Element::GetCenterX() const
    {
        return this->GetLeftPadding() + (this->GetInnerWidth() / 2);
    }
    int Element::GetCenterY() const
    {
        return this->GetTopPadding() + (this->GetInnerHeight() / 2);
    }



    void Element::GetAbsoluteRect(GTLib::Rect<int> &rect) const
    {
        GTLib::Rect<int> parentRect;
        if (this->parent != nullptr)
        {
            this->parent->GetAbsoluteRect(parentRect);
        }

        rect.left   = parentRect.left + this->x;
        rect.top    = parentRect.top  + this->y;
        rect.right  = rect.left + this->width;
        rect.bottom = rect.top  + this->height;
    }

    void Element::GetAbsoluteInnerRect(GTLib::Rect<int> &rect) const
    {
        this->GetAbsoluteRect(rect);

        int leftPadding = this->GetLeftPadding();
        int topPadding  = this->GetTopPadding();

        rect.left   += leftPadding;
        rect.right  += leftPadding;
        rect.top    += topPadding;
        rect.bottom += topPadding;
    }


    int Element::GetInnerXOffset() const
    {
        return static_cast<int>(this->style.innerOffsetX->value);
    }

    int Element::GetInnerYOffset() const
    {
        return static_cast<int>(this->style.innerOffsetY->value);
    }


    int Element::GetZIndex() const
    {
        // If the z-index is 'auto', it depends on the parent.
        if (this->style.zIndex->format == StyleNumberFormat_Automatic)
        {
            if (this->parent != nullptr)
            {
                return this->parent->GetZIndex();
            }

            return 0;
        }

        return static_cast<int>(this->style.zIndex->value);
    }

    bool Element::UsesZIndex() const
    {
        return this->style.positioning->value != Positioning_Auto;
    }

    const char * Element::GetText() const
    {
        if (this->HasText())
        {
            return this->textManager.GetText();
        }

        return nullptr;
    }

    void Element::SetText(const char* text, bool blockEvent)
    {
        if (this->GetFont() == nullptr)
        {
            this->UpdateFontFromStyle(true);        // <-- 'true' means to block the OnTextChanged event from getting fired (it'll be fired later).
        }

        // Setting the text in the text manager will call its event handler, which in turn will call this->OnTextChanged(). Thus, there
        // is no need to manually call this->OnTextChanged().
        this->textManager.SetText(text, blockEvent);

        // If we're blocking the event, some stuff won't have been called on the server, so we'll need to do that now.
        if (blockEvent)
        {
            this->server.OnTextChanged(*this, blockEvent);

            if (this->server.GetCaret().GetOwner() == this)
            {
                this->server.PositionCaret(false, false);
            }
        }
    }
    
    void Element::SetFont(GTLib::Font* font)
    {
        this->textManager.SetDefaultFont(font);
    }
    
    
    void Element::UpdateFontFromStyle(bool blockOnTextChangedEvent)
    {
        GTLib::FontInfo fi;
        this->server.GetFontInfoFromElement(*this, fi);
        
        auto font = this->server.AcquireFont(fi);
        if (font != nullptr)
        {
            auto prevFont = this->GetFont();
            if (font != prevFont)
            {
                this->SetFont(font);
            
                // If this element owns the text cursor, it size will need to be updated.
                if (this->HasEditableText() && this->IsFocused())
                {
                    this->server.GetCaret().SetSize(1, font->GetLineHeight());
                }
            
            
                // Now that the font has changed, the size of the element may have also changed. We just pretend the text has changed.
                if (!blockOnTextChangedEvent)
                {
                    this->server.OnTextChanged(*this);
                }


                // The old font needs to be unacquired.
                this->server.UnacquireFont(prevFont);
            }
        }
    }


    void Element::GetTextRect(GTLib::Rect<int> &rect) const
    {
        // We retrieve the text rectangle based on the text rect from the text manager. After retrieving, we just offset by the top/left padding.
        GTLib::Rect<int> textRect;
        this->textManager.GetTextRect(textRect);

        int paddingLeft = this->GetLeftPadding();
        int paddingTop  = this->GetTopPadding();

        rect.left   = textRect.left   + paddingLeft;
        rect.right  = textRect.right  + paddingLeft;
        rect.top    = textRect.top    + paddingTop;
        rect.bottom = textRect.bottom + paddingTop;
    }

    void Element::SelectAllText()
    {
        this->textManager.SelectAll();

        if (this->server.GetFocusedElement() == this)
        {
            this->textManager.MoveCursorToEnd();
            this->server.PositionCaret(true, true);
        }

        this->InvalidateTextRenderingData();
    }

    void Element::GoToLine(unsigned int lineNumber)
    {
        this->textManager.MoveCursorToLine(lineNumber - 1, 0);
        this->server.PositionCaret(false, true);
    }


    void Element::FilterChildrenByPositioning(GTLib::List<Element *> &automatic, GTLib::List<Element *> &relative, GTLib::List<Element *> &absolute, bool ignoreHidden)
    {
        for (Element *i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            // Ignore hidden, if applicable.
            if (ignoreHidden && !i->IsVisible())
            {
                continue;
            }

            if (i->style.positioning->value == Positioning_Relative)
            {
                relative.Append(i);
            }
            else if (i->style.positioning->value == Positioning_Absolute)
            {
                absolute.Append(i);
            }
            else
            {
                automatic.Append(i);
            }
        }
    }


    void Element::GetAncestors(GTLib::List<Element *> &ancestors)
    {
        if (this->parent != nullptr)
        {
            ancestors.Append(this->parent);
            this->parent->GetAncestors(ancestors);
        }
    }

    void Element::GetRects(const GTLib::Rect<int> &parentRect, const GTLib::Rect<int> &parentScissorRect, GTLib::Rect<int> &rect, GTLib::Rect<int> &scissorRect, GTLib::Rect<int> &childrenScissorRect) const
    {
        if (this->style.positioning->value == Positioning_Auto)
        {
            // The position is relative to the parent.
            rect.left   = parentRect.left + this->x;
            rect.top    = parentRect.top  + this->y;
            rect.right  = rect.left       + this->width;
            rect.bottom = rect.top        + this->height;

            scissorRect = rect;
            scissorRect.Clamp(parentScissorRect);
        }
        else if (this->style.positioning->value == Positioning_Relative)
        {
            // The position is relative to the parent.
            rect.left   = parentRect.left + this->x;
            rect.top    = parentRect.top  + this->y;
            rect.right  = rect.left       + this->width;
            rect.bottom = rect.top        + this->height;

            scissorRect = rect;
        }
        else
        {
            // The position is absolute.
            rect.left   = this->x;
            rect.top    = this->y;
            rect.right  = rect.left + this->width;
            rect.bottom = rect.top  + this->height;

            scissorRect = rect;
        }


        // The children's scissor rectangle needs to be pulled in based on the padding and offset.
        childrenScissorRect = rect;

        childrenScissorRect.left   += this->GetLeftPadding();
        childrenScissorRect.right  -= this->GetRightPadding();
        childrenScissorRect.top    += this->GetTopPadding();
        childrenScissorRect.bottom -= this->GetBottomPadding();

        childrenScissorRect.Clamp(scissorRect);
    }

    void Element::AbsoluteToRelative(int &xInOut, int &yInOut)
    {
        GTLib::Rect<int> absRect;
        this->GetAbsoluteRect(absRect);

        xInOut -= absRect.left;
        yInOut -= absRect.top;
    }

    void Element::UpdateTextManagerContainerSize()
    {
        this->textManager.SetContainerSize(this->GetInnerWidth(), this->GetInnerHeight());
    }

    void Element::UpdateTextManagerContainerOffset()
    {
        this->textManager.SetContainerOffset(static_cast<int>(this->style.innerOffsetX->value), static_cast<int>(this->style.innerOffsetY->value));
    }

    void Element::UpdateTextManagerAlignment()
    {
        GTLib::Alignment horizontalAlign;
        GTLib::Alignment verticalAlign;

        if (this->style.horizontalAlign->value == Align_Right)
        {
            horizontalAlign = GTLib::Alignment_Right;
        }
        else if (this->style.horizontalAlign->value == Align_Center)
        {
            horizontalAlign = GTLib::Alignment_Center;
        }
        else
        {
            horizontalAlign = GTLib::Alignment_Left;
        }

        if (this->style.verticalAlign->value == Align_Bottom)
        {
            verticalAlign = GTLib::Alignment_Bottom;
        }
        else if (this->style.verticalAlign->value == Align_Center)
        {
            verticalAlign = GTLib::Alignment_Center;
        }
        else
        {
            verticalAlign = GTLib::Alignment_Top;
        }

        this->textManager.SetHorizontalAlign(horizontalAlign);
        this->textManager.SetVerticalAlign(verticalAlign);
    }

    void Element::UpdateTextManagerLayout()
    {
        this->UpdateTextManagerContainerSize();
        this->UpdateTextManagerContainerOffset();
        this->UpdateTextManagerAlignment();

        if (this->server.GetCaret().GetOwner() == this)
        {
            this->server.PositionCaret(true, false);
        }
    }

    bool Element::HasText() const
    {
        return this->textManager.HasText();
    }

    bool Element::HasEditableText() const
    {
        return this->style.editableText->value;
    }


    void Element::ValidateTextRenderingData()
    {
        if (!this->isTextRenderingDataValid)
        {
            auto &textColour                = this->style.textColour->value;
            auto &selectionBackgroundColour = (this->server.GetFocusedElement() == this) ? this->style.textSelectionBackgroundColour->value : this->style.textSelectionBackgroundColourBlurred->value;

            this->textManager.SetDefaultTextColour(textColour.r, textColour.g, textColour.b);
            this->textManager.SetSelectionBackgroundColour(selectionBackgroundColour.r, selectionBackgroundColour.g, selectionBackgroundColour.b);


            this->renderingData.UpdateTextMeshes(*this);
            this->isTextRenderingDataValid = true;
        }
    }

    void Element::InvalidateTextRenderingData()
    {
        this->isTextRenderingDataValid = false;
    }



    void Element::ValidateShadowRenderingData()
    {
        if (!this->isShadowRenderingDataValid)
        {
            this->renderingData.UpdateShadowMesh(*this);
            this->isShadowRenderingDataValid = true;
        }
    }

    void Element::InvalidateShadowRenderingData()
    {
        this->isShadowRenderingDataValid = false;
    }



    void Element::ValidateBorderRenderingData()
    {
        if (!this->isBorderRenderingDataValid)
        {
            this->renderingData.UpdateBorderMesh(*this);
            this->isBorderRenderingDataValid = true;
        }
    }

    void Element::InvalidateBorderRenderingData()
    {
        this->isBorderRenderingDataValid = false;
    }


    void Element::ValidateBackgroundRenderingData()
    {
        if (!this->isBackgroundRenderingDataValid)
        {
            this->renderingData.UpdateBackgroundMeshes(*this);
            this->isBackgroundRenderingDataValid = true;
        }
    }

    void Element::InvalidateBackgroundRenderingData()
    {
        this->isBackgroundRenderingDataValid = false;
    }



    void Element::Show()
    {
        if (!this->style.visible->value)    // Do not use this->IsVisible() here.
        {
            // The style is set on the elements primary style.
            auto styleClass = this->GetPrimaryStyleClass();
            if (styleClass != nullptr)
            {
                styleClass->SetAttribute("visible", "true");
            }
        }
    }

    void Element::Hide()
    {
        if (this->style.visible->value)     // Do not use this->IsVisible() here.
        {
            // The style is set on the elements primary style.
            auto styleClass = this->GetPrimaryStyleClass();
            if (styleClass != nullptr)
            {
                styleClass->SetAttribute("visible", "false");
            }
        }
    }


    void Element::SetPosition(int xIn, int yIn)
    {
        auto styleClass = this->GetPrimaryStyleClass();
        if (styleClass != nullptr)
        {
            // First we need to make sure we are positioned relatively.
            if (this->style.positioning->value == Positioning_Auto)
            {
                styleClass->SetAttribute("positioning", "relative");
            }
            
            // Now we set the actual position.
            styleClass->SetAttribute("left", GTLib::ToString(xIn).c_str());
            styleClass->SetAttribute("top",  GTLib::ToString(yIn).c_str());
        }
    }

    void Element::SetSize(unsigned int widthIn, unsigned int heightIn)
    {
        auto styleClass = this->GetPrimaryStyleClass();
        if (styleClass != nullptr)
        {
            styleClass->SetAttribute("width",  GTLib::ToString(widthIn).c_str());
            styleClass->SetAttribute("height", GTLib::ToString(heightIn).c_str());
        }
    }


    float Element::GetLocalOpacity() const
    {
        if (this->style.opacity->InPercent())               // <-- Allow for opacity values such as '50%'.
        {
            return this->style.opacity->value / 100.0f;
        }
        else
        {
            return this->style.opacity->value;
        }
    }

    void Element::SetLocalOpacity(float newOpacity)
    {
        auto styleClass = this->GetPrimaryStyleClass();
        if (styleClass != nullptr)
        {
            styleClass->SetAttribute("opacity", GTLib::ToString(newOpacity).c_str());
        }
    }

    float Element::GetAbsoluteOpacity() const
    {
        return this->renderingData.GetAbsoluteOpacity();
    }

    float Element::CalculateAbsoluteOpacity() const
    {
        // The absolute opacity is based on the parent.
        if (this->parent != nullptr && this->style.compoundOpacity->value == true)
        {
            return GTLib::Clamp(this->parent->GetAbsoluteOpacity() * this->GetLocalOpacity(), 0.0f, 1.0f);
        }

        return GTLib::Clamp(this->GetLocalOpacity(), 0.0f, 1.0f);
    }

    void Element::UpdateAbsoluteOpacity()
    {
        this->InvalidateBackgroundRenderingData();
        this->InvalidateBorderRenderingData();
        this->InvalidateTextRenderingData();
        this->InvalidateShadowRenderingData();
        this->renderingData.SetAbsoluteOpacity(this->CalculateAbsoluteOpacity());

        // Children need to have their opacity updated, too.
        for (auto child = this->firstChild; child != nullptr; child = child->nextSibling)
        {
            if (child->style.compoundOpacity->value == true)
            {
                child->UpdateAbsoluteOpacity();
            }
        }
    }


    void Element::Enable()
    {
        auto styleClass = this->GetPrimaryStyleClass();
        if (styleClass != nullptr)
        {
            if (!styleClass->enabled.value)
            {
                styleClass->SetAttribute("enabled", "true");
            }
        }
    }

    void Element::Disable()
    {
        auto styleClass = this->GetPrimaryStyleClass();
        if (styleClass != nullptr)
        {
            if (styleClass->enabled.value)
            {
                styleClass->SetAttribute("enabled", "false");
            }
        }
    }

    bool Element::IsEnabled() const
    {
        if (this->parent != nullptr)
        {
            if (!this->parent->IsEnabled())
            {
                return false;
            }
        }

        return this->style.enabled->value;
    }


    float Element::GetAbsoluteShadowOpacity() const
    {
        float opacity = this->GetAbsoluteOpacity();

        if (this->style.shadowOpacity->InPercent())               // <-- Allow for opacity values such as '50%'.
        {
            return opacity * (this->style.shadowOpacity->value / 100.0f);
        }
        else
        {
            return opacity * this->style.shadowOpacity->value;
        }
    }

    void Element::SetBackgroundImage(const char* imageURL)
    {
        auto sc = this->GetPrimaryStyleClass();
        assert(sc != nullptr);
       
        char absoluteURL[EASYVFS_MAX_PATH];
        if (!easyvfs_find_absolute_path(GTEngine::g_EngineContext->GetVFS(), imageURL, absoluteURL, sizeof(absoluteURL)))
        {
            strcpy_s(absoluteURL, sizeof(absoluteURL), imageURL);
        }

        sc->SetAttribute("background-image", absoluteURL);
    }


    bool Element::IsFocused() const
    {
        return this->server.GetFocusedElement() == this;
    }
    


    int Element::GetParentRelativeWidth() const
    {
        assert(this->parent != nullptr);

        if (this->style.relativeWidthMode->value == "outer")
        {
            return this->parent->GetWidth();
        }
        else if (this->style.relativeWidthMode->value == "inner-border")
        {
            return static_cast<int>(this->parent->GetWidth() - this->parent->style.borderLeftWidth->value - this->parent->style.borderRightWidth->value);
        }
        else    // inner by default.
        {
            return this->parent->GetInnerWidth();
        }
    }

    int Element::GetParentRelativeHeight() const
    {
        assert(this->parent != nullptr);

        if (this->style.relativeHeightMode->value == "outer")
        {
            return this->parent->GetHeight();
        }
        else if (this->style.relativeHeightMode->value == "inner-border")
        {
            return static_cast<int>(this->parent->GetHeight() - this->parent->style.borderTopWidth->value - this->parent->style.borderBottomWidth->value);
        }
        else    // inner by default.
        {
            return this->parent->GetInnerHeight();
        }
    }


    Element* Element::GetNextVisibleAutoSibling()
    {
        for (auto i = this->nextSibling; i != nullptr; i = i->nextSibling)
        {
            if (i->style.positioning->value == Positioning_Auto && i->IsVisible())
            {
                return i;
            }
        }

        return nullptr;
    }

    Element* Element::GetPrevVisibleAutoSibling()
    {
        for (auto i = this->prevSibling; i != nullptr; i = i->prevSibling)
        {
            if (i->style.positioning->value == Positioning_Auto && i->IsVisible())
            {
                return i;
            }
        }

        return nullptr;
    }

    Element* Element::GetFirstVisibleAutoChild()
    {
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            if (i->style.positioning->value == Positioning_Auto && i->IsVisible())
            {
                return i;
            }
        }

        return nullptr;
    }


    void Element::OnMouseEnter()
    {
        this->style.ActivateModifierClasses(StyleClassType_Hovered);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnMouseEnter(*this);
        }
    }

    void Element::OnMouseLeave()
    {
        this->style.DeactivateModifierClasses(StyleClassType_Hovered);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnMouseLeave(*this);
        }
    }

    void Element::OnPush()
    {
        this->style.ActivateModifierClasses(StyleClassType_Pushed);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnPush(*this);
        }
    }

    void Element::OnRelease()
    {
        this->style.DeactivateModifierClasses(StyleClassType_Pushed);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnRelease(*this);
        }
    }

    void Element::OnPressed()
    {
        this->style.DeactivateModifierClasses(StyleClassType_Pushed);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnPressed(*this);
        }
    }
}

// --- Private ---
namespace GTGUI
{
    void Element::AttachStyleClass(StyleClass &styleClass, GTLib::Vector<StyleClass*> &alreadyAttached, bool refresh)
    {
        assert(alreadyAttached.Exists(&styleClass) == false);
        {
            alreadyAttached.PushBack(&styleClass);

            GTLib::Strings::Tokenizer include(styleClass.includes);
            while (include)
            {
                // We only attach the include if it hasn't already been included.
                auto includeStyleClass = this->server.GetStyleServer().GetStyleClass(include.start, include.end - include.start);

                if (includeStyleClass != nullptr && !alreadyAttached.Exists(includeStyleClass))
                {
                    this->AttachStyleClass(*includeStyleClass, alreadyAttached, false);     // <-- 'false' = no refresh (done later).
                }

                ++include;
            }

            this->style.Attach(styleClass, refresh);
        }
    }

    void Element::DetachStyleClass(StyleClass &styleClass, GTLib::Vector<StyleClass*> &alreadyDetached, bool refresh)
    {
        assert(alreadyDetached.Exists(&styleClass) == false);
        {
            alreadyDetached.PushBack(&styleClass);

            GTLib::Strings::Tokenizer include(styleClass.includes);
            while (include)
            {
                // We only attach the include if it hasn't already been included.
                auto includeStyleClass = this->server.GetStyleServer().GetStyleClass(include.start, include.end - include.start);

                if (includeStyleClass != nullptr && !alreadyDetached.Exists(includeStyleClass))
                {
                    this->DetachStyleClass(*includeStyleClass, alreadyDetached, false);     // <-- 'false' = no refresh (done later).
                }

                ++include;
            }

            this->style.Detach(styleClass, refresh);
        }
    }
}


/////////////////////////////////////////////////
// TextEventHandler

namespace GTGUI
{
    void Element::TextEventHandler::OnTextChanged(const char*)
    {
        // If we don't yet have a font, we'll want one.
        if (element.GetFont() == nullptr)
        {
            element.UpdateFontFromStyle();
        }
        
        
        element.server.OnTextChanged(element);
        element.OnTextChanged();
        

        if (element.server.GetCaret().GetOwner() == &element)
        {
            element.server.PositionCaret(false, false);
        }
    }
}
