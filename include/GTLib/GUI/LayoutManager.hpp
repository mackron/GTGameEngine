// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_LayoutManager_hpp_
#define __GTLib_GUI_LayoutManager_hpp_

#include "Element.hpp"
#include <GTLib/BinarySearchTree.hpp>
#include <GTLib/List.hpp>

namespace GTGUI
{
    class LayoutManager
    {
    public:

        /// Constructor.
        LayoutManager();

        /// Destructor.
        ~LayoutManager();


        /// Marks the width of the given element as invalid.
        void InvalidateWidth(Element &element);

        /// Marks the height of the given element as invalid.
        void InvalidateHeight(Element &element);

        /// Marks the position of the given element as invalid.
        void InvalidatePosition(Element &element);

        /// Marks the text of the given element as invalid.
        void InvalidateText(Element &element);


        /// Validates the layouts of everything that has been invalidated.
        void Validate();


        /// Removes all references to the given element.
        ///
        /// @param element [in] A reference to the element to remove.
        void RemoveElement(Element &element);


    private:

        /// Helper for validating the width of an element.
        void ValidateWidth(Element &element);

        /// Helper for validating the height of an element.
        void ValidateHeight(Element &element);

        /// Helper for validating the position of an element.
        void ValidatePosition(Element &element, bool invalidateSiblings = true);

        /// Helper for validating the position of an auto-positioned element.
        void ValidatePosition_Auto(Element &element, bool validateSiblings);

        /// Helper for validating the position of a relative-positioned element.
        void ValidatePosition_Relative(Element &element);

        /// Helper for validating the position of an absolute-positioned element.
        void ValidatePosition_Absolute(Element &element);

        /// Helper for validating the text of an element.
        void ValidateText(Element &element, bool validateDependants = true);



        /// Helper for marking an element whose had their size changed.
        void MarkSizeAsChanged(Element &element);

        /// Helper for marking an element whose had their position changed.
        void MarkPositionAsChanged(Element &element);

        /// Attempts to mark the element as a top-level element.
        void TryMarkAsTopLevelElement(Element &element);


        /// Determines whether or not the width of the given element is invalid.
        bool IsWidthInvalid(Element &element) const;

        /// Determines whether or not the height of the given element is invalid.
        bool IsHeightInvalid(Element &element) const;

        /// Determines whether or not the position of the given element is invalid.
        bool IsPositionInvalid(Element &element) const;


        /// Retrieves the provoking sibling of the given element.
        ///
        /// @param element [in] A reference to the element whose provoking sibling is being retrieved.
        ///
        /// @remarks
        ///     The provoking sibling is the sibling that comes just before the element in the layout. An element is positioned relative to
        ///     that sibling.
        const Element* GetProvokingSibling(Element &element) const;


        /// Marks the element as not clipping with the parent.
        void MarkElementAsUnclipped(Element &element) const;

        /// Marks the element as clipping with the parent.
        void MarkElementAsClipped(Element &element) const;

        /// Checks if the given element needs to be clipped against the parent and marks it as such.
        void CheckAndMarkElementAsClipped(Element &element) const;


        /// Recursively updates the absolute layout properties of the given element and it's children.
        ///
        /// @element [in] A reference to the element whose absolute layout properties are being updated.
        ///
        /// @remarks
        ///     This is called recursively on children.
        void UpdateAbsoluteLayoutProperties(Element &element) const;


        /// Performs a post-process step on each of the elements that have just had their layouts re-validated.
        void PostProcess();


    private:

        /// The elements that have an invalid layout property. We need fast insertions and removals here, so we'll use a linked list.
        GTLib::List<Element*> invalidElements;


        /// Structure representing an element that has been validated. This is only used during the
        /// post-process step.
        struct ValidatedElement
        {
            /// A pointer to the element.
            Element* element;

            /// The flags specifying what has changed.
            uint32_t flags;


            /// Constructor.
            ValidatedElement(Element &elementIn)
                : element(&elementIn), flags(0)
            {
            }

            /// Constructor.
            ValidatedElement(Element* elementIn)
                : element(elementIn), flags(0)
            {
            }

            /// Constructor.
            ValidatedElement(Element &elementIn, uint32_t flagsIn)
                : element(&elementIn), flags(flagsIn)
            {
            }



            bool operator<(const ValidatedElement &other) const
            {
                return element < other.element;
            }

            bool operator>(const ValidatedElement &other) const
            {
                return element > other.element;
            }

            bool operator==(const ValidatedElement &other) const
            {
                return element == other.element;
            }


            enum Flags
            {
                SizeChanged     = (1 << 1),
                PositionChanged = (1 << 2),
            };
        };

        /// The list of elements who have had their layout validated.
        GTLib::BinarySearchTree<ValidatedElement> validatedElements;

        /// The list of top level elements that will recursively update absolute layouts during the post-process stage.
        GTLib::Vector<Element*> topLevelValidatedElements;



        ///////////////////////////////////////////////////////////
        // Static Helpers.

    public:

        /// Evaluates the width of the given element for the given style attribute.
        ///
        /// @param element [in] A reference to the element whose width is being evaluated.
        /// @param width   [in] A reference to the style attribute whose width is being evaluated.
        static int EvaluateWidth(const Element &element, const StyleAttribute_Number &width, bool calculateFlexed = true);

        /// Evaluates the height of the given element for the given style attribute.
        ///
        /// @param element [in] A reference to the element whose height is being evaluated.
        /// @param height  [in] A reference to the style attribute whose height is being evaluated.
        static int EvaluateHeight(const Element &element, const StyleAttribute_Number &height, bool calculateFlexed = true);


        /// Updates the width of the given element based on the current state of the hierarchy.
        ///
        /// @param element [in] A reference to the element whose width is being updated.
        ///
        /// @return The new outer width of the element.
        static int UpdateWidth(Element &element);

        /// Updates the height of the given element based on the current state of the hierarchy.
        ///
        /// @param element [in] A reference to the element whose height is being updated.
        ///
        /// @return The new outer height of the element.
        static int UpdateHeight(Element &element);


    private:

        /// Calculates the total width of the children of the given element.
        ///
        /// @param element [in] A reference to the element whose children are being measured.
        ///
        /// @remarks
        ///     This function is specific to the layout manager in that it performs a slightly different calculation
        ///     for elements who use a % min and/or max width. Use Element::GetChildrenWidth() for a proper calculation.
        static int GetChildrenWidth(const Element &element);

        /// Calculates the total height of the children of the given element.
        ///
        /// @param element [in] A reference to the element whose children are being measured.
        ///
        /// @remarks
        ///     This function is specific to the layout manager in that it performs a slightly different calculation
        ///     for elements who use a % min and/or max height. Use Element::GetChildrenHeight() for a proper calculation.
        static int GetChildrenHeight(const Element &element);
    };
}

#endif
