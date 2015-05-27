// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_Element_hpp_
#define __GT_GUI_Element_hpp_

#include <GTLib/String.hpp>
#include <GTLib/List.hpp>
#include <GTLib/Vector.hpp>
#include "GUIDataTypes.hpp"
#include "GUIElementStyle.hpp"
#include "GUIEventHandler.hpp"
#include <GTLib/Font.hpp>
#include <GTLib/TextManager.hpp>

namespace GT
{
    static const uint16_t LayoutFlag_RelativeXPositionInvalid = (1 << 0);
    static const uint16_t LayoutFlag_RelativeYPositionInvalid = (1 << 1);
    static const uint16_t LayoutFlag_WidthInvalid             = (1 << 4);
    static const uint16_t LayoutFlag_HeightInvalid            = (1 << 5);
    static const uint16_t LayoutFlag_TextInvalid              = (1 << 6);
    
    static const uint16_t LayoutFlag_PositionInvalid          = LayoutFlag_RelativeXPositionInvalid | LayoutFlag_RelativeYPositionInvalid;
    static const uint16_t LayoutFlag_SizeInvalid              = LayoutFlag_WidthInvalid | LayoutFlag_HeightInvalid;
    static const uint16_t LayoutFlag_SizeAndPositionInvalid   = LayoutFlag_SizeInvalid | LayoutFlag_PositionInvalid;


    static const uint16_t LayoutFlag_PositionChanged = (1 << 0);
    static const uint16_t LayoutFlag_SizeChanged     = (1 << 1);

    struct GUISurface;

    /// The data structure for GUI elements. This structure should only ever be touched by the GUIContext class.
    struct GUIElement
    {
    public:

        /// Constructor.
        GUIElement(HGUIElement handle);

        /// Destructor.
        ~GUIElement();



        /// The handle to the element.
        HGUIElement handle;


        /// A handle to the surface this element is part of.
        HGUISurface hSurface;

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
                  flags(0), layoutChangeFlags(0),
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


            /// Flags for internal use by the GUI context so that it can quickly know the state of the layout and which layout properties
            /// are valid and which are invalid and need updating. The was this is designed is such that when this is 0, the layout is
            /// entirely valid.
            uint16_t flags;

            /// Keeps track of a set of flags that specifies which layout properties were recently changed. This will be cleared by the
            /// context at the end of layout validation.
            uint16_t layoutChangeFlags;
            
            /// The list item for the element in the invalid elements list. This is used by the layout validation component of the GUI context.
            GTLib::ListItem<GUIElement*>* layoutValidationListItem;

        } layout;

        /// The style of the element.
        GUIElementStyle style;


        /// A handle to the parent element.
        HGUIElement parent;

        /// A handle to the first child element.
        HGUIElement firstChild;

        /// A handle to the last child element.
        HGUIElement lastChild;

        /// A handle to the previous sibling element.
        HGUIElement prevSibling;

        /// A handle to the next sibling element.
        HGUIElement nextSibling;


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


        /// The element's text manager.
        GTLib::TextManager textManager;


        /// The list of event handlers currently attached to the element.
        GTLib::Vector<GUIEventHandler*> eventHandlers;


    private:    // No copying.
        GUIElement(const GUIElement &);
        GUIElement & operator=(const GUIElement &);
    };
}

#endif
