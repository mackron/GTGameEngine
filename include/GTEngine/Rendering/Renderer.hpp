// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Renderer_hpp_
#define __GTEngine_Renderer_hpp_

#include <GTCore/Window.hpp>
#include "Texture2D.hpp"
#include "TextureCube.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "Framebuffer.hpp"
#include "DrawModes.hpp"
#include "BufferTypes.hpp"
#include "BlendFunctions.hpp"
#include "BlendEquations.hpp"
#include "RendererFunctions.hpp"
#include "StencilOperations.hpp"
#include "PolygonModes.hpp"

#undef CreateWindow

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
    ///
    ///
    /// To simplify and encourage one-way throughput of the renderer, there is no way to retrieve the current state via the public API. This
    /// should be done at a higher level.
    class Renderer
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
        // Global State Changes

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


        /// Sets the current shader.
        ///
        /// @param shaderToMakeCurrent [in] A pointer to the shader to make current.
        ///
        /// @remarks
        ///     This will bind any referenced textures to their respective units.
        ///     @par
        ///     The given shader should not be null. We use a pointer here to keep it consistent with SetCurrentFramebuffer().
        static void SetCurrentShader(Shader* shaderToMakeCurrent);

        /// Sets the current framebuffer.
        ///
        /// @param framebufferToMakeCurrent [in] A pointer to the framebuffer to make current.
        ///
        /// @remarks
        ///     If the given framebuffer is null rendering operations will be performed on the main framebuffer.
        static void SetCurrentFramebuffer(Framebuffer* framebufferToMakeCurrent);


        /// Enables scissor testing.
        static void EnableScissorTest();

        /// Disables scissor testing.
        static void DisableScissorTest();


        /// Enables blending.
        static void EnableBlending();

        /// Disables blending.
        static void DisableBlending();

        /// Sets the blending function.
        static void SetBlendFunction(BlendFunc sourceFactor, BlendFunc destFactor);

        /// Sets the blending equation.
        static void SetBlendEquation(BlendEquation equation);

        /// Sets the colour to use with the constant-colour blending factors.
        static void SetBlendColour(float r, float g, float b, float a);
        static void SetBlendColour(const glm::vec4 &colour) { SetBlendColour(colour.x, colour.y, colour.z, colour.w); }
        


        /// Enables alpha testing.
        static void EnableAlphaTest();

        /// Disables alpha testing.
        static void DisableAlphaTest();

        /// Sets the alpha testing function.
        ///
        /// @param function [in] The new alpha test function.
        /// @param ref      [in] The reference value that incoming alpha values are compared to
        ///
        /// @remarks
        ///     The default value is AlphaTestFunc_Always.
        static void SetAlphaTestFunction(RendererFunction function, float ref);


        /// Enables colour writing.
        static void EnableColourWrites();

        /// Disables colour writing.
        static void DisableColourWrites();

        /// Enables depth writing.
        static void EnableDepthWrites();

        /// Disables depth writing.
        static void DisableDepthWrites();

        


        /// Enables depth testing.
        static void EnableDepthTest();

        /// Disables depth testing.
        static void DisableDepthTest();

        /// Sets the function to use with depth testing.
        ///
        /// @param function [in] The new depth function.
        static void SetDepthFunction(RendererFunction function);


        /// Enables stencil testing.
        static void EnableStencilTest();

        /// Disables stencil testing.
        static void DisableStencilTest();

        /// Sets the stencil mask.
        static void SetStencilMaskSeparate(bool frontFace, bool backFace, unsigned int mask);

        /// Sets the stencil function and reference value to use with stencil testing for the given faces.
        ///
        /// @param frontFace [in] Whether or not the function for the front face is updated.
        /// @param backFace  [in] Whether or not the function for the back face is updated.
        /// @param func      [in] The function to use with stencil testing.
        /// @param ref       [in] The reference value.
        /// @param mask      [in] Specifies a mask that is ANDed with both the reference value and the stored stencil value when the test is done.
        static void SetStencilFuncSeparate(bool frontFace, bool backFace, RendererFunction func, int ref, unsigned int mask);

        /// Sets the action to take during stencil testing for the given faces.
        ///
        /// @param frontFace   [in] Whether or not the action for the front face is updated.
        /// @param backFace    [in] Whether or not the action for the back face is updated.
        /// @param stencilFail [in] The action to take when the stencil test fails.
        /// @param depthFail   [in] The action to take when the depth test fails.
        /// @param pass        [in] The action to take when both the depth and stenil tests pass.
        static void SetStencilOpSeparate(bool frontFace, bool backFace, StencilOp stencilFail, StencilOp depthFail, StencilOp pass);


        // Stencil Helpers.
        static void SetStencilMask(unsigned int mask)                                        { SetStencilMaskSeparate(true, true, mask); }
        static void SetStencilFunc(RendererFunction func, int ref, unsigned int mask)        { SetStencilFuncSeparate(true, true, func, ref, mask); }
        static void SetStencilOp(StencilOp stencilFail, StencilOp depthFail, StencilOp pass) { SetStencilOpSeparate(true, true, stencilFail, depthFail, pass); }
        



        /// Sets the face culling mode.
        ///
        /// @param  cullFront [in] Specifies whether or not front faces should be culled. A value of true will cull.
        /// @param  cullBack  [in] Specifies whether or not back faces should be culled. A value of true will cull.
        static void SetFaceCulling(bool cullFront, bool cullBack);



        /// Enables polygon offset for the given polygon mode.
        ///
        /// @param mode [in] The polygon mode where polygon offset is being enabled.
        static void EnablePolygonOffset(PolygonMode mode);

        /// Disables polygon offset for the given polygon mode.
        ///
        /// @param mode [in] The polygon mode where polygon offset is being disabled.
        static void DisablePolygonOffset(PolygonMode mode);

        /// Sets the polygon mode (fill, wireframe or points).
        ///
        /// @param mode [in] The polygond mode to use.
        static void SetPolygonMode(bool frontFaces, bool backFaces, PolygonMode mode);

        /// Sets the polygon offset.
        ///
        /// @param factor [in] Scale factor.
        /// @param units  [in] A value to use for doing a constant scale factor.
        static void SetPolygonOffset(float factor, float units);


        /// Sets the indices of the colour attachments to use as the render targets for future drawing operations.
        ///
        /// @param count  [in] The number of buffers to use.
        /// @param buffer [in] The indices of the colour buffers to use.
        static void SetDrawBuffers(size_t count, int* buffers);


        /// Sets the swap interval (controls v-sync).
        ///
        /// @param  interval [in] The swap interval to use. 0 means v-sync is disabled, 1 means v-sync is enabled.
        ///
        /// @remarks
        ///     The interval can also be set to -1, in which case it will use synchronization but will tear on late frames.
        static void SetSwapInterval(int interval);



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
        /// @param vertices    [in] A pointer to the vertex data.
        /// @param vertexCount [in] The number of vertices defined in 'vertices'.
        /// @param indices     [in] A pointer to the buffer containing the index data.
        /// @param format      [in] The format of the vertex data.
        /// @param mode        [in] The primitives to draw (triangles or lines).
        ///
        /// @remarks
        ///     This function will copy the data. 'vertexCount' is only used in determining how much data to copy.
        static void Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, const VertexFormat &format, DrawMode mode = DrawMode_Triangles);

        /// Draws the elements defined in the given vertex data.
        ///
        /// @param vertices [in] A pointer to the vertex data.
        /// @param indices  [in] A pointer to the buffer containing the index data.
        /// @param format   [in] The format of the vertex data.
        /// @param mode     [in] The primitives to draw (triangles or lines).
        ///
        /// @remarks
        ///     Internally, this function needs to copy the data. To do this, it needs to have a reliable vertex count. This version of Draw() will
        ///     iterate over each index in determining how much data to copy. If the vertex count is already know, use the version of Draw() that
        ///     takes the vertex count.
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

        /// Pushes the vertex data of the given vertex array to the renderer.
        ///
        /// @param vertexArray [in] A reference to the vertex array whose data is getting pushed to the renderer.
        ///
        /// @remarks
        ///     This will immediately make a copy of the data, so it is safe to delete the local copy if needed.
        static void PushVertexArrayVertexData(const VertexArray &vertexArray);

        /// Pushes the index data of the given vertex array to the renderer.
        ///
        /// @param vertexArray [in] A reference to the vertex array whose data is getting pushed to the renderer.
        ///
        /// @remarks
        ///     This will immediately make a copy of the data, so it is safe to delete the local copy if needed.
        static void PushVertexArrayIndexData(const VertexArray &vertexArray);

        /// Pushes both the vertex and index data of the given vertex array to the renderer.
        ///
        /// @param vertexArray [in] A reference to the vertex array whose data is getting pushed to the renderer.
        ///
        /// @remarks
        ///     This will immediately make a copy of the data, so it is safe to delete the local copy if needed.
        static void PushVertexArrayData(const VertexArray &vertexArray);



        /// Creates a texture.
        ///
        /// @return A pointer to the new texture.
        ///
        /// @remarks
        ///     The returned texture will be completely empty. You can set the data on the returned object and then push that data to the
        ///     renderer with PushTexture2DData().
        static Texture2D* CreateTexture2D();
        static TextureCube* CreateTextureCube();

        /// Deletes a texture created with CreateTexture*().
        ///
        /// @param textureToDelete [in] A pointer to the texture to delete.
        static void DeleteTexture2D(Texture2D* textureToDelete);
        static void DeleteTextureCube(TextureCube* textureToDelete);

        /// Pushes the image data of the given 1D texture to the renderer.
        ///
        /// @param texture [in] A reference to the texture whose data is being pushed.
        /// @param mipmap  [in] The index of the mipmap whose data is being set. Set to -1 (default) to push every mipmap.
        ///
        /// @remarks
        ///     This will immediately make a copy of the data, so it is safe to delete the local copy is needed.
        //static void PushTexture1DData(const Texture1D &texture, int mipmap = -1);

        /// Pushes the image data of the given 2D texture to the renderer.
        ///
        /// @param texture [in] A reference to the texture whose data is being pushed.
        /// @param mipmap  [in] The index of the mipmap whose data is being set. Set to -1 (default) to push every mipmap.
        ///
        /// @remarks
        ///     This will immediately make a copy of the data, so it is safe to delete the local copy if needed.
        static void PushTexture2DData(const Texture2D &texture, int mipmap = -1);

        /// Pushes the image data of the given 3D texture to the renderer.
        ///
        /// @param texture [in] A reference to the texture whose data is being pushed.
        /// @param mipmap  [in] The index of the mipmap whose data is being set. Set to -1 (default) to push every mipmap.
        ///
        /// @remarks
        ///     This will immediately make a copy of the data, so it is safe to delete the local copy if needed.
        //static void PushTexture3DData(const Texture3D &texture, int mipmap = -1);

        /// Pushes the image data of the given cube map to the renderer.
        ///
        /// @param texture [in] A reference to the texture whose data is being pushed.
        ///
        /// @remarks
        ///     This will immediately make a copy of the data, so it is safe to delete the local copy if needed.
        ///     @par
        ///     Currently, only the base mipmap level can be updated manually. Mipmaps can be generated with Renderer::GenerateTextureCubeMipmaps().
        static void PushTextureCubeData(const TextureCube &texture);

        /// Sets the minification and magnification filter of the given texture.
        ///
        /// @param texture       [in] A reference to the texture whose filter is being set.
        /// @param minification  [in] The minification filter to use with the texture.
        /// @param magnification [in] The magnification filter to use with the texture.
        static void SetTexture2DFilter(const Texture2D &texture, TextureFilter minification, TextureFilter magnification);
        static void SetTextureCubeFilter(const TextureCube &texture, TextureFilter minification, TextureFilter magnification);

        /// Sets the level of anisotropic filter to use with the given texture.
        ///
        /// @param texture    [in] A reference to the texture.
        /// @param anisotropy [in] The level of anisotropic filter to use with the texture.
        static void SetTexture2DAnisotropy(const Texture2D &texture, unsigned int anisotropy);
        static void SetTextureCubeAnisotropy(const TextureCube &texture, unsigned int anisotropy);

        /// Sets the wrapping mode to use with the given texture.
        ///
        /// @param texture  [in] A reference to the texture.
        /// @param wrapMode [in] The wrapping mode to use with the given texture.
        static void SetTexture2DWrapMode(const Texture2D &texture, TextureWrapMode wrapMode);
        static void SetTextureCubeWrapMode(const TextureCube &texture, TextureWrapMode wrapMode);

        /// Sets the mipmap range to use with the given texture.
        ///
        /// @param texture   [in] A reference to the texture whose mipmapping range is being set.
        /// @param baseLevel [in] The base mipmap level to use with the texture.
        /// @param maxLevel  [in] The max mipmap level to use with the texture.
        static void SetTexture2DMipmapLevels(const Texture2D &texture, unsigned int baseLevel, unsigned int maxLevel);
        static void SetTextureCubeMipmapLevels(const TextureCube &texture, unsigned int baseLevel, unsigned int maxLevel);

        /// Generates mipmaps for the given texture.
        ///
        /// @param texture [in] A reference to the texture whose mipmaps are being generation.
        ///
        /// @remarks
        ///     This does not modify the client-side texture data. If the texture data is needed on the client side, you will need to do manualy
        ///     mipmap generation.
        static void GenerateTexture2DMipmaps(const Texture2D &texture);
        static void GenerateTextureCubeMipmaps(const TextureCube &texture);




        /// Creates a shader.
        ///
        /// @param vertexShaderSource   [in] The vertex shader source.
        /// @param fragmentShaderSource [in] The fragment shader source.
        /// @param geometryShaderSource [in] The geometry shader source.
        ///
        /// @remarks
        ///     Shaders are monolithic OpenGL style.
        ///     @par
        ///     The shader code will not be compiled until ExecuteCallCache() is called.
        ///     @par
        ///     The shader strings will be copied immediately so it is safe to delete the strings immediately after this function returns.
        static Shader* CreateShader(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource = nullptr);

        /// Deletes a shader created with CreateShader().
        ///
        /// @param shaderToDelete [in] The shader to delete.
        static void DeleteShader(Shader* shaderToDelete);

        /// Pushings any pending shader uniform parameters of the given shader.
        ///
        /// @param shader [in] The shadow whose pending uniforms are being pushed.
        ///
        /// @remarks
        ///     This will clear any pending uniforms.
        static void PushPendingUniforms(const Shader &shader);




        /// Creates an empty framebuffer object.
        ///
        /// @return A pointer to the new framebuffer object.
        ///
        /// @remarks
        ///     The returned framebuffer will be completely empty.
        static Framebuffer* CreateFramebuffer();

        /// Deletes a framebuffer that was created with CreateFramebuffer().
        ///
        /// @param framebufferToDelete [in] The framebuffer to delete.
        static void DeleteFramebuffer(Framebuffer* framebufferToDelete);

        /// Pushes the attached buffers of the given framebuffer to the renderer.
        ///
        /// @param framebuffer [in] The framebuffer whose attachments are being pushed.
        static void PushAttachments(const Framebuffer &framebuffer);




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