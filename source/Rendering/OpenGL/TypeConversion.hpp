// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_TypeConversion_hpp_
#define __GTEngine_TypeConversion_hpp_

#include <GTEngine/Rendering/TextureFilters.hpp>
#include <GTEngine/Rendering/TextureWrapModes.hpp>
#include <GTEngine/Rendering/VertexArray.hpp>
#include <GTEngine/Rendering/DrawModes.hpp>
#include <GTEngine/Rendering/BlendFunctions.hpp>
#include <GTEngine/Rendering/BlendEquations.hpp>
#include <GTEngine/Rendering/RendererFunctions.hpp>
#include <GTEngine/Rendering/StencilOperations.hpp>
#include <GTEngine/Core/ImageFormats.hpp>
#include <gtgl/gtgl.h>

#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunreachable-code"
#endif

namespace GTEngine
{
    inline GLint ToOpenGLTextureFilter(TextureFilter filter)
    {
        switch (filter)
        {
        case TextureFilter_Nearest: return GL_NEAREST;
        case TextureFilter_Linear:  return GL_LINEAR;

        case TextureFilter_NearestNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilter_NearestLinear:  return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilter_LinearNearest:  return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilter_LinearLinear:   return GL_LINEAR_MIPMAP_LINEAR;

        default: break;
        }

        return GL_LINEAR;
    }

    inline GLint ToOpenGLTextureMinificationFilter(TextureFilter filter, bool mipmapFilter = true)
    {
        auto filterGL = ToOpenGLTextureFilter(filter);
        if (!mipmapFilter)
        {
            // If we get here, it means we only have a single mip level. In this case, we don't want to be using a mipmap filter.
            if (filterGL == GL_LINEAR_MIPMAP_LINEAR || filterGL == GL_LINEAR_MIPMAP_NEAREST)
            {
                filterGL = GL_LINEAR;
            }
            else if (filterGL == GL_NEAREST_MIPMAP_LINEAR || filterGL == GL_NEAREST_MIPMAP_NEAREST)
            {
                filterGL = GL_NEAREST;
            }
        }

        return filterGL;
    }

    inline GLint ToOpenGLTextureMagnificationFilter(TextureFilter filter)
    {
        auto filterGL = ToOpenGLTextureFilter(filter);
        if (filterGL == GL_LINEAR_MIPMAP_LINEAR || filterGL == GL_LINEAR_MIPMAP_NEAREST)
        {
            filterGL = GL_LINEAR;
        }
        else if (filterGL == GL_NEAREST_MIPMAP_LINEAR || filterGL == GL_NEAREST_MIPMAP_NEAREST)
        {
            filterGL = GL_NEAREST;
        }

        return filterGL;
    }


    inline GLint ToOpenGLWrapMode(TextureWrapMode wrapMode)
    {
        switch (wrapMode)
        {
        case TextureWrapMode_Repeat:        return GL_REPEAT;
        case TextureWrapMode_Clamp:         return GL_CLAMP;
        case TextureWrapMode_ClampToEdge:   return GL_CLAMP_TO_EDGE;

        default: break;
        }

        return GL_REPEAT;
    }

    inline GLint ToOpenGLInternalFormat(GTLib::ImageFormat format)
    {
        switch (format)
        {
        case GTLib::ImageFormat_R8:               return GL_R8;
        case GTLib::ImageFormat_RG8:              return GL_RG8;
        case GTLib::ImageFormat_RGB8:             return GL_RGB8;
        case GTLib::ImageFormat_RGBA8:            return GL_RGBA8;
        case GTLib::ImageFormat_RGBA16:           return GL_RGBA16;
        case GTLib::ImageFormat_RGBA32:           return GL_RGBA32UI;
        case GTLib::ImageFormat_R16F:             return GL_R16F;
        case GTLib::ImageFormat_RG16F:            return GL_RG16F;
        case GTLib::ImageFormat_RGB16F:           return GL_RGB16F;
        case GTLib::ImageFormat_RGBA16F:          return GL_RGBA16F;
        case GTLib::ImageFormat_R32F:             return GL_R32F;
        case GTLib::ImageFormat_RG32F:            return GL_RG32F;
        case GTLib::ImageFormat_RGB32F:           return GL_RGB32F;
        case GTLib::ImageFormat_RGBA32F:          return GL_RGBA32F;

        case GTLib::ImageFormat_Depth16:          return GL_DEPTH_COMPONENT16;
        case GTLib::ImageFormat_Depth24:          return GL_DEPTH_COMPONENT24;
        case GTLib::ImageFormat_Depth32:          return GL_DEPTH_COMPONENT32;
        case GTLib::ImageFormat_Depth24_Stencil8: return GL_DEPTH24_STENCIL8;

        case GTLib::ImageFormat_R10G10B10A2:      return GL_RGB10_A2;

        case GTLib::ImageFormat_Auto:
        default: break;
        }

        return GL_RGBA8;
    }

