// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUI_Element
#define GT_GUI_Element

#include <GTLib/String.hpp>
#include <GTLib/List.hpp>
#include <GTLib/Vector.hpp>
#include "GUIDataTypes.hpp"
#include "GUIElementStyle.hpp"
#include "GUIEventHandler.hpp"
#include "GUITextLayout.hpp"

namespace GT
{
    static const uint16_t LayoutFlag_RelativeXPositionInvalid = (1 << 0);
    static const uint16_t LayoutFlag_RelativeYPositionInvalid = (1 << 1);
    static const uint16_t LayoutFlag_WidthInvalid             = (1 << 4);
    static const uint16_t LayoutFlag_HeightInvalid            = (1 << 5);
    static const uint16_t LayoutFlag_TextInvalid              = (1 << 6);
    static const uint16_t LayoutFlag_ForceReposition          = (1 << 7);   // Specify this flag when an element changes parent. This is because the position of the element has changed, however the relative position will remain constant and thus won't be detected as a position change.
    static const uint16_t LayoutFlag_ForceWidthRevalidation   = (1 << 8);   // Specify this when the width needs to be re-evaluated regardless of whether or not it's actually changed.
    static const uint16_t LayoutFlag_ForceHeightRevalidation  = (1 << 9);   // As above.
    static const uint16_t LayoutFlag_PositionInvalid          = LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid;
    static const uint16_t LayoutFlag_SizeInvalid              = LayoutFlag_WidthInvalid | LayoutFlag_HeightInvalid;
    static const uint16_t LayoutFlag_SizeAndPositionInvalid   = LayoutFlag_SizeInvalid | LayoutFlag_PositionInvalid;


    static const uint16_t LayoutFlag_PositionChanged = (1 << 0);
    static const uint16_t LayoutFlag_SizeChanged     = (1 << 1);


    struct GUISurface;

    /// The data structure for GUI elements. This structure should only ever be touched by the GUIContext class.
    struct GUIElement
    {
        /// Constructor.
        GUIElement();

        /// Destructor.
        ~GUIElement();


        /// A pointer to the surface this element is part of.
        GUISurface* pSurface;


        /// The ID of the element.
        GTLib::String id;

        /// The layout of the element. This is computed based on the style, hierarchy, etc.
        struct _GUIElementLayout
        {
            _GUIElementLayout()
                : relativePosX(0.0f),
                  relativePosY(0.0f),
                  absolutePosX(0.0f),
                  absolutePosY(0.0f),
                  width(0.0f),
                  height(0.0f),
                  unclampedWidth(0.0f),
                  unclampedHeight(0.0f),
                  marginLeft(0.0f), marginRight(0.0f), marginTop(0.0f), marginBottom(0.0f),
                  borderLeftWidth(0.0f), borderRightWidth(0.0f), borderTopWidth(0.0f), borderBottomWidth(0.0f),
                  paddingLeft(0.0f), paddingRight(0.0f), paddingTop(0.0f), paddingBottom(0.0f),
                  innerOffsetX(0.0f), innerOffsetY(0.0f),
                  invalidFlags(0), changedFlags(0),
                  layoutValidationListItem(nullptr)
            {
            }


            /// The actual relative X position of the element to it's parent's top-left corner.
            float relativePosX;

            /// The actual relative Y position of the element to it's parent's top-left corner.
            float relativePosY;

            /// The actual absolute X position of the element to the surface it is being drawn on.
            float absolutePosX;

            /// The actual absolute Y position of the element to the surface it is being drawn on.
            float absolutePosY;


            /// The actual width of the element.
            float width;

            /// The actual height of the element.
            float height;

            /// The unclamped width of the element (as if it were not clamped between the min and max width.)
            float unclampedWidth;

            /// The unclamped height of the element (as if it were not clamped between the min and max height.)
            float unclampedHeight;


            /// The actual left margin of the element.
            float marginLeft;

            /// The actual right margin of the element.
            float marginRight;

            /// The actual top margin of the element.
            float marginTop;

            /// The actual bottom margin of the element.
            float marginBottom;


            /// The actual left border width of the element.
            float borderLeftWidth;

            /// The actual right border width of the element.
            float borderRightWidth;

            /// The actual top border width of the element.
            float borderTopWidth;

            /// The actual bottom border width of the element.
            float borderBottomWidth;


            /// The actual left padding of the element.
            float paddingLeft;

            /// The actual right padding of the element.
            float paddingRight;

            /// The actual top padding of the element.
            float paddingTop;

            /// The actual bottom padding of the element.
            float paddingBottom;


            /// The inner offset on the X axis.
            float innerOffsetX;

            /// The inner offset on the Y axis.
            float innerOffsetY;


            /// Flags for internal use by the GUI context so that it can quickly know the state of the layout and which layout properties
            /// are invalid and need updating. The was this is designed is such that when this is 0, the layout is entirely valid.
            uint16_t invalidFlags;

            /// Keeps track of a set of flags that specifies which layout properties were recently changed. This will be cleared by the
            /// context at the end of layout validation.
            uint16_t changedFlags;

            /// The list item for the element in the invalid elements list. This is used by the layout validation component of the GUI context.
            GTLib::ListItem<GUIElement*>* layoutValidationListItem;

        } layout;

        /// The style of the element.
        GUIElementStyle style;


        /// A pointer to the parent element.
        GUIElement* pParent;

        /// A pointer to the first child element.
        GUIElement* pFirstChild;

        /// A pointer to the last child element.
        GUIElement* pLastChild;

        /// A pointer to the previous sibling element.
        GUIElement* pPrevSibling;

        /// A pointer to the next sibling element.
        GUIElement* pNextSibling;


        /// The element's font.
        HGUIFont hFont;

        /// The element's text layout.
        GUITextLayout* pTextLayout;


        /// The list of event handlers currently attached to the element.
        GTLib::Vector<GUIEventHandler*> eventHandlers;

        /// The structure containing the inlined event handlers.
        LocalCallbackEventHandlers* pCallbackEventHandlers;


        /// Miscellaneous boolean flags.
        uint32_t flags;



        ///////////////////////////////////////
        // Flags

        static const uint32_t IsMarkedForDeletion = (1 << 0);


    private:    // No copying.
        GUIElement(const GUIElement &);
        GUIElement & operator=(const GUIElement &);
    };

    struct GUIElementWithHandle : public GUIElement
    {
        GUIElementWithHandle(HGUIElement handleIn)
            : GUIElement(), handle(handleIn)
        {
        }

        /// A handle tot eh element.
        HGUIElement handle;
    };
}

#endif
