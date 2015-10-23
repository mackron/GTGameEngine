// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/MipmapGenerator.hpp>
#include <GTLib/ImageUtils.hpp>
#include <GTLib/stdlib.hpp>
#include <cstdint>

namespace GTLib
{
    bool MipmapGenerator::Generate(const Mipmap &source, Mipmap &dest)
    {
        dest.DeleteLocalData();

        dest.format = source.format;
        dest.width  = ImageUtils::CalculateMipmapWidth(1, source.width);
        dest.height = ImageUtils::CalculateMipmapHeight(1, source.height);
        dest.data   = malloc(ImageUtils::CalculateDataSize(dest.width, dest.height, dest.format));

        unsigned int componentCount = GetImageFormatComponentCount(dest.format);
        unsigned int componentSize  = GetImageFormatComponentSize(dest.format);

        if (dest.format == ImageFormat_RGB8)
        {
            MipmapGenerator::GenerateRGB8(source.width, source.height, source.data, dest.data);
        }
        else if (dest.format == ImageFormat_RGBA8)
        {
            MipmapGenerator::GenerateRGBA8(source.width, source.height, source.data, dest.data);
        }
        else
        {
            // Now we need to generate the mipmap depending on the format data type.
            int componentDataType = GetImageFormatDataType(dest.format);
            if (componentDataType == ImageFormatDataTypes::Integer)
            {
                switch (componentSize)
                {
                case 1: MipmapGenerator::Generate<uint8_t >(source.width, source.height, componentCount, source.data, dest.data); break;
                case 2: MipmapGenerator::Generate<uint16_t>(source.width, source.height, componentCount, source.data, dest.data); break;
                case 4: MipmapGenerator::Generate<uint32_t>(source.width, source.height, componentCount, source.data, dest.data); break;
                case 8: MipmapGenerator::Generate<uint64_t>(source.width, source.height, componentCount, source.data, dest.data); break;

                default:
                    {
                        // Error. Unknown or unsupported component size.
                        return false;
                    }
                }
            }
            else if (componentDataType == ImageFormatDataTypes::Float)
            {
                switch (componentSize)
                {
                case 4: MipmapGenerator::Generate<float >(source.width, source.height, componentCount, source.data, dest.data); break;
                case 8: MipmapGenerator::Generate<double>(source.width, source.height, componentCount, source.data, dest.data); break;

                default:
                    {
                        // Error. Unknown or unsupported component size.
                        return false;
                    }
                }
            }
            else
            {
                // Error. Can't generate mipmaps with this format.
                return false;
            }
        }

        return true;
    }


    void MipmapGenerator::GenerateRGB8(unsigned int sourceWidth, unsigned int sourceHeight, const void* sourceDataIn, void* destDataIn)
    {
        const uint8_t* sourceData = reinterpret_cast<const uint8_t*>(sourceDataIn);
              uint8_t* destData   = reinterpret_cast<      uint8_t*>(destDataIn);

        unsigned int destWidth  = ImageUtils::CalculateMipmapWidth(1, sourceWidth);
        unsigned int destHeight = ImageUtils::CalculateMipmapHeight(1, sourceHeight);

        unsigned int destTexelIndex = 0;

        unsigned int texel00 = 0;
        unsigned int texel01 = sourceWidth * 3;
        unsigned int texel11 = sourceWidth * 3 + 3;
        unsigned int texel10 = 3;

        for (unsigned int iRow = 0; iRow < destHeight; ++iRow)
        {
            for (unsigned int iCol = 0; iCol < destWidth; ++iCol)
            {
                // 00
                unsigned int accumulatedR = sourceData[texel00 + 0];
                unsigned int accumulatedG = sourceData[texel00 + 1];
                unsigned int accumulatedB = sourceData[texel00 + 2];

                // 01
                accumulatedR += sourceData[texel01 + 0];
                accumulatedG += sourceData[texel01 + 1];
                accumulatedB += sourceData[texel01 + 2];

                // 11
                accumulatedR += sourceData[texel11 + 0];
                accumulatedG += sourceData[texel11 + 1];
                accumulatedB += sourceData[texel11 + 2];

                // 10
                accumulatedR += sourceData[texel10 + 0];
                accumulatedG += sourceData[texel10 + 1];
                accumulatedB += sourceData[texel10 + 2];


                // Construct the final value.
                destData[destTexelIndex + 0] = static_cast<uint8_t>(accumulatedR >> 2);
                destData[destTexelIndex + 1] = static_cast<uint8_t>(accumulatedG >> 2);
                destData[destTexelIndex + 2] = static_cast<uint8_t>(accumulatedB >> 2);


                destTexelIndex += 3;


                texel00 += 6;
                texel01 += 6;
                texel11 += 6;
                texel10 += 6;
            }

            texel00 += sourceWidth * 3;
            texel01 += sourceWidth * 3;
            texel11 += sourceWidth * 3;
            texel10 += sourceWidth * 3;
        }
    }

