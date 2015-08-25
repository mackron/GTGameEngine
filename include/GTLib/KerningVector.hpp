// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GTLib_KerningVector
#define GTLib_KerningVector

namespace GTLib
{
    /// Stores the kerning information of two characters.
    struct KerningVector
    {
        KerningVector()
            : x(0), y(0)
        {
        }

        int x, y;
    };
}

#endif // __GTLib_KerningVector_hpp_
