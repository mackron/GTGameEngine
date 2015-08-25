// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GTLib_Colour
#define GTLib_Colour

#include <cstddef>

namespace GTLib
{
    class Colour
    {
    public:

        Colour()
            : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
        {
        }

        Colour(float rIn, float gIn, float bIn, float aIn = 1.0f)
            : r(rIn), g(gIn), b(bIn), a(aIn)
        {
        }

        Colour(const char *value, ptrdiff_t valueSizeInBytes = -1);


        inline       float* GetPointer()       { return &this->r; }
        inline const float* GetPointer() const { return &this->r; }

        inline       float & operator[] (int index)       { return (&this->r)[index]; }
        inline const float & operator[] (int index) const { return (&this->r)[index]; }

        inline bool operator==(const Colour &other) const { return this->r == other.r && this->g == other.g && this->b == other.b && this->a == other.a; }
        inline bool operator!=(const Colour &other) const { return !(*this == other); }

    public:

        /// The red, green, blue and alpha colour components.
        float r, g, b, a;


    public:

        static Colour Black;
        static Colour White;
        static Colour TransparentBlack;
        static Colour TransparentWhite;
        static Colour Red;
        static Colour Green;
        static Colour Blue;
        static Colour Alpha;

        static Colour DefaultBackground;


    public:

        /// Tries to convert a string to a colour, returning true if successful.
        static bool TryParse(Colour &colour, const char *value, ptrdiff_t valueSizeInBytes = -1);

        /// Converts a colour to a string. The returned string will be in the format #xxxxxx
        static void ToString(char *dest, size_t destSize, const Colour &colour);
    };
}


#endif
