// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_MipmapGenerator
#define GT_MipmapGenerator

#include "ImageUtils.hpp"

namespace GT
{
    /// Static class for generating mipmaps.
    class MipmapGenerator
    {
    public:

        /// Mipmap generation function optimized for RGB8 image formats.
        ///
        /// @param sourceWidth  [in] The width of the source mipmap.
        /// @param sourceHeight [in] The height of the source mipmap.
        /// @param sourceData   [in] A pointer to the source mipmaps image data.
        /// @param destData     [in] A pointer to the destination buffer. This must be preallocated.
        ///
        /// @remarks
        ///     The destination buffer must be pre-allocated.
        static void GenerateRGB8(unsigned int sourceWidth, unsigned int sourceHeight, const void* sourceData, void* destData);

        /// Mipmap generation function optimized for RGBA8 image formats.
        ///
        /// @param sourceWidth  [in] The width of the source mipmap.
        /// @param sourceHeight [in] The height of the source mipmap.
        /// @param sourceData   [in] A pointer to the source mipmaps image data.
        /// @param destData     [in] A pointer to the destination buffer. This must be preallocated.
        ///
        /// @remarks
        ///     The destination buffer must be pre-allocated.
        static void GenerateRGBA8(unsigned int sourceWidth, unsigned int sourceHeight, const void* sourceData, void* destData);


        /// Generic function for generating mipmap data.
        ///
        /// @param sourceWidth    [in] The width of the source mipmap.
        /// @param sourceHeight   [in] The height of the source mipmap.
        /// @param componentCount [in] The number of components in each texel.
        /// @param sourceData     [in] A pointer to the source mipmaps image data.
        /// @param destData       [in] A pointer to the destination buffer. This must be preallocated.
        ///
        /// @remarks
        ///     The destination buffer must be pre-allocated. The <T> template argument is the data type of each component of a texel. For example,
        ///     the RGB8 format will be an unsigned char or uint8_t.
        template <typename T>
        static void Generate(unsigned int sourceWidth, unsigned int sourceHeight, unsigned int componentCount, const void* sourceData, void* destData)
        {
            unsigned int destWidth  = GT::CalculateMipmapSize(1, sourceWidth);
            unsigned int destHeight = GT::CalculateMipmapSize(1, sourceHeight);

            for (unsigned int iRow = 0; iRow < destHeight; ++iRow)
            {
                unsigned int y = iRow << 1;

                for (unsigned int iCol = 0; iCol < destWidth; ++iCol)
                {
                    unsigned int x = iCol << 1;

                    for (unsigned int iComp = 0; iComp < componentCount; ++iComp)
                    {
                        unsigned int texel00 = (((y + 0) * sourceWidth) + (x + 0)) * componentCount + iComp;
				        unsigned int texel01 = (((y + 1) * sourceWidth) + (x + 0)) * componentCount + iComp;
				        unsigned int texel11 = (((y + 1) * sourceWidth) + (x + 1)) * componentCount + iComp;
				        unsigned int texel10 = (((y + 0) * sourceWidth) + (x + 1)) * componentCount + iComp;

                        T data00 = reinterpret_cast<const T*>(sourceData)[texel00];
                        T data01 = reinterpret_cast<const T*>(sourceData)[texel01];
                        T data11 = reinterpret_cast<const T*>(sourceData)[texel11];
                        T data10 = reinterpret_cast<const T*>(sourceData)[texel10];

				        static_cast<T*>(destData)[((iRow * destWidth) + iCol) * componentCount + iComp] = static_cast<T>((data00 + data01 + data11 + data10) / T(4.0));
                    }
                }
            }
        }
    };
}

#endif
