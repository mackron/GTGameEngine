// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Material_hpp_
#define __GTEngine_Material_hpp_

#include <GTCore/String.hpp>
#include <GTCore/Map.hpp>
#include "ShaderParameterCache.hpp"
#include "Serialization.hpp"
#include "Rendering/BlendEquations.hpp"
#include "Rendering/BlendFunctions.hpp"

namespace GTEngine
{
    /// Class representing a materials structure. XML material definitions are converted into a descriptor. The material library
    /// uses descriptors for efficient material storage and creation. It allows the material definition to be stored in a way
    /// where the XML doesn't need to be parsed every time a material is instantiated.
    ///
    /// Use LoadFromXML() or LoadFromFile() to load the material definition. This will create and shader and texture objects,
    /// using the respective libraries (ShaderLibrary for shaders and ImageLibrary for textures).
    class MaterialDefinition
    {
    public:

        /// Default constructor. Use LoadFromXML() to load the definition from an XML script.
        MaterialDefinition();

        /// Destructor.
        ~MaterialDefinition();


        /// Loads the material definition from an XML script.
        /// @param xml [in] The XML script.
        /// @return True if the material definition is loaded successfully; false otherwise.
        bool LoadFromXML(const char* xml);
        bool LoadFromXML(char* xml);        // <-- Needed because rapidxml takes a non-const string.

        /// Helper method for loading the material definition from a file.
        bool LoadFromFile(const char* fileName);



        /// Sets the metadata pointer to use with the given key.
        ///
        /// @param key  [in] The key to use with the given pointer.
        /// @param data [in] The pointer to associate with the given key.
        ///
        /// @remarks
        ///     The pointer can be retrieved with GetMetadata().
        ///     @par
        ///     If a pointer already exists for the given key, it is overwritten.
        void SetMetadata(size_t key, void* data);

        /// Retrieves the metadata associated with the given key.
        ///
        /// @param key [in] The key of the metadata being retrieved.
        ///
        /// @return A pointer to the metadata associated with the key or null if there is no metadata for that key.
        void* GetMetadata(size_t key);

        /// Removes a metadata value.
        ///
        /// @param key [in] The key of the metadata to remove.
        void RemoveMetadata(size_t key);


        /// Determines whether or not the material is blended.
        bool IsBlended() const { return this->isBlended && !this->isRefractive; }

        /// Retrieves the blending equation to use with this material.
        BlendEquation GetBlendEquation() const { return this->blendEquation; }

        /// Retrieves the source blending factor.
        BlendFunc GetBlendSourceFactor() const { return this->blendSourceFactor; }

        /// Retrieves the destination blending factor.
        BlendFunc GetBlendDestinationFactor() const { return this->blendDestinationFactor; }



        /// Determines whether or not the material is refractive.
        bool IsRefractive() const { return this->isRefractive; }

        

    public:

        /// The name of the file used to create the definition. This will be an empty string if it was not created from a file.
        GTCore::String fileName;

        /// The ID of the diffuse shader.
        GTCore::String diffuseShaderID;

        /// The ID of the emissive shader.
        GTCore::String emissiveShaderID;

        /// The ID of the shininess shader.
        GTCore::String shininessShaderID;

        /// The ID of the normal shader.
        GTCore::String normalShaderID;

        /// The ID of the refraction shader. This can actually be null if this->refractive is false.
        GTCore::String refractionShaderID;

        /// The ID of the specular shader.
        GTCore::String specularShaderID;



        /// The default parameters.
        ShaderParameterCache defaultParams;


        /// Every material can have metadata associated with it. This is one way of doing things like associating a shader to a material or whatnot. The default
        /// renderer will use this system. This is a map allowing something to associate a pointer to a key. The key is a size_t which will allow a pointer to
        /// an object to be used if desired.
        GTCore::Map<size_t, void*> metadata;



    private:

        /// Keeps track of whether or not the material is blended.
        bool isBlended;

        /// Keeps track of whether or not the material is refractive.
        bool isRefractive;


        /// The blending equation.
        BlendEquation blendEquation;

