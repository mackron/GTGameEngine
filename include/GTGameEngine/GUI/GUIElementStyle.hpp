// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_ElementStyle
#define GT_GUI_ElementStyle

#include <cstdint>
#include <GTLib/Colour.hpp>

namespace GT
{
    static const unsigned int GUIStyleNumber_MinUnsigned = 0x0;
    static const unsigned int GUIStyleNumber_MaxUnsigned = 0xFFFFFFF;

    static const int          GUIStyleNumber_MinSigned   = -134217728;
    static const int          GUIStyleNumber_MaxSigned   =  134217727;


    /// Bit codes for boolean style options.
    static const uint32_t StyleBit_Invisible                = (1 << 0);
    static const uint32_t StyleBit_Disabled                 = (1 << 1);
    static const uint32_t StyleBit_FlexChildrenWidth        = (1 << 2);
    static const uint32_t StyleBit_FlexChildrenHeight       = (1 << 3);
    static const uint32_t StyleBit_PassThroughCursorInput   = (1 << 4);
    static const uint32_t StyleBit_CanReceiveFocusFromMouse = (1 << 5);
    static const uint32_t StyleBit_EditableText             = (1 << 6);
    static const uint32_t StyleBit_MultiLineText            = (1 << 7);
    static const uint32_t StyleBit_CompoundOpacity          = (1 << 8);
    static const uint32_t StyleBit_EnableShadow             = (1 << 9);
    static const uint32_t StyleBit_RightHasPriority         = (1 << 10);
    static const uint32_t StyleBit_BottomHasPriority        = (1 << 11);
    static const uint32_t StyleBit_InheritFont              = (1 << 12);        // Whether or not the font should be inherited from the parent element.

    // Type codes for number values. Note that these are bit-shifted such that they are in the top 8-bits. This is to make
    // it easier to construct the encoded number value.
    static const uint32_t NumberType_Absolute = (0 << 28);
    static const uint32_t NumberType_Percent  = (1 << 28);
    static const uint32_t NumberType_Pixels   = (2 << 28);
    static const uint32_t NumberType_Points   = (3 << 28);        // Same as pixels, but scaled based on DPI.
    static const uint32_t NumberType_Auto     = (4 << 28);        // Usually depends on context.

    // The two possible axes for children - horizontal or vertical. Only requires 1 bit.
    enum ChildAxis
    {
        ChildAxis_Vertical   = 0,
        ChildAxis_Horizontal = 1
    };

    // The possible horizontal alignments. Requires 2 bits.
    enum HorizontalAlign
    {
        HorizontalAlign_Left   = 0,
        HorizontalAlign_Center = 1,
        HorizontalAlign_Right  = 2
    };

    // The possible vertical alignments. Requires 2 bits.
    enum VerticalAlign
    {
        VerticalAlign_Top    = 0,
        VerticalAlign_Center = 1,
        VerticalAlign_Bottom = 2
    };

    // The possible positioning modes (absolute, relative, auto)
    enum Positioning
    {
        Positioning_Auto     = 0,
        Positioning_Relative = 1,
        Positioning_Absolute = 2
    };

    // The possible boundries for doing relative sizing based on the size of a parent element. Requires 2 bits.
    enum ChildrenSizeBoundary
    {
        ChildrenSizeBoundary_Inner       = 0,
        ChildrenSizeBoundary_InnerBorder = 1,
        ChildrenSizeBoundary_Outer       = 2
    };

    // The possible origin for relative positioning of child elements. Requires 2 bits.
    enum PositionOrigin
    {
        PositionOrigin_Inner       = 0,         // Inside the border and padding.
        PositionOrigin_InnerBorder = 1,         // Inside the border, but outside the padding.
        PositionOrigin_Outer       = 2          // Outside the border, but inside the margin.
    };

    // The possible clipping mode for clipping against the parent. Request 2 bits.
    enum ClippingMode
    {
        ClippingMode_Auto     = 0,
        ClippingMode_Enabled  = 1,
        ClippingMode_Disabled = 2,

        ClippingMode_Default = ClippingMode_Auto
    };

    // The clipping boundary for clipping children.
    enum ClippingBoundary
    {
        ClippingBoundary_Inner       = 0,
        ClippingBoundary_InnerBorder = 1,
        ClippingBoundary_Outer       = 2
    };


    // The boundary of the background.
    enum BackgroundBoundary
    {
        BackgroundBoundary_InnerBorder = 0,     ///< Inside border, outside padding.
        BackgroundBoundary_Inner       = 1,     ///< Inside border, inside padding
        BackgroundBoundary_Outer       = 2      ///< Outside border
    };

    // The alignment of the background image.
    enum BackgroundAlign
    {
        BackgroundAlign_Center = 0,

        BackgroundAlign_Left   = 1,
        BackgroundAlign_Right  = 2,

        BackgroundAlign_Top    = 1,
        BackgroundAlign_Bottom = 2
    };

    // The repeat mode of the background image.
    enum BackgroundRepeat
    {
        BackgroundRepeat_Stretch = 0,       ///< No repeat, stretch.
        BackgroundRepeat_None    = 1,       ///< No repeat, no stretch.
        BackgroundRepeat_Repeat  = 2        ///< Repeat, no stretch.
    };


    // The various font weights.
    enum FontWeight
    {
        FontWeight_Medium     = 0,
        FontWeight_Thin       = 1,
        FontWeight_ExtraLight = 2,
        FontWeight_Light      = 3,
        FontWeight_SemiBold   = 4,
        FontWeight_Bold       = 5,
        FontWeight_ExtraBold  = 6,
        FontWeight_Heavy      = 7,

        FontWeight_Normal     = FontWeight_Medium,
        FontWeight_Default    = FontWeight_Medium
    };

    // The various font slants.
    enum FontSlant
    {
        FontSlant_None    = 0,
        FontSlant_Italic  = 1,
        FontSlant_Oblique = 2
    };


