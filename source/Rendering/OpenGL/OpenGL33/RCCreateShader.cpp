// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateShader.hpp"
#include <GTEngine/Logging.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCCreateShader::RCCreateShader()
        : programObject(nullptr), vertexShaderSource(), fragmentShaderSource(), geometryShaderSource()
    {
    }


    void RCCreateShader::CreateShader(GLuint* programObject, const char* vertexShaderSourceIn, const char* fragmentShaderSourceIn, const char* geometryShaderSourceIn)
    {
        this->programObject = programObject;

        this->vertexShaderSource   = vertexShaderSourceIn;
        this->fragmentShaderSource = fragmentShaderSourceIn;

        if (geometryShaderSourceIn != nullptr)
        {
            this->geometryShaderSource = geometryShaderSourceIn;
        }
    }


    void RCCreateShader::Execute()
    {
        // 1) Create the shader objects (vertex, fragment and geometry shader objects).
        GLuint vertexShaderObject   = this->CreateShader(GL_VERTEX_SHADER,   this->vertexShaderSource.c_str());
        GLuint fragmentShaderObject = this->CreateShader(GL_FRAGMENT_SHADER, this->fragmentShaderSource.c_str());
        GLuint geometryShaderObject = 0;
        if (!this->geometryShaderSource.IsEmpty())
        {
            geometryShaderObject = this->CreateShader(GL_GEOMETRY_SHADER, this->geometryShaderSource.c_str());
        }


        // 2) Link the program.
        *this->programObject = this->LinkProgram(vertexShaderObject, fragmentShaderObject, geometryShaderObject);



        // 3) Delete the shader objects.
        if (vertexShaderObject   != 0) glDeleteShader(vertexShaderObject);
        if (fragmentShaderObject != 0) glDeleteShader(fragmentShaderObject);
        if (geometryShaderObject != 0) glDeleteShader(geometryShaderObject);
    }



    ////////////////////////////////////////////////////
    // Private

    GLuint RCCreateShader::CreateShader(GLenum type, const GTCore::String &sourceIn)
    {
        auto source       = sourceIn.c_str();
        auto sourceLength = static_cast<GLint>(sourceIn.GetLengthInTs());

        auto shaderObject = glCreateShader(type);
        glShaderSource(shaderObject, 1, &source, &sourceLength);
        glCompileShader(shaderObject);


        // Here we need to log any details.
        GLint compileStatus;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint shaderType;
            glGetShaderiv(shaderObject, GL_SHADER_TYPE, &shaderType);

            GLint logLength;
            glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLength);

            auto log = static_cast<char*>(malloc(logLength));
            glGetShaderInfoLog(shaderObject, logLength, nullptr, log);


            GTCore::String title;
            if (shaderType == GL_VERTEX_SHADER)
            {
                title = "Vertex Shader Info Log";
            }
            else if (shaderType == GL_FRAGMENT_SHADER)
            {
                title = "Fragment Shader Info Log";
            }
            else
            {
                title = "Geometry Shader Info Log";
            }

            GTEngine::Log("--- %s ---\n%s\n%s", title.c_str(), log, source);


            glDeleteShader(shaderObject);
            shaderObject = 0;

            free(log);
        }


        return shaderObject;
    }

    GLuint RCCreateShader::LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader)
    {
        // We'll need a program object.
        GLuint programObject = glCreateProgram();


        // We need to have concretely defined vertex attributes for OpenGL 2.0 GLSL since we don't really have much control of vertex attributes
        // from inside the shader code. Thus, we're going to have to use hard coded attributes names. Later on we might make this configurable
        // via the shader library or a config file.
        glBindAttribLocation(programObject, 0, "VertexInput_Position");
        glBindAttribLocation(programObject, 1, "VertexInput_TexCoord");
        glBindAttribLocation(programObject, 2, "VertexInput_Normal");
        glBindAttribLocation(programObject, 3, "VertexInput_Tangent");
        glBindAttribLocation(programObject, 4, "VertexInput_Bitangent");
        glBindAttribLocation(programObject, 5, "VertexInput_Colour");


        // Finally we reattach the shaders, link the program and check for errors.
        if (vertexShader   != 0) glAttachShader(programObject, vertexShader);
        if (fragmentShader != 0) glAttachShader(programObject, fragmentShader);
        if (geometryShader != 0) glAttachShader(programObject, geometryShader);

        glLinkProgram(programObject);


        // Check for link errors.
        GLint linkStatus;
        glGetProgramiv(programObject, GL_LINK_STATUS, &linkStatus);

        if (linkStatus == GL_FALSE)
        {
            GLint logLength;
            glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &logLength);

            auto log = static_cast<char*>(malloc(logLength));
            glGetProgramInfoLog(programObject, logLength, nullptr, log);

            GTEngine::Log("--- Program Link Status ---\n%s", log);

            free(log);
            return false;
        }

        return true;
    }
}
