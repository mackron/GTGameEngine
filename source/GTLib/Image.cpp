// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Image.hpp>
#include <GTLib/ImageDataConverter.hpp>
#include <GTLib/ImageUtils.hpp>
#include <GTLib/MipmapGenerator.hpp>
#include <GTLib/Math.hpp>
#include <cassert>

#include <GTLib/Timing.hpp>
#include <cstdio>

namespace GTLib
{
    Image::Image()
        : m_format(ImageFormat_Auto), m_mipmaps(), m_loader(nullptr), m_flipped(false)
    {
    }

    Image::Image(unsigned int width, unsigned int height, ImageFormat format, const void * data)
        : m_format(ImageFormat_Auto), m_mipmaps(), m_loader(nullptr), m_flipped(false)
    {
        if (!this->Load(width, height, format, data))
        {
            assert(false);
        }
    }

    Image::Image(const char *filename)
        : m_format(ImageFormat_Auto), m_mipmaps(), m_loader(nullptr), m_flipped(false)
    {
        this->LinkToFile(filename);
    }

    Image::~Image()
    {
        // Resetting will clear any memory.
        this->Reset();
    }

    bool Image::LinkToFile(const char *filename)
    {
        // The image must be reset.
        this->Reset();

        m_loader = ImageLoader::Create(filename);
        
        return this->InitFromImageLoader();
    }

    const char* Image::GetLinkedFileName() const
    {
        if (m_loader != nullptr)
        {
            return m_loader->GetFileName();
        }

        return nullptr;
    }

    bool Image::LoadMipmapFromFile(unsigned int mipmapIndex)
    {
        if (m_loader != nullptr)
        {
            if (m_loader->HasFileChanged())
            {
                // When a file changes, the loader needs to be re-opened...
                if (m_loader->Open())
                {
                    // ... and re-initialised from the loader.
                    if (this->InitFromImageLoader())
                    {
                        // Finally we need to post a message that the file has changed. This will allow an application
                        // to decide for itself what it wants to do.
                        this->OnFileChanged();
                    }
                }
            }

            if (mipmapIndex < m_mipmaps.count)
            {
                Mipmap &mipmap = m_mipmaps[mipmapIndex];
                if (m_loader->LoadMipmap(mipmapIndex, mipmap))
                {
                    this->OnMipmapChanged(mipmapIndex);
                    return true;
                }
            }
        }

        return false;
    }

