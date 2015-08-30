// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultGraphicsWorld_OpenGL
#define GT_DefaultGraphicsWorld_OpenGL

#include "../Config.hpp"
#if defined(GT_BUILD_OPENGL)
#include "GraphicsWorld.hpp"
#include "GraphicsAPI_OpenGL.hpp"
#include <GTLib/Vector.hpp>

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
        DefaultGraphicsWorld_OpenGL(GUIContext &gui, GraphicsAPI_OpenGL &gl);

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


        /// @copydoc GraphicsWorld::CreateMaterialResource()
        HGraphicsResource CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc);

        /// @copydoc GraphicsWorld::SetMaterialResourceInputVariable()
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::CreateMeshResource()
        HGraphicsResource CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterial()
        void SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterialInputVariable()
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::DeleteResource()
        void DeleteResource(HGraphicsResource hResource);



        ////////////////////
        // Objects

        /// @copydoc GraphicsWorld::SetObjectTransform()
        void SetObjectTransform(HGraphicsObject hObject, const vec4 &position, const quat &rotation, const vec4 &scale);

        /// @copydoc GraphicsWorld::SetObjectPosition()
        void SetObjectPosition(HGraphicsObject hObject, const vec4 &position);

        /// @copydoc GraphicsWorld::SetObjectRotation()
        void SetObjectRotation(HGraphicsObject hObject, const quat &rotation);

        /// @copydoc GraphicsWorld::SetObjectScale()
        void SetObjectScale(HGraphicsObject hObject, const vec4 &scale);


        /// @copydoc GraphicsWorld::CreateMeshObject()
        HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position = vec4(0, 0, 0, 1), const quat &rotation = quat::identity, const vec4 &scale = vec4(1, 1, 1, 1));

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

#if defined(GT_PLATFORM_WINDOWS)
        /// @copydoc GraphicsWorld::CreateRenderTargetFromWindow()
        HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags);

        /// @copydoc GraphicsWorld::GetRenderTargetByWindow()
        HGraphicsRenderTarget GetRenderTargetByWindow(HWND hWnd) const;
#endif

        /// @copydoc GraphicsWorld::CreateRenderTargetFromTexture()
        HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags);

        /// @copydoc GraphicsWorld::DeleteRenderTarget()
        void DeleteRenderTarget(HGraphicsRenderTarget hRT);


        /// @copydoc GraphicsWorld::SetRenderTargetViewport()
        void SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height);

        /// @copydoc GraphicsWorld::GetRenderTargetViewport()
        void GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetClearColor()
        void SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GTLib::Colour &color);

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
        void SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const mat4 &projection, const mat4 &view);

        /// @copydoc GraphicsWorld::SetRenderTargetProjection()
        void SetRenderTargetProjection(HGraphicsRenderTarget hRT, const mat4 &projection);

        /// @copydoc GraphicsWorld::SetRenderTargetView()
        void SetRenderTargetView(HGraphicsRenderTarget hRT, const mat4 &view);

        /// @copydoc GraphicsWorld::GetRenderTargetProjectionAndView()
        void GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, mat4 &projectionOut, mat4 &viewOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetGUISurface()
        void SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, HGUISurface hSurface);

        /// @copydoc GraphicsWorld::GetRenderTargetGUISurface()
        HGUISurface GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const;

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


        // GUI

        /// @copydoc DefaultGraphicsWorld::GUI_BeginPaintSurface()
        void GUI_BeginPaintSurface(GUIContext &gui, HGUISurface hSurface, void* pInputData);

        /// @copydoc DefaultGraphicsWorld::GUI_EndPaintSurface()
        void GUI_EndPaintSurface(GUIContext &gui);

        /// @copydoc DefaultGraphicsWorld::Clear()
        void GUI_Clear(GUIContext &gui);

        /// @copydoc DefaultGraphicsWorld::Clear()
        void GUI_Clear(GUIContext &gui, const GTLib::Rect<int> &rect);

        /// @copydoc DefaultGraphicsWorld::DrawRectangle()
        void GUI_DrawRectangle(GUIContext &gui, GTLib::Rect<int> rect, GTLib::Colour colour);

        /// @copydoc DefaultGraphicsWorld::SetClippingRect()
        void GUI_SetClippingRect(GUIContext &gui, GTLib::Rect<int> rect);

        /// @copydoc DefaultGraphicsWorld::CanDrawText()
        bool GUI_CanDrawText(GUIContext &gui, HGUIFont hFont);

        /// @copydoc DefaultGraphicsWorld::DrawText()
        void GUI_DrawText(GUIContext &gui, const GUITextRunDesc &textRunDesc);

        /// @copydoc GUIRenderer::DrawRawImage()
        void GUI_DrawRawImage(GUIContext &gui, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent);

        /// @copydoc GUIRenderer::DrawTexturedRectangle()
        void GUI_DrawTexturedRectangle(GUIContext &gui, GTLib::Rect<int> rect, HGraphicsResource hTextureResource, GTLib::Colour colour, unsigned int subImageOffsetX, unsigned int subImageOffsetY, unsigned int subImageWidth, unsigned int subImageHeight);



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


#if defined(GT_PLATFORM_WINDOWS)
        /// The rendering context to use with this world. This is created from information provided by the GraphicsAPI_OpenGL object.
        HGLRC m_hRC;
#endif


        /// The list of window render targets.
        GTLib::Vector<HGraphicsRenderTarget> m_windowRTs;

        /// The list of texture render targets.
        GTLib::Vector<HGraphicsRenderTarget> m_textureRTs;


        /// The list of mesh objects. This probably tempoary since we'll place these into an accelerated structure.
        GTLib::Vector<HGraphicsObject> m_meshObjects;


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


        /// The program to use when drawing GUI text.
        GLuint m_guiTextProgram;


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
        mat4 m_guiProjection;

        /// The width of the current surface.
        unsigned int m_currentSurfaceWidth;

        /// The height of the current surface.
        unsigned int m_currentSurfaceHeight;
    };
}

#endif

#endif
