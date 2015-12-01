// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIElement
#define GT_GUIElement

#include "StyleStack.hpp"
#include "ElementEventHandler.hpp"
#include "Rendering/GUIElementRenderingData.hpp"

#include <GTEngine/Core/Rect.hpp>
#include <GTEngine/Core/Threading.hpp>
#include <GTEngine/Core/List.hpp>
#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/TextManager.hpp>
#include <bitset>

namespace GTGUI
{
    class GUIServer;

    /**
    *   \brief  Class representing a basic element.
    */
    class GUIElement
    {
    public:
    
        /**
        *   \brief  Constructor.
        */
        GUIElement(const char *id, GUIServer &server);
        
        /**
        *   \brief  Destructor.
        */
        ~GUIElement();

        /**
        *   \brief  Retrieves a reference to the server that owns this element.
        */
        GUIServer & GetServer() { return this->server; };
        
        
        /// Retrieves the ID of the element.
        const char* GetID() const { return this->id; }
        

        /// Appends an element as a child.
        ///
        /// @param child [in] A reference to the child being appended.
        void AppendChild(GUIElement &child);

        /// Prepends an element as a child.
        ///
        /// @param child [in] A reference ot the child being prepended.
        void PrependChild(GUIElement &child);


        /// Removes a child, but does not delete it.
        ///
        /// @param child                  [in] A reference to the child to remove.
        /// @param invalidateParentLayout [in] Whether or not the layout of the parent should be invalidated.
        /// @param updateScript           [in] Whether or not the scripting environment should be updated.
        ///
        /// @remarks
        ///     This will not delete the child.
        void RemoveChild(GUIElement &child, bool invalidateParentLayout = true, bool updateScript = true);

        /// Removes every child, but does not delete them.
        void RemoveAllChildren();


        /// Deletes every child.
        void DeleteAllChildren();


        /**
        *   \brief             Determines if the given element is an ancestor of this element.
        *   \param  other [in] The other element to check ancestory.
        */
        bool IsAncestor(GUIElement &ancestor);

        /**
        *   \brief             Determines if the given element is a child of this element.
        *   \param  other [in] The child element.
        *
        *   \remarks
        *       This does a deep recursive search.
        */
        bool IsChild(GUIElement &child);

        
        /**
        *   \brief                  Attaches a style class to the element, making that style class the priority class.
        *   \param  styleClass [in] A pointer to the style class being added.
        */
        void AttachStyleClass(GUIStyleClass &styleClass, bool refresh = true);
        void AttachStyleClass(GUIStyleClass* styleClass, bool refresh = true) { if (styleClass != nullptr) this->AttachStyleClass(*styleClass, refresh); }
        void AttachStyleClass(const char* styleClass, bool refresh = true);
        
        /**
        *   \brief                  Removes a style class from the element.
        *   \param  styleClass [in] A pointer ot the style class being detached.
        */
        void DetachStyleClass(GUIStyleClass &styleClass, bool refresh = true);
        void DetachStyleClass(GUIStyleClass* styleClass, bool refresh = true) { if (styleClass != nullptr) this->DetachStyleClass(*styleClass, refresh); }
        void DetachStyleClass(const char* styleClass, bool refresh = true);

        /**
        *   \brief  Retrieves a pointer to this element's primary style class.
        */
        GUIStyleClass* GetPrimaryStyleClass() const;

        /// Deletes the primary style class.
        ///
        /// @remarks
        ///     This should only ever be called by the garbage collector and destructor.
        void DeletePrimaryStyleClass();


        /// Sets the value of a style attribute.
        ///
        /// @param name  [in] The name of the attribute being set.
        /// @param value [in] The new value of the attribute.
        void SetGUIStyleAttribute(const char* name, const char* value);


        /**
        *   \brief  Attacheds an event handler to the element.
        */
        void AttachEventHandler(GUIElementEventHandler &eventHandler);

