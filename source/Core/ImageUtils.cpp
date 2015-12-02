// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/ImageUtils.hpp>
#include <GTGE/Core/Math.hpp>
#include <GTGE/Core/stdlib.hpp>
#include <cstring>      // For memcpy().

namespace GT
{
    namespace ImageUtils
    {
        unsigned int CalculateMipmapWidth(unsigned int mipmapIndex, unsigned int baseWidth)
        {
            return static_cast<unsigned int>(Max(1.0, baseWidth / std::pow(2.0, (int)mipmapIndex)));
        }

        unsigned int CalculateMipmapHeight(unsigned int mipmapIndex, unsigned int baseHeight)
        {
            return static_cast<unsigned int>(Max(1.0, baseHeight / std::pow(2.0, (int)mipmapIndex)));
        }

        unsigned int CalculateMipmapCount(unsigned int baseWidth)
        {
            return CalculateMipmapCount(baseWidth, 1, 1);
        }

        unsigned int CalculateMipmapCount(unsigned int baseWidth, unsigned int baseHeight)
        {
            return CalculateMipmapCount(baseWidth, baseHeight, 1);
        }

        unsigned int CalculateMipmapCount(unsigned int baseWidth, unsigned int baseHeight, unsigned int baseDepth)
        {
            float maxsize = static_cast<float>(Max(Max(baseWidth, baseHeight), baseDepth));

            return static_cast<unsigned int>((std::log(maxsize) / std::log(2.0f)) + 1);
        }

        size_t CalculatePitch(unsigned int width, ImageFormat format)
        {
            if (format == ImageFormat_R10G10B10A2)
            {
                return width * 4;
            }

            if (format != ImageFormat_None)
            {
                return width * GetTexelSizeInBytes(format);
            }

            return width;
        }

        size_t CalculateDataSize(unsigned int width, unsigned int height, ImageFormat format)
        {
            return height * ImageUtils::CalculatePitch(width, format);
        }

        void* CopyImageData(ImageFormat format, unsigned int width, unsigned int height, const void *sourceData, bool flip)
        {
            if (sourceData != nullptr)
            {
                void* result = malloc(CalculateDataSize(width, height, format));
            
                CopyImageData(result, sourceData, width, height, format, flip);
                
                return result;
            }
            
            return nullptr;
        }
        
        void CopyImageData(void* dstBuffer, const void* srcBuffer, unsigned int width, unsigned int height, ImageFormat format, bool flip)
        {
            if (srcBuffer != nullptr)
            {
                size_t pitch      = ImageUtils::CalculatePitch(width, format);
                size_t resultSize = pitch * height;

                if (flip)
                {
                    // When flipped, we copy bottom up.
                    for (unsigned int i = 0; i < height; ++i)
                    {
                        char* sourceRow = ((char *)srcBuffer) + ((height - i - 1) * pitch);
                        char* destRow   = ((char *)dstBuffer) + (i * pitch);

                        memcpy(destRow, sourceRow, pitch);
                    }
                }
                else
                {
                    memcpy(dstBuffer, srcBuffer, resultSize);
                }
            }
        }
        
        void FlipData(unsigned int pitch, unsigned int height, void* data)
        {
            // We need a swap buffer.
            auto swapBuffer = malloc(pitch);
            
            unsigned int halfHeight = height / 2;
            for (unsigned int iRow = 0; iRow < halfHeight; ++iRow)
            {
                auto row0 = reinterpret_cast<uint8_t*>(data) + (pitch * iRow);
                auto row1 = reinterpret_cast<uint8_t*>(data) + (pitch * (height - iRow - 1));
                
                memcpy(swapBuffer, row0,       pitch);
                memcpy(row0,       row1,       pitch);
                memcpy(row1,       swapBuffer, pitch);
            }
            
            free(swapBuffer);
        }
    }
}
