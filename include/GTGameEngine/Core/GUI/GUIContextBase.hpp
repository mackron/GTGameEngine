// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GUIContextBase_hpp_
#define __GT_GUIContextBase_hpp_

#include "GUIElement.hpp"
#include "GUISurface.hpp"

// TODO:
//      - Replace hSurface with pSurface
//      - Replace hElement with pElement

namespace GT
{
    /// Base class for the GUI context.
    ///
    /// This class should only be used internally.
    ///
    /// The GUI context is built with a layered architectured. This class represents the lowest level layer. This layer works
    /// directly on pointers. Higher level layers abstract pointers into handles.
    ///
    /// This layer does not store a list of surfaces and elements. That needs to be handled at a higher level layer. Higher
    /// level layers need to implement iteration functions.
    class GUIContextBase
    {
    protected:

        /// Constructor.
        GUIContextBase();

        /// Destructor
        virtual ~GUIContextBase();


        /////////////////////////////////////////////////////////////////
        // Surfaces

        /// Creates a new surface.
        ///
        /// @remarks
        ///     The initial size will be 0,0
        GUISurface* CreateSurface();

        /// Deletes the given surface.
        ///
        /// @param pSurface [in] A pointer to the surface to delete.
        void DeleteSurface(GUISurface* pSurface);

        /// Retrieves a handle to a surface by it's ID.
        ///
        /// @param id [in] The ID of the surface to retrieve.
        ///
        /// @return A pointer to the surface with the given ID, or null if a surface of the given ID could not be found.
        ///
        /// @remarks
        ///     This runs in linear time.
        GUISurface* FindSurfaceByID(const char* id) const;

        /// Sets the ID of the given surface.
        ///
        /// @param pSurface [in] A pointer to the surface to delete.
        /// @param newID    [in] The new ID of the surface.
        void SetSurfaceID(GUISurface* pSurface, const char* id);

        /// Retrieves the ID of the given surface.
        ///
        /// @param pSurface [in] The surface whose ID is being retrieved.
        const char* GetSurfaceID(GUISurface* pSurface) const;

        /// Sets the size of a surface.
        ///
        /// @param pSurface  [in] A pointer to the surface whose size is being set.
        /// @param newWidth  [in] The new width of the surface.
        /// @param newHeight [in] The new height of the surface.
        ///
        /// @remarks
        ///     The relevant GUI elements will be resized and repositioned in response to a change in the size of the surface.
        void SetSurfaceSize(GUISurface* pSurface, unsigned int newWidth, unsigned int newHeight);

        /// Retrieves the size of a surface.
        ///
        /// @param hSurface  [in]  The handle to the surface whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        void GetSurfaceSize(GUISurface* pSurface, unsigned int &widthOut, unsigned int &heightOut) const;

        /// Determines whether or not the given surface contains the given element.
        ///
        /// @param hSurface [in] The surface to query.
        /// @param hElement [in] The element to look for.
        ///
        /// @return True if the given element is on the given surface.
        ///
        /// @remarks
        ///     This runs in linear time.
        bool DoesSurfaceContainGUIElement(GUISurface* pSurface, GUIElement* pElement) const;


        /// Sets the given surface's aux. data pointer.
        ///
        /// @param hElement [in] A handle to the surface.
        /// @param auxData  [in] A pointer to the aux. data.
        ///
        /// @remarks
        ///     The aux. data can be retrieved with GetSurfaceAuxData().
        void SetSurfaceAuxData(GUISurface* pSurface, void* auxData)
        {
            if (pSurface != nullptr)
            {
                pSurface->auxData = auxData;
            }
        }

        /// Retrieve's the given surface's aux. data pointer.
        ///
        /// @param hElement [in] A handle to the surface.
        ///
        /// @return A pointer to the aux. data.
        template <typename T>
        T* GetSurfaceAuxData(GUISurface* pSurface)
        {
            if (pSurface != nullptr)
            {
                return reinterpret_cast<T*>(pSurface->auxData);
            }

            return reinterpret_cast<T*>(nullptr);
        }

