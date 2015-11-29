// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_ImageFormats_hpp_
#define __GTLib_ImageFormats_hpp_

#include <cstddef>

#define GTIMAGE_GENIMAGEFORMATID(componentCount, componentSize, componentType) \
    (componentCount << 24) | (componentSize << 16) | (componentType << 8)

namespace GT
{
    namespace ImageFormatDataTypes
    {
        static const int Integer      = 1;
        static const int Float        = 2;
        static const int Depth        = 3;
        static const int DepthStencil = 4;
    }

    /// Enumerator for the different image formats. We encode information about the formats in the IDs, which are generated with
    /// the GTIMAGE_GENIMAGEFORMATID macro.
    enum ImageFormat
    {
        ImageFormat_Auto  = 0,          ///< The default format. The format of the image will be determined based on the data that was used when loading/setting the data.
        ImageFormat_None  = 0,

        ImageFormat_R8               = GTIMAGE_GENIMAGEFORMATID(1, 1, ImageFormatDataTypes::Integer),       ///< 8-bit
        ImageFormat_RG8              = GTIMAGE_GENIMAGEFORMATID(2, 1, ImageFormatDataTypes::Integer),       ///< 16-bit
        ImageFormat_RGB8             = GTIMAGE_GENIMAGEFORMATID(3, 1, ImageFormatDataTypes::Integer),       ///< 24-bit RGB format.
        ImageFormat_RGBA8            = GTIMAGE_GENIMAGEFORMATID(4, 1, ImageFormatDataTypes::Integer),       ///< 32-bit RGBA format.
        ImageFormat_RGBA16           = GTIMAGE_GENIMAGEFORMATID(4, 2, ImageFormatDataTypes::Integer),       ///< 64-bit
        ImageFormat_RGBA32           = GTIMAGE_GENIMAGEFORMATID(4, 4, ImageFormatDataTypes::Integer),       ///< 128-bit
        ImageFormat_R16F             = GTIMAGE_GENIMAGEFORMATID(1, 2, ImageFormatDataTypes::Float),
        ImageFormat_RG16F            = GTIMAGE_GENIMAGEFORMATID(2, 2, ImageFormatDataTypes::Float),
        ImageFormat_RGB16F           = GTIMAGE_GENIMAGEFORMATID(3, 2, ImageFormatDataTypes::Float),
        ImageFormat_RGBA16F          = GTIMAGE_GENIMAGEFORMATID(4, 2, ImageFormatDataTypes::Float),         ///< 64-bit floating-point.
        ImageFormat_R32F             = GTIMAGE_GENIMAGEFORMATID(1, 4, ImageFormatDataTypes::Float),
        ImageFormat_RG32F            = GTIMAGE_GENIMAGEFORMATID(2, 4, ImageFormatDataTypes::Float),
        ImageFormat_RGB32F           = GTIMAGE_GENIMAGEFORMATID(3, 4, ImageFormatDataTypes::Float),
        ImageFormat_RGBA32F          = GTIMAGE_GENIMAGEFORMATID(4, 4, ImageFormatDataTypes::Float),         ///< 128-bit floating-point.

        ImageFormat_Depth16          = GTIMAGE_GENIMAGEFORMATID(1, 2, ImageFormatDataTypes::Depth),         ///< 16-bit depth, no stencil.
        ImageFormat_Depth24          = GTIMAGE_GENIMAGEFORMATID(1, 3, ImageFormatDataTypes::Depth),         ///< 24-bit depth, no stencil.
        ImageFormat_Depth32          = GTIMAGE_GENIMAGEFORMATID(1, 4, ImageFormatDataTypes::Depth),         ///< 32-bit depth, no stencil.
        ImageFormat_Depth24_Stencil8 = GTIMAGE_GENIMAGEFORMATID(2, 4, ImageFormatDataTypes::DepthStencil),  ///< 24-bit depth, 8-bit stencil (32-bit).

        
        // Special formats.
        ImageFormat_R10G10B10A2      = 1,
    };

    /// Retrieves the number of components in the given image format. For example, RGB8 will have 3 component (R, G and B).
    inline unsigned int GetImageFormatComponentCount(ImageFormat format)
    {
        if (format == ImageFormat_R10G10B10A2)
        {
            return 4;
        }

        return (static_cast<unsigned int>(format) & 0xFF000000) >> 24;
    }

    /// Retrieve the size of each component, in bytes.
    inline unsigned int GetImageFormatComponentSize(ImageFormat format)
    {
        if (format == ImageFormat_R10G10B10A2)
        {
            return 4;
        }

        return (static_cast<unsigned int>(format) & 0x00FF0000) >> 16;
    }

    /// Retrieves the data type of each component.
    inline int GetImageFormatDataType(ImageFormat format)
    {
        if (format == ImageFormat_R10G10B10A2)
        {
            return ImageFormatDataTypes::Integer;
        }

        return (static_cast<int>(format) & 0x0000FF00) >> 8;
    }

    /// Determines whether or not the format has an alpha component.
    inline bool DoesImageFormatHaveAlphaComponent(ImageFormat format)
    {
        return GetImageFormatComponentCount(format) == 4;
    }
    
    /// Retrieves the size of a texel, in bytes.
    inline size_t GetTexelSizeInBytes(ImageFormat format)
    {
        return GetImageFormatComponentCount(format) * GetImageFormatComponentSize(format);
    }
}

#endif
