// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Renderer2_hpp_
#define __GTEngine_Renderer2_hpp_

#include <GTCore/Window.hpp>

namespace GTEngine
{
    /// Static class for performing rendering operations.
    ///
    /// This is built as a static class to make things a bit easier to use. To switch implementations (OpenGL 3.3, D3D10, etc), just build
    /// the appropriate source file. This will be controlled with a pre-processor.
    ///
    /// There are two general parts to the renderer. The single-threaded part and the multi-threaded part. The single-threaded part calls
    /// the rendering API functions directly. In this case of OpenGL, it will call the gl*() functions directly, for example. These all
    /// fun directly on the thread that they were called.
    ///
    /// The second part is the multi-threaded part. This part is where most rendering operations are performed. These functions are not
    /// called immediately, but instead are cached until the next call to ExecuteCachedCalls().
    ///
    /// In a multithreaded environment, it's recommended that the rendering thread be run on the main thread, or at the very least the thead
    /// that created the window.
    ///
    /// There are two call caches internally. They are analogous to the the front and back buffers in a double-buffered environment. The
    /// back cache/queue is filled in a seperate thread (probably the update thread). The front cache is getting executed by the rendering
    /// thread. The caches need to swapped before ExecuteCachedCalls() is called. This is done with SwapCallCaches().
    ///
    /// Here is the general workflow:
    ///     Pre-frame stuff (event handling, synchronous operations, etc)
    ///     SwapCallCaches()
    ///     StartUpdateThread()     // <-- asynchronous, returns quickly.
    ///     ExecuteCachedCalls()
    ///     SwapBuffers()
    ///     WaitForUpdateThread()
    class Renderer2
    {
    public:

        /////////////////////////////////////////////////////////////
        // Direct Calls
        //
        // This section will execute the commands immediately on the calling thread. These should only ever be called on the rendering thread.

        /// Starts up the the renderer.
        ///
        /// @return True if the renderer is started up correctly; false otherwise.
        ///
        /// @remarks
        ///     This function asserts that the renderer was not previously initialised.
        static bool Startup();

        /// Shuts down the renderer.
        ///
        /// @remarks
        ///     This function asserts that the renderer was initialised.
        static void Shutdown();

        /// Creates a window that can be used as the main application window.
        ///
        /// @return A pointer to the new window.
        ///
        /// @remarks
        ///     This function asserts that the renderer has been successfully initialised with Startup().
        static GTCore::Window* CreateWindow();

        /// Sets the window that future rendering operations should take place on.
        ///
        /// @param window [in] A pointer to the window to make current.
        ///
        /// @remarks
        ///     This function asserts that the renderer has been successfully initialised with Startup().
        static void SetCurrentWindow(GTCore::Window* window);


        /// Swaps the call caches.
        ///
        /// @remarks
        ///     This will clear the new back call cache and leave it ready for filling by another thread.
        static void SwapCallCaches();

        /// Executes the front call cache.
        ///
        /// @remarks
        ///     This should be called after swapping the call caches. The idea is to do the following:
        ///         Fill the back call cache
        ///         Swap the caches (bring the back to the front) with SwapCallCaches()
        ///         Execute the front call cache (this function)
        ///
        ///     This will not swap the call caches.
        static void ExecuteCallCache();

        /// Swaps the back buffer with the front buffer, showing the final frame.
        ///
        /// @remarks
        ///     This will block if vertical synchronization is enabled.
        static void SwapBuffers();



        /////////////////////////////////////////////////////////////
        // Cached Calls
        //
        // This section contains the main rendering functions. These are not called immediately, but instead are cached until the next call to
        // ExecuteCallCache().





        /////////////////////////////////////////////////////////////
        // Support Queries
        //
        // These functions can be called on any thread. They return constants that define the level of support of various things provided by
        // the renderer. These settings will be initialised at startup and are simple accessors, so no need to worry about synchronization. An
        // example of functions in this section would be something like GetMaxAnisotropy().

        /// Determines if the renderer uses upside-down textures.
        ///
        /// @return True if the renderer uses upside down texture; false otherwise.
        ///
        /// @remarks
        ///     The main motivation to this function is the difference between OpenGL and Direct3D. OpenGL will store image data upside down
        ///     whereas Direct3D will store them right way up. Direct3D > OpenGL in this regard.
        static bool HasFlippedTextures();

        /// Retrieves the maximum number of colour attachments that can be attached to an FBO.
        ///
        /// @return The maximum number of colour buffers that can be attached to an FBO.
        static unsigned int GetMaxColourAttachments();

        /// Retrieves the maximum number of draw buffers that can be used with MRT.
        ///
        /// @return The maximum number of draw buffers that can be used with MRT.
        static unsigned int GetMaxDrawBuffers();

        /// Retrieves the maximum number of texture units that can be used simultaneously.
        ///
        /// @return The maximum number of texture units that can be used simultaneously.
        static unsigned int GetMaxTextureUnits();





        /////////////////////////////////////////////////////////////
        // Variables

        // TODO: Check to see if we can get rid of this variable.
        /// The index being used to identify the back call-cache.
        ///
        /// This is actually a carry-over from the previous renderer and may be made private later on.
        //size_t BackCallCacheIndex = 0;
    };
}

#endif