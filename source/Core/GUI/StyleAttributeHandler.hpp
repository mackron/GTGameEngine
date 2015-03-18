// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __LICKGUI_STYLEATTRIBUTEHANDLER_HPP_
#define __LICKGUI_STYLEATTRIBUTEHANDLER_HPP_

#include <GTLib/GUI/StyleStack.hpp>
#include <GTLib/GUI/Element.hpp>
#include <GTLib/String.hpp>

namespace GTGUI
{
    class Element;

    /**
    *   \brief  Class for handling different style attributes.
    *
    *   A class inherits this class for each style attribute (background-color, border, etc). It handles things like parsing
    *   values.
    *
    *   There is actually two components to this class - the static and non-static components. The static component is used
    *   for taking all of the instantiated attribute handlers and working with them in order to efficiently handle an attribute.
    */
    class StyleAttributeHandler
    {
    public:

        /**
        *   \brief  Constructor.
        */
        StyleAttributeHandler();

        /**
        *   \brief  Destructor.
        */
        virtual ~StyleAttributeHandler();



        /**
        *   \brief  Retrieves a pointer to the name of the attribute this class is handling.
        */
        virtual const char* GetAttributeName() const = 0;

        /**
        *   \brief  Virtual function for applying the value of an attribute.
        */
        virtual bool Set(StyleClass &styleClass, const char* value, ptrdiff_t valueSize) = 0;

        /**
        *   \brief  Virtual function for retrieving the value of an attribute as a string.
        */
        virtual GTLib::String Get(StyleClass &styleClass) const = 0;


        /// Unsets the attribute.
        virtual void Unset(StyleClass &) {};


        /// Sets whether or not the style attribute should be set to 'inherit'.
        ///
        /// @param sc      [in] The style class whose attribute is being set.
        /// @param inherit [in] True if the property should be set to 'inherit'; false otherwise.
        /// @param refresh [in] Whether or not the attribute should be refreshed.
        virtual void SetInherit(StyleClass &, bool, bool) {};



    private:    // No copying.
        StyleAttributeHandler(const StyleAttributeHandler &);
        StyleAttributeHandler & operator=(const StyleAttributeHandler &);
    };
}


#define DECLARE_ATTRIBUTE_HANDLER(name, attribName) \
    struct name : public StyleAttributeHandler \
    { \
        name() : StyleAttributeHandler() {} \
        const char* GetAttributeName() const { return attribName; } \
        \
        bool Set(StyleClass &sc, const char* value, ptrdiff_t valueSize); \
        GTLib::String Get(StyleClass &sc) const; \
        \
        void Unset(StyleClass &sc) \
        { \
            sc.name.inherit = false; \
            sc.name.isset   = false; \
            \
            Refresh(sc); \
        } \
        \
        void SetInherit(StyleClass &sc, bool inheritIn, bool refresh) \
        { \
            sc.name.inherit = inheritIn; \
            sc.name.isset   = inheritIn; /* If we set 'inherit' to true, it means the variable is set. Otherwise, it's unset. */ \
            \
            if (refresh) \
            { \
                Refresh(sc); \
            } \
        } \
        \
        static void Refresh(StyleClass &sc, bool updateElements = true) \
        { \
            for (auto i = sc.hosts.root; i != nullptr; i = i->next) \
            { \
                auto host = i->value; \
                if (host != nullptr) \
                { \
                    RefreshStack(*host); \
                    \
                    if (updateElements) \
                    { \
                        OnChanged(host->owner); \
                    } \
                } \
            } \
        } \
        \
        static void RefreshStack(StyleStack &stack) \
        { \
            auto prev = stack.name; \
            for (auto iClass = stack.classes.root; iClass != nullptr; iClass = iClass->next) \
            { \
                auto value = iClass->value; \
                if (value != nullptr && value->name.isset) \
                { \
                    if (!value->name.inherit) \
                    { \
                        stack.name = &value->name; \
                    } \
                    else \
                    { \
                        stack.name = stack.owner.parent->style.name; \
                    } \
                    \
                    RefreshChildrenStacks(stack); \
                    break; \
                } \
            } \
            \
            if (stack.name != prev && stack.name->value != prev->value) \
            { \
                OnChanged(stack.owner); \
            } \
        } \
        \
        static void RefreshChildrenStacks(StyleStack &stack) \
        { \
            for (auto iChild = stack.owner.firstChild; iChild != nullptr; iChild = iChild->nextSibling) \
            { \
                RefreshStack(iChild->style); \
            } \
        } \
        \
        static void OnChanged(Element &element); \
    };

