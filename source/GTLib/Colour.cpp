// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Colour.hpp>
#include <GTLib/Strings/Trim.hpp>
#include <GTLib/Parse.hpp>
#include <GTLib/stdio.hpp>  // for snprintf

namespace GTLib
{
    Colour Colour::Black(0.0f, 0.0f, 0.0f, 1.0f);
    Colour Colour::White(1.0f, 1.0f, 1.0f, 1.0f);
    Colour Colour::TransparentBlack(0.0f, 0.0f, 0.0f, 0.0f);
    Colour Colour::TransparentWhite(1.0f, 1.0f, 1.0f, 0.0f);
    Colour Colour::Red(1.0f, 0.0f, 0.0f, 1.0f);
    Colour Colour::Green(0.0f, 1.0f, 0.0f, 1.0f);
    Colour Colour::Blue(0.0f, 0.0f, 1.0f, 1.0f);
    Colour Colour::Alpha(0.0f, 0.0f, 0.0f, 1.0f);
    
    Colour Colour::DefaultBackground(0.16f, 0.164f, 0.20f, 1.0f);
    
    
    Colour::Colour(const char *value, ptrdiff_t valueSize)
        : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
    {
        TryParse(*this, value, valueSize);
    }
    
    bool Colour::TryParse(Colour &colour, const char *value, ptrdiff_t valueSize)
    {
        value     = GTLib::Strings::TrimStart(value, valueSize);
        valueSize = GTLib::Strings::TrimEnd(value, valueSize) - value;

        if (valueSize > 0)
        {
            if (*value == '#')
            {
                // Looking at a HTML colour.
                ++value;
                --valueSize;

                uint32_t r = 0;
                uint32_t g = 0;
                uint32_t b = 0;
                uint32_t a = 255;

                char rStr[3] = { '\0' };
                char gStr[3] = { '\0' };
                char bStr[3] = { '\0' };

                // valueSize must be 3 or 6. Anything else, and we can't convert.
                if (valueSize == 6)
                {
                    rStr[0] = value[0]; rStr[1] = value[1];
                    gStr[0] = value[2]; gStr[1] = value[3];
                    bStr[0] = value[4]; bStr[1] = value[5];
                }
                else if (valueSize == 3)
                {
                    rStr[0] = value[0]; rStr[1] = value[0];
                    gStr[0] = value[1]; gStr[1] = value[1];
                    bStr[0] = value[2]; bStr[1] = value[2];
                }
                else
                {
                    return false;
                }

                r = GTLib::Parse<uint32_t>(rStr, 16);
                g = GTLib::Parse<uint32_t>(gStr, 16);
                b = GTLib::Parse<uint32_t>(bStr, 16);

                colour.r = r / 255.0f;
                colour.g = g / 255.0f;
                colour.b = b / 255.0f;
                colour.a = a / 255.0f;
                
                return true;
            }
        }
        
        return false;
    }

    void Colour::ToString(char *dest, size_t destSize, const Colour &colour)
    {
        char r[3];
        GTLib::IO::snprintf(r, 3, "%.2x", static_cast<uint32_t>(colour.r * 255.0f));

        char g[3];
        GTLib::IO::snprintf(g, 3, "%.2x", static_cast<uint32_t>(colour.g * 255.0f));

        char b[3];
        GTLib::IO::snprintf(b, 3, "%.2x", static_cast<uint32_t>(colour.b * 255.0f));

        GTLib::IO::snprintf(dest, destSize, "#%s%s%s", r, g, b);
    }
}
