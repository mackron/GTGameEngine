// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_ImageManager_Default_hpp_
#define __GT_GUI_ImageManager_Default_hpp_

#include "GUIImageManager.hpp"
#include "../HandleManager.hpp"

namespace GT
{
    /// The default implementation of the image manager.
    ///
    /// This structure stores the raw image data in a simple block of memory. Mapping the image data returns a pointer to the raw data.
    class GUIImageManager_Default : public GUIImageManager
    {
    private:

        /// Simple structure containing the actual image data.
        struct Image
        {
            /// The handle.
            ImageHandle handle;

            /// The image width.
            unsigned int width;
            
            /// The image height.
            unsigned int height;

            /// The image format.
            ImageFormat format;

            /// A pointer to the raw image data.
            void* data;
        };


    public:

        /// Constructor.
        GUIImageManager_Default();

        /// Destructor.
        virtual ~GUIImageManager_Default();


        ///////////////////////////////////////////
        // Create
    
        /// ImageManager::CreateImage().
        ImageHandle CreateImage(unsigned int width, unsigned int height, ImageFormat format, void* data);
        ImageHandle CreateImage(unsigned int width, unsigned int height, ImageFormat format, const void* data);
    
        /// ImageManager::LoadImage().
        ImageHandle LoadImage(const char* filePath);
    
    
        ///////////////////////////////////////////
        // Delete
    
        /// ImageManager::DeleteImage().
        void DeleteImage(ImageHandle imageToDelete);



        ///////////////////////////////////////////
        // Mapping

        /// ImageManager::MapImageData().
        void* MapImageData(ImageHandle hImage);

        /// ImageManager::UnmapImageData().
        void UnmapImageData(ImageHandle hImage);
    
    
        ///////////////////////////////////////////
        // Update
    
        /// ImageManager::SetImageSubData().
        void SetImageSubData(ImageHandle hImage, unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, void* data);
    
    
        ///////////////////////////////////////////
        // Properties
    
        /// ImageManager::GetImageDimensions().
        void GetImageDimensions(ImageHandle hImage, unsigned int &widthOut, unsigned int &heightOut) const;
    
        /// ImageManager::GetImageFormat().
        ImageFormat GetImageFormat(ImageHandle hImage) const;




    private:

        /// Retrieves the pointer to the ImageData structure from the given handle.
        Image* GetImagePtr(ImageHandle hImage) const;


        ///////////////////////////////////////////
        // Static Helpers

        /// Helper function for retrieving the bytes-per-pixel for the given image format.
        ///
        /// @param format [in] The image format whose BPP is being determined.
        static unsigned int GetFormatBPP(ImageFormat format);

    
    private:

        /// The handle manager for tracking image handles.
        GT::HandleManager<ImageHandle, Image> handles;
    };
}

#endif
