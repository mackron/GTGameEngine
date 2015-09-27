// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Colour
#define GT_Colour

#include <cstddef>

namespace GT
{
    class Color
    {
    public:

        Color()
            : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
        {
        }

        Color(float rIn, float gIn, float bIn, float aIn = 1.0f)
            : r(rIn), g(gIn), b(bIn), a(aIn)
        {
        }

        Color(const char *value, ptrdiff_t valueSizeInBytes = -1);


        inline       float* GetPointer()       { return &this->r; }
        inline const float* GetPointer() const { return &this->r; }

        inline       float & operator[] (int index)       { return (&this->r)[index]; }
        inline const float & operator[] (int index) const { return (&this->r)[index]; }

        inline bool operator==(const Color &other) const { return this->r == other.r && this->g == other.g && this->b == other.b && this->a == other.a; }
        inline bool operator!=(const Color &other) const { return !(*this == other); }

    public:

        /// The red, green, blue and alpha colour components.
        float r, g, b, a;


    public:

        static Color Black;
        static Color White;
        static Color TransparentBlack;
        static Color TransparentWhite;
        static Color Red;
        static Color Green;
        static Color Blue;
        static Color Alpha;

        static Color DefaultBackground;


    public:

        /// Tries to convert a string to a colour, returning true if successful.
        static bool TryParse(Color &colour, const char *value, ptrdiff_t valueSizeInBytes = -1);

        /// Converts a colour to a string. The returned string will be in the format #xxxxxx
        static void ToString(char *dest, size_t destSize, const Color &colour);
    };
}


#endif