        /// Sets the painting mode for the given surface - immediate or deferred.
        ///
        /// @param pSurface     [in] A pointer to the surface whose painting mode is being set.
        /// @param paintingMode [in] The new painting mode.
        ///
        /// @remarks
        ///     When \c paintingMode is set to \c GUIPaintingMode::Immediate (the default), painting commands are posted automatically
        ///     and immediately whenever there is a change to the graphical representation of an element that is attached to the given
        ///     surface.
        ///     @par
        ///     When \c paintingMode is set to \c GUIPaintingMode::Deferred, painting commands are posted via an explicit call to
        ///     PaintSurface(). In this case, painting commands are posted such that the entire surface is redrawn.
        void SetSurfacePaintingMode(GUISurface* pSurface, GUIPaintingMode paintingMode);



        /////////////////////////////////////////////////////////////////
        // Elements

        /// Creates a new GUI element.
        ///
        /// @return A pointer to the new GUI element.
        GUIElement* CreateElement();

        /// Deletes a GUI element that was created with CreateGUIElement().
        ///
        /// @param pElement [in] The GUI element to delete.
        void DeleteElement(GUIElement* pElement);

        /// Sets the ID of the given element.
        ///
        /// @param pElement [in] The GUI element whose having it's ID changed.
        /// @param newID    [in] The new ID of the GUI element.
        void SetElementID(GUIElement* pElement, const char* id);

        /// Retrieves the ID of the given element.
        ///
        /// @param pElement [in] The GUI element whose ID is being retrieved.
        ///
        /// @return The ID of the given element.
        const char* GetElementID(GUIElement* pElement) const;

        /// Finds an element by it's ID.
        ///
        /// @param elementID [in] The ID of the element to search for.
        ///
        /// @return
        ///     A pointer to the first element with the given ID, or 0 if an element could not be found.
        GUIElement* FindElementByID(const char* elementID) const;


        /// Retrieves the parent element of the given element.
        ///
        /// @param pChildElement [in] The GUI element whose parent is being retrieved.
        ///
        /// @return A handle to the parent GUI element, or null if it does not have one.
        GUIElement* GetElementParent(GUIElement* pChildElement) const;

        /// Assigns the element to a different parent.
        ///
        /// @param pChildElement  [in] The GUI element that is being reparented.
        /// @param pParentElement [in] The new parent GUI element.
        ///
        /// @remarks
        ///     When pParentElement is not null, this is equivalent to AppendChildGUIElement(pParentElement, hElement). When hParent is null, the element
        ///     will be detached from it's parent.
        void SetElementParent(GUIElement* pChildElement, GUIElement* pParentElement);

        /// Detaches an element from it's parent.
        ///
        /// @param pChildElement [in] The GUI element that is being detached from it's parent.
        ///
        /// @remarks
        ///     This is equivalent to this->RemoveChildGUIElement(pElement->pParent, pElement). If the element does not have a parent, this does
        ///     nothing.
        void DetachElementFromParent(GUIElement* pChildElement);

        /// Detaches an element from it's parent and siblings.
        ///
        /// @param pChildElement [in] The GUI element that is being detached from it's siblings.
        ///
        /// @remarks
        ///     If the element has a parent, it will be detached from that, too. The difference between this and DetachGUIElementFromParent() is that
        ///     that the later will not detach from siblings if it does not have a parent.
        void DetachElementFromParentAndSiblings(GUIElement* pChildElement);

        /// Appends a child GUI element.
        ///
        /// @param pParentElement [in] The GUI element that is having a child element appended.
        /// @param pChildElement  [in] The child GUI element to append.
        ///
        /// @remarks
        ///     An element can be prepended with PrependChildGUIElement().
        void AppendChildElement(GUIElement* pParentElement, GUIElement* pChildElement);

        /// Prepends a child GUI element.
        ///
        /// @param pParentElement [in] The GUI element that is having a child element prepended.
        /// @param pChildElement  [in] The child GUI element to append.
        ///
        /// @remarks
        ///     An element can be appended to the end using AppendChildGUIElement().
        void PrependChildElement(GUIElement* pParentElement, GUIElement* pChildElement);

        /// Appends an element to the given sibling.
        ///
        /// @param pElementToAppendTo [in] A pointer to the element being appended to.
        /// @param pElementToAppend   [in] A pointer to the element to append.
        ///
        /// @remarks
        ///     If the elements have different parents, the element to insert will be detached from it's current parent first.
        void AppendSiblingElement(GUIElement* pElementToAppendTo, GUIElement* pElementToAppend);

