// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_FontInfo_hpp_
#define __GTLib_FontInfo_hpp_

#include "FontStyles.hpp"
#include "GlyphRenderingModes.hpp"
#include <GTLib/String.hpp>
#include <cstddef>

namespace GTLib
{
    /**
    *   \brief  Structure containing information about the font.
    *
    *   This structure is used in multiple parts of the library. Some attributes are not used in certain situations.
    */
    struct FontInfo
    {
        FontInfo()
            : family(), styleFlags(0), sizeInPoints(10.0f), sizeInPixels(0.0f),
              dpiX(96), dpiY(96)
        {
        }
        
        FontInfo(const FontInfo &info)
            : family(info.family), styleFlags(info.styleFlags), sizeInPoints(info.sizeInPoints), sizeInPixels(info.sizeInPixels),
              dpiX(info.dpiX), dpiY(info.dpiY)
        {
        }
        
        virtual ~FontInfo()
        {
        }
        
        FontInfo & operator=(const FontInfo &other)
        {
            if (this != &other)
            {
                this->family       = other.family;
                this->styleFlags   = other.styleFlags;
                this->sizeInPoints = other.sizeInPoints;
                this->sizeInPixels = other.sizeInPixels;
                this->dpiX         = other.dpiX;
                this->dpiY         = other.dpiY;
            }
            
            return *this;
        }

        bool IsEqual(const FontInfo &other)
        {
            return this->family          == other.family        &&
                   this->styleFlags      == other.styleFlags    &&
                   this->sizeInPoints    == other.sizeInPoints  &&
                   this->sizeInPixels    == other.sizeInPixels  &&
                   this->dpiX            == other.dpiX          &&
                   this->dpiY            == other.dpiY;
        }
        
        
        /// The font family (Arial, Times, Courier, etc)
        GTLib::String family;
        
        /// The style flags of the font.
        uint32_t styleFlags;
        
        
        /// The size of the font, in points. We use a float here so we can have values like 8.5.
        float sizeInPoints;
        float sizeInPixels;
        
        /// The X DPI to use for the font. Default is 96.
        unsigned int dpiX;
        
        /// The Y DPI to use for the font. Default is 96.
        unsigned int dpiY;
    };
}


#endif
