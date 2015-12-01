// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include "GUIStyleAttributeHandler.hpp"
#include <GTEngine/GUI/GUIStyleStack.hpp>
#include <GTEngine/GUI/GUIElement.hpp>
#include <GTEngine/GUI/GUIServer.hpp>
#include <GTEngine/Core/Strings/Tokenizer.hpp>

/* !!! Don't forget to add new attribute handlers to GUIStyleServer::LoadGUIStyleAttributeHandlers(). It's at the end of GUIStyleServer.cpp. !!! */

namespace GT
{
    // Primitive style attributes. Shorthand attributes are below these ones.
    namespace AttributeHandlers
    {
        // width
        bool width::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String width::Get(GUIStyleClass &sc) const
        {
            return sc.width.GetValueAsString();
        }
        void width::OnChanged(GUIElement &element)
        {
            element.server.OnWidthChanged(element);
        }

        // height
        bool height::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String height::Get(GUIStyleClass &sc) const
        {
            return sc.height.GetValueAsString();
        }
        void height::OnChanged(GUIElement &element)
        {
            element.server.OnHeightChanged(element);
        }


        // min-width
        bool minWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String minWidth::Get(GUIStyleClass &sc) const
        {
            return sc.minWidth.GetValueAsString();
        }
        void minWidth::OnChanged(GUIElement &element)
        {
            element.server.OnWidthChanged(element);
        }

        // max-width
        bool maxWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String maxWidth::Get(GUIStyleClass &sc) const
        {
            return sc.maxWidth.GetValueAsString();
        }
        void maxWidth::OnChanged(GUIElement &element)
        {
            element.server.OnWidthChanged(element);
        }


        // min-height
        bool minHeight::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String minHeight::Get(GUIStyleClass &sc) const
        {
            return sc.minHeight.GetValueAsString();
        }
        void minHeight::OnChanged(GUIElement &element)
        {
            element.server.OnHeightChanged(element);
        }

        // max-height
        bool maxHeight::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String maxHeight::Get(GUIStyleClass &sc) const
        {
            return sc.maxHeight.GetValueAsString();
        }
        void maxHeight::OnChanged(GUIElement &element)
        {
            element.server.OnHeightChanged(element);
        }


        // relative-width-mode
        bool relativeWidthMode::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.relativeWidthMode.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String relativeWidthMode::Get(GUIStyleClass &sc) const
        {
            return sc.relativeWidthMode.GetValueAsString();
        }
        void relativeWidthMode::OnChanged(GUIElement &)
        {
        }


        // relative-height-mode
        bool relativeHeightMode::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.relativeHeightMode.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String relativeHeightMode::Get(GUIStyleClass &sc) const
        {
            return sc.relativeHeightMode.GetValueAsString();
        }
        void relativeHeightMode::OnChanged(GUIElement &)
        {
        }


        // flex-child-width
        bool flexChildWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.flexChildWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String flexChildWidth::Get(GUIStyleClass &sc) const
        {
            return sc.flexChildWidth.GetValueAsString();
        }
        void flexChildWidth::OnChanged(GUIElement &element)
        {
            element.server.OnFlexWidthChanged(element);
        }


