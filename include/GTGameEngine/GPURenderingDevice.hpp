// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_hpp_
#define __GT_GE_GPURenderingDevice_hpp_

#include "Config.hpp"
#include "GPURenderingDeviceInfo.hpp"
#include "Graphics/GraphicsTypes.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

#include <GTLib/ResultCodes.hpp>
#include <GTLib/BasicBuffer.hpp>
#include <GTLib/Vector.hpp>

#include "Graphics/GraphicsInterface.hpp"

namespace GT
{
    inline size_t GetRowPitch(unsigned int width, TextureFormat format)
    {
        return (width * g_TextureFormatsBPPTable[format]) / 8;
    }

    inline size_t GetImageSizeInBytes(unsigned int width, unsigned int height, TextureFormat format)
    {
        return GetRowPitch(width, format) * height;
    }


    /// Class representing a rendering GPU device.
    ///
    /// A rendering device cannot be used across multiple threads.
    class GPURenderingDevice : public GraphicsInterface
    {
    public:

        /// Constructor.
        GPURenderingDevice(const GPURenderingDeviceInfo &info);

        /// Destructor.
        virtual ~GPURenderingDevice();


        /// Retrieves the device information.
        ///
        /// @param infoOut [out] A reference to the object that will receive the info.
        void GetInfo(GPURenderingDeviceInfo &infoOut) const;



        /// Initializes the rendering device.
        ///
        /// @return <0 if an error occurs, >=0 if successful.
        virtual ResultCode Startup() = 0;

        /// Uninitializes the rendering device.
        virtual void Shutdown() = 0;


        /// Retrieves a code representing the rendering API being used internally by the device.
        ///
        /// @return A code representing the rendering API being used internally by the device.
        virtual RenderingAPI GetRenderingAPI() const = 0;

        /// Retrieves the handedness used by the rendering API.
        ///
        /// @remarks
        ///     Direct3D is right-handed, whereas OpenGL is left handed.
        ///     @par
        ///     A front-facing triangle is wound clockwise for a right-handed coordinate system, and counter-clockwise for a left handed system. In addition, the
        ///     Z axis is such that looking forward, or into the screen, is the +Z direction for right-handed, and -Z for left handed.
        virtual GraphicsHandedness GetHandedness() const = 0;


        /// Sets the swap interval.
        ///
        /// @param swapInterval [in] The new swap interval. See remarks.
        ///
        /// @remarks
        ///     A value of 0 will disable v-sync. A value >0 will enable v-sync.
        virtual void SetSwapInterval(int swapInterval) = 0;


        ////////////////////////////////////////////
        // Drawing

        /// Renders the contents of the currently bound vertex buffer using the currently bound index buffer.
        ///
        /// @param indexCount         [in] The number of indices to draw.
        /// @param startIndexLocation [in] The zero-based location of the first index in the index buffer to start drawing from.
        /// @param baseVertex         [in] A value added to each index in the index buffer.
        ///
        /// @remarks
        ///     The \c indexCount argument refers to how many indices to iterate over when determining which primitives to draw. For example, when drawing
        ///     a single triangle, you would pass the number 3 (number of indices used to build the triangle).
        virtual void Draw(unsigned int indexCount, unsigned int startIndexLocation, int baseVertex) = 0;

        /// Renders the contents of the currently bound vertex buffer using the currently bound index buffer.
        ///
        /// @param indexCount         [in] The number of indices to draw.
        /// @param startIndexLocation [in] The zero-based location of the first index in the index buffer to start drawing from.
        /// @param baseVertex         [in] A value added to each index in the index buffer.
        /// @param instanceCount      [in] The number of instances to draw.
        /// @param baseInstance       [in] A value added to each instance index.
        ///
        /// @remarks
        ///     The \c indexCount argument refers to how many indices to iterate over when determining which primitives to draw. For example, when drawing
        ///     a single triangle, you would pass the number 3 (number of indices used to build the triangle).
        virtual void DrawInstanced(unsigned int indexCount, unsigned int startIndexLocation, int baseVertex, unsigned int instanceCount, unsigned int baseInstance) = 0;



        /////////////////////////////////////////////
        // State