        /**
        *   \brief  Detaches an event handler from the element.
        */
        void DetachEventHandler(GUIElementEventHandler &eventHandler);

        /// Detaches every event handler.
        void DetachAllEventHandlers();


        /**
        *   \brief  Determines whether or not the element is visible. This is determined by the element's style.
        */
        bool IsVisible() const;

        /**
        *   \brief  Determines whether or not the size of this elements affects it's parent.
        */
        bool DoesSizeAffectParent() const;
        
        
        /// Retrieves the relative X position of the element.
        int GetRelativePosX() const;
        
        /// Retrieves the relative y position of the element.
        int GetRelativePosY() const;
        
        /// Retrieves the relative position of the element.
        void GetRelativePosition(int &relativePosXOut, int &relativePosYOut) const;
        
        
        /// Retrieves the absolute X position of the element.
        int GetAbsolutePosX() const;
        
        /// Retrieves the absolute Y position of the element.
        int GetAbsolutePosY() const;
        
        /// Retrieves the absolute position of the element.
        void GetAbsolutePosition(int &absolutePosXOut, int &absolutePosYOut) const;

    
        /**
        *   \brief  Retrieves the width of the element, from border to border. Does not include margins, but includes padding.
        */
        int GetWidth() const;

        /**
        *   \brief  Retrieves the height of the element, from border to border. Does not include margins, but includes padding.
        */
        int GetHeight() const;
    
        /**
        *   \brief  Calculates the inner width of the element based on the normal width and padding.
        */
        int GetInnerWidth() const;
        
        /**
        *   \brief  Calculates the inner height of the element based on the normal height and padding.
        */
        int GetInnerHeight() const;

        /// Calculates the outer width of the element.
        ///
        /// @return The width of the element + margins.
        int GetOuterWidth() const;

        /// Calculates the outer height of the element.
        ///
        /// @return The height of the element + margins.
        int GetOuterHeight() const;


        /**
        *   \brief  Retrieves the width of all the children of this element.
        *
        *   \remarks
        *       This will not include children whose size does not affect the parent, which will include invisible elements.
        */
        int GetChildrenWidth() const;

        /**
        *   \brief  Retrieves the height of all the children of this element.
        *
        *   \remarks
        *       This will not include children whose size does not affect the parent, which will include invisible elements.
        */
        int GetChildrenHeight() const;


        /// Retrieves the size of the left margin.
        int GetLeftMargin() const;

        /// Retrieves the size of the right margin.
        int GetRightMargin() const;

        /// Retrieves the size of the top margin.
        int GetTopMargin() const;

        /// Retrieves the size of the bottom margin.
        int GetBottomMargin() const;


        /// Calculates the total horizontal margin (left + right).
        int GetHorizontalMargin() const;
        
        /// Calculates the total vertical margin (top + bottom).
        int GetVerticalMargin() const;


        /**
        *   \brief  Retrieves the amount of padding on the left edge including the padding-left and border-left-width style attributes.
        */
        int GetLeftPadding() const;

        /**
        *   \brief  Retrieves the amount of padding on the right edge including the padding-right and border-right-width style attributes.
        */
        int GetRightPadding() const;

        /**
        *   \brief  Retrieves the amount of padding on the top edge including the padding-top and border-top-width style attributes.
        */
        int GetTopPadding() const;

        /**
        *   \brief  Retrieves the amount of padding on the bottom edge including the padding-bottom and border-bottom-width style attributes.
        */
        int GetBottomPadding() const;

        /**
        *   \brief  Calculates the total horizontal padding.
        */
        int GetHorizontalPadding() const;
        
        /**
        *   \brief  Calculates the total vertical padding.
        */
        int GetVerticalPadding() const;


        /**
        *   \brief  Retrieves the X position of the inner left edge, taking padding and borders into account.
        */
        int GetInnerLeftEdge() const;

