// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIStyleNumber
#define GT_GUIStyleNumber

namespace GT
{
    enum GUIStyleNumberFormat
    {
        GUIStyleNumberFormat_Pixels = 0,   // The default format.
        GUIStyleNumberFormat_Percent,
        GUIStyleNumberFormat_Points,       // For fonts sizes.
        GUIStyleNumberFormat_Automatic     // Automatically determine the value. This has different effects in different context's.
    };
}

#endif
