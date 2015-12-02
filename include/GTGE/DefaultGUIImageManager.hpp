// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_DefaultGUIImageManager
#define GT_DefaultGUIImageManager

#include <GTGE/GUI/GUIImageManager.hpp>
#include "Rendering/Texture2D.hpp"

namespace GT
{
    class DefaultGUIImageManager : public GUIImageManager
    {
    public:

        /// Constructor.
        DefaultGUIImageManager();
        
        /// Destructor.
        ~DefaultGUIImageManager();
        
        
        ///////////////////////////////////////////
        // Create
        
        /// GUIImageManager::CreateImage().
        GUIImageHandle CreateImage(unsigned int width, unsigned int height, GUIImageFormat format, void* data);
        GUIImageHandle CreateImage(unsigned int width, unsigned int height, GUIImageFormat format, const void* data);
        
        /// GUIImageManager::LoadImage().
        GUIImageHandle LoadImage(const char* filePath);
        
        
        ///////////////////////////////////////////
        // Delete
        
        /// GUIImageManager::DeleteImage().
        void DeleteImage(GUIImageHandle imageToDelete);
        
        
        ///////////////////////////////////////////
        // Update
        
        /// GUIImageManager::SetImageSubData().
        void SetImageSubData(GUIImageHandle image, unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, void* data);
        
        
        ///////////////////////////////////////////
        // Properties
        
        /// GUIImageManager::GetImageDimensions().
        void GetImageDimensions(GUIImageHandle image, unsigned int &widthOut, unsigned int &heightOut) const;
        
        /// GUIImageManager::GetImageFormat().
        GUIImageFormat GetImageFormat(GUIImageHandle image) const;



    private:
    };
}

#endif
