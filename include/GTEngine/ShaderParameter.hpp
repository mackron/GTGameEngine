
#ifndef __GTEngine_ShaderParameter_hpp_
#define __GTEngine_ShaderParameter_hpp_

#define GLM_FORCE_ONLY_XYZW
#include <glm/glm.hpp>

#include "Rendering/Renderer.hpp"
#include "Texture2DLibrary.hpp"

namespace GTEngine
{
    /// Enumerator for the different types of material properties.
    enum ShaderParameterType
    {
        ShaderParameterType_Float,
        ShaderParameterType_Float2,
        ShaderParameterType_Float3,
        ShaderParameterType_Float4,

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

        /// Destructor.
        virtual ~ShaderParameter()
        {
        }

        /// Virtual method for applying the value onto the current shader. Use Renderer::SetShaderParameter(). This method
        /// is intended to be called from the rendering thread.
        virtual void SetOnCurrentShader(const char* name) = 0;
    };

    struct ShaderParameter_Float : public ShaderParameter
    {
        float value;

        /// Constructor.
        ShaderParameter_Float(float value)
            : ShaderParameter(ShaderParameterType_Float), value(value)
        {
        }

        /// Copy constructor.
        ShaderParameter_Float(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Float), value()
        {
            auto other = ShaderParameter_Float::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }

        /// Shader setter.
        void SetOnCurrentShader(const char* name) { Renderer::SetShaderParameter(name, this->value); }