        /// Binds the given texture view to the given slot.
        ///
        /// @param slotIndex    [in] The index of the slot to bind the texture to.
        /// @param hTextureView [in] A handle to the texture view to bind to the given slot.
        virtual void BindTexture(unsigned int slotIndex, HTextureView hTextureView) = 0;

        /// Binds the given sampler to the given slot.
        ///
        /// @param slotIndex [in] The index of the slot to bind the sampler to.
        /// @param hSampler  [in] A handle to the sampler to bind to the given slot.
        virtual void BindSampler(unsigned int slotIndex, HSampler hSampler) = 0;

        /// Binds the given constant buffers starting at the given slot index.
        ///
        /// @param firstSlotIndex [in] The index of the first slot to bind the buffers to.
        /// @param count          [in] The number of buffers to bind.
        /// @param buffers        [in] A pointer to the list of buffers to bind.
        /// @param offsets        [in] The offsets to use for each buffer.
        /// @param sizes          [in] The size in bytes of each buffer.
        virtual void BindConstantBuffer(unsigned int slotIndex, HBuffer buffer) = 0;
        virtual void BindConstantBuffer(unsigned int slotIndex, HBuffer buffer, size_t offset, size_t size) = 0;
        virtual void BindConstantBuffer(unsigned int slotIndex, size_t count, HBuffer* buffers, size_t* offsets, size_t* sizes) = 0;

        /// Binds the given framebuffer.
        ///
        /// @param hFramebuffer [in] A handle to the framebuffer to bind.
        virtual void BindFramebuffer(HFramebuffer hFramebuffer) = 0;


        /// Sets the vertex shader.
        ///
        /// @param hShader [in] The vertex shader to make current.
        virtual void SetVertexShader(HShader hShader) = 0;

        /// Sets the tessellation control shader (hull shader in Direct3D).
        ///
        /// @param hShader [in] The tessellation control shader to make current.
        virtual void SetTessellationControlShader(HShader hShader) = 0;

        /// Sets the tessellation evaluation shader (domain shader in Direct3D).
        ///
        /// @param hShader [in] The tessellation evaluation shader to make current.
        virtual void SetTessellationEvaluationShader(HShader hShader) = 0;

        /// Sets the geometry shader.
        ///
        /// @param hShader [in] The geometry shader to make current.
        virtual void SetGeometryShader(HShader hShader) = 0;

        /// Sets the fragment shader.
        ///
        /// @param hShader [in] The fragment shader to make current.
        virtual void SetFragmentShader(HShader hShader) = 0;

        /// Sets the compute shader.
        ///
        /// @param hShader [in] The compute shader to make current.
        virtual void SetComputeShader(HShader hShader) = 0;


        /////////////////////////////////////////////////////////////////////////////
        //
        // Stages
        //
        /////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////
        // Input-Assembler Stage

        /// Sets the primitive topology to use for future draw calls.
        ///
        /// @param topology [in] The new primitive topology to use for drawing.
        ///
        /// @remarks
        ///     The default topology is triangles.
        ///     @par
        ///     This is analogous to D3D11's IASetPrimitiveTopology().
        virtual void IASetPrimitiveTopology(PrimitiveTopology topology) = 0;

        /// Binds the given input layout object to the input-assembler stage.
        ///
        /// @para hInputLayout [in] A pointer to the input layout object to bind.
        ///
        /// @remarks
        ///     This is analogous to D3D11's IASetInputLayout().
        virtual void IASetInputLayout(HVSInputLayout hInputLayout) = 0;

        /// Binds the given vertex buffer to the given slot for the input-assembler stage.
        ///
        /// @param slotIndex [in] The slot index to bind the vertex buffer to. This cannot be larger than 31.
        /// @param buffer    [in] A pointer to the buffer to bind.
        /// @param stride    [in] The number of bytes making up the elements of a single vertex in the buffer. See remarks
        /// @param offset    [in] The number of bytes from the beginning of the buffer to start from.
        ///
        /// @remarks
        ///     This is analogous to D3D11's IASetVertexBuffers().
        virtual void IASetVertexBuffer(unsigned int slotIndex, HBuffer hBuffer, size_t stride, size_t offset) = 0;

