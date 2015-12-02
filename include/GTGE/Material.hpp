// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Material
#define GT_Material

#include <GTGE/Core/String.hpp>
#include "ShaderParameterCache.hpp"
#include "Serialization.hpp"
#include "Rendering/BlendEquations.hpp"
#include "Rendering/BlendFunctions.hpp"

namespace GT
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
        ///
        /// @remarks
        ///     If 'fileName' is absolute, a relative path must also be specified. It is an error for 'fileName' to be absolute and 'relativePath' to be null.
        bool LoadFromFile(const char* fileName, const char* relativePath = nullptr);


        /// Retrieves the XML string.
        const char* GetXMLString() const;


        /// Retrieves the shader ID of the given channel.
        ///
        /// @param channelName [in] The name of the channel whose shader ID is being retrieved.
        ///
        /// @return A string containing the shader ID. The string will be empty if the channel is undefined.
        ///
        /// @remarks
        ///     When a channel is not specified, this will return an empty string, in which case the renderer can choose what to do by default.
        String GetChannelShaderID(const char* channelName) const;

        /// Determines whether or not the ID of the given channel is defind.
        ///
        /// @param channelName [in] The name of the channel.
        bool IsChannelDefined(const char* channelName) const;



        /// Determines whether or not the material is blended.
        bool IsBlended() const { return this->isBlended && !this->isRefractive; }

        /// Retrieves the blending equation to use with this material.
        BlendEquation GetBlendEquation() const { return this->blendEquation; }

        /// Retrieves the source blending factor.
        BlendFunc GetBlendSourceFactor() const { return this->blendSourceFactor; }

        /// Retrieves the destination blending factor.
        BlendFunc GetBlendDestinationFactor() const { return this->blendDestinationFactor; }

        /// Retrieves the blend colour.
        const glm::vec4 & GetBlendColour() const { return this->blendColour; }


        /// Determines whether or not the material has a normal channel.
        bool HasNormalChannel() const { return this->hasNormalChannel; }

        /// Determines whether or not the material is refractive.
        bool IsRefractive() const { return this->isRefractive; }


    private:

        /// Resets the definition to it's default state - i.e. clears the material.
        void Reset();
        

    public:

        /// The absolute paht of the material.
        String absolutePath;

        /// The relative path of the material.
        String relativePath;


        /// The XML string of the material.
        String xmlString;


        /// The list of channels, keyed by the channel name.
        Dictionary<String> channelShaderIDs;


        /// The default parameters.
        ShaderParameterCache defaultParams;


    private:

        /// Keeps track of whether or not the material has a normal channel.
        bool hasNormalChannel;

        /// Keeps track of whether or not the material is refractive.
        bool isRefractive;

        /// Keeps track of whether or not the material is blended.
        bool isBlended;


        /// The blending equation.
        BlendEquation blendEquation;

        /// The source blending factor.
        BlendFunc blendSourceFactor;

        /// The destination blending factor.
        BlendFunc blendDestinationFactor;

        /// The colour to use with the "constant" blending factors.
        glm::vec4 blendColour;



    private:

        /// Parses a list of floats.
        static void ParseFloatArray(const char* str, float* dest, size_t count);
    };
}

namespace GT
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



        /// Retrieves the shader ID of the given channel.
        ///
        /// @param channelName [in] The name of the channel whose shader ID is being retrieved.
        ///
        /// @return A string containing the shader ID. The string will be empty if the channel is undefined.
        ///
        /// @remarks
        ///     When a channel is not specified, this will return an empty string, in which case the renderer can choose what to do by default.
        String GetChannelShaderID(const char* channelName) const { return this->definition.GetChannelShaderID(channelName); }

        /// Determines whether or not the ID of the given channel is defind.
        ///
        /// @param channelName [in] The name of the channel.
        bool IsChannelDefined(const char* channelName) const { return this->definition.IsChannelDefined(channelName); }



        /// Retrieves a reference to the list of default parameters.
        const ShaderParameterCache & GetDefaultParameters() const { return this->definition.defaultParams; }

        /// Retrieves a reference to the list of parameters for this material.
        const ShaderParameterCache & GetParameters() const { return this->parameters; }


        /// Sets the parameters from another parameter list.
        void SetParameters(const ShaderParameterCache &other);

        /// Removes the parameters by name in the given parameters.
        void UnsetParameters(const ShaderParameterCache &parametersToUnset);

        /// Resets the default parameters, but leaves modified ones alone.
        void ResetDefaultParameters(const ShaderParameterCache &oldDefaultParameters);



        /// Determines whether or not the material is blended.
        bool IsBlended() const { return this->definition.IsBlended(); }

        /// Retrieves the blending equation to use with this material.
        BlendEquation GetBlendEquation() const { return this->definition.GetBlendEquation(); }

        /// Retrieves the source blending factor.
        BlendFunc GetBlendSourceFactor() const { return this->definition.GetBlendSourceFactor(); }

        /// Retrieves the destination blending factor.
        BlendFunc GetBlendDestinationFactor() const { return this->definition.GetBlendDestinationFactor(); }

        /// Retrieves the blend colour.
        const glm::vec4 & GetBlendColour() const { return this->blendColour; }


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
        void Serialize(Serializer &serializer) const;

        /// Deserializes the state of the material.
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        ///
        /// @remarks
        ///     This does NOT load an actual .material file. It just restores parameter states.
        void Deserialize(Deserializer &deserializer);



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
        

        /*
        // Unsets a parameter by it's name.
        void UnsetParameter(const char* name)
        {
            this->parameters.Unset(name);
        }
        */


    private:

        /// The material definition.
        const MaterialDefinition &definition;

        /// The parameters being used by the material.
        ShaderParameterCache parameters;

        /// The blend colour for the "constant colour" blending factors.
        glm::vec4 blendColour;



    private:    // No copying, for now.
        Material(const Material &);
        Material & operator=(const Material &);
    };
}

#endif