    bool Image::PullNextMipmap()
    {
        if (m_loader != nullptr)
        {
            if (m_loader->HasFileChanged())
            {
                // When a file changes, the loader needs to be re-opened...
                if (m_loader->Open())
                {
                    // ... and re-initialised from the loader.
                    if (this->InitFromImageLoader())
                    {
                        // Finally we need to post a message that the file has changed. This will allow an application
                        // to decide for itself what it wants to do. For example, it may decide to invalidate all mipmaps
                        // so that a fresh copy of the data is loaded during future pull requests.
                        this->OnFileChanged();
                    }
                }
            }

            // Now we need to find the index of the next mipmap to load, remembering mipmaps are loaded smallest to largest (in size). The way
            // we do this is we loop over each mipmap available by the image and load the first one that maps to an empty mipmap in the Image
            // object. We then load that specific mipmap.
            ImageFileInfo fi;
            m_loader->GetImageInfo(fi);

            // We loop over the file's mipmaps backwards.
            for (unsigned int i = fi.mipmapCount; i > 0; --i)
            {
                unsigned int mipmapIndex = i - 1;
                auto & mipmap            = m_mipmaps[mipmapIndex];

                if (mipmap.data == nullptr)
                {
                    // We have found the mipmap. Now we load into it.
                    if (m_loader->LoadMipmap(mipmapIndex, mipmap))
                    {
                        this->OnMipmapChanged(mipmapIndex);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

        return false;
    }

    bool Image::PullAllMipmaps()
    {
        if (m_loader != nullptr)
        {
            while (this->PullNextMipmap());
            return true;
        }

        return false;
    }

    bool Image::Load(unsigned int width, unsigned int height, ImageFormat format, const void* data)
    {
        // The image must be reset before loading.
        this->Reset();

        // Dimensions of 0 is not allowed...
        if (width > 0 || height > 0)
        {
            // We need a copy of the data.
            void* newdata = ImageUtils::CopyImageData(format, width, height, data, m_flipped);
            if ((data != nullptr && newdata != nullptr) || data == nullptr)
            {
                // Mipmaps need to be reset before adding any data.
                this->ResetMipmaps(ImageUtils::CalculateMipmapCount(width, height));

                Mipmap &base = m_mipmaps[0];
                base.format  = format;
                base.width   = width;
                base.height  = height;
                base.data    = newdata;
                this->OnMipmapChanged(0);

                m_format = format;

                return true;
            }
        }

        return false;
    }
    
    bool Image::SetSubData(int xPos, int yPos, unsigned int width, unsigned int height, const void* data)
    {
        if (m_mipmaps.count > 0)
        {
            auto &mipmap = m_mipmaps[0];
            
            if (mipmap.data != nullptr)
            {
                if (xPos + width  <= mipmap.width &&
                    yPos + height <= mipmap.height)
                {
                    if (xPos >= 0 && yPos >= 0)
                    {
                        // Input is OK. We need to update, remembering to take whether or not the image is flipped into account. We do the copy row-by-row.
                        size_t srcPitch = ImageUtils::CalculatePitch(width,        m_format);
                        size_t dstPitch = ImageUtils::CalculatePitch(mipmap.width, m_format);
                        
                        if (m_flipped)
                        {
                            //yPos = mipmap.height - yPos - height;
                            
                            for (unsigned int iSourceRow = 0; iSourceRow < height; ++iSourceRow)
                            {
                                const uint8_t* srcData = reinterpret_cast<const uint8_t*>(data) + (iSourceRow * srcPitch);
                                      uint8_t* dstData = reinterpret_cast<      uint8_t*>(mipmap.data);
                                dstData += ((yPos + (height - iSourceRow - 1)) * dstPitch);     // <-- Y offset.
                                dstData += (xPos * GetTexelSizeInBytes(m_format));          // <-- X offset.
                                
                                memcpy(dstData, srcData, srcPitch);
                            }
                        }
                        else
                        {
                            for (unsigned int iSourceRow = 0; iSourceRow < height; ++iSourceRow)
                            {
                                const uint8_t* srcData = reinterpret_cast<const uint8_t*>(data) + (iSourceRow * srcPitch);
                                      uint8_t* dstData = reinterpret_cast<      uint8_t*>(mipmap.data);
                                dstData += ((yPos + iSourceRow) * dstPitch);                    // <-- Y offset.
                                dstData += (xPos * GetTexelSizeInBytes(m_format));          // <-- X offset.
                                
                                memcpy(dstData, srcData, srcPitch);
                            }
                        }
                        
                        return true;
                    }
                }
            }
        }
        
        return false;
    }

    bool Image::GenerateMipmaps()
    {
        // In order to generate mipmaps, we must have at least a base mipmap.
        if (m_mipmaps.count > 0 && m_mipmaps[0].IsValid())
        {
            for (size_t i = 1; i < m_mipmaps.count; ++i)    // Important to start at 1. Each mipmap is generate based on the previous one.
            {
                auto &mipmap = m_mipmaps[i];
                if (!mipmap.IsValid())              // We don't generate mipmaps for already-valid mipmaps.
                {
                    auto &sourceMipmap = m_mipmaps[i - 1];
                    MipmapGenerator::Generate(sourceMipmap, mipmap);

                    this->OnMipmapChanged(static_cast<unsigned int>(i));
                }
            }

            return true;
        }

        // Mipmap generation requires at least a base mipmap.
        return false;
    }

    size_t Image::GetMipmapCount() const
    {
        return m_mipmaps.count;
    }

    Mipmap & Image::GetMipmap(unsigned int index)
    {
        assert(index < m_mipmaps.count);
        {
            return m_mipmaps[index];
        }
    }

    const Mipmap & Image::GetMipmap(unsigned int index) const
    {
        assert(index < m_mipmaps.count);
        {
            return m_mipmaps[index];
        }
    }

    ImageFormat Image::GetFormat() const
    {
        return m_format;
    }

    bool Image::IsFloatingPointFormat() const
    {
        return GetImageFormatDataType(m_format) == ImageFormatDataTypes::Float;
    }

    unsigned int Image::GetWidth() const
    {
        if (m_mipmaps.count > 0 && m_mipmaps[0].IsValid())
        {
            return m_mipmaps[0].width;
        }

        return 0;
    }

    unsigned int Image::GetHeight() const
    {
        if (m_mipmaps.count > 0)
        {
            return m_mipmaps[0].height;
        }

        return 0;
    }

    const void* Image::GetBaseMipmapData() const
    {
        if (m_mipmaps.count > 0)
        {
            return m_mipmaps[0].data;
        }

        return nullptr;
    }

    size_t Image::GetBaseMipmapDataSize() const
    {
        if (m_mipmaps.count > 0)
        {
            return m_mipmaps[0].GetDataSizeInBytes();
        }

        return 0;
    }

    void Image::FlipVertically()
    {
        // Any existing data needs to be flipped.
        for (size_t i = 0; i < m_mipmaps.count; ++i)
        {
            if (m_mipmaps[i].data != nullptr)
            {
                m_mipmaps[i].FlipVertically();
                this->OnMipmapChanged(static_cast<unsigned int>(i));
            }
        }

        m_flipped = !m_flipped;
    }

    bool Image::GetValidMipmapRange(size_t &baseLevel, size_t &maxLevel) const
    {
        bool foundBase = false;

        // First we find the base level...
        for (size_t i = 0; i < m_mipmaps.count; ++i)
        {
            if (m_mipmaps[i].IsValid())
            {
                baseLevel = i;
                maxLevel  = i;
                foundBase = true;

                break;
            }
        }

        if (foundBase)
        {
            // Now we find the max level.
            for (size_t i = baseLevel + 1; i < m_mipmaps.count; ++i)
            {
                if (m_mipmaps[i].IsValid())
                {
                    ++maxLevel;
                }
                else
                {
                    break;
                }
            }

            return true;
        }
        else
        {
            // We'll get here if we couldn't find a base level. We set both to 0.
            baseLevel = 0;
            maxLevel  = 0;

            return false;
        }
    }


    void Image::Reset()
    {
        m_format = ImageFormat_Auto;

        // Any mipmaps must be deleted.
        this->ResetMipmaps(0);

        ImageLoader::Delete(m_loader);
        m_loader = nullptr;
    }

    void Image::ResetMipmaps(size_t newMipmapCount)
    {
        // We need to post events letting everything know that mipmaps are being deleted.
        for (size_t i = m_mipmaps.count; i > 0; --i)
        {
            this->OnMipmapDeleted(static_cast<unsigned int>(i) - 1);
        }

        m_mipmaps.Clear();
        m_mipmaps.Resize(newMipmapCount);
    }

    bool Image::InitFromImageLoader()
    {
        if (m_loader != nullptr)
        {
            // First we need image information.
            ImageFileInfo info;
            m_loader->GetImageInfo(info);

            // Our mipmaps need to be reset.
            this->ResetMipmaps(ImageUtils::CalculateMipmapCount(info.width, info.height));


            // Now we can set the base mipmap. It won't have any data at this point.
            Mipmap &base = m_mipmaps[0];
            base.format  = info.format;
            base.width   = info.width;
            base.height  = info.height;
            
            m_format = info.format;

            return true;
        }

        return false;
    }



    // Events.
    void Image::OnMipmapChanged(unsigned int)
    {
    }

    void Image::OnMipmapCreated(unsigned int)
    {
    }

    void Image::OnMipmapDeleted(unsigned int)
    {
    }

    void Image::OnFileChanged()
    {
    }
}
