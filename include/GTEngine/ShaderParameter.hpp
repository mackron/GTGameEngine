// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ShaderParameter_hpp_
#define __GTEngine_ShaderParameter_hpp_

#include "Texture2DLibrary.hpp"
#include "Rendering/TextureCube.hpp"

#define GLM_FORCE_ONLY_XYZW
#include <glm/glm.hpp>


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
    };



#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunreachable-code"
#endif

    /// Creates a copy of the given material property. Delete the property with 'delete'.
    inline ShaderParameter* CopyShaderParameter(const ShaderParameter* propToCopy)              // <-- TODO: Should be able to get rid of this with the new system.
    {
        assert(propToCopy != nullptr);

        ShaderParameter* newProp = nullptr;

        switch (propToCopy->type)
        {
        case ShaderParameterType_Float:
            {
                newProp = new ShaderParameter_Float(static_cast<const ShaderParameter_Float &>(*propToCopy));
                break;
            }
        case ShaderParameterType_Float2:
            {
                newProp = new ShaderParameter_Float2(static_cast<const ShaderParameter_Float2 &>(*propToCopy));
                break;
            }
        case ShaderParameterType_Float3:
            {
                newProp = new ShaderParameter_Float3(static_cast<const ShaderParameter_Float3 &>(*propToCopy));
                break;
            }
        case ShaderParameterType_Float4:
            {
                newProp = new ShaderParameter_Float4(static_cast<const ShaderParameter_Float4 &>(*propToCopy));
                break;
            }


        case ShaderParameterType_Integer:
            {
                //newProp = new ShaderParameter_Integer(propToCopy);
                break;
            }
        case ShaderParameterType_Integer2:
            {
                //newProp = new ShaderParameter_Integer2(propToCopy);
                break;
            }
        case ShaderParameterType_Integer3:
            {
                //newProp = new ShaderParameter_Integer3(propToCopy);
                break;
            }
        case ShaderParameterType_Integer4:
            {
                //newProp = new ShaderParameter_Integer4(propToCopy);
                break;
            }


        case ShaderParameterType_Float2x2:
            {
                newProp = new ShaderParameter_Float2x2(static_cast<const ShaderParameter_Float2x2 &>(*propToCopy));
                break;
            }
        case ShaderParameterType_Float3x3:
            {
                newProp = new ShaderParameter_Float3x3(static_cast<const ShaderParameter_Float3x3 &>(*propToCopy));
                break;
            }
        case ShaderParameterType_Float4x4:
            {
                newProp = new ShaderParameter_Float4x4(static_cast<const ShaderParameter_Float4x4 &>(*propToCopy));
                break;
            }


        case ShaderParameterType_Texture1D:
            {
                //newProp = new ShaderParameter_Texture1D(propToCopy);
                break;
            }

        case ShaderParameterType_Texture2D:
            {
                newProp = new ShaderParameter_Texture2D(static_cast<const ShaderParameter_Texture2D &>(*propToCopy));
                break;
            }

        case ShaderParameterType_Texture3D:
            {
                //newProp = new ShaderParameter_Texture3D(propToCopy);
                break;
            }

        case ShaderParameterType_TextureCube:
            {
                newProp = new ShaderParameter_TextureCube(static_cast<const ShaderParameter_TextureCube &>(*propToCopy));
                break;
            }


        default: break;
        };

        return newProp;
    }


    /// Compares two shader parameters for equality.
    inline bool CompareShaderParameters(const ShaderParameter* parameterA, const ShaderParameter* parameterB)
    {
        assert(parameterA != nullptr);
        assert(parameterB != nullptr);
        {
            if (parameterA->type == parameterB->type)
            {
                switch (parameterA->type)
                {
                case ShaderParameterType_Float:
                    {
                        return static_cast<const ShaderParameter_Float*>(parameterA)->value == static_cast<const ShaderParameter_Float*>(parameterB)->value;
                    }
                case ShaderParameterType_Float2:
                    {
                        return static_cast<const ShaderParameter_Float2*>(parameterA)->value == static_cast<const ShaderParameter_Float2*>(parameterB)->value;
                    }
                case ShaderParameterType_Float3:
                    {
                        return static_cast<const ShaderParameter_Float3*>(parameterA)->value == static_cast<const ShaderParameter_Float3*>(parameterB)->value;
                    }
                case ShaderParameterType_Float4:
                    {
                        return static_cast<const ShaderParameter_Float4*>(parameterA)->value == static_cast<const ShaderParameter_Float4*>(parameterB)->value;
                    }


                case ShaderParameterType_Integer:
                    {
                        //return static_cast<const ShaderParameter_Integer*>(parameterA)->value == static_cast<const ShaderParameter_Integer*>(parameterB)->value;
                        break;
                    }
                case ShaderParameterType_Integer2:
                    {
                        //return static_cast<const ShaderParameter_Integer2*>(parameterA)->value == static_cast<const ShaderParameter_Integer2*>(parameterB)->value;
                        break;
                    }
                case ShaderParameterType_Integer3:
                    {
                        //return static_cast<const ShaderParameter_Integer3*>(parameterA)->value == static_cast<const ShaderParameter_Integer3*>(parameterB)->value;
                        break;
                    }
                case ShaderParameterType_Integer4:
                    {
                        //return static_cast<const ShaderParameter_Integer4*>(parameterA)->value == static_cast<const ShaderParameter_Integer4*>(parameterB)->value;
                        break;
                    }


                case ShaderParameterType_Float2x2:
                    {
                        return static_cast<const ShaderParameter_Float2x2*>(parameterA)->value == static_cast<const ShaderParameter_Float2x2*>(parameterB)->value;
                    }
                case ShaderParameterType_Float3x3:
                    {
                        return static_cast<const ShaderParameter_Float3x3*>(parameterA)->value == static_cast<const ShaderParameter_Float3x3*>(parameterB)->value;
                    }
                case ShaderParameterType_Float4x4:
                    {
                        return static_cast<const ShaderParameter_Float4x4*>(parameterA)->value == static_cast<const ShaderParameter_Float4x4*>(parameterB)->value;
                    }


                case ShaderParameterType_Texture1D:
                    {
                        break;
                    }

                case ShaderParameterType_Texture2D:
                    {
                        return static_cast<const ShaderParameter_Texture2D*>(parameterA)->value == static_cast<const ShaderParameter_Texture2D*>(parameterB)->value;
                    }

                case ShaderParameterType_Texture3D:
                    {
                        break;
                    }

                case ShaderParameterType_TextureCube:
                    {
                        return static_cast<const ShaderParameter_TextureCube*>(parameterA)->value == static_cast<const ShaderParameter_TextureCube*>(parameterB)->value;
                    }


                default: break;
                }
            }
        }

        return false;
    }

    
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif
};

#endif