    inline GLenum ToOpenGLFormat(GTLib::ImageFormat format)
    {
        switch (format)
        {
        case GTLib::ImageFormat_R8:
        case GTLib::ImageFormat_R16F:
        case GTLib::ImageFormat_R32F:
            {
                return GL_RED;
            }

        case GTLib::ImageFormat_RG8:
        case GTLib::ImageFormat_RG16F:
        case GTLib::ImageFormat_RG32F:
            {
                return GL_RG;
            }

        case GTLib::ImageFormat_RGB8:
        case GTLib::ImageFormat_RGB16F:
        case GTLib::ImageFormat_RGB32F:
            {
                return GL_RGB;
            }

        case GTLib::ImageFormat_RGBA8:
        case GTLib::ImageFormat_RGBA16:
        case GTLib::ImageFormat_RGBA32:
        case GTLib::ImageFormat_RGBA16F:
        case GTLib::ImageFormat_RGBA32F:
        case GTLib::ImageFormat_R10G10B10A2:
            {
                return GL_RGBA;
            }

        case GTLib::ImageFormat_Depth16:
        case GTLib::ImageFormat_Depth24:
        case GTLib::ImageFormat_Depth32:
            {
                return GL_DEPTH_COMPONENT;
            }

        case GTLib::ImageFormat_Depth24_Stencil8:
            {
                return GL_DEPTH_STENCIL;
            }

        case GTLib::ImageFormat_Auto:
        default: break;
        }

        return GL_RGBA;
    }

    inline GLenum ToOpenGLType(GTLib::ImageFormat format)
    {
        switch (format)
        {
        case GTLib::ImageFormat_R8:
        case GTLib::ImageFormat_RG8:
        case GTLib::ImageFormat_RGB8:
        case GTLib::ImageFormat_RGBA8:
            {
                return GL_UNSIGNED_BYTE;
            }

        case GTLib::ImageFormat_RGBA16:
            {
                return GL_UNSIGNED_SHORT;
            }

        case GTLib::ImageFormat_RGBA32:
            {
                return GL_UNSIGNED_INT;
            }

        case GTLib::ImageFormat_R16F:
        case GTLib::ImageFormat_RG16F:
        case GTLib::ImageFormat_RGB16F:
        case GTLib::ImageFormat_RGBA16F:
            {
                return GL_HALF_FLOAT;
            }

        case GTLib::ImageFormat_R32F:
        case GTLib::ImageFormat_RG32F:
        case GTLib::ImageFormat_RGB32F:
        case GTLib::ImageFormat_RGBA32F:
            {
                return GL_FLOAT;
            }


        case GTLib::ImageFormat_Depth16:
        case GTLib::ImageFormat_Depth24:
        case GTLib::ImageFormat_Depth32:
            {
                return GL_FLOAT;
            }

        case GTLib::ImageFormat_Depth24_Stencil8:
            {
                return GL_UNSIGNED_INT_24_8;
            }


        case GTLib::ImageFormat_R10G10B10A2:
            {
                return GL_UNSIGNED_INT_10_10_10_2;
            }


        case GTLib::ImageFormat_Auto:
        default: break;
        }

        return GL_UNSIGNED_BYTE;
    }

