// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUI_Context
#define GT_GUI_Context

#include "GUIContextBase.hpp"
#include "GUIRenderer.hpp"
#include "GUIEventHandler.hpp"
#include <GTLib/HandleManager.hpp>
#include <GTLib/Vector.hpp>

//#define GT_GUI_DEBUGGING 1

#if defined(GT_GUI_DEBUGGING)
#include "GUILogging.hpp"
#endif

namespace GT
{
    /////////////////////////////////////////////////////////////////////////
    // Misc documentation

    // Layout
    //
    // Layout is a complex issue that has several different parts to it. The first part is handling when an element needs to have
    // it's layout changed and the other is when to actually apply an update.
    //
    // When a particular style of an element or one of it's relatives is changed, a flag will be set that controls whether or not
    // the element needs to have a particular layout property updated. For example, if an element has it's "width" style changed, a
    // flag will be set that tells the layout manager that the width needs to be updated. Note that an update does not happen
    // immediately - this is for of efficiency.
    //
    // Each layout-changing style will effect different elements depending on how each element is related. For example, when the
    // size of an element changes, it will not only affect the layout of the element itself, but will have transending effects on
    // sibling elements because they will now have a different position.
    //
    // The layout will be re-evaluated in response to certain events, such as after a mouse movement, key stroke, etc.
    //
    //
    // Surfaces
    //
    // A surface is the object that elements are ultimately attached to. Typically, an element is attached to a parent, but the top-
    // most ancestor (an element without a parent) needs to be attached to something, which is where the surface comes in. At it's
    // most basic level, a surface is just a rectangle with a width and height property. It is up to the application to decide what
    // to do with a surface.
    //
    // There exists a default surface where all elements are initially attached. The surface that the element is attached to will
    // change when either the parent changes, or the surface itself is changed.
    //
    // When a surface is deleted, all of the elements that are attached to it are also deleted. If this is not desired, then it is
    // up to the application to manage the moving of elements from the surface.
    //
    //
    // Hierarchy
    //
    // Elements are linked to each other using a standard tree structure. Each element can be attached to an element as a child. When
    // a parent element is deleted, so is it's children.
    //
    // There are several hierarchy operations that can be performed in order to rearrange elements and their relationships.


    /// Class representing the GUI context.
    ///
    /// Every GUI operation will be executed through the context. The context is essentially an arbiter of all GUI operations to
    /// ensure everything is done properly, and to properly handle cases when it isn't.
    class GUIContext : public GUIContextBase
    {
    public:

        /// Constructor.
        GUIContext(GUIRenderer* renderer = nullptr, GUIFontManager* pFontManager = nullptr, GUIResourceManager* pResourceManager = nullptr);

        /// Destructor.
        ~GUIContext();


    private:

        /////////////////////////////////////////////////////////////////
        // Virtual Method Implementations

        /// @copydoc GUIContextBase::CreateSurfacePtr()
        GUISurface* CreateSurfacePtr();

        /// @copydoc GUIContextBase::DeleteSurfacePtr()
        void DeleteSurfacePtr(GUISurface* pSurface);


        /// @copydoc GUIContextBase::CreateElementPr()
        GUIElement* CreateElementPtr();

        /// @copydoc GUIContextBase::DeleteElementPtr()
        void DeleteElementPtr(GUIElement* pElement);


        /// @copydoc GUIContextBase::IterateSurfaces()
        void IterateSurfaces(std::function<bool (GUISurface* pSurface)> handler) const;

        /// @copydoc GUIContextBase::IterateElements()
        void IterateElements(std::function<bool (GUIElement* pElement)> handler) const;


        /// @copydoc GUIContextBase::PostEvent_OnSize()
        void PostEvent_OnSize(GUIElement* pElement, unsigned int width, unsigned int height);

        /// @copydoc GUIContextBase::PostEvent_ONMove()
        void PostEvent_OnMove(GUIElement* pElement, int x, int y);

        /// @copydoc GUIContextBase::PostEvent_OnSizeAndOnMove()
        void PostEvent_OnSizeAndOnMove(GUIElement* pElement, unsigned int width, unsigned height, int x, int y);

        /// @copydoc GUIContextBase::PostEvent_OnMouseEnter()
        void PostEvent_OnMouseEnter(GUIElement* pElement);

        /// @copydoc GUIContextBase::PostEvent_OnMouseLeave()
        void PostEvent_OnMouseLeave(GUIElement* pElement);

        /// @copydoc GUIContextBase::PostEvent_OnMouseMove()
        void PostEvent_OnMouseMove(GUIElement* pElement, int mousePosX, int mousePosY);

