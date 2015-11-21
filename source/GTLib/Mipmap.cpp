// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Mipmap.hpp>
#include <GTLib/ImageUtils.hpp>
#include <GTLib/stdlib.hpp>
#include <cstring>

namespace GTLib
{
    Mipmap::Mipmap()
        : format(ImageFormat_Auto), width(0), height(0), data(nullptr)
    {
    }

    Mipmap::Mipmap(const Mipmap &other)
        : format(other.format), width(other.width), height(other.height), data(ImageUtils::CopyImageData(other.format, other.width, other.height, other.data))
    {
    }

    Mipmap::Mipmap(ImageFormat format, unsigned int width, unsigned int height, void* data)
        : format(format), width(width), height(height), data(data)
    {
    }

    Mipmap::~Mipmap()
    {
        free(this->data);
    }

    void Mipmap::FlipVertically()
    {
        if (this->data != nullptr)
        {
            auto pitch   = this->GetPitch();
            auto tempRow = reinterpret_cast<char *>(malloc(pitch));

            auto halfHeight = this->height >> 1;
            for (auto i = 0U; i < halfHeight; ++i)
            {
                char *rowA = ((char *)this->data) + (i * pitch);
                char *rowB = ((char *)this->data) + ((height - i - 1) * pitch);

                // Now we just do a standard swap.
                memcpy(tempRow, rowA,    pitch);
                memcpy(rowA,    rowB,    pitch);
                memcpy(rowB,    tempRow, pitch);
            }

            free(tempRow);
        }
    }

    size_t Mipmap::GetPitch() const
    {
        return ImageUtils::CalculatePitch(this->width, this->format);
    }

    size_t Mipmap::GetDataSizeInBytes() const
    {
        return ImageUtils::CalculateDataSize(this->width, this->height, this->format);
    }

    void Mipmap::DeleteLocalData()
    {
        free(this->data);
        this->data = nullptr;
    }

    void Mipmap::SetDataDirect(void *dataIn)
    {
        free(this->data);
        this->data = dataIn;
    }

    void Mipmap::Reset()
    {
        this->format = ImageFormat_Auto;
        this->width  = 0;
        this->height = 0;

        free(this->data);
        this->data = nullptr;
    }
}
