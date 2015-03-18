// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_StyleNumber_hpp_
#define __GTLib_GUI_StyleNumber_hpp_

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
