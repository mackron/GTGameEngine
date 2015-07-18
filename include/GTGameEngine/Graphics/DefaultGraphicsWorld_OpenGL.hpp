// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_DefaultGraphicsWorld_OpenGL_hpp_
#define __GT_DefaultGraphicsWorld_OpenGL_hpp_

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
        HGraphicsResource CreateTextureResource(unsigned int width, unsigned int height, unsigned int depth, TextureFormat format, const void* pData);

        /// @copydoc GraphicsWorld::CreateMaterialResource()
        HGraphicsResource CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc);

        /// @copydoc GraphicsWorld::CreateMeshResource()
        HGraphicsResource CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterial()
        void SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource);

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

        /// @copydoc GraphicsWorld::GetRenderTargetProjectionAndView()
        void GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, mat4 &projectionOut, mat4 &viewOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetGUISurface()
        void SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, HGUISurface hSurface);

        /// @copydoc GraphicsWorld::GetRenderTargetGUISurface()
        HGUISurface GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const;



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


        /// Deletes the given texture resource.
        void DeleteTextureResource(HGraphicsResource hTextureResource);

        /// Deletes the given material resource.
        void DeleteMaterialResource(HGraphicsResource hMaterialResource);

        /// Deletes the given mesh resource.
        void DeleteMeshResource(HGraphicsResource hMeshResource);


        /// Deletes the given mesh object.
        void DeleteMeshObject(HGraphicsResource hMeshObject);

        /// Helper for retrieving a pointer to the buffer that contains the value of the given variable.
        ///
        /// @remarks
        ///     This is used when setting the value of a material input variable for the given mesh object.
        void* GetMeshObjectMaterialInputVariableBufferPtr(HGraphicsObject hMeshObject, unsigned int materialSlotIndex, const char* variableName) const;


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


        /// Temporary program object for testing.
        GLuint m_testProgramObjectGL;
    };
}

#endif

#endif
