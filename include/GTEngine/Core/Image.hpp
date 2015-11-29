// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Image_hpp_
#define __GTLib_Image_hpp_

#include "ImageFormats.hpp"
#include "Mipmap.hpp"
#include "ImageLoader.hpp"
#include <GTEngine/Core/Vector.hpp>

namespace GTLib
{
    /**
    *   \brief  The main image class for GTLib.
    *
    *   This class represents an image. Instead of thinking of an image as a flat 2D plane, GTLib thinks of them as an
    *   array of mipmaps, each of which is half the dimensions of the previous.
    *
    *   Images can be loaded from files or stored completely in-memory. When loading from a file, you first "link" to to
    *   the file. Linking to a file does not actually load the image data. Instead, the image data is manually loaded using
    *   PullNextMipmap(), PullAllMipmaps(), etc. This style of API gives applications a good level of control over exactly
    *   when an image is loaded.
    *
    *   PullNextMipmap() loads the mipmaps from smallest to largest.
    *
    *   The image class is designed to be inheritted. For example, a 2D texture class in a game engine may inherit from
    *   this class.
    *
    *   As an image is modified, it will call virtual methods that can be handled by a child class. This is key to enabling
    *   background image loading.
    */
    class Image
    {
    public:

        /**
        *   \brief  Default constructor.
        */
        Image();

        /**
        *   \brief              Constructor for loading the base image data.
        *   \param  format [in] The format of the image data being loading into the image.
        *   \param  width  [in] The width of the image.
        *   \param  height [in] The height of the image.
        *   \param  data   [in] A pointer to the image data. Can be null.
        *
        *   \remarks
        *       This constructor will not generate mipmaps automatically. You can use GenerateMipmaps() afterwards to generate the mipmaps.
        */
        Image(unsigned int width, unsigned int height, ImageFormat format, const void* data = nullptr);

        /**
        *   \brief  Constructs the image and links it to the given file.
        *
        *   \remarks
        *       This constructor does not load the image data; it only links to the file. Use PullNextMipmap() straight after construction
        *       to load the first mipmap. Use PullAllMipmaps() to load every mipmap.
        */
        Image(const char* filename);

        /**
        *   \brief  Destructor.
        */
        virtual ~Image();

        /**
        *   \brief                Links the image to a file.
        *   \param  filename [in] The name of the file to link to.
        *   \return               True if the file is linked successfully; false otherwise.
        */
        bool LinkToFile(const char *filename);

        /// Determines whether or not the image is linked to a file.
        ///
        /// @return True if the image is linked to a file.
        bool IsLinkedToFile() const { return m_loader != nullptr; }


        /// Retrieves the name of the file as specified by the constructor or LinkToFile().
        const char* GetLinkedFileName() const;


        /// Pulls in the next mipmap, starting from the smallest and working it's way upwards to the largest.
        bool PullNextMipmap();

        /// Pulls every remaining mipmap.
        bool PullAllMipmaps();


        /**
        *   \brief                   Loads a mipmap from the attached file.
        *   \param  mipmapIndex [in] The index of the mipmap to load.
        *   \return                  True if the mipmap is loaded successfully; false otherwise.
        *
        *   \remarks
        *       The file name extension will be used in determine how the file should be loaded.
        */
        bool LoadMipmapFromFile(unsigned int mipmapIndex);

        /**
        *   \brief              Loads the base image data.
        *   \param  format [in] The format of the image data being loading into the image.
        *   \param  width  [in] The width of the image.
        *   \param  height [in] The height of the image.
        *   \param  data   [in] A pointer to the image data. Can be null.
        *   \return             True if the data is set correctly; false otherwise.
        *
        *   \remarks
        *       If the target can not be converted to the image's internal format (as specified in the constructor), false is returned.
        *       \par
        *       Load() will not generate mipmaps automatically. You can use GenerateMipmaps() to generate the mipmaps automatically.
        *       \par
        *       Load() will unload any previous mipmaps, sending the appropriate events through to the event handler.
        */
        bool Load(unsigned int width, unsigned int height, ImageFormat format, const void *data = nullptr);
        