    inline GLenum ToOpenGLBufferUsage(VertexArrayUsage usage)
    {
        switch (usage)
        {
        case VertexArrayUsage_Static:   return GL_STATIC_DRAW;
        case VertexArrayUsage_Dynamic:  return GL_DYNAMIC_DRAW;
        case VertexArrayUsage_Stream:   return GL_STREAM_DRAW;

        default: break;
        }

        return GL_STATIC_DRAW;
    }

    inline GLenum ToOpenGLDrawMode(DrawMode mode)
    {
        switch (mode)
        {
        case DrawMode_Triangles:    return GL_TRIANGLES;
        case DrawMode_Lines:        return GL_LINES;

        default: break;
        }

        return GL_TRIANGLES;
    }

    
    inline GLenum ToOpenGLBlendFunc(BlendFunc func)
    {
        switch (func)
        {
        case BlendFunc_Zero:                    return GL_ZERO;
        case BlendFunc_One:                     return GL_ONE;

        case BlendFunc_SourceColour:            return GL_SRC_COLOR;
        case BlendFunc_OneMinusSourceColour:    return GL_ONE_MINUS_SRC_COLOR;
        case BlendFunc_SourceAlpha:             return GL_SRC_ALPHA;
        case BlendFunc_OneMinusSourceAlpha:     return GL_ONE_MINUS_SRC_ALPHA;

        case BlendFunc_DestColour:              return GL_DST_COLOR;
        case BlendFunc_OneMinusDestColour:      return GL_ONE_MINUS_DST_COLOR;
        case BlendFunc_DestAlpha:               return GL_DST_ALPHA;
        case BlendFunc_OneMinusDestAlpha:       return GL_ONE_MINUS_DST_ALPHA;

        case BlendFunc_ConstantColour:          return GL_CONSTANT_COLOR;
        case BlendFunc_OneMinusConstantColour:  return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFunc_ConstantAlpha:           return GL_CONSTANT_ALPHA;
        case BlendFunc_OneMinusConstantAlpha:   return GL_ONE_MINUS_CONSTANT_ALPHA;

        case BlendFunc_SourceAlphaSaturate:     return GL_SRC_ALPHA_SATURATE;

        default: break;
        }

        return GL_SRC_COLOR;
    }

    inline GLenum ToOpenGLBlendEquation(BlendEquation equation)
    {
        switch (equation)
        {
        case BlendEquation_Add:             return GL_FUNC_ADD;
        case BlendEquation_Subtract:        return GL_FUNC_SUBTRACT;
        case BlendEquation_ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendEquation_Min:             return GL_MIN;
        case BlendEquation_Max:             return GL_MAX;

        default: break;
        }

        return GL_FUNC_ADD;
    }

    inline GLenum ToOpenGLFunc(RendererFunction func)
    {
        switch (func)
        {
        case RendererFunction_Never:    return GL_NEVER;
        case RendererFunction_Less:     return GL_LESS;
        case RendererFunction_Equal:    return GL_EQUAL;
        case RendererFunction_LEqual:   return GL_LEQUAL;
        case RendererFunction_Greater:  return GL_GREATER;
        case RendererFunction_NotEqual: return GL_NOTEQUAL;
        case RendererFunction_GEqual:   return GL_GEQUAL;
        case RendererFunction_Always:   return GL_ALWAYS;
        default: break;
        }

        return GL_ALWAYS;
    }

    inline GLenum ToOpenGLStencilOp(StencilOp op)
    {
        switch (op)
        {
        case StencilOp_Keep:          return GL_KEEP;
        case StencilOp_Zero:          return GL_ZERO;
        case StencilOp_Replace:       return GL_REPLACE;
        case StencilOp_Increment:     return GL_INCR;
        case StencilOp_IncrementWrap: return GL_INCR_WRAP;
        case StencilOp_Decrement:     return GL_DECR;
        case StencilOp_DecrementWrap: return GL_DECR_WRAP;
        case StencilOp_Invert:        return GL_INVERT;
        default: break;
        }

        return GL_KEEP;
    }
}


#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#endif