        // flex-child-height
        bool flexChildHeight::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.flexChildHeight.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String flexChildHeight::Get(GUIStyleClass &sc) const
        {
            return sc.flexChildHeight.GetValueAsString();
        }
        void flexChildHeight::OnChanged(GUIElement &element)
        {
            element.server.OnFlexHeightChanged(element);
        }

    
        // background-color
        bool backgroundColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundColour::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundColour.GetValueAsString();
        }
        void backgroundColour::OnChanged(GUIElement &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-image
        bool backgroundImage::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundImage.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundImage::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundImage.GetValueAsString();
        }
        void backgroundImage::OnChanged(GUIElement &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-image-color
        bool backgroundImageColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundImageColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundImageColour::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundImageColour.GetValueAsString();
        }
        void backgroundImageColour::OnChanged(GUIElement &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-align-x
        bool backgroundAlignX::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundAlignX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundAlignX::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundAlignX.GetValueAsString();
        }
        void backgroundAlignX::OnChanged(GUIElement &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-align-x
        bool backgroundAlignY::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundAlignY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundAlignY::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundAlignY.GetValueAsString();
        }
        void backgroundAlignY::OnChanged(GUIElement &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-repeat-x
        bool backgroundRepeatX::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundRepeatX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundRepeatX::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundRepeatX.GetValueAsString();
        }
        void backgroundRepeatX::OnChanged(GUIElement &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // background-repeat-y
        bool backgroundRepeatY::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.backgroundRepeatY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String backgroundRepeatY::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundRepeatY.GetValueAsString();
        }
        void backgroundRepeatY::OnChanged(GUIElement &element)
        {
            element.InvalidateBackgroundRenderingData();
        }


        // border-left-width
        bool borderLeftWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderLeftWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderLeftWidth::Get(GUIStyleClass &sc) const
        {
            return sc.borderLeftWidth.GetValueAsString();
        }
        void borderLeftWidth::OnChanged(GUIElement &element)
        {
            element.server.OnPaddingXChanged(element);
            element.InvalidateBorderRenderingData();
        }

    
        // border-right-width
        bool borderRightWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderRightWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderRightWidth::Get(GUIStyleClass &sc) const
        {
            return sc.borderRightWidth.GetValueAsString();
        }
        void borderRightWidth::OnChanged(GUIElement &element)
        {
            element.server.OnPaddingXChanged(element);
            element.InvalidateBorderRenderingData();
        }

    
        // border-top-width
        bool borderTopWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderTopWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderTopWidth::Get(GUIStyleClass &sc) const
        {
            return sc.borderTopWidth.GetValueAsString();
        }
        void borderTopWidth::OnChanged(GUIElement &element)
        {
            element.server.OnPaddingYChanged(element);
            element.InvalidateBorderRenderingData();
        }

    
        // border-bottom-width
        bool borderBottomWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderBottomWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderBottomWidth::Get(GUIStyleClass &sc) const
        {
            return sc.borderBottomWidth.GetValueAsString();
        }
        void borderBottomWidth::OnChanged(GUIElement &element)
        {
            element.server.OnPaddingYChanged(element);
            element.InvalidateBorderRenderingData();
        }


        // border-left-color
        bool borderLeftColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderLeftColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderLeftColour::Get(GUIStyleClass &sc) const
        {
            return sc.borderLeftColour.GetValueAsString();
        }
        void borderLeftColour::OnChanged(GUIElement &element)
        {
            element.InvalidateBorderRenderingData();
        }

    
        // border-right-color
        bool borderRightColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderRightColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderRightColour::Get(GUIStyleClass &sc) const
        {
            return sc.borderRightColour.GetValueAsString();
        }
        void borderRightColour::OnChanged(GUIElement &element)
        {
            element.InvalidateBorderRenderingData();
        }

    
        // border-top-color
        bool borderTopColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderTopColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderTopColour::Get(GUIStyleClass &sc) const
        {
            return sc.borderTopColour.GetValueAsString();
        }
        void borderTopColour::OnChanged(GUIElement &element)
        {
            element.InvalidateBorderRenderingData();
        }

    
        // border-bottom-color
        bool borderBottomColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.borderBottomColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String borderBottomColour::Get(GUIStyleClass &sc) const
        {
            return sc.borderBottomColour.GetValueAsString();
        }
        void borderBottomColour::OnChanged(GUIElement &element)
        {
            element.InvalidateBorderRenderingData();
        }


        // padding-left
        bool paddingLeft::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.paddingLeft.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String paddingLeft::Get(GUIStyleClass &sc) const
        {
            return sc.paddingLeft.GetValueAsString();
        }
        void paddingLeft::OnChanged(GUIElement &element)
        {
            element.server.OnPaddingXChanged(element);
        }