        /// Sets a region of the data of the base mipmap.
        ///
        /// @param xPos   [in] The x position of the rectangle to update.
        /// @param yPos   [in] The y position of the rectangle to update.
        /// @param width  [in] The width of the rectangle to update.
        /// @param height [in] The height of the rectangle to update.
        /// @param data   [in] The new image data.
        ///
        /// @remarks
        ///     If the rectangle falls outside of the range of the image, this method will fail and return false.
        ///     @par
        ///     This does not update any mipmaps other than the base one.
        ///     @par
        ///     This will fail if the client-side data has been deallocated.
        bool SetSubData(int xPos, int yPos, unsigned int width, unsigned int height, const void* data);


        /**
        *   \brief  A helper function for automatically generating the mipmaps of the image.
        *   \return True if the mipmaps were generated successfully; false otherwise.
        *
        *   \remarks
        *       This method requires at least a base mipmap. If it does not have one, it will return false.
        */
        bool GenerateMipmaps();


        /**
        *   \brief  Retrieves the number of mipmaps that are making up this image.
        */
        size_t GetMipmapCount() const;

        /**
        *   \brief                   Retrieves a mipmap by it's index.
        *   \param  mipmapIndex [in] The index of the mipmap to retrieve.
        *   \return                  A pointer to the mipmap at the given index, or null if image does not have a mipmap at the given index.
        */
              Mipmap & GetMipmap(unsigned int mipmapIndex);
        const Mipmap & GetMipmap(unsigned int mipmapIndex) const;

        /**
        *   \brief  Retrieves the format of the image.
        */
        ImageFormat GetFormat() const;

        /**
        *   \brief  Determines whether or not the image is using a floating point format.
        */
        bool IsFloatingPointFormat() const;

        /**
        *   \brief  Retrieves the width of the image.
        */
        unsigned int GetWidth() const;

        /**
        *   \brief  Retrieves the height of the image.
        */
        unsigned int GetHeight() const;

        /// Retrieves a pointer to the data of the base mipmap.
        ///
        /// @remarks
        ///     If no mipmaps are loaded, returns nullptr.
        const void* GetBaseMipmapData() const;

        /// Retrieves the size of the data of the base mipmap.
        ///
        /// @remarks
        ///     If no mipmaps are loaded, this will return 0.
        size_t GetBaseMipmapDataSize() const;

        /**
        *   \brief  Vertically flips the image data on all mipmaps.
        *
        *   \remarks
        *       Any existing data will be flipped. Data will be automatically flipped when loaded; it's best to call this before loading
        *       any data for the sake of efficiency.
        */
        void FlipVertically();


        /// Retrieves the range of valid mipmap levels, as mipmap indices.
        /// @param baseLevel [in] A reference to the variable that receives the first valid mip level which will act as the base level.
        /// @param maxLevel  [in] A reference to the variable that will receive the maximum mip level.
        /// @return True if valid values were returned; false otherwise.
        ///
        /// @remarks
        ///     If this function returns false, <baseLevel> and <maxLevel> will both be set to 0.
        bool GetValidMipmapRange(size_t &baseLevel, size_t &maxLevel) const;



    protected:  // Events.

        /// Called after a mipmap has been modified.
        virtual void OnMipmapChanged(unsigned int mipmapIndex);

        /// Called when a mipmap has been loaded with data.
        virtual void OnMipmapCreated(unsigned int mipmapIndex);

        /// Called when a mipmaps data has been unloaded.
        virtual void OnMipmapDeleted(unsigned int mipmapIndex);

        /// Called when the file this image is linked to has changed. Changes are only detected when a Load*() function is called.
        virtual void OnFileChanged();

    private:

        /**
        *   \brief  Resets the image to it's default state.
        */
        void Reset();

        /// Resets the internal mipmap array.
        void ResetMipmaps(size_t newMipmapCount);

        /**
        *   \brief  Initialises the image based on the current image file loader.
        */
        bool InitFromImageLoader();


    private:

        /// The format of the image's data. This is something like RGBA8.
        ImageFormat m_format;

        /// The array of mipmaps making up this image. This will always contain a full mipmap array, even when only some are used. For example,
        /// a PNG file will start out with only a single valid mipmap (the base mipmap), but the array will still be filled with a full array of
        /// mipmaps. The empty/invalid mipmaps will not allocate any data until they are actually needed.
        GTLib::Vector<Mipmap> m_mipmaps;

        /// The loader for the linked file. This will be null if no file is linked to the image.
        ImageLoader* m_loader;

        /// Whether or not the image is vertically flipped.
        bool m_flipped;


    private:    // No copying, for now.
        Image(const Image &);
        Image & operator=(const Image &);
    };
}

#endif