    /// Structure containing the raw data of a GUI style.
    ///
    /// This structure is intentionally compressed. It is designed such that the operations below this structure declaration should
    /// control the style data.
    ///
    /// Some rules for this structure:
    ///     - Only use basic integer types
    ///     - Handles to complex types like fonts should be mapped to an n-bit ID (usually a handle) which will be controlled by the GUI context.
    struct GUIElementStyle
    {
        uint32_t left;                                      // [1,4] type | [5,32] value
        uint32_t top;                                       // [1,4] type | [5,32] value
        uint32_t right;                                     // [1,4] type | [5,32] value
        uint32_t bottom;                                    // [1,4] type | [5,32] value

        uint32_t width;                                     // [1,4] type | [5,32] value
        uint32_t minWidth;                                  // [1,4] type | [5,32] value
        uint32_t maxWidth;                                  // [1,4] type | [5,32] value

        uint32_t height;                                    // [1,4] type | [5,32] value
        uint32_t minHeight;                                 // [1,4] type | [5,32] value
        uint32_t maxHeight;                                 // [1,4] type | [5,32] value

        uint8_t childAxisAndAlignmentAndPositioning;        // [1,1] child axis              | [2,3] horizontal align         | [4,5] vertical align  | [6,7] positioning
        uint8_t childrenSizeBoundaryAndPositionOrigin;      // [1,2] children width boundary | [3,4] children height boundary | [5,6] position origin
        uint8_t clipping;                                   // [1,2] clipping mode           | [3,4] child clipping boundary

        uint8_t  backgroundImageAlignAndRepeat;             // [1,2] X align | [3,4] Y align | [4,6] X repeat | [7,8] Y repeat
        uint8_t  backgroundColourR;
        uint8_t  backgroundColourG;
        uint8_t  backgroundColourB;
        uint8_t  backgroundColourA;
        uint32_t backgroundImagePath;                       // 32-bit hash representation of the image path.
        uint32_t backgroundImageColor;                      // [1,8] red | [9,16] green | [17,24] blue | [24,32] alpha
        uint32_t backgroundSubImageOffsetX;                 // Unsigned and always in texels.
        uint32_t backgroundSubImageOffsetY;                 // Unsigned and always in texels.
        uint32_t backgroundSubImageWidth;                   // Unsigned and always in texels. If 0, uses the width of the entire image.
        uint32_t backgroundSubImageHeight;                  // Unsigned and always in texels. If 0, uses the height of the entire image.
        uint32_t backgroundBoundary;                        // [1,2] background color boundary | [3,4] backgound image boundary
        uint32_t backgroundImageScaleX;                     // 32-bit float
        uint32_t backgroundImageScaleY;                     // 32-bit float

        uint32_t borderLeftWidth;                           // [1,4] type | [5,32] value
        uint32_t borderTopWidth;                            // [1,4] type | [5,32] value
        uint32_t borderRightWidth;                          // [1,4] type | [5,32] value
        uint32_t borderBottomWidth;                         // [1,4] type | [5,32] value
        uint32_t borderLeftColour;                          // [1,8] red | [9,16] green | [17,24] blue | [24,32] alpha
        uint32_t borderTopColour;                           // [1,8] red | [9,16] green | [17,24] blue | [24,32] alpha
        uint32_t borderRightColour;                         // [1,8] red | [9,16] green | [17,24] blue | [24,32] alpha
        uint32_t borderBottomColour;                        // [1,8] red | [9,16] green | [17,24] blue | [24,32] alpha
        uint32_t borderLeftMaskOffset;                      // [1,4] type | [5,32] value
        uint32_t borderTopMaskOffset;                       // [1,4] type | [5,32] value
        uint32_t borderRightMaskOffset;                     // [1,4] type | [5,32] value
        uint32_t borderBottomMaskOffset;                    // [1,4] type | [5,32] value
        uint32_t borderLeftMaskLength;                      // [1,4] type | [5,32] value
        uint32_t borderTopMaskLength;                       // [1,4] type | [5,32] value
        uint32_t borderRightMaskLength;                     // [1,4] type | [5,32] value
        uint32_t borderBottomMaskLength;                    // [1,4] type | [5,32] value
        uint32_t borderLeftMaskColor;
        uint32_t borderTopMaskColor;
        uint32_t borderRightMaskColor;
        uint32_t borderBottomMaskColor;



        uint32_t paddingLeft;                               // [1,4] type | [5,32] value
        uint32_t paddingTop;                                // [1,4] type | [5,32] value
        uint32_t paddingRight;                              // [1,4] type | [5,32] value
        uint32_t paddingBottom;                             // [1,4] type | [5,32] value

        uint32_t marginLeft;                                // [1,4] type | [5,32] value
        uint32_t marginTop;                                 // [1,4] type | [5,32] value
        uint32_t marginRight;                               // [1,4] type | [5,32] value
        uint32_t marginBottom;                              // [1,4] type | [5,32] value

        uint32_t fontFamily;                                // 32-bit hash representation of the font family.
        uint32_t fontSize;                                  // [1,4] type   | [5,32] value
        uint32_t fontStyle;                                 // [1,4] weight | [5,7]  slant

        uint8_t textColorR;
        uint8_t textColorG;
        uint8_t textColorB;
        uint8_t textColorA;


        uint16_t booleanField;
        uint16_t padding2;
    };


    //////////////////////////////////////////////
    // Layout Styles

    // left
    inline void GUIElementStyle_Set_left(GUIElementStyle &style, int32_t value, uint32_t type = NumberType_Absolute)
    {
        style.left = ((0x80000000 & value) >> 4) | (0x0FFFFFFF & value) | type;
    }
    inline int32_t GUIElementStyle_Get_left(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.left & 0xF0000000;

        if (style.left & 0x08000000)
        {
            return (0xF8000000 | (style.left & 0x07FFFFFF));
        }
        else
        {
            return style.left & 0x07FFFFFF;
        }
    }

