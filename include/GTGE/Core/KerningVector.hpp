// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_KerningVector
#define GT_KerningVector

namespace GT
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

#endif // GT_KerningVector