        /**
        *   \brief  Retrieves the X position of the inner right edge, taking padding and borders into account.
        */
        int GetInnerRightEdge() const;

        /**
        *   \brief  Retrieves the Y position of the inner top edge, taking padding and borders into account.
        */
        int GetInnerTopEdge() const;

        /**
        *   \brief  Retrieves the X position of the inner bottom edge, taking padding and borders into account.
        */
        int GetInnerBottomEdge() const;

        /**
        *   \brief  Retrieves the center point on the X axis taking padding and borders into account.
        */
        int GetCenterX() const;

        /**
        *   \brief  Retrieves the center point on the X axis, taking padding and borders into account.
        */
        int GetCenterY() const;
    

        /// Retrieves the rectangle area of the element in absolute coordinates. Useful for rendering time...
        void GetAbsoluteRect(GT::Rect<int> &rect) const;

        /// Retrieves the inner rectangle area of the element in absolute coordinates.
        void GetAbsoluteInnerRect(GT::Rect<int> &rect) const;


        /// Retrieves the X offset of the element.
        int GetInnerXOffset() const;

        /// Retrieves the Y offset of the element.
        int GetInnerYOffset() const;


        /**
        *   \brief  Retrieves the element's z-index.
        */
        int GetZIndex() const;

        /**
        *   \brief  Determines whether or not the element uses the z-index property.
        */
        bool UsesZIndex() const;


        /**
        *   \brief  Retrieves a null-terminated pointer to the element's text. Returns null if the element does not have text.
        *
        *   \remarks
        *       Due to some limitations with XML, an element can not display text if it also has children. Each block of text
        *       must be placed in it's own div.
        */
        const char* GetText() const;

        /**
        *   \brief  Sets the element's text.
        *
        *   \remarks
        *       Text will not be displayed if the element has children.
        */
        void SetText(const char* text, bool blockEvent = false);


        /**
        *   \brief  Retrieves a pointer to the element's font.
        */
        GT::Font* GetFont() const { return this->textManager.GetDefaultFont(); }

        /**
        *   \brief  Sets the element's font. This should only be called by FontManager.
        */
        void SetFont(GT::Font* font);
        
        
        /// Updates the font of the element based on the current style of the element.
        ///
        /// @param blockOnTextChangedEvent [in] Whether or not the OnTextChanged event should be prevent from being called.
        void UpdateFontFromStyle(bool blockOnTextChangedEvent = false);


        /**
        *   \brief  Retrieves the rectangle area containing the text, relative to the element.
        */
        void GetTextRect(GT::Rect<int> &rect) const;

        /// Selects all of the text and moves the text cursor to the end.
        ///
        /// @remarks
        ///     This will do nothing if the element does not have editable text.
        void SelectAllText();

        /// Moves the cursor and scrolls to the position of the given line.
        ///
        /// @remarks
        ///     This will do nothing if the element does not have editable text.
        void GoToLine(unsigned int lineNumber);



        /**
        *   \brief                    Filters the children of this node based on their width positioning.
        *   \param  automatic    [in] A reference to the list that will receive pointers to the 'auto' positioned elements.
        *   \param  relative     [in] A reference to the list that will receive pointers to the 'relative' positioned elements.
        *   \param  absolute     [in] A reference to the list that will receive pointers to the 'absolute' positioned elements.
        *   \param  ignoreHidden [in] Whether or not hidden elements should be omitted from the results.
        */
        void FilterChildrenByPositioning(GT::List<GUIElement *> &automatic, GT::List<GUIElement *> &relative, GT::List<GUIElement *> &absolute, bool ignoreHidden = false);
        
        /**
        *   \brief  Retrieves the chain of ancestors of this element.
        */
        void GetAncestors(GT::List<GUIElement *> &ancestors);


