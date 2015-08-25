// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GTLib_FontMetrics
#define GTLib_FontMetrics

namespace GTLib
{
    /// Structure containing the relevant metrics of the given font.
    struct FontMetrics
    {
        FontMetrics()
            : ascent(0), descent(0), lineHeight(0)
        {
        }


        /// The space between the base line and the top.
        int ascent;

        /// The space between the base line and the bottom.
        int descent;

        /// The height of the line.
        unsigned int lineHeight;
    };
}

#endif // __GTLib_FontMetrics_hpp_
