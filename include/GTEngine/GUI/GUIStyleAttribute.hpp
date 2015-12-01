// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIStyleAttribute
#define GT_GUIStyleAttribute

#include "GUIStyleNumber.hpp"
#include "GUIPositioning.hpp"
#include "GUIPlanes.hpp"
#include "GUIAlign.hpp"
#include "GUIFontStyles.hpp"
#include <GTEngine/Core/Colour.hpp>
#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/Parse.hpp>
#include <GTEngine/Core/stdio.hpp>
#include <GTEngine/Core/stdlib.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    class GUIStyleClass;

    /// Base class for style attributes.
    template <typename T>
    struct GUIStyleAttribute
    {
    public:

        /// Default constructor.
        GUIStyleAttribute()
            : value(), inherit(false), isset(false)
        {
        }

        /// Constructor.
        GUIStyleAttribute(const T &defaultValue)
            : value(defaultValue), inherit(false), isset(false)
        {
        }

        /// Destructor.
        virtual ~GUIStyleAttribute()
        {
        }




    public:

        /// The value of the item.
        T value;

        /// Keeps track of whether or not the style attribute is set to 'inherit'.
        bool inherit;

        /// Keeps track of whether or not the attribute is set.
        bool isset;
    };


    // Number
    struct GUIStyleAttribute_Number : public GUIStyleAttribute<float>
    {
        GUIStyleAttribute_Number()
            : GUIStyleAttribute(0.0f), hasSuffix(false), setFromInteger(false), format(GUIStyleNumberFormat_Pixels)
        {
        }

        inline bool Automatic() const { return this->format == GUIStyleNumberFormat_Automatic; }
        inline bool InPixels()  const { return this->format == GUIStyleNumberFormat_Pixels; }
        inline bool InPercent() const { return this->format == GUIStyleNumberFormat_Percent; }
        inline bool InPoints()  const { return this->format == GUIStyleNumberFormat_Points; }

        bool SetFromInteger(int valueIn)
        {
            return this->SetFromFloat(static_cast<float>(valueIn));
        }

        bool SetFromFloat(float valueIn)
        {
            this->value     = valueIn;
            this->inherit   = false;
            this->isset     = true;
            this->hasSuffix = false;
            this->format    = GUIStyleNumberFormat_Pixels;

            return true;
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            // In the event of an error, we will always set a default value.
            this->value  = 0;
            this->format = GUIStyleNumberFormat_Pixels;

            // We use this to keep track of whether or not the suffix should be appended to the end of the stringified value.
            this->hasSuffix = true;

            // We're going to work with our own null-terminated string.
            GT::String valueStr(valueIn, valueSizeInBytes);

            if (GT::Strings::Equal(valueIn, valueSizeInBytes, "auto"))
            {
                this->format = GUIStyleNumberFormat_Automatic;
            }
            else
            {
                // We're going to assume it's a number. We will convert to an integer, looking at the returned
                // values to determine the format. If it's an unknown format, we return false. The number extension
                // can be '%', 'px', 'pt' or nothing. In the case of nothing, we assume pixels. The value has to
                // be null terminated before we can convert.

                char* end = nullptr;
                this->value = static_cast<float>(std::strtod(valueStr.c_str(), &end));
                if (end[0] == '\0')
                {
                    this->format = GUIStyleNumberFormat_Pixels;
                    this->hasSuffix = false;
                }
                else if (end[0] == '%')
                {
                    this->format = GUIStyleNumberFormat_Percent;
                }
                else if (end[0] == 'p')
                {
                    // Can be either px or pt.
                    ++end;
                    if (end[0] != '\0')
                    {
                        if (end[0] == 'x')
                        {
                            this->format = GUIStyleNumberFormat_Pixels;
                        }
                        else if (end[0] == 't')
                        {
                            this->format = GUIStyleNumberFormat_Points;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }

            // Here we are going to check if we have a dot in the value string. If so, it means we were setting from a float. Otherwise, an integer.
            this->setFromInteger = true;
            for (size_t i = 0; i < valueStr.GetLengthInTs(); ++i)
            {
                if (valueStr.c_str()[i] == '.')
                {
                    this->setFromInteger = false;
                    break;
                }
            }

            this->inherit = false;
            this->isset   = true;
            return true;
        }

        GT::String GetValueAsString() const
        {
            char valueStr[16];

            if (this->format == GUIStyleNumberFormat_Automatic)
            {
                return "auto";
            }
            else
            {
                if (this->setFromInteger)
                {
                    if (this->hasSuffix)
                    {
                        switch (this->format)
                        {
                        case GUIStyleNumberFormat_Pixels:
                            {
                                GT::IO::snprintf(valueStr, 16, "%dpx", static_cast<int>(this->value));
                                break;
                            }

                        case GUIStyleNumberFormat_Points:
                            {
                                GT::IO::snprintf(valueStr, 16, "%dpt", static_cast<int>(this->value));
                                break;
                            }

                        case GUIStyleNumberFormat_Percent:
                            {
                                GT::IO::snprintf(valueStr, 16, "%d%%", static_cast<int>(this->value));
                                break;
                            }

                        case GUIStyleNumberFormat_Automatic:
                        default: break;
                        }
                    }
                    else
                    {
                        GT::IO::snprintf(valueStr, 16, "%d", static_cast<int>(this->value));
                    }
                }
                else
                {
                    if (this->hasSuffix)
                    {
                        switch (this->format)
                        {
                        case GUIStyleNumberFormat_Pixels:
                            {
                                GT::IO::snprintf(valueStr, 16, "%fpx", this->value);
                                break;
                            }

                        case GUIStyleNumberFormat_Points:
                            {
                                GT::IO::snprintf(valueStr, 16, "%fpt", this->value);
                                break;
                            }

                        case GUIStyleNumberFormat_Percent:
                            {
                                GT::IO::snprintf(valueStr, 16, "%f%%", this->value);
                                break;
                            }

                        case GUIStyleNumberFormat_Automatic:
                        default: break;
                        }
                    }
                    else
                    {
                        GT::IO::snprintf(valueStr, 16, "%f", this->value);
                    }
                }
            }

            return valueStr;
        }


        GUIStyleAttribute_Number & operator= (const char* valueIn)
        {
            this->SetFromString(valueIn);
            return *this;
        }

        GUIStyleAttribute_Number & operator= (int valueIn)
        {
            this->SetFromInteger(valueIn);
            return *this;
        }



    public:

        /// Whether or not the number was set with a suffix.
        bool hasSuffix;

        /// Whether or not the number was set from an integer.
        bool setFromInteger;

        /// The format of the number - pixels, percent, etc.
        GUIStyleNumberFormat format;
    };


    // GT::String
    struct GUIStyleAttribute_String : public GUIStyleAttribute<GT::String>
    {
        GUIStyleAttribute_String()
            : GUIStyleAttribute()
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (valueIn)
            {
                this->value.Assign(valueIn, valueSizeInBytes);
                this->isset = true;

                return true;
            }

            return false;
        }

        // This is here for consistency.
        const GT::String & GetValueAsString() const
        {
            return this->value;
        }
    };


    // Boolean
    struct GUIStyleAttribute_Boolean : public GUIStyleAttribute<bool>
    {
        GUIStyleAttribute_Boolean()
            : GUIStyleAttribute(false)
        {
        }

        bool SetFromString(const char *valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GT::Strings::Equal(valueIn, valueSizeInBytes, "true"))
            {
                this->value = true;
                this->isset = true;

                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "false"))
            {
                this->value = false;
                this->isset = true;

                return true;
            }
            else
            {
                this->value = false;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            return (this->value) ? "true" : "false";
        }
    };


    // Color
    struct GUIStyleAttribute_Colour : public GUIStyleAttribute<GT::Colour>
    {
        GUIStyleAttribute_Colour()
            : GUIStyleAttribute(GT::Colour::TransparentBlack), isnone(true)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GT::Colour::TryParse(this->value, valueIn, valueSizeInBytes))
            {
                this->isset  = true;
                this->isnone = false;

                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "none", -1))
            {
                this->isset  = true;
                this->isnone = true;

                return true;
            }

            return false;
        }

        GT::String GetValueAsString() const
        {
            char valueStr[8];
            GT::Colour::ToString(valueStr, 8, this->value);

            return valueStr;
        }

        GUIStyleAttribute_Colour & operator= (const GT::Colour &valueIn)
        {
            this->value = valueIn;
            this->isset = true;

            return *this;
        }

    public:

        /// Whether or not the colour is set to 'none'.
        bool isnone;
    };


    // Positioning
    struct GUIStyleAttribute_Positioning : public GUIStyleAttribute<GUIPositioning>
    {
        GUIStyleAttribute_Positioning()
            : GUIStyleAttribute(GUIPositioning_Auto)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GT::Strings::Equal(valueIn, valueSizeInBytes, "relative"))
            {
                this->value = GUIPositioning_Relative;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "absolute"))
            {
                this->value = GUIPositioning_Absolute;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "auto"))
            {
                this->value = GUIPositioning_Auto;
                this->isset = true;
                return true;
            }
            else    // 'auto' by default.
            {
                this->value = GUIPositioning_Auto;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == GUIPositioning_Relative)
            {
                return "relative";
            }
            else if (this->value == GUIPositioning_Absolute)
            {
                return "absolute";
            }
            else    // 'auto' by default.
            {
                return "auto";
            }
        }
    };


    // Plane
    struct GUIStyleAttribute_Plane : public GUIStyleAttribute<GUIPlane>
    {
        GUIStyleAttribute_Plane()
            : GUIStyleAttribute(GUIPlane_Vertical)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GT::Strings::Equal(valueIn, valueSizeInBytes, "horizontal"))
            {
                this->value = GUIPlane_Horizontal;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "vertical"))
            {
                this->value = GUIPlane_Vertical;
                this->isset = true;
                return true;
            }
            else    // 'vertical' by default.
            {
                this->value = GUIPlane_Vertical;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == GUIPlane_Horizontal)
            {
                return "horizontal";
            }
            else    // 'vertical' by default.
            {
                return "vertical";
            }
        }
    };


    // Align
    struct GUIStyleAttribute_Align : public GUIStyleAttribute<GUIAlign>
    {
        GUIStyleAttribute_Align()
            : GUIStyleAttribute(GUIAlign_Center)  // <-- 'center' is used here since it can be used both horizontally and vertically.
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GT::Strings::Equal(valueIn, valueSizeInBytes, "left"))
            {
                this->value = GUIAlign_Left;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "right"))
            {
                this->value = GUIAlign_Right;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "center"))
            {
                this->value = GUIAlign_Center;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "top"))
            {
                this->value = GUIAlign_Top;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "bottom"))
            {
                this->value = GUIAlign_Bottom;
                this->isset = true;
                return true;
            }
            else
            {
                this->value = GUIAlign_Center;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == GUIAlign_Left)
            {
                return "left";
            }
            else if (this->value == GUIAlign_Right)
            {
                return "right";
            }
            else if (this->value == GUIAlign_Center)
            {
                return "center";
            }
            else if (this->value == GUIAlign_Top)
            {
                return "top";
            }
            else if (this->value == GUIAlign_Bottom)
            {
                return "bottom";
            }
            else
            {
                // Should never get here, but we'll return 'center' in this case which is a common value for alignments.
                return "center";
            }
        }
    };



    enum PositionOrigin
    {
        PositionOrigin_Inner,
        PositionOrigin_InnerBorder,
        PositionOrigin_Outer
    };

    // PositionOrigin
    struct GUIStyleAttribute_PositionOrigin : public GUIStyleAttribute<PositionOrigin>
    {
        GUIStyleAttribute_PositionOrigin()
            : GUIStyleAttribute(PositionOrigin_Inner)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GT::Strings::Equal(valueIn, valueSizeInBytes, "inner"))
            {
                this->value = PositionOrigin_Inner;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "inner-border"))
            {
                this->value = PositionOrigin_InnerBorder;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "outer"))
            {
                this->value = PositionOrigin_Outer;
                this->isset = true;
                return true;
            }
            else    // 'inner' by default.
            {
                this->value = PositionOrigin_Inner;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == PositionOrigin_InnerBorder)
            {
                return "inner-border";
            }
            else if (this->value == PositionOrigin_Outer)
            {
                return "outer";
            }
            else    // 'inner' by default.
            {
                return "inner";
            }
        }
    };
    
    
    // GUIFontWeight
    struct GUIStyleAttribute_FontWeight : public GUIStyleAttribute<GUIFontWeight>
    {
        GUIStyleAttribute_FontWeight()
            : GUIStyleAttribute(GUIFontWeight_Default)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            // We want to order this by most common in an attempt to avoid as many false string comparisons as we can.
            if (GT::Strings::Equal(valueIn, valueSizeInBytes, "default"))
            {
                this->value = GUIFontWeight_Default;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "bold"))
            {
                this->value = GUIFontWeight_Bold;
                this->isset = true;
                return true;
            }
            else
            {
                // These are the not-as-common styles. We'll just organize these by weight.
                if (GT::Strings::Equal(valueIn, valueSizeInBytes, "thin"))
                {
                    this->value = GUIFontWeight_Thin;
                    this->isset = true;
                    return true;
                }
                else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "extra-light"))
                {
                    this->value = GUIFontWeight_ExtraLight;
                    this->isset = true;
                    return true;
                }
                else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "light"))
                {
                    this->value = GUIFontWeight_Light;
                    this->isset = true;
                    return true;
                }
                else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "medium"))
                {
                    this->value = GUIFontWeight_Medium;
                    this->isset = true;
                    return true;
                }
                else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "semi-bold"))
                {
                    this->value = GUIFontWeight_SemiBold;
                    this->isset = true;
                    return true;
                }
                else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "extra-bold"))
                {
                    this->value = GUIFontWeight_ExtraBold;
                    this->isset = true;
                    return true;
                }
                else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "heavy"))
                {
                    this->value = GUIFontWeight_Heavy;
                    this->isset = true;
                    return true;
                }
                else    // 'default' by default.
                {
                    this->value = GUIFontWeight_Default;
                    return false;
                }
            }
        }

        const char* GetValueAsString() const
        {
            switch (this->value)
            {
            case GUIFontWeight_Thin:
                {
                    return "thin";
                }
                
            case GUIFontWeight_ExtraLight:
                {
                    return "extra-light";
                }
                
            case GUIFontWeight_Light:
                {
                    return "light";
                }
                
            case GUIFontWeight_Default:
                {
                    return "default";
                }
                
            case GUIFontWeight_Medium:
                {
                    return "medium";
                }
                
            case GUIFontWeight_SemiBold:
                {
                    return "semi-bold";
                }
                
            case GUIFontWeight_Bold:
                {
                    return "bold";
                }
                
            case GUIFontWeight_ExtraBold:
                {
                    return "extra-bold";
                }
                
            case GUIFontWeight_Heavy:
                {
                    return "heavy";
                }
                
                
            default:
                {
                    return "default";
                }
            }
        }
    };
    
    
    // GUIFontSlant
    struct GUIStyleAttribute_FontSlant : public GUIStyleAttribute<GUIFontSlant>
    {
        GUIStyleAttribute_FontSlant()
            : GUIStyleAttribute(GUIFontSlant_Default)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GT::Strings::Equal(valueIn, valueSizeInBytes, "default"))
            {
                this->value = GUIFontSlant_Default;
                this->isset = true;
                return true;
            }
            else if (GT::Strings::Equal(valueIn, valueSizeInBytes, "italic"))
            {
                this->value = GUIFontSlant_Italic;
                this->isset = true;
                return true;
            }
            else    // 'default' by default.
            {
                this->value = GUIFontSlant_Default;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == GUIFontSlant_Italic)
            {
                return "italic";
            }
            else    // 'default' by default.
            {
                return "default";
            }
        }
    };


    // This is a special style attribute type for modifier classes (pushed, hovered, etc).
    struct GUIStyleAttribute_StyleClass
    {
        GUIStyleAttribute_StyleClass()
            : isset(false), value(nullptr)
        {
        }

        GUIStyleAttribute_StyleClass & operator=(GUIStyleClass* valueIn)
        {
            this->value = valueIn;
            this->isset = true;

            return *this;
        }

    public:

        bool        isset;
        GUIStyleClass* value;
    };
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
