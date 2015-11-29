// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_StyleAttribute_hpp_
#define __GTLib_GUI_StyleAttribute_hpp_

#include "StyleNumber.hpp"
#include "Positioning.hpp"
#include "Planes.hpp"
#include "Align.hpp"
#include "FontStyles.hpp"
#include <GTEngine/Core/Colour.hpp>
#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/Parse.hpp>
#include <GTEngine/Core/stdio.hpp>
#include <GTEngine/Core/stdlib.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GTGUI
{
    class StyleClass;

    /// Base class for style attributes.
    template <typename T>
    struct StyleAttribute
    {
    public:

        /// Default constructor.
        StyleAttribute()
            : value(), inherit(false), isset(false)
        {
        }

        /// Constructor.
        StyleAttribute(const T &defaultValue)
            : value(defaultValue), inherit(false), isset(false)
        {
        }

        /// Destructor.
        virtual ~StyleAttribute()
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
    struct StyleAttribute_Number : public StyleAttribute<float>
    {
        StyleAttribute_Number()
            : StyleAttribute(0.0f), hasSuffix(false), setFromInteger(false), format(StyleNumberFormat_Pixels)
        {
        }

        inline bool Automatic() const { return this->format == StyleNumberFormat_Automatic; }
        inline bool InPixels()  const { return this->format == StyleNumberFormat_Pixels; }
        inline bool InPercent() const { return this->format == StyleNumberFormat_Percent; }
        inline bool InPoints()  const { return this->format == StyleNumberFormat_Points; }

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
            this->format    = StyleNumberFormat_Pixels;

            return true;
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            // In the event of an error, we will always set a default value.
            this->value  = 0;
            this->format = StyleNumberFormat_Pixels;

            // We use this to keep track of whether or not the suffix should be appended to the end of the stringified value.
            this->hasSuffix = true;

            // We're going to work with our own null-terminated string.
            GTLib::String valueStr(valueIn, valueSizeInBytes);

            if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "auto"))
            {
                this->format = StyleNumberFormat_Automatic;
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
                    this->format = StyleNumberFormat_Pixels;
                    this->hasSuffix = false;
                }
                else if (end[0] == '%')
                {
                    this->format = StyleNumberFormat_Percent;
                }
                else if (end[0] == 'p')
                {
                    // Can be either px or pt.
                    ++end;
                    if (end[0] != '\0')
                    {
                        if (end[0] == 'x')
                        {
                            this->format = StyleNumberFormat_Pixels;
                        }
                        else if (end[0] == 't')
                        {
                            this->format = StyleNumberFormat_Points;
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

        GTLib::String GetValueAsString() const
        {
            char valueStr[16];

            if (this->format == StyleNumberFormat_Automatic)
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
                        case StyleNumberFormat_Pixels:
                            {
                                GTLib::IO::snprintf(valueStr, 16, "%dpx", static_cast<int>(this->value));
                                break;
                            }

                        case StyleNumberFormat_Points:
                            {
                                GTLib::IO::snprintf(valueStr, 16, "%dpt", static_cast<int>(this->value));
                                break;
                            }

                        case StyleNumberFormat_Percent:
                            {
                                GTLib::IO::snprintf(valueStr, 16, "%d%%", static_cast<int>(this->value));
                                break;
                            }

                        case StyleNumberFormat_Automatic:
                        default: break;
                        }
                    }
                    else
                    {
                        GTLib::IO::snprintf(valueStr, 16, "%d", static_cast<int>(this->value));
                    }
                }
                else
                {
                    if (this->hasSuffix)
                    {
                        switch (this->format)
                        {
                        case StyleNumberFormat_Pixels:
                            {
                                GTLib::IO::snprintf(valueStr, 16, "%fpx", this->value);
                                break;
                            }

                        case StyleNumberFormat_Points:
                            {
                                GTLib::IO::snprintf(valueStr, 16, "%fpt", this->value);
                                break;
                            }

                        case StyleNumberFormat_Percent:
                            {
                                GTLib::IO::snprintf(valueStr, 16, "%f%%", this->value);
                                break;
                            }

                        case StyleNumberFormat_Automatic:
                        default: break;
                        }
                    }
                    else
                    {
                        GTLib::IO::snprintf(valueStr, 16, "%f", this->value);
                    }
                }
            }

            return valueStr;
        }


        StyleAttribute_Number & operator= (const char* valueIn)
        {
            this->SetFromString(valueIn);
            return *this;
        }

        StyleAttribute_Number & operator= (int valueIn)
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
        StyleNumberFormat format;
    };


    // String
    struct StyleAttribute_String : public StyleAttribute<GTLib::String>
    {
        StyleAttribute_String()
            : StyleAttribute()
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
        const GTLib::String & GetValueAsString() const
        {
            return this->value;
        }
    };


    // Boolean
    struct StyleAttribute_Boolean : public StyleAttribute<bool>
    {
        StyleAttribute_Boolean()
            : StyleAttribute(false)
        {
        }

        bool SetFromString(const char *valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "true"))
            {
                this->value = true;
                this->isset = true;

                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "false"))
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
    struct StyleAttribute_Colour : public StyleAttribute<GTLib::Colour>
    {
        StyleAttribute_Colour()
            : StyleAttribute(GTLib::Colour::TransparentBlack), isnone(true)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GTLib::Colour::TryParse(this->value, valueIn, valueSizeInBytes))
            {
                this->isset  = true;
                this->isnone = false;

                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "none", -1))
            {
                this->isset  = true;
                this->isnone = true;

                return true;
            }

            return false;
        }

        GTLib::String GetValueAsString() const
        {
            char valueStr[8];
            GTLib::Colour::ToString(valueStr, 8, this->value);

            return valueStr;
        }

        StyleAttribute_Colour & operator= (const GTLib::Colour &valueIn)
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
    struct StyleAttribute_Positioning : public StyleAttribute<Positioning>
    {
        StyleAttribute_Positioning()
            : StyleAttribute(Positioning_Auto)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "relative"))
            {
                this->value = GTGUI::Positioning_Relative;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "absolute"))
            {
                this->value = GTGUI::Positioning_Absolute;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "auto"))
            {
                this->value = GTGUI::Positioning_Auto;
                this->isset = true;
                return true;
            }
            else    // 'auto' by default.
            {
                this->value = GTGUI::Positioning_Auto;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == GTGUI::Positioning_Relative)
            {
                return "relative";
            }
            else if (this->value == GTGUI::Positioning_Absolute)
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
    struct StyleAttribute_Plane : public StyleAttribute<Plane>
    {
        StyleAttribute_Plane()
            : StyleAttribute(Plane_Vertical)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "horizontal"))
            {
                this->value = Plane_Horizontal;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "vertical"))
            {
                this->value = Plane_Vertical;
                this->isset = true;
                return true;
            }
            else    // 'vertical' by default.
            {
                this->value = Plane_Vertical;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == Plane_Horizontal)
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
    struct StyleAttribute_Align : public StyleAttribute<Align>
    {
        StyleAttribute_Align()
            : StyleAttribute(Align_Center)  // <-- 'center' is used here since it can be used both horizontally and vertically.
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "left"))
            {
                this->value = Align_Left;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "right"))
            {
                this->value = Align_Right;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "center"))
            {
                this->value = Align_Center;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "top"))
            {
                this->value = Align_Top;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "bottom"))
            {
                this->value = Align_Bottom;
                this->isset = true;
                return true;
            }
            else
            {
                this->value = Align_Center;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == Align_Left)
            {
                return "left";
            }
            else if (this->value == Align_Right)
            {
                return "right";
            }
            else if (this->value == Align_Center)
            {
                return "center";
            }
            else if (this->value == Align_Top)
            {
                return "top";
            }
            else if (this->value == Align_Bottom)
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
    struct StyleAttribute_PositionOrigin : public StyleAttribute<PositionOrigin>
    {
        StyleAttribute_PositionOrigin()
            : StyleAttribute(PositionOrigin_Inner)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "inner"))
            {
                this->value = PositionOrigin_Inner;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "inner-border"))
            {
                this->value = PositionOrigin_InnerBorder;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "outer"))
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
    
    
    // FontWeight
    struct StyleAttribute_FontWeight : public StyleAttribute<FontWeight>
    {
        StyleAttribute_FontWeight()
            : StyleAttribute(FontWeight_Default)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            // We want to order this by most common in an attempt to avoid as many false string comparisons as we can.
            if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "default"))
            {
                this->value = FontWeight_Default;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "bold"))
            {
                this->value = FontWeight_Bold;
                this->isset = true;
                return true;
            }
            else
            {
                // These are the not-as-common styles. We'll just organize these by weight.
                if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "thin"))
                {
                    this->value = FontWeight_Thin;
                    this->isset = true;
                    return true;
                }
                else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "extra-light"))
                {
                    this->value = FontWeight_ExtraLight;
                    this->isset = true;
                    return true;
                }
                else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "light"))
                {
                    this->value = FontWeight_Light;
                    this->isset = true;
                    return true;
                }
                else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "medium"))
                {
                    this->value = FontWeight_Medium;
                    this->isset = true;
                    return true;
                }
                else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "semi-bold"))
                {
                    this->value = FontWeight_SemiBold;
                    this->isset = true;
                    return true;
                }
                else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "extra-bold"))
                {
                    this->value = FontWeight_ExtraBold;
                    this->isset = true;
                    return true;
                }
                else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "heavy"))
                {
                    this->value = FontWeight_Heavy;
                    this->isset = true;
                    return true;
                }
                else    // 'default' by default.
                {
                    this->value = FontWeight_Default;
                    return false;
                }
            }
        }

        const char* GetValueAsString() const
        {
            switch (this->value)
            {
            case FontWeight_Thin:
                {
                    return "thin";
                }
                
            case FontWeight_ExtraLight:
                {
                    return "extra-light";
                }
                
            case FontWeight_Light:
                {
                    return "light";
                }
                
            case FontWeight_Default:
                {
                    return "default";
                }
                
            case FontWeight_Medium:
                {
                    return "medium";
                }
                
            case FontWeight_SemiBold:
                {
                    return "semi-bold";
                }
                
            case FontWeight_Bold:
                {
                    return "bold";
                }
                
            case FontWeight_ExtraBold:
                {
                    return "extra-bold";
                }
                
            case FontWeight_Heavy:
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
    
    
    // FontSlant
    struct StyleAttribute_FontSlant : public StyleAttribute<FontSlant>
    {
        StyleAttribute_FontSlant()
            : StyleAttribute(FontSlant_Default)
        {
        }

        bool SetFromString(const char* valueIn, ptrdiff_t valueSizeInBytes = -1)
        {
            if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "default"))
            {
                this->value = FontSlant_Default;
                this->isset = true;
                return true;
            }
            else if (GTLib::Strings::Equal(valueIn, valueSizeInBytes, "italic"))
            {
                this->value = FontSlant_Italic;
                this->isset = true;
                return true;
            }
            else    // 'default' by default.
            {
                this->value = FontSlant_Default;
                return false;
            }
        }

        const char* GetValueAsString() const
        {
            if (this->value == FontSlant_Italic)
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
    struct StyleAttribute_StyleClass
    {
        StyleAttribute_StyleClass()
            : isset(false), value(nullptr)
        {
        }

        StyleAttribute_StyleClass & operator=(StyleClass* valueIn)
        {
            this->value = valueIn;
            this->isset = true;

            return *this;
        }

    public:

        bool        isset;
        StyleClass* value;
    };
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
