// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Texture2DLibrary
#define GT_Texture2DLibrary

namespace GT
{
    class Texture2D;
    class Context;

    /// Library class for Texture2Ds.
    ///
    /// Textures are a single-instantiation resource. This means we use an Acquire/Unacquire system. Note that
    /// acquiring and unacquiring is not thread safe. Keep this in mind when working with textures across multiple
    /// threads.
    class Texture2DLibrary
    {
    public:

        /// Constructor.
        Texture2DLibrary(Context &context);

        /// Destructor.
        ~Texture2DLibrary();


        /// Starts up the library.
        bool Startup();

        /// Shuts down the library.
        void Shutdown();



        /////////////////////////////////////////////////////
        // Acquiring/Unacquiring

        /// Acquires a texture object by it's name, relative to the data directory. See remarks.
        ///
        /// @param fileName       [in] The file name of the texture being loaded, relative to the data directory.
        /// @param makeRelativeTo [in] If 'fileName' is absolute, this will be used to turn it into a relative path.
        ///
        /// @return A pointer to the texture object, or null if the texture does not exist.
        ///
        /// @remarks
        ///     Textures are a single-instantiation resource. If the texture has already been loaded, that same texture object
        ///     will be returned.
        ///     @par
        ///     Each call to this function must match up to an Unacquire().
        ///     @par
        ///     All resources must have a relative path somewhere. If it doesn't, there will be errors with serialization. Thus,
        ///     this will return null if 'fileName' is absolute and 'makeRelativeTo' is null.
        Texture2D* Acquire(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Acquires an already-acquired texture object. This simply increments the internal reference count.
        ///
        /// @param texture [in] The texture being reacquired.
        ///
        /// @return <texture>. Done for consistency with Acquire(const char*).
        ///
        /// @remarks
        ///     It is safe to call this generically on textures that were not initially acquired by this library. In this case, the
        ///     acquire/unacquire operations will simply be ignored.
        Texture2D* Acquire(Texture2D* texture);


        /// Unacquires a texture.
        ///
        /// @param texture [in] A pointer to the texture to unacquire.
        void Unacquire(Texture2D* texture);


        /// Reloads the texture of the given file.
        ///
        /// @param file [in] The file name of the texture being reloaded.
        ///
        /// @return True if the image was reloaded successfully; false otherwise.
        ///
        /// @remarks
        ///     This will NOT load the texture if the texture has not already been loaded.
        bool Reload(const char* fileName);



        /////////////////////////////////////////////////////
        // System/Engine textures.

        /// Retrieves a 1x1 black texture.
        Texture2D* Get1x1BlackTexture();



        /////////////////////////////////////////////////////
        // Misc.

        /// Sets the default level of anisotropy to apply to newly acquired textures.
        void SetDefaultAnisotropy(unsigned int defaultAnisotropy);

        /// Sets the default minification filter.
        void SetDefaultMinFilter(TextureFilter filter);

        /// Sets the default magnification filter.
        void SetDefaultMagFilter(TextureFilter filter);

        /// Determines whether or not the given extension is a supported image extension.
        static bool IsExtensionSupported(const char* extension);



    private:

        /// A reference to the context that owns this library.
        Context &m_context;


        /// The list of loaded textures, indexed by file name.
        Dictionary<Texture2D*> m_loadedTextures;

        /// The default level of anistropy to apply to all textures.
        unsigned int m_defaultAnisotropy;

        /// The default minification filter to apply to textures.
        TextureFilter m_defaultMinFilter;

        /// The default magnification filter to apply to textures.
        TextureFilter m_defaultMagFilter;


        // Global Textures.
        Texture2D* Black1x1Texture;
    };
}

#endif