        /// Binds the given index buffer for the input-assembler stage.
        ///
        /// @param buffer [in] A pointer to the buffer to bind.
        /// @param format [in] The format of the indices (8-bit, 16-bit or 32-bit).
        /// @param offset [in] The starting point within the buffer to start reading indices from.
        ///
        /// @remarks
        ///     This is analogous to D3D11's IASetIndexBuffer().
        virtual void IASetIndexBuffer(HBuffer hBuffer, IndexFormat format, size_t offset) = 0;



        /////////////////////////////////////////////
        // Rasterization Stage

        /// Sets the state for the rasterization state.
        ///
        /// @param hState [in] The rasterizer state to make current on the rasterization state.
        virtual void RSSetState(HRasterizerState hState) = 0;

        /// Sets the viewports for the rasterization stage.
        ///
        /// @param viewports [in] The list of GraphicsViewport objects.
        /// @param viewportCount [in] The number of viewports to set.
        ///
        /// @remarks
        ///     This is analogous to D3D11's RSSetViewports().
        ///     @par
        ///     If the rendering API does not support multiple viewports (such as OpenGL 2.1), this will only look at the first one in the list.
        virtual void RSSetViewports(GraphicsViewport* viewports, size_t viewportCount) = 0;

        /// Sets the scissor rectanges for the rasterization stage.
        ///
        /// @param scissorRects [in] The list of scissor rectangles.
        /// @param scissorCount [in] The number of scissor rectangles to set.
        virtual void RSSetScissorRects(ScissorRect* scissorRects, size_t scissorCounts) = 0;


        /////////////////////////////////////////////
        // Output Merger Stage

        /// Sets the depth/stencil state.
        ///
        /// @param hState     [in] The depth/stencil state to make current on the output merger stage.
        /// @param stencilRef [in] Reference value that is bitwise-and when performing the depth/stencil test.
        virtual void OMSetDepthStencilState(HDepthStencilState hState, unsigned int stencilRef) = 0;
        
        /// Sets the blend state.
        ///
        /// @param hState      [in] The blend state to make current on the output merger stage.
        virtual void OMSetBlendState(HBlendState hState) = 0;

        /// Sets the blend factor for when the blend parameter of the current state is set to BlendParameter_BlendFactor or BlendParameter_Inv_BlendFactor.
        ///
        /// @param blendFactor [in] The blend factor to use when the blend parameter is set to BlendParameter_BlendFactor or BlendParameter_Inv_BlendFactor.
        ///
        /// @remarks
        ///     The default blend factor is (0.0, 0.0, 0.0, 0.0).
        virtual void OMSetBlendFactor(float blendFactor[4]) = 0;

        /// Sets the sample mask.
        ///
        /// @param sampleMask [in] The new sample mask.
        ///
        /// @remarks
        ///     The default sample mask is 0xFFFFFFFF.
        virtual void OMSetSampleMask(uint32_t sampleMask) = 0;


        /////////////////////////////////////////////////////////////////////////////
        //
        // Object Creation and Deletion
        //
        /////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////
        // State Objects

        /// Creates a rasterizer state object.
        ///
        /// @param desc [in] A reference to the object that describes the rasterizer state.
        ///
        /// @return A handle to the new rasterizer state object, or 0 if there was an error.
        virtual HRasterizerState CreateRasterizerState(const RasterizerStateDesc &desc) = 0;

        /// Deletes the given reasterizer state object.
        ///
        /// @param hState [in] A handle to the rasterizer state object to delete.
        virtual void DeleteRasterizerState(HRasterizerState hState) = 0;


        /// Creates a depth/stencil state object.
        ///
        /// @param desc [in] A reference to the object that describes the depth/stencil state.
        ///
        /// @return A handle to the new depth/stencil state object, or 0 if there was an error.
        virtual HDepthStencilState CreateDepthStencilState(const DepthStencilStateDesc &desc) = 0;

        /// Deletes the given depth/stencil state object.
        ///
        /// @param hState [in] A handle to the depth/stencil state object to delete.
        virtual void DeleteDepthStencilState(HDepthStencilState hState) = 0;


        /// Creates a blend state object.
        ///
        /// @param desc [in] A reference to the object that describes the blend state.
        ///
        /// @return A handle to the new blend state object, or 0 if there was an error.
        virtual HBlendState CreateBlendState(const BlendStateDesc &desc) = 0;

