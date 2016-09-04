// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_ImageLoader
#define GT_ImageLoader

namespace GT
{
    /**
    *   \brief  Base class for image file loaders.
    *
    *   A class is inheritted from this class for each file format: PNG, DDS, etc.
    */
    class ImageLoader
    {
    public:

        /**
        *   \brief  Constructor.
        */
        ImageLoader(const char *fileName);

        /**
        *   \brief  Destructor.
        */
        virtual ~ImageLoader();


        /// Retrieves the file name that was passed to the constructor.
        const char* GetFileName() const;

        /// Retrieves the absolute file name that was passed to the constructor.
        const char* GetAbsolutePath() const;


        /**
        *   \brief  Virtual method to open the file and check it's validity.
        *
        *   \remarks
        *       This will not load any image data. It's only used to check the file is valid, and to set various
        *       properties like the width/height.
        *       \par
        *       Implementations should use \c absolutePath as the file to open.
        *       \par
        *       This will be called in ImageLoader::Create().
        */
        virtual bool Open() = 0;

        /**
        *   \brief  Retrieves information about the image file at the time it was last opened with Open().
        */
        virtual void GetImageInfo(ImageFileInfo &info) = 0;

        /**
        *   \brief                        Loads the data of a mipmap.
        *   \param  mipmapIndex     [in]  The index of the mipmap to load.
        *   \param  dest            [in]  A reference to the Mipmap object that will receive the loaded data.
        *   \param  imageHasChanged [out] A pointer to a boolean that will receive a value determining whether or not the image has changed since the last load.
        *   \return                       True if the mipmap was loaded successfully; false otherwise.
        */
        virtual bool LoadMipmap(unsigned int mipmapIndex, Mipmap &dest) = 0;

        /**
        *   \brief  Checks if the file has changed since it was first opened.
        *
        *   \remarks
        *       If this returns true, it will continue to do so until Open() is called again.
        */
        virtual bool HasFileChanged() const = 0;


    protected:

        /// The file name used to load the image file, as given in the constructor.
        String fileName;

        /// The absolute file path of the image file. This is set during the ImageLoader constructor and can be used by
        /// inheritted classes.
        String absolutePath;



    public: // Static portion.

        /**
        *   \brief                Creates a new loader object based on the given file name.
        *   \param  filename [in] The file name of the image that's being loaded.
        *
        *   \remarks
        *       This function will return nullptr if the image file is unsupported.
        */
        static ImageLoader* Create(const char* filename);

        /**
        *   \brief  Deletes a loader that was previously created with Create().
        */
        static void Delete(ImageLoader *loader);


        /// Determines whether or not the given extension is supported.
        ///
        /// @param extension [in] The extension. For example: "png", "dds".
        static bool IsExtensionSupported(const char* extension);


    private:    // No copying
        ImageLoader(const ImageLoader &);
        ImageLoader & operator=(const ImageLoader &);
    };
}

#endif