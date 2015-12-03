// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/ImageLoader.hpp>
#include <GTGE/Core/Strings/Equal.hpp>
#include <GTGE/Core/Strings/Create.hpp>
#include <GTGE/GTEngine.hpp>
#include <easy_path/easy_path.h>

// TODO: Delete this once DDS is implemented.
#define GTLIB_NO_DDS

#ifndef GTLIB_NO_PNG
#include "ImageLoader_PNG.hpp"
#endif

#ifndef GTLIB_NO_DDS
#include "ImageLoader_DDS.hpp"
#endif

// non-static part.
namespace GT
{
    ImageLoader::ImageLoader(const char *fileNameIn)
        : fileName(fileNameIn), absolutePath()
    {
        // We need to find the absolute path.
        char absolutePathTemp[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_EngineContext->GetVFS(), fileNameIn, absolutePathTemp, sizeof(absolutePathTemp))) {
            this->absolutePath = absolutePathTemp;
        }
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
namespace GT
{
    ImageLoader * ImageLoader::Create(const char *filename)
    {
        const char*  extension = easypath_extension(filename);
        ImageLoader* loader    = nullptr;
        
#ifndef GTLIB_NO_PNG
        if (Strings::Equal<false>(extension, "png"))
        {
            loader = new ImageLoader_PNG(filename);
        }
#endif
        
#ifndef GTLIB_NO_DDS
        if (Strings::Equal<false>(extension, "dds"))
        {
            loader = new ImageLoader_DDS(filename);
        }
#endif

        // If we found the loader, we need to try opening it. If opening fails, we need to delete
        // the loader and return nullptr.
        if (loader != nullptr)
        {
            if (loader->Open())
            {
                return loader;
            }
            else
            {
                delete loader;
                return nullptr;
            }
        }

        // If we made it here, we have no loader.
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
        if (Strings::Equal<false>(extension, "png"))
        {
            result = true;
        }
#endif
        
#ifndef GTIMAGE_NO_DDS
        if (Strings::Equal<false>(extension, "dds"))
        {
            result = true;
        }
#endif

        return result;
    }
}