        /**
        *   \brief  Retrieves rectangles that can be used for rendering and input handling. These depend on the rectangles of the parent.
        */
        void GetRects(const GT::Rect<int> &parentRect, const GT::Rect<int> &parentScissorRect, GT::Rect<int> &rect, GT::Rect<int> &scissorRect, GT::Rect<int> &childrenScissorRect) const;


        /**
        *   \brief  Locks style refreshing.
        */
        void LockStyleRefreshing() { this->style.LockRefresh(); }
        
        /**
        *   \brief  Unlocks style refreshing.
        */
        void UnlockStyleRefreshing() { this->style.UnlockRefresh(); }

        /**
        *   \brief  Refreshes the elements style.
        */
        void RefreshStyle() { this->style.Refresh(); }


        /**
        *   \brief              Converts an absolute point to relative based on this element.
        *   \param  x [in, out] The x coordinate to be converted.
        *   \param  y [in, out] The y coordinate to be converted.
        */
        void AbsoluteToRelative(int &x, int &y);


        /// Updates the text manager's container size.
        void UpdateTextManagerContainerSize();

        /// Updates the text manager's container offset.
        void UpdateTextManagerContainerOffset();

        /// Updates the text manager's alignment.
        void UpdateTextManagerAlignment();

        /// Updates the layout of the text manager.
        void UpdateTextManagerLayout();


        /// Determines whether or not the element has text.
        bool HasText() const;

        /// Determines whether or not the element has editable text.
        bool HasEditableText() const;



        /// Updates the text rendering data.
        void ValidateTextRenderingData();

        /// Marks the text rendering data as invalid, causing it to be updated during the next step.
        void InvalidateTextRenderingData();


        /// Updates the shadow rendering data.
        void ValidateShadowRenderingData();

        /// Marks the shadow rendering data as invalid, causing it to be updated during the next step.
        void InvalidateShadowRenderingData();


        /// Updates the border rendering data.
        void ValidateBorderRenderingData();

        /// Marks the border rendering data as invalid, which will cause it to be updated during the next step.
        void InvalidateBorderRenderingData();


        /// Updates the background rendering data.
        void ValidateBackgroundRenderingData();

        /// Marks the background rendering data as invalid, which will cause it to be updated during the next step.
        void InvalidateBackgroundRenderingData();

        
        /// Helper for showing the element.
        void Show();

        /// Helper for hiding the element.
        void Hide();


        /**
        *   \brief  Helper for setting the position of the element using relative coordinates.
        *
        *   \remarks
        *       If the element is an 'auto' positioned element, it will be changed to relative. If it is absolute, it remains the same.
        *       \par
        *       The 'left' and 'top' style attributes will be used for positioning.
        */
        void SetPosition(int x, int y);

        /// Helper method for setting the size of the element using the 'width' and 'height' style attributes.
        ///
        /// @param width  [in] The new width, in pixels.
        /// @param height [in] The new height, in pixels.
        void SetSize(unsigned int width, unsigned int height);


        /// Retrieves the elements local opacity. The return value will be in the range of 0..1, where 0 is completely transparent and 1 is opaque.
        float GetLocalOpacity() const;

        /// Sets the elements local opacity.
        ///
        /// @param newOpacity [in] The element's new local opacity. Should be in the range of 0 (transparent) and 1 (opaque).
        ///
        /// @remarks
        ///     <opacity> will be clamped to 0..1
        ///     @par
        ///     The opacity will be set on the element's primary style class.
        void SetLocalOpacity(float newOpacity);

        /// Retrieves the element's absolute level of opacity, taking the parents into account.
        float GetAbsoluteOpacity() const;

        /// Recursively calculates the absolute level of opacity.
        float CalculateAbsoluteOpacity() const;

        /// Recursively updates the absolute opacity of the element.
        void UpdateAbsoluteOpacity();


        /// Enables the element.
        void Enable();

        /// Disables the element.
        void Disable();

