// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUI_ResourceManager
#define GT_GUI_ResourceManager

namespace GT
{
    typedef size_t HGUIImage;

    enum class GUIImageFormat
    {
        None,
        Unknown = None,

        RGB8,
        RGBA8,
        A8
    };

    /// The base class for resource mananagers.
    ///
    /// A resource manager is used to allow applications to user their own loading routines for resources. All the resource
    /// manager does is provides raw data to the GUI system and does not do anything like rendering or whatnot.
    ///
    /// Loading and unloading resources works on a per-context context basis, but it's possible for applications to use
    /// a single resource manager object for every GUI context. If this is desired, then the application will need to
    /// do their own reference counting and whatnot.
    class GUIResourceManager
    {
    public:

        /// Destructor.
        virtual ~GUIResourceManager();


        /// Loads an image.
        ///
        /// @param filePath [in] The path of the file to load.
        virtual HGUIImage LoadImage(const char* filePath) = 0;

        /// Unloads the given image.
        virtual void UnloadImage(HGUIImage hImage) = 0;


        /// Retrieves the size of the given image.
        virtual bool GetImageSize(HGUIImage hImage, unsigned int &widthOut, unsigned int &heightOut) const = 0;

        /// Retrieves the data format of the given image.
        virtual GUIImageFormat GetImageFormat(HGUIImage hImage) const = 0;

        /// Retrieves a pointer to the raw data of the given image.
        virtual const void* GetImageData(HGUIImage hImage) const = 0;
    };
}

#endif