#define DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(name, attribName) \
    struct name : public StyleAttributeHandler \
    { \
        name() : StyleAttributeHandler() {} \
        const char* GetAttributeName() const { return attribName; } \
        \
        bool Set(StyleClass &sc, const char* value, ptrdiff_t valueSize); \
        GTLib::String Get(StyleClass &sc) const; \
        \
        void Unset(StyleClass &sc); \
        \
        void SetInherit(StyleClass &sc, bool inheritIn, bool refresh); \
        \
    };


namespace GTGUI
{
    // This group is the primitive attribute handlers. Shorthand handlers are after these.
    namespace AttributeHandlers
    {
        DECLARE_ATTRIBUTE_HANDLER(width,                                "width")
        DECLARE_ATTRIBUTE_HANDLER(height,                               "height")
        DECLARE_ATTRIBUTE_HANDLER(minWidth,                             "min-width")
        DECLARE_ATTRIBUTE_HANDLER(maxWidth,                             "max-width")
        DECLARE_ATTRIBUTE_HANDLER(minHeight,                            "min-height")
        DECLARE_ATTRIBUTE_HANDLER(maxHeight,                            "max-height")
        DECLARE_ATTRIBUTE_HANDLER(relativeWidthMode,                    "relative-width-mode")
        DECLARE_ATTRIBUTE_HANDLER(relativeHeightMode,                   "relative-height-mode")
        DECLARE_ATTRIBUTE_HANDLER(flexChildWidth,                       "flex-child-width")
        DECLARE_ATTRIBUTE_HANDLER(flexChildHeight,                      "flex-child-height")
        DECLARE_ATTRIBUTE_HANDLER(backgroundColour,                     "background-color")
        DECLARE_ATTRIBUTE_HANDLER(backgroundImage,                      "background-image")
        DECLARE_ATTRIBUTE_HANDLER(backgroundImageColour,                "background-image-color")
        DECLARE_ATTRIBUTE_HANDLER(backgroundAlignX,                     "background-align-x")
        DECLARE_ATTRIBUTE_HANDLER(backgroundAlignY,                     "background-align-y")
        DECLARE_ATTRIBUTE_HANDLER(backgroundRepeatX,                    "background-repeat-x")
        DECLARE_ATTRIBUTE_HANDLER(backgroundRepeatY,                    "background-repeat-y")
        DECLARE_ATTRIBUTE_HANDLER(borderLeftWidth,                      "border-left-width")
        DECLARE_ATTRIBUTE_HANDLER(borderRightWidth,                     "border-right-width")
        DECLARE_ATTRIBUTE_HANDLER(borderTopWidth,                       "border-top-width")
        DECLARE_ATTRIBUTE_HANDLER(borderBottomWidth,                    "border-bottom-width")
        DECLARE_ATTRIBUTE_HANDLER(borderLeftColour,                     "border-left-color")
        DECLARE_ATTRIBUTE_HANDLER(borderRightColour,                    "border-right-color")
        DECLARE_ATTRIBUTE_HANDLER(borderTopColour,                      "border-top-color")
        DECLARE_ATTRIBUTE_HANDLER(borderBottomColour,                   "border-bottom-color")
        DECLARE_ATTRIBUTE_HANDLER(paddingLeft,                          "padding-left")
        DECLARE_ATTRIBUTE_HANDLER(paddingRight,                         "padding-right")
        DECLARE_ATTRIBUTE_HANDLER(paddingTop,                           "padding-top")
        DECLARE_ATTRIBUTE_HANDLER(paddingBottom,                        "padding-bottom")
        DECLARE_ATTRIBUTE_HANDLER(marginLeft,                           "margin-left")
        DECLARE_ATTRIBUTE_HANDLER(marginRight,                          "margin-right")
        DECLARE_ATTRIBUTE_HANDLER(marginTop,                            "margin-top")
        DECLARE_ATTRIBUTE_HANDLER(marginBottom,                         "margin-bottom")
        DECLARE_ATTRIBUTE_HANDLER(childPlane,                           "child-plane")
        DECLARE_ATTRIBUTE_HANDLER(horizontalAlign,                      "horizontal-align")
        DECLARE_ATTRIBUTE_HANDLER(verticalAlign,                        "vertical-align")
        DECLARE_ATTRIBUTE_HANDLER(cursor,                               "cursor")
        DECLARE_ATTRIBUTE_HANDLER(visible,                              "visible")
        DECLARE_ATTRIBUTE_HANDLER(zIndex,                               "z-index")
        DECLARE_ATTRIBUTE_HANDLER(transparentMouseInput,                "transparent-mouse-input")
        DECLARE_ATTRIBUTE_HANDLER(enabled,                              "enabled")
        DECLARE_ATTRIBUTE_HANDLER(textCursorColour,                     "text-cursor-color")
        DECLARE_ATTRIBUTE_HANDLER(canReceiveFocusFromMouse,             "can-receive-focus-from-mouse")
        DECLARE_ATTRIBUTE_HANDLER(positioning,                          "positioning")
        DECLARE_ATTRIBUTE_HANDLER(left,                                 "left")
        DECLARE_ATTRIBUTE_HANDLER(right,                                "right")
        DECLARE_ATTRIBUTE_HANDLER(top,                                  "top")
        DECLARE_ATTRIBUTE_HANDLER(bottom,                               "bottom")
        DECLARE_ATTRIBUTE_HANDLER(positionOrigin,                       "position-origin")
        DECLARE_ATTRIBUTE_HANDLER(innerOffsetX,                         "inner-offset-x")
        DECLARE_ATTRIBUTE_HANDLER(innerOffsetY,                         "inner-offset-y")
        DECLARE_ATTRIBUTE_HANDLER(fontFamily,                           "font-family")
        DECLARE_ATTRIBUTE_HANDLER(fontSize,                             "font-size")
        DECLARE_ATTRIBUTE_HANDLER(fontWeight,                           "font-weight")
        DECLARE_ATTRIBUTE_HANDLER(fontSlant,                            "font-slant")
        DECLARE_ATTRIBUTE_HANDLER(textColour,                           "text-color")
        DECLARE_ATTRIBUTE_HANDLER(textSelectionColour,                  "text-selection-color")
        DECLARE_ATTRIBUTE_HANDLER(textSelectionBackgroundColour,        "text-selection-background-color")
        DECLARE_ATTRIBUTE_HANDLER(textSelectionBackgroundColourBlurred, "text-selection-background-color-blurred")
        DECLARE_ATTRIBUTE_HANDLER(editableText,                         "editable-text")
        DECLARE_ATTRIBUTE_HANDLER(singleLineText,                       "single-line-text")
        DECLARE_ATTRIBUTE_HANDLER(opacity,                              "opacity")
        DECLARE_ATTRIBUTE_HANDLER(compoundOpacity,                      "compound-opacity")
        DECLARE_ATTRIBUTE_HANDLER(enableShadow,                         "enable-shadow")
        DECLARE_ATTRIBUTE_HANDLER(shadowColour,                         "shadow-color")
        DECLARE_ATTRIBUTE_HANDLER(shadowBlurRadius,                     "shadow-blur-radius")
        DECLARE_ATTRIBUTE_HANDLER(shadowOffsetX,                        "shadow-offset-x")
        DECLARE_ATTRIBUTE_HANDLER(shadowOffsetY,                        "shadow-offset-y")
        DECLARE_ATTRIBUTE_HANDLER(shadowExtrusionX,                     "shadow-extrusion-x")
        DECLARE_ATTRIBUTE_HANDLER(shadowExtrusionY,                     "shadow-extrusion-y")
        DECLARE_ATTRIBUTE_HANDLER(shadowOpacity,                        "shadow-opacity")
        DECLARE_ATTRIBUTE_HANDLER(allowMouseDrag,                       "allow-mouse-drag")
        DECLARE_ATTRIBUTE_HANDLER(constrainMouseDragX,                  "constrain-mouse-drag-x")
        DECLARE_ATTRIBUTE_HANDLER(constrainMouseDragY,                  "constrain-mouse-drag-y")
        DECLARE_ATTRIBUTE_HANDLER(mouseDragClampModeX,                  "mouse-drag-clamp-mode-x")        // none/clickpos/border
        DECLARE_ATTRIBUTE_HANDLER(mouseDragClampModeY,                  "mouse-drag-clamp-mode-y")        // none/clickpos/border
        DECLARE_ATTRIBUTE_HANDLER(allowMouseResize,                     "allow-mouse-resize")
        DECLARE_ATTRIBUTE_HANDLER(leftGripperWidth,                     "left-gripper-width")
        DECLARE_ATTRIBUTE_HANDLER(rightGripperWidth,                    "right-gripper-width")
        DECLARE_ATTRIBUTE_HANDLER(topGripperWidth,                      "top-gripper-width")
        DECLARE_ATTRIBUTE_HANDLER(bottomGripperWidth,                   "bottom-gripper-width")
    }


    // Shorthand attribute handlers. These are handled slightly differently to the primitive ones, thus we'll keep them separate.
    namespace AttributeHandlers
    {
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(backgroundAlign,            "background-align")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(backgroundRepeat,           "background-repeat")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(background,                 "background")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(borderWidth,                "border-width")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(borderColour,               "border-color")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(borderLeft,                 "border-left")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(borderRight,                "border-right")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(borderTop,                  "border-top")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(borderBottom,               "border-bottom")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(border,                     "border")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(padding,                    "padding")
        DECLARE_SHORTHAND_ATTRIBUTE_HANDLER(margin,                     "margin")
    }
}


#endif
