
#ifndef __GTEngine_Material_hpp_
#define __GTEngine_Material_hpp_

#include <GTCore/String.hpp>
#include "ShaderParameterCache.hpp"

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

        

    public:

        /// The ID of the diffuse shader.
        GTCore::String diffuseShaderID;

        /// The ID of the emissive shader.
        GTCore::String emissiveShaderID;

        /// The ID of the shininess shader.
        GTCore::String shininessShaderID;

        /// The ID of the normal shader.
        GTCore::String normalShaderID;

        /// The default parameters.
        ShaderParameterCache defaultParams;


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

        /// Retrieves a reference to the list of default parameters.
        const GTCore::Dictionary<ShaderParameter*> & DetDefaultParameters() const { return this->definition.defaultParams.GetParameters(); }


        /// Retrieves a reference to the list of parameters for this material.
        const GTCore::Dictionary<ShaderParameter*> & GetParameters() const { return this->parameters.GetParameters(); }


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
    };
}

#endif