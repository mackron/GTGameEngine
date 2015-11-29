// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_ImageInfo_hpp_
#define __GTLib_ImageInfo_hpp_

#include "ImageFormats.hpp"
#include <easy_fs/easy_vfs.h>

namespace GTLib
{
    struct ImageFileInfo : public easyvfs_file_info
    {
        ImageFileInfo()
            : easyvfs_file_info(), format(ImageFormat_Auto), width(0), height(0), mipmapCount(0)
        {
        }

        ~ImageFileInfo()
        {
        }

        /// Assignment.
        ImageFileInfo & operator=(const ImageFileInfo &other)
        {
            easyvfs_file_info::operator=(other);

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
