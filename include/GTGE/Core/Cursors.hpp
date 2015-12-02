// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Cursors
#define GT_Cursors

#include <GTGE/Core/Strings/Equal.hpp>

namespace GT
{
    enum Cursor
    {
        Cursor_None = 0,

        Cursor_Arrow,
        Cursor_Beam,
        Cursor_Hand,
        Cursor_Cross,

        Cursor_VertDoubleArrow,
        Cursor_HorzDoubleArrow,

        Cursor_SizeArrowTop,
        Cursor_SizeArrowBottom,
        Cursor_SizeArrowLeft,
        Cursor_SizeArrowRight,
        Cursor_SizeArrowTopLeft,
        Cursor_SizeArrowTopRight,
        Cursor_SizeArrowBottomLeft,
        Cursor_SizeArrowBottomRight,

        Cursor_Count,       //  <-- Used for counting the number of cursor types.
    };

    /// Converts a style attribute cursor string identifier to a Cursor enumerator ID.
    inline Cursor StringToCursor(const char* str, ptrdiff_t strSizeInTs = -1)
    {
        if (Strings::Equal<false>(str, strSizeInTs, "arrow"))
        {
            return Cursor_Arrow;
        }

        if (Strings::Equal<false>(str, strSizeInTs, "beam"))
        {
            return Cursor_Beam;
        }

        if (Strings::Equal<false>(str, strSizeInTs, "hand"))
        {
            return Cursor_Hand;
        }

        if (Strings::Equal<false>(str, strSizeInTs, "cross"))
        {
            return Cursor_Cross;
        }


        if (Strings::Equal<false>(str, strSizeInTs, "vert-double-arrow"))
        {
            return Cursor_VertDoubleArrow;
        }

        if (Strings::Equal<false>(str, strSizeInTs, "horz-double-arrow"))
        {
            return Cursor_HorzDoubleArrow;
        }


        if (Strings::Equal<false>(str, strSizeInTs, "size-top"))
        {
            return Cursor_SizeArrowTop;
        }

        if (Strings::Equal<false>(str, strSizeInTs, "size-bottom"))
        {
            return Cursor_SizeArrowBottom;
        }

        if (Strings::Equal<false>(str, strSizeInTs, "size-left"))
        {
            return Cursor_SizeArrowLeft;
        }

        if (Strings::Equal<false>(str, strSizeInTs, "size-right"))
        {
            return Cursor_SizeArrowRight;
        }

        return Cursor_Arrow;
    }
}

#endif
