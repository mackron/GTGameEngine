// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_ShaderParameterCache_hpp_
#define __GTEngine_ShaderParameterCache_hpp_

#include "ShaderParameter.hpp"
#include "Math.hpp"
#include "Serialization.hpp"
#include <GTEngine/Core/Dictionary.hpp>

namespace GTEngine
{
    class Texture2D;
    class TextureCube;

    /// Class representing a cache of shader parameters.
    class ShaderParameterCache
    {
    public:

        /// Constructor.
        ShaderParameterCache();

        /// Copy constructor.
        ShaderParameterCache(const ShaderParameterCache &other);

        /// Destructor.
        ~ShaderParameterCache();


        /// Setters.
        void Set(const char* name, float x);
        void Set(const char* name, float x, float y)                   { this->Set(name, glm::vec2(x, y)); }
        void Set(const char* name, float x, float y, float z)          { this->Set(name, glm::vec3(x, y, z)); }
        void Set(const char* name, float x, float y, float z, float w) { this->Set(name, glm::vec4(x, y, z, w)); }
        void Set(const char* name, const glm::vec2 &v);
        void Set(const char* name, const glm::vec3 &v);
        void Set(const char* name, const glm::vec4 &v);

#if 0
        void Set(const char* name, int x);
        void Set(const char* name, int x, int y)               { this->Set(name, glm::vec2(x, y)); }
        void Set(const char* name, int x, int y, int z)        { this->Set(name, glm::vec3(x, y, z)); }
        void Set(const char* name, int x, int y, int z, int w) { this->Set(name, glm::vec4(x, y, z, w)); }
        void Set(const char* name, const glm::ivec2 &v);
        void Set(const char* name, const glm::ivec3 &v);
        void Set(const char* name, const glm::ivec4 &v);
#endif

        void Set(const char* name, const glm::mat2 &v);
        void Set(const char* name, const glm::mat3 &v);
        void Set(const char* name, const glm::mat4 &v);

        void Set(const char* name, Texture2D*   texture);
        void Set(const char* name, TextureCube* texture);


        /// Unsets a parameter by name.
        void UnsetFloat(const char* name);
        void UnsetFloat2(const char* name);
        void UnsetFloat3(const char* name);
        void UnsetFloat4(const char* name);
        void UnsetFloat2x2(const char* name);
        void UnsetFloat3x3(const char* name);
        void UnsetFloat4x4(const char* name);
        void UnsetTexture2D(const char* name);
        void UnsetTextureCube(const char* name);


        /// Retrieves a pointer to the given parameter.
        ///
        /// @remarks
        ///     If the parameter does not exist, null will be returned.
        ///     @par
        ///     The returned pointer will be made invalid when a parameter is set or unset.
        const ShaderParameter_Float*       GetFloatParameter(const char* name) const;
        const ShaderParameter_Float2*      GetFloat2Parameter(const char* name) const;
        const ShaderParameter_Float3*      GetFloat3Parameter(const char* name) const;
        const ShaderParameter_Float4*      GetFloat4Parameter(const char* name) const;
        const ShaderParameter_Float2x2*    GetFloat2x2Parameter(const char* name) const;
        const ShaderParameter_Float3x3*    GetFloat3x3Parameter(const char* name) const;
        const ShaderParameter_Float4x4*    GetFloat4x4Parameter(const char* name) const;
        const ShaderParameter_Texture2D*   GetTexture2DParameter(const char* name) const;
        const ShaderParameter_TextureCube* GetTextureCubeParameter(const char* name) const;

        
        /// Sets the parameters from another parameter list.
        void SetParameters(const ShaderParameterCache &other);

        /// Unsets the given parameters.
        void UnsetParameters(const ShaderParameterCache &other);

        /// Updates parameters that are set, but unchanged compared to the given parameters.
        void UpdateUnchangedParameters(const ShaderParameterCache &oldParameters);



        /// Clears the cache.
        void Clear();


        /// Retrieves the internal list of parameters.
        const GTLib::Dictionary<ShaderParameter_Float>       & GetFloatParameters()       const { return this->floatParameters;       }
        const GTLib::Dictionary<ShaderParameter_Float2>      & GetFloat2Parameters()      const { return this->float2Parameters;      }
        const GTLib::Dictionary<ShaderParameter_Float3>      & GetFloat3Parameters()      const { return this->float3Parameters;      }
        const GTLib::Dictionary<ShaderParameter_Float4>      & GetFloat4Parameters()      const { return this->float4Parameters;      }
        const GTLib::Dictionary<ShaderParameter_Float2x2>    & GetFloat2x2Parameters()    const { return this->float2x2Parameters;    }
        const GTLib::Dictionary<ShaderParameter_Float3x3>    & GetFloat3x3Parameters()    const { return this->float3x3Parameters;    }
        const GTLib::Dictionary<ShaderParameter_Float4x4>    & GetFloat4x4Parameters()    const { return this->float4x4Parameters;    }
        const GTLib::Dictionary<ShaderParameter_Texture2D>   & GetTexture2DParameters()   const { return this->texture2DParameters;   }
        const GTLib::Dictionary<ShaderParameter_TextureCube> & GetTextureCubeParameters() const { return this->textureCubeParameters; }



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the parameter cache.
        ///
        /// @param serializer [in] A reference to the serializer to write to.
        void Serialize(GTLib::Serializer &serializer) const;

        /// Deserializes the parameter cache.
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        void Deserialize(GTLib::Deserializer &deserializer);


    private:

        GTLib::Dictionary<ShaderParameter_Float>       floatParameters;
        GTLib::Dictionary<ShaderParameter_Float2>      float2Parameters;
        GTLib::Dictionary<ShaderParameter_Float3>      float3Parameters;
        GTLib::Dictionary<ShaderParameter_Float4>      float4Parameters;
        GTLib::Dictionary<ShaderParameter_Float2x2>    float2x2Parameters;
        GTLib::Dictionary<ShaderParameter_Float3x3>    float3x3Parameters;
        GTLib::Dictionary<ShaderParameter_Float4x4>    float4x4Parameters;
        GTLib::Dictionary<ShaderParameter_Texture2D>   texture2DParameters;
        GTLib::Dictionary<ShaderParameter_TextureCube> textureCubeParameters;




        ///////////////////////////////////////////
        // Static
    public:

        /// Performs a difference operation, returning the difference and intersections.
        static void Difference(const ShaderParameterCache &parametersA, const ShaderParameterCache &parametersB, ShaderParameterCache &difference);
    };
}


#endif