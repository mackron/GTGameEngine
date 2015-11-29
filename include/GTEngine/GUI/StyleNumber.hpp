// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_StyleNumber
#define GT_GUI_StyleNumber

namespace GTGUI
{
    enum StyleNumberFormat
    {
        StyleNumberFormat_Pixels = 0,   // The default format.
        StyleNumberFormat_Percent,
        StyleNumberFormat_Points,       // For fonts sizes.
        StyleNumberFormat_Automatic     // Automatically determine the value. This has different effects in different context's.
    };
}

#endif
