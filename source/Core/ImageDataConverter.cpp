// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/ImageDataConverter.hpp>
#include <GTEngine/Core/ImageUtils.hpp>
#include <GTEngine/Core/stdlib.hpp>
#include <cstring>

namespace GT
{
    ImageDataConverter::ImageDataConverter(ImageFormat from, ImageFormat to)
        : sourceFormat(from), destFormat(to)
    {
    }

    ImageDataConverter::~ImageDataConverter()
    {
    }

    void* ImageDataConverter::Convert(unsigned int sourceWidth, unsigned int sourceHeight, const void *sourceData, bool flip)
    {
        // The default implementation is just a straight copy.
        return ImageUtils::CopyImageData(this->sourceFormat, sourceWidth, sourceHeight, sourceData, flip);
    }







    ////////////////////////////////////////////////////
    // Static Functions

    ImageDataConverter* ImageDataConverter::Create(ImageFormat sourceFormat, ImageFormat destFormat)
    {
        if (sourceFormat == destFormat || destFormat == ImageFormat_Auto)
        {
            // We only get here if the formats are the same. To hand the case when destFormat is ImageFormat_Auto, we just
            // pass the source format as both arguments.
            return new ImageDataConverter(sourceFormat, sourceFormat);
        }


        // If we make it here, the converter is not supported.
        return nullptr;
    }

    void ImageDataConverter::Delete(ImageDataConverter *converter)
    {
        delete converter;
    }
}
