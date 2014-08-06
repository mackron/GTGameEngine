// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_ShaderParameter_hpp_
#define __GTEngine_ShaderParameter_hpp_

#include "Texture2DLibrary.hpp"
#include "Math.hpp"
#include "Rendering/TextureCube.hpp"

namespace GTEngine
{
    class Texture2D;
    class TextureCube;

    /// Enumerator for the different types of material properties.
    ///
    /// @remarks
    ///     This enumerator needs to be synchronized with the version in the scripting environment.
    enum ShaderParameterType
    {
        ShaderParameterType_Float,
        ShaderParameterType_Float2,
        ShaderParameterType_Float3,
        ShaderParameterType_Float4,

        ShaderParameterType_Integer,
        ShaderParameterType_Integer2,
        ShaderParameterType_Integer3,
        ShaderParameterType_Integer4,

        ShaderParameterType_Float2x2,
        ShaderParameterType_Float3x3,
        ShaderParameterType_Float4x4,

        ShaderParameterType_Texture1D,
        ShaderParameterType_Texture2D,
        ShaderParameterType_Texture3D,
        ShaderParameterType_TextureCube,
    };

    /// Base class for shader parameters.
    struct ShaderParameter
    {
        /// The type of this property. We use this for casting to the correct type.
        ShaderParameterType type;

        /// Constructor.
        ShaderParameter(ShaderParameterType type)
            : type(type)
        {
        }

        /// Copy constructor.
        ShaderParameter(const ShaderParameter &other)
            : type(other.type)
        {
        }

        /// Destructor.
        virtual ~ShaderParameter() {}
    };


    struct ShaderParameter_Float : public ShaderParameter
    {
        float value;

        ShaderParameter_Float(float valueIn)
            : ShaderParameter(ShaderParameterType_Float), value(valueIn)
        {
        }

        ShaderParameter_Float(const ShaderParameter_Float &other)
            : ShaderParameter(other), value(other.value)
        {
        }
    };

    struct ShaderParameter_Float2 : public ShaderParameter
    {
        glm::vec2 value;

        ShaderParameter_Float2(const glm::vec2 &valueIn)
            : ShaderParameter(ShaderParameterType_Float2), value(valueIn)
        {
        }

        ShaderParameter_Float2(const ShaderParameter_Float2 &other)
            : ShaderParameter(other), value(other.value)
        {
        }
    };

    struct ShaderParameter_Float3 : public ShaderParameter
    {
        glm::vec3 value;

        ShaderParameter_Float3(const glm::vec3 &valueIn)
            : ShaderParameter(ShaderParameterType_Float3), value(valueIn)
        {
        }

        ShaderParameter_Float3(const ShaderParameter_Float3 &other)
            : ShaderParameter(other), value(other.value)
        {
        }
    };

    struct ShaderParameter_Float4 : public ShaderParameter
    {
        glm::vec4 value;

        ShaderParameter_Float4(const glm::vec4 &valueIn)
            : ShaderParameter(ShaderParameterType_Float4), value(valueIn)
        {
        }

        ShaderParameter_Float4(const ShaderParameter_Float4 &other)
            : ShaderParameter(other), value(other.value)
        {
        }
    };


    struct ShaderParameter_Float2x2 : public ShaderParameter
    {
        glm::mat2 value;

        ShaderParameter_Float2x2(const glm::mat2 &valueIn)
            : ShaderParameter(ShaderParameterType_Float2x2), value(valueIn)
        {
        }

        ShaderParameter_Float2x2(const ShaderParameter_Float2x2 &other)
            : ShaderParameter(other), value(other.value)
        {
        }
    };

    struct ShaderParameter_Float3x3 : public ShaderParameter
    {
        glm::mat3 value;

        ShaderParameter_Float3x3(const glm::mat3 &valueIn)
            : ShaderParameter(ShaderParameterType_Float3x3), value(valueIn)
        {
        }

        ShaderParameter_Float3x3(const ShaderParameter_Float3x3 &other)
            : ShaderParameter(other), value(other.value)
        {
        }
    };

    struct ShaderParameter_Float4x4 : public ShaderParameter
    {
        glm::mat4 value;

        ShaderParameter_Float4x4(const glm::mat4 &valueIn)
            : ShaderParameter(ShaderParameterType_Float4x4), value(valueIn)
        {
        }

        ShaderParameter_Float4x4(const ShaderParameter_Float4x4 &other)
            : ShaderParameter(other), value(other.value)
        {
        }
    };


    struct ShaderParameter_Texture2D : public ShaderParameter
    {
        Texture2D* value;

        ShaderParameter_Texture2D(Texture2D* valueIn)
            : ShaderParameter(ShaderParameterType_Texture2D), value(Texture2DLibrary::Acquire(valueIn))
        {
        }

        ShaderParameter_Texture2D(const ShaderParameter_Texture2D &other)
            : ShaderParameter(other), value(Texture2DLibrary::Acquire(other.value))
        {
        }

        ~ShaderParameter_Texture2D()
        {
            Texture2DLibrary::Unacquire(this->value);
        }


        ShaderParameter_Texture2D & operator=(const ShaderParameter_Texture2D &other)
        {
            ShaderParameter::operator=(other);

            Texture2DLibrary::Unacquire(this->value);
            this->value = Texture2DLibrary::Acquire(other.value);

            return *this;
        }
    };

    struct ShaderParameter_TextureCube : public ShaderParameter
    {
        TextureCube* value;

        ShaderParameter_TextureCube(TextureCube* valueIn)
            : ShaderParameter(ShaderParameterType_TextureCube), value(valueIn)
        {
        }

        ShaderParameter_TextureCube(const ShaderParameter_TextureCube &other)
            : ShaderParameter(other), value(other.value)
        {
        }


        ShaderParameter_TextureCube & operator=(const ShaderParameter_TextureCube &other)
        {
            ShaderParameter_TextureCube::operator=(other);
            this->value = other.value;

            return *this;
        }
    };
}

#endif