    void MipmapGenerator::GenerateRGBA8(unsigned int sourceWidth, unsigned int sourceHeight, const void* sourceDataIn, void* destDataIn)
    {
        const uint32_t rMask = 0xFF000000;
        const uint32_t gMask = 0x00FF0000;
        const uint32_t bMask = 0x0000FF00;
        const uint32_t aMask = 0x000000FF;
        const uint32_t rShift = 24;
        const uint32_t gShift = 16;
        const uint32_t bShift = 8;
        const uint32_t aShift = 0;

        const uint32_t* sourceData = reinterpret_cast<const uint32_t*>(sourceDataIn);
              uint32_t* destData   = reinterpret_cast<      uint32_t*>(destDataIn);

        unsigned int destWidth  = ImageUtils::CalculateMipmapWidth(1, sourceWidth);
        unsigned int destHeight = ImageUtils::CalculateMipmapHeight(1, sourceHeight);

        unsigned int destTexelIndex = 0;

        unsigned int texel00 = 0;
        unsigned int texel01 = sourceWidth;
        unsigned int texel11 = sourceWidth + 1;
        unsigned int texel10 = 1;

        unsigned int temp00;
        unsigned int temp01;
        unsigned int temp11;
        unsigned int temp10;

        unsigned int accumulatedR;
        unsigned int accumulatedG;
        unsigned int accumulatedB;
        unsigned int accumulatedA;

        for (unsigned int iRow = 0; iRow < destHeight; ++iRow)
        {
            for (unsigned int iCol = 0; iCol < destWidth; ++iCol)
            {
                temp00 = sourceData[texel00];
                temp01 = sourceData[texel01];
                temp11 = sourceData[texel11];
                temp10 = sourceData[texel10];

                // 00
                accumulatedR  = (temp00 & rMask) >> rShift;
                accumulatedG  = (temp00 & gMask) >> gShift;
                accumulatedB  = (temp00 & bMask) >> bShift;
                accumulatedA  = (temp00 & aMask) >> aShift;

                // 01
                accumulatedR += (temp01 & rMask) >> rShift;
                accumulatedG += (temp01 & gMask) >> gShift;
                accumulatedB += (temp01 & bMask) >> bShift;
                accumulatedA += (temp01 & aMask) >> aShift;

                // 11
                accumulatedR += (temp11 & rMask) >> rShift;
                accumulatedG += (temp11 & gMask) >> gShift;
                accumulatedB += (temp11 & bMask) >> bShift;
                accumulatedA += (temp11 & aMask) >> aShift;

                // 10
                accumulatedR += (temp10 & rMask) >> rShift;
                accumulatedG += (temp10 & gMask) >> gShift;
                accumulatedB += (temp10 & bMask) >> bShift;
                accumulatedA += (temp10 & aMask) >> aShift;


                // Construct the final value.
                uint32_t finalColour =
                    ((accumulatedA >> 2) << aShift) |
                    ((accumulatedB >> 2) << bShift) |
                    ((accumulatedG >> 2) << gShift) |
                    ((accumulatedR >> 2) << rShift);

                destData[destTexelIndex] = finalColour;


                ++destTexelIndex;


                texel00 += 2;
                texel01 += 2;
                texel11 += 2;
                texel10 += 2;
            }

            texel00 += sourceWidth;
            texel01 += sourceWidth;
            texel11 += sourceWidth;
            texel10 += sourceWidth;
        }
    }
}