    // top
    inline void GUIElementStyle_Set_top(GUIElementStyle &style, int32_t value, uint32_t type = NumberType_Absolute)
    {
        style.top = ((0x80000000 & value) >> 4) | (0x0FFFFFFF & value) | type;
    }
    inline int32_t GUIElementStyle_Get_top(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.top & 0xF0000000;

        if (style.top & 0x08000000)
        {
            return (0xF8000000 | (style.top & 0x07FFFFFF));
        }
        else
        {
            return style.top & 0x07FFFFFF;
        }
    }

    // right
    inline void GUIElementStyle_Set_right(GUIElementStyle &style, int32_t value, uint32_t type = NumberType_Absolute)
    {
        style.right = ((0x80000000 & value) >> 4) | (0x0FFFFFFF & value) | type;
    }
    inline int32_t GUIElementStyle_Get_right(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.right & 0xF0000000;

        if (style.right & 0x08000000)
        {
            return (0xF8000000 | (style.right & 0x07FFFFFF));
        }
        else
        {
            return style.right & 0x07FFFFFF;
        }
    }

    // bottom
    inline void GUIElementStyle_Set_bottom(GUIElementStyle &style, int32_t value, uint32_t type = NumberType_Absolute)
    {
        style.bottom = ((0x80000000 & value) >> 4) | (0x0FFFFFFF & value) | type;
    }
    inline int32_t GUIElementStyle_Get_bottom(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.bottom & 0xF0000000;

        if (style.bottom & 0x08000000)
        {
            return (0xF8000000 | (style.bottom & 0x07FFFFFF));
        }
        else
        {
            return style.bottom & 0x07FFFFFF;
        }
    }


    // width
    inline void GUIElementStyle_Set_width(GUIElementStyle &style, uint32_t value, uint32_t type = NumberType_Absolute)
    {
        style.width = value | type;
    }
    inline uint32_t GUIElementStyle_Get_width(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.width & 0xF0000000;
        return style.width & 0x0FFFFFFF;
    }

    // min-width
    inline void GUIElementStyle_Set_minwidth(GUIElementStyle &style, uint32_t value, uint32_t type = NumberType_Absolute)
    {
        style.minWidth = value | type;
    }
    inline uint32_t GUIElementStyle_Get_minwidth(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.minWidth & 0xF0000000;
        return style.minWidth & 0x0FFFFFFF;
    }

    // min-width
    inline void GUIElementStyle_Set_maxwidth(GUIElementStyle &style, uint32_t value, uint32_t type = NumberType_Absolute)
    {
        style.maxWidth = value | type;
    }
    inline uint32_t GUIElementStyle_Get_maxwidth(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.maxWidth & 0xF0000000;
        return style.maxWidth & 0x0FFFFFFF;
    }


    // height
    inline void GUIElementStyle_Set_height(GUIElementStyle &style, uint32_t value, uint32_t type = NumberType_Absolute)
    {
        style.height = value | type;
    }
    inline uint32_t GUIElementStyle_Get_height(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.height & 0xF0000000;
        return style.height & 0x0FFFFFFF;
    }

    // min-height
    inline void GUIElementStyle_Set_minheight(GUIElementStyle &style, uint32_t value, uint32_t type = NumberType_Absolute)
    {
        style.minHeight = value | type;
    }
    inline uint32_t GUIElementStyle_Get_minheight(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.minHeight & 0xF0000000;
        return style.minHeight & 0x0FFFFFFF;
    }

    // max-height
    inline void GUIElementStyle_Set_maxheight(GUIElementStyle &style, uint32_t value, uint32_t type = NumberType_Absolute)
    {
        style.maxHeight = value | type;
    }
    inline uint32_t GUIElementStyle_Get_maxheight(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.maxHeight & 0xF0000000;
        return style.maxHeight & 0x0FFFFFFF;
    }


    // Child axis.
    inline void GUIElementStyle_Set_childaxis(GUIElementStyle &style, ChildAxis childAxis)
    {
        style.childAxisAndAlignmentAndPositioning = (style.childAxisAndAlignmentAndPositioning & ~0x01) | static_cast<uint8_t>(childAxis);
    }
    inline ChildAxis GUIElementStyle_Get_childaxis(const GUIElementStyle &style)
    {
        return static_cast<ChildAxis>(style.childAxisAndAlignmentAndPositioning & 0x01);
    }

    // Horizontal align.
    inline void GUIElementStyle_Set_horizontalalign(GUIElementStyle &style, HorizontalAlign align)
    {
        style.childAxisAndAlignmentAndPositioning = uint8_t((style.childAxisAndAlignmentAndPositioning & ~0x06) | (uint8_t(align) << 1));
    }
    inline HorizontalAlign GUIElementStyle_Get_horizontalalign(const GUIElementStyle &style)
    {
        return static_cast<HorizontalAlign>((style.childAxisAndAlignmentAndPositioning & 0x06) >> 1);
    }

    // Vertical align.
    inline void GUIElementStyle_Set_verticalalign(GUIElementStyle &style, VerticalAlign align)
    {
        style.childAxisAndAlignmentAndPositioning = uint8_t((style.childAxisAndAlignmentAndPositioning & ~0x18) | (uint8_t(align) << 3));
    }
    inline VerticalAlign GUIElementStyle_Get_verticalalign(const GUIElementStyle &style)
    {
        return static_cast<VerticalAlign>((style.childAxisAndAlignmentAndPositioning & 0x18) >> 3);
    }

    // Positioning.
    inline void GUIElementStyle_Set_positioning(GUIElementStyle &style, Positioning positioning)
    {
        style.childAxisAndAlignmentAndPositioning = uint8_t((style.childAxisAndAlignmentAndPositioning & ~0x60) | (uint8_t(positioning) << 5));
    }
    inline Positioning GUIElementStyle_Get_positioning(GUIElementStyle &style)
    {
        return static_cast<Positioning>((style.childAxisAndAlignmentAndPositioning & 0x60) >> 5);
    }

