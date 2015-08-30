// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUI_ResourceManager
#define GT_GUI_ResourceManager

#include <cstdint>
#include <GTLib/Vector.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/String.hpp>

#define GT_GUI_MAX_RESOURCE_PATH    256

namespace GT
{
    typedef size_t HGUIImage;

    enum class GUIImageFormat
    {
        None,
        Unknown = None,

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

        /// Constructor.
        GUIResourceManager();

        /// Destructor.
        virtual ~GUIResourceManager();


        /// Loads an image by the given path, or if it has already been loaded, returns a handle to the existing one.
        HGUIImage AcquireImage(const char* filePath, unsigned int &referenceCountOut);

        /// Unacquires the given image.
        void UnacquireImage(HGUIImage hImage, unsigned int &referenceCountOut);


        /// Encodes a resource file path.
        ///
        /// @param filePath [in] The file path to encode.
        ///
        /// @return A 32-bit token that can be passed to DecodeFilePath() to retrieve the decoded font family string.
        uint32_t EncodeFilePath(const char* filePath);

        /// Decodes a resource file path string from the given token.
        ///
        /// @param encodedFilePath [in] The font family token that was previously returned by EncodeFilePath().
        ///
        /// @return The decoded font family string, or null if the string could not be decoded.
        const char* DecodeFilePath(uint32_t encodedFilePath);


        /// Helper for unloading every resource.
        void UnloadAllResources();



        /////////////////////////////////////////////
        // Virtual Methods

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


    public:

        /// Structure representing an image that's already loaded in the cache.
        struct LoadedImage
        {
            /// Constructor.
            LoadedImage(HGUIImage hImageIn, const char* filePathIn)
                : hImage(hImageIn), filePath(), referenceCount(1)
            {
                strcpy_s(filePath, GT_GUI_MAX_RESOURCE_PATH, filePathIn);
            }

            /// Copy constructor.
            LoadedImage(const LoadedImage &other)
                : hImage(other.hImage), filePath(), referenceCount(other.referenceCount)
            {
                strcpy_s(filePath, GT_GUI_MAX_RESOURCE_PATH, other.filePath);
            }

            /// Assignment operator.
            LoadedImage & operator=(const LoadedImage &other)
            {
                if (this != &other)
                {
                    this->hImage         = other.hImage;
                    strcpy_s(filePath, GT_GUI_MAX_RESOURCE_PATH, other.filePath);
                    this->referenceCount = other.referenceCount;
                }

                return *this;
            }


            /// A pointer to the main font object.
            HGUIImage hImage;

            /// The path of the image.
            char filePath[GT_GUI_MAX_RESOURCE_PATH];

            /// The reference count. When this hits zero, it marks the end of the font.
            int referenceCount;
        };


        /// Retrieves a pointer to the already-loaded image that has the same path.
        ///
        /// @param filePath [in] The file path of the image.
        ///
        /// @return A pointer to the already-loaded image, or null if it isn't already loaded.
        ///
        /// @remarks
        ///     This does not create a new image - it simply looks at the already loaded images.
        ///     @par
        ///     This will increment the reference counter if non-null is returned.
        HGUIImage AcquireLoadedImage(const char* filePath, unsigned int &referenceCountOut);

        /// Retrieves a pointer to a LoadedImage structure that is mapped to the given image handle.
        ///
        /// @param hImage   [in]  A pointer to the image handle whose corresponding LoadedImage object is being retrieved.
        /// @param indexOut [out] A reference to the variable that will receive the index of the image.
        ///
        /// @return A pointer to the LoadedImage object that corresponds to the given image.
        LoadedImage* FindLoadedImage(HGUIImage hImage, size_t &indexOut);



    private:

        /// The list of loaded fonts.
        GTLib::Vector<LoadedImage> m_loadedImages;

        /// A map for mapping a font-family hash to it's actual string value.
        GTLib::Map<uint32_t, GTLib::String> m_encodedResourcePaths;
    };
}

#endif