        /// Deletes the given blend state object.
        ///
        /// @param hState [in] A handle to the state object to delete.
        virtual void DeleteBlendState(HBlendState hState) = 0;


        ////////////////////////////////////////////
        // Input Layout

        /// Creates a vertex input layout object that is used to describe the format of the input data for a vertex shader.
        ///
        /// @param hVertexShader   [in] A pointer to the shader program to create the layout object from.
        /// @param attribDesc      [in] An array of GPUVertexInputAttribLayoutDesc objects that describes each input variable.
        /// @param attribDescCount [in] The number of items in \c attribDesc.
        ///
        /// @return A handle to the new input layout object.
        ///
        /// @remarks
        ///     Direct3D 11 does not support 3-dimensional 16- and 8-bit integral formats. Therefore, this is not allowed.
        virtual HVSInputLayout CreateVSInputLayout(HShader hVertexShader, const VSInputAttribFormat* attribDesc, size_t attribDescCount) = 0;

        /// Deletes the given input layout object.
        ///
        /// @param hInputLayout [in] A handle to the vertex input layout object to delete.
        virtual void DeleteVSInputLayout(HVSInputLayout hInputLayout) = 0;


        ////////////////////////////////////////////
        // Shaders

        /// Compiles a shader from source, returning a compiled representation of the shader.
        ///
        /// @param source                 [in]  The shader source.
        /// @param sourceLength           [in]  The length of the shader source. Can be 0, in which case it is assumed the string is null-terminated.
        /// @param defines                [in]  An array of name/value pairs describing the #define's to include. NULL terminated.
        /// @param language               [in]  The shader language. See remarks.
        /// @param type                   [in]  The shader type being compiled (vertex, fragment, etc.)
        /// @param byteCodeOut            [out] A reference to the buffer object that will receive the compiled byte-code.
        /// @param messagesOut            [out] A reference to the buffer object that will receive the error/warning/message string. This is a null-terminated string in UTF-8 format.
        ///
        /// @remarks
        ///     The \c source paramter refers to the actual human-readable shader source code, and is <b>not</b> a file path.
        ///     @par
        ///     The possible values for \c language are dependant on the rendering API being used. Use GetSupportedShaderLanguageCount() and GetSupportedShaderLanguage() to determine which
        ///     shader languages are available.
        ///     @par
        ///     For compatibility with OpenGL, all shaders must use main() as the entry point.
        ///     @par
        ///     When using the OpenGL API, the shader source should NOT include the "#version <version>" statement - this will be included automatically based on \c target.
        ///     @par
        ///     The #include mechanism is not currently supported.
        ///     @par
        ///     The compiled data can be saved to disk, however be aware that the original source code will be included so this is not enough to obfuscate your shader code if you
        ///     feel that is important. This reason for this is that in some cases the shader code may need to be re-compiled internally in the event of something like a change in
        ///     driver versions or whatnot.
        virtual ResultCode CompileShader(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut) = 0;

        /// Determines whether or not the given shader language is supported by the rendering device.
        ///
        /// @param language [in] The shader language to check.
        virtual bool IsShaderLanguageSupported(ShaderLanguage language) const = 0;


        /// Creates a shader of the given type.
        ///
        /// @param shaderData     [in] A pointer ot the buffer containing the shader data.
        /// @param shaderDataSize [in] The size in bytes of the shader data.
        /// @param shaderType     [in] The type of shader being created (vertex, fragment, etc.)
        virtual HShader CreateShader(const void* shaderData, size_t shaderDataSize, ShaderType shaderType) = 0;

        /// Deletes the given shader.
        ///
        /// @param hShader [in] A handle to the shader to delete.
        virtual void DeleteShader(HShader hShader) = 0;


        ////////////////////////////////////////////
        // Buffers