    // Relative width boundary.
    inline void GUIElementStyle_Set_childrenwidthboundary(GUIElementStyle &style, ChildrenSizeBoundary boundary)
    {
        style.childrenSizeBoundaryAndPositionOrigin = uint8_t((style.childrenSizeBoundaryAndPositionOrigin & ~0x03) | (uint8_t(boundary)));
    }
    inline ChildrenSizeBoundary GUIElementStyle_Get_childrenwidthboundary(const GUIElementStyle &style)
    {
        return static_cast<ChildrenSizeBoundary>((style.childrenSizeBoundaryAndPositionOrigin & 0x03));
    }

    // Relative height boundary.
    inline void GUIElementStyle_Set_childrenheightboundary(GUIElementStyle &style, ChildrenSizeBoundary boundary)
    {
        style.childrenSizeBoundaryAndPositionOrigin = uint8_t((style.childrenSizeBoundaryAndPositionOrigin & ~0x0C) | (uint8_t(boundary) << 2));
    }
    inline ChildrenSizeBoundary GUIElementStyle_Get_childrenheightboundary(const GUIElementStyle &style)
    {
        return static_cast<ChildrenSizeBoundary>((style.childrenSizeBoundaryAndPositionOrigin & 0x0C) >> 2);
    }

    // Position origin.
    inline void GUIElementStyle_Set_positionorigin(GUIElementStyle &style, PositionOrigin origin)
    {
        style.childrenSizeBoundaryAndPositionOrigin = uint8_t((style.childrenSizeBoundaryAndPositionOrigin & ~0x30) | (uint8_t(origin) << 4));
    }
    inline PositionOrigin GUIElementStyle_Get_positionorigin(const GUIElementStyle &style)
    {
        return static_cast<PositionOrigin>((style.childrenSizeBoundaryAndPositionOrigin & 0x30) >> 4);
    }


    // Clipping mode.
    inline void GUIElementStyle_Set_clippingmode(GUIElementStyle &style, ClippingMode mode)
    {
        style.clipping = (style.clipping & ~0x03) | (static_cast<uint8_t>(mode));
    }
    inline ClippingMode GUIElementStyle_Get_clippingmode(const GUIElementStyle &style)
    {
        return static_cast<ClippingMode>((style.clipping & 0x03));
    }

    // Clipping boundary.
    inline void GUIElementStyle_Set_clippingboundary(GUIElementStyle &style, ClippingBoundary boundary)
    {
        style.clipping = uint8_t((style.clipping & ~0x0C) | (uint8_t(boundary) << 2));
    }
    inline ClippingBoundary GUIElementStyle_Get_clippingboundary(const GUIElementStyle &style)
    {
        return static_cast<ClippingBoundary>((style.clipping & 0x0C) >> 2);
    }


    ///////////////////////////////////////////
    // Decorative Styles

    // Background colour.
    inline void GUIElementStyle_Set_backgroundcolor(GUIElementStyle &style, const GTLib::Colour &colour)
    {
        style.backgroundColourR = uint8_t(colour.r * 255);
        style.backgroundColourG = uint8_t(colour.g * 255);
        style.backgroundColourB = uint8_t(colour.b * 255);
        style.backgroundColourA = uint8_t(colour.a * 255);
    }
    inline GTLib::Colour GUIElementStyle_Get_backgroundcolor(const GUIElementStyle &style)
    {
        GTLib::Colour colour;
        colour.r = style.backgroundColourR / 255.0f;
        colour.g = style.backgroundColourG / 255.0f;
        colour.b = style.backgroundColourB / 255.0f;
        colour.a = style.backgroundColourA / 255.0f;

        return colour;
    }

    inline void GUIElementStyle_Set_backgroundimage(GUIElementStyle &style, uint32_t backgroundImagePathHash)
    {
        style.backgroundImagePath = backgroundImagePathHash;
    }
    inline uint32_t GUIElementStyle_Get_backgroundimage(const GUIElementStyle &style)
    {
        return style.backgroundImagePath;
    }