        /// The source blending factor.
        BlendFunc blendSourceFactor;

        /// The destination blending factor.
        BlendFunc blendDestinationFactor;


    private:

        /// Parses a list of floats.
        static void ParseFloatArray(const char* str, float* dest, size_t count);
    };
}

namespace GTEngine
{
    /// Class representing a material.
    ///
    /// A material is defined as an XML script.
    ///
    /// It is recommended to use the material library (MaterialLibrary) to create material objects. This does management operations
    /// such as only loading and parsing a material file once.
    class Material
    {
    public:

        /// Constructor. We don't load anything from the constructor because the material can be loaded from either a file name or
        /// an XML script. There is ambiguity there. Use LoadFromXML() or LoadFromFile() to load the material properly.
        Material(const MaterialDefinition &definition);

        /// Destructor.
        ~Material();

        /// Retrieves a reference to the material's definition.
        const MaterialDefinition & GetDefinition() const { return this->definition; }

        /// Retrieves the ID of the diffuse shader.
        const char* GetDiffuseShaderID() const { return this->definition.diffuseShaderID.c_str(); }

        /// Retrieves the ID of the emissive shader.
        const char* GetEmissiveShaderID() const { return this->definition.emissiveShaderID.c_str(); }

        /// Retrieves the ID of the shininess shader.
        const char* GetShininessShaderID() const { return this->definition.shininessShaderID.c_str(); }

        /// Retrieves the ID of the normals shader.
        const char* GetNormalShaderID() const { return this->definition.normalShaderID.c_str(); }

        /// Retrieves the ID of the specular shader.
        const char* GetSpecularShaderID() const { return this->definition.specularShaderID.c_str(); }

        /// Retrieves the ID of the refraction shader.
        const char* GetRefractionShaderID() const { return this->definition.refractionShaderID.c_str(); }


        /// Retrieves a reference to the list of default parameters.
        const GTCore::Dictionary<ShaderParameter*> & DetDefaultParameters() const { return this->definition.defaultParams.GetParameters(); }


        /// Retrieves a reference to the list of parameters for this material.
        const GTCore::Dictionary<ShaderParameter*> & GetParameters() const { return this->parameters.GetParameters(); }


        /// Determines whether or not the material is blended.
        bool IsBlended() const { return this->definition.IsBlended(); }

        /// Retrieves the blending equation to use with this material.
        BlendEquation GetBlendEquation() const { return this->definition.GetBlendEquation(); }

        /// Retrieves the source blending factor.
        BlendFunc GetBlendSourceFactor() const { return this->definition.GetBlendSourceFactor(); }

        /// Retrieves the destination blending factor.
        BlendFunc GetBlendDestinationFactor() const { return this->definition.GetBlendDestinationFactor(); }


        /// Determines whether or not the material is refractive.
        bool IsRefractive() const { return this->definition.IsRefractive(); }




        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the state of the material.
        ///
        /// @param serializer [in] A reference to the serializer to write to.
        ///
        /// @remarks
        ///     This does not save an actual .material file. Instead it will just save the parameters and their values.
        void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the state of the material.
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        ///
        /// @remarks
        ///     This does NOT load an actual .material file. It just restores parameter states.
        void Deserialize(GTCore::Deserializer &deserializer);



    // Parameter setters.
    public:

        template <typename T>
        void SetParameter(const char* name, const T &value)
        {
            this->parameters.Set(name, value);
        }

        void SetParameter(const char* name, float x, float y)                   { this->SetParameter(name, glm::vec2(x, y)); }
        void SetParameter(const char* name, float x, float y, float z)          { this->SetParameter(name, glm::vec3(x, y, z)); }
        void SetParameter(const char* name, float x, float y, float z, float w) { this->SetParameter(name, glm::vec4(x, y, z, w)); }
        


    private:

        /// The material definition.
        const MaterialDefinition &definition;

        /// The parameters being used by the material.
        ShaderParameterCache parameters;



    private:    // No copying, for now.
        Material(const Material &);
        Material & operator=(const Material &);
    };
}

#endif