        /// Creates a buffer object of the given type.
        ///
        /// @param type           [in] The buffer type (vertex, index, constant, etc.)
        /// @param usage          [in] The usage type of the buffer (immutable, dynamic, etc.)
        /// @param cpuAccessFlags [in] Flags specifying how the CPU is allowed to access the buffer's data. This affects map/unmap operations.
        /// @param sizeInBytes    [in] The size in bytes of the buffer.
        /// @param data           [in] A pointer to the buffer that contains the initial data.
        ///
        /// @return A handle to the buffer object, or 0 if there was an error.
        ///
        /// @remarks
        ///     When \c usage is BufferUsage_Immutable, \c data cannot be null.
        ///     @par
        ///     If \c cpuAccessFlags is set to BufferCPUAccess_None (0), all map and unmap operations will fail.
        ///     @par
        ///     The \c usage argument controls whether or not the buffer can be from the CPU side.
        ///       - Immutable - Can never be updated after creation from the CPU side.
        ///       - Default - SetBufferData()
        ///       - Dynamic - Write Mapping
        ///       - Staging - SetBufferData() / Write Mapping / Read Mapping
        ///     @par
        ///     A dynamic buffer cannot be used as an output for a pipeline stage.
        ///     @par
        ///     A staging buffer cannot be used as an input nor an output for a pipeline stage. 
        virtual HBuffer CreateBuffer(BufferType type, BufferUsage usage, BufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data) = 0;

        /// Deletes the given buffer object.
        ///
        /// @param hBuffer [in] A handle to the buffer to delete.
        virtual void DeleteBuffer(HBuffer hBuffer) = 0;


        /// Maps the given buffer's data for use on the CPU side.
        ///
        /// @param hBuffer [in] A handle to the buffer object whose data is being mapped.
        /// @param mapType [in] The type of mapping to perform. See remarks.
        ///
        /// @return A pointer to the buffer data, or null if an error occurs.
        ///
        /// @remarks
        ///     This does not, conceptually, allocate a buffer - it simply outputs a pointer to the internal buffer.
        ///     @par
        ///     The buffer must be unmapped with UnmapBuffer().
        ///     @par
        ///     This cannot be used when the buffer was created with BufferUsage_Immutable.
        ///     @par
        ///     When \c mapType is \c BufferMapType_Read or \c BufferMapType_ReadWrite, the buffer must have been created with \c BufferCPUAccess_Read.
        ///     @par
        ///     When \c mapType is \c BufferMapType_Write, \c BufferMapType_ReadWrite, \c BufferMapType_Write_Discard or \c BufferMapType_Write_NoOverwrite, the buffer must have been created with \c BufferCPUAccess_Write.
        ///     @par
        ///     When the buffer was created with Dynamic usage, the map type must be either \c BufferMapType_Write_Discard or \c BufferMapType_Write_NoOverwrite.
        virtual void* MapBuffer(HBuffer hBuffer, BufferMapType mapType) = 0;

        /// Unmaps the given buffer's data.
        ///
        /// @param hBuffer [in] A handle to the buffer object whose data is being unmapped.
        ///
        /// @remarks
        ///     This should be paired with a prior call to MapBuffer(). After this is called, the pointer returned by MapBuffer will become invalid.
        virtual void UnmapBuffer(HBuffer hBuffer) = 0;

        /// Sets the data for the given buffer.
        ///
        /// @param buffer        [in] A pointer to the buffer object whose data is being updated.
        /// @param offsetInBytes [in] The offset in bytes to start the data placement.
        /// @param sizeInBytes   [in] The size in bytes of the new data.
        /// @param data          [in] A pointer to the buffer containing the new data.
        ///
        /// @remarks
        ///     This will fail if the buffer was created with BufferUsage_Immutable.
        ///     @par
        ///     This will fail if the buffer is currently mapped.
        ///     @par
        ///     When using the Direct3D 11 API, this will use map/unmap internally when the buffer was created with BufferUsage_Dynamic.
        virtual void SetBufferData(HBuffer hBuffer, size_t offsetInBytes, size_t sizeInBytes, const void* data) = 0;



        ///////////////////////////////////////////
        // Textures

        /// Creates a texture.
        ///
        /// @param desc [in] A reference to the structure describing the properties of the texture.
        ///
        /// @return A handle to the texture, of 0 if there was an error.
        virtual HTexture CreateTexture(const TextureDesc &desc) = 0;

        /// Deletes the given texture.
        ///
        /// @param hTexture [in] A handle to the texture to delete.
        virtual void DeleteTexture(HTexture hTexture) = 0;