        /// @copydoc GUIContextBase::PostEvent_OnMouseButtonPressed()
        void PostEvent_OnMouseButtonPressed(GUIElement* pElement, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIContextBase::PostEvent_OnMouseButtonReleased()
        void PostEvent_OnMouseButtonReleased(GUIElement* pElement, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIContextBase::PostEvent_OnMouseButtonDoubleClicked()
        void PostEvent_OnMouseButtonDoubleClicked(GUIElement* pElement, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIContextBase::PostEvent_OnMouseWheel()
        void PostEvent_OnMouseWheel(GUIElement* pElement, int delta, int mousePosX, int mousePosY);

        /// @copydoc GUIContextBase::PostEvent_OnSetMouseEventCapture()
        void PostEvent_OnSetMouseEventCapture(GUIElement* pElement);

        /// @copydoc GUIContextBase::PostEvent_OnReleaseMouseEventCapture()
        void PostEvent_OnReleaseMouseEventCapture(GUIElement* pElement);


        /// @copydoc GUIContextBase::PostEvent_OnSurfaceNeedsRepaint()
        void PostEvent_OnSurfaceNeedsRepaint(GUISurface* pSurface, const GTLib::Rect<int> &rect);

        /// @copydoc GUIContextBase::PostEvent_OnCursorNeedsToChange()
        void PostEvent_OnCursorNeedsToChange(GUISystemCursor cursor);


        /// @copydoc GUIContextBase::Renderer_BeginPaintSurface()
        void Renderer_BeginPaintSurface(GUISurface* pSurface, void* pInputData);

        /// @copydoc GUIContextBase::Renderer_EndPaintSurface()
        void Renderer_EndPaintSurface();

        /// @copydoc GUIContextBase::Renderer_Clear()
        void Renderer_Clear(const GTLib::Rect<int> &rect);

        /// @copydoc GUIContextBase::Renderer_DrawRectangle()
        void Renderer_DrawRectangle(GTLib::Rect<int> rect, GTLib::Colour colour);

        /// @copydoc GUIContextBase::Renderer_SetClippingRect()
        void Renderer_SetClippingRect(GTLib::Rect<int> clippingRect);

        /// @copydoc GUIContextBase::Renderer_CanDrawText()
        bool Renderer_CanDrawText(HGUIFont hFont);

        /// @copydoc GUIContextBase::Renderer_DrawText()
        void Renderer_DrawText(const GUITextRunDesc &textRunDesc);

        /// @copydoc GUIContextBase::Renderer_DrawRawImage()
        void Renderer_DrawRawImage(int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent);

        /// @copydoc GUIContextBase::Renderer_UninitializeImage()
        void Renderer_InitializeImage(HGUIImage hImage, unsigned int width, unsigned int height, GUIImageFormat format, const void* pData);

        /// @copydoc GUIContextBase::Renderer_UninitializeImage()
        void Renderer_UninitializeImage(HGUIImage hImage);

        /// @copydoc GUIContextBase::Renderer_DrawTexturedRectangle()
        void Renderer_DrawTexturedRectangle(GTLib::Rect<int> rect, HGUIImage hImage, GTLib::Colour colour, unsigned int subImageOffsetX, unsigned int subImageOffsetY, unsigned int subImageWidth, unsigned int subImageHeight);



    public:

        /////////////////////////////////////////////////////////////////
        // Surfaces

        /// Creates a new surface.
        ///
        /// @remarks
        ///     The initial size will be 0,0
        HGUISurface CreateSurface();

        /// Deletes a surface.
        ///
        /// @param hSurface [in] A handle to the surface to delete.
        void DeleteSurface(HGUISurface hSurface);

        /// Retrieves a handle to a surface by it's ID.
        ///
        /// @param id [in] The ID of the surface to retrieve.
        ///
        /// @return A handle to the surface with the given ID, or 0 if a handle of the given ID could not be found.
        ///
        /// @remarks
        ///     This runs in linear time.
        HGUISurface FindSurfaceByID(const char* id) const;

        /// Sets the ID of the given surface.
        ///
        /// @param hSurface [in] A handle to the surface to delete.
        /// @param id       [in] The new ID of the surface.
        void SetSurfaceID(HGUISurface hSurface, const char* id);

        /// Retrieves the ID of the given surface.
        ///
        /// @param hSurface [in] The surface whose ID is being retrieved.
        const char* GetSurfaceID(HGUISurface hSurface) const;

        /// Sets the size of a surface.
        ///
        /// @param hSurface  [in] The handle to the surface whose size is being set.
        /// @param newWidth  [in] The new width of the surface.
        /// @param newHeight [in] The new height of the surface.
        ///
        /// @remarks
        ///     The relevant GUI elements will be resized and repositioned in response to a change in the size of the surface.
        void SetSurfaceSize(HGUISurface hSurface, unsigned int newWidth, unsigned int newHeight);

        /// Retrieves the size of a surface.
        ///
        /// @param hSurface  [in]  The handle to the surface whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        void GetSurfaceSize(HGUISurface hSurface, unsigned int &widthOut, unsigned int &heightOut) const;

        /// Determines whether or not the given surface contains the given element.
        ///
        /// @param hSurface [in] The surface to query.
        /// @param hElement [in] The element to look for.
        ///
        /// @return True if the given element is on the given surface.
        ///
        /// @remarks
        ///     This runs in linear time.
        bool DoesSurfaceContainGUIElement(HGUISurface hSurface, HGUIElement hElement) const;


        /// Sets the given surface's user data data pointer.
        ///
        /// @param hSurface  [in] A handle to the surface.
        /// @param pUserData [in] A pointer to the user data.
        ///
        /// @remarks
        ///     The user data can be retrieved with GetSurfaceUserData().
        void SetSurfaceUserData(HGUISurface hSurface, void* pUserData)
        {
            auto surface = this->GetSurfacePtr(hSurface);
            if (surface != nullptr)
            {
                surface->pUserData = pUserData;
            }
        }

        /// Retrieve's the given surface's user data pointer.
        ///
        /// @param hSurface [in] A handle to the surface.
        ///
        /// @return A pointer to the user data.
        template <typename T>
        T* GetSurfaceUserData(HGUISurface hSurface) const
        {
            auto surface = this->GetSurfacePtr(hSurface);
            if (surface != nullptr)
            {
                return reinterpret_cast<T*>(surface->pUserData);
            }

            return reinterpret_cast<T*>(0);
        }


        /// Iterates over every element that is attached to the given surface.
        ///
        /// @param hSurface [in] The a handle to surface whose elements are being iterated.
        /// @param handler  [in] The function that will be called for each element. If this returns false, the iteration will stop.
        void IterateSurfaceElements(HGUISurface hSurface, std::function<bool (HGUIElement)> handler);

        /// @copydoc GUIContextBase::GetSurfaceUnderMouse()
        HGUISurface GetSurfaceUnderMouse() const;


        ////////////////////////////////////////////////////////////////
        // Elements

        /// Creates a new GUI element.
        ///
        /// @return A handle to the new GUI element.
        HGUIElement CreateElement();

        /// Deletes a GUI element that was created with CreateGUIElement().
        ///
        /// @param hElement [in] The GUI element to delete.
        void DeleteElement(HGUIElement hElement);

        /// Sets the ID of the given element.
        ///
        /// @param hElement [in] The GUI element whose having it's ID changed.
        /// @param id       [in] The new ID of the GUI element.
        void SetElementID(HGUIElement hElement, const char* id);

        /// Retrieves the ID of the given element.
        ///
        /// @param hElement [in] The GUI element whose ID is being retrieved.
        ///
        /// @return The ID of the given element.
        const char* GetElementID(HGUIElement hElement) const;

        /// Finds an element by it's ID.
        ///
        /// @param elementID [in] The ID of the element to search for.
        ///
        /// @return
        ///     A handle to the first element with the given ID, or 0 if an element could not be found.
        HGUIElement FindElementByID(const char* elementID) const;


        /// Retrieves the parent element of the given element.
        ///
        /// @param hChildElement [in] The GUI element whose parent is being retrieved.
        ///
        /// @return A handle to the parent GUI element, or 0 if it does not have one.
        HGUIElement GetElementParent(HGUIElement hChildElement) const;

        /// Assigns the element to a different parent.
        ///
        /// @param hChildElement  [in] The GUI element that is being reparented.
        /// @param hParentElement [in] The new parent GUI element.
        ///
        /// @remarks
        ///     When hParentElement is not 0, this is equivalent to AppendChildGUIElement(hParentElement, hElement). When hParent is 0, the element
        ///     will be detached from it's parent.
        void SetElementParent(HGUIElement hChildElement, HGUIElement hParentElement);

        /// Detaches an element from it's parent.
        ///
        /// @param hChildElement [in] The GUI element that is being detached from it's parent.
        ///
        /// @remarks
        ///     This is equivalent to this->RemoveChildGUIElement(hElement->m_parent, hElement). If the element does not have a parent, this does
        ///     nothing.
        void DetachElementFromParent(HGUIElement hChildElement);

        /// Detaches an element from it's parent and siblings.
        ///
        /// @param hChildElement [in] The GUI element that is being detached from it's siblings.
        ///
        /// @remarks
        ///     If the element has a parent, it will be detached from that, too. The difference between this and DetachGUIElementFromParent() is that
        ///     that the later will not detach from siblings if it does not have a parent.
        void DetachElementFromParentAndSiblings(HGUIElement hChildElement);

        /// Appends a child GUI element.
        ///
        /// @param hParentElement [in] The GUI element that is having a child element appended.
        /// @param hChildElement  [in] The child GUI element to append.
        ///
        /// @remarks
        ///     An element can be prepended with PrependChildGUIElement().
        void AppendChildElement(HGUIElement hParentElement, HGUIElement hChildElement);

        /// Prepends a child GUI element.
        ///
        /// @param hParentElement [in] The GUI element that is having a child element prepended.
        /// @param hChildElement  [in] The child GUI element to append.
        ///
        /// @remarks
        ///     An element can be appended to the end using AppendChildGUIElement().
        void PrependChildElement(HGUIElement hParentElement, HGUIElement hChildElement);

        /// Appends an element to the given sibling.
        ///
        /// @param hElementToAppendTo [in] A handle to the element being appended to.
        /// @param hElementToAppend   [in] A handle to the element to append.
        ///
        /// @remarks
        ///     If the elements have different parents, the element to insert will be detached from it's current parent first.
        void AppendSiblingElement(HGUIElement hElementToAppendTo, HGUIElement hElementToAppend);

        /// Prepends an element to the given sibling.
        ///
        /// @param hElementToPrependTo [in] A handle to the element being prepended to.
        /// @param hElementToPrepend   [in] A handle to the element to prepend.
        ///
        /// @remarks
        ///     If the elements have different parents, the element to insert will be detached from it's current parent first.
        void PrependSiblingElement(HGUIElement hElementToPrependTo, HGUIElement hElementToPrepend);

        /// Retrieves a handle to the given element's next sibling.
        ///
        /// @param hElement [in] The GUI element whose next sibling is being retrieved.
        ///
        /// @return A handle to the GUI element of the given element's next sibling, or 0 if the element does not have a sibling or is the last one.
        HGUIElement GetElementNextSibling(HGUIElement hElement) const;

        /// Retrieves a handle to the given element's previous sibling.
        ///
        /// @param hElement [in] The GUI element whose previous sibling is being retrieved.
        ///
        /// @return A handle to the GUI element whose previous sibling is being retrieved.
        HGUIElement GetElementPreviousSibling(HGUIElement hElement) const;


        /// @copydoc GUIContextBase::IterateElementPrevSiblings()
        bool IterateElementPrevSiblings(HGUIElement hElement, std::function<bool (HGUIElement)> handler);

        /// @copydoc GUIContextBase::IterateElementPrevSiblingsReverse()
        bool IterateElementPrevSiblingsReverse(HGUIElement hElement, std::function<bool (HGUIElement)> handler) const;

        /// @copydoc GUIContextBase::IterateElementNextSiblings()
        bool IterateElementNextSiblings(HGUIElement hElement, std::function<bool (HGUIElement)> handler);

        /// @copydoc GUIContextBase::IterateElementNextSiblingsReverse()
        bool IterateElementNextSiblingsReverse(HGUIElement hElement, std::function<bool (HGUIElement)> handler) const;

        /// @copydoc GUIContextBase::IterateElementSiblings()
        bool IterateElementSiblings(HGUIElement hElement, std::function<bool (HGUIElement)> handler) const;

        /// @copydoc GUIContextBase::IterateElementSiblingsAndNotSelf()
        bool IterateElementSiblingsAndNotSelf(HGUIElement hElement, std::function<bool (HGUIElement)> handler) const { return this->IterateElementSiblings(hElement, handler); }

        /// @copydoc GUIContextBase::IterateElementSiblingsAndSelf()
        bool IterateElementSiblingsAndSelf(HGUIElement hElement, std::function<bool (HGUIElement)> handler);


        /// Determines if the given element is a direct child of the given element.
        ///
        /// @param hParentElement [in] The parent GUI element.
        /// @param hChildElement  [in] The child GUI element.
        ///
        /// @return True of the given element is a direct child of the other element.
        bool IsElementChild(HGUIElement hParentElement, HGUIElement hChildElement) const;

        /// Determines if the given element is the parent of the other given element.
        ///
        /// @param hChildElement [in] The child GUI element.
        /// @param hParentElement [in] The parent GUI element.
        ///
        /// @return True if the given element is the parent of the other given element.
        bool IsElementParent(HGUIElement hChildElement, HGUIElement hParentElement) const;

        /// Determines if the given element is a descendant of the given parent.
        ///
        /// @param hParentElement [in] The parent GUI element.
        /// @param hChildElement  [in] The child GUI element.
        ///
        /// @return True if the given child is a descendant of the given element.
        ///
        /// @remarks
        ///     This is a resursive search. Use IsChild() to only check the direct children.
        bool IsElementDescendant(HGUIElement hParentElement, HGUIElement hChildElement) const;

        /// Determines if the given element is an ancestor of the given element.
        ///
        /// @param hChildElement    [in] The parent GUI element.
        /// @param hAncestorElement [in] The child GUI element.
        ///
        /// @return True if the given ancestor element is an ancestor of the given element.
        bool IsElementAncestor(HGUIElement hChildElement, HGUIElement hAncestorElement) const;




        /// Sets the relative position of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose position is being set.
        /// @param xPos     [in] The X position.
        /// @param yPos     [in] The Y position.
        ///
        /// @remarks
        ///     The will set the positioning to relative and give the left/top corder priority.
        void SetElementPosition(HGUIElement hElement, float xPos, float yPos);
        void SetElementPosition(HGUIElement hElement, int xPos, int yPos);

        /// Sets the left style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose left position is being set.
        /// @param left     [in] The new left position.
        ///
        /// @remarks
        ///     This will give the left position priority over right.
        void SetElementLeftPosition(HGUIElement hElement, int left);

        /// Sets the top style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose top position is being set.
        /// @param top      [in] The new top position.
        ///
        /// @remarks
        ///     This will give the top position priority over bottom.
        void SetElementTopPosition(HGUIElement hElement, int top);

        /// Sets the right style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose right position is being set.
        /// @param right    [in] The new right position.
        ///
        /// @remarks
        ///     This will give the right position priority over left.
        void SetElementRightPosition(HGUIElement hElement, int right);

        /// Sets the bottom style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose bottom position is being set.
        /// @param bottom   [in] The new bottom position.
        ///
        /// @remarks
        ///     This will give the bottom position priority over top.
        void SetElementBottomPosition(HGUIElement hElement, int bottom);


        /// Sets whether the element is positioned relative to the inside of the padding, border, or ouside border of the parent element.
        ///
        /// @param hElement [in] The GUI element whose position origin is being set.
        /// @param origin   [in] The new position origin.
        ///
        /// @remarks
        ///     This is only used when the position is relative.
        void SetElementRelativePositionOrigin(HGUIElement hElement, PositionOrigin origin);

        /// Determines whether the element is positioned relative to the inside of the padding, border, or ouside border of the parent element.
        ///
        /// @param hElement [in] The GUI elemetn whose position origin is being retrieved.
        ///
        /// @return The position origin.
        ///
        /// @remarks
        ///     This is only relevant when the position is relative.
        PositionOrigin GetElementRelativePositionOrigin(HGUIElement hElement) const;


        /// Retrieves the actual relative position of the given element.
        ///
        /// @param hElement [in]  The GUI element whose position is being retrieved.
        /// @param xPosOut  [out] A reference to the variable that will receive the X position.
        /// @param yPosOut  [out] A reference to the variable that will receive the Y position.
        void GetElementPosition(HGUIElement hElement, float &xPosOut, float &yPosOut) const;
        void GetElementPosition(HGUIElement hElement, int &xPosOut, int &yPosOut) const;

        /// Retrieves the absolute position of the given element.
        ///
        /// @param hElement [in] The GUI element whose absolute position is being retrieved.
        /// @param xPosOut  [out] A reference to the variable that will receive the absolute X position.
        /// @param yPosOut  [out] A reference to the variable that will receive the absolute Y position.
        ///
        /// @remarks
        ///     This is a simple retrieval function. This will not dynamically compute the position.
        void GetElementAbsolutePosition(HGUIElement hElement, float &xPosOut, float &yPosOut) const;
        void GetElementAbsolutePosition(HGUIElement hElement, int &xPosOut, int &yPosOut) const;

        /// Sets the size of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose size is being set.
        /// @param width    [in] The width of the element.
        /// @param height   [in] The height of the element.
        void SetElementSize(HGUIElement hElement, float width, float height);
        void SetElementSize(HGUIElement hElement, unsigned int width, unsigned int height);

        /// Sets the size of the given element to a ratio of it's parent's size.
        ///
        /// @param hElement    [in] The GUI element whose size is being set.
        /// @param widthRatio  [in] The new width ratio of the element.
        /// @param heightRatio [in] The new height ratio of the element.
        void SetElementSizeRatio(HGUIElement hElement, float widthRatio, float heightRatio);

        /// Sets the size of the given element to tightly wrap it's children.
        ///
        /// @param hElement [in] The GUI element whose size is being set.
        void SetElementSizeToChildren(HGUIElement hElement);


        /// Sets the width of the element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose width is being set.
        /// @param width    [in] The new width of the element.
        ///
        /// @remarks
        ///     If both the width and height needs to be changed use SetGUIElementSize instead.
        void SetElementWidth(HGUIElement hElement, float width);
        void SetElementWidth(HGUIElement hElement, unsigned int width);

        /// Sets the width of the element as a ratio of the parent's width.
        ///
        /// @param hElement   [in] The GUI element whose width is being set.
        /// @param widthRatio [in] The new width ratio of the element.
        ///
        /// @remarks
        ///     If both the width and height needs to be changed, and they are both ratios, use SetGUIElementSizeRatio().
        ///     @par
        ///     The ratio cannot be more than 2 decimal places.
        void SetElementWidthRatio(HGUIElement hElement, float widthRatio);

        /// Sets the width of the element so that it is sized based on it's children.
        ///
        /// @param hElement [in] The GUI element whose width is being set.
        void SetElementWidthToChildren(HGUIElement hElement);


        /// Sets the minimum width of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose min width is being set.
        /// @param minWidth [in] The min width of the element.
        void SetElementMinWidth(HGUIElement hElement, float minWidth);
        void SetElementMinWidth(HGUIElement hElement, unsigned int minWidth);

        /// Sets the minimum width of the given element to a ratio of the parent's width.
        ///
        /// @param hElement      [in] The GUI element whose min width is being set.
        /// @param minWidthRatio [in] The new min width ratio.
        ///
        /// @remarks
        ///     If both the min width and height needs to be changed, and they're both ratios, use SetGUIElementMinSizeRatio().
        ///     @par
        ///     The ratio cannot be to more than 2 decimal places.
        void SetElementMinWidthRatio(HGUIElement hElement, float minWidthRatio);

        /// Sets the min width of the given element to that of it's children.
        ///
        /// @param hElement [in] The GUI element whose having minimum width set.
        void SetElementMinWidthToChildren(HGUIElement hElement);

        /// Sets the min width of the given element to it's default.
        ///
        /// @param hElement [in] The GUI element whose min width is being reset.
        ///
        /// @remarks
        ///     This is equivalent to setting the min width to 0.
        void SetElementMinWidthToDefault(HGUIElement hElement) { this->SetElementMinWidth(hElement, 0U); }

        /// Sets the maxium width of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose max width is being set.
        /// @param maxWidth [in] The max width of the element.
        void SetElementMaxWidth(HGUIElement hElement, float maxWidth);
        void SetElementMaxWidth(HGUIElement hElement, unsigned int maxWidth);

        /// Sets the minimum width of the given element to a ratio of the parent's width.
        ///
        /// @param hElement      [in] The GUI element whose min width is being set.
        /// @param minWidthRatio [in] The new min width ratio.
        ///
        /// @remarks
        ///     If both the min width and height needs to be changed, and they're both ratios, use SetGUIElementMinSizeRatio().
        ///     @par
        ///     The ratio cannot be to more than 2 decimal places.
        void SetElementMaxWidthRatio(HGUIElement hElement, float minWidthRatio);

        /// Sets the min width of the given element to that of it's children.
        ///
        /// @param hElement [in] The GUI element whose having minimum width set.
        void SetElementMaxWidthToChildren(HGUIElement hElement);

        /// Sets the min width of the given element to it's default.
        ///
        /// @param hElement [in] The GUI element whose min width is being reset.
        ///
        /// @remarks
        ///     This is equivalent to setting the min width to 0.
        void SetElementMaxWidthToDefault(HGUIElement hElement) { this->SetElementMinWidth(hElement, GUIStyleNumber_MaxUnsigned); }




        /// Sets the height of the element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose height is being set.
        /// @param height   [in] The new height of the element.
        ///
        /// @remarks
        ///     If both the width and height needs to be changed use SetGUIElementSize instead.
        void SetElementHeight(HGUIElement hElement, float height);
        void SetElementHeight(HGUIElement hElement, unsigned int height);

        /// Sets the height of the element as a ratio of the parent's height.
        ///
        /// @param hElement    [in] The GUI element whose height is being set.
        /// @param heightRatio [in] The new height ratio of the element.
        ///
        /// @remarks
        ///     If both the width and height needs to be changed, and they are both ratios, use SetGUIElementSizeRatio().
        ///     @par
        ///     The ratio cannot be more than 2 decimal places.
        void SetElementHeightRatio(HGUIElement hElement, float heightRatio);

        /// Sets the height of the element so that it is sized based on it's children.
        ///
        /// @param hElement [in] The GUI element whose height is being set.
        void SetElementHeightToChildren(HGUIElement hElement);


        /// Sets the minimum height of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose min height is being set.
        /// @param minHeight [in] The min height of the element.
        void SetElementMinHeight(HGUIElement hElement, float minHeight);
        void SetElementMinHeight(HGUIElement hElement, unsigned int minHeight);

        /// Sets the minimum height of the given element to a ratio of the parent's height.
        ///
        /// @param hElement       [in] The GUI element whose min height is being set.
        /// @param minHeightRatio [in] The new min height ratio.
        ///
        /// @remarks
        ///     If both the min height and height needs to be changed, and they're both ratios, use SetGUIElementMinSizeRatio().
        ///     @par
        ///     The ratio cannot be to more than 2 decimal places.
        void SetElementMinHeightRatio(HGUIElement hElement, float minHeightRatio);

        /// Sets the min height of the given element to that of it's children.
        ///
        /// @param hElement [in] The GUI element whose having minimum height set.
        void SetElementMinHeightToChildren(HGUIElement hElement);

        /// Sets the min height of the given element to it's default.
        ///
        /// @param hElement [in] The GUI element whose min height is being reset.
        ///
        /// @remarks
        ///     This is equivalent to setting the min height to 0.
        void SetElementMinHeightToDefault(HGUIElement hElement) { this->SetElementMinHeight(hElement, 0U); }

        /// Sets the maxium height of the given element, in DPI-aware points.
        ///
        /// @param hElement  [in] The GUI element whose max height is being set.
        /// @param maxHeight [in] The max height of the element.
        void SetElementMaxHeight(HGUIElement hElement, float maxHeight);
        void SetElementMaxHeight(HGUIElement hElement, unsigned int maxHeight);

        /// Sets the minimum height of the given element to a ratio of the parent's height.
        ///
        /// @param hElement       [in] The GUI element whose min height is being set.
        /// @param minHeightRatio [in] The new min height ratio.
        ///
        /// @remarks
        ///     If both the min height and height needs to be changed, and they're both ratios, use SetGUIElementMinSizeRatio().
        ///     @par
        ///     The ratio cannot be to more than 2 decimal places.
        void SetElementMaxHeightRatio(HGUIElement hElement, float minHeightRatio);

        /// Sets the min height of the given element to that of it's children.
        ///
        /// @param hElement [in] The GUI element whose having minimum height set.
        void SetElementMaxHeightToChildren(HGUIElement hElement);

        /// Sets the min height of the given element to it's default.
        ///
        /// @param hElement [in] The GUI element whose min height is being reset.
        ///
        /// @remarks
        ///     This is equivalent to setting the min height to 0.
        void SetElementMaxHeightToDefault(HGUIElement hElement) { this->SetElementMinHeight(hElement, GUIStyleNumber_MaxUnsigned); }




        /// Retrieves the actual width of the element.
        ///
        /// @param hElement [in] The GUI element whose actual width is being retrieved.
        ///
        /// @return The actual width of the element.
        float GetElementWidth(HGUIElement hElement) const;
        float GetElementWidthInPoints(HGUIElement hElement) const;

        /// Retrieves the actual height of the element.
        ///
        /// @param hElement [in] The GUI element whose actual height is being retrieved.
        ///
        /// @return The actual height of the element.
        float GetElementHeight(HGUIElement hElement) const;
        float GetElementHeightInPoints(HGUIElement hElement) const;


        /// Retrieves the absolute rectangle of the given element.
        ///
        /// @param hElement [in] The GUI element whose absolute rectangle is being retrieved.
        /// @param rectOut  [out] A reference to the structure that will receive the absolute rectangle of the given element.
        void GetElementAbsoluteRect(HGUIElement hElement, GTLib::Rect<float> &rectOut) const;
        void GetElementAbsoluteRect(HGUIElement hElement, GTLib::Rect<int> &rectOut) const;

        /// Retrieves the absolute rectangle of the inner region of the given element.
        ///
        /// @param hElement [in]  The GUI element whose absolute rectangle is being retrieved.
        /// @param rectOut  [out] A reference to the structure that will receive the absolute rectangle of the given element.
        void GetElementAbsoluteInnerRect(HGUIElement hElement, GTLib::Rect<float> &rectOut) const;
        void GetElementAbsoluteInnerRect(HGUIElement hElement, GTLib::Rect<int> &rectOut) const;

        /// Retrieves the absolute rectangle of the inner region of the given element, but outside the padding (just inside the border).
        ///
        /// @param hElement [in]  The GUI element whose absolute rectangle is being retrieved.
        /// @param rectOut  [out] A reference to the structure that will receive the absolute rectangle of the given element.
        void GetElementAbsoluteInnerBorderRect(HGUIElement hElement, GTLib::Rect<float> &rectOut) const;
        void GetElementAbsoluteInnerBorderRect(HGUIElement hElement, GTLib::Rect<int> &rectOut) const;



        /// Sets the positiong of the given element.
        ///
        /// @param hElement    [in] The element whose positioning is changing.
        /// @param positioning [in] The new position to use with the element.
        ///
        /// @remarks
        ///     Use this to change between absolute, relative and auto-positioning.
        void SetElementPositioning(HGUIElement hElement, Positioning positioning);

        /// Retrieves the positioning of the given element.
        ///
        /// @param hElement [in] The element whose positioning is being retrieved.
        ///
        /// @return The positioning of the element.
        Positioning GetElementPositioning(HGUIElement hElement) const;


        /// Sets the background colour of the given element.
        ///
        /// @param hElement [in] The GUI element whose background colour is being changed.
        /// @param colour   [in] The new background colour.
        void SetElementBackgroundColor(HGUIElement hElement, const GTLib::Colour &colour);

        /// Retrieves the background colour of the given element.
        ///
        /// @param hElement [in] The GUI element whose background colour is being retrieved.
        ///
        /// @return The background colour of the given element.
        GTLib::Colour GetElementBackgroundColor(HGUIElement hElement) const;

        /// @copydoc GUIContextBase::SetElementBackgroundColorBoundary()
        void SetElementBackgroundColorBoundary(HGUIElement hElement, BackgroundBoundary boundary);

        /// @copydoc GUIContextBase::GetElementBackgroundColorBoundary()
        BackgroundBoundary GetElementBackgroundColorBoundary(HGUIElement hElement) const;


        /// Sets the background image to use with the given element.
        ///
        /// @param pElement      [in] The GUI element whose backgroundimage is being changed.
        /// @param imageFilePath [in] The file path of the image to use as the background. Can be null, in which case no background image is used.
        ///
        /// @return True if the background image was set successfully; false otherwise.
        bool SetElementBackgroundImage(HGUIElement hElement, const char* imageFilePath, unsigned int subImageOffsetX = 0, unsigned int subImageOffsetY = 0, unsigned int subImageWidth = 0, unsigned int subImageHeight = 0);

        /// Retrieves the background image of the given element.
        ///
        /// @param pElement [in] The GUI element whose background image is being retrieved.
        HGUIImage GetElementBackgroundImage(HGUIElement hElement);

        /// @copydoc GUIContextBase::SetElementBackgroundImageColor()
        void SetElementBackgroundImageColor(HGUIElement hElement, const GTLib::Colour &colour);

        /// @copydoc GUIContextBase::GetElementBackgroundImageColor()
        GTLib::Colour GetElementBackgroundImageColor(HGUIElement hElement) const;

        /// Sets the offset of the given element's background sub-image.
        void SetElementBackgroundSubImageOffset(HGUIElement hElement, unsigned int subImageOffsetX, unsigned int subImageOffsetY);

        /// Retrieves the given element's background sub-image offset.
        bool GetElementBackgroundSubImageOffset(HGUIElement hElement, unsigned int &offsetXOut, unsigned int &offsetYOut) const;

        /// Sets the size of the given element's background sub-image.
        void SetElementBackgroundSubImageSize(HGUIElement hElement, unsigned int subImageWidth, unsigned int subImageHeight);

        /// Retrieves the sie of the given element's background sub-image.
        bool GetElementBackgroundSubImageSize(HGUIElement hElement, unsigned int &widthOut, unsigned int &heightOut) const;

        /// @copydoc GUIContextBase::SetElementBackgroundColorBoundary()
        void SetElementBackgroundImageBoundary(HGUIElement hElement, BackgroundBoundary boundary);

        /// @copydoc GUIContextBase::GetElementBackgroundColorBoundary()
        BackgroundBoundary GetElementBackgroundImageBoundary(HGUIElement hElement) const;

        /// @copydoc GUIContextBase::SetElementBackgroundImageAlign()
        void SetElementBackgroundImageAlign(HGUIElement hElement, BackgroundAlign alignX, BackgroundAlign alignY);

        /// @copydoc GUIContextBase::GetElementBackgroundImageAlign()
        bool GetElementBackgroundImageAlign(HGUIElement hElement, BackgroundAlign &alignXOut, BackgroundAlign &alignYOut) const;

        /// @copydoc GUIContextBase::SetElementBackgroundImageRepeat()
        void SetElementBackgroundImageRepeat(HGUIElement hElement, BackgroundRepeat repeatX, BackgroundRepeat repeatY);

        /// @copydoc GUIContextBase::GetElementBackgroundImageRepeat()
        bool GetElementBackgroundImageRepeat(HGUIElement hElement, BackgroundRepeat &repeatXOut, BackgroundRepeat &repeatYOut) const;

        /// @copydoc GUIContextBase::SetElementBackgroundImageScale()
        void SetElementBackgroundImageScale(HGUIElement hElement, float scaleX, float scaleY);

        /// @copydoc GUIContextBase::GetElementBackgroundImageScale()
        bool GetElementBackgroundImageScale(HGUIElement hElement, float &scaleXOut, float &scaleYOut) const;



        /// Sets the width of the left border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderLeftWidth(HGUIElement hElement, uint32_t borderWidth);

        /// Sets the width of the top border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderTopWidth(HGUIElement hElement, uint32_t borderWidth);

        /// Sets the width of the right border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderRightWidth(HGUIElement hElement, uint32_t borderWidth);

        /// Sets the width of the bottom border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderBottomWidth(HGUIElement hElement, uint32_t borderWidth);

        /// Sets the width of the border of the given element, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border width is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderWidth(HGUIElement hElement, uint32_t borderWidth);


        /// Sets the colour of the left border of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderLeftColor(HGUIElement hElement, const GTLib::Colour &borderColor);

        /// Sets the colour of the top border of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderTopColor(HGUIElement hElement, const GTLib::Colour &borderColor);

        /// Sets the colour of the right border of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderRightColor(HGUIElement hElement, const GTLib::Colour &borderColor);

        /// Sets the colour of the bottom border of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderBottomColor(HGUIElement hElement, const GTLib::Colour &borderColor);

        /// Sets the colour of all four borders of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderColor(HGUIElement hElement, const GTLib::Colour &borderColor);


        /// Sets the width and color of the left border.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The width of the border, in DPI-aware points.
        /// @param borderColor [in] The colour of the border.
        void SetElementBorderLeft(HGUIElement hElement, uint32_t borderWidth, const GTLib::Colour &borderColor);

        /// Sets the width and color of the top border.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The width of the border, in DPI-aware points.
        /// @param borderColor [in] The colour of the border.
        void SetElementBorderTop(HGUIElement hElement, uint32_t borderWidth, const GTLib::Colour &borderColor);

        /// Sets the width and color of the right border.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The width of the border, in DPI-aware points.
        /// @param borderColor [in] The colour of the border.
        void SetElementBorderRight(HGUIElement hElement, uint32_t borderWidth, const GTLib::Colour &borderColor);

        /// Sets the width and color of the bottom border.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The width of the border, in DPI-aware points.
        /// @param borderColor [in] The colour of the border.
        void SetElementBorderBottom(HGUIElement hElement, uint32_t borderWidth, const GTLib::Colour &borderColor);

        /// Sets the border of the given element.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The width of the border, in DPI-aware points.
        /// @param borderColor [in] The colour of the border.
        void SetElementBorder(HGUIElement hElement, uint32_t borderWidth, const GTLib::Colour &borderColor);


        /// @copydoc GUIContext::SetElementBorderLeftMask()
        void SetElementBorderLeftMaskInPixels(HGUIElement hElement, uint32_t maskOffset, uint32_t maskLength, const GTLib::Colour &maskColor = GTLib::Colour(0, 0, 0, 0));

        /// @copydoc GUIContext::SetElementBorderTopMask()
        void SetElementBorderTopMaskInPixels(HGUIElement hElement, uint32_t maskOffset, uint32_t maskLength, const GTLib::Colour &maskColor = GTLib::Colour(0, 0, 0, 0));

        /// @copydoc GUIContext::SetElementBorderRightMask()
        void SetElementBorderRightMaskInPixels(HGUIElement hElement, uint32_t maskOffset, uint32_t maskLength, const GTLib::Colour &maskColor = GTLib::Colour(0, 0, 0, 0));

        /// @copydoc GUIContext::SetElementBorderBottomMask()
        void SetElementBorderBottomMaskInPixels(HGUIElement hElement, uint32_t maskOffset, uint32_t maskLength, const GTLib::Colour &maskColor = GTLib::Colour(0, 0, 0, 0));



        /// Sets the left padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The left padding.
        void SetElementPaddingLeft(HGUIElement hElement, uint32_t padding);

        /// Sets the top padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The top padding.
        void SetElementPaddingTop(HGUIElement hElement, uint32_t padding);

        /// Sets the right padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The right padding.
        void SetElementPaddingRight(HGUIElement hElement, uint32_t padding);

        /// Sets the bottom padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The bottom padding.
        void SetElementPaddingBottom(HGUIElement hElement, uint32_t padding);

        /// Sets the padding of all sides of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The padding that will be applied to all sides.
        ///
        /// @remarks
        ///     This will set the padding to all sides.
        void SetElementPadding(HGUIElement hElement, uint32_t padding);


        /// Retrieves the left padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being retrieved.
        uint32_t GetElementPaddingLeft(HGUIElement hElement) const;

        /// Retrieves the top padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being retrieved.
        uint32_t GetElementPaddingTop(HGUIElement hElement) const;

        /// Retrieves the right padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being retrieved.
        uint32_t GetElementPaddingRight(HGUIElement hElement) const;

        /// Retrieves the bottom padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being retrieved.
        uint32_t GetElementPaddingBottom(HGUIElement hElement) const;




        /// Sets the left margin of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose left margin is being set.
        /// @param margin   [in] The left margin.
        void SetElementMarginLeft(HGUIElement hElement, uint32_t margin);

        /// Sets the top margin of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose top margin is being set.
        /// @param margin   [in] The top margin.
        void SetElementMarginTop(HGUIElement hElement, uint32_t margin);

        /// Sets the right margin of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose right margin is being set.
        /// @param margin   [in] The right margin.
        void SetElementMarginRight(HGUIElement hElement, uint32_t margin);

        /// Sets the bottom margin of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose bottom margin is being set.
        /// @param margin   [in] The bottom margin.
        void SetElementMarginBottom(HGUIElement hElement, uint32_t margin);

        /// Sets the margin of all sides of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose margin is being set.
        /// @param margin   [in] The margin that will be applied to all sides.
        ///
        /// @remarks
        ///     This will set th emargin to all sides.
        void SetElementMargin(HGUIElement hElement, uint32_t margin);



        /// Sets the axis the children of the given element run along (vertical or horizontal)
        ///
        /// @param hElement [in] The GUI element whose child axis is changing.
        /// @param axis     [in] The new axis.
        void SetElementChildAxis(HGUIElement hElement, ChildAxis axis);

        /// Retrieves the axis the children of thi given element run along (vertical or horizontal)
        ///
        /// @param hElement [in] The GUI element whose child axis is being retrieved.
        ///
        /// @return The axis the children of the given element run along (vertical or horizontal)
        ChildAxis GetElementChildAxis(HGUIElement hElement) const;


        /// Sets the horizontal alignment of the children.
        ///
        /// @param hElement [in] The GUI element whose horizontal alignment is being set.
        /// @param align    [in] The new horizontal alignment for children.
        void SetElementHorizontalAlign(HGUIElement hElement, HorizontalAlign align);

        /// Retrieves the horizontal alignment of children of the given element.
        ///
        /// @param hElement [in] The GUI element whose horizontal alignment is being retrieved.
        ///
        /// @return The horizontal alignment of children of the given element.
        HorizontalAlign GetElementHorizontalAlign(HGUIElement hElement) const;

        /// Sets the vertical alignment of the children.
        ///
        /// @param hElement [in] The GUI element whose vertical alignment is being set.
        /// @param align    [in] The new vertical alignment for children.
        void SetElementVerticalAlign(HGUIElement hElement, VerticalAlign align);

        /// Retrieves the vertical alignment of children of the given element.
        ///
        /// @param hElement [in] The GUI element whose vertical alignment is being retrieved.
        ///
        /// @return The vertical alignment of children of the given element.
        VerticalAlign GetElementVerticalAlign(HGUIElement hElement) const;


        /// Sets the boundary for the widths of child elements with relative widths.
        ///
        /// @param hElement [in] The GUI element whose width boundary is being changed.
        /// @param boundary [in] The new boundary mode.
        void SetElementChildrenWidthBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary);

        /// Retrieves the boundary for the widths of child elements with relative widths.
        ChildrenSizeBoundary GetElementChildrenWidthBoundary(HGUIElement hElement) const;

        /// Sets the boundary for the heights of child elements with relative heights.
        ///
        /// @param hElement [in] The GUI element whose height boundary is being changed.
        /// @param boundary [in] The new boundary mode.
        void SetElementChildrenHeightBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary);

        /// Retrieves the boundary for the heights of child elements with relative heights.
        ChildrenSizeBoundary GetElementChildrenHeightBoundary(HGUIElement hElement) const;

        /// Helper function for setting the boundary for the sizes of children elements with relative sizes.
        ///
        /// @param hElement [in] The GUI element whose size boundary is being changed.
        /// @param boundary [in] The new boundary mode.
        void SetElementChildrenSizeBoundary(HGUIElement hElement, ChildrenSizeBoundary boundary);


        /// Sets the clipping boundary for child elements.
        ///
        /// @param hElement [in] A reference to the element whose clipping boundary is being set.
        /// @param boundary [in] The new clipping boundary.
        ///
        /// @remarks
        ///     This method controls whether or not children are clipped against the elements padding, border, or outer area.
        void SetElementClippingBoundary(HGUIElement hElement, ClippingBoundary boundary);

        /// Retrieves the clipping boundary used with the given element.
        ///
        /// @param hElement [in] A reference to the element whose clipping boundary is being retrieved.
        ClippingBoundary GetElementClippingBoundary(HGUIElement hElement) const;


        /// Sets the clipping mode of the given element.
        ///
        /// @param hElement [in] A handle to the element whose clipping mode is being set.
        /// @param clippingMode [in] The new clipping mode.
        ///
        /// @remarks
        ///     To revert back to defaults, use ClippingMode_Auto or ClippingMode_Default.
        ///     @par
        ///     Use this to change the clipping mode from the defaults.
        void SetElementClippingMode(HGUIElement hElement, ClippingMode clippingMode);

        /// Retrieves the clipping mode of the given element.
        ///
        /// @param hElement [in] A handle to the element whose clipping mode is being retrieved.
        ///
        /// @return The clipping mode of the given element.
        ClippingMode GetElementClippingMode(HGUIElement hElement) const;


        /// Sets whether or not the element is visible.
        ///
        /// @param hElement [in] The GUI element whose visibility state is being set.
        /// @param visible  [in] Whether or not the element should be made visible.
        void SetElementVisibility(HGUIElement hElement, bool visible);

        /// Hides the given element.
        ///
        /// @param hElement [in] The GUI element that's being hiden.
        void HideElement(HGUIElement hElement) { this->SetElementVisibility(hElement, false); }

        /// Shows the given element.
        void ShowElement(HGUIElement hElement) { this->SetElementVisibility(hElement, true); }

        /// Determines whether or not the element is visible.
        ///
        /// @param hElement [in] The GUI element whose visibility state is being determined.
        ///
        /// @return True if the given element is visible; false otherwise.
        ///
        /// @remarks
        ///     Because an invisible element will render it's descendants invisible, this will be called recursively on ancestors.
        bool IsElementVisible(HGUIElement hElement) const;


        /// Enables flexing of the width's of percentage-sized child elements.
        ///
        /// @param hElement [in] The GUI element that child flexing is being enabled on.
        void EnableElementChildWidthFlexing(HGUIElement hElement);

        /// Disbles flexing of the width's of percentage-sized child elements.
        ///
        /// @param hElement [in] The GUI element that child flexing is being disabled on.
        void DisableElementChildWidthFlexing(HGUIElement hElement);

        /// Determines whether or not flexing is enabled on the children of the given element.
        ///
        /// @param hElement [in] The GUI element whose child flexing state is being retrieved.
        ///
        /// @return True if flexing is enabled on the width's of the children of the given element.
        bool IsElementChildWidthFlexingEnabled(HGUIElement hElement);


        /// Enables flexing of the width's of percentage-sized child elements.
        ///
        /// @param hElement [in] The GUI element that child flexing is being enabled on.
        void EnableElementChildHeightFlexing(HGUIElement hElement);

        /// Disbles flexing of the heights's of percentage-sized child elements.
        ///
        /// @param hElement [in] The GUI element that child flexing is being disabled on.
        void DisableElementChildHeightFlexing(HGUIElement hElement);

        /// Determines whether or not flexing is enabled on the children of the given element.
        ///
        /// @param hElement [in] The GUI element whose child flexing state is being retrieved.
        ///
        /// @return True if flexing is enabled on the width's of the children of the given element.
        bool IsElementChildHeightFlexingEnabled(HGUIElement hElement);


        /// Sets the given element's inner offset on the x axis.
        ///
        /// @param hElement     [in] The element whose inner offset is being set.
        /// @param innerOffsetX [in] The inner offset on the x offset.
        void SetElementInnerOffsetX(HGUIElement hElement, float innerOffsetX);

        /// Sets the given element's inner offset on the y axis.
        ///
        /// @param hElement     [in] The element whose inner offset is being set.
        /// @param innerOffsetY [in] The inner offset on the x offset.
        void SetElementInnerOffsetY(HGUIElement hElement, float innerOffsetY);

        /// Sets the given element's inner offset on both the x and y axis.
        ///
        /// @param hElement     [in] The element whose inner offset is being set.
        /// @param innerOffsetX [in] The inner offset on the x offset.
        /// @param innerOffsetY [in] The inner offset on the y offset.
        void SetElementInnerOffset(HGUIElement hElement, float innerOffsetX, float innerOffsetY);

        /// Retrieves the given element's inner offset on the x axis.
        ///
        /// @param hElement [in] The element whose inner offset is being retrieved.
        ///
        /// @return The inner offset on the x axis.
        float GetElementInnerOffsetX(HGUIElement hElement) const;

        /// Retrieves the given element's inner offset on the y axis.
        ///
        /// @param hElement [in] The element whose inner offset is being retrieved.
        ///
        /// @return The inner offset on the y axis.
        float GetElementInnerOffsetY(HGUIElement hElement) const;


        /// Sets the text of the given element.
        ///
        /// @param hElement [in] A handle to the element whose text is being set.
        /// @param text     [in] The element's text.
        void SetElementText(HGUIElement hElement, const char* text);

        /// Retrieves the text of the given element.
        ///
        /// @param hElement [in] A handle to the element whose text is being retrieved.
        ///
        /// @return A pointer to the buffer containing the element's text. This will be null-terminated.
        const char* GetElementText(HGUIElement hElement) const;

        /// Deterimes whether or not the given element has text.
        ///
        /// @param hElement [in] A handle to the element whose to check.
        ///
        /// @return True if the given element has any text.
        bool DoesElementHaveText(HGUIElement hElement) const;


        /// Sets the font of the given element.
        ///
        /// @param hElement [in] A handle to the GUI element whose font is being set.
        /// @param family   [in] The font family ('Arial', 'Courier', etc.)
        /// @param weight   [in] The font's weight (bold, medium, etc.)
        /// @param slant    [in] The font's slant (none, italic, oblique)
        /// @param size     [in] The size of the font.
        /// @param sizeType [in] Specifies whether or not the size is in points or pixels. Pixels by default.
        ///
        /// @return A pointer to the font object that is assigned to the element.
        HGUIFont SetElementFont(HGUIElement hElement, const char* family, FontWeight weight, FontSlant slant, uint32_t size, uint32_t sizeType = NumberType_Pixels);

        /// Retrieves the current font of the given element.
        ///
        /// @param hElement [in] A handle to the GUI element whose font is being retrieved.
        ///
        /// @return A pointer to the font object.
        HGUIFont GetElementFont(HGUIElement hElement) const;


        /// @copydoc GUIElementBase::SetElementTextColor()
        void SetElementTextColor(HGUIElement hElement, const GTLib::Colour &color);

        /// @copydoc GUIElementbase::GetElementTextColor()
        GTLib::Colour GetElementTextColor(HGUIElement hElement) const;


        /// @copydoc GUIElementbase::EnableEditableText()
        void EnableEditableText(HGUIElement hElement);

        /// @copydoc GUIElementbase::DisableEditableText()
        void DisableEditableText(HGUIElement hElement);

        /// @copydoc GUIElementbase::IsEditableTextEnabled()
        bool IsEditableTextEnabled(HGUIElement hElement) const;

        /// @copydoc GUIElementbase::EnableMultiLineText()
        void EnableMultiLineText(HGUIElement hElement);

        /// @copydoc GUIElementbase::DisableMultiLineText()
        void DisableMultiLineText(HGUIElement hElement);

        /// @copydoc GUIElementbase::IsMultiLineTextEnabled()
        bool IsMultiLineTextEnabled(HGUIElement hElement) const;


        /// @copydoc GUIElementbase::EnableFocusOnMouseClick()
        void EnableFocusOnMouseClick(HGUIElement hElement);

        /// @copydoc GUIElementbase::DisableFocusOnMouseClick()
        void DisableFocusOnMouseClick(HGUIElement hElement);

        /// @copydoc GUIElementbase::IsFocusOnMouseClickEnabled()
        bool IsFocusOnMouseClickEnabled(HGUIElement hElement) const;


        /// @copydoc GUIElementbase::SetElementCursor()
        void SetElementCursor(HGUIElement hElement, GUISystemCursor cursor);

        /// @copydoc GUIElementbase::GetElementCursor()
        GUISystemCursor GetElementCursor(HGUIElement hElement) const;



        /// Attaches the given element to the given surface.
        ///
        /// @param hElement [in] A handle to the element whose surface is changing.
        /// @param hSurface [in] A handle to the new surface.
        ///
        /// @return True if the surface is changed; false otherwise.
        ///
        /// @remarks
        ///     The surface of a child element can not be changed - it is always set the surface of it's ancestors.
        ///     @par
        ///     This will recursively change the surface of the children.
        bool AttachElementToSurface(HGUIElement hElement, HGUISurface hSurface);

        /// Detaches the given element from the given surface.
        ///
        /// @param hElement [in] A handle to the element being detached.
        ///
        /// @return True if the surface is changed; false otherwise.
        ///
        /// @remarks
        ///     The surface of a child element can not be changed - it is always set the surface of it's ancestors.
        ///     @par
        ///     This will recursively change the surface of the children.
        ///     @par
        ///     This will return false if hElement is not a root level element.
        bool DetachElementFromSurface(HGUIElement hElement);

        /// Retrieves the surface the element is currently attached to.
        ///
        /// @param hElement [in] A handle to the element whose surface is being retrieved.
        HGUISurface GetElementSurface(HGUIElement hElement) const;

        /// @copydoc GUIContextBase::GetElementUnderMouse()
        HGUIElement GetElementUnderMouse() const;

        /// @copydoc GUIContextBase::IsElementUnderPoint()
        bool IsElementUnderMouse(HGUIElement hElement) const;


        /// @copydoc GUIContextBase::EnableCursorPassThrough()
        void EnableCursorPassThrough(HGUIElement hElement);

        /// @copydoc GUIContextBase::DisableCursorPassThrough()
        void DisableCursorPassThrough(HGUIElement hElement);

        /// @copydoc GUIContextBase::IsCursorPassThroughEnabled()
        bool IsCursorPassThroughEnabled(HGUIElement hElement) const;



        ////////////////////////////////////////////////////////////////
        // Batching

        /// Begins a batch operator.
        ///
        /// @remarks
        ///     Batched operations allow the context to more efficeintly perform bulk operations. The main thing here is batching
        ///     layout and painting operations to avoid redundancy.
        ///     @par
        ///     Begin/end batching pairs can be called within each other, but they should always be paired together correctly.
        void BeginBatch();

        /// Ends a batch operations, and performs all of the required operations to get the relevant state up to date.
        ///
        /// @remarks
        ///     This should be paired with a called to BeginBatch(). This is where repainting will occur for surfaces whose painting
        ///     mode is not deferred.
        void EndBatch();



        ////////////////////////////////////////////////////////////////
        // Painting

        /// Invalidates a rectangular region of the given surface.
        ///
        /// @param hSurface [in] The surface whose rectangle region is being invalidated.
        /// @param rect     [in] The rectangle to invalidate.
        void InvalidateSurfaceRect(HGUISurface hSurface, const GTLib::Rect<int> &rect);
        void InvalidateSurfaceRect(HGUISurface hSurcface);

        /// Paints a portion of the given surface.
        ///
        /// @param hSurface [in] The surface to paint.
        /// @param rect     [in] The rectangle region to paint.
        void PaintSurface(HGUISurface hSurface, const GTLib::Rect<int> &rect, void* pInputData);

        /// Paints the entire region of the given surface.
        ///
        /// @param hSurface [in] A handle to the surface to paint.
        void PaintSurface(HGUISurface hSurface, void* pInputData);



        /////////////////////////////////////////////////////////////////
        // Events
        //
        // There are two scopes an event handler can be attached to:
        //   1) Local - attached to an individual element.
        //   2) Global - attached at a global level.

        /// Attaches an event handler to local scope.
        ///
        /// @param hElement     [in] A handle to the element to attach the event handler to.
        /// @param eventHandler [in] A reference to the event handler to attach.
        void AttachLocalEventHandler(HGUIElement hElement, GUIEventHandler &eventHandler);

        /// Detaches a local event handler.
        ///
        /// @param hElement     [in] A handle to the element whose event handler is being detached.
        /// @param eventHandler [in] A reference to the event handler to detach.
        void DetachLocalEventHandler(HGUIElement hElement, GUIEventHandler &eventHandler);

        /// Attaches a global event handler.
        ///
        /// @param eventHandler [in] A reference to the event handler to attach.
        void AttachGlobalEventHandler(GUIEventHandler &eventHandler);

        /// Detaches a global event handler.
        ///
        /// @param eventHandler [in] A reference to the event handler to detach,.
        void DetachGlobalEventHandler(GUIEventHandler &eventHandler);


        void OnElementSize(HGUIElement hElement, LocalOnSizeProc handler);
        void OnElementMove(HGUIElement hElement, LocalOnMoveProc handler);
        void OnElementMouseEnter(HGUIElement hElement, LocalOnMouseEnterProc handler);
        void OnElementMouseLeave(HGUIElement hElement, LocalOnMouseLeaveProc handler);
        void OnElementMouseMove(HGUIElement hElement, LocalOnMouseMoveProc handler);
        void OnElementMouseButtonPressed(HGUIElement hElement, LocalOnMouseButtonPressedProc handler);
        void OnElementMouseButtonReleased(HGUIElement hElement, LocalOnMouseButtonReleasedProc handler);
        void OnElementMouseButtonDoubleClicked(HGUIElement hElement, LocalOnMouseButtonDoubleClickedProc handler);
        void OnElementMouseWheel(HGUIElement hElement, LocalOnMouseWheelProc handler);
        void OnElementSetMouseEventCapture(HGUIElement hElement, LocalOnSetMouseEventCaptureProc handler);
        void OnElementReleaseMouseEventCapture(HGUIElement hElement, LocalOnReleaseMouseEventCaptureProc handler);

        void OnGlobalElementSize(GlobalOnSizeProc handler);
        void OnGlobalElementMove(GlobalOnMoveProc handler);
        void OnGlobalElementMouseEnter(GlobalOnMouseEnterProc handler);
        void OnGlobalElementMouseLeave(GlobalOnMouseLeaveProc handler);
        void OnGlobalElementMouseMove(GlobalOnMouseMoveProc handler);
        void OnGlobalElementMouseButtonPressed(GlobalOnMouseButtonPressedProc handler);
        void OnGlobalElementMouseButtonReleased(GlobalOnMouseButtonReleasedProc handler);
        void OnGlobalElementMouseButtonDoubleClicked(GlobalOnMouseButtonDoubleClickedProc handler);
        void OnGlobalElementMouseWheel(GlobalOnMouseWheelProc handler);
        void OnGlobalElementSetMouseEventCapture(GlobalOnSetMouseEventCaptureProc handler);
        void OnGlobalElementReleaseMouseEventCapture(GlobalOnReleaseMouseEventCaptureProc handler);
        void OnGlobalElementSurfaceNeedsRepaint(GlobalOnSurfaceNeedsRepaintProc handler);
        void OnGlobalCursorNeedsToChange(GlobalOnCursorNeedsToChangeProc handler);




        ////////////////////////////////////////////////////////////////
        // Inbound Events
        //
        // These methods are called by the host application in response to certain events such as when the mouse moves within a surface.

        /// Called by the host application when the mouse moves within the given surface.
        ///
        /// @param hSurface  [in] A handle to the surface the mouse was moved within.
        /// @param mousePosX [in] The x position of the cursor on the surface relative to the top left corner.
        /// @param mousePosY [in] The y position of the cursor on the surface relative to the top left corner.
        void OnMouseMove(HGUISurface hSurface, int mousePosX, int mousePosY);

        /// Called by the host application when the mouse enters the given surface.
        ///
        /// @param hSurface [in] A handle to the surface the mouse just entered.
        void OnMouseEnter(HGUISurface hSurface);

        /// Called by the host application when the mouse leaves the given surface.
        ///
        /// @param hSurface [in] A handle to the surface the mouse just left.
        void OnMouseLeave(HGUISurface hSurface);

        /// @copydoc GUIContextBase::OnMouseButtonClicked()
        void OnMouseButtonPressed(HGUISurface hSurface, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIContextBase::OnMouseButtonReleased()
        void OnMouseButtonReleased(HGUISurface hSurface, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIContextBase::OnMouseButtonDoubleClicked()
        void OnMouseButtonDoubleClicked(HGUISurface hSurface, int mouseButton, int mousePosX, int mousePosY);

        /// @copydoc GUIContextBase::OnMouseWheel()
        void OnMouseWheel(HGUISurface hSurface, int delta, int mousePosX, int mousePosY);


        ////////////////////////////////////////////////////////////////
        // Finding/Gathering

        /// Finds the element sitting directly under the given point on the given surface.
        ///
        /// @param hSurface [in] A handle to the surface to search for.
        /// @param x        [in] The position of the point on the x axis relative to the top left corner.
        /// @param y        [in] The position of the point on the y axis relative to the top left corner.
        ///
        /// @return A handle to the element sitting directly under the point, or 0 if no element is sitting under the point.
        ///
        /// @remarks
        ///     This will take clipping into account.
        HGUIElement FindElementUnderPoint(HGUISurface hSurface, int x, int y);


        ////////////////////////////////////////////////////////////////
        // DPI / Scaling

        /// Retrieves the base DPI on the X axis.
        unsigned int GetBaseXDPI() const;

        /// Retrieves the base DPI on the Y axis.
        unsigned int GetBaseYDPI() const;

        /// Retrieves the base DPI on both the X and Y axis.
        ///
        /// @param xBaseDPIOut [out] A reference to the variable that will receive the base DPI on the X axis.
        /// @param yBaseDPIOut [out] A reference to the variable that will receive the base DPI on the Y axis.
        void GetBaseDPI(unsigned int &xBaseDPIOut, unsigned int &yBaseDPIOut) const;

        /// Sets the base DPI on both the X and Y axis.
        ///
        /// @param xBaseDPI [in] The new base DPI on the X axis.
        /// @param yBaseDPI [in] The new base DPI on the Y axis.
        ///
        /// @remarks
        ///     This will cause a re-validation of the layout and will redraw every surface.
        void SetBaseDPI(unsigned int xBaseDPI, unsigned int yBaseDPI);


        /// Retrieves the DPI on the X axis.
        unsigned int GetXDPI(HGUISurface hSurface) const;

        /// Retrieves the DPI on the Y axis.
        unsigned int GetYDPI(HGUISurface hSurface) const;

        /// Retrieves the DPI on both the X and Y axis.
        ///
        /// @param xDPIOut [out] A reference to the variable that will receive the DPI on the X axis.
        /// @param yDPIOut [out] A reference to the variable that will receive the DPI on the Y axis.
        void GetDPI(HGUISurface hSurface, unsigned int &xDPIOut, unsigned int &yDPIOut);

        /// Sets the DPI on both the X and Y axis.
        ///
        /// @param xDPI [in] The new DPI on the X axis.
        /// @param yDPI [in] The new DPI on the Y axis.
        ///
        /// @remarks
        ///     This will cause a re-validation of the layout and will redraw every surface.
        void SetDPI(HGUISurface hSurface, unsigned int xDPI, unsigned int yDPI);


        /// Retrieves the DPI scaling factor for the X axis based on the given surface's DPI.
        ///
        /// @param hSurface [in] The surface whose DPI is being used to calculate the scaling factor.
        ///
        /// @return A float equal to the current DPI divided by the base DPI for the X axis.
        float GetXDPIScalingFactor(HGUISurface hSurface) const;

        /// Retrieves the DPI scaling factor for the Y axis based on the given surface's DPI.
        ///
        /// @param hSurface [in] The surface whose DPI is being used to calculate the scaling factor.
        ///
        /// @return a float equal to the current DPI divided by the base DPI for the Y axis.
        float GetYDPIScalingFactor(HGUISurface hSurface) const;


        /// Converts the given value from pixels to DPI-aware points based on the given element's X DPI scaling.
        float PixelsToPointsX(HGUIElement hElement, float x);
        float PixelsToPointsX(HGUIElement hElement, int x);

        /// Converts the given value from pixels to DPI-aware points based on the given element's Y DPI scaling.
        float PixelsToPointsY(HGUIElement hElement, float y);
        float PixelsToPointsY(HGUIElement hElement, int y);


        ////////////////////////////////////////////////////////////////
        // Utilities

        /// Converts a point from absolute to relative coordinates based on the given element.
        ///
        /// @param hElement [in]  A handle to the element to make the point relative to.
        /// @param xIn      [in]  The absolute position on the x axis to convert.
        /// @param yIn      [in]  The absolute position on the y axis to convert.
        /// @param xOut     [out] A reference to the variable that will receive the relative position on the x axis.
        /// @param yOut     [out] A reference to the variable that will receive the relative position on the y axis.
        void AbsoluteToRelative(HGUIElement hElement, int xIn, int yIn, int &xOut, int &yOut) const;

        /// Converts a point from relative to absolute coordinates based on the given element.
        ///
        /// @param hElement [in] A handle to the element the given point is relative to.
        /// @param xIn      [in] The relative position on the x axis.
        /// @param yIn      [in] The relative position on the y axis.
        /// @param xOut     [out] A reference to the variable that will receive the absolute position on the x axis.
        /// @param yOut     [out] A reference to the variable that will receive the absolute position on the y axis.
        void RelativeToAbsolute(HGUIElement hElement, int xIn, int yIn, int &xOut, int &yOut) const;


        ////////////////////////////////////////////////////////////////
        // Miscellaneous

        /// Retrieves a pointer to the font manager.
        ///
        /// @return A pointer to the font manager.
        GUIFontManager* GetFontManager();

        /// Sets the element that has captured the mouse input events.
        ///
        /// @param hElement [in] A pointer to the element that will receive the mouse events.
        ///
        /// @remarks
        ///     All mouse events will be routed to the given element until the capture changes, or is released.
        void SetMouseEventCapture(HGUIElement hElement);

        /// Retrieves the element that is currently capturing mouse events.
        ///
        /// @return A pointer to the element that is currently capturing mouse events.
        HGUIElement GetMouseEventCapture() const;

        /// Releases the mouse event capture and restores default mouse message handling.
        void ReleaseMouseEventCapture();


    private:

        /// Retrieve a pointer to the actual surface referred to by the given handle.
        ///
        /// @param hSurface [in] The handle to the surface whose pointer is being retrieved.
        ///
        /// @return A pointer to the actual surface, or null if the handle is invalid.
        ///
        /// @remarks
        ///     If the handle is invalid, null will be returned.
        GUISurfaceWithHandle* GetSurfacePtr(HGUISurface hSurface) const;

        /// Retrieves a pointer to the actual GUI element referred to by the given handle.
        ///
        /// @param hElement [in] The handle to the element whose pointer is being retrieved.
        ///
        /// @return A pointer to the actual GUI element, or null if the handle is invalid.
        ///
        /// @remarks
        ///     If the handle is invalid, null will be returned.
        GUIElementWithHandle* GetElementPtr(HGUIElement hElement) const;


        //////////////////////////////////////////////////
        // Event Posting

        /// Safely iterates over each local event handler.
        ///
        /// @param hElement [in] A handle to the element whose event handlers should be iterated over.
        /// @param func     [in] The function to call for each event handler iteration.
        ///
        /// @remarks
        ///     This method should safely handle cases when the element is deleted and event handlers are added and removed during iteration.
        ///     @par
        ///     If the iteration function returns false, iteration will stop.
        void IterateLocalEventHandlers(HGUIElement hElement, std::function<bool (GUIEventHandler &)> func);

        /// Safely iterates over each global event handler.
        ///
        /// @param func [in] The function to call for each event handler iteration.
        ///
        /// @remarks
        ///     This method should safely handles cases when event handlers are added or removed during iration.
        ///     @par
        ///     If the iteration function returns false, iteration will stop.
        void IterateGlobalEventHandlers(std::function<bool (GUIEventHandler &)> func);


    private:

        /// The handle manager for elements.
        HandleManager<HGUIElement, GUIElementWithHandle> m_elementHandles;

        /// The handle manager for surfaces.
        HandleManager<HGUISurface, GUISurfaceWithHandle> m_surfaceHandles;


        /// A pointer to the renderer.
        GUIRenderer* m_renderer;

        /// Keeps track of whether or not the context owns the renderer object.
        bool m_ownsRenderer;


        /// The list of global event handlers.
        GTLib::Vector<GUIEventHandler*> m_globalEventHandlers;

        /// The callback based global event handlers.
        GlobalCallbackEventHandlers m_callbackGlobalEventHandlers;


#if defined(GT_GUI_DEBUGGING)
        GUILayoutLogger m_layoutLogger;
#endif


    private:    // No copying.
        GUIContext(const GUIContext &);
        GUIContext & operator=(const GUIContext &);
    };
}

#endif
