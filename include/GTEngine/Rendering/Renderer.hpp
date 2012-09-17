
#ifndef __GTEngine_Renderer_hpp_
#define __GTEngine_Renderer_hpp_

#include "RCQueue.hpp"
#include "Texture2D.hpp"
#include "TextureCube.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "DrawModes.hpp"
#include "../Math.hpp"

#include <GTCore/Window.hpp>

namespace GTGUI
{
    class Server;
}

namespace GTEngine
{
    static const unsigned int ColourBuffer  = 0x01;
    static const unsigned int DepthBuffer   = 0x02;
    static const unsigned int StencilBuffer = 0x04;

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


    // TODO: Consider removing this. No longer needed, I think.
    /// Base class for event handlers that can be attached to the renderer to respond to certain events by the renderer.
    class RendererEventHandler
    {
    public:
        
        virtual ~RendererEventHandler();

        /// Called when the RC queues are swapped on the renderer.
        ///
        /// @remarks
        ///     This is a good place to do operations requiring thread safety such as framebuffer resizes.
        virtual void OnSwapRCQueues() {}
    };


    /**
    *   \brief  Class representing a renderer.
    *
    *   \section Caching Draw Calls
    *
    *   The renderer supports the notion of a draw-call cache. This is where draw calls are cached, as opposed to executing
    *   immediately. What this allows is one thread to fill the draw cache, while another empties it. Basically, it allows
    *   for multi-threaded rendering.
    *
    *   There are two caches - the back and the front. The back cache is filled from one thread. The front cache is emptied
    *   from another. The back and front caches are swapped with SwapCaches(). The back/front caches are analogous to the
    *   back/front buffers in OpenGL and Direct3D.
    *
    *   Here is the general workflow for caching draw calls.
    *       1) Create a RenderCommand object.
    *       2) Add the RenderCommand object to the renderer's back cache with AddToCache().
    *       3) When all draw calls have been cached, swap the back/front caches with SwapCache().
    *       4) Execute the draw calls on the front cache with ExecuteAndClearCache().
    *       5) Handle the OnDrawCallDone() renderer event to know when a RenderCommand object can be deallocated.
    *
    *   It's unintuitive, but for the sake of performance, none of the cache functions are thread-safe. In practice, this shouldn't
    *   actually be much of an issue.
    *
    *   AddToCache() operates on the back cache, whereas ExecuteAndClearCache() operates on the front cache. Use SwapCaches() to
    *   swap the back and front caches.
    */
    class Renderer
    {

    // The functions below are cross-renderer. They will be implemented in Renderer.cpp.
    public:

        /**
        *   \brief  Adds a RenderCommand object to the back cache.
        */
        static void AppendToBackBuffer(RenderCommand &cmd);

        /**
        *   \brief  Executes and clears every RC in the front RC queue.
        *
        *   \remarks
        *       This will also collect any garbage.
        */
        static void ExecuteFrontRCQueue();

        /**
        *   \brief  Clears the back RC queue.
        */
        static void ClearBackRCQueue();

        /**
        *   \brief  Swaps the back and front rendering RC queues.
        *
        *   \remarks
        *       You'll usually want to ensure ExecuteFrontRCQueue() has been executed before swapping the caches.
        *       \par
        *       This function will also clear the back RC queue.
        */
        static void SwapRCQueues();


        /**
        *   \brief                Sets the swap interval (controls v-sync).
        *   \param  interval [in] The swap interval to use. 0 means v-sync is disabled, 1 means v-sync is enabled.
        *
        *   \remarks
        *       The interval can also be set to -1, in which case it will use synchronization but will tear on late frames.
        */
        static void SetSwapInterval(int interval);

        /**
        *   \brief  Retrieves the swap interval.
        */
        static int GetSwapInterval();


       
        /////////////////////////////////////////////
        // Event Handling.

        /// Attaches an event handler to the renderer.
        ///
        /// @param eventHandler [in] A reference to the event handler to attach.
        static void AttachEventHandler(RendererEventHandler &eventHandler);