        /// Prepends an element to the given sibling.
        ///
        /// @param pElementToPrependTo [in] A pointer to the element being prepended to.
        /// @param pElementToPrepend   [in] A pointer to the element to prepend.
        ///
        /// @remarks
        ///     If the elements have different parents, the element to insert will be detached from it's current parent first.
        void PrependSiblingElement(GUIElement* pElementToPrependTo, GUIElement* pElementToPrepend);

        /// Retrieves a handle to the given element's next sibling.
        ///
        /// @param hElement [in] The GUI element whose next sibling is being retrieved.
        ///
        /// @return A handle to the GUI element of the given element's next sibling, or 0 if the element does not have a sibling or is the last one.
        GUIElement* GetElementNextSibling(GUIElement* pElement) const;

        /// Retrieves a handle to the given element's previous sibling.
        ///
        /// @param hElement [in] The GUI element whose previous sibling is being retrieved.
        ///
        /// @return A handle to the GUI element whose previous sibling is being retrieved.
        GUIElement* GetElementPreviousSibling(GUIElement* pElement) const;


        /// Iterates over the previous siblings of the given element in forward order.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementPrevSiblings(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the next siblings of the given element.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementNextSiblings(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the siblings of the given element, including itself.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each sibling.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementSiblingsAndSelf(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the children of the given element.
        ///
        /// @param pElement [in] The element whose children are being iterated.
        /// @param handler  [in] The function to call for each child.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        ///
        /// @remarks
        ///     This is not recursive. Use IterateElementDescendants() to iterate recursively.
        bool IterateElementChildren(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the descendants of the given element.
        ///
        /// @param pElement [in] The element whose descendants are being iterated.
        /// @param handler  [in] The function to call for each descendant.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        ///
        /// @remarks
        ///     This is recursive.
        bool IterateElementDescendants(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the ancestor of the given element.
        ///
        /// @param pElement [in] The element whose ancestors are being iterated.
        /// @param handler  [in] The function to call for each ancestory.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        ///
        /// @remarks
        ///     This is recursive.
        bool IterateElementAncestors(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;


        /// Determines if the given element is a direct child of the given element.
        ///
        /// @param pParentElement [in] The parent GUI element.
        /// @param pChildElement  [in] The child GUI element.
        ///
        /// @return True of the given element is a direct child of the other element.
        ///
        /// @remarks
        ///     This is not recursive. To recursively test if the element is a descendant, use IsElementDescendant().
        bool IsElementChild(GUIElement* pParentElement, GUIElement* pChildElement) const;

        /// Determines if the given element is the parent of the other given element.
        ///
        /// @param pChildElement  [in] The child GUI element.
        /// @param pParentElement [in] The parent GUI element.
        ///
        /// @return True if the given element is the parent of the other given element.
        ///
        /// @remarks
        ///     This is not recursive. To recursively test if the element is an ancestor, use IsElementAncestor().
        bool IsElementParent(GUIElement* pChildElement, GUIElement* pParentElement) const;

        /// Determines if the given element is a descendant of the given parent.
        ///
        /// @param pParentElement [in] The parent GUI element.
        /// @param pChildElement  [in] The child GUI element.
        ///
        /// @return True if the given child is a descendant of the given element.
        bool IsElementDescendant(GUIElement* pParentElement, GUIElement* pChildElement) const;

        /// Determines if the given element is an ancestor of the given element.
        ///
        /// @param pChildElement    [in] The parent GUI element.
        /// @param pAncestorElement [in] The child GUI element.
        ///
        /// @return True if the given ancestor element is an ancestor of the given element.
        bool IsElementAncestor(GUIElement* pChildElement, GUIElement* pAncestorElement) const;


        /// Sets the relative position of the given element, in DPI-aware points.
        ///
        /// @param pElement [in] The element whose position is being set.
        /// @param xPos     [in] The X position.
        /// @param yPos     [in] The Y position.
        ///
        /// @remarks
        ///     The will set the positioning to relative and give the left/top corder priority.
        void SetElementPosition(GUIElement* pElement, float xPos, float yPos);
        void SetElementPosition(GUIElement* pElement, int xPos, int yPos);

        /// Sets the left style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose left position is being set.
        /// @param left     [in] The new left position.
        ///
        /// @remarks
        ///     This will give the left position priority over right.
        void SetElementLeftPosition(GUIElement* pElement, int left);

        /// Sets the top style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose top position is being set.
        /// @param top      [in] The new top position.
        ///
        /// @remarks
        ///     This will give the top position priority over bottom.
        void SetElementTopPosition(GUIElement* pElement, int top);

        /// Sets the right style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose right position is being set.
        /// @param right    [in] The new right position.
        ///
        /// @remarks
        ///     This will give the right position priority over left.
        void SetElementRightPosition(GUIElement* pElement, int right);

        /// Sets the bottom style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose bottom position is being set.
        /// @param bottom   [in] The new bottom position.
        ///
        /// @remarks
        ///     This will give the bottom position priority over top.
        void SetElementBottomPosition(GUIElement* pElement, int bottom);


        /// Sets whether the element is positioned relative to the inside of the padding, border, or ouside border of the parent element.
        ///
        /// @param hElement [in] The GUI element whose position origin is being set.
        /// @param origin   [in] The new position origin.
        ///
        /// @remarks
        ///     This is only used when the position is relative.
        void SetElementRelativePositionOrigin(GUIElement* pElement, PositionOrigin origin);

        /// Determines whether the element is positioned relative to the inside of the padding, border, or ouside border of the parent element.
        ///
        /// @param hElement [in] The GUI elemetn whose position origin is being retrieved.
        ///
        /// @return The position origin.
        ///
        /// @remarks
        ///     This is only relevant when the position is relative.
        PositionOrigin GetElementRelativePositionOrigin(GUIElement* pElement) const;


        /// Retrieves the actual relative position of the given element.
        ///
        /// @param hElement [in]  The GUI element whose position is being retrieved.
        /// @param xPosOut  [out] A reference to the variable that will receive the X position.
        /// @param yPosOut  [out] A reference to the variable that will receive the Y position.
        void GetElementPosition(GUIElement* pElement, float &xPosOut, float &yPosOut) const;
        void GetElementPosition(GUIElement* pElement, int &xPosOut, int &yPosOut) const;

        /// Retrieves the absolute position of the given element.
        ///
        /// @param hElement [in] The GUI element whose absolute position is being retrieved.
        /// @param xPosOut  [out] A reference to the variable that will receive the absolute X position.
        /// @param yPosOut  [out] A reference to the variable that will receive the absolute Y position.
        ///
        /// @remarks
        ///     This is a simple retrieval function. This will not dynamically compute the position.
        void GetElementAbsolutePosition(GUIElement* pElement, float &xPosOut, float &yPosOut) const;
        void GetElementAbsolutePosition(GUIElement* pElement, int &xPosOut, int &yPosOut) const;

        /// Sets the size of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose size is being set.
        /// @param width    [in] The width of the element.
        /// @param height   [in] The height of the element.
        void SetElementSize(GUIElement* pElement, float width, float height);
        void SetElementSize(GUIElement* pElement, unsigned int width, unsigned int height);

        /// Sets the size of the given element to a ratio of it's parent's size.
        ///
        /// @param hElement    [in] The GUI element whose size is being set.
        /// @param widthRatio  [in] The new width ratio of the element.
        /// @param heightRatio [in] The new height ratio of the element.
        void SetElementSizeRatio(GUIElement* pElement, float widthRatio, float heightRatio);

        /// Sets the size of the given element to tightly wrap it's children.
        ///
        /// @param hElement [in] The GUI element whose size is being set.
        void SetElementSizeToChildren(GUIElement* pElement);



        /////////////////////////////////////////////////////////////////
        // Virtual Methods

        /// Iterates over each surface.
        ///
        /// @param handler [in] The function to call for each surface.
        virtual void IterateSurfaces(std::function<bool (GUISurface* pSurface)> handler) const = 0;

        /// Iterates over each element.
        ///
        /// @param handler [in] The function to call for each element.
        virtual void IterateElements(std::function<bool (GUIElement* pElement)> handler) const = 0;


    private:

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
    };
}

#endif