        /// Updates the data of the given texture.
        ///
        /// @param hTexture    [in] A handle to the texture whose image data is being updated.
        /// @param x           [in] The x position of the region to update.
        /// @param y           [in] The y position of the region to update. For 1D textures, this is the array slice to update.
        /// @param z           [in] The z position of the region to update. For 2D textures, this is the array slice to update.
        /// @param width       [in] The width of the region to update.
        /// @param height      [in] The height of the region to update. For 1D textures, this must be set to 1.
        /// @param depth       [in] The depth of the region to update. For 2D textures, this must be set to 1.
        /// @param mipmapLevel [in] The mipmap level to update.
        /// @param srcData     [in] A pointer to the new image data.
        virtual void UpdateTexture(HTexture hTexture, int x, int y, int z, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmapLevel, const void* srcData) = 0;


        /// Creates a texture view from the given texture.
        ///
        /// @param originalTexture [in] A handle to the texture the new texture view will be referencing.
        /// @param type            [in] The type of texture the texture view is representing.
        /// @param format          [in] The format of the image data.
        /// @param minLevel        [in] The index of the most detailed mipmap to use.
        /// @param numLevels       [in] The number of mipmap levels to include in the view.
        /// @param minLayer        [in] The minimum layer in the texture to use.
        /// @param numLayers       [in] The number of layers to include in the view.
        ///
        /// @return A handle to the new texture view object, of 0 if an error occurs.
        ///
        /// @return
        ///     If the texture type is not an array format, numLayers should never be greater than 1.
        virtual HTextureView CreateTextureView(HTexture hOriginalTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers) = 0;
        
        /// Deletes the given texture view object.
        ///
        /// @param hTextureView [in] A handle to the texture view object to delete.
        virtual void DeleteTextureView(HTextureView hTextureView) = 0;



        ///////////////////////////////////////////
        // Samplers

        /// Creates a sampler object from the given sampler description.
        ///
        /// @return A handle to the new sampler object, or 0 if an error occurs.
        virtual HSampler CreateSampler(const SamplerDesc &desc) = 0;

        /// Deletes the given sampler object.
        ///
        /// @param hSampler [in] A handle to the sampler object to delete.
        virtual void DeleteSampler(HSampler hSampler) = 0;


        ///////////////////////////////////////////
        // Framebuffers

        /// Creates an empty framebuffer object.
        ///
        /// @return A handle to the new framebuffer object.
        virtual HFramebuffer CreateFramebuffer() = 0;

        /// Deletes the given framebuffer object.
        ///
        /// @param hFramebuffer [in] A handle to the framebuffer to delete.
        virtual void DeleteFramebuffer(HFramebuffer hFramebuffer) = 0;

        /// Attaches a texture as a render target to the given framebuffer.
        ///
        /// @param hFramebuffer    [in] A handle to the framebuffer to attach the render target to.
        /// @param attachmentIndex [in] The index of the attachment point to attach the render target to.
        /// @param hTexture        [in] A handle to the texture to use as the render target.
        /// @param mipmapLevel     [in] The mipmap in the texture to draw to.
        /// @param arrayLayer      [in] The index of the array layer to draw to for array or 3D textures.
        virtual void AttachFramebufferRenderTarget(HFramebuffer hFramebuffer, unsigned int attachmentIndex, HTexture hTexture, unsigned int mipmapLevel, unsigned int arrayLayer) = 0;

        /// Attaches a depth/stencil target to the given framebuffer.
        ///
        /// @param hFramebuffer [in] A handle to the framebuffer to attach the depth/stencil target to.
        /// @param hTexture     [in] A handle to the texture to use as the depth/stencil target.
        /// @param mipmapLevel  [in] the mipmap in the texture to draw/read.
        /// @param arrayLayer   [in] the index of the array layer to draw to for array or 3D textures.
        virtual void AttachFramebufferDepthStencilTarget(HFramebuffer hFramebuffer, HTexture hTexture, unsigned int mipmapLevel, unsigned int arrayLayer) = 0;

        virtual void ClearFramebufferColor(HFramebuffer hFramebuffer, unsigned int attachmentIndex, float color[4]) = 0;
        virtual void ClearFramebufferDepth(HFramebuffer hFramebuffer, float depth) = 0;
        virtual void ClearFramebufferStencil(HFramebuffer hFramebuffer, uint8_t stencil) = 0;
        virtual void ClearFramebufferDepthStencil(HFramebuffer hFramebuffer, float depth, uint8_t stencil) = 0;