        /// Detaches an event handler from the renderer.
        ///
        /// @param eventHandler [in] A reference to the event handler to detach.
        static void DetachEventHandler(RendererEventHandler &eventHandler);


        /// Called the OnSwapRCQueues event on every attached event handler.
        static void OnSwapRCQueues();


    public:

        /// Initialises the renderer.
        static bool Startup();

        /**
        *   \brief  Unintialises the renderer.
        *
        *   \remarks
        *       Make sure garbage is collected with CollectGarbage().
        */
        static void Shutdown();


        /// Creates the game window.
        static GTCore::Window * CreateGameWindow();

        /// Determines whether or not the renderer uses upside down textures. True for OpenGL.
        static bool HasFlippedTextures();


    // General rendering operations. These will immediatly call the corresponding OpenGL/Direct3D calls. Make sure you are on
    // the rendering thread when calling these functions.
    public:

        /// Swaps the back and front colour buffers.
        static void SwapBuffers();

        /// Sets the rendering viewport, relative to the bottom-left corner.
        static void SetViewport(int x, int y, unsigned int width, unsigned int height);

        /// Retrieves the current viewport.
        static void GetViewport(int &x, int &y, unsigned int &width, unsigned int &height);

        /// Sets the scissor rectangle.
        static void SetScissor(int x, int y, unsigned int width, unsigned int height);

        /// Retrieves the current scissor rectangle.
        static void GetScissor(int &x, int &y, unsigned int &width, unsigned int &height);

        /// Clears the specified buffers.
        static void Clear(unsigned int bufferMask);


        /// Sets the colour to use for the colour buffer when it's cleared.
        static void ClearColour(float r, float g, float b, float a);

        /// Sets the value to use for the depth buffer after it's cleared.
        static void ClearDepth(float depth);

        /// Sets the value to use for the stencil buffer after it's cleared.
        static void ClearStencil(int stencil);


        /// Draws a vertex array.
        static void Draw(const VertexArray* vertexArray, DrawMode mode = DrawMode_Triangles);
        static void Draw(const float *vertices, const unsigned int *indices, size_t indexCount, const VertexFormat &format, DrawMode mode = DrawMode_Triangles);

        /// Enabled scissor testing.
        static void EnableScissorTest();

        /// Disables scissor testing.
        static void DisableScissorTest();

        /// Enables blending.
        static void EnableBlending();

        /// Disables blending.
        static void DisableBlending();

        /// Sets the blending function.
        static void SetBlendFunc(BlendFunc sourceFactor, BlendFunc destFactor);

        /// Sets the blending equation.
        static void SetBlendEquation(BlendEquation equation);

        /// Helper for enabling alpha blending.
        static void EnableAlphaBlending();


        /// Enables alpha testing.
        static void EnableAlphaTest();

        /// Disables alpha testing.
        static void DisableAlphaTest();

        /// Sets the alpha testing function.
        ///
        /// @param func [in] The new alpha test function.
        /// @param ref  [in] The reference value that incoming alpha values are compared to
        ///
        /// @remarks
        ///     The default value is AlphaTestFunc_Always.
        static void SetAlphaTestFunc(RendererFunction func, float ref);



        /// Enables colour writing.
        static void EnableColourWrites();

        /// Disables colour writing.
        static void DisableColourWrites();

        /// Enables depth testing.
        static void EnableDepthTest();
        
        /// Disable depth testing.
        static void DisableDepthTest();

        /// Enables depth writes.
        static void EnableDepthWrites();

        /// Disables depth writes.
        static void DisableDepthWrites();

        /// Sets the function to use with depth testing.
        ///
        /// @param func [in] The new depth function.
        static void SetDepthFunc(RendererFunction func);



        /// Enables stencil testing.
        static void EnableStencilTest();

        /// Disables stencil testing.
        static void DisableStencilTest();

        /// Sets the stencil mask.
        ///
        /// @param mask [in] The bit mask to enable and disable writing of individual bits in the stencil planes.
        static void SetStencilMask(unsigned int mask);