        /// Upcaster.
        static ShaderParameter_Float* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Float)
            {
                return static_cast<ShaderParameter_Float*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_Float2 : public ShaderParameter
    {
        glm::vec2 value;
        
        /// Constructor.
        ShaderParameter_Float2(const glm::vec2 &value)
            : ShaderParameter(ShaderParameterType_Float2), value(value)
        {
        }

        /// Constructor.
        ShaderParameter_Float2(float x, float y)
            : ShaderParameter(ShaderParameterType_Float2), value(x, y)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_Float2(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Float2), value()
        {
            auto other = ShaderParameter_Float2::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }

        /// Shader setter.
        void SetOnCurrentShader(const char* name) { Renderer::SetShaderParameter(name, this->value); }

        /// Upcaster.
        static ShaderParameter_Float2* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Float2)
            {
                return static_cast<ShaderParameter_Float2*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_Float3 : public ShaderParameter
    {
        glm::vec3 value;

        /// Constructor.
        ShaderParameter_Float3(const glm::vec3 &value)
            : ShaderParameter(ShaderParameterType_Float3), value(value)
        {
        }
        
        /// Constructor.
        ShaderParameter_Float3(float x, float y, float z)
            : ShaderParameter(ShaderParameterType_Float3), value(x, y, z)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_Float3(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Float3), value()
        {
            auto other = ShaderParameter_Float3::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }
        
        /// Shader setter.
        void SetOnCurrentShader(const char* name) { Renderer::SetShaderParameter(name, this->value); }

        /// Upcaster.
        static ShaderParameter_Float3* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Float3)
            {
                return static_cast<ShaderParameter_Float3*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_Float4 : public ShaderParameter
    {
        glm::vec4 value;

        /// Constructor.
        ShaderParameter_Float4(const glm::vec4 &value)
            : ShaderParameter(ShaderParameterType_Float4), value(value)
        {
        }
        
        /// Constructor.
        ShaderParameter_Float4(float x, float y, float z, float w)
            : ShaderParameter(ShaderParameterType_Float4), value(x, y, z, w)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_Float4(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Float4), value()
        {
            auto other = ShaderParameter_Float4::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }
        
        /// Shader setter.
        void SetOnCurrentShader(const char* name) { Renderer::SetShaderParameter(name, this->value); }

        /// Upcaster.
        static ShaderParameter_Float4* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Float4)
            {
                return static_cast<ShaderParameter_Float4*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_Float2x2 : public ShaderParameter
    {
        glm::mat2 value;
        
        /// Constructor.
        ShaderParameter_Float2x2(const glm::mat2 &value)
            : ShaderParameter(ShaderParameterType_Float2x2), value(value)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_Float2x2(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Float2x2), value()
        {
            auto other = ShaderParameter_Float2x2::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }
        
        /// Shader setter.
        void SetOnCurrentShader(const char* name) { Renderer::SetShaderParameter(name, this->value); }

        /// Upcaster.
        static ShaderParameter_Float2x2* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Float2x2)
            {
                return static_cast<ShaderParameter_Float2x2*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_Float3x3 : public ShaderParameter
    {
        glm::mat3 value;
        
        /// Constructor.
        ShaderParameter_Float3x3(const glm::mat3 &value)
            : ShaderParameter(ShaderParameterType_Float3x3), value(value)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_Float3x3(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Float3x3), value()
        {
            auto other = ShaderParameter_Float3x3::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }
        
        /// Shader setter.
        void SetOnCurrentShader(const char* name) { Renderer::SetShaderParameter(name, this->value); }

        /// Upcaster.
        static ShaderParameter_Float3x3* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Float3x3)
            {
                return static_cast<ShaderParameter_Float3x3*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_Float4x4 : public ShaderParameter
    {
        glm::mat4 value;
        
        /// Constructor.
        ShaderParameter_Float4x4(const glm::mat4 &value)
            : ShaderParameter(ShaderParameterType_Float4x4), value(value)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_Float4x4(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Float4x4), value()
        {
            auto other = ShaderParameter_Float4x4::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }

        /// Shader setter.
        void SetOnCurrentShader(const char* name) { Renderer::SetShaderParameter(name, this->value); }

        /// Upcaster.
        static ShaderParameter_Float4x4* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Float4x4)
            {
                return static_cast<ShaderParameter_Float4x4*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_Texture1D : public ShaderParameter
    {
        int value;
        
        /// Constructor.
        ShaderParameter_Texture1D(int value)
            : ShaderParameter(ShaderParameterType_Texture1D), value(value)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_Texture1D(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Texture1D), value()
        {
            auto other = ShaderParameter_Texture1D::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }
        
        /// Shader setter.
        void SetOnCurrentShader(const char*) {  }

        /// Upcaster.
        static ShaderParameter_Texture1D* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Texture1D)
            {
                return static_cast<ShaderParameter_Texture1D*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_Texture2D : public ShaderParameter
    {
        Texture2D* value;
        
        /// Constructor.
        ShaderParameter_Texture2D(Texture2D* value)
            : ShaderParameter(ShaderParameterType_Texture2D), value(value)
        {
        }

        /// Copy constructor.
        ShaderParameter_Texture2D(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Texture2D), value(nullptr)
        {
            auto other = ShaderParameter_Texture2D::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }

        /// Destructor. Needed for unacquiring the texture.
        ~ShaderParameter_Texture2D()
        {
        }
        
        /// Shader setter.
        void SetOnCurrentShader(const char* name) { Renderer::SetShaderParameter(name, this->value); }

        /// Upcaster.
        static ShaderParameter_Texture2D* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Texture2D)
            {
                return static_cast<ShaderParameter_Texture2D*>(self);
            }

            return nullptr;
        }
    };
    
    struct ShaderParameter_Texture3D : public ShaderParameter
    {
        int value;
        
        /// Constructor.
        ShaderParameter_Texture3D(int value)
            : ShaderParameter(ShaderParameterType_Texture3D), value(value)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_Texture3D(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_Texture3D), value()
        {
            auto other = ShaderParameter_Texture3D::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }
        
        /// Shader setter.
        void SetOnCurrentShader(const char*) {  }

        /// Upcaster.
        static ShaderParameter_Texture3D* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_Texture3D)
            {
                return static_cast<ShaderParameter_Texture3D*>(self);
            }

            return nullptr;
        }
    };

    struct ShaderParameter_TextureCube : public ShaderParameter
    {
        int value;
        
        /// Constructor.
        ShaderParameter_TextureCube(int value)
            : ShaderParameter(ShaderParameterType_TextureCube), value(value)
        {
        }
        
        /// Copy constructor.
        ShaderParameter_TextureCube(ShaderParameter* otherIn)
            : ShaderParameter(ShaderParameterType_TextureCube), value()
        {
            auto other = ShaderParameter_TextureCube::Upcast(otherIn);
            assert(other != nullptr);

            this->value = other->value;
        }
        
        /// Shader setter.
        void SetOnCurrentShader(const char*) {  }

        /// Upcaster.
        static ShaderParameter_TextureCube* Upcast(ShaderParameter* self)
        {
            if (self->type == ShaderParameterType_TextureCube)
            {
                return static_cast<ShaderParameter_TextureCube*>(self);
            }

            return nullptr;
        }
    };



    /// Creates a copy of the given material property. Delete the property with 'delete'.
    inline ShaderParameter* CopyShaderParameter(ShaderParameter* propToCopy)
    {
        assert(propToCopy != nullptr);

        ShaderParameter* newProp = nullptr;

        switch (propToCopy->type)
        {
        case ShaderParameterType_Float:
            {
                newProp = new ShaderParameter_Float(propToCopy);
                break;
            }
        case ShaderParameterType_Float2:
            {
                newProp = new ShaderParameter_Float2(propToCopy);
                break;
            }
        case ShaderParameterType_Float3:
            {
                newProp = new ShaderParameter_Float3(propToCopy);
                break;
            }
        case ShaderParameterType_Float4:
            {
                newProp = new ShaderParameter_Float4(propToCopy);
                break;
            }


        case ShaderParameterType_Float2x2:
            {
                newProp = new ShaderParameter_Float2x2(propToCopy);
                break;
            }
        case ShaderParameterType_Float3x3:
            {
                newProp = new ShaderParameter_Float3x3(propToCopy);
                break;
            }
        case ShaderParameterType_Float4x4:
            {
                newProp = new ShaderParameter_Float4x4(propToCopy);
                break;
            }


        case ShaderParameterType_Texture1D:
            {
                newProp = new ShaderParameter_Texture1D(propToCopy);
                break;
            }
        case ShaderParameterType_Texture2D:
            {
                newProp = new ShaderParameter_Texture2D(propToCopy);
                break;
            }
        case ShaderParameterType_Texture3D:
            {
                newProp = new ShaderParameter_Texture3D(propToCopy);
                break;
            }
        case ShaderParameterType_TextureCube:
            {
                newProp = new ShaderParameter_TextureCube(propToCopy);
                break;
            }


        default: break;
        };

        return newProp;
    }
};

#endif