        //////////////////////////////////////////
        // Platform-Specific Methods

        

#if defined(GT_PLATFORM_WINDOWS)
        /// Win32: Creates a framebuffer for the given window so that it can be drawn to.
        ///
        /// @param hWnd                [in] A handle to the window whose framebuffer is being initialized.
        /// @param includeDepthStencil [in] Whether or not a depth/stencil buffer should also be created.
        virtual ResultCode InitWindowBuffers(HWND hWnd, bool includeDepthStencil) = 0;

        /// Win32: Uninitialises the framebuffer of the given window.
        ///
        /// @param hWnd [in] A handle to the window whose framebuffer is being unitialized.
        virtual void UninitWindowBuffers(HWND hWnd) = 0;

        /// Updates the size of the give window's framebuffer.
        ///
        /// @param hWnd [in] A handle to the window whose framebuffer needs to be resized.
        ///
        /// @remarks
        ///     This should be called in response to the window's size changing.
        virtual void ResizeWindowBuffers(HWND hWnd) = 0;

		/// Swaps the buffers for the current window.
		virtual void SwapWindowBuffers(HWND hWnd, int swapInterval) = 0;

		/// Win32: Sets the current window where rendering will be output to.
		///
		/// @param hWnd [in] A handle to the window to make current.
		///
		/// @return <0 if there was an error making the window current. >=0 if the window was set successfully.
		///
		/// @remarks
		///     Removing the current binding by pass NULL.
		///     @par
		///     InvalidWindowRenderTarget will be returned if the window was not first initialized with InitWindowFramebuffer().
		virtual ResultCode SetCurrentWindow(HWND hWnd) = 0;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif


    protected:

        /// Creates a shader binary buffer from the given information. 
        static ResultCode CreateShaderBinaryData(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, const void* binary, size_t binarySizeInBytes, int binaryVersion, GT::BasicBuffer &byteCodeOut);

        /// Takes the shader binary data created by CreateShaderBinaryData() and reads it's various components.
        static ResultCode ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const char* &sourceOut, size_t &sourceLengthOut, GTLib::Vector<ShaderDefine> &definesOut, ShaderLanguage &languageOut, ShaderType &typeOut, const void* &binaryOut, size_t &binarySizeOut, int &binaryVersionOut);
        static ResultCode ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const char* &sourceOut, size_t &sourceLengthOut, GTLib::Vector<ShaderDefine> &definesOut, ShaderLanguage &languageOut, ShaderType &typeOut);
        static ResultCode ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const void* &binaryOut, size_t &binarySizeOut);

        /// Validates the given list of vertex layout attributes.
        static bool IsInputLayoutValid(const VSInputAttribFormat* attribDesc, size_t attribDescCount);

    protected:

        /// The device info structure.
        GPURenderingDeviceInfo m_info;



    public:

        //////////////////////////////////////////////////////
        // Error Codes

        static const ResultCode RenderingAPINotSupported          = (1 << 31) | 0x00000001;
        static const ResultCode InvalidWindowRenderTarget         = (1 << 31) | 0x00000002;      //< Fired when a window is attempted to be made current, but the window was never initialized with a framebuffer.
        static const ResultCode ShaderTargetNotSupported          = (1 << 31) | 0x00000010;
        static const ResultCode ShaderTargetNotCompatible         = (1 << 31) | 0x00000011;
        static const ResultCode UnknownBufferType              = (1 << 31) | 0x00000020;
        static const ResultCode UnknownBufferUsage             = (1 << 31) | 0x00000021;
        static const ResultCode UnsupportedBufferType          = (1 << 31) | 0x00000022;
        static const ResultCode UnsupportedBufferUsage         = (1 << 31) | 0x00000023;
        static const ResultCode NoDataSpecifiedForImmutableBuffer = (1 << 31) | 0x00000024;
        static const ResultCode GPUBufferIsImmutable              = (1 << 31) | 0x00000025;
        static const ResultCode FailedToMapGPUBuffer              = (1 << 31) | 0x00000026;
        static const ResultCode UnknownBufferMapType           = (1 << 31) | 0x00000027;
            


    private:    // No copying.
        GPURenderingDevice(const GPURenderingDevice &);
        GPURenderingDevice & operator=(const GPURenderingDevice &);
    };
}

#endif