        // padding-right
        bool paddingRight::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.paddingRight.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String paddingRight::Get(GUIStyleClass &sc) const
        {
            return sc.paddingRight.GetValueAsString();
        }
        void paddingRight::OnChanged(GUIElement &element)
        {
            element.server.OnPaddingXChanged(element);
        }

    
        // padding-top
        bool paddingTop::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.paddingTop.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String paddingTop::Get(GUIStyleClass &sc) const
        {
            return sc.paddingTop.GetValueAsString();
        }
        void paddingTop::OnChanged(GUIElement &element)
        {
            element.server.OnPaddingYChanged(element);
        }


        // padding-bottom
        bool paddingBottom::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.paddingBottom.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String paddingBottom::Get(GUIStyleClass &sc) const
        {
            return sc.paddingBottom.GetValueAsString();
        }
        void paddingBottom::OnChanged(GUIElement &element)
        {
            element.server.OnPaddingYChanged(element);
        }


        // margin-left
        bool marginLeft::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.marginLeft.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String marginLeft::Get(GUIStyleClass &sc) const
        {
            return sc.marginLeft.GetValueAsString();
        }
        void marginLeft::OnChanged(GUIElement &element)
        {
            element.server.OnMarginsXChanged(element);
        }

    
        // margin-right
        bool marginRight::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.marginRight.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String marginRight::Get(GUIStyleClass &sc) const
        {
            return sc.marginRight.GetValueAsString();
        }
        void marginRight::OnChanged(GUIElement &element)
        {
            element.server.OnMarginsXChanged(element);
        }

    
        // margin-top
        bool marginTop::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.marginTop.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String marginTop::Get(GUIStyleClass &sc) const
        {
            return sc.marginTop.GetValueAsString();
        }
        void marginTop::OnChanged(GUIElement &element)
        {
            element.server.OnMarginsYChanged(element);
        }

    
        // margin-bottom
        bool marginBottom::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.marginBottom.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String marginBottom::Get(GUIStyleClass &sc) const
        {
            return sc.marginBottom.GetValueAsString();
        }
        void marginBottom::OnChanged(GUIElement &element)
        {
            element.server.OnMarginsYChanged(element);
        }


        // child-plane
        bool childPlane::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String childPlane::Get(GUIStyleClass &sc) const
        {
            return sc.childPlane.GetValueAsString();
        }
        void childPlane::OnChanged(GUIElement &element)
        {
            element.server.OnChildPlaneChanged(element);
        }

    
        // horizontal-align
        bool horizontalAlign::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String horizontalAlign::Get(GUIStyleClass &sc) const
        {
            return sc.horizontalAlign.GetValueAsString();
        }
        void horizontalAlign::OnChanged(GUIElement &element)
        {
            element.server.OnHorizontalAlignChanged(element);
        }


        // vertical-align
        bool verticalAlign::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String verticalAlign::Get(GUIStyleClass &sc) const
        {
            return sc.verticalAlign.GetValueAsString();
        }
        void verticalAlign::OnChanged(GUIElement &element)
        {
            element.server.OnVerticalAlignChanged(element);
        }


        // cursor
        bool cursor::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String cursor::Get(GUIStyleClass &sc) const
        {
            return sc.cursor.GetValueAsString();
        }
        void cursor::OnChanged(GUIElement &)
        {
        }


