// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_OpenGL21_RCCreateShader_hpp_
#define __GTEngine_Rendering_OpenGL21_RCCreateShader_hpp_

#include <GTEngine/Core/String.hpp>
#include <gtgl/gtgl.h>

#include "../../RenderCommand.hpp"
#include "ShaderState_OpenGL21.hpp"

namespace GTEngine
{
    /// Render command for clearing the buffers.
    class RCCreateShader : public RenderCommand
    {
    public:

        /// Constructor.
        RCCreateShader();

        
        /// Renderer::CreateShader().
        void CreateShader(ShaderState_OpenGL21* programState, const char* vertexShaderSourceIn, const char* fragmentSourceSourceIn, const char* geometryShaderSourceIn);




        /// RenderCommand::Execute()
        void Execute();



    private:

        /// Creates a shader object from the given source.
        ///
        /// @remarks
        ///     This does not create the main program object, but the individual vertex, fragment or geometry shader object.
        ///     @par
        ///     The compilation log is output to the engines main log.
        GLuint CreateShader(GLenum type, const GTLib::String &source);

        /// Links the given shaders to create the main shader program.
        ///
        /// @remarks
        ///     The geometry shader can be 0, but the vertex and fragment shader must not.
        GLuint LinkProgram(GLuint vertexShaderObject, GLuint fragmentShaderObject, GLuint geometryShaderObject);


    private:

        /// A pointer to the program object.
        ShaderState_OpenGL21* programState;


        /// The vertex shader source.
        GTLib::String vertexShaderSource;

        /// The fragment shader source.
        GTLib::String fragmentShaderSource;

        /// The geometry shader source. Can be null.
        GTLib::String geometryShaderSource;


    private:    // No copying.
        RCCreateShader(const RCCreateShader &);
        RCCreateShader & operator=(const RCCreateShader &);
    };
}

#endif
