// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIImageManager
#define GT_GUIImageManager

#include <GTEngine/GUI/ImageManager.hpp>
#include "Rendering/Texture2D.hpp"

namespace GT
{
    class GUIImageManager : public GTGUI::GUIImageManager
    {
    public:

        /// Constructor.
        GUIImageManager();
        
        /// Destructor.
        ~GUIImageManager();
        
        
        ///////////////////////////////////////////
        // Create
        
        /// GTGUI::GUIImageManager::CreateImage().
        GTGUI::GUIImageHandle CreateImage(unsigned int width, unsigned int height, GTGUI::GUIImageFormat format, void* data);
        GTGUI::GUIImageHandle CreateImage(unsigned int width, unsigned int height, GTGUI::GUIImageFormat format, const void* data);
        
        /// GTGUI::GUIImageManager::LoadImage().
        GTGUI::GUIImageHandle LoadImage(const char* filePath);
        
        
        ///////////////////////////////////////////
        // Delete
        
        /// GTGUI::GUIImageManager::DeleteImage().
        void DeleteImage(GTGUI::GUIImageHandle imageToDelete);
        
        
        ///////////////////////////////////////////
        // Update
        
        /// GTGUI::GUIImageManager::SetImageSubData().
        void SetImageSubData(GTGUI::GUIImageHandle image, unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, void* data);
        
        
        ///////////////////////////////////////////
        // Properties
        
        /// GTGUI::GUIImageManager::GetImageDimensions().
        void GetImageDimensions(GTGUI::GUIImageHandle image, unsigned int &widthOut, unsigned int &heightOut) const;
        
        /// GTGUI::GUIImageManager::GetImageFormat().
        GTGUI::GUIImageFormat GetImageFormat(GTGUI::GUIImageHandle image) const;



    private:
    };
}

#endif