        // visible
        bool visible::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.visible.SetFromString(value, valueSize))
            {
                this->Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String visible::Get(GUIStyleClass &sc) const
        {
            return sc.visible.GetValueAsString();
        }
        void visible::OnChanged(GUIElement &element)
        {
            element.server.OnVisibilityChanged(element);
        }


        // z-index
        bool zIndex::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String zIndex::Get(GUIStyleClass &sc) const
        {
            return sc.zIndex.GetValueAsString();
        }
        void zIndex::OnChanged(GUIElement &element)
        {
            element.server.OnZIndexChanged(element);
        }


        // transparent-mouse-input
        bool transparentMouseInput::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.transparentMouseInput.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String transparentMouseInput::Get(GUIStyleClass &sc) const
        {
            return sc.transparentMouseInput.GetValueAsString();
        }
        void transparentMouseInput::OnChanged(GUIElement &)
        {
        }


        // enabled
        void RecursivelyActivateDisabledModifierClass(GUIElement &element)
        {
            element.style.DeactivateAllModifierClasses();
            element.style.ActivateModifierClasses(GUIStyleClassType_Disabled);

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

        void RecursivelyDeactivateDisabledModifierClass(GUIElement &element)
        {
            if (element.style.enabled->value)
            {
                element.style.DeactivateModifierClasses(GUIStyleClassType_Disabled);


                // Children.
                for (auto iChild = element.firstChild; iChild != nullptr; iChild = iChild->nextSibling)
                {
                    RecursivelyDeactivateDisabledModifierClass(*iChild);
                }
            }
        }

        bool enabled::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.enabled.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String enabled::Get(GUIStyleClass &sc) const
        {
            return sc.enabled.GetValueAsString();
        }
        void enabled::OnChanged(GUIElement &element)
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
        bool textCursorColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textCursorColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textCursorColour::Get(GUIStyleClass &sc) const
        {
            return sc.textCursorColour.GetValueAsString();
        }
        void textCursorColour::OnChanged(GUIElement &)
        {
        }
        
        
        // can-receive-focus-from-mouse
        bool canReceiveFocusFromMouse::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.canReceiveFocusFromMouse.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String canReceiveFocusFromMouse::Get(GUIStyleClass &sc) const
        {
            return sc.canReceiveFocusFromMouse.GetValueAsString();
        }
        void canReceiveFocusFromMouse::OnChanged(GUIElement &)
        {
        }


        // positioning
        bool positioning::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String positioning::Get(GUIStyleClass &sc) const
        {
            return sc.positioning.GetValueAsString();
        }
        void positioning::OnChanged(GUIElement &element)
        {
            element.server.OnPositioningChanged(element);
        }


        // left
        bool left::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.left.SetFromString(value, valueSize))
            {
                sc.rightHasPriority = false;       // <-- Set this before refreshing.
                Refresh(sc);
            
                return true;
            }

            return false;
        }
        GT::String left::Get(GUIStyleClass &sc) const
        {
            return sc.left.GetValueAsString();
        }
        void left::OnChanged(GUIElement &element)
        {
            element.server.OnLeftChanged(element);
        }


        // right
        bool right::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.right.SetFromString(value, valueSize))
            {
                sc.rightHasPriority = true;       // <-- Set this before refreshing.
                Refresh(sc);
            
                return true;
            }

            return false;
        }
        GT::String right::Get(GUIStyleClass &sc) const
        {
            return sc.right.GetValueAsString();
        }
        void right::OnChanged(GUIElement &element)
        {
            element.server.OnRightChanged(element);
        }


        // top
        bool top::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.top.SetFromString(value, valueSize))
            {
                sc.bottomHasPriority = false;      // <-- Set this before refreshing.
                Refresh(sc);
            
                return true;
            }

            return false;
        }
        GT::String top::Get(GUIStyleClass &sc) const
        {
            return sc.top.GetValueAsString();
        }
        void top::OnChanged(GUIElement &element)
        {
            element.server.OnTopChanged(element);
        }


        // bottom
        bool bottom::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.bottom.SetFromString(value, valueSize))
            {
                sc.bottomHasPriority = true;      // <-- Set this before refreshing.
                Refresh(sc);
            
                return true;
            }

            return false;
        }
        GT::String bottom::Get(GUIStyleClass &sc) const
        {
            return sc.bottom.GetValueAsString();
        }
        void bottom::OnChanged(GUIElement &element)
        {
            element.server.OnBottomChanged(element);
        }


        // position-origin
        bool positionOrigin::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.positionOrigin.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String positionOrigin::Get(GUIStyleClass &sc) const
        {
            return sc.positionOrigin.GetValueAsString();
        }
        void positionOrigin::OnChanged(GUIElement &element)
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
        bool innerOffsetX::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.innerOffsetX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String innerOffsetX::Get(GUIStyleClass &sc) const
        {
            return sc.innerOffsetX.GetValueAsString();
        }
        void innerOffsetX::OnChanged(GUIElement &element)
        {
            element.server.OnInnerOffsetXChanged(element);
        }


        // inner-offset-y
        bool innerOffsetY::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.innerOffsetY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String innerOffsetY::Get(GUIStyleClass &sc) const
        {
            return sc.innerOffsetY.GetValueAsString();
        }
        void innerOffsetY::OnChanged(GUIElement &element)
        {
            element.server.OnInnerOffsetYChanged(element);
        }


        // font-family
        bool fontFamily::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.fontFamily.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String fontFamily::Get(GUIStyleClass &sc) const
        {
            return sc.fontFamily.GetValueAsString();
        }
        void fontFamily::OnChanged(GUIElement &element)
        {
            element.UpdateFontFromStyle();
        }

    
        // font-size
        bool fontSize::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.fontSize.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String fontSize::Get(GUIStyleClass &sc) const
        {
            return sc.fontSize.GetValueAsString();
        }
        void fontSize::OnChanged(GUIElement &element)
        {
            element.UpdateFontFromStyle();
        }


        // font-weight
        bool fontWeight::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.fontWeight.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }
            
            return false;
        }
        GT::String fontWeight::Get(GUIStyleClass &sc) const
        {
            return sc.fontWeight.GetValueAsString();
        }
        void fontWeight::OnChanged(GUIElement &element)
        {
            element.UpdateFontFromStyle();
        }
        
        
        // font-slant
        bool fontSlant::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.fontSlant.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }
            
            return false;
        }
        GT::String fontSlant::Get(GUIStyleClass &sc) const
        {
            return sc.fontSlant.GetValueAsString();
        }
        void fontSlant::OnChanged(GUIElement &element)
        {
            element.UpdateFontFromStyle();
        }


        // text-color
        bool textColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textColour::Get(GUIStyleClass &sc) const
        {
            return sc.textColour.GetValueAsString();
        }
        void textColour::OnChanged(GUIElement &element)
        {
            element.InvalidateTextRenderingData();
        }

        // text-selection-color
        bool textSelectionColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textSelectionColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textSelectionColour::Get(GUIStyleClass &sc) const
        {
            return sc.textSelectionColour.GetValueAsString();
        }
        void textSelectionColour::OnChanged(GUIElement &element)
        {
            element.InvalidateTextRenderingData();
        }

        // text-selection-background-color
        bool textSelectionBackgroundColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textSelectionBackgroundColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textSelectionBackgroundColour::Get(GUIStyleClass &sc) const
        {
            return sc.textSelectionBackgroundColour.GetValueAsString();
        }
        void textSelectionBackgroundColour::OnChanged(GUIElement &element)
        {
            element.InvalidateTextRenderingData();
        }

        // text-selection-background-color-blurred
        bool textSelectionBackgroundColourBlurred::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.textSelectionBackgroundColourBlurred.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String textSelectionBackgroundColourBlurred::Get(GUIStyleClass &sc) const
        {
            return sc.textSelectionBackgroundColourBlurred.GetValueAsString();
        }
        void textSelectionBackgroundColourBlurred::OnChanged(GUIElement &element)
        {
            element.InvalidateTextRenderingData();
        }




        // editable-text
        bool editableText::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.editableText.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String editableText::Get(GUIStyleClass &sc) const
        {
            return sc.editableText.GetValueAsString();
        }
        void editableText::OnChanged(GUIElement &)
        {
        }


        // single-line-text
        bool singleLineText::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.singleLineText.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String singleLineText::Get(GUIStyleClass &sc) const
        {
            return sc.singleLineText.GetValueAsString();
        }
        void singleLineText::OnChanged(GUIElement &)
        {
            // TODO: Update the text manager.
        }

        

        // opacity
        bool opacity::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.opacity.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String opacity::Get(GUIStyleClass &sc) const
        {
            return sc.opacity.GetValueAsString();
        }
        void opacity::OnChanged(GUIElement &element)
        {
            element.UpdateAbsoluteOpacity();    // <-- Recursive on children.
        }


        // compound-opacity
        bool compoundOpacity::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.compoundOpacity.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String compoundOpacity::Get(GUIStyleClass &sc) const
        {
            return sc.compoundOpacity.GetValueAsString();
        }
        void compoundOpacity::OnChanged(GUIElement &element)
        {
            element.UpdateAbsoluteOpacity();    // <-- Recursive on children.
        }


        // enable-shadow
        bool enableShadow::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.enableShadow.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String enableShadow::Get(GUIStyleClass &sc) const
        {
            return sc.enableShadow.GetValueAsString();
        }
        void enableShadow::OnChanged(GUIElement &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-color
        bool shadowColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowColour.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowColour::Get(GUIStyleClass &sc) const
        {
            return sc.shadowColour.GetValueAsString();
        }
        void shadowColour::OnChanged(GUIElement &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-blur-radius
        bool shadowBlurRadius::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowBlurRadius.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowBlurRadius::Get(GUIStyleClass &sc) const
        {
            return sc.shadowBlurRadius.GetValueAsString();
        }
        void shadowBlurRadius::OnChanged(GUIElement &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-offset-x
        bool shadowOffsetX::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowOffsetX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowOffsetX::Get(GUIStyleClass &sc) const
        {
            return sc.shadowOffsetX.GetValueAsString();
        }
        void shadowOffsetX::OnChanged(GUIElement &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-offset-y
        bool shadowOffsetY::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowOffsetY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowOffsetY::Get(GUIStyleClass &sc) const
        {
            return sc.shadowOffsetY.GetValueAsString();
        }
        void shadowOffsetY::OnChanged(GUIElement &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-extrusion-x
        bool shadowExtrusionX::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowExtrusionX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowExtrusionX::Get(GUIStyleClass &sc) const
        {
            return sc.shadowExtrusionX.GetValueAsString();
        }
        void shadowExtrusionX::OnChanged(GUIElement &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-extrusion-y
        bool shadowExtrusionY::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowExtrusionY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowExtrusionY::Get(GUIStyleClass &sc) const
        {
            return sc.shadowExtrusionY.GetValueAsString();
        }
        void shadowExtrusionY::OnChanged(GUIElement &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // shadow-opacity
        bool shadowOpacity::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.shadowOpacity.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String shadowOpacity::Get(GUIStyleClass &sc) const
        {
            return sc.shadowOpacity.GetValueAsString();
        }
        void shadowOpacity::OnChanged(GUIElement &element)
        {
            element.InvalidateShadowRenderingData();
        }


        // allow-mouse-drag
        bool allowMouseDrag::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.allowMouseDrag.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String allowMouseDrag::Get(GUIStyleClass &sc) const
        {
            return sc.allowMouseDrag.GetValueAsString();
        }
        void allowMouseDrag::OnChanged(GUIElement &)
        {
        }


        // constrain-mouse-drag-x
        bool constrainMouseDragX::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.constrainMouseDragX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String constrainMouseDragX::Get(GUIStyleClass &sc) const
        {
            return sc.constrainMouseDragX.GetValueAsString();
        }
        void constrainMouseDragX::OnChanged(GUIElement &)
        {
        }

        // constrain-mouse-drag-y
        bool constrainMouseDragY::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.constrainMouseDragY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String constrainMouseDragY::Get(GUIStyleClass &sc) const
        {
            return sc.constrainMouseDragY.GetValueAsString();
        }
        void constrainMouseDragY::OnChanged(GUIElement &)
        {
        }


        // mouse-drag-clamp-mode-x
        bool mouseDragClampModeX::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.mouseDragClampModeX.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String mouseDragClampModeX::Get(GUIStyleClass &sc) const
        {
            return sc.mouseDragClampModeX.GetValueAsString();
        }
        void mouseDragClampModeX::OnChanged(GUIElement &)
        {
        }

        // mouse-drag-clamp-mode-y
        bool mouseDragClampModeY::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.mouseDragClampModeY.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String mouseDragClampModeY::Get(GUIStyleClass &sc) const
        {
            return sc.mouseDragClampModeY.GetValueAsString();
        }
        void mouseDragClampModeY::OnChanged(GUIElement &)
        {
        }


        // allow-mouse-resize
        bool allowMouseResize::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.allowMouseResize.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String allowMouseResize::Get(GUIStyleClass &sc) const
        {
            return sc.allowMouseResize.GetValueAsString();
        }
        void allowMouseResize::OnChanged(GUIElement &)
        {
        }

        // left-gripper-width
        bool leftGripperWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.leftGripperWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String leftGripperWidth::Get(GUIStyleClass &sc) const
        {
            return sc.leftGripperWidth.GetValueAsString();
        }
        void leftGripperWidth::OnChanged(GUIElement &)
        {
        }

        // right-gripper-width
        bool rightGripperWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.rightGripperWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String rightGripperWidth::Get(GUIStyleClass &sc) const
        {
            return sc.rightGripperWidth.GetValueAsString();
        }
        void rightGripperWidth::OnChanged(GUIElement &)
        {
        }

        // top-gripper-width
        bool topGripperWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.topGripperWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String topGripperWidth::Get(GUIStyleClass &sc) const
        {
            return sc.topGripperWidth.GetValueAsString();
        }
        void topGripperWidth::OnChanged(GUIElement &)
        {
        }

        // bottom-gripper-width
        bool bottomGripperWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            if (sc.bottomGripperWidth.SetFromString(value, valueSize))
            {
                Refresh(sc);
                return true;
            }

            return false;
        }
        GT::String bottomGripperWidth::Get(GUIStyleClass &sc) const
        {
            return sc.bottomGripperWidth.GetValueAsString();
        }
        void bottomGripperWidth::OnChanged(GUIElement &)
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
        bool backgroundAlign::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void backgroundAlign::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "background-align-x");
            sc.server.UnsetGUIStyleAttribute(sc, "background-align-y");
        }
        GT::String backgroundAlign::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundAlignX.GetValueAsString() + " " + sc.backgroundAlignY.GetValueAsString();
        }
        void backgroundAlign::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool backgroundRepeat::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void backgroundRepeat::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "background-repeat-x");
            sc.server.UnsetGUIStyleAttribute(sc, "background-repeat-y");
        }
        GT::String backgroundRepeat::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundRepeatX.GetValueAsString() + " " + sc.backgroundRepeatY.GetValueAsString();
        }
        void backgroundRepeat::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool background::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
        {
            // At the moment background is the same as background-color. But in the future we will add things like images.
            if (sc.backgroundColour.SetFromString(value, valueSize))
            {
                backgroundColour::Refresh(sc);
                return true;
            }

            return false;
        }
        void background::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "background-color");
        }
        GT::String background::Get(GUIStyleClass &sc) const
        {
            return sc.backgroundColour.GetValueAsString();
        }
        void background::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
        {
            sc.backgroundColour.inherit = inherit;
            sc.backgroundColour.isset   = inherit;

            if (refresh)
            {
                backgroundColour::Refresh(sc);
            }
        }


        // border-width
        bool borderWidth::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void borderWidth::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "border-left-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-right-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-top-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-bottom-width");
        }
        GT::String borderWidth::Get(GUIStyleClass &sc) const
        {
            return sc.borderLeftWidth.GetValueAsString() + " " + sc.borderRightWidth.GetValueAsString() + " " + sc.borderTopWidth.GetValueAsString() + " " + sc.borderBottomWidth.GetValueAsString();
        }
        void borderWidth::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool borderColour::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void borderColour::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "border-left-color");
            sc.server.UnsetGUIStyleAttribute(sc, "border-right-color");
            sc.server.UnsetGUIStyleAttribute(sc, "border-top-color");
            sc.server.UnsetGUIStyleAttribute(sc, "border-bottom-color");
        }
        GT::String borderColour::Get(GUIStyleClass &sc) const
        {
            return sc.borderLeftColour.GetValueAsString() + " " + sc.borderRightColour.GetValueAsString() + " " + sc.borderTopColour.GetValueAsString() + " " + sc.borderBottomColour.GetValueAsString();
        }
        void borderColour::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool borderLeft::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void borderLeft::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "border-left-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-left-color");
        }
        GT::String borderLeft::Get(GUIStyleClass &sc) const
        {
            return sc.borderLeftWidth.GetValueAsString() + " " + sc.borderLeftColour.GetValueAsString();
        }
        void borderLeft::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool borderRight::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void borderRight::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "border-right-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-right-color");
        }
        GT::String borderRight::Get(GUIStyleClass &sc) const
        {
            return sc.borderRightWidth.GetValueAsString() + " " + sc.borderRightColour.GetValueAsString();
        }
        void borderRight::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool borderTop::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void borderTop::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "border-top-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-top-color");
        }
        GT::String borderTop::Get(GUIStyleClass &sc) const
        {
            return sc.borderTopWidth.GetValueAsString() + " " + sc.borderTopColour.GetValueAsString();
        }
        void borderTop::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool borderBottom::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void borderBottom::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "border-bottom-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-bottom-color");
        }
        GT::String borderBottom::Get(GUIStyleClass &sc) const
        {
            return sc.borderBottomWidth.GetValueAsString() + " " + sc.borderBottomColour.GetValueAsString();
        }
        void borderBottom::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool border::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        GT::String border::Get(GUIStyleClass &sc) const
        {
            return sc.borderLeftWidth.GetValueAsString() + " " + sc.borderLeftColour.GetValueAsString();
        }
        void border::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "border-left-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-right-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-top-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-bottom-width");
            sc.server.UnsetGUIStyleAttribute(sc, "border-left-colour");
            sc.server.UnsetGUIStyleAttribute(sc, "border-right-colour");
            sc.server.UnsetGUIStyleAttribute(sc, "border-top-colour");
            sc.server.UnsetGUIStyleAttribute(sc, "border-bottom-colour");
        }
        void border::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool padding::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void padding::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "padding-left");
            sc.server.UnsetGUIStyleAttribute(sc, "padding-right");
            sc.server.UnsetGUIStyleAttribute(sc, "padding-top");
            sc.server.UnsetGUIStyleAttribute(sc, "padding-bottom");
        }
        GT::String padding::Get(GUIStyleClass &sc) const
        {
            return sc.paddingLeft.GetValueAsString() + " " + sc.paddingRight.GetValueAsString() + " " + sc.paddingTop.GetValueAsString() + " " + sc.paddingBottom.GetValueAsString();
        }
        void padding::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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
        bool margin::Set(GUIStyleClass &sc, const char* value, ptrdiff_t valueSize)
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
        void margin::Unset(GUIStyleClass &sc)
        {
            sc.server.UnsetGUIStyleAttribute(sc, "margin-left");
            sc.server.UnsetGUIStyleAttribute(sc, "margin-right");
            sc.server.UnsetGUIStyleAttribute(sc, "margin-top");
            sc.server.UnsetGUIStyleAttribute(sc, "margin-bottom");
        }
        GT::String margin::Get(GUIStyleClass &sc) const
        {
            return sc.marginLeft.GetValueAsString() + " " + sc.marginRight.GetValueAsString() + " " + sc.marginTop.GetValueAsString() + " " + sc.marginBottom.GetValueAsString();
        }
        void margin::SetInherit(GUIStyleClass &sc, bool inherit, bool refresh)
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



namespace GT
{
    GUIStyleAttributeHandler::GUIStyleAttributeHandler()
    {
    }
    
    GUIStyleAttributeHandler::~GUIStyleAttributeHandler()
    {
    }
}
