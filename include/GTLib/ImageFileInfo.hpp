// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_ImageInfo_hpp_
#define __GTLib_ImageInfo_hpp_

#include "ImageFormats.hpp"
#include <GTLib/IO.hpp>

namespace GTLib
{
    struct ImageFileInfo : public GTLib::FileInfo
    {
        ImageFileInfo()
            : FileInfo(), format(ImageFormat_Auto), width(0), height(0), mipmapCount(0)
        {
        }

        ImageFileInfo(const char *filename)
            : FileInfo(filename), format(ImageFormat_Auto), width(0), height(0), mipmapCount(0)
        {
        }
        
        ~ImageFileInfo()
        {
        }

        /// Assignment.
        ImageFileInfo & operator=(const ImageFileInfo &other)
        {
            GTLib::FileInfo::operator=(other);

            this->format      = other.format;
            this->width       = other.width;
            this->height      = other.height;
            this->mipmapCount = other.mipmapCount;

            return *this;
        }

        ImageFormat format;
        unsigned int width;
        unsigned int height;
        unsigned int mipmapCount;
    };
}

#endif
