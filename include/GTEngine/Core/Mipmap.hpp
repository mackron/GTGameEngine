// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Mipmap
#define GT_Mipmap

#include "ImageFormats.hpp"

namespace GT
{
    class Mipmap
    {
    public:

        /// Default constructor.
        ///
        /// @remarks
        ///     A mipmap constructed with this constructor starts out invalid.
        Mipmap();

        /// Copy constructor.
        Mipmap(const Mipmap &other);

        /**
        *   \brief  Constructor.
        *
        *   \remarks
        *       The constructor does not make a copy of the data pointer. Instead it just references it directly. The
        *       destructor will free the data with free().
        */
        Mipmap(ImageFormat format, unsigned int width, unsigned int height, void *data);

        /**
        *   \brief  Destructor.
        */
        ~Mipmap();

        /**
        *   \brief  Vertically flips the mipmap's image data.
        */
        void FlipVertically();

        /**
        *   \brief  Calculates the size of a row in the mipmap.
        */
        size_t GetPitch() const;

        /// Retrieves the size of the data in bytes.
        ///
        /// @remarks
        ///     This will return a valid value event if the data pointer is nullptr.
        size_t GetDataSizeInBytes() const;

        /**
        *   \brief  Retrieves a pointer to the mipmap data.
        */
        const void* GetData() const { return this->data; }

        /**
        *   \brief  Deletes the mipmaps local image data, but keeps the mipmap valid.
        */
        void DeleteLocalData();

        /**
        *   \brief  Sets the pointer to the mipmap data.
        */
        void SetDataDirect(void* data);

        /**
        *   \brief  Resets the mipmap to it's default state.
        *
        *   \remarks
        *       This will free the image data and will mark the mipmap as invalid.
        */
        void Reset();


        /// Determines whether or not the mipmap is valid.
        bool IsValid() const { return this->width != 0 && this->height != 0; }


    public:

        /// The format of the mipmap's data.
        ImageFormat format;

        unsigned int width;     ///< The width of this mipmap.
        unsigned int height;    ///< The height of this mipmap.

        /// A pointer to the image data of this mipmap. This is allowed to be null. This size of the
        /// image data can be derived from \c format, \c width and \c height.
        void* data;


    private:
        Mipmap & operator=(const Mipmap &);
    };
}

#endif