        /// Sets the stencil function and reference value for stencil testing.
        ///
        /// @param func [in] The function to use with stencil testing.
        /// @param ref  [in] The reference value.
        /// @param mask [in] Specifies a mask that is ANDed with both the reference value and the stored stencil value when the test is done.
        static void SetStencilFunc(RendererFunction func, int ref, unsigned int mask);

        /// Sets the action to take during stencil testing.
        ///
        /// @param stencilFail [in] The action to take when the stencil test fails.
        /// @param depthFail   [in] The action to take when the depth test fails.
        /// @param pass        [in] The action to take when both the depth and stenil tests pass.
        static void SetStencilOp(StencilOp stencilFail, StencilOp depthFail, StencilOp pass);

        /// Sets the stencil mask for the given faces.
        ///
        /// @param frontFace [in] Whether or not the write mask for the front face is updated.
        /// @param backFace  [in] Whether or not the write mask for the back face is updated.
        /// @param mask      [in] The new write mask.
        static void SetStencilMaskSeparate(bool frontFace, bool backFace, unsigned mask);

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



        /// Sets the face culling mode.
        ///
        /// @param  cullFront [in] Specifies whether or not front faces should be culled. A value of true will cull.
        /// @param  cullBack  [in] Specified whether or not back faces should be culled. A value of true will cull.
        static void SetFaceCulling(bool cullFront, bool cullBack);


        /// Enables sRGB.
        static void EnableSRGB();

        /// Disables sRGB.
        static void DisableSRGB();


        /**
        *   \brief                   Sets the current framebuffer for future rendering operations.
        *   \param  framebuffer [in] A pointer to the framebuffer to use for future rendering operations. Can be null; see remarks.
        *
        *   \remarks
        *       If framebuffer is set to nullptr, rendering operations will be performed on the main framebuffer.
        */
        static void SetFramebuffer(Framebuffer* framebuffer);


        /**
        *   \brief  Retrieves the current shader.
        */
        static Shader* GetCurrentShader();

        /**
        *   \brief              Sets the current shader.
        *   \param  shader [in] A pointer to the shader to bind. Can be null; see remarks.
        *
        *   \remarks
        *       Shaders in GTEngine are OpenGL-style where a single shader object is used for everything.
        *       \par
        *       If \c shader is null, fixed function will be used for that particular stage.
        */
        static void SetShader(Shader *shader);

        /// Updates the shader parameters of the current shader.
        ///
        /// @remarks
        ///     The renderer will apply shader parameters during SetShader(), but in order to change parameters on the current shader, an
        ///     application will need to "apply" those parameters by using this function instead.
        static void UpdateCurrentShaderParameters();

        /// Activates, enables and binds the textures of the current shader.
        static void BindCurrentShaderTextures();

        
        static void SetShaderParameter(const char *paramName, Texture2D*   texture);
        static void SetShaderParameter(const char* paramName, TextureCube* texture);
        static void SetShaderParameter(const char *paramName, float x);
        static void SetShaderParameter(const char *paramName, float x, float y);
        static void SetShaderParameter(const char *paramName, float x, float y, float z);
        static void SetShaderParameter(const char *paramName, float x, float y, float z, float w);
        static void SetShaderParameter(const char *paramName, const glm::mat2 &value);
        static void SetShaderParameter(const char *paramName, const glm::mat3 &value);
        static void SetShaderParameter(const char *paramName, const glm::mat4 &value);

        static inline void SetShaderParameter(const char *paramName, const glm::vec2 &value)
        {
            SetShaderParameter(paramName, value.x, value.y);
        }
        static void SetShaderParameter(const char *paramName, const glm::vec3 &value)
        {
            SetShaderParameter(paramName, value.x, value.y, value.z);
        }
        static void SetShaderParameter(const char *paramName, const glm::vec4 &value)
        {
            SetShaderParameter(paramName, value.x, value.y, value.z, value.w);
        }
        



        /**
        *   \brief  Sets the indices of the colour attachments to use as the render targets for future drawing operations.
        */
        static void SetDrawBuffers(size_t count, int* buffers);



