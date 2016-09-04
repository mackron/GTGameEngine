// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_ImageUtils
#define GT_ImageUtils

namespace GT
{
    namespace ImageUtils
    {
        /**
        *   \brief  Calculates the width of a mipmap based on it's index and the base width.
        */
        unsigned int CalculateMipmapWidth(unsigned int mipmapIndex, unsigned int baseWidth);

        /**
        *   \brief  Calculates the height of a mipmap based on it's index and the base width.
        */
        unsigned int CalculateMipmapHeight(unsigned int mipmapIndex, unsigned int baseHeight);

        /**
        *   \brief  Retrieves the total number of mipmaps that would be created based on the given image dimensions. This includes the base mipmap.
        */
        unsigned int CalculateMipmapCount(unsigned int baseWidth);
        unsigned int CalculateMipmapCount(unsigned int baseWidth, unsigned int baseHeight);
        unsigned int CalculateMipmapCount(unsigned int baseWidth, unsigned int baseHeight, unsigned int baseDepth);

        /**
        *   \brief  Calculates the size of a row based on the width and the format.
        */
        size_t CalculatePitch(unsigned int width, ImageFormat format);

        /// Calculates the size of an image based on its dimensions and format.
        ///
        /// @param width  [in] The width of the image.
        /// @param height [in] The height of the image.
        /// @param format [in] The format of the image.
        size_t CalculateDataSize(unsigned int width, unsigned int height, ImageFormat format);

        /**
        *   \brief  Makes a copy of the given image data.
        *
        *   \remarks
        *       If \c data is null, null will be returned.
        *       \par
        *       Deallocate the returned pointer with free() when it is no longer needed.
        */
        // TODO: Change the ordering of these parameters.
        void* CopyImageData(ImageFormat format, unsigned int width, unsigned int height, const void *data, bool flip = false);
        
        /// Copies the image data from one buffer to another, with the option of flipping.
        ///
        /// @param dstBuffer [in] A pointer to the destination buffer.
        /// @param srcBuffer [in] A pointer to the source buffer.
        /// @param width     [in] The width of the image.
        /// @param height    [in] The height of the image.
        /// @param flip      [in] Whether or not to flip the data.
        void CopyImageData(void* dstBuffer, const void* srcBuffer, unsigned int width, unsigned int height, ImageFormat format, bool flip = false);
        
        /// Flips the given image data.
        ///
        /// @param pitch  [in] The size in bytes of a row.
        /// @param height [in] The height of the image.
        /// @param data   [in] The image data.
        void FlipData(unsigned int pitch, unsigned int height, void* data);

        /// Retrieves the number of components in the given format. For example, RGB is 3 components.
        /// @param format [in] The image format whose component count is being retrieved.
        size_t GetFormatComponentCount(ImageFormat format);
    }
}

#endif