    inline void GUIElementStyle_Set_backgroundimagecolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.backgroundImageColor = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_backgroundimagecolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.backgroundImageColor & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.backgroundImageColor & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.backgroundImageColor & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.backgroundImageColor & 0x000000FF) >>  0) / 255.0f;

        return color;
    }

    inline void GUIElementStyle_Set_backgroundsubimageoffsetx(GUIElementStyle &style, uint32_t value)
    {
        style.backgroundSubImageOffsetX = value;
    }
    inline uint32_t GUIElementStyle_Get_backgroundsubimageoffsetx(const GUIElementStyle &style)
    {
        return style.backgroundSubImageOffsetX;
    }

    inline void GUIElementStyle_Set_backgroundsubimageoffsety(GUIElementStyle &style, uint32_t value)
    {
        style.backgroundSubImageOffsetY = value;
    }
    inline uint32_t GUIElementStyle_Get_backgroundsubimageoffsety(const GUIElementStyle &style)
    {
        return style.backgroundSubImageOffsetY;
    }

    inline void GUIElementStyle_Set_backgroundsubimagewidth(GUIElementStyle &style, uint32_t value)
    {
        style.backgroundSubImageWidth = value;
    }
    inline uint32_t GUIElementStyle_Get_backgroundsubimagewidth(const GUIElementStyle &style)
    {
        return style.backgroundSubImageWidth;
    }

    inline void GUIElementStyle_Set_backgroundsubimageheight(GUIElementStyle &style, uint32_t value)
    {
        style.backgroundSubImageHeight = value;
    }
    inline uint32_t GUIElementStyle_Get_backgroundsubimageheight(const GUIElementStyle &style)
    {
        return style.backgroundSubImageHeight;
    }

    inline void GUIElementStyle_Set_backgroundcolorboundary(GUIElementStyle &style, BackgroundBoundary boundary)
    {
        style.backgroundBoundary = (style.backgroundBoundary & ~0x03) | (uint8_t(boundary) << 0);
    }
    inline BackgroundBoundary GUIElementStyle_Get_backgroundcolorboundary(const GUIElementStyle &style)
    {
        return static_cast<BackgroundBoundary>((style.backgroundBoundary & 0x03));
    }

    inline void GUIElementStyle_Set_backgroundimageboundary(GUIElementStyle &style, BackgroundBoundary boundary)
    {
        style.backgroundBoundary = uint8_t((style.backgroundBoundary & ~0x0C) | (uint8_t(boundary) << 2));
    }
    inline BackgroundBoundary GUIElementStyle_Get_backgroundimageboundary(const GUIElementStyle &style)
    {
        return static_cast<BackgroundBoundary>((style.backgroundBoundary & 0x0C) >> 2);
    }

    inline void GUIElementStyle_Set_backgroundimagealignx(GUIElementStyle &style, BackgroundAlign align)
    {
        style.backgroundImageAlignAndRepeat = (style.backgroundImageAlignAndRepeat & ~0x03) | (uint8_t(align) << 0);
    }
    inline BackgroundAlign GUIElementStyle_Get_backgroundimagealignx(const GUIElementStyle &style)
    {
        return static_cast<BackgroundAlign>((style.backgroundImageAlignAndRepeat & 0x03) >> 0);
    }

    inline void GUIElementStyle_Set_backgroundimagealigny(GUIElementStyle &style, BackgroundAlign align)
    {
        style.backgroundImageAlignAndRepeat = (style.backgroundImageAlignAndRepeat & ~0x0C) | (uint8_t(align) << 2);
    }
    inline BackgroundAlign GUIElementStyle_Get_backgroundimagealigny(const GUIElementStyle &style)
    {
        return static_cast<BackgroundAlign>((style.backgroundImageAlignAndRepeat & 0x0C) >> 2);
    }

    inline void GUIElementStyle_Set_backgroundimagerepeatx(GUIElementStyle &style, BackgroundRepeat repeat)
    {
        style.backgroundImageAlignAndRepeat = (style.backgroundImageAlignAndRepeat & ~0x30) | (uint8_t(repeat) << 4);
    }
    inline BackgroundRepeat GUIElementStyle_Get_backgroundimagerepeatx(const GUIElementStyle &style)
    {
        return static_cast<BackgroundRepeat>((style.backgroundImageAlignAndRepeat & 0x30) >> 4);
    }

    inline void GUIElementStyle_Set_backgroundimagerepeaty(GUIElementStyle &style, BackgroundRepeat repeat)
    {
        style.backgroundImageAlignAndRepeat = (style.backgroundImageAlignAndRepeat & ~0xC0) | (uint8_t(repeat) << 6);
    }
    inline BackgroundRepeat GUIElementStyle_Get_backgroundimagerepeaty(const GUIElementStyle &style)
    {
        return static_cast<BackgroundRepeat>((style.backgroundImageAlignAndRepeat & 0xC0) >> 6);
    }

    inline void GUIElementStyle_Set_backgroundimagescalex(GUIElementStyle &style, float scale)
    {
        style.backgroundImageScaleX = *reinterpret_cast<uint32_t*>(&scale);
    }
    inline float GUIElementStyle_Get_backgroundimagescalex(const GUIElementStyle &style)
    {
        return *reinterpret_cast<const float*>(&style.backgroundImageScaleX);
    }

    inline void GUIElementStyle_Set_backgroundimagescaley(GUIElementStyle &style, float scale)
    {
        style.backgroundImageScaleY = *reinterpret_cast<uint32_t*>(&scale);
    }
    inline float GUIElementStyle_Get_backgroundimagescaley(const GUIElementStyle &style)
    {
        return *reinterpret_cast<const float*>(&style.backgroundImageScaleY);
    }



    // Border width.
    inline void GUIElementStyle_Set_borderleftwidth(GUIElementStyle &style, uint32_t width, uint32_t type = NumberType_Absolute)
    {
        style.borderLeftWidth = type | width;
    }
    inline uint32_t GUIElementStyle_Get_borderleftwidth(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderLeftWidth & 0xF0000000;
        return style.borderLeftWidth & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_bordertopwidth(GUIElementStyle &style, uint32_t width, uint32_t type = NumberType_Absolute)
    {
        style.borderTopWidth = type | width;
    }
    inline uint32_t GUIElementStyle_Get_bordertopwidth(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderTopWidth & 0xF0000000;
        return style.borderTopWidth & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_borderrightwidth(GUIElementStyle &style, uint32_t width, uint32_t type = NumberType_Absolute)
    {
        style.borderRightWidth = type | width;
    }
    inline uint32_t GUIElementStyle_Get_borderrightwidth(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderRightWidth & 0xF0000000;
        return style.borderRightWidth & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_borderbottomwidth(GUIElementStyle &style, uint32_t width, uint32_t type = NumberType_Absolute)
    {
        style.borderBottomWidth = type | width;
    }
    inline uint32_t GUIElementStyle_Get_borderbottomwidth(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderBottomWidth & 0xF0000000;
        return style.borderBottomWidth & 0x0FFFFFFF;
    }

    // Border colour.
    inline void GUIElementStyle_Set_borderleftcolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderLeftColour = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_borderleftcolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.borderLeftColour & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.borderLeftColour & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.borderLeftColour & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.borderLeftColour & 0x000000FF) >>  0) / 255.0f;

        return color;
    }

    inline void GUIElementStyle_Set_bordertopcolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderTopColour = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_bordertopcolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.borderTopColour & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.borderTopColour & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.borderTopColour & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.borderTopColour & 0x000000FF) >>  0) / 255.0f;

        return color;
    }

    inline void GUIElementStyle_Set_borderrightcolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderRightColour = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_borderrightcolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.borderRightColour & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.borderRightColour & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.borderRightColour & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.borderRightColour & 0x000000FF) >>  0) / 255.0f;

        return color;
    }

    inline void GUIElementStyle_Set_borderbottomcolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderBottomColour = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_borderbottomcolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.borderBottomColour & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.borderBottomColour & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.borderBottomColour & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.borderBottomColour & 0x000000FF) >>  0) / 255.0f;

        return color;
    }


    inline void GUIElementStyle_Set_borderleftmaskoffset(GUIElementStyle &style, uint32_t maskOffset, uint32_t type = NumberType_Absolute)
    {
        style.borderLeftMaskOffset = type | maskOffset;
    }
    inline uint32_t GUIElementStyle_Get_borderleftmaskoffset(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderLeftMaskOffset & 0xF0000000;
        return style.borderLeftMaskOffset & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_bordertopmaskoffset(GUIElementStyle &style, uint32_t maskOffset, uint32_t type = NumberType_Absolute)
    {
        style.borderTopMaskOffset = type | maskOffset;
    }
    inline uint32_t GUIElementStyle_Get_bordertopmaskoffset(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderTopMaskOffset & 0xF0000000;
        return style.borderTopMaskOffset & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_borderrightmaskoffset(GUIElementStyle &style, uint32_t maskOffset, uint32_t type = NumberType_Absolute)
    {
        style.borderRightMaskOffset = type | maskOffset;
    }
    inline uint32_t GUIElementStyle_Get_borderrightmaskoffset(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderRightMaskOffset & 0xF0000000;
        return style.borderRightMaskOffset & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_borderbottommaskoffset(GUIElementStyle &style, uint32_t maskOffset, uint32_t type = NumberType_Absolute)
    {
        style.borderBottomMaskOffset = type | maskOffset;
    }
    inline uint32_t GUIElementStyle_Get_borderbottommaskoffset(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderBottomMaskOffset & 0xF0000000;
        return style.borderBottomMaskOffset & 0x0FFFFFFF;
    }


    inline void GUIElementStyle_Set_borderleftmasklength(GUIElementStyle &style, uint32_t maskLength, uint32_t type = NumberType_Absolute)
    {
        style.borderLeftMaskLength = type | maskLength;
    }
    inline uint32_t GUIElementStyle_Get_borderleftmasklength(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderLeftMaskLength & 0xF0000000;
        return style.borderLeftMaskLength & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_bordertopmasklength(GUIElementStyle &style, uint32_t maskLength, uint32_t type = NumberType_Absolute)
    {
        style.borderTopMaskLength = type | maskLength;
    }
    inline uint32_t GUIElementStyle_Get_bordertopmasklength(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderTopMaskLength & 0xF0000000;
        return style.borderTopMaskLength & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_borderrightmasklength(GUIElementStyle &style, uint32_t maskLength, uint32_t type = NumberType_Absolute)
    {
        style.borderRightMaskLength = type | maskLength;
    }
    inline uint32_t GUIElementStyle_Get_borderrightmasklength(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderRightMaskLength & 0xF0000000;
        return style.borderRightMaskLength & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_borderbottommasklength(GUIElementStyle &style, uint32_t maskLength, uint32_t type = NumberType_Absolute)
    {
        style.borderBottomMaskLength = type | maskLength;
    }
    inline uint32_t GUIElementStyle_Get_borderbottommasklength(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.borderBottomMaskLength & 0xF0000000;
        return style.borderBottomMaskLength & 0x0FFFFFFF;
    }



    inline void GUIElementStyle_Set_borderleftmaskcolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderLeftMaskColor = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_borderleftmaskcolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.borderLeftMaskColor & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.borderLeftMaskColor & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.borderLeftMaskColor & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.borderLeftMaskColor & 0x000000FF) >>  0) / 255.0f;

        return color;
    }

    inline void GUIElementStyle_Set_bordertopmaskcolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderTopMaskColor = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_bordertopmaskcolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.borderTopMaskColor & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.borderTopMaskColor & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.borderTopMaskColor & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.borderTopMaskColor & 0x000000FF) >>  0) / 255.0f;

        return color;
    }

    inline void GUIElementStyle_Set_borderrightmaskcolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderRightMaskColor = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_borderrightmaskcolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.borderRightMaskColor & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.borderRightMaskColor & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.borderRightMaskColor & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.borderRightMaskColor & 0x000000FF) >>  0) / 255.0f;

        return color;
    }

    inline void GUIElementStyle_Set_borderbottommaskcolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderBottomMaskColor = value;
    }
    inline GTLib::Colour GUIElementStyle_Get_borderbottommaskcolor(const GUIElementStyle &style)
    {
        GTLib::Colour color;
        color.r = static_cast<float>((style.borderBottomMaskColor & 0xFF000000) >> 24) / 255.0f;
        color.g = static_cast<float>((style.borderBottomMaskColor & 0x00FF0000) >> 16) / 255.0f;
        color.b = static_cast<float>((style.borderBottomMaskColor & 0x0000FF00) >>  8) / 255.0f;
        color.a = static_cast<float>((style.borderBottomMaskColor & 0x000000FF) >>  0) / 255.0f;

        return color;
    }



    // Padding.
    inline void GUIElementStyle_Set_paddingleft(GUIElementStyle &style, uint32_t padding, uint32_t type = NumberType_Absolute)
    {
        style.paddingLeft = type | padding;
    }
    inline uint32_t GUIElementStyle_Get_paddingleft(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.paddingLeft & 0xF0000000;
        return style.paddingLeft & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_paddingtop(GUIElementStyle &style, uint32_t padding, uint32_t type = NumberType_Absolute)
    {
        style.paddingTop = type | padding;
    }
    inline uint32_t GUIElementStyle_Get_paddingtop(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.paddingTop & 0xF0000000;
        return style.paddingTop & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_paddingright(GUIElementStyle &style, uint32_t padding, uint32_t type = NumberType_Absolute)
    {
        style.paddingRight = type | padding;
    }
    inline uint32_t GUIElementStyle_Get_paddingright(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.paddingRight & 0xF0000000;
        return style.paddingRight & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_paddingbottom(GUIElementStyle &style, uint32_t padding, uint32_t type = NumberType_Absolute)
    {
        style.paddingBottom = type | padding;
    }
    inline uint32_t GUIElementStyle_Get_paddingbottom(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.paddingBottom & 0xF0000000;
        return style.paddingBottom & 0x0FFFFFFF;
    }


    // Margin.
    inline void GUIElementStyle_Set_marginleft(GUIElementStyle &style, uint32_t margin, uint32_t type = NumberType_Absolute)
    {
        style.marginLeft = type | margin;
    }
    inline uint32_t GUIElementStyle_Get_marginleft(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.marginLeft & 0xF0000000;
        return style.marginLeft & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_margintop(GUIElementStyle &style, uint32_t margin, uint32_t type = NumberType_Absolute)
    {
        style.marginTop = type | margin;
    }
    inline uint32_t GUIElementStyle_Get_margintop(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.marginTop & 0xF0000000;
        return style.marginTop & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_marginright(GUIElementStyle &style, uint32_t margin, uint32_t type = NumberType_Absolute)
    {
        style.marginRight = type | margin;
    }
    inline uint32_t GUIElementStyle_Get_marginright(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.marginRight & 0xF0000000;
        return style.marginRight & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_marginbottom(GUIElementStyle &style, uint32_t margin, uint32_t type = NumberType_Absolute)
    {
        style.marginBottom = type | margin;
    }
    inline uint32_t GUIElementStyle_Get_marginbottom(const GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.marginBottom & 0xF0000000;
        return style.marginBottom & 0x0FFFFFFF;
    }



    // Font.
    inline void GUIElementStyle_Set_fontfamily(GUIElementStyle &style, uint32_t fontFamily)
    {
        style.fontFamily = fontFamily;
    }
    inline uint32_t GUIElementStyle_Get_fontfamily(const GUIElementStyle &style)
    {
        return style.fontFamily;
    }

    inline void GUIElementStyle_Set_fontsize(GUIElementStyle &style, uint32_t size, uint32_t type = NumberType_Pixels)
    {
        style.fontSize = type | size;
    }
    inline uint32_t GUIElementStyle_Get_fontsize(GUIElementStyle &style, uint32_t &typeOut)
    {
        typeOut = style.fontSize & 0xF0000000;
        return style.fontSize & 0x0FFFFFFF;
    }

    inline void GUIElementStyle_Set_fontweight(GUIElementStyle &style, FontWeight weight)
    {
        style.fontStyle = (style.fontStyle & ~0x0F) | (static_cast<uint32_t>(weight) << 0);
    }
    inline FontWeight GUIElementStyle_Get_fontweight(GUIElementStyle &style)
    {
        return static_cast<FontWeight>((style.fontStyle & 0x0F) >> 0);
    }

    inline void GUIElementStyle_Set_fontslant(GUIElementStyle &style, FontSlant slant)
    {
        style.fontStyle = (style.fontStyle & ~0x30) | (static_cast<uint32_t>(slant) << 4);
    }
    inline FontSlant GUIElementStyle_Get_fontslant(GUIElementStyle &style)
    {
        return static_cast<FontSlant>((style.fontStyle & 0x30) >> 4);
    }


    // Text.
    inline void GUIElementStyle_Set_textcolor(GUIElementStyle &style, const GTLib::Colour &colour)
    {
        style.textColorR = static_cast<uint8_t>(colour.r * 255);
        style.textColorG = static_cast<uint8_t>(colour.g * 255);
        style.textColorB = static_cast<uint8_t>(colour.b * 255);
        style.textColorA = static_cast<uint8_t>(colour.a * 255);
    }
    inline GTLib::Colour GUIElementStyle_Get_textcolor(const GUIElementStyle &style)
    {
        GTLib::Colour colour;
        colour.r = style.textColorR / 255.0f;
        colour.g = style.textColorG / 255.0f;
        colour.b = style.textColorB / 255.0f;
        colour.a = style.textColorA / 255.0f;

        return colour;
    }




    ///////////////////////////////////////////
    // Booleans
    //
    // Bits here are set using the superscalar optimized version of the bit hack found here: https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching

    // Visible.
    inline void GUIElementStyle_Set_visible(GUIElementStyle &style, bool visible)
    {
        style.booleanField = (style.booleanField & ~StyleBit_Invisible) | (-static_cast<int32_t>(!visible) & StyleBit_Invisible);
    }
    inline bool GUIElementStyle_Get_visible(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_Invisible) == 0;
    }

    // Flex children width.
    inline void GUIElementStyle_Set_flexchildrenwidth(GUIElementStyle &style, bool flex)
    {
        style.booleanField = (style.booleanField & ~StyleBit_FlexChildrenWidth) | (-static_cast<int32_t>(flex) & StyleBit_FlexChildrenWidth);
    }
    inline bool GUIElementStyle_Get_flexchildrenwidth(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_FlexChildrenWidth) != 0;
    }

    // Flex children height.
    inline void GUIElementStyle_Set_flexchildrenheight(GUIElementStyle &style, bool flex)
    {
        style.booleanField = (style.booleanField & ~StyleBit_FlexChildrenHeight) | (-static_cast<int32_t>(flex) & StyleBit_FlexChildrenHeight);
    }
    inline bool GUIElementStyle_Get_flexchildrenheight(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_FlexChildrenHeight) != 0;
    }

    // Left has priority.
    inline void GUIElementStyle_Set_lefthaspriority(GUIElementStyle &style, bool hasPriority)
    {
        style.booleanField = (style.booleanField & ~StyleBit_RightHasPriority) | (-static_cast<int32_t>(!hasPriority) & StyleBit_RightHasPriority);
    }
    inline bool GUIElementStyle_Get_lefthaspriority(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_RightHasPriority) == 0;
    }

    // Right has priority.
    inline void GUIElementStyle_Set_righthaspriority(GUIElementStyle &style, bool hasPriority)
    {
        style.booleanField = (style.booleanField & ~StyleBit_RightHasPriority) | (-static_cast<int32_t>(hasPriority) & StyleBit_RightHasPriority);
    }
    inline bool GUIElementStyle_Get_righthaspriority(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_RightHasPriority) != 0;
    }

    // Top has priority.
    inline void GUIElementStyle_Set_tophaspriority(GUIElementStyle &style, bool hasPriority)
    {
        style.booleanField = (style.booleanField & ~StyleBit_BottomHasPriority) | (-static_cast<int32_t>(!hasPriority) & StyleBit_BottomHasPriority);
    }
    inline bool GUIElementStyle_Get_tophaspriority(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_BottomHasPriority) == 0;
    }

    // Bottom has priority.
    inline void GUIElementStyle_Set_bottomhaspriority(GUIElementStyle &style, bool hasPriority)
    {
        style.booleanField = (style.booleanField & ~StyleBit_BottomHasPriority) | (-static_cast<int32_t>(hasPriority) & StyleBit_BottomHasPriority);
    }
    inline bool GUIElementStyle_Get_bottomhaspriority(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_BottomHasPriority) != 0;
    }

    // Cursor pass-through.
    inline void GUIElementStyle_Set_passthroughcursorinput(GUIElementStyle &style, bool passThroughCursorInput)
    {
        style.booleanField = (style.booleanField & ~StyleBit_PassThroughCursorInput) | (-static_cast<int32_t>(passThroughCursorInput) & StyleBit_PassThroughCursorInput);
    }
    inline bool GUIElementStyle_Get_passthroughcursorinput(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_PassThroughCursorInput) != 0;
    }

    // Font inheritance.
    inline void GUIElementStyle_Set_inheritfont(GUIElementStyle &style, bool inheritFont)
    {
        style.booleanField = (style.booleanField & ~StyleBit_InheritFont) | (-static_cast<int32_t>(inheritFont) & StyleBit_InheritFont);
    }
    inline bool GUIElementStyle_Get_inheritfont(const GUIElementStyle &style)
    {
        return (style.booleanField & StyleBit_InheritFont) != 0;
    }



    ///////////////////////////////////////////
    // Misc helpers.

    /// Retrieves the types of the width, min-width and max-width style properties.
    inline void GUIElementStyle_Get_allwidthtypes(GUIElementStyle &style, uint32_t &widthTypeOut, uint32_t &minWidthTypeOut, uint32_t &maxWidthTypeOut)
    {
        widthTypeOut    = style.width    & 0xFF000000;
        minWidthTypeOut = style.minWidth & 0xFF000000;
        maxWidthTypeOut = style.maxWidth & 0xFF000000;
    }

    /// Retrieves the types of the width, min-width and max-width style properties.
    inline void GUIElementStyle_Get_allheighttypes(GUIElementStyle &style, uint32_t &heightTypeOut, uint32_t &minHeightTypeOut, uint32_t &maxHeightTypeOut)
    {
        heightTypeOut    = style.height    & 0xFF000000;
        minHeightTypeOut = style.minHeight & 0xFF000000;
        maxHeightTypeOut = style.maxHeight & 0xFF000000;
    }

    /// Determines whether or not any of the width, min-width or max-width style properties are of the given type.
    inline bool GUIElementStyle_Get_IsAnyWidthOfType(GUIElementStyle &style, uint32_t type)
    {
        return
            (style.width    & 0xFF000000) == type ||
            (style.minWidth & 0xFF000000) == type ||
            (style.maxWidth & 0xFF000000) == type;
    }

    /// Determines whether or not any of the height, min-height or max-height style properties are of the given type.
    inline bool GUIElementStyle_Get_IsAnyHeightOfType(GUIElementStyle &style, uint32_t type)
    {
        return
            (style.height    & 0xFF000000) == type ||
            (style.minHeight & 0xFF000000) == type ||
            (style.maxHeight & 0xFF000000) == type;
    }


    /// Sets the size of every border.
    inline void GUIElementStyle_Set_borderwidth(GUIElementStyle &style, uint32_t width, uint32_t type = NumberType_Absolute)
    {
        uint32_t value = type | width;

        style.borderLeftWidth   = value;
        style.borderTopWidth    = value;
        style.borderRightWidth  = value;
        style.borderBottomWidth = value;
    }

    /// Sets the colour of every border.
    inline void GUIElementStyle_Set_bordercolor(GUIElementStyle &style, const GTLib::Colour &color)
    {
        uint32_t value =
            (static_cast<uint32_t>(static_cast<uint8_t>(color.r * 255)) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.g * 255)) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.b * 255)) <<  8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(color.a * 255)) <<  0);

        style.borderLeftColour   = value;
        style.borderTopColour    = value;
        style.borderRightColour  = value;
        style.borderBottomColour = value;
    }

    /// Sets the width and colour of every border.
    inline void GUIElementStyle_Set_border(GUIElementStyle &style, uint32_t width, uint32_t widthType, const GTLib::Colour &color)
    {
        GUIElementStyle_Set_borderwidth(style, width, widthType);
        GUIElementStyle_Set_bordercolor(style, color);
    }
    inline void GUIElementStyle_Set_border(GUIElementStyle &style, uint32_t width, const GTLib::Colour &color)
    {
        GUIElementStyle_Set_border(style, width, NumberType_Absolute, color);
    }

    /// Set the padding for all sides.
    inline void GUIElementStyle_Set_padding(GUIElementStyle &style, uint32_t padding, uint32_t type = NumberType_Absolute)
    {
        uint32_t value = type | padding;

        style.paddingLeft   = value;
        style.paddingTop    = value;
        style.paddingRight  = value;
        style.paddingBottom = value;
    }

    /// Set the margin for all sides.
    inline void GUIElementStyle_Set_margin(GUIElementStyle &style, uint32_t margin, uint32_t type = NumberType_Absolute)
    {
        uint32_t value = type | margin;

        style.marginLeft   = value;
        style.marginTop    = value;
        style.marginRight  = value;
        style.marginBottom = value;
    }
}

#endif