        /**
        *   \brief  Draws a 2D GUI.
        */
        static void DrawGUI(const GTGUI::Server &gui);


        /// Performs a framebuffer blit.
        ///
        /// @remarks
        ///     This will blit the whole framebuffers.
        static void FramebufferBlit(Framebuffer* sourceFramebuffer, unsigned int sourceWidth, unsigned int sourceHeight, Framebuffer* destFramebuffer, unsigned int destWidth, unsigned int destHeight);




        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Resource Events.
        //
        // Because resources can be manipulated on a different thread to the renderer, we need to provide ways of letting the renderer
        // know when a resource needs to be updated on the rendering thread. The way we do this at the public level is via events, which
        // are declared below.
        //
        // The events below will add a special render command that will be executed before the normal render commands at the beginning
        // of each frame.

        /////////////////////////////////////////////
        // Texture2Ds.

        /// Called when a texture has been created.
        ///
        /// @param texture [in] The texture that was just created.
        ///
        /// @remarks
        ///     If the given texture is a face on a cube map, this will always be called straight after a call to OnTextureCubeCreated().
        static void OnTexture2DCreated(Texture2D &texture);

        /// Called when a texture has been deleted.
        ///
        /// @param texture [in] The texture that is being deleted.
        ///
        /// @remarks
        ///     If the given texture is a face on a cube map, this will be called before OnTextureCubeDeleted().
        static void OnTexture2DDeleted(Texture2D &texture);

        /// Called when the data of a texture has changed.
        ///
        /// @param texture [in] The texture whose data has changed.
        /// @parma mipmap  [in] The index of the mipmap that has changed.
        static void OnTexture2DMipmapChanged(Texture2D &texture, size_t mipmap);

        /// Called when the minification filter of a texture has changed.
        ///
        /// @param texture [in] The texture whose minification fitler has changed.
        static void OnTexture2DMinificationFilterChanged(Texture2D &texture);

        /// Called when the magnification filter of a texture has changed.
        ///
        /// @param texture [in] The texture whose magnification filter has changed.
        static void OnTexture2DMagnificationFilterChanged(Texture2D &texture);

        /// Called when the anisotropy of a texture has changed.
        ///
        /// @param texture [in] The texture whose anisotopy has changed.
        static void OnTexture2DAnisotropyChanged(Texture2D &texture);

        /// Called when the wrapping mode of a texture has changed.
        ///
        /// @param texture [in] The texture whose wrapping mode has changed.
        static void OnTexture2DWrapModeChanged(Texture2D &texture);



        /////////////////////////////////////////////
        // TextureCubes.

        /// Called when a cube texture has been created.
        ///
        /// @param texture [in] The texture that was just created.
        static void OnTextureCubeCreated(TextureCube &texture);

        /// Called when a cube texture has been deleted.
        ///
        /// @param texture [in] The texture that is being deleted.
        static void OnTextureCubeDeleted(TextureCube &texture);

        /// Called when the minification filter of a texture has changed.
        ///
        /// @param texture [in] The texture whose minification fitler has changed.
        static void OnTextureCubeMinificationFilterChanged(TextureCube &texture);

        /// Called when the magnification filter of a texture has changed.
        ///
        /// @param texture [in] The texture whose magnification filter has changed.
        static void OnTextureCubeMagnificationFilterChanged(TextureCube &texture);

        /// Called when the anisotropy of a texture has changed.
        ///
        /// @param texture [in] The texture whose anisotopy has changed.
        static void OnTextureCubeAnisotropyChanged(TextureCube &texture);



        /////////////////////////////////////////////
        // VertexArrays.

        /// Called when a vertex array is created.
        ///
        /// @param vertexArray [in] The vertex array that was just created.
        static void OnVertexArrayCreated(VertexArray &vertexArray);

        /// Called when a vertex array is deleted.
        ///
        /// @param vertexArray [in] The vertex array that is being deleted.
        static void OnVertexArrayDeleted(VertexArray &vertexArray);

