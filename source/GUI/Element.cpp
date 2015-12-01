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
    GUIElement::GUIElement(const char* id, GUIServer &server)
        : id(GT::Strings::Create(id)), server(server), parent(nullptr), firstChild(nullptr), lastChild(nullptr), prevSibling(nullptr), nextSibling(nullptr),
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
        
        this->bst.hashedID = GT::Hash(id);
    }
    
    GUIElement::~GUIElement()
    {
        // Since no other elements will be using this element's primary style class, it can be deleted.
        this->DeletePrimaryStyleClass();


        // We need to ensure the font is unacquired.
        //this->server.UnacquireFont(this);
        this->server.UnacquireFont(this->GetFont());

        // Important we do this last because some stuff might need to reference it during this destructor.
        GT::Strings::Delete(this->id);
    }
    
    void GUIElement::AppendChild(GUIElement &child)
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

    void GUIElement::PrependChild(GUIElement &child)
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

    
    void GUIElement::RemoveChild(GUIElement &child, bool invalidateParentLayout, bool updateScript)
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

    void GUIElement::RemoveAllChildren()
    {
        while (this->firstChild != nullptr)
        {
            this->RemoveChild(*this->firstChild, false, false);
        }

        this->server.OnChildDetached(*this);
        this->server.GetScriptServer().RemoveAllChildren(*this);
    }

    void GUIElement::DeleteAllChildren()
    {
        while (this->firstChild != nullptr)
        {
            this->server.DeleteElement(this->firstChild, true, false);
        }

        this->server.OnChildDetached(*this);
    }

    bool GUIElement::IsAncestor(GUIElement &ancestor)
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

    bool GUIElement::IsChild(GUIElement &child)
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
    
    void GUIElement::AttachStyleClass(GUIStyleClass &styleClass, bool refresh)
    {
        // This method is actually just a helper/convenience method. In order to avoid cyclic includes, we need to keep track
        // of every include that has already been included.
        GT::Vector<GUIStyleClass*> alreadyAttached;
        this->AttachStyleClass(styleClass, alreadyAttached, refresh);
    }

    void GUIElement::AttachStyleClass(const char* styleClassName, bool refresh)
    {
        this->AttachStyleClass(this->server.GetStyleServer().GetStyleClass(styleClassName), refresh);
    }

    
    void GUIElement::DetachStyleClass(GUIStyleClass &styleClass, bool refresh)
    {
        // This method works just like AttachStyleClass(), only we detach instead of attach.
        GT::Vector<GUIStyleClass*> alreadyDetached;
        this->DetachStyleClass(styleClass, alreadyDetached, refresh);
    }

    void GUIElement::DetachStyleClass(const char* styleClassName, bool refresh)
    {
        this->DetachStyleClass(this->server.GetStyleServer().GetStyleClass(styleClassName), refresh);
    }


    GUIStyleClass* GUIElement::GetPrimaryStyleClass() const
    {
        if (m_primaryStyleClass == nullptr)
        {
            GT::Strings::List<char> className;
            className.Append("#");
            className.Append(this->id);

            m_primaryStyleClass = this->server.GetStyleServer().GetStyleClass(className.c_str());
        }

        return m_primaryStyleClass;
    }

    void GUIElement::DeletePrimaryStyleClass()
    {
        auto primaryStyleClass = this->GetPrimaryStyleClass();
        if (primaryStyleClass != nullptr)
        {
            this->GetServer().GetStyleServer().DeleteStyleClass(*primaryStyleClass, false);      // Last arguments means to not refresh the style stack. Setting this to true will crash.
            m_primaryStyleClass = nullptr;
        }
    }

    void GUIElement::SetGUIStyleAttribute(const char* name, const char* value)
    {
        auto sc = this->GetPrimaryStyleClass();
        if (sc != nullptr)
        {
            sc->SetAttribute(name, value);
        }
    }

    void GUIElement::AttachEventHandler(GUIElementEventHandler &eventHandler)
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

    void GUIElement::DetachEventHandler(GUIElementEventHandler &eventHandler)
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

    void GUIElement::DetachAllEventHandlers()
    {
        this->eventHandlers.Clear();
        this->isHandlingOnDraw = false;
    }

    bool GUIElement::IsVisible() const
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

    bool GUIElement::DoesSizeAffectParent() const
    {
        // Size won't affect the parent if this element is marked as invisible. Do NOT use IsVisible() here because
        // that will do a hierarchial traversal, which we don't want.

        if (this->style.visible->value)
        {
            // Relative and absolute elements do not affect the parent. We may add a style to control this, but we don't need it yet.
            return this->style.positioning->value == GUIPositioning_Auto;
        }

        return false;
    }
    
    
    int GUIElement::GetRelativePosX() const
    {
        return this->x;
    }
    
    int GUIElement::GetRelativePosY() const
    {
        return this->y;
    }
    
    void GUIElement::GetRelativePosition(int &relativePosXOut, int &relativePosYOut) const
    {
        relativePosXOut = this->x;
        relativePosYOut = this->y;
    }
    
    
    int GUIElement::GetAbsolutePosX() const
    {
        return this->layout.absoluteX;
    }
    
    int GUIElement::GetAbsolutePosY() const
    {
        return this->layout.absoluteY;
    }
    
    void GUIElement::GetAbsolutePosition(int &absolutePosXOut, int &absolutePosYOut) const
    {
        absolutePosXOut = this->layout.absoluteX;
        absolutePosYOut = this->layout.absoluteY;
    }
    

    int GUIElement::GetWidth() const
    {
        return this->width;
    }

    int GUIElement::GetHeight() const
    {
        return this->height;
    }
    
    int GUIElement::GetInnerWidth() const
    {
        int padding = this->GetHorizontalPadding();
        if (this->width > padding)
        {
            return this->width - padding;
        }
        
        return 0;
    }
    int GUIElement::GetInnerHeight() const
    {
        int padding = this->GetVerticalPadding();
        if (this->height > padding)
        {
            return this->height - padding;
        }

        return 0;
    }

    int GUIElement::GetOuterWidth() const
    {
        return this->width + this->GetHorizontalMargin();
    }
    int GUIElement::GetOuterHeight() const
    {
        return this->height + this->GetVerticalMargin();
    }


    int GUIElement::GetChildrenWidth() const
    {
        int result = 0;

        if (this->style.childPlane->value == GUIPlane_Horizontal)
        {
            for (GUIElement *i = this->firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    result += i->GetOuterWidth();
                }
            }
        }
        else // vertical
        {
            for (GUIElement *i = this->firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    result = GT::Max(result, i->GetOuterWidth());
                }
            }
        }

        return result;
    }

    int GUIElement::GetChildrenHeight() const
    {
        int result = 0;

        if (this->style.childPlane->value == GUIPlane_Vertical)
        {
            for (GUIElement *i = this->firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    result += i->GetOuterHeight();
                }
            }
        }
        else // horizontal
        {
            for (GUIElement *i = this->firstChild; i != nullptr; i = i->nextSibling)
            {
                if (i->DoesSizeAffectParent())
                {
                    result = GT::Max(result, i->GetOuterHeight());
                }
            }
        }

        return result;
    }


    int GUIElement::GetLeftMargin() const
    {
        return static_cast<int>(this->layout.marginLeft);
    }
    int GUIElement::GetRightMargin() const
    {
        return static_cast<int>(this->layout.marginRight);
    }
    int GUIElement::GetTopMargin() const
    {
        return static_cast<int>(this->layout.marginTop);
    }
    int GUIElement::GetBottomMargin() const
    {
        return static_cast<int>(this->layout.marginBottom);
    }

    int GUIElement::GetHorizontalMargin() const
    {
        return this->GetLeftMargin() + this->GetRightMargin();
    }
    int GUIElement::GetVerticalMargin() const
    {
        return this->GetTopMargin() + this->GetBottomMargin();
    }


    int GUIElement::GetLeftPadding() const
    {
        return static_cast<int>(this->layout.paddingLeft);
    }
    int GUIElement::GetRightPadding() const
    {
        return static_cast<int>(this->layout.paddingRight);
    }
    int GUIElement::GetTopPadding() const
    {
        return static_cast<int>(this->layout.paddingTop);
    }
    int GUIElement::GetBottomPadding() const
    {
        return static_cast<int>(this->layout.paddingBottom);
    }

    int GUIElement::GetHorizontalPadding() const
    {
        return this->GetLeftPadding() + this->GetRightPadding();
    }
    int GUIElement::GetVerticalPadding() const
    {
        return this->GetTopPadding() + this->GetBottomPadding();
    }

    int GUIElement::GetInnerLeftEdge() const
    {
        return static_cast<int>(this->style.paddingLeft->value + this->style.borderLeftWidth->value);
    }
    int GUIElement::GetInnerRightEdge() const
    {
        return this->width - static_cast<int>(this->style.paddingRight->value + this->style.borderRightWidth->value);
    }
    int GUIElement::GetInnerTopEdge() const
    {
        return static_cast<int>(this->style.paddingTop->value + this->style.borderTopWidth->value);
    }
    int GUIElement::GetInnerBottomEdge() const
    {
        return this->height - static_cast<int>(this->style.paddingBottom->value + this->style.borderBottomWidth->value);
    }

    int GUIElement::GetCenterX() const
    {
        return this->GetLeftPadding() + (this->GetInnerWidth() / 2);
    }
    int GUIElement::GetCenterY() const
    {
        return this->GetTopPadding() + (this->GetInnerHeight() / 2);
    }



    void GUIElement::GetAbsoluteRect(GT::Rect<int> &rect) const
    {
        GT::Rect<int> parentRect;
        if (this->parent != nullptr)
        {
            this->parent->GetAbsoluteRect(parentRect);
        }

        rect.left   = parentRect.left + this->x;
        rect.top    = parentRect.top  + this->y;
        rect.right  = rect.left + this->width;
        rect.bottom = rect.top  + this->height;
    }

    void GUIElement::GetAbsoluteInnerRect(GT::Rect<int> &rect) const
    {
        this->GetAbsoluteRect(rect);

        int leftPadding = this->GetLeftPadding();
        int topPadding  = this->GetTopPadding();

        rect.left   += leftPadding;
        rect.right  += leftPadding;
        rect.top    += topPadding;
        rect.bottom += topPadding;
    }


    int GUIElement::GetInnerXOffset() const
    {
        return static_cast<int>(this->style.innerOffsetX->value);
    }

    int GUIElement::GetInnerYOffset() const
    {
        return static_cast<int>(this->style.innerOffsetY->value);
    }


    int GUIElement::GetZIndex() const
    {
        // If the z-index is 'auto', it depends on the parent.
        if (this->style.zIndex->format == GUIStyleNumberFormat_Automatic)
        {
            if (this->parent != nullptr)
            {
                return this->parent->GetZIndex();
            }

            return 0;
        }

        return static_cast<int>(this->style.zIndex->value);
    }

    bool GUIElement::UsesZIndex() const
    {
        return this->style.positioning->value != GUIPositioning_Auto;
    }

    const char * GUIElement::GetText() const
    {
        if (this->HasText())
        {
            return this->textManager.GetText();
        }

        return nullptr;
    }

    void GUIElement::SetText(const char* text, bool blockEvent)
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
    
    void GUIElement::SetFont(GT::Font* font)
    {
        this->textManager.SetDefaultFont(font);
    }
    
    
    void GUIElement::UpdateFontFromStyle(bool blockOnTextChangedEvent)
    {
        GT::FontInfo fi;
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


    void GUIElement::GetTextRect(GT::Rect<int> &rect) const
    {
        // We retrieve the text rectangle based on the text rect from the text manager. After retrieving, we just offset by the top/left padding.
        GT::Rect<int> textRect;
        this->textManager.GetTextRect(textRect);

        int paddingLeft = this->GetLeftPadding();
        int paddingTop  = this->GetTopPadding();

        rect.left   = textRect.left   + paddingLeft;
        rect.right  = textRect.right  + paddingLeft;
        rect.top    = textRect.top    + paddingTop;
        rect.bottom = textRect.bottom + paddingTop;
    }

    void GUIElement::SelectAllText()
    {
        this->textManager.SelectAll();

        if (this->server.GetFocusedElement() == this)
        {
            this->textManager.MoveCursorToEnd();
            this->server.PositionCaret(true, true);
        }

        this->InvalidateTextRenderingData();
    }

    void GUIElement::GoToLine(unsigned int lineNumber)
    {
        this->textManager.MoveCursorToLine(lineNumber - 1, 0);
        this->server.PositionCaret(false, true);
    }


    void GUIElement::FilterChildrenByPositioning(GT::List<GUIElement *> &automatic, GT::List<GUIElement *> &relative, GT::List<GUIElement *> &absolute, bool ignoreHidden)
    {
        for (GUIElement *i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            // Ignore hidden, if applicable.
            if (ignoreHidden && !i->IsVisible())
            {
                continue;
            }

            if (i->style.positioning->value == GUIPositioning_Relative)
            {
                relative.Append(i);
            }
            else if (i->style.positioning->value == GUIPositioning_Absolute)
            {
                absolute.Append(i);
            }
            else
            {
                automatic.Append(i);
            }
        }
    }


    void GUIElement::GetAncestors(GT::List<GUIElement *> &ancestors)
    {
        if (this->parent != nullptr)
        {
            ancestors.Append(this->parent);
            this->parent->GetAncestors(ancestors);
        }
    }

    void GUIElement::GetRects(const GT::Rect<int> &parentRect, const GT::Rect<int> &parentScissorRect, GT::Rect<int> &rect, GT::Rect<int> &scissorRect, GT::Rect<int> &childrenScissorRect) const
    {
        if (this->style.positioning->value == GUIPositioning_Auto)
        {
            // The position is relative to the parent.
            rect.left   = parentRect.left + this->x;
            rect.top    = parentRect.top  + this->y;
            rect.right  = rect.left       + this->width;
            rect.bottom = rect.top        + this->height;

            scissorRect = rect;
            scissorRect.Clamp(parentScissorRect);
        }
        else if (this->style.positioning->value == GUIPositioning_Relative)
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

    void GUIElement::AbsoluteToRelative(int &xInOut, int &yInOut)
    {
        GT::Rect<int> absRect;
        this->GetAbsoluteRect(absRect);

        xInOut -= absRect.left;
        yInOut -= absRect.top;
    }

    void GUIElement::UpdateTextManagerContainerSize()
    {
        this->textManager.SetContainerSize(this->GetInnerWidth(), this->GetInnerHeight());
    }

    void GUIElement::UpdateTextManagerContainerOffset()
    {
        this->textManager.SetContainerOffset(static_cast<int>(this->style.innerOffsetX->value), static_cast<int>(this->style.innerOffsetY->value));
    }

    void GUIElement::UpdateTextManagerAlignment()
    {
        GT::Alignment horizontalAlign;
        GT::Alignment verticalAlign;

        if (this->style.horizontalAlign->value == GUIAlign_Right)
        {
            horizontalAlign = GT::Alignment_Right;
        }
        else if (this->style.horizontalAlign->value == GUIAlign_Center)
        {
            horizontalAlign = GT::Alignment_Center;
        }
        else
        {
            horizontalAlign = GT::Alignment_Left;
        }

        if (this->style.verticalAlign->value == GUIAlign_Bottom)
        {
            verticalAlign = GT::Alignment_Bottom;
        }
        else if (this->style.verticalAlign->value == GUIAlign_Center)
        {
            verticalAlign = GT::Alignment_Center;
        }
        else
        {
            verticalAlign = GT::Alignment_Top;
        }

        this->textManager.SetHorizontalAlign(horizontalAlign);
        this->textManager.SetVerticalAlign(verticalAlign);
    }

    void GUIElement::UpdateTextManagerLayout()
    {
        this->UpdateTextManagerContainerSize();
        this->UpdateTextManagerContainerOffset();
        this->UpdateTextManagerAlignment();

        if (this->server.GetCaret().GetOwner() == this)
        {
            this->server.PositionCaret(true, false);
        }
    }

    bool GUIElement::HasText() const
    {
        return this->textManager.HasText();
    }

    bool GUIElement::HasEditableText() const
    {
        return this->style.editableText->value;
    }


    void GUIElement::ValidateTextRenderingData()
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

    void GUIElement::InvalidateTextRenderingData()
    {
        this->isTextRenderingDataValid = false;
    }



    void GUIElement::ValidateShadowRenderingData()
    {
        if (!this->isShadowRenderingDataValid)
        {
            this->renderingData.UpdateShadowMesh(*this);
            this->isShadowRenderingDataValid = true;
        }
    }

    void GUIElement::InvalidateShadowRenderingData()
    {
        this->isShadowRenderingDataValid = false;
    }



    void GUIElement::ValidateBorderRenderingData()
    {
        if (!this->isBorderRenderingDataValid)
        {
            this->renderingData.UpdateBorderMesh(*this);
            this->isBorderRenderingDataValid = true;
        }
    }

    void GUIElement::InvalidateBorderRenderingData()
    {
        this->isBorderRenderingDataValid = false;
    }


    void GUIElement::ValidateBackgroundRenderingData()
    {
        if (!this->isBackgroundRenderingDataValid)
        {
            this->renderingData.UpdateBackgroundMeshes(*this);
            this->isBackgroundRenderingDataValid = true;
        }
    }

    void GUIElement::InvalidateBackgroundRenderingData()
    {
        this->isBackgroundRenderingDataValid = false;
    }



    void GUIElement::Show()
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

    void GUIElement::Hide()
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


    void GUIElement::SetPosition(int xIn, int yIn)
    {
        auto styleClass = this->GetPrimaryStyleClass();
        if (styleClass != nullptr)
        {
            // First we need to make sure we are positioned relatively.
            if (this->style.positioning->value == GUIPositioning_Auto)
            {
                styleClass->SetAttribute("positioning", "relative");
            }
            
            // Now we set the actual position.
            styleClass->SetAttribute("left", GT::ToString(xIn).c_str());
            styleClass->SetAttribute("top",  GT::ToString(yIn).c_str());
        }
    }

    void GUIElement::SetSize(unsigned int widthIn, unsigned int heightIn)
    {
        auto styleClass = this->GetPrimaryStyleClass();
        if (styleClass != nullptr)
        {
            styleClass->SetAttribute("width",  GT::ToString(widthIn).c_str());
            styleClass->SetAttribute("height", GT::ToString(heightIn).c_str());
        }
    }


    float GUIElement::GetLocalOpacity() const
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

    void GUIElement::SetLocalOpacity(float newOpacity)
    {
        auto styleClass = this->GetPrimaryStyleClass();
        if (styleClass != nullptr)
        {
            styleClass->SetAttribute("opacity", GT::ToString(newOpacity).c_str());
        }
    }

    float GUIElement::GetAbsoluteOpacity() const
    {
        return this->renderingData.GetAbsoluteOpacity();
    }

    float GUIElement::CalculateAbsoluteOpacity() const
    {
        // The absolute opacity is based on the parent.
        if (this->parent != nullptr && this->style.compoundOpacity->value == true)
        {
            return GT::Clamp(this->parent->GetAbsoluteOpacity() * this->GetLocalOpacity(), 0.0f, 1.0f);
        }

        return GT::Clamp(this->GetLocalOpacity(), 0.0f, 1.0f);
    }

    void GUIElement::UpdateAbsoluteOpacity()
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


    void GUIElement::Enable()
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

    void GUIElement::Disable()
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

    bool GUIElement::IsEnabled() const
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


    float GUIElement::GetAbsoluteShadowOpacity() const
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

    void GUIElement::SetBackgroundImage(const char* imageURL)
    {
        auto sc = this->GetPrimaryStyleClass();
        assert(sc != nullptr);
       
        char absoluteURL[EASYVFS_MAX_PATH];
        if (!easyvfs_find_absolute_path(GT::g_EngineContext->GetVFS(), imageURL, absoluteURL, sizeof(absoluteURL)))
        {
            strcpy_s(absoluteURL, sizeof(absoluteURL), imageURL);
        }

        sc->SetAttribute("background-image", absoluteURL);
    }


    bool GUIElement::IsFocused() const
    {
        return this->server.GetFocusedElement() == this;
    }
    


    int GUIElement::GetParentRelativeWidth() const
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

    int GUIElement::GetParentRelativeHeight() const
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


    GUIElement* GUIElement::GetNextVisibleAutoSibling()
    {
        for (auto i = this->nextSibling; i != nullptr; i = i->nextSibling)
        {
            if (i->style.positioning->value == GUIPositioning_Auto && i->IsVisible())
            {
                return i;
            }
        }

        return nullptr;
    }

    GUIElement* GUIElement::GetPrevVisibleAutoSibling()
    {
        for (auto i = this->prevSibling; i != nullptr; i = i->prevSibling)
        {
            if (i->style.positioning->value == GUIPositioning_Auto && i->IsVisible())
            {
                return i;
            }
        }

        return nullptr;
    }

    GUIElement* GUIElement::GetFirstVisibleAutoChild()
    {
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            if (i->style.positioning->value == GUIPositioning_Auto && i->IsVisible())
            {
                return i;
            }
        }

        return nullptr;
    }


    void GUIElement::OnMouseEnter()
    {
        this->style.ActivateModifierClasses(GUIStyleClassType_Hovered);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnMouseEnter(*this);
        }
    }

    void GUIElement::OnMouseLeave()
    {
        this->style.DeactivateModifierClasses(GUIStyleClassType_Hovered);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnMouseLeave(*this);
        }
    }

    void GUIElement::OnPush()
    {
        this->style.ActivateModifierClasses(GUIStyleClassType_Pushed);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnPush(*this);
        }
    }

    void GUIElement::OnRelease()
    {
        this->style.DeactivateModifierClasses(GUIStyleClassType_Pushed);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnRelease(*this);
        }
    }

    void GUIElement::OnPressed()
    {
        this->style.DeactivateModifierClasses(GUIStyleClassType_Pushed);

        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnPressed(*this);
        }
    }
}

// --- Private ---
namespace GTGUI
{
    void GUIElement::AttachStyleClass(GUIStyleClass &styleClass, GT::Vector<GUIStyleClass*> &alreadyAttached, bool refresh)
    {
        assert(alreadyAttached.Exists(&styleClass) == false);
        {
            alreadyAttached.PushBack(&styleClass);

            GT::Strings::Tokenizer include(styleClass.includes);
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

    void GUIElement::DetachStyleClass(GUIStyleClass &styleClass, GT::Vector<GUIStyleClass*> &alreadyDetached, bool refresh)
    {
        assert(alreadyDetached.Exists(&styleClass) == false);
        {
            alreadyDetached.PushBack(&styleClass);

            GT::Strings::Tokenizer include(styleClass.includes);
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
    void GUIElement::TextEventHandler::OnTextChanged(const char*)
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
