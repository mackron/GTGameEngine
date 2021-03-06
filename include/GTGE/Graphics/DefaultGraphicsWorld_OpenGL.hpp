// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultGraphicsWorld_OpenGL
#define GT_DefaultGraphicsWorld_OpenGL

#include "../Config.hpp"
#if defined(GT_BUILD_OPENGL)
#include "GraphicsWorld.hpp"
#include "GraphicsAPI_OpenGL.hpp"
#include <GTGE/Core/Vector.hpp>

namespace GT
{
    // An implementation of the default graphics world that does not do anything.
    //
    // This should only be used as a fallback in cases when no rendering APIs are supported so that a crash can be prevented. In the
    // future this may be replaced with a software implementation.
    class DefaultGraphicsWorld_OpenGL : public GraphicsWorld
    {
    public:

        /// Constructor.
        DefaultGraphicsWorld_OpenGL(drgui_context* pGUI, GraphicsAPI_OpenGL &gl);

        /// Destructor.
        ~DefaultGraphicsWorld_OpenGL();


        ////////////////////////////////////////////
        // Virtual Methods

        /// @copydoc GraphicsWorld::Startup()
        bool Startup();

        /// @copydoc GraphicsWorld::Shutdown()
        void Shutdown();



        ////////////////////
        // Resources

        /// @copydoc GraphicsWorld::CreateTextureResource()
        HGraphicsResource CreateTextureResource(const GraphicsTextureResourceDesc &textureDesc);

        /// @copydoc GraphicsWorld::GetTextureSize()
        void GetTextureSize(HGraphicsResource hTextureResource, unsigned int &widthOut, unsigned int &heightOut, unsigned int &depthOut);

        /// @copydoc GraphicsWorld::GetTextureFormat()
        TextureFormat GetTextureFormat(HGraphicsResource hTextureResource);

        /// @copydoc GraphicsWorld::GetTextureData()
        bool GetTextureData(HGraphicsResource hTextureResource, void* pDataOut);



        /// @copydoc GraphicsWorld::CreateMaterialResource()
        HGraphicsResource CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc);

        /// @copydoc GraphicsWorld::SetMaterialResourceInputVariable()
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y, float z);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y, float z, float w);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y, int z);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y, int z, int w);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::CreateMeshResource()
        HGraphicsResource CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterial()
        void SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterialInputVariable()
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y, float z);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y, int z);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::DeleteResource()
        void DeleteResource(HGraphicsResource hResource);



        ////////////////////
        // Objects

        /// @copydoc GraphicsWorld::SetObjectTransform()
        void SetObjectTransform(HGraphicsObject hObject, const glm::vec4 &position, const glm::quat &rotation, const glm::vec4 &scale);

        /// @copydoc GraphicsWorld::SetObjectPosition()
        void SetObjectPosition(HGraphicsObject hObject, const glm::vec4 &position);

        /// @copydoc GraphicsWorld::SetObjectRotation()
        void SetObjectRotation(HGraphicsObject hObject, const glm::quat &rotation);

        /// @copydoc GraphicsWorld::SetObjectScale()
        void SetObjectScale(HGraphicsObject hObject, const glm::vec4 &scale);


        /// @copydoc GraphicsWorld::CreateMeshObject()
        HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const glm::vec4 &position = glm::vec4(0, 0, 0, 1), const glm::quat &rotation = glm::quat(), const glm::vec4 &scale = glm::vec4(1, 1, 1, 1));

        /// @copydoc GraphicsWorld::SetMeshObjectMaterial()
        void SetMeshObjectMaterial(HGraphicsObject hMeshObject, unsigned int materialSlot, HGraphicsResource hMaterialResource);

        /// @copydoc GraphicsWorld::SetMeshObjectMaterialInputVariable()
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::DeleteObject()
        void DeleteObject(HGraphicsObject);



        ////////////////////
        // Render Targets

#if defined(_WIN32)
        /// @copydoc GraphicsWorld::CreateRenderTargetFromWindow()
        HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags);

        /// @copydoc GraphicsWorld::GetRenderTargetByWindow()
        HGraphicsRenderTarget GetRenderTargetByWindow(HWND hWnd) const;
