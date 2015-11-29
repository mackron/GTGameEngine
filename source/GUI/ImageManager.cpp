// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/ImageManager.hpp>

namespace GTGUI
{
    ImageManager::ImageManager()
    {
    }
    
    ImageManager::~ImageManager()
    {
    }
    
    void ImageManager::GetImageDimensions(ImageHandle image, float &widthOut, float &heightOut) const
    {
        unsigned int width;
        unsigned int height;
        this->GetImageDimensions(image, width, height);
        
        widthOut  = static_cast<float>(width);
        heightOut = static_cast<float>(height);
    }
    
    void ImageManager::GetImageDimensions(ImageHandle image, double &widthOut, double &heightOut) const
    {
        unsigned int width;
        unsigned int height;
        this->GetImageDimensions(image, width, height);
        
        widthOut  = static_cast<double>(width);
        heightOut = static_cast<double>(height);
    }
}
