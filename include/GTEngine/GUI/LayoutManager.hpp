// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_LayoutManager
#define GT_GUI_LayoutManager

#include "Element.hpp"
#include <GTEngine/Core/BinarySearchTree.hpp>
#include <GTEngine/Core/List.hpp>

namespace GTGUI
{
    class GUILayoutManager
    {
    public:

        /// Constructor.
        GUILayoutManager();

        /// Destructor.
        ~GUILayoutManager();


        /// Marks the width of the given element as invalid.
        void InvalidateWidth(GUIElement &element);

        /// Marks the height of the given element as invalid.
        void InvalidateHeight(GUIElement &element);

        /// Marks the position of the given element as invalid.
        void InvalidatePosition(GUIElement &element);

        /// Marks the text of the given element as invalid.
        void InvalidateText(GUIElement &element);


        /// Validates the layouts of everything that has been invalidated.
        void Validate();


        /// Removes all references to the given element.
        ///
        /// @param element [in] A reference to the element to remove.
        void RemoveElement(GUIElement &element);


    private:

        /// Helper for validating the width of an element.
        void ValidateWidth(GUIElement &element);

        /// Helper for validating the height of an element.
        void ValidateHeight(GUIElement &element);

        /// Helper for validating the position of an element.
        void ValidatePosition(GUIElement &element, bool invalidateSiblings = true);

        /// Helper for validating the position of an auto-positioned element.
        void ValidatePosition_Auto(GUIElement &element, bool validateSiblings);

        /// Helper for validating the position of a relative-positioned element.
        void ValidatePosition_Relative(GUIElement &element);

        /// Helper for validating the position of an absolute-positioned element.
        void ValidatePosition_Absolute(GUIElement &element);

        /// Helper for validating the text of an element.
        void ValidateText(GUIElement &element, bool validateDependants = true);



        /// Helper for marking an element whose had their size changed.
        void MarkSizeAsChanged(GUIElement &element);

        /// Helper for marking an element whose had their position changed.
        void MarkPositionAsChanged(GUIElement &element);

        /// Attempts to mark the element as a top-level element.
        void TryMarkAsTopLevelElement(GUIElement &element);


        /// Determines whether or not the width of the given element is invalid.
        bool IsWidthInvalid(GUIElement &element) const;

        /// Determines whether or not the height of the given element is invalid.
        bool IsHeightInvalid(GUIElement &element) const;

        /// Determines whether or not the position of the given element is invalid.
        bool IsPositionInvalid(GUIElement &element) const;


        /// Retrieves the provoking sibling of the given element.
        ///
        /// @param element [in] A reference to the element whose provoking sibling is being retrieved.
        ///
        /// @remarks
        ///     The provoking sibling is the sibling that comes just before the element in the layout. An element is positioned relative to
        ///     that sibling.
        const GUIElement* GetProvokingSibling(GUIElement &element) const;


        /// Marks the element as not clipping with the parent.
        void MarkElementAsUnclipped(GUIElement &element) const;

        /// Marks the element as clipping with the parent.
        void MarkElementAsClipped(GUIElement &element) const;

        /// Checks if the given element needs to be clipped against the parent and marks it as such.
        void CheckAndMarkElementAsClipped(GUIElement &element) const;


        /// Recursively updates the absolute layout properties of the given element and it's children.
        ///
        /// @element [in] A reference to the element whose absolute layout properties are being updated.
        ///
        /// @remarks
        ///     This is called recursively on children.
        void UpdateAbsoluteLayoutProperties(GUIElement &element) const;


        /// Performs a post-process step on each of the elements that have just had their layouts re-validated.
        void PostProcess();


    private:

        /// The elements that have an invalid layout property. We need fast insertions and removals here, so we'll use a linked list.
        GT::List<GUIElement*> invalidElements;


        /// Structure representing an element that has been validated. This is only used during the
        /// post-process step.
        struct ValidatedElement
        {
            /// A pointer to the element.
            GUIElement* element;

            /// The flags specifying what has changed.
            uint32_t flags;


            /// Constructor.
            ValidatedElement(GUIElement &elementIn)
                : element(&elementIn), flags(0)
            {
            }

            /// Constructor.
            ValidatedElement(GUIElement* elementIn)
                : element(elementIn), flags(0)
            {
            }

            /// Constructor.
            ValidatedElement(GUIElement &elementIn, uint32_t flagsIn)
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
        GT::BinarySearchTree<ValidatedElement> validatedElements;

        /// The list of top level elements that will recursively update absolute layouts during the post-process stage.
        GT::Vector<GUIElement*> topLevelValidatedElements;



        ///////////////////////////////////////////////////////////
        // Static Helpers.

    public:

        /// Evaluates the width of the given element for the given style attribute.
        ///
        /// @param element [in] A reference to the element whose width is being evaluated.
        /// @param width   [in] A reference to the style attribute whose width is being evaluated.
        static int EvaluateWidth(const GUIElement &element, const GUIStyleAttribute_Number &width, bool calculateFlexed = true);

        /// Evaluates the height of the given element for the given style attribute.
        ///
        /// @param element [in] A reference to the element whose height is being evaluated.
        /// @param height  [in] A reference to the style attribute whose height is being evaluated.
        static int EvaluateHeight(const GUIElement &element, const GUIStyleAttribute_Number &height, bool calculateFlexed = true);


        /// Updates the width of the given element based on the current state of the hierarchy.
        ///
        /// @param element [in] A reference to the element whose width is being updated.
        ///
        /// @return The new outer width of the element.
        static int UpdateWidth(GUIElement &element);

        /// Updates the height of the given element based on the current state of the hierarchy.
        ///
        /// @param element [in] A reference to the element whose height is being updated.
        ///
        /// @return The new outer height of the element.
        static int UpdateHeight(GUIElement &element);


    private:

        /// Calculates the total width of the children of the given element.
        ///
        /// @param element [in] A reference to the element whose children are being measured.
        ///
        /// @remarks
        ///     This function is specific to the layout manager in that it performs a slightly different calculation
        ///     for elements who use a % min and/or max width. Use GUIElement::GetChildrenWidth() for a proper calculation.
        static int GetChildrenWidth(const GUIElement &element);

        /// Calculates the total height of the children of the given element.
        ///
        /// @param element [in] A reference to the element whose children are being measured.
        ///
        /// @remarks
        ///     This function is specific to the layout manager in that it performs a slightly different calculation
        ///     for elements who use a % min and/or max height. Use GUIElement::GetChildrenHeight() for a proper calculation.
        static int GetChildrenHeight(const GUIElement &element);
    };
}

#endif
