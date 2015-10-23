// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/GUI2/GUIImageManager_Default.hpp>
#include <GTLib/ImageLoader.hpp>

namespace GT
{
    GUIImageManager_Default::GUIImageManager_Default()
        : handles()
    {
    }

    GUIImageManager_Default::~GUIImageManager_Default()
    {
    }


    ImageHandle GUIImageManager_Default::CreateImage(unsigned int width, unsigned int height, ImageFormat format, const void* data)
    {
        size_t dataSize = width * height * GUIImageManager_Default::GetFormatBPP(format);

        Image* image = new Image;
        image->width = width;
        image->height = height;
        image->format = format;
        image->data = malloc(dataSize);

        if (data != nullptr)
        {
            memcpy(image->data, data, dataSize);
        }


        image->handle = handles.CreateHandle();
        if (image->handle != 0)
        {
            handles.AssociateObjectWithHandle(image->handle, image);
        }

        return image->handle;
    }

    ImageHandle GUIImageManager_Default::CreateImage(unsigned int width, unsigned int height, ImageFormat format, void* data)
    {
        return this->CreateImage(width, height, format, const_cast<const void*>(data));
    }

    ImageHandle GUIImageManager_Default::LoadImage(const char* filePath)
    {
        // TODO: This can be optimized. There is a redundant copy here. Better option is to read data directly into image buffer.

        ImageHandle image = 0;

        auto loader = GTLib::ImageLoader::Create(filePath);
        if (loader != nullptr)
        {
            GTLib::ImageFileInfo info;
            loader->GetImageInfo(info);
        
            if (info.format == GTLib::ImageFormat_RGB8 || info.format == GTLib::ImageFormat_RGBA8)      // <-- Only supporting RGB8 and RGBA8 formats.
            {
                GTLib::Mipmap mipmap;
                if (loader->LoadMipmap(0, mipmap))      // <-- '0' means the base mipmap.
                {
                    image = this->CreateImage(mipmap.width, mipmap.height, (info.format == GTLib::ImageFormat_RGB8) ? ImageFormat_RGB8 : ImageFormat_RGBA8, mipmap.data);
                }
                else
                {
                    // Error loading the mipmap. Fall through and return null.
                }
            }
            else
            {
                // Not a supported format. Fall through and return null.
            }
        }

        GTLib::ImageLoader::Delete(loader);
    
        return image;
    }


    void GUIImageManager_Default::DeleteImage(ImageHandle hImage)
    {
        auto image = this->GetImagePtr(hImage);
        if (image != nullptr)
        {
            delete image;
            handles.DeleteHandle(hImage);
        }
    }


    void* GUIImageManager_Default::MapImageData(ImageHandle hImage)
    {
        auto image = this->GetImagePtr(hImage);
        if (image != nullptr)
        {
            return image->data;
        }

        return nullptr;
    }

    void GUIImageManager_Default::UnmapImageData(ImageHandle hImage)
    {
        // Don't actually need to do anything here...
        (void)hImage;
    }
    

    void GUIImageManager_Default::SetImageSubData(ImageHandle hImage, unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, void* data)
    {
        auto image = this->GetImagePtr(hImage);
        if (image != nullptr)
        {
            if (xPos < image->width && yPos < image->height)
            {
                // Clamp the width and height to the image boundary.
                if (xPos + width  > image->width)  { width  -= (xPos + width)  - image->width;  }
                if (yPos + height > image->height) { height -= (yPos + height) - image->height; }

                switch (image->format)
                {
                case ImageFormat_A8:
                    {
                        uint8_t* srcData = reinterpret_cast<uint8_t*>(data);
                        uint8_t* dstData = reinterpret_cast<uint8_t*>(image->data);

                        for (unsigned int y = 0; y < height; ++y)
                        {
                            unsigned int srcOffset = (y)        * width;
                            unsigned int dstOffset = (y + yPos) * image->width;
                            for (unsigned int x = 0; x < width; ++x)
                            {
                                unsigned int iSrc = srcOffset + x;
                                unsigned int iDst = dstOffset + x + xPos;

                                dstData[iDst] = srcData[iSrc];
                            }
                        }

                        break;
                    }

                case ImageFormat_RGB8:
                    {
                        uint8_t* srcData = reinterpret_cast<uint8_t*>(data);
                        uint8_t* dstData = reinterpret_cast<uint8_t*>(image->data);

                        for (unsigned int y = 0; y < height; ++y)
                        {
                            unsigned int srcOffset = (y)        * width        * 3;
                            unsigned int dstOffset = (y + yPos) * image->width * 3;
                            for (unsigned int x = 0; x < width; ++x)
                            {
                                unsigned int iSrc = srcOffset + (x*3);
                                unsigned int iDst = dstOffset + ((x + xPos)*3);

                                dstData[iDst+0] = srcData[iSrc+0];
                                dstData[iDst+1] = srcData[iSrc+1];
                                dstData[iDst+2] = srcData[iSrc+2];
                            }
                        }

                        break;
                    }

                case ImageFormat_RGBA8:
                    {
                        uint32_t* srcData = reinterpret_cast<uint32_t*>(data);
                        uint32_t* dstData = reinterpret_cast<uint32_t*>(image->data);

                        for (unsigned int y = 0; y < height; ++y)
                        {
                            unsigned int srcOffset = (y)        * width;
                            unsigned int dstOffset = (y + yPos) * image->width;
                            for (unsigned int x = 0; x < width; ++x)
                            {
                                unsigned int iSrc = srcOffset + x;
                                unsigned int iDst = dstOffset + x + xPos;

                                dstData[iDst] = srcData[iSrc];
                            }
                        }

                        break;
                    }


                case ImageFormat_None:
                default:
                    {
                        // Do nothing.
                        break;
                    }
                }
            }
        }
    }

    
    void GUIImageManager_Default::GetImageDimensions(ImageHandle hImage, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto image = this->GetImagePtr(hImage);
        if (image != nullptr)
        {
            widthOut  = image->width;
            heightOut = image->height;
        }
    }

    ImageFormat GUIImageManager_Default::GetImageFormat(ImageHandle hImage) const
    {
        auto image = this->GetImagePtr(hImage);
        if (image != nullptr)
        {
            return image->format;
        }

        return ImageFormat_None;
    }



    //////////////////////////////////////////////
    // Private

    GUIImageManager_Default::Image* GUIImageManager_Default::GetImagePtr(ImageHandle hImage) const
    {
        auto image = handles.GetAssociatedObject(hImage);
        if (image != nullptr)
        {
            if (hImage == image->handle)
            {
                return image;
            }
            else
            {
                // Invalid handle (uniqueness IDs do not match).
                return nullptr;
            }
        }
        else
        {
            // Invalid handle (deleted or null).
            return nullptr;
        }
    }




    ///////////////////////////////////////////
    // Static Helpers

    unsigned int GUIImageManager_Default::GetFormatBPP(ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat_RGB8:  return 3;
        case ImageFormat_RGBA8: return 4;
        case ImageFormat_A8:    return 1;

        case ImageFormat_None:  return 0;
        default: return 1;
        }
    }
}