        /// Called when the vertex data of a vertex array is modified.
        ///
        /// @param vertexArray [in] The vertex array whose vertex data has been modified.
        static void OnVertexArrayVertexDataChanged(VertexArray &vertexArray);

        /// Called when the index data of a vertex array is modified.
        ///
        /// @param vertexArray [in] The vertex array whose index data has been modified.
        static void OnVertexArrayIndexDataChanged(VertexArray &vertexArray);



        /////////////////////////////////////////////
        // Shaders.

        /// Called when a shader is created.
        ///
        /// @param shader [in] The shader that was just created.
        static void OnShaderCreated(Shader &shader);

        /// Called when a shader is deleted.
        ///
        /// @param shader [in] The shader that is being deleted.
        static void OnShaderDeleted(Shader &shader);



        /////////////////////////////////////////////
        // Framebuffers.

        /// Called when a framebuffer is created.
        ///
        /// @param framebuffer [in] The framebuffer that was just created.
        static void OnFramebufferCreated(Framebuffer &framebuffer);

        /// Called when a framebuffer is deleted.
        ///
        /// @param framebuffer [in] The framebuffer that is being deleted.
        static void OnFramebufferDeleted(Framebuffer &framebuffer);

        /// Called when a colour buffer is attached to a framebuffer.
        ///
        /// @param framebuffer [in] The framebuffer that has just had a colour buffer attached.
        /// @param index       [in] The index of the colour buffer that was just attached.
        static void OnColourBufferAttached(Framebuffer &framebuffer, size_t index);

        /// Called when a colour buffer is detached from a framebuffer.
        ///
        /// @param framebuffer [in] The framebuffer that has just had a colour buffer detached.
        /// @param index       [in] The index of the colour buffer that was just detached.
        static void OnColourBufferDetached(Framebuffer &framebuffer, size_t index);

        /// Called when the depth/stencil buffer is attached to a framebuffer.
        ///
        /// @param framebuffer [in] The framebuffer that has just had a depth/stencil buffer attached.
        static void OnDepthStencilBufferAttached(Framebuffer &framebuffer);

        /// Called when the depth/stencil buffer is detached from a framebuffer.
        ///
        /// @param framebuffer [in] The framebuffer that has just had a depth/stencil buffer detached.
        static void OnDepthStencilBufferDetached(Framebuffer &framebuffer);

        /// Called when a framebuffer needs to be checked.
        ///
        /// @param framebuffer [in] The framebuffer that is being checked.
        static void OnCheckFramebuffer(Framebuffer &framebuffer);



        /// Executes the front resources RC queues.
        static void ExecuteFrontResourceRCQueue();



    // Features support. These are set at initialisation time and can be called on any thread without syncs.
    public:

        /// Determines whether or not floating-point textures are supported. Needed for floating-point buffers for HDR, etc.
        static bool SupportFloatTextures();

        /// Determines whether or not framebuffer blitting is supported.
        static bool SupportFramebufferBlit();


        /// Retrieves the maximum number of colour attachments for framebuffers.
        static size_t GetMaxColourAttachments();

        /// Retrieves the maximum number of draw buffers for MRT.
        static size_t GetMaxDrawBuffers();



    public:

        /// A pointer to the back cache. Will never be null after the renderer has been successfully initialised.
        static RCQueue* BackRCQueue;

        /// A pointer to the front cache. Will never be null after the renderer has been successfully initialised.
        static RCQueue* FrontRCQueue;
        
        /// In many cases it is appropriate to use a pair of objects to manage the multithreaded environment correctly. For example, meshes use
        /// a pair of VertexArray's to store skinned geometry. One of them will be updated by the update thread while the other is used by the
        /// rendering thread. BackIndex is used to access the array item to use for updating on the non-rendering threads. This will always be
        /// 0 or 1. This is toggled between 0 and 1 during every call to SwapRCQueues().
        static size_t BackIndex;



    // Helpers.
    public:

        struct Helpers
        {
            static void DrawFullscreenQuad(Texture2D* texture);
        };
    };
}

#endif
