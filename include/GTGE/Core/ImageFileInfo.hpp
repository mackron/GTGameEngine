// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_ImageInfo
#define GT_ImageInfo

#include "ImageFormats.hpp"
#include <dr_libs/dr_vfs.h>

namespace GT
{
    struct ImageFileInfo : public drvfs_file_info
    {
        ImageFileInfo()
            : drvfs_file_info(), format(ImageFormat_Auto), width(0), height(0), mipmapCount(0)
        {
        }

        ~ImageFileInfo()
        {
        }

        /// Assignment.
        ImageFileInfo & operator=(const ImageFileInfo &other)
        {
            drvfs_file_info::operator=(other);

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
