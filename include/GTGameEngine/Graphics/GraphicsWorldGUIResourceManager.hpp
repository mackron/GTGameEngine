// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GraphicsWorldGUIResourceManager
#define GT_GraphicsWorldGUIResourceManager

#include "../GUI/GUIResourceManager.hpp"
#include "GraphicsAssetResourceManager.hpp"

namespace GT
{
    class GraphicsWorldGUIResourceManager : public GUIResourceManager
    {
    public:

        /// Constructor
        GraphicsWorldGUIResourceManager();

        /// Destructor.
        ~GraphicsWorldGUIResourceManager();


        /// Sets the graphics asset resource manager to use for loading resource.
        void SetGraphicsAssetResourceManager(GraphicsAssetResourceManager* pGraphicsResourceManager);


        /// @copydoc GUIResourceManager::LoadImage()
        HGUIImage LoadImage(const char* filePath);

        /// @copydoc GUIResourceManager::LoadImage()
        void UnloadImage(HGUIImage hImage);

        /// @copydoc GUIResourceManager::LoadImage()
        bool GetImageSize(HGUIImage hImage, unsigned int &widthOut, unsigned int &heightOut) const;

        /// @copydoc GUIResourceManager::LoadImage()
        GUIImageFormat GetImageFormat(HGUIImage hImage) const;

        /// @copydoc GUIResourceManager::LoadImage()
        const void* GetImageData(HGUIImage hImage) const;


    private:

        /// A pointer to the graphics asset resource manager that will be used to load images. This is initialized to null and set with SetGraphicsAssetResourceManager().
        GraphicsAssetResourceManager* m_pGraphicsResourceManager;
    };
}

#endif
