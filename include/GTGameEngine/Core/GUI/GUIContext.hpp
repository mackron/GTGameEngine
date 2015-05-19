// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_Context_hpp_
#define __GT_GUI_Context_hpp_

#include "GUIElement.hpp"
#include "GUISurface.hpp"
#include "GUIRenderer.hpp"
#include "GUIImageManager.hpp"
#include "GUIFontManager.hpp"
#include "GUIEventHandler.hpp"
#include <GTLib/HandleManager.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/List.hpp>

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
    class GUIContext
    {
    public:

        /// Constructor.
        GUIContext(GUIRenderer* renderer = nullptr);

        /// Destructor.
        ~GUIContext();


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
        /// @param newID    [in] The new ID of the surface.
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
        void GetSurfaceSize(HGUISurface hSurface, float &widthOut, float &heightOut) const;
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


        /// Sets the given surface's aux. data pointer.
        ///
        /// @param hElement [in] A handle to the surface.
        /// @param auxData  [in] A pointer to the aux. data.
        ///
        /// @remarks
        ///     The aux. data can be retrieved with GetSurfaceAuxData().
        void SetSurfaceAuxData(HGUISurface hSurface, void* auxData)
        {
            auto surface = this->GetSurfacePtr(hSurface);
            if (surface != nullptr)
            {
                surface->auxData = auxData;
            }
        }

        /// Retrieve's the given surface's aux. data pointer.
        ///
        /// @param hElement [in] A handle to the surface.
        ///
        /// @return A pointer to the aux. data.
        template <typename T>
        T* GetSurfaceAuxData(HGUISurface hSurface)
        {
            auto surface = this->GetSurfacePtr(hSurface);
            if (surface != nullptr)
            {
                return reinterpret_cast<T*>(surface->auxData);
            }

            return reinterpret_cast<T*>(nullptr);
        }


        /// Iterates over every element that is attached to the given surface.
        ///
        /// @param hSurface [in] The a handle to surface whose elements are being iterated.
        /// @param handler  [in] The function that will be called for each element. If this returns false, the iteration will stop.
        void IterateSurfaceElements(HGUISurface hSurface, std::function<bool (HGUIElement)> handler);


        /// Sets the painting mode for the given surface - immediate or deferred.
        ///
        /// @param hSurface     [in] A handle to the surface whose painting mode is being set.
        /// @param paintingMode [in] The new painting mode.
        ///
        /// @remarks
        ///     When \c paintingMode is set to \c GUIPaintingMode::Immediate (the default), painting commands are posted automatically
        ///     and immediately whenever there is a change to the graphical representation of an element that is attached to the given
        ///     surface.
        ///     @par
        ///     When \c paintingMode is set to \c GUIPaintingMode::Deferred, painting commands are posted via an explicit call to
        ///     PaintSurface(). In this case, painting commands are posted such that the entire surface is redrawn.
        void SetSurfacePaintingMode(HGUISurface hSurface, GUIPaintingMode paintingMode);



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
        /// @param newID    [in] The new ID of the GUI element.
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


        /// Iterates over the previous siblings of the given element in forward order.
        ///
        /// @param hElement [in] The GUI element whose siblings are being iterated.
        /// @param func     [in] The function to call for each element.
        void IterateElementPrevSiblings(HGUIElement hElement, std::function<void (HGUIElement)> func);

        /// Iterates over the next siblings of the given element.
        ///
        /// @param hElement [in] The GUI element whose siblings are being iterated.
        /// @param func     [in] The function to call for each element.
        void IterateElementNextSiblings(HGUIElement hElement, std::function<void (HGUIElement)> func);

        /// Iterates over the siblings of the given element, including itself.
        ///
        /// @param hElement [in] The GUI element whose siblings are being iterated.
        /// @param func     [in] The function to call for each sibling.
        void IterateElementSiblingsAndSelf(HGUIElement hElement, std::function<void (HGUIElement)> func);


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
        /// @param hElement [in] The GUI element whose height is being set.
        /// @param height   [in] The new height ratio of the element.
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

        /// Retrieves the actual height of the element.
        ///
        /// @param hElement [in] The GUI element whose actual height is being retrieved.
        ///
        /// @return The actual height of the element.
        float GetElementHeight(HGUIElement hElement) const;


        /// Retrieves the absolute rectangle of the given element.
        ///
        /// @param hElement [in] The GUI element whose absolute rectangle is being retrieved.
        /// @param rectOut  [out] A reference to the structure that will receive the absolute rectangle of the given element.
        void GetElementAbsoluteRect(HGUIElement hElement, GTLib::Rect<float> &rectOut) const;
        void GetElementAbsoluteRect(HGUIElement hElement, GTLib::Rect<int> &rectOut) const;



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
        void SetElementBackgroundColour(HGUIElement hElement, const GTLib::Colour &colour);

        /// Retrieves the background colour of the given element.
        ///
        /// @param hElement [in] The GUI element whose background colour is being retrieved.
        ///
        /// @return The background colour of the given element.
        GTLib::Colour GetElementBackgroundColour(HGUIElement hElement) const;



        /// Sets the width of the left border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderLeftWidth(HGUIElement hElement, uint32_t width);

        /// Sets the width of the top border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderTopWidth(HGUIElement hElement, uint32_t width);

        /// Sets the width of the right border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderRightWidth(HGUIElement hElement, uint32_t width);

        /// Sets the width of the bottom border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderBottomWidth(HGUIElement hElement, uint32_t width);

        /// Sets the width of the border of the given element, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border width is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderWidth(HGUIElement hElement, uint32_t width);


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
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorderLeft(HGUIElement hElement, uint32_t width, const GTLib::Colour &borderColor);

        /// Sets the width and color of the top border.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorderTop(HGUIElement hElement, uint32_t width, const GTLib::Colour &borderColor);

        /// Sets the width and color of the right border.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorderRight(HGUIElement hElement, uint32_t width, const GTLib::Colour &borderColor);

        /// Sets the width and color of the bottom border.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorderBottom(HGUIElement hElement, uint32_t width, const GTLib::Colour &borderColor);

        /// Sets the border of the given element.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorder(HGUIElement hElement, uint32_t width, const GTLib::Colour &colour);



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
        /// @param 
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
        const GTLib::Font* SetElementFont(HGUIElement hElement, const char* family, FontWeight weight, FontSlant slant, uint32_t size, uint32_t sizeType = NumberType_Pixels);

        /// Retrieves the current font of the given element.
        ///
        /// @param hElement [in] A handle to the GUI element whose font is being retrieved.
        ///
        /// @return A pointer to the font object.
        const GTLib::Font* GetElementFont(HGUIElement hElement) const;


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

        /// Paints a portion of the given surface.
        ///
        /// @param hSurface [in] The surface to paint.
        /// @param rect     [in] The rectangle region to paint.
        void PaintSurface(HGUISurface hSurface, const GTLib::Rect<int> &rect);

        /// Paints the entire region of the given surface.
        ///
        /// @param hSurface [in] A handle to the surface to paint.
        void PaintSurface(HGUISurface hSurface);



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



    private:

        /// Retrieve a pointer to the actual surface referred to by the given handle.
        ///
        /// @param hSurface [in] The handle to the surface whose pointer is being retrieved.
        ///
        /// @param A pointer to the actual surface, or null if the handle is invalid.
        ///
        /// @remarks
        ///     If the handle is invalid, null will be returned.
        GUISurface* GetSurfacePtr(HGUISurface hSurface) const;

        /// Retrieves a pointer to the actual GUI element referred to by the given handle.
        ///
        /// @param hElement [in] The handle to the element whose pointer is being retrieved.
        ///
        /// @return A pointer to the actual GUI element, or null if the handle is invalid.
        ///
        /// @remarks
        ///     If the handle is invalid, null will be returned.
        GUIElement* GetElementPtr(HGUIElement hElement) const;


        /// Retrieves 


        /// Recursively sets the surface for the given element and it's children.
        ///
        /// @param element  [in] A reference to the element whose surface is being set.
        /// @param hSurface [in] A handle to the surface.
        void SetElementSurfaceRecursive(GUIElement &element, HGUISurface hSurface);


        /// Retrieves the absolute rectangle of the given element.
        ///
        /// @param hElement [in]  The GUI element whose absolute rectangle is being retrieved.
        /// @param rectOut  [out] A reference to the structure that will receive the absolute rectangle of the given element.
        void GetElementAbsoluteRect(GUIElement &element, GTLib::Rect<float> &rectOut) const;
        void GetElementAbsoluteRect(GUIElement &element, GTLib::Rect<int> &rectOut) const;

        /// Retrieves the rectangle children of the given element will be clipped against.
        ///
        /// @param hElement [in]  The GUI element whose clipping rectangle is being retrieved.
        /// @param rectOut  [out] A reference to the object that will receive the children clipping rectangle.
        void GetElementChildrenClippingRect(GUIElement &element, GTLib::Rect<float> &rectOut) const;
        void GetElementChildrenClippingRect(GUIElement &element, GTLib::Rect<int> &rectOut) const;


        
        /// Determines whether or not the given element is visible.
        ///
        /// @param element [in] A reference to the element whose visibility state is being determined.
        ///
        /// @return True if the element is visible; false otherwise.
        ///
        /// @remarks
        ///     Because an invisible element will render it's descendants invisible, this will be called recursively on the element's ancestors. 
        bool IsElementVisible(GUIElement &element) const;


        /// Determines whether or not the given element is being clipped against it's parent.
        ///
        /// @param element [in] A reference to the element whose clipping state is being retrieved.
        bool IsElementClippedAgainstParent(GUIElement &element) const;


        /// Performs a clipped traversals of the children of the given element.
        ///
        /// @param element      [in] A reference the root element.
        /// @param clippingRect [in] The rectangle to clip against.
        /// @param func         [in] The function to call for the input element and each of it's children.
        ///
        /// @remarks
        ///     This will do a pre-order depth-first traversal, and will include the root element.
        ///     @par
        ///     The delegate function will not be called if the child is completely clipped.
        void ClippedTraversal(GUIElement &element, const GTLib::Rect<float> &clippingRect, std::function<void (GUIElement &, const GTLib::Rect<int> &)> func);


        /// Updates the given element's font based on it's current style.
        ///
        /// @param element [in] A reference to the element whose having it's font updated.
        ///
        /// @return A pointer to the element's new font.
        const GTLib::Font* UpdateElementFontFromStyle(GUIElement &element);


        //////////////////////////////////////////////////
        // Direct element hierarchy operations.

        /// Attaches a child element to the given parent element.
        ///
        /// @param parentElement [in] A reference to the parent element.
        /// @param childElement  [in] A reference to the child element.
        ///
        /// @remarks
        ///     If the child element is already attached to a parent, it will be detached.
        void AppendChildElement(GUIElement &parentElement, GUIElement &childElement);

        /// Prepends a child child element to the given parent element.
        ///
        /// @param parentElement [in] A reference to the parent element.
        /// @param childElement  [in] A reference to the child element.
        ///
        /// @remarks
        ///     If the child element is already attached to a parent, it will be detached.
        void PrependChildElement(GUIElement &parentElement, GUIElement &childElement);

        /// Appends an element to the given sibling.
        ///
        /// @param siblingElement  [in] A reference to the sibling that the child will be appended to.
        /// @param elementToInsert [in] A reference to the element to insert.
        ///
        /// @remarks
        ///     If the elements have different parents, the element to insert will be detached from it's current parent first.
        void AppendSiblingElement(GUIElement &siblingElement, GUIElement &elementToInsert);

        /// Prepends an element to the given sibling.
        ///
        /// @param siblingElement  [in] A reference to the sibling that the element will be prepended to.
        /// @param elementToInsert [in] A reference to the element to insert.
        ///
        /// @remarks
        ///     If the elements have different parents, the element to insert will be detached from it's current parent first.
        void PrependSiblingElement(GUIElement &siblingElement, GUIElement &elementToInsert);

        /// Detaches a child element from it's parent element.
        ///
        /// @param element [in] A reference to the element to detach from it's parent.
        ///
        /// @remarks
        ///     The element will also be detached from it's siblings. The difference between this and DetachGUIElementFromSiblingsAndParent()
        ///     is that if the element is already parentless, it will not detach from any siblings.
        void DetachElementFromParent(GUIElement &element);

        /// Detaches an element from it's siblings, and by extension, it's parent.
        ///
        /// @param element [in] A reference to the element to detach from it's siblings.
        ///
        /// @remarks
        ///     Detaching an element from it's siblings will isolate it from siblings and it's parent, but will keep it's children. The
        ///     difference between this and DetachGUIElementFromParent() is that this will detach from siblings even when the element does
        ///     not have a parent.
        void DetachElementFromParentAndSiblings(GUIElement &element);



        ////////////////////////////////////////////////////////////////
        // Iteration
        //
        // All iterators here are standard. When the handler function returns false, the iteration will stop.
        
        /// Iterates over every surface.
        ///
        /// @param handler [in] The function to call for each surface.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateSurfaces(std::function<bool (GUISurface* pSurface)> handler);

        /// Iterates over every element.
        ///
        /// @param handler [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElements(std::function<bool (GUIElement* pElement)> handler);

        /// Iterates over each element that is attached to the given surface.
        ///
        /// @param surface [in] A reference to the surface whose elements are being iterated.
        /// @param handler [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateSurfaceElements(GUISurface* pSurface, std::function<bool (GUIElement* pElement)> handler);

        /// Recursively iterates over every child of the given element.
        ///
        /// @param element [in] A reference to the element whose children are being iterated.
        /// @param handler [in] The function to call for each child element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementChildrenRecursive(GUIElement* pElement, std::function<bool (GUIElement* pElement)> handler);

        /// Iterates over every child of the given element.
        ///
        /// @param element [in] A reference to the element whose children are being iterated.
        /// @param handler [in] The function to call for each child element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        ///
        /// @remarks
        ///     This is not recursive.
        bool IterateElementChildren(GUIElement* pElement, std::function<bool (GUIElement* pElement)> handler);




        ////////////////////////////////////////////////////////////////
        // DPI / Scaling

        /// Updates every element as required as a result of a change in DPI.
        ///
        /// @param surface [in] A reference to the surface whose DPI has changed.
        ///
        /// @remarks
        ///     This will update the layout and font of every element.
        void UpdateAllElementsOnDPIChange(GUISurface &surface);
        void UpdateAllElementsOnDPIChange();

        /// Updates the size of each border of the given element based on their style and current DPI scaling.
        ///
        /// @param element [in] A reference to the element whose border sizes are being updated.
        ///
        /// @remarks
        ///     This does not invalidate the layout.
        void UpdateElementBorderSizes(GUIElement &element);

        /// Updates the size of the margins of the given element based on their style and current DPI scaling.
        ///
        /// @param element [in] A reference to the element whose margin sizes are being updated.
        ///
        /// @remarks
        ///     This does not invalidate the layout.
        void UpdateElementMarginSizes(GUIElement &element);

        /// Updates the size of the padding of the given element based on their style and current DPI scaling.
        ///
        /// @param element [in] A reference to the element whose padding sizes are being updated.
        ///
        /// @remarks
        ///     This does not invalidate the layout.
        void UpdateElementPaddingSizes(GUIElement &element);



        //////////////////////////////////////////////////
        // Painting

        /// Marks the given rectangle as invalid which will cause a repaint.
        ///
        /// @param surface [in] The surface whose rectangle region is being invalidated.
        /// @param rect    [in] The rectangle to invalidate.
        ///
        /// @remarks
        ///     Invalidating a rectangle does not redraw it. It will instead be redrawn when the host application requests it with Paint().
        void Painting_InvalidateRect(GUISurface &surface, const GTLib::Rect<int> &rect);
        void Painting_InvalidateRect(HGUISurface surface, const GTLib::Rect<int> &rect);

        /// Invalidates the rectangle of the given element.
        ///
        /// @param element [in] A reference to the element whose rectangle is being invalidated.
        void Painting_InvalidateElementRect(GUIElement &element);

        /// Paints the invalid rectangles of every surface that has an invalid region.
        ///
        /// @remarks
        ///     This does not paint surfaces whose painting modes are set to GUIPaintingMode::Deferred.
        ///     @par
        ///     This will mark every invalid rectangle as valid.
        void Painting_PaintAndValidateSurfaceRects();

        /// Paints the given element and it's descendents if they fall into the given rectangle.
        ///
        /// @param surface      [in] The surface being drawn to.
        /// @param element      [in] A reference to the element to draw.
        /// @param clippingRect [in] The rectangle to clip against.
        ///
        /// @remarks
        ///     If the element falls outside the given rectangle, it will ignored, as will it's descendents that are clipped against it.
        void Painting_PaintElement(GUISurface &surface, GUIElement &element, const GTLib::Rect<int> &clippingRect);

        /// Optimized function for setting the clipping and drawing the rectangle at the same time.
        ///
        /// @param surface [in] The surface to draw on.
        /// @param rect    [in] The clipping rectangle, and the rectangle to draw.
        /// @param color   [in] The color to draw the clipping rectangle.
        ///
        /// @remarks
        ///     This function simply combines SetClippingRect and DrawRectangle into a single optimized call.
        void Painting_DrawAndSetClippingRect(GUISurface &surface, const GTLib::Rect<int> &rect, const GTLib::Colour &color);



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

        /// Posts the OnSize event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        /// @param width   [in] The new width of the element.
        /// @param height  [in] The new height of the element.
        void PostEvent_OnSize(HGUIElement hElement, unsigned int width, unsigned int height);

        /// Posts the OnMove event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        /// @param x       [in] The new position of the element on the x axis, relative to the top left corner of the parent.
        /// @param y       [in] The new position of the element on the y axis, relative to the top left corner of the parent.
        void PostEvent_OnMove(HGUIElement hElement, int x, int y);

        /// Posts the OnMouseEnter event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        void PostEvent_OnMouseEnter(HGUIElement hElement);

        /// Posts the OnMouseLeave event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        void PostEvent_OnMouseLeave(HGUIElement hElement);

        /// Posts the OnMouseMove event.
        ///
        /// @param element   [in] A reference to the element receiving the event.
        /// @param mousePosX [in] The position of the mouse on the x axis relative to the top left corner of the element.
        /// @param mousePosY [in] The position of the mouse on the y axis relative to the top left corner of the element.
        void PostEvent_OnMouseMove(HGUIElement hElement, int mousePosX, int mousePosY);

        /// Posts the OnPaint event.
        ///
        /// @param hSurface [in] A handle to the surface that has been painted.
        /// @param rect     [in] The rectangle that will just painted.
        void PostEvent_OnPaint(HGUISurface hSurface, const GTLib::Rect<int> &rect);


        //////////////////////////////////////////////////
        // Layout

        /// Helper method for setting the size of the given element.
        ///
        /// @param hElement   [in] A handle to the element whose size is being set.
        /// @param width      [in] The width of the element as a 32-bit unsigned int, the value of which is determined by widthType
        /// @param widthType  [in] The type of number the width is (actual, percentage, etc.)
        /// @param height     [in] The height of the element as a 32-bit unsigned int, the value of which is determined by heightType
        /// @param heightType [in] The type of number the height is (actual, precentage, etc.)
        ///
        /// @remarks
        ///     widthType and heightType can be one of the NumberType_* contants.
        void Layout_SetElementSize(HGUIElement hElement, uint32_t width, uint32_t widthType, uint32_t height, uint32_t heightType);

        /// Helper method for setting the width of the given element.
        ///
        /// @param hElement   [in] A handle to the element whose size is being set.
        /// @param width      [in] The width of the element as a 32-bit unsigned int, the value of which is determined by widthType
        /// @param widthType  [in] The type of number the width is (actual, percentage, etc.)
        ///
        /// @remarks
        ///     widthType can be one of the NumberType_* contants.
        void Layout_SetElementWidth(HGUIElement hElement, uint32_t width, uint32_t widthType);

        /// Helper method for setting the min width of the given element.
        ///
        /// @param hElement  [in] A handle to the element whose min size is being set.
        /// @param minWidth  [in] The min width of the element as a 32-bit unsigned int, the value of which is determined by widthType.
        /// @param widthType [in] The type of number the width is (actual, percentage, etc.)
        void Layout_SetElementMinWidth(HGUIElement hElement, uint32_t minWidth, uint32_t widthType);

        /// Helper method for setting the max width of the given element.
        ///
        /// @param hElement  [in] A handle to the element whose min size is being set.
        /// @param minWidth  [in] The ax width of the element as a 32-bit unsigned int, the value of which is determined by widthType.
        /// @param widthType [in] The type of number the width is (actual, percentage, etc.)
        void Layout_SetElementMaxWidth(HGUIElement hElement, uint32_t maxWidth, uint32_t widthType);

        /// Helper method for setting the height of the given element.
        ///
        /// @param hElement   [in] A handle to the element whose size is being set.
        /// @param height     [in] The height of the element as a 32-bit unsigned int, the value of which is determined by heightType
        /// @param heightType [in] The type of number the height is (actual, precentage, etc.)
        ///
        /// @remarks
        ///     heightType can be one of the NumberType_* contants.
        void Layout_SetElementHeight(HGUIElement hElement, uint32_t height, uint32_t heightType);

        /// Helper method for setting the min height of the given element.
        ///
        /// @param hElement  [in] A handle to the element whose min size is being set.
        /// @param minWidth  [in] The min height of the element as a 32-bit unsigned int, the value of which is determined by widthType.
        /// @param widthType [in] The type of number the height is (actual, percentage, etc.)
        void Layout_SetElementMinHeight(HGUIElement hElement, uint32_t minHeight, uint32_t heightType);

        /// Helper method for setting the max height of the given element.
        ///
        /// @param hElement  [in] A handle to the element whose min size is being set.
        /// @param minWidth  [in] The ax height of the element as a 32-bit unsigned int, the value of which is determined by widthType.
        /// @param widthType [in] The type of number the height is (actual, percentage, etc.)
        void Layout_SetElementMaxHeight(HGUIElement hElement, uint32_t maxHeight, uint32_t heightType);


        /// Invalidates the layout of the given element.
        ///
        /// @param hElement [in] A reference to the element whose layout is being invaliated.
        /// @param flags    [in] The flags specifying which layout properties have changed.
        ///
        /// @remarks
        ///     When a change is made to an element such that it's layout needs updating, this will need to be called on that element.
        ///     @par
        ///     Basically, this function just marks the layout as invalid - it does not actually update the layout. The layout will be
        ///     updated when ValidateGUIElementLayouts() is called.
        void Layout_InvalidateElementLayout(GUIElement &element, unsigned int flags);
        void Layout_InvalidateElementLayout( HGUIElement hElement, unsigned int flags)
        {
            auto element = this->GetElementPtr(hElement);
            if (element != 0)
            {
                this->Layout_InvalidateElementLayout(*element, flags);
            }
        }

        /// Invalidates the applicable layouts of the applicable elements based on when the inner width of the given element changes.
        ///
        /// @param element [in] A reference to the element whose inner width has changed.
        void Layout_InvalidateElementLayoutsOnInnerWidthChange(GUIElement &element);

        /// Invalidates the applicable layouts of the applicable elements based on when the inner height of the given element changes.
        ///
        /// @param element [in] A reference to the element whose inner height has changed.
        void Layout_InvalidateElementLayoutsOnInnerHeightChange(GUIElement &element);

        /// Invalidates the applicable layouts of the applicable elements based on when the inner size (width and height) of the given element changes.
        ///
        /// @param element [in] A reference to the element whose inner size has changed.
        void Layout_InvalidateElementLayoutsOnInnerSizeChange(GUIElement &element);


        /// Invalidates the applicable layouts of the applicable elements based on when the child axis of the given element changes.
        ///
        /// @param element [in] A reference to the element whose child axis has changed.
        void Layout_InvalidateElementLayoutsOnChildAxisChange(GUIElement &element);


        /// Invalidates the applicable layouts of the applicable elements based on when the horizontal align of the given element changes.
        ///
        /// @param element [in] A reference to the element whose horizontal alignment has changed.
        void Layout_InvalidateElementLayoutsOnHorizontalAlignChange(GUIElement &element);

        /// Invalidates the applicable layouts of the applicable elements based on when the vertical align of the given element changes.
        ///
        /// @param element [in] A reference to the element whose vertical alignment has changed.
        void Layout_InvalidateElementLayoutsOnVerticalAlignChange(GUIElement &element);



        /// Validates the layouts of every element whose layout is invalid.
        ///
        /// @remarks
        ///     This is where the layouts of elements are actually updated. By the time this method returns, all elements should have valid layouts.
        void Layout_ValidateElementLayouts();

        /// Validates the layout of the given element.
        ///
        /// @param element [in] The element whose layout is being validated.
        void Layout_ValidateElementLayout(GUIElement &element);

        /// Validates the width of the given element.
        ///
        /// @param element [in] The element whose width is being validated.
        void Layout_ValidateElementWidth(GUIElement &element);

        /// Validates the height of the given element.
        ///
        /// @param element [in] The element whose hieght is being validated.
        void Layout_ValidateElementHeight(GUIElement &element);

        /// Validates the position of the given element.
        ///
        /// @param element               [in] The element whose position is being validated.
        ///
        /// @remarks
        ///     The will validate both the X and Y position.
        void Layout_ValidateElementPosition(GUIElement &element);

        /// Validates the position of the given element based on absolute positioning.
        ///
        /// @param element               [in] The element whose position is being validated.
        ///
        /// @remarks
        ///     This should only be called when the positioning of the given element is absolute.
        void Layout_ValidateElementPosition_Absolute(GUIElement &element);

        /// Validates the position of the given element based on relative positioning.
        ///
        /// @param element [in] The element whose position is being validated.
        ///
        /// @remarks
        ///     This should only be called when the positioning of the given element is relative.
        void Layout_ValidateElementPosition_Relative(GUIElement &element);

        /// Validates the position of the given element based on automatic positioning.
        ///
        /// @param element [in] The element whose position is being validated.
        ///
        /// @remarks
        ///     This should only be called when the positioning of the given element is automatic.
        void Layout_ValidateElementPosition_Auto(GUIElement &element);

        /// Validates the text layout of the given element.
        ///
        /// @param element [in] The element whose text layout is being validated.
        void Layout_ValidateElementText(GUIElement &element);


        /// Marks the width of the givn element as valid.
        ///
        /// @param element [in] A reference to the element whose width is being marked as valid.
        void Layout_MarkElementWidthAsValid(GUIElement &element);

        /// Marks the height of the givn element as valid.
        ///
        /// @param element [in] A reference to the element whose height is being marked as valid.
        void Layout_MarkElementHeightAsValid(GUIElement &element);

        /// Marks the position of the given element as valid.
        ///
        /// @param element [in] A reference to the element whose position is being marked as valid.
        void Layout_MarkElementPositionAsValid(GUIElement &element);

        /// Marks the layout of the text of the given element as valid.
        ///
        /// @param element [in] A reference to the element whose text layout is being marked as valid.
        void Layout_MarkElementTextAsValid(GUIElement &element);


        /// Updates the width of the given element, returning it's new outer width (width + horizontal margins).
        ///
        /// @param element [in] The element whose width is being updated.
        float Layout_UpdateElementWidth(GUIElement &element);

        /// Updastes the height of the given element, returning it's new outer height (width + vertical margins).
        ///
        /// @param element [in] The element whose height is being updated.
        float Layout_UpdateElementHeight(GUIElement &element);


        /// Calculates the width of the given element based on the given width style attribute.
        ///
        /// @param element         [in] The element whose new width is being calculated.
        /// @param width           [in] The width style value.
        /// @param widthType       [in] The width style value type (%, absolute, etc).
        /// @param calculateFlexed [in] Whether or not flexing should be calculated.
        ///
        /// @remarks
        ///     This does not set the new width.
        float Layout_CalculateElementWidth(GUIElement &element, uint32_t width, uint32_t widthType, bool calculateFlexed);

        /// Calculates the height of the given element based on the given height style attribute.
        ///
        /// @param element         [in] The element whose height is being calculated.
        /// @param height          [in] The height style value.
        /// @param heightType      [in] The height style value type (%, absolute, etc).
        /// @param calculateFlexed [in] Whether or not flexing should be calculated.
        ///
        /// @remarks
        ///     This does not set the new width.
        float Layout_CalculateElementHeight(GUIElement &element, uint32_t height, uint32_t heightType, bool calculateFlexed);


        /// Calculates the width of the children of the given element for when the parent is auto-sized based on the width of it's children.
        ///
        /// @param element [in] The element whose children width is being calculated.
        ///
        /// @return The width of the children of the given element for when it is auto-sized based on the width of the it's children.
        ///
        /// @remarks
        ///     This will only include children whose size would actually affect the parent. This only considers auto-positioned children. Absolute
        ///     and relative positioned elements are ignored.
        float Layout_CalculateElementChildrenWidthForAutoSize(GUIElement &element) const;

        /// Calculates the height of the children of the given element for when the parent is auto-sized based on the height of it's children.
        ///
        /// @param element [in] The element whose children height is being calculated.
        ///
        /// @return The height of the children of the given element for when it is auto-sized based on the height of the it's children.
        ///
        /// @remarks
        ///     This will only include children whose size would actually affect the parent. This only considers auto-positioned children. Absolute
        ///     and relative positioned elements are ignored.
        float Layout_CalculateElementChildrenHeightForAutoSize(GUIElement &element) const;

        /// Retrieves the total width of the children of the given element that would contribute to it's width when auto-sized.
        ///
        /// @param element [in] A reference to the element whose childrens' size is being retrieved.
        ///
        /// @remarks
        ///     This will not include invisible children, not those that would not contribute to the size of the element if it were sized based on the children.
        float Layout_CalculateElementChildrenWidth(GUIElement &element);

        /// Retrieves the total height of the children of the given element that would contribute to it's width when auto-sized.
        ///
        /// @param element [in] A reference to the element whose childrens' size is being retrieved.
        ///
        /// @remarks
        ///     This will not include invisible children, not those that would not contribute to the size of the element if it were sized based on the children.
        float Layout_CalculateElementChildrenHeight(GUIElement &element);


        /// Retrieves the total width of the given element and it's visible, auto-positioned siblings.
        ///
        /// @param element [in] A reference to the element whose siblings size is being retrieved.
        ///
        /// @remarks
        ///     This will not include invisible siblings, nor those that are not auto-positioned.
        float Layout_CalculateTotalVisibleAutoPositionedSiblingsWidth(GUIElement &element);

        /// Retrieves the total height of the given element and it's visible, auto-positioned siblings.
        ///
        /// @param element [in] A reference to the element whose siblings size is being retrieved.
        ///
        /// @remarks
        ///     This will not include invisible siblings, nor those that are not auto-positioned.
        float Layout_CalculateTotalVisibleAutoPositionedSiblingsHeight(GUIElement &element);


        /// Finds the closest neighboring auto-positioned sibling of the given element.
        ///
        /// @param element [in] The element whose neighboring sibling is being retrieved.
        ///
        /// @remarks
        ///     This is used when needing to update the position of auto-positioned sibling elements because they all depend on each other.
        GUIElement* Layout_GetNeighboringAutoPositionedSibling(GUIElement &element);

        /// Finds the first auto-positioned child of the given element.
        ///
        /// @param element [in] The element whose first auto-positioned child is being retrieved.
        ///
        /// @remarks
        ///     This is used when needing to update the position of auto-positioned children.
        GUIElement* Layout_GetFirstAutoPositionedChild(GUIElement &element);



        /// Marks the position of the given element as changed.
        ///
        /// @param element [in] A reference to the element whose position is being marked as changed.
        ///
        /// @remarks
        ///     At the end of validation, all elements whose position has changed will have move events posted.
        void Layout_MarkElementPositionAsChanged(GUIElement &element);

        /// Marks the size of the given element as changed.
        ///
        /// @param element [in] A reference to the element whose size is being marked as changed.
        ///
        /// @remarks
        ///     At the end of layout validation, all elements whose size is marked as changed will have size events posted.
        void Layout_MarkElementSizeAsChanged(GUIElement &element);


        /// Recursively determines whether or not the absolute position of the given element has changed.
        ///
        /// @param element [in] A reference to the element whose absolute position change state is being checked.
        bool Layout_HasElementAbsolutePositionChanged(GUIElement &element);


        /// Updates the absolute positions of the elements whose absolute positions are currently invalid.
        ///
        /// @remarks
        ///     This is called at the end of a layout validation process.
        void Layout_UpdateElementAbsolutePositions();

        /// Updates the absolute position of the given element, and then recursively does the same thing for it's children.
        ///
        /// @param element [in] The element whose position is being updated.
        void Layout_UpdateElementAbsolutePosition(GUIElement &element);


        /// Posts OnSize and OnMove events to every element whose size and/or position has changed.
        ///
        /// @remarks
        ///     This is called at the end of a layout validation process.
        void Layout_InvalidateRectsAndPostEventsToElements();



        ////////////////////////////
        // Static Helpers
        
        /// Retrieves the horizontal padding (left padding + right padding) of the given element.
        ///
        /// @param element [in] A reference to the element whose horizontal padding is being retrieved.
        static float Layout_GetElementHorizontalPadding(GUIElement &element);

        /// Retrieves the vertical padding (top padding + bottom padding) of the given element.
        ///
        /// @param element [in] A reference to the element whose vertical padding is being retrieved.
        static float Layout_GetElementVerticalPadding(GUIElement &element);

        /// Retrieves the horizontal margin (left margin + right margin) of the given element.
        ///
        /// @param element [in] A reference to the element whose horizontal margin is being retrieved.
        static float Layout_GetElementHorizontalMargin(GUIElement &element);

        /// Retrieves the vertical margin (top margin + bottom margin) of the given element.
        ///
        /// @param element [in] A reference to the element whose vertical margin is being retrieved.
        static float Layout_GetElementVerticalMargin(GUIElement &element);

        /// Retrieves the size of the horizontal borders (left and right borders) of the given element.
        ///
        /// @param element [in] A reference to the element whose horizontal border size is being retrieved.
        static float Layout_GetElementHorizontalBorderSize(GUIElement &element);

        /// Retrieves the size of the vertical borders (top and bottom borders) of the given element.
        ///
        /// @param element [in] A reference to the element whose vertical border size is being retrieved.
        static float Layout_GetElementVerticalBorderSize(GUIElement &element);

        /// Retrieves the width + left margin + right margin of the given element.
        ///
        /// @param element [in] A reference to the element whose outer width is being retrieved.
        ///
        /// @remarks
        ///     The outer width is the width of the element plus the left and right margins.
        static float Layout_GetElementOuterWidth(GUIElement &element);

        /// Retrieves the height + top margin + bottom margin of the given element.
        ///
        /// @param element [in] A reference to the element whose outer height is being retrieved.
        ///
        /// @remarks
        ///     The outer height is the height of the element plus the top and bottom margins.
        static float Layout_GetElementOuterHeight(GUIElement &element);

        /// Retrieves the width minus the left and right padding and borders of the given element.
        ///
        /// @param element [in] A reference to the element whose inner width is being retrieved.
        ///
        /// @remarks
        ///     The inner width is the width of the element minus the left and right padding and borders.
        static float Layout_GetElementInnerWidth(GUIElement &element);

        /// Retrieves the height minus the top and bottom padding and borders of the given element.
        ///
        /// @param element [in] A reference to the element whose inner height is being retrieved.
        ///
        /// @remarks
        ///     The inner height is the height of the element minus the top and bottom padding and borders.
        static float Layout_GetElementInnerHeight(GUIElement &element);

        /// Retrieves the width minus the left and right borders of the given element.
        ///
        /// @param element [in] A reference to the element whose inner border width is being retrieved.
        ///
        /// @remarks
        ///     The inner-border width is the width of the element minus the left and right borders, but including the padding.
        static float Layout_GetElementInnerBorderWidth(GUIElement &element);

        /// Retrieves the height minus the top and bottom borders of the given element.
        ///
        /// @param element [in] A reference to the element whose inner-border height is being retrieved.
        ///
        /// @remarks
        ///     The inner-border height is the height of the element minus the top and bottom borders, but including the padding.
        static float Layout_GetElementInnerBorderHeight(GUIElement &element);

        /// Retrieves the width of the given element that should be used when calculating the relative width of a child.
        ///
        /// @param element [in] A reference to the element whose width is being calculated.
        static float Layout_GetElementWidthForRelativeSizing(GUIElement &element);

        /// Retrieves the height of the given element that should be used when calculating the relative height of a child.
        ///
        /// @param element [in] A reference to the element whose height is being calculated.
        static float Layout_GetElementHeightForRelativeSizing(GUIElement &element);

        /// Determines whether or not the width of the given element is invalid.
        ///
        /// @param element [in] A reference to the element whose width validation state is being determined.
        ///
        /// @return True if the width of the given element is invalid; false otherwise.
        static bool Layout_IsElementWidthInvalid(GUIElement &element);

        /// Determines whether or not the height of the given element is invalid.
        ///
        /// @param element [in] A reference to the element whose height validation state is being determined.
        ///
        /// @return True if the height of the given element is invalid; false otherwise.
        static bool Layout_IsElementHeightInvalid(GUIElement &element);

        /// Determines whether or not the position of the given element is invalid.
        ///
        /// @param element [in] A reference to the element whose position validation state is being determined.
        ///
        /// @return True if the position of the given element is invalid; false otherwise.
        static bool Layout_IsElementPositionInvalid(GUIElement &element);


    private:

        /// The handle manager for elements.
        HandleManager<HGUIElement, GUIElement> m_elementHandles;

        /// The handle manager for surfaces.
        HandleManager<HGUISurface, GUISurface> m_surfaceHandles;


        /// The image manager for creating, deleting and updating images.
        //GUIImageManager_Default m_imageManager;

        /// The font manager for working with fonts. This depends on m_imageManager, so must be declared and initialized afterwards.
        GUIFontManager m_fontManager;

        /// The default font for when elements have not yet been assigned one.
        GTLib::Font* m_defaultFont;


        /// The base DPI on the X axis for calculating the scaling factor. Defaults to 96.
        unsigned int m_xBaseDPI;

        /// The base DPI on yhe Y axis for calculating the scaling factor. Defaults to 96.
        unsigned int m_yBaseDPI;

        


        /// A pointer to the renderer.
        GUIRenderer* m_renderer;

        /// Keeps track of whether or not the context owns the renderer object.
        bool m_ownsRenderer;


        /// Object containing the necessary variables for handling layout updates.
        struct GUILayoutContext
        {
            GUILayoutContext()
                : invalidElements(),
                  validatedElements(),
                  elementsWithInvalidAbsolutePositions()
            {
            }

            /// The elements that have an invalid layout property. We need fast insertions and removals here, so we'll use a linked list.
            GTLib::List<GUIElement*> invalidElements;

            /// The elements whose layout has changed. We use this during the post-process stage of layout validation so we can know which elements to post events to.
            GTLib::Vector<GUIElement*> validatedElements;

            /// The elements that need to have their absolute positions updated.
            GTLib::Vector<GUIElement*> elementsWithInvalidAbsolutePositions;

        } m_layoutContext;


        /// The batch operation lock counter. When this is >0, any operation that is controlled by batching will be locked.
        uint32_t m_batchLockCounter;

        /// The list of global event handlers.
        GTLib::Vector<GUIEventHandler*> m_globalEventHandlers;


#if defined(GT_GUI_DEBUGGING)
        GUILayoutLogger m_layoutLogger;
#endif


    private:    // No copying.
        GUIContext(const GUIContext &);
        GUIContext & operator=(const GUIContext &);
    };
}

#endif
