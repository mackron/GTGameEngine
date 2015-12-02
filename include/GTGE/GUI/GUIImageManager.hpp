// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIImageManager
#define GT_GUIImageManager

#include <cstddef>

namespace GT
{
    typedef size_t GUIImageHandle;
    
    
    /// Enumerator for the different supported image formats.
    enum GUIImageFormat
    {
        GUIImageFormat_None,
        GUIImageFormat_Unknown = GUIImageFormat_None,
        
        GUIImageFormat_RGB8,
        GUIImageFormat_RGBA8,
        GUIImageFormat_A8,
    };
    
    
    /// Abstract class for managing the creation, deletion and updating of images.
    ///
    /// GTGUI does not load image data itself. Instead, it lets the owner library or application decide how it wants
    /// to handle loading. This system offers greater flexibility at the expense of a little bit more complexity.
    class GUIImageManager
    {
    public:
    
        /// Constructor.
        GUIImageManager();
        
        /// Destructor.
        virtual ~GUIImageManager();
        
        
        ///////////////////////////////////////////
        // Create
        
        /// Creates a new image of the given size, initialized with the given data.
        ///
        /// @param width  [in] The width of the image.
        /// @param height [in] The height of the image.
        /// @param format [in] The format of the data. Either RGB8 or RGBA8.
        /// @param data   [in] The image data.
        ///
        /// @return A handle to the new image. Can be null, which indicates an error.
        ///
        /// @remarks
        ///     If an error occurs, return null.
        ///     @par
        ///     The returned handle will be deleted with DeleteImage().
        ///     @par
        ///     Implementations of this method are free to mutate the provided data. This is mainly to allow implementations to
        ///     flip the image data (such as for use with OpenGL) without needing to do a full memory allocation.
        virtual GUIImageHandle CreateImage(unsigned int width, unsigned int height, GUIImageFormat format, void* data) = 0;
        virtual GUIImageHandle CreateImage(unsigned int width, unsigned int height, GUIImageFormat format, const void* data) = 0;
        
        /// Loads an image form a file.
        ///
        /// @param filePath [in] The path of the file to load.
        ///
        /// @remarks
        ///     If an error occurs, return null.
        ///     @par
        ///     The returned handle will be deleted with DeleteImage().
        virtual GUIImageHandle LoadImage(const char* filePath) = 0;
        
        
        ///////////////////////////////////////////
        // Delete
        
        /// Deletes an image that was created with CreateImage() or LoadImage().
        ///
        /// @param imageToDelete [in] A handle to the image to delete.
        virtual void DeleteImage(GUIImageHandle imageToDelete) = 0;
        
        
        ///////////////////////////////////////////
        // Update
        
        /// Sets the data of a region of the given image.
        ///
        /// @param image  [in] A handle to the image whose data is being set.
        /// @param xPos   [in] The x position of the rectangle region being updated.
        /// @param yPos   [in] The y position of the rectangle region being updated.
        /// @param width  [in] The width of the rectangle region being updated.
        /// @param height [in] The height of the rectangle region being updated.
        /// @param data   [in] The new data of the region.
        ///
        /// @remarks
        ///     Implementations of this method are free to mutate the provided data. This is mainly to allow implementations to
        ///     flip the image data (such as for use with OpenGL) without needing to do a full memory allocation.
        virtual void SetImageSubData(GUIImageHandle image, unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, void* data) = 0;
        
        
        ///////////////////////////////////////////
        // Properties
        
        /// Retrieves the dimensions of the given image.
        ///
        /// @param image     [in]  A handle to the image whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        virtual void GetImageDimensions(GUIImageHandle image, unsigned int &widthOut, unsigned int &heightOut) const = 0;
        void GetImageDimensions(GUIImageHandle image, float  &widthOut, float  &heightOut) const;
        void GetImageDimensions(GUIImageHandle image, double &widthOut, double &heightOut) const;
        
        /// Retrieves the format of the given image.
        ///
        /// @param image [in] A handle to the image whose format is being retrieved.
        virtual GUIImageFormat GetImageFormat(GUIImageHandle image) const = 0;
    };
}

#endif
