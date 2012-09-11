
#ifndef __GTEngine_ShaderParameter_hpp_
#define __GTEngine_ShaderParameter_hpp_

#define GLM_FORCE_ONLY_XYZW
#include <glm/glm.hpp>

#define GTENGINE_SHADERPARAMETER_DECL_COPYCTOR(typeName, typeID) \
        typeName(const ShaderParameter* otherIn) \
            : ShaderParameter(typeID), value() \
        { \
            auto other = typeName::Upcast(otherIn); \
            assert(other != nullptr); \
            \
            this->value = other->value; \
        } \

#define GTENGINE_SHADERPARAMETER_DECL_SETONCURRENTSHADER() \
        void SetOnCurrentShader(const char* name) const; \

#define GTENGINE_SHADERPARAMETER_DECL_UPCAST(typeName, typeID) \
        static const typeName* Upcast(const ShaderParameter* self) \
        { \
            if (self->type == typeID) \
            { \
                return static_cast<const typeName*>(self); \
            } \
            \
            return nullptr; \
        } \
        \
        static typeName* Upcast(ShaderParameter* self) \
        { \
            if (self->type == typeID) \
            { \
                return static_cast<typeName*>(self); \
            } \
            \
            return nullptr; \
        } \


#define GTENGINE_SHADERPARAMETER_DECL(typeName, typeID, valueType) \
    struct typeName : public ShaderParameter \
    { \
        valueType value; \
        \
        typeName(const valueType &valueIn) \
            : ShaderParameter(typeID), value(valueIn) \
        { \
        } \
        \
        GTENGINE_SHADERPARAMETER_DECL_COPYCTOR(typeName, typeID) \
        GTENGINE_SHADERPARAMETER_DECL_SETONCURRENTSHADER() \
        GTENGINE_SHADERPARAMETER_DECL_UPCAST(typeName, typeID) \
    }; \

#define GTENGINE_SHADERPARAMETER_DECL_PTR(typeName, typeID, valueType) \
    struct typeName : public ShaderParameter \
    { \
        valueType* value; \
        \
        typeName(valueType* valueIn) \
            : ShaderParameter(typeID), value(valueIn) \
        { \
        } \
        \
        GTENGINE_SHADERPARAMETER_DECL_COPYCTOR(typeName, typeID) \
        GTENGINE_SHADERPARAMETER_DECL_SETONCURRENTSHADER() \
        GTENGINE_SHADERPARAMETER_DECL_UPCAST(typeName, typeID) \
        \
    private: \
        typeName(const typeName &); \
        typeName & operator=(const typeName &); \
    }; \


namespace GTEngine
{
    class Texture2D;
    class TextureCube;

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
        virtual void SetOnCurrentShader(const char* name) const = 0;
    };

    GTENGINE_SHADERPARAMETER_DECL(ShaderParameter_Float,    ShaderParameterType_Float,    float);
    GTENGINE_SHADERPARAMETER_DECL(ShaderParameter_Float2,   ShaderParameterType_Float2,   glm::vec2);
    GTENGINE_SHADERPARAMETER_DECL(ShaderParameter_Float3,   ShaderParameterType_Float3,   glm::vec3);
    GTENGINE_SHADERPARAMETER_DECL(ShaderParameter_Float4,   ShaderParameterType_Float4,   glm::vec4);
    GTENGINE_SHADERPARAMETER_DECL(ShaderParameter_Float2x2, ShaderParameterType_Float2x2, glm::mat2);
    GTENGINE_SHADERPARAMETER_DECL(ShaderParameter_Float3x3, ShaderParameterType_Float3x3, glm::mat3);
    GTENGINE_SHADERPARAMETER_DECL(ShaderParameter_Float4x4, ShaderParameterType_Float4x4, glm::mat4);

    //GTENGINE_SHADERPARAMETER_DECL_PTR(ShaderParameter_Texture1D,   ShaderParameterType_Texture1D,   Texture1D);
    GTENGINE_SHADERPARAMETER_DECL_PTR(ShaderParameter_Texture2D,   ShaderParameterType_Texture2D,   Texture2D);
    //GTENGINE_SHADERPARAMETER_DECL_PTR(ShaderParameter_Texture3D,   ShaderParameterType_Texture3D,   Texture3D);
    GTENGINE_SHADERPARAMETER_DECL_PTR(ShaderParameter_TextureCube, ShaderParameterType_TextureCube, TextureCube);



    /// Creates a copy of the given material property. Delete the property with 'delete'.
    inline ShaderParameter* CopyShaderParameter(const ShaderParameter* propToCopy)
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
                //newProp = new ShaderParameter_Texture1D(propToCopy);
                break;
            }
            
        case ShaderParameterType_Texture2D:
            {
                newProp = new ShaderParameter_Texture2D(propToCopy);
                break;
            }
        
        case ShaderParameterType_Texture3D:
            {
                //newProp = new ShaderParameter_Texture3D(propToCopy);
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
