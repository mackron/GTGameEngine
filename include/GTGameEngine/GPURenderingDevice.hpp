// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_hpp_
#define __GT_GE_GPURenderingDevice_hpp_

#include "Config.hpp"
#include "GPURenderingDeviceInfo.hpp"
#include "Rendering/RenderingTypes.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

#include <GTLib/ResultCodes.hpp>
#include <GTLib/BasicBuffer.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    /// Class representing a rendering GPU device.
    ///
    /// A rendering device cannot be used across multiple threads.
    class GPURenderingDevice
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


        /// Retrieves the generation number for this renderer.
        ///
        /// @return An integer representing the generation of rendering technologies this rendering device supports. See remarks.
        ///
        /// @remarks
        ///     This is used in determining whether or not this device can be cased to GPURenderingDevice_Gen1 or GPURenderingDevice_Gen2. 
        ///     @par
        ///     If this returns 1, the rendering device supports Direct3D 9 / OpenGL 2.1 class of functionality.
        ///     @par
        ///     If this returns 2, the rendering device supports Direct3D 11 / OpenGL 4.5 class of functionality.
        virtual unsigned int GetGeneration() const = 0;

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
        virtual GPUHandedness GetHandedness() const = 0;


        /// Sets the swap interval.
        ///
        /// @param swapInterval [in] The new swap interval. See remarks.
        ///
        /// @remarks
        ///     A value of 0 will disable v-sync. A value >0 will enable v-sync.
        virtual void SetSwapInterval(int swapInterval) = 0;


        ////////////////////////////////////////////
        // Drawing

        /// Clears the current framebuffer with the given color.
        ///
        /// @param r [in] The red component.
        /// @param g [in] The green component.
        /// @param b [in] The blue component.
        /// @param a [in] The alpha component.
        virtual void ClearColor(float r, float g, float b, float a) = 0;

        /// Clears the depth/stencil buffer of the current framebuffer with the given values.
        ///
        /// @param clearFlags [in] Flags specifying which buffers to clear.
        /// @param depth      [in] The value to set for the depth buffer.
        /// @param stencil    [in] The value to set for the stencil buffer.
        virtual void ClearDepthStencil(GPUClearFlag clearFlags, float depth, uint8_t stencil) = 0;


        /// Renders the contents of the currently bound vertex buffer using the currently bound index buffer.
        ///
        /// @param indexCount         [in] The number of indices to draw.
        /// @param startIndexLocation [in] The zero-based location of the first index in the index buffer to start drawing from.
        ///
        /// @remarks
        ///     The \c indexCount argument refers to how many indices to iterate over when determining which primitives to draw. For example, when drawing
        ///     a triangle, you would pass the number 3 (number of indices used to build the triangle).
        virtual void Draw(unsigned int indexCount, unsigned int startIndexLocation) = 0;



        /////////////////////////////////////////////
        // State

        /// Sets the current shader program.
        ///
        /// @param hShaderProgram [in] A reference to the shader program object to make current.
        virtual void SetCurrentShaderProgram(HShaderProgram hShaderProgram) = 0;

        /// Binds the given texture view to the given slot.
        ///
        /// @param hTextureView [in] A handle to the texture view to bind to the given slot.
        /// @param slotIndex    [in] The index of the slot to bind the texture to.
        virtual void BindTexture(HTextureView hTextureView, unsigned int slotIndex) = 0;

        /// Binds the given sampler to the given slot.
        ///
        /// @param hSampler  [in] A handle to the sampler to bind to the given slot.
        /// @param slotIndex [in] The index of the slot to bind the sampler to.
        virtual void BindSampler(HSampler hSampler, unsigned int slotIndex) = 0;


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
        virtual void IASetPrimitiveTopology(GPUPrimitiveTopology topology) = 0;

        /// Binds the given input layout object to the input-assembler stage.
        ///
        /// @para hInputLayout [in] A pointer to the input layout object to bind.
        ///
        /// @remarks
        ///     This is analogous to D3D11's IASetInputLayout().
        virtual void IASetInputLayout(HInputLayout hInputLayout) = 0;

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
        virtual void IASetIndexBuffer(HBuffer hBuffer, GPUIndexFormat format, size_t offset) = 0;



        /////////////////////////////////////////////
        // Rasterization Stage

        /// Sets the state for the rasterization state.
        ///
        /// @param hState [in] The rasterizer state to make current on the rasterization state.
        virtual void RSSetState(HRasterizerState hState) = 0;

        /// Sets the viewports for the rasterization stage.
        ///
        /// @param viewports [in] The list of GPUViewport objects.
        /// @param viewportCount [in] The number of viewports to set.
        ///
        /// @remarks
        ///     This is analogous to D3D11's RSSetViewports().
        ///     @par
        ///     If the rendering API does not support multiple viewports (such as OpenGL 2.1), this will only look at the first one in the list.
        virtual void RSSetViewports(GPUViewport* viewports, size_t viewportCount) = 0;



        /////////////////////////////////////////////
        // Output Merger Stage

        /// Sets the depth/stencil state.
        ///
        /// @param hState     [in] The depth/stencil state to make current on the output merger state.
        /// @param stencilRef [in] Reference value that is bitwise-and when performing the depth/stencil test.
        virtual void OMSetDepthStencilState(HDepthStencilState hState, unsigned int stencilRef) = 0;



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
        virtual HRasterizerState CreateRasterizerState(const GPURasterizerStateDesc &desc) = 0;

        /// Decrements the internal reference counter of the given rasterizer state and deletes the object if it hits 0.
        ///
        /// @param hState [in] A handle to the rasterizer state object to release.
        ///
        /// @remarks
        ///     This is thread safe.
        ///     @par
        ///     It is possible that the internal API-specific data structure may not be deleted until the next flush or buffer swap in the interest of
        ///     synchronization.
        virtual void ReleaseRasterizerState(HRasterizerState hState) = 0;

        /// Increments the internal reference counter of the given rasterizer state object.
        ///
        /// @param hState [in] A pointer to the rasterizer state object to hold.
        ///
        /// @remarks
        ///     This is thread safe.
        virtual void HoldRasterizerState(HRasterizerState hState) = 0;


        /// Creates a depth/stencil state object.
        ///
        /// @param desc [in] A reference to the object that describes the depth/stencil state.
        ///
        /// @return A handle to the new depth/stencil state object, or 0 if there was an error.
        virtual HDepthStencilState CreateDepthStencilState(const GPUDepthStencilStateDesc &desc) = 0;

        /// Decrements the internal reference counter fo the given depth/stencil state and deletes the object if it hits 0.
        ///
        /// @param hState [in] A handle to the depth/stencil state object to release.
        ///
        /// @remarks
        ///     This is thread safe.
        ///     @par
        ///     It is possible that the internal API-specific data structure may not be deleted until the next flush or buffer swap in the interest of
        ///     synchronization.
        virtual void DeleteDepthStencilState(HDepthStencilState hState) = 0;

        /// Increments the internal reference counter of the given rasterizer state object.
        ///
        /// @param hState [in] A handle to the depth/stencil state object to hold.
        ///
        /// @remarks
        ///     This is thread safe.
        virtual void HoldDepthStencilState(HDepthStencilState hState) = 0;



        ////////////////////////////////////////////
        // Input Layout

        /// Creates a vertex input layout object that is used to describe the format of the input data for a vertex shader.
        ///
        /// @param hShaderProgram  [in] A pointer to the shader program to create the layout object from.
        /// @param attribDesc      [in] An array of GPUVertexInputAttribLayoutDesc objects that describes each input variable.
        /// @param attribDescCount [in] The number of items in \c attribDesc.
        ///
        /// @return A handle to the new input layout object.
        virtual HInputLayout CreateInputLayout(HShaderProgram hShaderProgram, const GPUInputLayoutAttribDesc* attribDesc, size_t attribDescCount) = 0;

        /// Decrements the internal reference counter fo the given depth/stencil state and deletes the object if it hits 0.
        ///
        /// @param hInputLayout [in] A handle to the vertex input layout object to release.
        ///
        /// @remarks
        ///     This is thread safe.
        ///     @par
        ///     It is possible that the internal API-specific data structure may not be deleted until the next flush or buffer swap in the interest of
        ///     synchronization.
        virtual void ReleaseInputLayout(HInputLayout hInputLayout) = 0;

        /// Increments the internal reference counter of the given input layout object.
        ///
        /// @param hInputLayout [in] A handle to the object to hold.
        ///
        /// @remarks
        ///     This is thread safe.
        virtual void HoldInputLayout(HInputLayout hInputLayout) = 0;


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
        virtual ResultCode CompileShader(const char* source, size_t sourceLength, const GPUShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut) = 0;

        /// Determines whether or not the given shader language is supported by the rendering device.
        ///
        /// @param language [in] The shader language to check.
        virtual bool IsShaderLanguageSupported(ShaderLanguage language) const = 0;


        /// Creates a shader program from a vertex and fragment shader.
        ///
        /// @param vertexShaderData       [in]  A pointer to the buffer containing the binary code of the vertex shader as returned from CompileShader().
        /// @param vertexShaderDataSize   [in]  The size in bytes of the vertex shader data.
        /// @param fragmentShaderData     [in]  A pointer to the buffer containing the binary code of the fragment/pixel shader as returned from CompileShader().
        /// @param fragmentShaderDataSize [in]  The size in bytes of the fragment shader data.
        /// @param messageOut             [out] A reference to the buffer that will receive any compilation/linking messages.
        ///
        /// @return A handle to the new shader program, or 0 if an error occurs.
        ///
        /// @remarks
        ///     The difference between a shader and a shader program is that a shader program is a monolithic object that is made up of a shader that
        ///     covers the vertex and fragment pipeline.
        ///     @par
        ///     The concept of a shader program exists only for compatibility with the OpenGL 2.1 API which does not support separate shader objects. Therefore,
        ///     it does not support anything other than vertex and fragment shaders.
        ///     @par
        ///     Generation 2 APIs which support separate shader objects do not need to ever use shader programs, however it can still be useful in order to
        ///     keep rendering operations consistent.
        virtual HShaderProgram CreateShaderProgram(const void* vertexShaderData, size_t vertexShaderDataSize, const void* fragmentShaderData, size_t fragmentShaderDataSize, GT::BasicBuffer &messagesOut) = 0;

        /// Decrements the reference count of the given shader program and deletes the internal object if it hits 0.
        ///
        /// @param hShaderProgram [in] A handle to the shader program to release.
        ///
        /// @remarks
        ///     This is thread-safe.
        ///     @par
        ///     It is possible that the internal API-specific data structure may not be deleted until the next flush or buffer swap in the interest of
        ///     synchronization.
        virtual void ReleaseShaderProgram(HShaderProgram hShaderProgram) = 0;

        /// Increments the internal reference counter of the given shader program object.
        ///
        /// @param hShaderProgram [in] A handle to the buffer object to hold.
        ///
        /// @remarks
        ///     This is thread safe.
        virtual void HoldShaderProgram(HShaderProgram hShaderProgram) = 0;



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
        ///     When \c usage is GPUBufferUsage_Immutable, \c data cannot be null.
        ///     @par
        ///     If \c cpuAccessFlags is set to GPUBufferCPUAccess_None (0), all map and unmap operations will fail.
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
        virtual HBuffer CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data) = 0;

        /// Reduces the reference count of the given buffer object and deletes the internal reference when it hits 0.
        ///
        /// @param hBuffer [in] A handle to the buffer to delete.
        ///
        /// @remarks
        ///     This is thread safe.
        ///     @par
        ///     It is possible that the internal API-specific data structure may not be deleted until the next flush or buffer swap in the interest of
        ///     synchronization.
        virtual void ReleaseBuffer(HBuffer hBuffer) = 0;
        
        /// Increments the internal reference counter of the given buffer object.
        ///
        /// @param hBuffer [in] A handle to the buffer object to hold.
        ///
        /// @remarks
        ///     This is thread safe.
        virtual void HoldBuffer(HBuffer hBuffer) = 0;


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
        ///     This cannot be used when the buffer was created with GPUBufferUsage_Immutable.
        ///     @par
        ///     When \c mapType is \c GPUBufferMapType_Read or \c GPUBufferMapType_ReadWrite, the buffer must have been created with \c GPUBufferCPUAccess_Read.
        ///     @par
        ///     When \c mapType is \c GPUBufferMapType_Write, \c GPUBufferMapType_ReadWrite, \c GPUBufferMapType_Write_Discard or \c GPUBufferMapType_Write_NoOverwrite, the buffer must have been created with \c GPUBufferCPUAccess_Write.
        virtual void* MapBuffer(HBuffer hBuffer, GPUBufferMapType mapType) = 0;

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
        ///     This will fail if the buffer was created with GPUBufferUsage_Immutable.
        ///     @par
        ///     This will fail if the buffer is currently mapped.
        ///     @par
        ///     When using the Direct3D 11 API, this will use map/unmap internally when the buffer was created with GPUBufferUsage_Dynamic.
        virtual void SetBufferData(HBuffer hBuffer, size_t offsetInBytes, size_t sizeInBytes, const void* data) = 0;



        ///////////////////////////////////////////
        // Textures

        virtual HTexture2D CreateTexture2D(const Texture2DDesc &desc) = 0;
        virtual void ReleaseTexture2D(HTexture2D hTexture) = 0;
        virtual void HoldTexture2D(HTexture2D hTexture) = 0;
        virtual void UpdateTexture2D(HTexture2D hTexture, int x, int y, unsigned int width, unsigned int height, unsigned int level, unsigned int layer, const void* srcData, unsigned int srcDataRowPitch) = 0;

        virtual HTextureView CreateTextureViewFrom1D(HTexture2D hTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers) = 0;
        virtual HTextureView CreateTextureViewFrom2D(HTexture2D hTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers) = 0;
        virtual HTextureView CreateTextureViewFrom2DMultisample(HTexture2DMultisample hTexture, TextureType type, TextureFormat format, unsigned int minLayer, unsigned int numLayers) = 0;
        virtual HTextureView CreateTextureViewFrom3D(HTexture3D hTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels) = 0;
        virtual HTextureView CreateTextureViewFromCube(HTextureCube hTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers) = 0;
        virtual void ReleaseTextureView(HTextureView hTextureView) = 0;
        virtual void HoldTextureView(HTextureView hTextureView) = 0;


        ///////////////////////////////////////////
        // Samplers

        virtual HSampler CreateSampler(const SamplerDesc &desc) = 0;
        virtual void ReleaseSampler(HSampler hSampler) = 0;
        virtual void HoldSampler(HSampler hSampler) = 0;


        ///////////////////////////////////////////
        // Framebuffers

        /// Creates a framebuffer object.
        ///
        /// @param framebufferOut [in] A reference to the variable that will receive a pointer to the new framebuffer, if successful.
        //virtual ResultCode CreateFramebuffer(Framebuffer* &framebuffer) = 0;



        //////////////////////////////////////////
        // Platform-Specific Methods

        /// Swaps the buffers for the current window.
        virtual void SwapBuffers() = 0;

