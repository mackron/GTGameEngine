// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/ImageLoader.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Log.hpp>
#include <GTLib/Strings/Equal.hpp>
#include <GTLib/Strings/Create.hpp>

// TODO: Delete this once DDS is implemented.
#define GTLIB_NO_DDS

#ifndef GTLIB_NO_PNG
#include "ImageLoader_PNG.hpp"
#endif

#ifndef GTLIB_NO_DDS
#include "ImageLoader_DDS.hpp"
#endif

// non-static part.
namespace GTLib
{
    ImageLoader::ImageLoader(const char *fileNameIn)
        : fileName(fileNameIn), absolutePath()
    {
        // We need to find the absolute path.
        GTLib::IO::FindAbsolutePath(fileNameIn, this->absolutePath);
    }

    ImageLoader::~ImageLoader()
    {
    }

    const char* ImageLoader::GetFileName() const
    {
        return this->fileName.c_str();
    }

    const char* ImageLoader::GetAbsolutePath() const
    {
        return this->absolutePath.c_str();
    }
}

// Static part.
namespace GTLib
{
    ImageLoader * ImageLoader::Create(const char *filename, bool loadUpsideDown)
    {
        auto    extension = GTLib::Path::Extension(filename);
        ImageLoader* loader    = nullptr;
        
#ifndef GTLIB_NO_PNG
        if (GTLib::Strings::Equal<false>(extension, "png"))
        {
            loader = new ImageLoader_PNG(filename);
        }
#endif
        
#ifndef GTLIB_NO_DDS
        if (GTLib::Strings::Equal<false>(extension, "dds"))
        {
            loader = new ImageLoader_DDS(filename);
        }
#endif

        // If we found the loader, we need to try opening it. If opening fails, we need to delete
        // the loader and return nullptr.
        if (loader != nullptr)
        {
            loader->SetLoadUpsideDown(loadUpsideDown);
            if (loader->Open())
            {
                return loader;
            }
            else
            {
                GTLib::GlobalLog.Write("GTLib - Failed to open image file: %s", filename);
                delete loader;
            }
        }

        // If we made it here, we have no loader.
        GTLib::GlobalLog.Write("GTLib - Failed to load image: %s. Unsupported file type.", filename);
        return nullptr;
    }

    void ImageLoader::Delete(ImageLoader *loader)
    {
        delete loader;
    }


    bool ImageLoader::IsExtensionSupported(const char* extension)
    {
        bool result = false;

#ifndef GTIMAGE_NO_PNG
        if (GTLib::Strings::Equal<false>(extension, "png"))
        {
            result = true;
        }
#endif
        
#ifndef GTIMAGE_NO_DDS
        if (GTLib::Strings::Equal<false>(extension, "dds"))
        {
            result = true;
        }
#endif

        return result;
    }
}