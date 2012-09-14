
#ifndef __GTEngine_Texture2DLibrary_hpp_
#define __GTEngine_Texture2DLibrary_hpp_

#include "Rendering/Texture2D.hpp"

namespace GTEngine
{
    /// Library class for Texture2Ds.
    ///
    /// Textures are a single-instantiation resource. This means we use an Acquire/Unacquire system. Note that
    /// acquiring and unacquiring is not thread safe. Keep this in mind when working with textures across multiple
    /// threads.
    class Texture2DLibrary
    {

    // Startup/Shutdown
    public:

        /// Starts up the library.
        static bool Startup();

        /// Shuts down the library.
        static void Shutdown();


    // Acquiring/Unacquiring
    public:

        /// Acquires a texture object by it's name, relative to the data directory. See remarks.
        ///
        /// @param fileName [in] The file name of the texture being loaded, relative to the data directory.
        ///
        /// @return A pointer to the texture object, or null if the texture does not exist.
        ///
        /// @remarks
        ///     Textures are a single-instantiation resource. If the texture has already been loaded, that same texture object
        ///     will be returned.
        ///     @par
        ///     Each call to this function must match up to an Unacquire().
        static Texture2D* Acquire(const char* fileName);

        /// Acquires an already-acquired texture object. This simply increments the internal reference count.
        ///
        /// @param texture [in] The texture being reacquired.
        ///
        /// @return <texture>. Done for consistency with Acquire(const char*).
        static Texture2D* Acquire(Texture2D* texture);


        /// Unacquires a texture.
        ///
        /// @param texture [in] A pointer to the texture to unacquire.
        static void Unacquire(const Texture2D* texture);


        /// Reloads the texture of the given file.
        ///
        /// @param file [in] The file name of the texture being reloaded.
        ///
        /// @return True if the image was reloaded successfully; false otherwise.
        ///
        /// @remarks
        ///     This will NOT load the texture if the texture has not already been loaded.
        static bool Reload(const char* fileName);



    /// System/Engine textures.
    public:

        /// Retrieves a 1x1 black texture.
        static Texture2D* Get1x1BlackTexture();


    /// Misc.
    public:

        /// Sets the default level of anisotropy to apply to newly acquired textures.
        static void SetDefaultAnisotropy(unsigned int defaultAnisotropy);

        /// Sets the default minification filter.
        static void SetDefaultMinFilter(TextureFilter filter);

        /// Sets the default magnification filter.
        static void SetDefaultMagFilter(TextureFilter filter);


        /// Determines whether or not the given extension is a supported image extension.
        static bool IsExtensionSupported(const char* extension);
    };
}

#endif