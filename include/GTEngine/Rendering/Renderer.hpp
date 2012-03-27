
#ifndef __GTEngine_Renderer_hpp_
#define __GTEngine_Renderer_hpp_

#include "RenderCommandBuffer.hpp"
#include "Texture2D.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "DrawModes.hpp"
#include "../Math.hpp"

#include <GTCore/Window.hpp>
#include <GTGUI/Server.hpp>


namespace GTEngine
{
    static const unsigned int ColourBuffer  = 0x01;
    static const unsigned int DepthBuffer   = 0x02;
    static const unsigned int StencilBuffer = 0x04;

    enum DepthFunc
    {
        DepthFunc_Never,
        DepthFunc_Less,
        DepthFunc_Equal,
        DepthFunc_LEqual,
        DepthFunc_Greater,
        DepthFunc_NotEqual,
        DepthFunc_GEqual,
        DepthFunc_Always,
    };

    enum BlendFunc
    {
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
        *   \brief  Executes and clears every draw call in the front cache.
        *
        *   \remarks
        *       This will also collect any garbage.
        */
        static void ExecuteFrontBuffer();

        /**
        *   \brief  Clears the back command buffer.
        */
        static void ClearBackBuffer();

        /**
        *   \brief  Swaps the back and front rendering command buffers.
        *
        *   \remarks
        *       You'll usually want to ensure ExecuteFrontBuffer() has been executed before swapping the caches.
        *       \par
        *       This function will also clear the back buffer.
        */
        static void SwapRCBuffers();


        /**
        *   \brief                Sets the swap interval (controls v-sync).
        *   \param  interval [in] The swap interval to use. 0 means v-sync is disabled, 1 means v-sync is enabled.
        *
        *   \remarks
        *       The interval can also be set to -1, in which case it will use synchronization but will tear on late frames.
        */
        static void SetSwapInterval(int interval);


        /*************************
        * Garbage Collection
        **************************/
        /**
        *   \brief                   Marks a resource for garbage collection. See remarks.
        *   \param  texture     [in] The texture that is being deleted and needs collecting.
        *   \param  framebuffer [in] The framebuffer that is being deleted and needs collecting.
        *   \param  shader      [in] The shader that is being deleted and needs collecting.
        *
        *   \remarks
        *       MarkForCollection() will be called in the destructors of the objects that are being deleted. Therefore, you
        *       must not assume the objects are still alive after returning from this function.
        *       \par
        *       This function must be thread-safe. It won't necessarily be called on the rendering thread.
        *       \par
        *       Internally, this function will take the 'rendererData' pointer of the resource and cache it. Then it will
        *       call Delete*Data() where it can then be deleted.
        */
        static void MarkForCollection(Texture2D* texture);
        static void MarkForCollection(Framebuffer* framebuffer);
        static void MarkForCollection(Shader* shader);
        static void MarkForCollection(VertexArray* vertexArray);

        /**
        *   \brief  Collects any garbage-collected resources.
        *
        *   \remarks
        *       This function must be run from the rendering thread.
        *       \par
        *       This function will call DeleteTexture2DData(), DeleteFramebufferData(), etc. These allow the renderer to delete
        *       the internal objects representing those resources.
        */
        static void CollectGarbage();


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

        /// Set the value to use for the depth buffer after it's cleared.
        static void ClearDepth(float depth);

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

        // TODO: Improve the way blending works.
        static void EnableAlphaBlending();

        /// Enables depth testing.
        static void EnableDepthTest();
        
        /// Disable depth testing.
        static void DisableDepthTest();

        static void EnableDepthWrites();
        static void DisableDepthWrites();

        static void SetDepthFunc(DepthFunc func);

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
        static Shader * GetCurrentShader();

        /**
        *   \brief              Sets the current shader.
        *   \param  shader [in] A pointer to the shader to bind. Can be null; see remarks.
        *
        *   \remarks
        *       Shaders in GTEngine are OpenGL-style where a single shader object is used for each stage.
        *       \par
        *       If \c shader is null, fixed function will be used for that particular stage.
        */
        static void SetShader(Shader *shader);

        static void SetShaderParameter(const char *paramName, Texture2D *texture);
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
        static void SetDrawBuffers(size_t count, int *buffers);


        /**
        *   \brief                    Deletes the data that was previously associated with a Texture2D.
        *   \param  rendererData [in] The rendererData pointer of the old Texture2D object.
        */
        static void DeleteTexture2DData(void *rendererData);
        static void DeleteFramebufferData(void *rendererData);
        static void DeleteShaderData(void *rendererData);
        static void DeleteVertexArrayData(void *rendererData);


        /**
        *   \brief  Draws a 2D GUI.
        */
        static void DrawGUI(const GTGUI::Server &gui);


    // Features support. These are set at initialisation time and can be called on any thread without syncs.
    public:

        /// Determines whether or not floating-point textures are supported. Needed for floating-point buffers for HDR, etc.
        static bool SupportFloatTextures();

        /// Retrieves the maximum number of colour attachments for framebuffers.
        static size_t GetMaxColourAttachments();

        /// Retrieves the maximum number of draw buffers for MRT.
        static size_t GetMaxDrawBuffers();


    public:

        /// A pointer to the back cache. Will never be null after the renderer has been successfully initialised.
        static RenderCommandBuffer *BackBuffer;

        /// A pointer to the front cache. Will never be null after the renderer has been successfully initialised.
        static RenderCommandBuffer *FrontBuffer;
        


    // Helpers.
    public:

        struct Helpers
        {
            static void DrawFullscreenQuad(Texture2D* texture);
        };
    };
}

#endif