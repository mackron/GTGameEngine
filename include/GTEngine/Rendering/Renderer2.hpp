// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Renderer2_hpp_
#define __GTEngine_Renderer2_hpp_

#include <GTCore/Window.hpp>
#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/TextureCube.hpp>
#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/VertexArray.hpp>
#include <GTEngine/Rendering/Framebuffer.hpp>
#include <GTEngine/Rendering/DrawModes.hpp>

namespace GTEngine
{
    enum BufferTypes
    {
        BufferType_Colour  = (1 << 1),
        BufferType_Depth   = (1 << 2),
        BufferType_Stencil = (1 << 3)
    };

    enum RendererFunction
    {
        RendererFunction_Never,
        RendererFunction_Less,
        RendererFunction_Equal,
        RendererFunction_LEqual,
        RendererFunction_Greater,
        RendererFunction_NotEqual,
        RendererFunction_GEqual,
        RendererFunction_Always,
    };

    enum BlendFunc
    {
        BlendFunc_Zero,
        BlendFunc_One,

        BlendFunc_SourceColour,
        BlendFunc_OneMinusSourceColour,
        BlendFunc_SourceAlpha,
        BlendFunc_OneMinusSourceAlpha,

        BlendFunc_DestColour,
        BlendFunc_OneMinusDestColour,
        BlendFunc_DestAlpha,
        BlendFunc_OneMinusDestAlpha,

        BlendFunc_ConstantColour,
        BlendFunc_OneMinusConstantColour,
        BlendFunc_ConstantAlpha,
        BlendFunc_OneMinusConstantAlpha,

        BlendFunc_SourceAlphaSaturate,
    };

    enum BlendEquation
    {
        BlendEquation_Add,
        BlendEquation_Subtract,
        BlendEquation_ReverseSubtract,
        BlendEquation_Min,
        BlendEquation_Max
    };

    enum StencilOp
    {
        StencilOp_Keep,
        StencilOp_Zero,
        StencilOp_Replace,
        StencilOp_Increment,
        StencilOp_IncrementWrap,
        StencilOp_Decrement,
        StencilOp_DecrementWrap,
        StencilOp_Invert
    };

    enum PolygonMode
    {
        PolygonMode_Fill,
        PolygonMode_Line,
        PolygonMode_Point
    };



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
    ///
    ///
    /// To simplify and encourage one-way throughput of the renderer, there is no way to retrieve the current state via the public API. This
    /// should be done at a higher level.
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

        ///////////////////////////
        // Simple State Changes

        /// Sets the viewport.
        ///
        /// @param x      [in] The x position in pixels.
        /// @param y      [in] The y position in pixels.
        /// @param width  [in] The width of the viewport.
        /// @param height [in] The height of the viewport.
        static void SetViewport(int x, int y, unsigned int width, unsigned int height);

        /// Sets the scissor rectangle.
        ///
        /// @param x      [in] The x position in pixels.
        /// @param y      [in] The y position in pixels.
        /// @param width  [in] The width of the scissor rectangle.
        /// @param height [in] The height of the scissor rectangle.
        static void SetScissor(int x, int y, unsigned int width, unsigned int height);

        /// Sets the clear colour for when Clear() is called with BufferType_Colour.
        ///
        /// @param r [in] The red component.
        /// @param g [in] The green component.
        /// @param b [in] The blue component.
        /// @param a [in] The alpha component.
        static void SetClearColour(float r, float g, float b, float a);

        /// Sets the clear depth for when Clear() is called with BufferType_Depth.
        ///
        /// @param depth [in] The depth value to apply when clearing the depth buffer.
        static void SetClearDepth(float depth);

        /// Sets the stencil value to apply when Clear() is called with BufferType_Stencil.
        ///
        /// @param stencil [in] The stencil value to apply when clearing the stencil buffer.
        static void SetClearStencil(int stencil);



        ///////////////////////////
        // Drawing

        /// Clears the back buffer.
        ///
        /// @param bufferMask [in] A bitfield specifying which buffers to clear.
        ///
        /// @remarks
        ///     'bufferMask' should be bitwise-or'd with BufferType_Colour, BufferType_Depth and BufferType_Stencil.
        static void Clear(unsigned int bufferMask);

        /// Draws the elements defined in the given vertex array.
        ///
        /// @param vertexArray [in] A reference to the vertex array to draw.
        /// @param mode        [in] The primitives to draw (triangles or lines).
        static void Draw(const VertexArray &vertexArray, DrawMode mode = DrawMode_Triangles);
        
        /// Draws the elements defined in the given vertex data.
        ///
        /// @param vertices [in] A pointer to the vertex data.
        /// @param indices  [in] A pointer to the buffer containing the index data.
        /// @param format   [in] The format of the vertex data.
        /// @param mode     [in] The primitives to draw (triangles or lines).
        static void Draw(const float* vertices, const unsigned int* indices, size_t indexCount, const VertexFormat &format, DrawMode mode = DrawMode_Triangles);



        ///////////////////////////
        // Resources
        //
        // Everything here is synchronized and can be called on any thread. The objects themselves will not be created until the next time
        // ExecuteCallCache() is called. Objects can be deleted at any time from any thread.
        //
        // The functions for creating and deleteing resources are slightly different to the rest. When ExecuteCallCache() is called, the
        // creation functions will be called first and deletion functions will be called last. This ensures all resources stay alive for
        // at least one frame. A side affect does occur, though. With anything that is bound such as programs and framebuffers, you can not
        // rely on a Delete function for unbinding - it must be unbound manually.

        /// Creates a vertex array.
        ///
        /// @param usage  [in] The usage type of the vertex array (static or dynamic).
        /// @param format [in] The format of the vertices.
        ///
        /// @return A pointer to the new vertex array.
        static VertexArray* CreateVertexArray(VertexArrayUsage usage, const VertexFormat &format);

        /// Deletes a vertex array created by CreateVertexArray().
        ///
        /// @param vertexArrayToDelete [in] A pointer to the vertex array to delete.
        static void DeleteVertexArray(VertexArray* vertexArrayToDelete);

        /// Updates the given vertex array's vertex data on the rendering side.
        ///
        /// @param vertexArray [in] A reference to the vertex array whose index data is being updated.
        static void UpdateVertexArrayVertexData(VertexArray &vertexArray);

        /// Updates the given vertex array's index data on the rendering side.
        ///
        /// @param vertexArray [in] A reference to the vertex array whose index data is being updated.
        static void UpdateVertexArrayIndexData(VertexArray &vertexArray);



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
    };
}

#endif