#if defined(GT_PLATFORM_WINDOWS)
        /// Win32: Creates a framebuffer for the given window so that it can be drawn to.
        ///
        /// @param hWnd                [in] A handle to the window whose framebuffer is being initialized.
        /// @param includeDepthStencil [in] Whether or not a depth/stencil buffer should also be created.
        virtual ResultCode InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil) = 0;

        /// Win32: Uninitialises the framebuffer of the given window.
        ///
        /// @param hWnd [in] A handle to the window whose framebuffer is being unitialized.
        virtual void UninitWindowFramebuffer(HWND hWnd) = 0;

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

        /// Updates the size of the give window's framebuffer.
        ///
        /// @param hWnd [in] A handle to the window whose framebuffer needs to be resized.
        ///
        /// @remarks
        ///     This should be called in response to the window's size changing.
        virtual void ResizeWindowFramebuffer(HWND hWnd) = 0;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif


    protected:

        /// Creates a shader binary buffer from the given information. 
        static ResultCode CreateShaderBinaryData(const char* source, size_t sourceLength, const GPUShaderDefine* defines, ShaderLanguage language, ShaderType type, const void* binary, size_t binarySizeInBytes, int binaryVersion, GT::BasicBuffer &byteCodeOut);

        /// Takes the shader binary data created by CreateShaderBinaryData() and reads it's various components.
        static ResultCode ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const char* &sourceOut, size_t &sourceLengthOut, GTLib::Vector<GPUShaderDefine> &definesOut, ShaderLanguage &languageOut, ShaderType &typeOut, const void* &binaryOut, size_t &binarySizeOut, int &binaryVersionOut);
        static ResultCode ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const char* &sourceOut, size_t &sourceLengthOut, GTLib::Vector<GPUShaderDefine> &definesOut, ShaderLanguage &languageOut, ShaderType &typeOut);
        static ResultCode ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const void* &binaryOut, size_t &binarySizeOut);


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
        static const ResultCode UnknownGPUBufferType              = (1 << 31) | 0x00000020;
        static const ResultCode UnknownGPUBufferUsage             = (1 << 31) | 0x00000021;
        static const ResultCode UnsupportedGPUBufferType          = (1 << 31) | 0x00000022;
        static const ResultCode UnsupportedGPUBufferUsage         = (1 << 31) | 0x00000023;
        static const ResultCode NoDataSpecifiedForImmutableBuffer = (1 << 31) | 0x00000024;
        static const ResultCode GPUBufferIsImmutable              = (1 << 31) | 0x00000025;
        static const ResultCode FailedToMapGPUBuffer              = (1 << 31) | 0x00000026;
        static const ResultCode UnknownGPUBufferMapType           = (1 << 31) | 0x00000027;
            


    private:    // No copying.
        GPURenderingDevice(const GPURenderingDevice &);
        GPURenderingDevice & operator=(const GPURenderingDevice &);
    };
}

#endif