        /// Determines whether or not the element is enabled.
        ///
        /// @return
        ///     True if the element is enabled; false otherwise.
        ///     @par
        ///     This function uses the current style stack in determining whether or not the element is enabled. It will return false
        ///     if any ancestors are disabled.
        bool IsEnabled() const;


        /// Retrieves the absolute shadow opacity. This value is adjusted based on the element's regular transparency.
        float GetAbsoluteShadowOpacity() const;


        /// Sets the background image to use with the element.
        ///
        /// @param imageURL [in] The URL of the image to use for the background of the element.
        ///
        /// @remarks
        ///     <imageURL> will be converted to an absolute string, so passing a relative URL is fine.
        void SetBackgroundImage(const char* imageURL);


        /// Determines whether or not OnDraw is handled by at least one event handler.
        bool IsHandlingOnDraw() const { return this->isHandlingOnDraw; }


        /// Determines whether or not the element is focused.
        ///
        /// @return True if the element is focused; false otherwise.
        bool IsFocused() const;
        

    // Layout stuff.
    public:

        /// Retrieves the width of the parent element for use in relative operations.
        int GetParentRelativeWidth() const;

        /// Retrieves the height of the parent element for use in relative operations.
        int GetParentRelativeHeight() const;


        /// Retrieves the next 'auto' positioned sibling.
        GUIElement* GetNextVisibleAutoSibling();

        /// Retrieves the previous 'auto' positioned sibling.
        GUIElement* GetPrevVisibleAutoSibling();

        /// Retrieves the first visible 'auto' child. This is useful when needing to invalidate the positions of all 'auto' positioned children.
        GUIElement* GetFirstVisibleAutoChild();


    // Event stuff.
    public:

        /// Called by GUIServer when the mouse enters the element.
        void OnMouseEnter();

        /// Called by GUIServer when the mouse leaves.
        void OnMouseLeave();

        /// Called by GUIServer when the element is pushed.
        void OnPush();

        /// Called by GUIServer when the element has gone from a pushed to released state.
        void OnRelease();

        /// Called by GUIServer when an element has been clicked, and then released without the mouse moving off the element.
        void OnPressed();


    // Event handler helpers.
    //
    // The loops here are a little strange in that they check that the root event handler is not null at every iteration. The reason is that if an
    // event handler deletes the element, we'll end up in a bad loop. When an element is deleted, the event handlers are all removed, and thus the
    // root element will be set to null.
    public:

        /// OnSize.
        void OnSize()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnSize(*this);
            }
        }

        /// OnMove
        void OnMove()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnMove(*this);
            }
        }


        /// OnDraw.
        void OnDraw()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnDraw(*this);
            }
        }

        /// OnMouseButtonDown.
        void OnMouseButtonDown(int button, int xIn, int yIn)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnMouseButtonDown(*this, button, xIn, yIn);
            }
        }

        /// OnMouseButtonUp.
        void OnMouseButtonUp(int button, int xIn, int yIn)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnMouseButtonUp(*this, button, xIn, yIn);
            }
        }
        
        /// OnMouseButtonDoubleClick
        void OnMouseButtonDoubleClick(int button, int xIn, int yIn)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnMouseButtonDoubleClick(*this, button, xIn, yIn);
            }
        }


        /// OnMouseMove
        void OnMouseMove(int xIn, int yIn)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnMouseMove(*this, xIn, yIn);
            }
        }

        /// OnMouseWheel
        void OnMouseWheel(int delta, int xIn, int yIn)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnMouseWheel(*this, delta, xIn, yIn);
            }
        }


        /// OnKeyPressed.
        void OnKeyPressed(GT::Key key)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnKeyPressed(*this, key);
            }
        }

        /// OnKeyDown.
        bool OnKeyDown(GT::Key key)
        {
            bool retValue = true;

            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                if (!i->value->OnKeyDown(*this, key))
                {
                    retValue = false;
                }
            }

            return retValue;
        }

        /// OnKeyUp.
        void OnKeyUp(GT::Key key)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnKeyUp(*this, key);
            }
        }

        /// OnTextChanged.
        void OnTextChanged()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnTextChanged(*this);
            }
        }

        /// OnFocus.
        void OnFocus()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnFocus(*this);
            }
        }

        /// OnBlur.
        void OnBlur()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnBlur(*this);
            }
        }


        /// OnTear.
        void OnTear()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnTear(*this);
            }
        }

        /// OnDrop.
        void OnDrop(GUIElement &droppedElement)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnDrop(*this, droppedElement);
            }
        }

        /// OnDragAndDropEnter.
        void OnDragAndDropEnter(GUIElement &dragAndDropElement)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnDragAndDropEnter(*this, dragAndDropElement);
            }
        }

        /// OnDragAndDropLeave.
        void OnDragAndDropLeave(GUIElement &dragAndDropElement)
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnDragAndDropLeave(*this, dragAndDropElement);
            }
        }

        /// OnDragAndDropProxyRemoved.
        void OnDragAndDropProxyRemoved()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnDragAndDropProxyRemoved(*this);
            }
        }


        /// OnShow.
        void OnShow()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnShow(*this);
            }
        }

        /// OnHide.
        void OnHide()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnHide(*this);
            }
        }


        /// OnShowTooltip.
        void OnShowTooltip()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnShowTooltip(*this);
            }
        }

        /// OnHideTooltip.
        void OnHideTooltip()
        {
            for (auto i = this->eventHandlers.root; this->eventHandlers.root != nullptr && i != nullptr; i = i->next)
            {
                i->value->OnHideTooltip(*this);
            }
        }


    private:

        /// Attaches the given style class including it's includes. This is called recursively on it's includes.
        ///
        /// @param styleClass      [in] The style class whose includes are being attached.
        /// @param alreadyAttached [in] A list of style class that have already been included. This is used in order to avoid cyclic attaching.
        void AttachStyleClass(GUIStyleClass &styleClass, GT::Vector<GUIStyleClass*> &alreadyAttached, bool refresh = true);

        /// Detaches the given style class including it's includes. This is called recursively on it's includes.
        ///
        /// @param styleClass      [in] The style class whose includes are being detached.
        /// @param alreadyDetached [in] A list of style classes that have already been detached. This is used in order to avoid cyclic detaching.
        void DetachStyleClass(GUIStyleClass &styleClass, GT::Vector<GUIStyleClass*> &alreadyDetached, bool refresh = true);


    public:
    
        /// The ID of the element.
        char* id;

        /// The server that owns this elements.
        GUIServer &server;
        
        /// A pointer to the parent element.
        GUIElement* parent;
        
        /// A pointer to the first child.
        GUIElement* firstChild;
        
        /// A pointer to the last child.
        GUIElement* lastChild;
        
        /// A pointer to the previous sibling.
        GUIElement* prevSibling;
        
        /// A pointer to the next sibling.
        GUIElement* nextSibling;
        
        /// The style stack containing the style information about an element.
        GUIStyleStack style;

        /// A pointer to the primary style class of this element. This is lazily initialised in GetPrimaryStyleClass(), and thus mutable.
        mutable GUIStyleClass* m_primaryStyleClass;

        // TODO: Consider using a pointer for this so that elements without event handlers (most elements) do not have to have a List object instantiated. This 
        //       will save a single pointer (List objects are only two pointers in size).
        /// The list of C++ event handlers attached to this element.
        GT::List<GUIElementEventHandler*> eventHandlers;

        // The attributes below are updated from the layout engine.
        int x;              ///< The x position relative to the parent.
        int y;              ///< The y position relative to the parent.
        int width;          ///< The actual width of the element, in pixels. This does not include margins.
        int height;         ///< The actual height of the element, in pixels. This does not include margins.


        /// Structure containing the layout of the element. This is updated by the layout manager.
        struct _layout
        {
            //int x;
            //int y;
            //int width;
            //int height;
            int              absoluteX;
            int              absoluteY;
            int              unconstrainedWidth;
            int              unconstrainedHeight;
            unsigned short   marginLeft;
            unsigned short   marginRight;
            unsigned short   marginTop;
            unsigned short   marginBottom;
            unsigned short   paddingLeft;         ///< The size of the left padding, including the border.
            unsigned short   paddingRight;        ///< The size of the right padding, including the border.
            unsigned short   paddingTop;          ///< The size of the top padding, including the border.
            unsigned short   paddingBottom;       ///< The size of the bottom padding, including the border.
            GT::Rect<int> clippingRect;        ///< The clipping rectangle for borders and background.
            GT::Rect<int> clippingRectInner;   ///< The clipping rectangle for children and text.
            uint32_t         flags;               ///< Layout flags for use by the layout manager.
            GT::ListItem<GUIElement*>* layoutManagerListItem;  ///< The list item used by the layout manager.

            _layout()
                : //x(0), y(0), width(0), height(0),
                  absoluteX(0), absoluteY(0),
                  unconstrainedWidth(0), unconstrainedHeight(0),
                  marginLeft(0),  marginRight(0),  marginTop(0),  marginBottom(0),
                  paddingLeft(0), paddingRight(0), paddingTop(0), paddingBottom(0),
                  clippingRect(), clippingRectInner(),
                  flags(0), layoutManagerListItem(nullptr)
            {
            }

        }layout;


        /// Keeps track of the number of children that need to be clipped by the element.
        int clippedChildCount;

        // TODO: Need to consider turning this into a pointer and only having an instantiation when the element actually uses text. The TextManager class
        //       isn't exactly tiny...

        /// The text manager that we'll use for managing the layout of the text.
        mutable GT::TextManager textManager;

        /// The event handler to attach to the text .
        class TextEventHandler : public GT::TextManagerEventHandler
        {
        public:

            /// Constructor.
            TextEventHandler(GUIElement &element)
                : element(element)
            {
            }

            /// OnTextChanged.
            void OnTextChanged(const char*);


        private:

            /// The element that owns this event handler.
            GUIElement &element;


        private:    // No copying.
            TextEventHandler(const TextEventHandler &);
            TextEventHandler & operator=(const TextEventHandler &);

        }textManagerEventHandler;



        // All rendering data here is mutable because we want to constify the rendering functions. Conceptually, and element is not being
        // modified when it is being rendered.


        /// The rendering data for this element.
        mutable GUIElementRenderingData renderingData;


        /// Keeps track of whether or not the text rendering data is valid.
        mutable bool isTextRenderingDataValid;

        /// Keeps track of whether or not the shadow rendering data is valid.
        mutable bool isShadowRenderingDataValid;

        /// Keeps track of whether or not the border rendering data is valid.
        mutable bool isBorderRenderingDataValid;

        /// Keeps track of whether or not the background rendering data is valid.
        mutable bool isBackgroundRenderingDataValid;

        /// Keeps track of whether or not the element handles the OnDraw event.
        bool isHandlingOnDraw;

        /// Keeps track of whether or not this element is getting clipped by the parent.
        bool isClippedByParent;
        
        
        /// Properties in this structure are used for efficiently storing the element in an ElementClass object.
        struct BST
        {
            /// Default constructor.
            BST()
                : hashedID(0), left(nullptr), right(nullptr), parent(nullptr)
            {
            }
            
            
            /// The hashed name. This will be set when the name is set.
            uint32_t hashedID;
            
            /// The left side child in the BST.
            GUIElement* left;
            
            /// The right side child in the BST.
            GUIElement* right;
            
            /// The parent in the BST.
            GUIElement* parent;
            
        }bst;


    private:    // No copy.
        GUIElement(const GUIElement &);
        GUIElement & operator=(const GUIElement &);
    };
}

#endif
