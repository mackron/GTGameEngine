// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_ImageDataConverter
#define GT_ImageDataConverter

#include "ImageFormats.hpp"

namespace GT
{
    /**
    *   \brief  Base class for data converters.
    *
    *   The base implementation does a direct copy of the data. Other converters, such as RGBA8 to DXT5, need to inherit from
    *   this class and implement Convert().
    */
    class ImageDataConverter
    {
    public:

        /**
        *   \brief  Constructor.
        */
        ImageDataConverter(ImageFormat from, ImageFormat to);

        /**
        *   \brief  Destructor.
        */
        virtual ~ImageDataConverter();

        /**
        *   \brief  Performs the actual data conversion.
        *
        *   \remarks
        *       The returned data should be freed with free().
        */
        virtual void * Convert(unsigned int sourceWidth, unsigned int sourceHeight, const void *source, bool flip = false);


    protected:

        ImageFormat sourceFormat;
        ImageFormat destFormat;


    // Static functions.
    public:

        /**
        *   \brief  Creates a data converter based on the given formats. If a converter can not be created, returns null.
        */
        static ImageDataConverter * Create(ImageFormat from, ImageFormat to);

        /**
        *   \brief  Deletes a converter created with Create.
        */
        static void Delete(ImageDataConverter *converter);

    };
}

#endif