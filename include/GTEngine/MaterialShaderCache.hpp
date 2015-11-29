// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_MaterialShaderCache_hpp_
#define __GTEngine_MaterialShaderCache_hpp_

#include "Material.hpp"
#include <GTEngine/Core/Map.hpp>

namespace GTEngine
{
    class Shader;

    /// A cache of material shaders. This is used for easily determining if a shader exists for a given material definition.
    ///
    /// This class does not allocate/deallocate the shader objects. That is managed at a higher level. At deallocation time,
    /// an application can call GetShaders() to retrieve a map of material/shader combinations if needed.
    class MaterialShaderCache
    {
    public:

        /// Constructor.
        MaterialShaderCache();

        /// Destructor.
        ~MaterialShaderCache();


        /// Finds a shader attached to the given material definition. Returns null if it does not exist.
        Shader* GetShader(const MaterialDefinition &material);
        Shader* GetShader(const Material &material) { return this->GetShader(material.GetDefinition()); }

        /// Sets the shader associated with the given material definition.
        void SetShader(const MaterialDefinition &material, Shader &shader);
        void SetShader(const Material &material, Shader &shader) { this->SetShader(material.GetDefinition(), shader); }


        /// Retrieves the internal map of shaders. Use this to gain access to the shaders for deletion.
        ///
        /// @remarks
        ///     Use the following as an example for deleting the shader objects.
        ///     \code
        ///     auto &shaders = someMaterialCache.GetShaders();
        ///     for (size_t i = 0; i < shaders.count; ++i)
        ///     {
        ///         delete shaders.buffer[i]->value;
        ///     }
        ///     \endcode
        GTLib::Map<const MaterialDefinition*, Shader*> & GetShaders() { return this->shaders; }


    private:

        /// A map of MaterialDefinition/Shader combinations. We index with a material definition point, with the shader as
        /// the value. We use a map here for fast searching.
        GTLib::Map<const MaterialDefinition*, Shader*> shaders;
    };
}
#endif