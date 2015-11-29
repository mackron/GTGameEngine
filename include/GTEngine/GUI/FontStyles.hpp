// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

namespace GTGUI
{
    enum FontWeight
    {
        FontWeight_Default = 0,
        FontWeight_Thin,
        FontWeight_ExtraLight,
        FontWeight_Light,
        FontWeight_Medium,
        FontWeight_SemiBold,
        FontWeight_Bold,
        FontWeight_ExtraBold,
        FontWeight_Heavy,
        
        
        // Alias'
        FontWeight_Regular    = FontWeight_Default,
        FontWeight_Normal     = FontWeight_Default,
        FontWeight_UltraLight = FontWeight_ExtraLight,
        FontWeight_DemiBold   = FontWeight_SemiBold,
        FontWeight_UltraBold  = FontWeight_ExtraBold,
        FontWeight_Black      = FontWeight_Heavy,
    };
    
    enum FontSlant
    {
        FontSlant_Default = 0,
        FontSlant_Italic,
    };
}