#endif

        /// @copydoc GraphicsWorld::CreateRenderTargetFromTexture()
        HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, AAType aaType, unsigned int aaQuality, uint32_t flags);

        /// @copydoc GraphicsWorld::DeleteRenderTarget()
        void DeleteRenderTarget(HGraphicsRenderTarget hRT);


        /// @copydoc GraphicsWorld::SetRenderTargetViewport()
        void SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height);

        /// @copydoc GraphicsWorld::GetRenderTargetViewport()
        void GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetClearColor()
        void SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GT::Color &color);

        /// @copydoc GraphicsWorld::EnableRenderTargetColorClearing()
        void EnableRenderTargetColorClearing(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::DisableRenderTargetColorClearing()
        void DisableRenderTargetColorClearing(HGraphicsRenderTarget hRT);


        /// @copydoc GraphicsWorld::SetRenderTargetPriority()
        void SetRenderTargetPriority(HGraphicsRenderTarget hRT, int priority);

        /// @copydoc GraphicsWorld::GetRenderTargetPriority()
        int GetRenderTargetPriority(HGraphicsRenderTarget hRT) const;


        /// @copydoc GraphicsWorld::EnableRenderTarget()
        void EnableRenderTarget(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::DisableRenderTarget()
        void DisableRenderTarget(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::IsRenderTargetEnabled()
        bool IsRenderTargetEnabled(HGraphicsRenderTarget hRT) const;


        /// @copydoc GraphicsWorld::SetRenderTargetProjectionAndView()
        void SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const glm::mat4 &projection, const glm::mat4 &view);

        /// @copydoc GraphicsWorld::SetRenderTargetProjection()
        void SetRenderTargetProjection(HGraphicsRenderTarget hRT, const glm::mat4 &projection);

        /// @copydoc GraphicsWorld::SetRenderTargetView()
        void SetRenderTargetView(HGraphicsRenderTarget hRT, const glm::mat4 &view);

        /// @copydoc GraphicsWorld::GetRenderTargetProjectionAndView()
        void GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, glm::mat4 &projectionOut, glm::mat4 &viewOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetGUIElement()
        void SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, drgui_element* pGUIElement);

        /// @copydoc GraphicsWorld::GetRenderTargetGUIElement()
        drgui_element* GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const;

        /// @copydoc GraphicsWorld::SetRenderTargetSurfaceSizeToRenderTarget()
        void SetRenderTargetSurfaceSizeToRenderTarget(HGraphicsRenderTarget hRT);



        ////////////////////
        // Rendering

        /// @copydoc GraphicsWorld::IsCommandBuffersSupported()
        bool IsCommandBuffersSupported() const;

        /// @copydoc GraphicsWorld::BuildCommandBuffers()
        void BuildCommandBuffers();

        /// @copydoc GraphicsWorld::ExecuteCommandBuffers()
        void ExecuteCommandBuffers();

        /// @copydoc GraphicsWorld::ExecuteRenderingCommands()
        void ExecuteRenderingCommands();



    private:

        /// Helper for making the world's OpenGL context current.
        void MakeOpenGLContextCurrent();

        /// Deletes a window render target.
        void DeleteWindowRenderTarget(HGraphicsRenderTarget hRT);

        /// Deletes a texture render target.
        void DeleteTextureRenderTarget(HGraphicsRenderTarget hRT);

        /// Sorts the window render targets.
        void SortWindowRenderTargets();

        /// Sorts the texture render targets.
        void SortTextureRenderTargets();

        /// Executes the rendering commands for the given window render target.
        void ExecuteWindowRTRenderingCommands(HGraphicsRenderTarget hRT);

        /// Executes the rendering commands for the given texture render target.
        void ExecuteTextureRTRenderingCommands(HGraphicsRenderTarget hRT);

        /// Executes the rendering commands for the current framebuffer.
        void ExecuteRTRenderingCommands(HGraphicsRenderTarget hRT);


        /// Deletes the given texture resource.
        void DeleteTextureResource(HGraphicsResource hTextureResource);

        /// Deletes the given material resource.
        void DeleteMaterialResource(HGraphicsResource hMaterialResource);

        /// Deletes the given mesh resource.
        void DeleteMeshResource(HGraphicsResource hMeshResource);


        /// Deletes the given mesh object.
        void DeleteMeshObject(HGraphicsObject hMeshObject);

        /// Helper for retrieving a pointer to the buffer that contains the value of the given variable.
        ///
        /// @remarks
        ///     This is used when setting the value of a material input variable for the given mesh object.
        void* GetMeshObjectMaterialInputVariableBufferPtr(HGraphicsObject hMeshObject, unsigned int materialSlotIndex, const char* variableName) const;

        /// Helper for retrieving a pointer to the buffer that contains the value of the given variable.
        ///
        /// @remarks
        ///     This is used when setting the value of a material input variable for the given mesh resource.
        void* GetMeshResourceMaterialInputVariableBufferPtr(HGraphicsResource hMeshResource, unsigned int materialSlotIndex, const char* variableName) const;

        /// Helper for retrieving a pointer to the buffer that contains the value of the given variable for the given material.
        ///
        /// @remarks
        ///     This is used when setting the value of a material input variable for the given material resource.
        void* GetMaterialInputVariableBufferPtr(HGraphicsResource hMaterialResource, const char* variableName) const;


        /// Creates a shader object from the given shader strings.
        GLuint CreateShader_GLSL(GLenum shaderType, GLsizei shaderStringsCount, const GLchar** shaderStrings, const GLint* shaderStringLengths);
        GLuint CreateShader_GLSL(GLenum shaderType, const GLchar* shaderString);

        /// Creates a shader from the given vertex and fragment shaders.
        GLuint CreateProgram_GLSL(GLuint vertexShader, GLuint fragmentShader);


    private:

        /// The graphics API. This is used to call OpenGL functions.
        GraphicsAPI_OpenGL &m_gl;


#if defined(_WIN32)
        /// The rendering context to use with this world. This is created from information provided by the GraphicsAPI_OpenGL object.
        HGLRC m_hRC;
#endif


        /// The list of window render targets.
        Vector<HGraphicsRenderTarget> m_windowRTs;

        /// The list of texture render targets.
        Vector<HGraphicsRenderTarget> m_textureRTs;


        /// The list of mesh objects. This probably tempoary since we'll place these into an accelerated structure.
        Vector<HGraphicsObject> m_meshObjects;


        /// The default texture to use for when a material's texture is set to 0. This is just a dark red.
        HGraphicsResource m_hDefaultTexture;



        /////////////////////////////////
        // GUI

        /// The program to use when drawing GUI rectangles.
        GLuint m_guiRectangleProgram;

        /// The location of the projection uniform variable for GUI's.
        GLint m_guiRectangleProgram_ProjectionLoc;

        /// The location of the rectangle color uniform variable.
        GLint m_guiRectangleProgram_ColorLoc;

        /// The location of the rectangle dimensions uniform variable.
        GLint m_guiRectangleProgram_RectLoc;

        /// The vertex buffer to use when drawing text.
        GLuint m_guiRectangleVertexBuffer;

        /// The index buffer to use when drawing text.
        GLuint m_guiRectangleIndexBuffer;



        /// The program to use when drawing GUI rectangles.
        GLuint m_guiTexturedRectangleProgram;

        /// The location of the projection uniform variable for GUI's.
        GLint m_guiTexturedRectangleProgram_ProjectionLoc;

        /// The location of the rectangle color uniform variable.
        GLint m_guiTexturedRectangleProgram_ColorLoc;

        /// The location of the rectangle dimensions uniform variable.
        GLint m_guiTexturedRectangleProgram_RectLoc;

        /// The location of the UV offset and scale uniform variable.
        GLint m_guiTexturedRectangleProgram_UVOffsetAndScaleLoc;

        /// The location of the texture uniform variable.
        GLint m_guiTexturedRectangleProgram_TextureLoc;

        /// The vertex buffer to use when drawing text.
        GLuint m_guiTexturedRectangleVertexBuffer;

        /// The index buffer to use when drawing text.
        GLuint m_guiTexturedRectangleIndexBuffer;



        /// The shader to use with DrawRawImage().
        GLuint m_guiDrawRawImageProgram;

        /// The location of the projection uniform variable for GUI_DrawRawImage().
        GLuint m_guiDrawRawImageProgram_ProjectionLoc;

        /// The location of the rectangle dimensions uniform variable for GUI_DrawRawImage().
        GLuint m_guiDrawRawImageProgram_RectLoc;

        /// The location of the color uniform variable for GUI_DrawRawImagE().
        GLuint m_guiDrawRawImageProgram_ColorLoc;

        /// The location of the texture uniform variable for GUI_DrawRawImage().
        GLuint m_guiDrawRawImageProgram_TextureLoc;

        /// The texture for use by DrawRawImage().
        GLuint m_guiDrawRawImageTexture;


        /// The projection matrix to use when drawing GUI elements.
        glm::mat4 m_guiProjection;

        /// The width of the current surface.
        unsigned int m_currentSurfaceWidth;

        /// The height of the current surface.
        unsigned int m_currentSurfaceHeight;
    };
}

#endif

#endif
