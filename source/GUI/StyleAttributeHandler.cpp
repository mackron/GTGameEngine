// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include "StyleAttributeHandler.hpp"
#include <GTEngine/GUI/StyleStack.hpp>
#include <GTEngine/GUI/Element.hpp>
#include <GTEngine/GUI/Server.hpp>
#include <GTEngine/Core/Strings/Tokenizer.hpp>

/* !!! Don't forget to add new attribute handlers to StyleServer::LoadStyleAttributeHandlers(). It's at the end of StyleServer.cpp. !!! */

namespace GTGUI
{
    // Primitive style attributes. Shorthand attributes are below these ones.
    namespace AttributeHandlers
    {
        // width
        bool width::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.width.SetFromString(value, valueSize))
            {
                // Can not be negative...
                if (sc.width.value < 0)
                {
                    sc.width.value = 0;
                }

                Refresh(sc);
                return true;
            }
        
            return false;
        }
        GT::String width::Get(StyleClass &sc) const
        {
            return sc.width.GetValueAsString();
        }
        void width::OnChanged(Element &element)
        {
            element.server.OnWidthChanged(element);
        }

        // height
        bool height::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.height.SetFromString(value, valueSize))
            {
                // Can not be negative...
                if (sc.height.value < 0)
                {
                    sc.height.value = 0;
                }

                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String height::Get(StyleClass &sc) const
        {
            return sc.height.GetValueAsString();
        }
        void height::OnChanged(Element &element)
        {
            element.server.OnHeightChanged(element);
        }


        // min-width
        bool minWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.minWidth.SetFromString(value, valueSize))
            {
                // Can not be negative...
                if (sc.minWidth.value < 0)
                {
                    sc.minWidth.value = 0;
                }

                Refresh(sc);
                return true;
            }
        
            return false;
        }
        GT::String minWidth::Get(StyleClass &sc) const
        {
            return sc.minWidth.GetValueAsString();
        }
        void minWidth::OnChanged(Element &element)
        {
            element.server.OnWidthChanged(element);
        }

        // max-width
        bool maxWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.maxWidth.SetFromString(value, valueSize))
            {
                // Can not be negative...
                if (sc.maxWidth.value < 0)
                {
                    sc.maxWidth.value = 0;
                }

                Refresh(sc);
                return true;
            }
        
            return false;
        }
        GT::String maxWidth::Get(StyleClass &sc) const
        {
            return sc.maxWidth.GetValueAsString();
        }
        void maxWidth::OnChanged(Element &element)
        {
            element.server.OnWidthChanged(element);
        }


        // min-height
        bool minHeight::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.minHeight.SetFromString(value, valueSize))
            {
                // Can not be negative...
                if (sc.minHeight.value < 0)
                {
                    sc.minHeight.value = 0;
                }

                Refresh(sc);
                return true;
            }
        
            return false;
        }
        GT::String minHeight::Get(StyleClass &sc) const
        {
            return sc.minHeight.GetValueAsString();
        }
        void minHeight::OnChanged(Element &element)
        {
            element.server.OnHeightChanged(element);
        }

        // max-height
        bool maxHeight::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.maxHeight.SetFromString(value, valueSize))
            {
                // Can not be negative...
                if (sc.maxHeight.value < 0)
                {
                    sc.maxHeight.value = 0;
                }

                Refresh(sc);
                return true;
            }
        
            return false;
        }
        GT::String maxHeight::Get(StyleClass &sc) const
        {
            return sc.maxHeight.GetValueAsString();
        }
        void maxHeight::OnChanged(Element &element)
        {
            element.server.OnHeightChanged(element);
        }


        // relative-width-mode
        bool relativeWidthMode::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.relativeWidthMode.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String relativeWidthMode::Get(StyleClass &sc) const
        {
            return sc.relativeWidthMode.GetValueAsString();
        }
        void relativeWidthMode::OnChanged(Element &)
        {
        }


        // relative-height-mode
        bool relativeHeightMode::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.relativeHeightMode.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String relativeHeightMode::Get(StyleClass &sc) const
        {
            return sc.relativeHeightMode.GetValueAsString();
        }
        void relativeHeightMode::OnChanged(Element &)
        {
        }


        // flex-child-width
        bool flexChildWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.flexChildWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String flexChildWidth::Get(StyleClass &sc) const
        {
            return sc.flexChildWidth.GetValueAsString();
        }
        void flexChildWidth::OnChanged(Element &element)
        {
            element.server.OnFlexWidthChanged(element);
        }


        // flex-child-height
        bool flexChildHeight::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.flexChildHeight.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String flexChildHeight::Get(StyleClass &sc) const
        {
            return sc.flexChildHeight.GetValueAsString();
        }
        void flexChildHeight::OnChanged(Element &element)
        {
            element.server.OnFlexHeightChanged(element);
        }

    
        // background-color
        bool backgroundColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundColour::Get(StyleClass &sc) const
        {
            return sc.backgroundColour.GetValueAsString();
        }
        void backgroundColour::OnChanged(Element &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-image
        bool backgroundImage::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundImage.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundImage::Get(StyleClass &sc) const
        {
            return sc.backgroundImage.GetValueAsString();
        }
        void backgroundImage::OnChanged(Element &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-image-color
        bool backgroundImageColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundImageColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundImageColour::Get(StyleClass &sc) const
        {
            return sc.backgroundImageColour.GetValueAsString();
        }
        void backgroundImageColour::OnChanged(Element &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-align-x
        bool backgroundAlignX::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundAlignX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundAlignX::Get(StyleClass &sc) const
        {
            return sc.backgroundAlignX.GetValueAsString();
        }
        void backgroundAlignX::OnChanged(Element &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-align-x
        bool backgroundAlignY::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundAlignY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundAlignY::Get(StyleClass &sc) const
        {
            return sc.backgroundAlignY.GetValueAsString();
        }
        void backgroundAlignY::OnChanged(Element &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-repeat-x
        bool backgroundRepeatX::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundRepeatX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundRepeatX::Get(StyleClass &sc) const
        {
            return sc.backgroundRepeatX.GetValueAsString();
        }
        void backgroundRepeatX::OnChanged(Element &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-repeat-y
        bool backgroundRepeatY::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundRepeatY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundRepeatY::Get(StyleClass &sc) const
        {
            return sc.backgroundRepeatY.GetValueAsString();
        }
        void backgroundRepeatY::OnChanged(Element &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // border-left-width
        bool borderLeftWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderLeftWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderLeftWidth::Get(StyleClass &sc) const
        {
            return sc.borderLeftWidth.GetValueAsString();
        }
        void borderLeftWidth::OnChanged(Element &element)
        {
            element.server.OnPaddingXChanged(element);
            element.InvalidateBorderRenderingData();
        }

    
        // border-right-width
        bool borderRightWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderRightWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderRightWidth::Get(StyleClass &sc) const
        {
            return sc.borderRightWidth.GetValueAsString();
        }
        void borderRightWidth::OnChanged(Element &element)
        {
            element.server.OnPaddingXChanged(element);
            element.InvalidateBorderRenderingData();
        }

    
        // border-top-width
        bool borderTopWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderTopWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderTopWidth::Get(StyleClass &sc) const
        {
            return sc.borderTopWidth.GetValueAsString();
        }
        void borderTopWidth::OnChanged(Element &element)
        {
            element.server.OnPaddingYChanged(element);
            element.InvalidateBorderRenderingData();
        }

    
        // border-bottom-width
        bool borderBottomWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderBottomWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderBottomWidth::Get(StyleClass &sc) const
        {
            return sc.borderBottomWidth.GetValueAsString();
        }
        void borderBottomWidth::OnChanged(Element &element)
        {
            element.server.OnPaddingYChanged(element);
            element.InvalidateBorderRenderingData();
        }


        // border-left-color
        bool borderLeftColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderLeftColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderLeftColour::Get(StyleClass &sc) const
        {
            return sc.borderLeftColour.GetValueAsString();
        }
        void borderLeftColour::OnChanged(Element &element)
        {
            element.InvalidateBorderRenderingData();
        }

    
        // border-right-color
        bool borderRightColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderRightColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderRightColour::Get(StyleClass &sc) const
        {
            return sc.borderRightColour.GetValueAsString();
        }
        void borderRightColour::OnChanged(Element &element)
        {
            element.InvalidateBorderRenderingData();
        }

    
        // border-top-color
        bool borderTopColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderTopColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderTopColour::Get(StyleClass &sc) const
        {
            return sc.borderTopColour.GetValueAsString();
        }
        void borderTopColour::OnChanged(Element &element)
        {
            element.InvalidateBorderRenderingData();
        }

    
        // border-bottom-color
        bool borderBottomColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderBottomColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderBottomColour::Get(StyleClass &sc) const
        {
            return sc.borderBottomColour.GetValueAsString();
        }
        void borderBottomColour::OnChanged(Element &element)
        {
            element.InvalidateBorderRenderingData();
        }


        // padding-left
        bool paddingLeft::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.paddingLeft.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String paddingLeft::Get(StyleClass &sc) const
        {
            return sc.paddingLeft.GetValueAsString();
        }
        void paddingLeft::OnChanged(Element &element)
        {
            element.server.OnPaddingXChanged(element);
        }


        // padding-right
        bool paddingRight::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.paddingRight.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String paddingRight::Get(StyleClass &sc) const
        {
            return sc.paddingRight.GetValueAsString();
        }
        void paddingRight::OnChanged(Element &element)
        {
            element.server.OnPaddingXChanged(element);
        }

    
        // padding-top
        bool paddingTop::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.paddingTop.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String paddingTop::Get(StyleClass &sc) const
        {
            return sc.paddingTop.GetValueAsString();
        }
        void paddingTop::OnChanged(Element &element)
        {
            element.server.OnPaddingYChanged(element);
        }


        // padding-bottom
        bool paddingBottom::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.paddingBottom.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String paddingBottom::Get(StyleClass &sc) const
        {
            return sc.paddingBottom.GetValueAsString();
        }
        void paddingBottom::OnChanged(Element &element)
        {
            element.server.OnPaddingYChanged(element);
        }


        // margin-left
        bool marginLeft::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.marginLeft.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String marginLeft::Get(StyleClass &sc) const
        {
            return sc.marginLeft.GetValueAsString();
        }
        void marginLeft::OnChanged(Element &element)
        {
            element.server.OnMarginsXChanged(element);
        }

    
        // margin-right
        bool marginRight::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.marginRight.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String marginRight::Get(StyleClass &sc) const
        {
            return sc.marginRight.GetValueAsString();
        }
        void marginRight::OnChanged(Element &element)
        {
            element.server.OnMarginsXChanged(element);
        }

    
        // margin-top
        bool marginTop::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.marginTop.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String marginTop::Get(StyleClass &sc) const
        {
            return sc.marginTop.GetValueAsString();
        }
        void marginTop::OnChanged(Element &element)
        {
            element.server.OnMarginsYChanged(element);
        }

    
        // margin-bottom
        bool marginBottom::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.marginBottom.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String marginBottom::Get(StyleClass &sc) const
        {
            return sc.marginBottom.GetValueAsString();
        }
        void marginBottom::OnChanged(Element &element)
        {
            element.server.OnMarginsYChanged(element);
        }


        // child-plane
        bool childPlane::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (GT::Strings::Equal(value, valueSize, "horizontal") ||
                GT::Strings::Equal(value, valueSize, "vertical"))
            {
                if (sc.childPlane.SetFromString(value, valueSize))
                {
                    Refresh(sc);
                    return true;
                }
            }

            return false;
        }
        GT::String childPlane::Get(StyleClass &sc) const
        {
            return sc.childPlane.GetValueAsString();
        }
        void childPlane::OnChanged(Element &element)
        {
            element.server.OnChildPlaneChanged(element);
        }

    
        // horizontal-align
        bool horizontalAlign::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (GT::Strings::Equal(value, valueSize, "left")  ||
                GT::Strings::Equal(value, valueSize, "right") ||
                GT::Strings::Equal(value, valueSize, "center"))
            {
                if (sc.horizontalAlign.SetFromString(value, valueSize))
                {
                    Refresh(sc);
                    return true;
                }
            }

            return false;
        }
        GT::String horizontalAlign::Get(StyleClass &sc) const
        {
            return sc.horizontalAlign.GetValueAsString();
        }
        void horizontalAlign::OnChanged(Element &element)
        {
            element.server.OnHorizontalAlignChanged(element);
        }


        // vertical-align
        bool verticalAlign::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (GT::Strings::Equal(value, valueSize, "top")    ||
                GT::Strings::Equal(value, valueSize, "bottom") ||
                GT::Strings::Equal(value, valueSize, "center"))
            {
                if (sc.verticalAlign.SetFromString(value, valueSize))
                {
                    Refresh(sc);
                    return true;
                }
            }

            return false;
        }
        GT::String verticalAlign::Get(StyleClass &sc) const
        {
            return sc.verticalAlign.GetValueAsString();
        }
        void verticalAlign::OnChanged(Element &element)
        {
            element.server.OnVerticalAlignChanged(element);
        }


        // cursor
        bool cursor::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (GT::Strings::Equal(value, valueSize, "arrow") ||
                GT::Strings::Equal(value, valueSize, "beam")  ||
                GT::Strings::Equal(value, valueSize, "hand"))
            {
                if (sc.cursor.SetFromString(value, valueSize))
                {
                    Refresh(sc);
                    return true;
                }
            }

            return false;
        }
        GT::String cursor::Get(StyleClass &sc) const
        {
            return sc.cursor.GetValueAsString();
        }
        void cursor::OnChanged(Element &)
        {
        }


        // visible
        bool visible::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.visible.SetFromString(value, valueSize))
            {
                this->Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String visible::Get(StyleClass &sc) const
        {
            return sc.visible.GetValueAsString();
        }
        void visible::OnChanged(Element &element)
        {
            element.server.OnVisibilityChanged(element);
        }


        // z-index
        bool zIndex::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.zIndex.SetFromString(value, valueSize))
            {
                // Can not be negative...
                if (sc.zIndex.value < 0)
                {
                    sc.zIndex.value = 0;
                }

                Refresh(sc);
                return true;
            }
        
            return false;
        }
        GT::String zIndex::Get(StyleClass &sc) const
        {
            return sc.zIndex.GetValueAsString();
        }
        void zIndex::OnChanged(Element &element)
        {
            element.server.OnZIndexChanged(element);
        }


        // transparent-mouse-input
        bool transparentMouseInput::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.transparentMouseInput.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String transparentMouseInput::Get(StyleClass &sc) const
        {
            return sc.transparentMouseInput.GetValueAsString();
        }
        void transparentMouseInput::OnChanged(Element &)
        {
        }


        // enabled
        void RecursivelyActivateDisabledModifierClass(Element &element)
        {
            element.style.DeactivateAllModifierClasses();
            element.style.ActivateModifierClasses(StyleClassType_Disabled);

            // If the focused element has just been disabled, it needs to lose focus.
            if (element.server.GetFocusedElement() == &element)
            {
                element.server.BlurFocusedElement();
            }


            // Now children.
            for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
            {
                RecursivelyActivateDisabledModifierClass(*iChild);
            }
        }

        void RecursivelyDeactivateDisabledModifierClass(Element &element)
        {
            if (element.style.enabled->value)
            {
                element.style.DeactivateModifierClasses(StyleClassType_Disabled);


                // Children.
                for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
                {
                    RecursivelyDeactivateDisabledModifierClass(*iChild);
                }
            }
        }

        bool enabled::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.enabled.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String enabled::Get(StyleClass &sc) const
        {
            return sc.enabled.GetValueAsString();
        }
        void enabled::OnChanged(Element &element)
        {
            if (!element.style.enabled->value)
            {
                RecursivelyActivateDisabledModifierClass(element);
            }
            else
            {
                RecursivelyDeactivateDisabledModifierClass(element);
            }

            
            // This will ensure we have the correct cursor.
            element.server.InvalidateMouse();
        }


        // text-cursor-color
        bool textCursorColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textCursorColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textCursorColour::Get(StyleClass &sc) const
        {
            return sc.textCursorColour.GetValueAsString();
        }
        void textCursorColour::OnChanged(Element &)
        {
        }
        
        
        // can-receive-focus-from-mouse
        bool canReceiveFocusFromMouse::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.canReceiveFocusFromMouse.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String canReceiveFocusFromMouse::Get(StyleClass &sc) const
        {
            return sc.canReceiveFocusFromMouse.GetValueAsString();
        }
        void canReceiveFocusFromMouse::OnChanged(Element &)
        {
        }


        // positioning
        bool positioning::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (GT::Strings::Equal(value, valueSize, "auto")     ||
                GT::Strings::Equal(value, valueSize, "relative") ||
                GT::Strings::Equal(value, valueSize, "absolute"))
            {
                if (sc.positioning.SetFromString(value, valueSize))
                {
                    Refresh(sc);
                    return true;
                }
            }

            return false;
        }
        GT::String positioning::Get(StyleClass &sc) const
        {
            return sc.positioning.GetValueAsString();
        }
        void positioning::OnChanged(Element &element)
        {
            element.server.OnPositioningChanged(element);
        }


        // left
        bool left::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.left.SetFromString(value, valueSize))
            {
                sc.rightHasPriority = false;       // <-- Set this before refreshing.
                Refresh(sc);
            
                return true;
            }

            return false;
        }
        GT::String left::Get(StyleClass &sc) const
        {
            return sc.left.GetValueAsString();
        }
        void left::OnChanged(Element &element)
        {
            element.server.OnLeftChanged(element);
        }


        // right
        bool right::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.right.SetFromString(value, valueSize))
            {
                sc.rightHasPriority = true;       // <-- Set this before refreshing.
                Refresh(sc);
            
                return true;
            }

            return false;
        }
        GT::String right::Get(StyleClass &sc) const
        {
            return sc.right.GetValueAsString();
        }
        void right::OnChanged(Element &element)
        {
            element.server.OnRightChanged(element);
        }


        // top
        bool top::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.top.SetFromString(value, valueSize))
            {
                sc.bottomHasPriority = false;      // <-- Set this before refreshing.
                Refresh(sc);
            
                return true;
            }

            return false;
        }
        GT::String top::Get(StyleClass &sc) const
        {
            return sc.top.GetValueAsString();
        }
        void top::OnChanged(Element &element)
        {
            element.server.OnTopChanged(element);
        }


        // bottom
        bool bottom::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.bottom.SetFromString(value, valueSize))
            {
                sc.bottomHasPriority = true;      // <-- Set this before refreshing.
                Refresh(sc);
            
                return true;
            }

            return false;
        }
        GT::String bottom::Get(StyleClass &sc) const
        {
            return sc.bottom.GetValueAsString();
        }
        void bottom::OnChanged(Element &element)
        {
            element.server.OnBottomChanged(element);
        }


        // position-origin
        bool positionOrigin::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.positionOrigin.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String positionOrigin::Get(StyleClass &sc) const
        {
            return sc.positionOrigin.GetValueAsString();
        }
        void positionOrigin::OnChanged(Element &element)
        {
            if (element.style.rightHasPriority)
            {
                element.server.OnRightChanged(element);
            }
            else
            {
                element.server.OnLeftChanged(element);
            }

            if (element.style.bottomHasPriority)
            {
                element.server.OnBottomChanged(element);
            }
            else
            {
                element.server.OnTopChanged(element);
            }
        }


        // inner-offset-x
        bool innerOffsetX::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.innerOffsetX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String innerOffsetX::Get(StyleClass &sc) const
        {
            return sc.innerOffsetX.GetValueAsString();
        }
        void innerOffsetX::OnChanged(Element &element)
        {
            element.server.OnInnerOffsetXChanged(element);
        }


        // inner-offset-y
        bool innerOffsetY::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.innerOffsetY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String innerOffsetY::Get(StyleClass &sc) const
        {
            return sc.innerOffsetY.GetValueAsString();
        }
        void innerOffsetY::OnChanged(Element &element)
        {
            element.server.OnInnerOffsetYChanged(element);
        }


        // font-family
        bool fontFamily::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.fontFamily.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String fontFamily::Get(StyleClass &sc) const
        {
            return sc.fontFamily.GetValueAsString();
        }
        void fontFamily::OnChanged(Element &element)
        {
            element.UpdateFontFromStyle();
        }

    
        // font-size
        bool fontSize::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.fontSize.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String fontSize::Get(StyleClass &sc) const
        {
            return sc.fontSize.GetValueAsString();
        }
        void fontSize::OnChanged(Element &element)
        {
            element.UpdateFontFromStyle();
        }


        // font-weight
        bool fontWeight::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.fontWeight.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }
            
            return false;
        }
        GT::String fontWeight::Get(StyleClass &sc) const
        {
            return sc.fontWeight.GetValueAsString();
        }
        void fontWeight::OnChanged(Element &element)
        {
            element.UpdateFontFromStyle();
        }
        
        
        // font-slant
        bool fontSlant::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.fontSlant.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }
            
            return false;
        }
        GT::String fontSlant::Get(StyleClass &sc) const
        {
            return sc.fontSlant.GetValueAsString();
        }
        void fontSlant::OnChanged(Element &element)
        {
            element.UpdateFontFromStyle();
        }


        // text-color
        bool textColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textColour::Get(StyleClass &sc) const
        {
            return sc.textColour.GetValueAsString();
        }
        void textColour::OnChanged(Element &element)
        {
            element.InvalidateTextRenderingData();
        }

        // text-selection-color
        bool textSelectionColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textSelectionColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textSelectionColour::Get(StyleClass &sc) const
        {
            return sc.textSelectionColour.GetValueAsString();
        }
        void textSelectionColour::OnChanged(Element &element)
        {
            element.InvalidateTextRenderingData();
        }

        // text-selection-background-color
        bool textSelectionBackgroundColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textSelectionBackgroundColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textSelectionBackgroundColour::Get(StyleClass &sc) const
        {
            return sc.textSelectionBackgroundColour.GetValueAsString();
        }
        void textSelectionBackgroundColour::OnChanged(Element &element)
        {
            element.InvalidateTextRenderingData();
        }

        // text-selection-background-color-blurred
        bool textSelectionBackgroundColourBlurred::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textSelectionBackgroundColourBlurred.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textSelectionBackgroundColourBlurred::Get(StyleClass &sc) const
        {
            return sc.textSelectionBackgroundColourBlurred.GetValueAsString();
        }
        void textSelectionBackgroundColourBlurred::OnChanged(Element &element)
        {
            element.InvalidateTextRenderingData();
        }




        // editable-text
        bool editableText::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.editableText.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String editableText::Get(StyleClass &sc) const
        {
            return sc.editableText.GetValueAsString();
        }
        void editableText::OnChanged(Element &)
        {
        }


        // single-line-text
        bool singleLineText::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.singleLineText.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String singleLineText::Get(StyleClass &sc) const
        {
            return sc.singleLineText.GetValueAsString();
        }
        void singleLineText::OnChanged(Element &)
        {
            // TODO: Update the text manager.
        }

        

        // opacity
        bool opacity::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.opacity.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String opacity::Get(StyleClass &sc) const
        {
            return sc.opacity.GetValueAsString();
        }
        void opacity::OnChanged(Element &element)
        {
            element.UpdateAbsoluteOpacity();    // <-- Recursive on children.
        }


        // compound-opacity
        bool compoundOpacity::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.compoundOpacity.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String compoundOpacity::Get(StyleClass &sc) const
        {
            return sc.compoundOpacity.GetValueAsString();
        }
        void compoundOpacity::OnChanged(Element &element)
        {
            element.UpdateAbsoluteOpacity();    // <-- Recursive on children.
        }


        // enable-shadow
        bool enableShadow::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.enableShadow.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String enableShadow::Get(StyleClass &sc) const
        {
            return sc.enableShadow.GetValueAsString();
        }
        void enableShadow::OnChanged(Element &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-color
        bool shadowColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowColour::Get(StyleClass &sc) const
        {
            return sc.shadowColour.GetValueAsString();
        }
        void shadowColour::OnChanged(Element &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-blur-radius
        bool shadowBlurRadius::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowBlurRadius.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowBlurRadius::Get(StyleClass &sc) const
        {
            return sc.shadowBlurRadius.GetValueAsString();
        }
        void shadowBlurRadius::OnChanged(Element &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-offset-x
        bool shadowOffsetX::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowOffsetX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowOffsetX::Get(StyleClass &sc) const
        {
            return sc.shadowOffsetX.GetValueAsString();
        }
        void shadowOffsetX::OnChanged(Element &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-offset-y
        bool shadowOffsetY::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowOffsetY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowOffsetY::Get(StyleClass &sc) const
        {
            return sc.shadowOffsetY.GetValueAsString();
        }
        void shadowOffsetY::OnChanged(Element &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-extrusion-x
        bool shadowExtrusionX::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowExtrusionX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowExtrusionX::Get(StyleClass &sc) const
        {
            return sc.shadowExtrusionX.GetValueAsString();
        }
        void shadowExtrusionX::OnChanged(Element &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-extrusion-y
        bool shadowExtrusionY::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowExtrusionY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowExtrusionY::Get(StyleClass &sc) const
        {
            return sc.shadowExtrusionY.GetValueAsString();
        }
        void shadowExtrusionY::OnChanged(Element &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-opacity
        bool shadowOpacity::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowOpacity.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowOpacity::Get(StyleClass &sc) const
        {
            return sc.shadowOpacity.GetValueAsString();
        }
        void shadowOpacity::OnChanged(Element &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // allow-mouse-drag
        bool allowMouseDrag::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.allowMouseDrag.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String allowMouseDrag::Get(StyleClass &sc) const
        {
            return sc.allowMouseDrag.GetValueAsString();
        }
        void allowMouseDrag::OnChanged(Element &)
        {
        }


        // constrain-mouse-drag-x
        bool constrainMouseDragX::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.constrainMouseDragX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String constrainMouseDragX::Get(StyleClass &sc) const
        {
            return sc.constrainMouseDragX.GetValueAsString();
        }
        void constrainMouseDragX::OnChanged(Element &)
        {
        }

        // constrain-mouse-drag-y
        bool constrainMouseDragY::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.constrainMouseDragY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String constrainMouseDragY::Get(StyleClass &sc) const
        {
            return sc.constrainMouseDragY.GetValueAsString();
        }
        void constrainMouseDragY::OnChanged(Element &)
        {
        }


        // mouse-drag-clamp-mode-x
        bool mouseDragClampModeX::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.mouseDragClampModeX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String mouseDragClampModeX::Get(StyleClass &sc) const
        {
            return sc.mouseDragClampModeX.GetValueAsString();
        }
        void mouseDragClampModeX::OnChanged(Element &)
        {
        }

        // mouse-drag-clamp-mode-y
        bool mouseDragClampModeY::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.mouseDragClampModeY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String mouseDragClampModeY::Get(StyleClass &sc) const
        {
            return sc.mouseDragClampModeY.GetValueAsString();
        }
        void mouseDragClampModeY::OnChanged(Element &)
        {
        }


        // allow-mouse-resize
        bool allowMouseResize::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.allowMouseResize.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String allowMouseResize::Get(StyleClass &sc) const
        {
            return sc.allowMouseResize.GetValueAsString();
        }
        void allowMouseResize::OnChanged(Element &)
        {
        }

        // left-gripper-width
        bool leftGripperWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.leftGripperWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String leftGripperWidth::Get(StyleClass &sc) const
        {
            return sc.leftGripperWidth.GetValueAsString();
        }
        void leftGripperWidth::OnChanged(Element &)
        {
        }

        // right-gripper-width
        bool rightGripperWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.rightGripperWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String rightGripperWidth::Get(StyleClass &sc) const
        {
            return sc.rightGripperWidth.GetValueAsString();
        }
        void rightGripperWidth::OnChanged(Element &)
        {
        }

        // top-gripper-width
        bool topGripperWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.topGripperWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String topGripperWidth::Get(StyleClass &sc) const
        {
            return sc.topGripperWidth.GetValueAsString();
        }
        void topGripperWidth::OnChanged(Element &)
        {
        }

        // bottom-gripper-width
        bool bottomGripperWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.bottomGripperWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String bottomGripperWidth::Get(StyleClass &sc) const
        {
            return sc.bottomGripperWidth.GetValueAsString();
        }
        void bottomGripperWidth::OnChanged(Element &)
        {
        }
    }


    ///////////////////////////////////////////////////////////////////////////
    // Shorthand style attributes.
    //
    // Every one of these needs to implement the Unset() method manually.
    namespace AttributeHandlers
    {
        // background-align
        bool backgroundAlign::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            // We must have two tokens here.
            const char* xStart = nullptr;
            const char* xEnd   = nullptr;
            const char* yStart = nullptr;
            const char* yEnd   = nullptr;

            GT::Strings::Tokenizer token(value, valueSize);

            // First token (X)
            if (token)
            {
                xStart = token.start;
                xEnd   = token.end;
            }

            // Second token (Y)
            if (++token)
            {
                yStart = token.start;
                yEnd   = token.end;
            }

            if (xStart != nullptr && yStart != nullptr)
            {
                if (sc.backgroundAlignX.SetFromString(xStart, xEnd - xStart) &&
                    sc.backgroundAlignY.SetFromString(yStart, yEnd - yStart))
                {
                    backgroundAlignX::Refresh(sc);
                    backgroundAlignY::Refresh(sc);

                    return true;
                }
            }

            return false;
        }
        void backgroundAlign::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "background-align-x");
            sc.server.UnsetStyleAttribute(sc, "background-align-y");
        }
        GT::String backgroundAlign::Get(StyleClass &sc) const
        {
            return sc.backgroundAlignX.GetValueAsString() + " " + sc.backgroundAlignY.GetValueAsString();
        }
        void backgroundAlign::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.backgroundAlignX.inherit = inherit;
            sc.backgroundAlignX.isset   = inherit;
            sc.backgroundAlignY.inherit = inherit;
            sc.backgroundAlignY.isset   = inherit;

            if (refresh)
            {
                backgroundAlignX::Refresh(sc);
                backgroundAlignY::Refresh(sc);
            }
        }


        // background-repeat
        bool backgroundRepeat::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            // If only a single token is present, we set the alignment for both X and Y. Otherwise, the first token is the X
            // alignment and the second is the Y alignment.
            GT::Strings::Tokenizer token(value, valueSize);
            if (token)
            {
                const char* xStart = token.start;
                const char* xEnd   = token.end;
                const char* yStart = xStart;
                const char* yEnd   = xEnd;

                if (++token)
                {
                    yStart = token.start;
                    yEnd   = token.end;
                }

                if (sc.backgroundRepeatX.SetFromString(xStart, xEnd - xStart) &&
                    sc.backgroundRepeatY.SetFromString(yStart, yEnd - yStart))
                {
                    backgroundRepeatX::Refresh(sc);
                    backgroundRepeatY::Refresh(sc);

                    return true;
                }
            }

            return false;
        }
        void backgroundRepeat::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "background-repeat-x");
            sc.server.UnsetStyleAttribute(sc, "background-repeat-y");
        }
        GT::String backgroundRepeat::Get(StyleClass &sc) const
        {
            return sc.backgroundRepeatX.GetValueAsString() + " " + sc.backgroundRepeatY.GetValueAsString();
        }
        void backgroundRepeat::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.backgroundRepeatX.inherit = inherit;
            sc.backgroundRepeatX.isset   = inherit;
            sc.backgroundRepeatY.inherit = inherit;
            sc.backgroundRepeatY.isset   = inherit;

            if (refresh)
            {
                backgroundRepeatX::Refresh(sc);
                backgroundRepeatY::Refresh(sc);
            }
        }


        // background
        bool background::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            // At the moment background is the same as background-color. But in the future we will add things like images.
            if (sc.backgroundColour.SetFromString(value, valueSize))
            {
                backgroundColour::Refresh(sc);
                return true;
            }

            return false;
        }
        void background::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "background-color");
        }
        GT::String background::Get(StyleClass &sc) const
        {
            return sc.backgroundColour.GetValueAsString();
        }
        void background::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.backgroundColour.inherit = inherit;
            sc.backgroundColour.isset   = inherit;

            if (refresh)
            {
                backgroundColour::Refresh(sc);
            }
        }


        // border-width
        bool borderWidth::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            // We need to tokenize the value and look at the tokens. It has a different meaning depending on how many
            // arguments it has. If it has only 1 token, it means all borders share that size. If there are two values,
            // it means the first value refers to the vertical borders (left/right) and the second refers to the horizontal
            // borders (top/bottom). If more than two are specified, it means there are individual values for each side.
            // the first two values are the left and right borders, respectively, and the last two are the top and bottom.
            const char* tokensStart[4] = {nullptr, nullptr, nullptr, nullptr};
            const char* tokensEnd[4]   = {nullptr, nullptr, nullptr, nullptr};
        
            int i = 0;
            GT::Strings::Tokenizer token(value, valueSize);
            while (i < 4 && token)
            {
                tokensStart[i] = token.start;
                tokensEnd[i]   = token.end;
            
                ++i;
                ++token;
            }
        
            // If the fourth token is set, we settings each width individually.
            if (tokensStart[3] != nullptr)
            {
                sc.borderLeftWidth.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                sc.borderRightWidth.SetFromString( tokensStart[1], tokensEnd[1] - tokensStart[1]);
                sc.borderTopWidth.SetFromString(   tokensStart[2], tokensEnd[2] - tokensStart[2]);
                sc.borderBottomWidth.SetFromString(tokensStart[3], tokensEnd[3] - tokensStart[3]);
            }
            else
            {
                // If the second token is set, we're doing the vertical and horizontal sides individually.
                if (tokensStart[1] != nullptr)
                {
                    sc.borderLeftWidth.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    sc.borderRightWidth.SetFromString( tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    sc.borderTopWidth.SetFromString(   tokensStart[1], tokensEnd[1] - tokensStart[1]);
                    sc.borderBottomWidth.SetFromString(tokensStart[1], tokensEnd[1] - tokensStart[1]);
                }
                else
                {
                    // If the first token is set, we're setting all borders to the same value.
                    if (tokensStart[0] != nullptr)
                    {
                        sc.borderLeftWidth.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.borderRightWidth.SetFromString( tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.borderTopWidth.SetFromString(   tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.borderBottomWidth.SetFromString(tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    }
                    else
                    {
                        return false;
                    }
                }
            }

            borderLeftWidth::Refresh(sc);
            borderRightWidth::Refresh(sc);
            borderTopWidth::Refresh(sc);
            borderBottomWidth::Refresh(sc);

            return true;
        }
        void borderWidth::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "border-left-width");
            sc.server.UnsetStyleAttribute(sc, "border-right-width");
            sc.server.UnsetStyleAttribute(sc, "border-top-width");
            sc.server.UnsetStyleAttribute(sc, "border-bottom-width");
        }
        GT::String borderWidth::Get(StyleClass &sc) const
        {
            return sc.borderLeftWidth.GetValueAsString() + " " + sc.borderRightWidth.GetValueAsString() + " " + sc.borderTopWidth.GetValueAsString() + " " + sc.borderBottomWidth.GetValueAsString();
        }
        void borderWidth::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.borderLeftWidth.inherit   = inherit;
            sc.borderLeftWidth.isset     = inherit;
            sc.borderRightWidth.inherit  = inherit;
            sc.borderRightWidth.isset    = inherit;
            sc.borderTopWidth.inherit    = inherit;
            sc.borderTopWidth.isset      = inherit;
            sc.borderBottomWidth.inherit = inherit;
            sc.borderBottomWidth.isset   = inherit;

            if (refresh)
            {
                borderLeftWidth::Refresh(sc);
                borderRightWidth::Refresh(sc);
                borderTopWidth::Refresh(sc);
                borderBottomWidth::Refresh(sc);
            }
        }


        // border-color
        bool borderColour::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            // border-color works in the same way as border-width.
            const char* tokensStart[4] = {nullptr, nullptr, nullptr, nullptr};
            const char* tokensEnd[4]   = {nullptr, nullptr, nullptr, nullptr};
        
            int i = 0;
            GT::Strings::Tokenizer token(value, valueSize);
            while (i < 4 && token)
            {
                tokensStart[i] = token.start;
                tokensEnd[i]   = token.end;
            
                ++i;
                ++token;
            }
        
            // If the fourth token is set, we settings each width individually.
            if (tokensStart[3] != nullptr)
            {
                sc.borderLeftColour.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                sc.borderRightColour.SetFromString( tokensStart[1], tokensEnd[1] - tokensStart[1]);
                sc.borderTopColour.SetFromString(   tokensStart[2], tokensEnd[2] - tokensStart[2]);
                sc.borderBottomColour.SetFromString(tokensStart[3], tokensEnd[3] - tokensStart[3]);
            }
            else
            {
                // If the second token is set, we're doing the vertical and horizontal sides individually.
                if (tokensStart[1] != nullptr)
                {
                    sc.borderLeftColour.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    sc.borderRightColour.SetFromString( tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    sc.borderTopColour.SetFromString(   tokensStart[1], tokensEnd[1] - tokensStart[1]);
                    sc.borderBottomColour.SetFromString(tokensStart[1], tokensEnd[1] - tokensStart[1]);
                }
                else
                {
                    // If the first token is set, we're setting all borders to the same value.
                    if (tokensStart[0] != nullptr)
                    {
                        sc.borderLeftColour.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.borderRightColour.SetFromString( tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.borderTopColour.SetFromString(   tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.borderBottomColour.SetFromString(tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    }
                    else
                    {
                        return false;
                    }
                }
            }

            borderLeftColour::Refresh(sc);
            borderRightColour::Refresh(sc);
            borderTopColour::Refresh(sc);
            borderBottomColour::Refresh(sc);

            return true;
        }
        void borderColour::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "border-left-color");
            sc.server.UnsetStyleAttribute(sc, "border-right-color");
            sc.server.UnsetStyleAttribute(sc, "border-top-color");
            sc.server.UnsetStyleAttribute(sc, "border-bottom-color");
        }
        GT::String borderColour::Get(StyleClass &sc) const
        {
            return sc.borderLeftColour.GetValueAsString() + " " + sc.borderRightColour.GetValueAsString() + " " + sc.borderTopColour.GetValueAsString() + " " + sc.borderBottomColour.GetValueAsString();
        }
        void borderColour::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.borderLeftColour.inherit   = inherit;
            sc.borderLeftColour.isset     = inherit;
            sc.borderRightColour.inherit  = inherit;
            sc.borderRightColour.isset    = inherit;
            sc.borderTopColour.inherit    = inherit;
            sc.borderTopColour.isset      = inherit;
            sc.borderBottomColour.inherit = inherit;
            sc.borderBottomColour.isset   = inherit;

            if (refresh)
            {
                borderLeftColour::Refresh(sc);
                borderRightColour::Refresh(sc);
                borderTopColour::Refresh(sc);
                borderBottomColour::Refresh(sc);
            }
        }


        // border-left
        bool borderLeft::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (!GT::Strings::Equal(value, valueSize, "none"))
            {
                // The border attribute is simple. It will take two parameters - the width and the colour. The values are
                // applied to all borders.
                GT::Colour colour;
        
                int i = 0;
                GT::Strings::Tokenizer token(value, valueSize);
                while (i < 2 && token)
                {
                    if (GT::Colour::TryParse(colour, token.start, token.end - token.start))
                    {
                        sc.borderLeftColour = colour;
                    }
                    else
                    {
                        // Assume it was a width.
                        sc.borderLeftWidth.SetFromString(token.start, token.end - token.start);
                    }

                    ++i;
                    ++token;
                }
        
                if (i != 0)
                {
                    borderLeftWidth::Refresh(sc);
                    borderLeftColour::Refresh(sc);

                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                sc.borderLeftColour = GT::Colour::TransparentBlack;
                sc.borderLeftWidth  = 0;

                borderLeftWidth::Refresh(sc);
                borderLeftColour::Refresh(sc);

                return true;
            }
        }
        void borderLeft::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "border-left-width");
            sc.server.UnsetStyleAttribute(sc, "border-left-color");
        }
        GT::String borderLeft::Get(StyleClass &sc) const
        {
            return sc.borderLeftWidth.GetValueAsString() + " " + sc.borderLeftColour.GetValueAsString();
        }
        void borderLeft::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.borderLeftWidth.inherit  = inherit;
            sc.borderLeftWidth.isset    = inherit;
            sc.borderLeftColour.inherit = inherit;
            sc.borderLeftColour.isset   = inherit;
            
            if (refresh)
            {
                borderLeftWidth::Refresh(sc);
                borderLeftColour::Refresh(sc);
            }
        }

    
        // border-right
        bool borderRight::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (!GT::Strings::Equal(value, valueSize, "none"))
            {
                // The border attribute is simple. It will take two parameters - the width and the colour. The values are
                // applied to all borders.
                GT::Colour colour;
        
                int i = 0;
                GT::Strings::Tokenizer token(value, valueSize);
                while (i < 2 && token)
                {
                    if (GT::Colour::TryParse(colour, token.start, token.end - token.start))
                    {
                        sc.borderRightColour = colour;
                    }
                    else
                    {
                        // Assume it was a width.
                        sc.borderRightWidth.SetFromString(token.start, token.end - token.start);
                    }

                    ++i;
                    ++token;
                }
        
                // If we didn't even have a token, return false.
                if (i != 0)
                {
                    borderRightWidth::Refresh(sc);
                    borderRightColour::Refresh(sc);

                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                sc.borderRightColour = GT::Colour::TransparentBlack;
                sc.borderRightWidth  = 0;

                borderRightWidth::Refresh(sc);
                borderRightColour::Refresh(sc);
            }

            return false;
        }
        void borderRight::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "border-right-width");
            sc.server.UnsetStyleAttribute(sc, "border-right-color");
        }
        GT::String borderRight::Get(StyleClass &sc) const
        {
            return sc.borderRightWidth.GetValueAsString() + " " + sc.borderRightColour.GetValueAsString();
        }
        void borderRight::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.borderRightWidth.inherit  = inherit;
            sc.borderRightWidth.isset    = inherit;
            sc.borderRightColour.inherit = inherit;
            sc.borderRightColour.isset   = inherit;
            
            if (refresh)
            {
                borderRightWidth::Refresh(sc);
                borderRightColour::Refresh(sc);
            }
        }

    
        // border-top
        bool borderTop::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (!GT::Strings::Equal(value, valueSize, "none"))
            {
                // The border attribute is simple. It will take two parameters - the width and the colour. The values are
                // applied to all borders.
                GT::Colour colour;
        
                int i = 0;
                GT::Strings::Tokenizer token(value, valueSize);
                while (i < 2 && token)
                {
                    if (GT::Colour::TryParse(colour, token.start, token.end - token.start))
                    {
                        sc.borderTopColour = colour;
                    }
                    else
                    {
                        // Assume it was a width.
                        sc.borderTopWidth.SetFromString(token.start, token.end - token.start);
                    }

                    ++i;
                    ++token;
                }
        
                // If we didn't even have a token, return false.
                if (i != 0)
                {
                    borderTopWidth::Refresh(sc);
                    borderTopColour::Refresh(sc);

                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                sc.borderTopColour = GT::Colour::TransparentBlack;
                sc.borderTopWidth  = 0;

                borderTopWidth::Refresh(sc);
                borderTopColour::Refresh(sc);
            }

            return false;
        }
        void borderTop::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "border-top-width");
            sc.server.UnsetStyleAttribute(sc, "border-top-color");
        }
        GT::String borderTop::Get(StyleClass &sc) const
        {
            return sc.borderTopWidth.GetValueAsString() + " " + sc.borderTopColour.GetValueAsString();
        }
        void borderTop::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.borderTopWidth.inherit  = inherit;
            sc.borderTopWidth.isset    = inherit;
            sc.borderTopColour.inherit = inherit;
            sc.borderTopColour.isset   = inherit;
            
            if (refresh)
            {
                borderTopWidth::Refresh(sc);
                borderTopColour::Refresh(sc);
            }
        }


        // border-bottom
        bool borderBottom::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (!GT::Strings::Equal(value, valueSize, "none"))
            {
                // The border attribute is simple. It will take two parameters - the width and the colour. The values are
                // applied to all borders.
                GT::Colour colour;
        
                int i = 0;
                GT::Strings::Tokenizer token(value, valueSize);
                while (i < 2 && token)
                {
                    if (GT::Colour::TryParse(colour, token.start, token.end - token.start))
                    {
                        sc.borderBottomColour = colour;
                    }
                    else
                    {
                        // Assume it was a width.
                        sc.borderBottomWidth.SetFromString(token.start, token.end - token.start);
                    }

                    ++i;
                    ++token;
                }
        
                // If we didn't even have a token, return false.
                if (i != 0)
                {
                    borderBottomWidth::Refresh(sc);
                    borderBottomColour::Refresh(sc);

                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                sc.borderBottomColour = GT::Colour::TransparentBlack;
                sc.borderBottomWidth  = 0;

                borderBottomWidth::Refresh(sc);
                borderBottomColour::Refresh(sc);
            }

            return false;
        }
        void borderBottom::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "border-bottom-width");
            sc.server.UnsetStyleAttribute(sc, "border-bottom-color");
        }
        GT::String borderBottom::Get(StyleClass &sc) const
        {
            return sc.borderBottomWidth.GetValueAsString() + " " + sc.borderBottomColour.GetValueAsString();
        }
        void borderBottom::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.borderBottomWidth.inherit  = inherit;
            sc.borderBottomWidth.isset    = inherit;
            sc.borderBottomColour.inherit = inherit;
            sc.borderBottomColour.isset   = inherit;
            
            if (refresh)
            {
                borderBottomWidth::Refresh(sc);
                borderBottomColour::Refresh(sc);
            }
        }

    
        // border
        bool border::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            bool success = false;

            if (GT::Strings::Equal(value, valueSize, "none"))
            {
                sc.borderLeftColour   = GT::Colour::TransparentBlack;
                sc.borderRightColour  = GT::Colour::TransparentBlack;
                sc.borderTopColour    = GT::Colour::TransparentBlack;
                sc.borderBottomColour = GT::Colour::TransparentBlack;
            
                sc.borderLeftWidth   = 0;
                sc.borderRightWidth  = 0;
                sc.borderTopWidth    = 0;
                sc.borderBottomWidth = 0;
            
                success = true;
            }
            else
            {
                // The border attribute is simple. It will take two parameters - the width and the colour. The values are
                // applied to all borders.
                GT::Colour colour;
            
                int i = 0;
                GT::Strings::Tokenizer token(value, valueSize);
                while (i < 2 && token)
                {
                    if (GT::Colour::TryParse(colour, token.start, token.end - token.start))
                    {
                        sc.borderLeftColour   = colour;
                        sc.borderRightColour  = colour;
                        sc.borderTopColour    = colour;
                        sc.borderBottomColour = colour;
                    }
                    else
                    {
                        // Assume it was a width.
                        sc.borderLeftWidth.SetFromString(  token.start, token.end - token.start);
                        sc.borderRightWidth.SetFromString( token.start, token.end - token.start);
                        sc.borderTopWidth.SetFromString(   token.start, token.end - token.start);
                        sc.borderBottomWidth.SetFromString(token.start, token.end - token.start);
                    }

                    ++i;
                    ++token;
                }
            
                // If we didn't have a token, return false.
                success = i != 0;
            }

            //if (success)
            {
                borderLeftWidth::Refresh(sc);
                borderLeftColour::Refresh(sc);
                borderRightWidth::Refresh(sc);
                borderRightColour::Refresh(sc);
                borderTopWidth::Refresh(sc);
                borderTopColour::Refresh(sc);
                borderBottomWidth::Refresh(sc);
                borderBottomColour::Refresh(sc);
            }

            return success;
        }
        GT::String border::Get(StyleClass &sc) const
        {
            return sc.borderLeftWidth.GetValueAsString() + " " + sc.borderLeftColour.GetValueAsString();
        }
        void border::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "border-left-width");
            sc.server.UnsetStyleAttribute(sc, "border-right-width");
            sc.server.UnsetStyleAttribute(sc, "border-top-width");
            sc.server.UnsetStyleAttribute(sc, "border-bottom-width");
            sc.server.UnsetStyleAttribute(sc, "border-left-colour");
            sc.server.UnsetStyleAttribute(sc, "border-right-colour");
            sc.server.UnsetStyleAttribute(sc, "border-top-colour");
            sc.server.UnsetStyleAttribute(sc, "border-bottom-colour");
        }
        void border::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.borderLeftWidth.inherit    = inherit;
            sc.borderLeftWidth.isset      = inherit;
            sc.borderLeftColour.inherit   = inherit;
            sc.borderLeftColour.isset     = inherit;

            sc.borderRightWidth.inherit   = inherit;
            sc.borderRightWidth.isset     = inherit;
            sc.borderRightColour.inherit  = inherit;
            sc.borderRightColour.isset    = inherit;

            sc.borderTopWidth.inherit     = inherit;
            sc.borderTopWidth.isset       = inherit;
            sc.borderTopColour.inherit    = inherit;
            sc.borderTopColour.isset      = inherit;

            sc.borderBottomWidth.inherit  = inherit;
            sc.borderBottomWidth.isset    = inherit;
            sc.borderBottomColour.inherit = inherit;
            sc.borderBottomColour.isset   = inherit;
            
            if (refresh)
            {
                borderLeftWidth::Refresh(sc);
                borderLeftColour::Refresh(sc);
                borderRightWidth::Refresh(sc);
                borderRightColour::Refresh(sc);
                borderTopWidth::Refresh(sc);
                borderTopColour::Refresh(sc);
                borderBottomWidth::Refresh(sc);
                borderBottomColour::Refresh(sc);
            }
        }
    
    
        // padding
        bool padding::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            // padding is the same as border-width.
            const char* tokensStart[4] = {nullptr, nullptr, nullptr, nullptr};
            const char* tokensEnd[4]   = {nullptr, nullptr, nullptr, nullptr};
        
            int i = 0;
            GT::Strings::Tokenizer token(value, valueSize);
            while (i < 4 && token)
            {
                tokensStart[i] = token.start;
                tokensEnd[i]   = token.end;
            
                ++i;
                ++token;
            }
        
            // If the fourth token is set, we settings each width individually.
            if (tokensStart[3] != nullptr)
            {
                sc.paddingLeft.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                sc.paddingRight.SetFromString( tokensStart[1], tokensEnd[1] - tokensStart[1]);
                sc.paddingTop.SetFromString(   tokensStart[2], tokensEnd[2] - tokensStart[2]);
                sc.paddingBottom.SetFromString(tokensStart[3], tokensEnd[3] - tokensStart[3]);
            }
            else
            {
                // If the second token is set, we're doing the vertical and horizontal sides individually.
                if (tokensStart[1] != nullptr)
                {
                    sc.paddingLeft.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    sc.paddingRight.SetFromString( tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    sc.paddingTop.SetFromString(   tokensStart[1], tokensEnd[1] - tokensStart[1]);
                    sc.paddingBottom.SetFromString(tokensStart[1], tokensEnd[1] - tokensStart[1]);
                }
                else
                {
                    // If the first token is set, we're setting all borders to the same value.
                    if (tokensStart[0] != nullptr)
                    {
                        sc.paddingLeft.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.paddingRight.SetFromString( tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.paddingTop.SetFromString(   tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.paddingBottom.SetFromString(tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    }
                    else
                    {
                        return false;
                    }
                }
            }

            paddingLeft::Refresh(sc);
            paddingRight::Refresh(sc);
            paddingTop::Refresh(sc);
            paddingBottom::Refresh(sc);

            return true;
        }
        void padding::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "padding-left");
            sc.server.UnsetStyleAttribute(sc, "padding-right");
            sc.server.UnsetStyleAttribute(sc, "padding-top");
            sc.server.UnsetStyleAttribute(sc, "padding-bottom");
        }
        GT::String padding::Get(StyleClass &sc) const
        {
            return sc.paddingLeft.GetValueAsString() + " " + sc.paddingRight.GetValueAsString() + " " + sc.paddingTop.GetValueAsString() + " " + sc.paddingBottom.GetValueAsString();
        }
        void padding::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.paddingLeft.inherit   = inherit;
            sc.paddingLeft.isset     = inherit;
            sc.paddingRight.inherit  = inherit;
            sc.paddingRight.isset    = inherit;
            sc.paddingTop.inherit    = inherit;
            sc.paddingTop.isset      = inherit;
            sc.paddingBottom.inherit = inherit;
            sc.paddingBottom.isset   = inherit;
            
            if (refresh)
            {
                paddingLeft::Refresh(sc);
                paddingRight::Refresh(sc);
                paddingTop::Refresh(sc);
                paddingBottom::Refresh(sc);
            }
        }
    

        // margin
        bool margin::Set(StyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            // margin is the same as border-width.
            const char* tokensStart[4] = {nullptr, nullptr, nullptr, nullptr};
            const char* tokensEnd[4]   = {nullptr, nullptr, nullptr, nullptr};
        
            int i = 0;
            GT::Strings::Tokenizer token(value, valueSize);
            while (i < 4 && token)
            {
                tokensStart[i] = token.start;
                tokensEnd[i]   = token.end;
            
                ++i;
                ++token;
            }
        
            // If the fourth token is set, we settings each width individually.
            if (tokensStart[3] != nullptr)
            {
                sc.marginLeft.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                sc.marginRight.SetFromString( tokensStart[1], tokensEnd[1] - tokensStart[1]);
                sc.marginTop.SetFromString(   tokensStart[2], tokensEnd[2] - tokensStart[2]);
                sc.marginBottom.SetFromString(tokensStart[3], tokensEnd[3] - tokensStart[3]);
            }
            else
            {
                // If the second token is set, we're doing the vertical and horizontal sides individually.
                if (tokensStart[1] != nullptr)
                {
                    sc.marginLeft.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    sc.marginRight.SetFromString( tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    sc.marginTop.SetFromString(   tokensStart[1], tokensEnd[1] - tokensStart[1]);
                    sc.marginBottom.SetFromString(tokensStart[1], tokensEnd[1] - tokensStart[1]);
                }
                else
                {
                    // If the first token is set, we're setting all borders to the same value.
                    if (tokensStart[0] != nullptr)
                    {
                        sc.marginLeft.SetFromString(  tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.marginRight.SetFromString( tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.marginTop.SetFromString(   tokensStart[0], tokensEnd[0] - tokensStart[0]);
                        sc.marginBottom.SetFromString(tokensStart[0], tokensEnd[0] - tokensStart[0]);
                    }
                    else
                    {
                        return false;
                    }
                }
            }

            marginLeft::Refresh(sc);
            marginRight::Refresh(sc);
            marginTop::Refresh(sc);
            marginBottom::Refresh(sc);

            return true;
        }
        void margin::Unset(StyleClass &sc)
        {
            sc.server.UnsetStyleAttribute(sc, "margin-left");
            sc.server.UnsetStyleAttribute(sc, "margin-right");
            sc.server.UnsetStyleAttribute(sc, "margin-top");
            sc.server.UnsetStyleAttribute(sc, "margin-bottom");
        }
        GT::String margin::Get(StyleClass &sc) const
        {
            return sc.marginLeft.GetValueAsString() + " " + sc.marginRight.GetValueAsString() + " " + sc.marginTop.GetValueAsString() + " " + sc.marginBottom.GetValueAsString();
        }
        void margin::SetInherit(StyleClass &sc, bool inherit, bool refresh)
        {
            sc.marginLeft.inherit   = inherit;
            sc.marginLeft.isset     = inherit;
            sc.marginRight.inherit  = inherit;
            sc.marginRight.isset    = inherit;
            sc.marginTop.inherit    = inherit;
            sc.marginTop.isset      = inherit;
            sc.marginBottom.inherit = inherit;
            sc.marginBottom.isset   = inherit;
            
            if (refresh)
            {
                marginLeft::Refresh(sc);
                marginRight::Refresh(sc);
                marginTop::Refresh(sc);
                marginBottom::Refresh(sc);
            }
        }
    }
}



namespace GTGUI
{
    StyleAttributeHandler::StyleAttributeHandler()
    {
    }
    
    StyleAttributeHandler::~StyleAttributeHandler()
    {
    }
}
