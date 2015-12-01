// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

namespace GT
{
    enum GUIFontWeight
    {
        GUIFontWeight_Default = 0,
        GUIFontWeight_Thin,
        GUIFontWeight_ExtraLight,
        GUIFontWeight_Light,
        GUIFontWeight_Medium,
        GUIFontWeight_SemiBold,
        GUIFontWeight_Bold,
        GUIFontWeight_ExtraBold,
        GUIFontWeight_Heavy,
        
        
        // Alias'
        GUIFontWeight_Regular    = GUIFontWeight_Default,
        GUIFontWeight_Normal     = GUIFontWeight_Default,
        GUIFontWeight_UltraLight = GUIFontWeight_ExtraLight,
        GUIFontWeight_DemiBold   = GUIFontWeight_SemiBold,
        GUIFontWeight_UltraBold  = GUIFontWeight_ExtraBold,
        GUIFontWeight_Black      = GUIFontWeight_Heavy,
    };
    
    enum GUIFontSlant
    {
        GUIFontSlant_Default = 0,
        GUIFontSlant_Italic,
    };
}
