// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Material.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/ShaderParameter.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Core/Parse.hpp>
#include <GTEngine/Core/Strings/Tokenizer.hpp>
#include <easy_path/easy_path.h>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <GTEngine/Core/rapidxml.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

using namespace rapidxml;

namespace GT
{
    static int AnonShaderIDCount = 0;

    /// Generates an ID for anonymous shaders.
    void GenerateAnonymousShaderID(String &dest)
    {
        char buffer[64];
        IO::snprintf(buffer, 64, "__AnonShaderID%d", AnonShaderIDCount);

        dest = buffer;

        ++AnonShaderIDCount;
    }


    MaterialDefinition::MaterialDefinition()
        : absolutePath(), relativePath(), xmlString(),
          //diffuseShaderID(), emissiveShaderID(), shininessShaderID(), normalShaderID(), refractionShaderID(), specularShaderID(),
          channelShaderIDs(),
          defaultParams(),
          hasNormalChannel(false), isRefractive(false), isBlended(false),
          blendEquation(BlendEquation_Add), blendSourceFactor(BlendFunc_One), blendDestinationFactor(BlendFunc_Zero), blendColour(0.0f, 0.0f, 0.0f, 0.0f)
    {
    }

    MaterialDefinition::~MaterialDefinition()
    {
        this->defaultParams.Clear();
    }


    bool MaterialDefinition::LoadFromXML(char* xml)
    {
        // We need to reset the definition.
        this->Reset();

        String diffuseShaderID    = "Material_DefaultDiffuse";
        String emissiveShaderID   = "Material_DefaultEmissive";
        String shininessShaderID  = "Material_DefaultShininess";
        String normalShaderID     = "";
        String refractionShaderID = "";
        String specularShaderID   = "Material_DefaultSpecular";
        this->hasNormalChannel   = false;
        this->isRefractive       = false;


        if (xml == nullptr || *xml == '\0')
        {
            // It's an empty file. This is valid.
            this->xmlString = "";
            return true;
        }

        String newXMLString(xml);


        xml_document<> document;

        try
        {
            document.parse<0>(xml);
        }
        catch (rapidxml::parse_error &e)
        {
            GT::PostError("Material: %s", e.what());
            return false;
        }

        auto materialNode = document.first_node("material");
        if (materialNode != nullptr)
        {
            // We need to keep track of the XML string.
            this->xmlString = newXMLString;


            // We need to iterate over each node.
            auto childNode = materialNode->first_node();
            while (childNode != nullptr)
            {
                // <channel>
                if (Strings::Equal(childNode->name(), "channel"))
                {
                    auto iNameAttribute = childNode->first_attribute("name");
                    if (iNameAttribute != nullptr)
                    {
                        // Retrieve the shader information.
                        String shaderID;

                        auto iShaderIDAttribute = childNode->first_attribute("shaderid");
                        if (iShaderIDAttribute != nullptr)
                        {
                            shaderID = iShaderIDAttribute->value();
                        }
                        else
                        {
                            GenerateAnonymousShaderID(shaderID);
                        }

                        if (childNode->value_size() > 0)
                        {
                            ShaderLibrary::AddShaderString(shaderID.c_str(), childNode->value());
                        }


                        // Add the channel.
                        this->channelShaderIDs.Add(iNameAttribute->value(), shaderID);
                    }
                    else
                    {
                        // It is an error for a <channel> tag to not have a name.
                        GT::PostError("Material: Warning: Missing 'name' attribute from a <channel> tag. Ignoring.");
                    }
                }



                // <blending>. Optional. If ommitted, blending is disabled (it's an opaque material).
                if (Strings::Equal(childNode->name(), "blending"))
                {
                    this->isBlended = true;

                    // <equation>
                    auto equationNode = childNode->first_node("equation");
                    if (equationNode != nullptr)
                    {
                        this->blendEquation = ToBlendEquation(equationNode->value());
                    }

                    // <sourcefactor>
                    auto sourcefactorNode = childNode->first_node("sourcefactor");
                    if (sourcefactorNode != nullptr)
                    {
                        this->blendSourceFactor = ToBlendFunction(sourcefactorNode->value());
                    }

                    // <destinationfactor>
                    auto destinationfactorNode = childNode->first_node("destinationfactor");
                    if (destinationfactorNode != nullptr)
                    {
                        this->blendDestinationFactor = ToBlendFunction(destinationfactorNode->value());
                    }

                    // <colour>
                    auto colourNode = childNode->first_node("colour");
                    if (colourNode != nullptr)
                    {
                        float colour[4];
                        MaterialDefinition::ParseFloatArray(colourNode->value(), colour, 4);

                        this->blendColour = glm::vec4(colour[0], colour[1], colour[2], colour[3]);
                    }
                }



                // <defaultproperties>. Optional.
                if (Strings::Equal(childNode->name(), "defaultproperties"))
                {
                    auto propertyNode = childNode->first_node();
                    while (propertyNode != nullptr)
                    {
                        if (Strings::Equal("float", propertyNode->name()))
                        {
                            auto nameAttr = propertyNode->first_attribute("name");
                            if (nameAttr != nullptr)
                            {
                                auto valueStr = propertyNode->value();

                                float value[1];
                                MaterialDefinition::ParseFloatArray(valueStr, value, 1);

                                this->defaultParams.Set(nameAttr->value(), value[0]);
                            }
                        }
                        else if (Strings::Equal("float2", propertyNode->name()))
                        {
                            auto nameAttr = propertyNode->first_attribute("name");
                            if (nameAttr != nullptr)
                            {
                                auto valueStr = propertyNode->value();

                                float value[2];
                                MaterialDefinition::ParseFloatArray(valueStr, value, 2);

                                this->defaultParams.Set(nameAttr->value(), value[0], value[1]);
                            }
                        }
                        else if (Strings::Equal("float3", propertyNode->name()))
                        {
                            auto nameAttr = propertyNode->first_attribute("name");
                            if (nameAttr != nullptr)
                            {
                                auto valueStr = propertyNode->value();

                                float value[3];
                                MaterialDefinition::ParseFloatArray(valueStr, value, 3);

                                this->defaultParams.Set(nameAttr->value(), value[0], value[1], value[2]);
                            }
                        }
                        else if (Strings::Equal("float4", propertyNode->name()))
                        {
                            auto nameAttr = propertyNode->first_attribute("name");
                            if (nameAttr != nullptr)
                            {
                                auto valueStr = propertyNode->value();

                                float value[4];
                                MaterialDefinition::ParseFloatArray(valueStr, value, 4);

                                this->defaultParams.Set(nameAttr->value(), value[0], value[1], value[2], value[3]);
                            }
                        }
                        else if (Strings::Equal("texture2D", propertyNode->name()))
                        {
                            auto nameAttr = propertyNode->first_attribute("name");
                            if (nameAttr != nullptr)
                            {
                                auto valueStr = propertyNode->value();

                                auto texture = Texture2DLibrary::Acquire(valueStr);
                                {
                                    this->defaultParams.Set(nameAttr->value(), texture);
                                }
                                Texture2DLibrary::Unacquire(texture);
                            }
                        }

                        propertyNode = propertyNode->next_sibling();
                    }
                }



                childNode = childNode->next_sibling();
            }

            return true;
        }

        GT::PostError("Material: Missing <material> node.");
        return false;
    }

    bool MaterialDefinition::LoadFromXML(const char* xml)
    {
        char* xmlCopy = Strings::Create(xml);

        bool result = this->LoadFromXML(xmlCopy);

        Strings::Delete(xmlCopy);

        return result;
    }

    bool MaterialDefinition::LoadFromFile(const char* fileNameIn, const char* relativePathIn)
    {
        char newAbsolutePath[EASYVFS_MAX_PATH];
        char newRelativePath[EASYVFS_MAX_PATH];


        if (easypath_is_absolute(fileNameIn))
        {
            strcpy_s(newAbsolutePath, sizeof(newAbsolutePath), fileNameIn);

            if (relativePathIn != nullptr)
            {
                strcpy_s(newRelativePath, sizeof(newRelativePath), relativePathIn);
            }
            else
            {
                GT::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileNameIn);
                return false;
            }
        }
        else
        {
            strcpy_s(newRelativePath, sizeof(newRelativePath), fileNameIn);

            if (!easyvfs_find_absolute_path(g_EngineContext->GetVFS(), fileNameIn, newAbsolutePath, sizeof(newAbsolutePath)))
            {
                return false;
            }
        }


        char* pFileData = easyvfs_open_and_read_text_file(g_EngineContext->GetVFS(), newAbsolutePath, NULL);
        if (pFileData != nullptr)
        {
            bool result = this->LoadFromXML(pFileData);


            // LoadFromXML() will have reset the paths to "" so we need to make sure they are set again.
            this->absolutePath = newAbsolutePath;
            this->relativePath = newRelativePath;

            easyvfs_free(pFileData);
            return result;
        }

        return false;
    }


    const char* MaterialDefinition::GetXMLString() const
    {
        return this->xmlString.c_str();
    }



    String MaterialDefinition::GetChannelShaderID(const char* channelName) const
    {
        auto iChannel = this->channelShaderIDs.Find(channelName);
        if (iChannel != nullptr)
        {
            return iChannel->value;
        }

        return "";
    }

    bool MaterialDefinition::IsChannelDefined(const char* channelName) const
    {
        return this->channelShaderIDs.Exists(channelName);
    }


    //////////////////////////////////
    // Private

    void MaterialDefinition::Reset()
    {
        this->absolutePath       = "";
        this->relativePath       = "";
        this->xmlString          = "";
        this->isBlended          = false;
        this->channelShaderIDs.Clear();
        this->defaultParams.Clear();

        

#if 0
        this->diffuseShaderID    = "";
        this->emissiveShaderID   = "";
        this->shininessShaderID  = "";
        this->normalShaderID     = "";
        this->refractionShaderID = "";
        this->specularShaderID   = "";
#endif

        this->hasNormalChannel   = false;
        this->isRefractive       = false;
    }

    void MaterialDefinition::ParseFloatArray(const char* str, float* dest, size_t count)
    {
        assert(str != nullptr);
        assert(dest != nullptr);

        char tempStr[16];

        Strings::Tokenizer tokens(str);
        while (tokens && count > 0)
        {
            Strings::Copy(tempStr, tokens.start, tokens.GetSizeInTs());

            *dest++ = Parse<float>(tempStr);

            ++tokens;
            --count;
        }
    }
}

namespace GT
{
    Material::Material(const MaterialDefinition &definition)
        : definition(definition), parameters(), blendColour(definition.GetBlendColour())
    {
        this->parameters = definition.defaultParams;
    }

    Material::~Material()
    {
        this->parameters.Clear();
    }


    void Material::SetParameters(const ShaderParameterCache &other)
    {
        this->parameters.SetParameters(other);
    }

    void Material::UnsetParameters(const ShaderParameterCache &parametersToUnset)
    {
        this->parameters.UnsetParameters(parametersToUnset);
    }

    void Material::ResetDefaultParameters(const ShaderParameterCache &oldDefaultParameters)
    {
        auto &oldFloatParameters       = oldDefaultParameters.GetFloatParameters();
        auto &oldFloat2Parameters      = oldDefaultParameters.GetFloat2Parameters();
        auto &oldFloat3Parameters      = oldDefaultParameters.GetFloat3Parameters();
        auto &oldFloat4Parameters      = oldDefaultParameters.GetFloat4Parameters();
        auto &oldFloat2x2Parameters    = oldDefaultParameters.GetFloat2x2Parameters();
        auto &oldFloat3x3Parameters    = oldDefaultParameters.GetFloat3x3Parameters();
        auto &oldFloat4x4Parameters    = oldDefaultParameters.GetFloat4x4Parameters();
        auto &oldTexture2DParameters   = oldDefaultParameters.GetTexture2DParameters();
        auto &oldTextureCubeParameters = oldDefaultParameters.GetTextureCubeParameters();

        auto &currentFloatParameters       = this->parameters.GetFloatParameters();
        auto &currentFloat2Parameters      = this->parameters.GetFloat2Parameters();
        auto &currentFloat3Parameters      = this->parameters.GetFloat3Parameters();
        auto &currentFloat4Parameters      = this->parameters.GetFloat4Parameters();
        auto &currentFloat2x2Parameters    = this->parameters.GetFloat2x2Parameters();
        auto &currentFloat3x3Parameters    = this->parameters.GetFloat3x3Parameters();
        auto &currentFloat4x4Parameters    = this->parameters.GetFloat4x4Parameters();
        auto &currentTexture2DParameters   = this->parameters.GetTexture2DParameters();
        auto &currentTextureCubeParameters = this->parameters.GetTextureCubeParameters();


        // Float
        for (size_t i = 0; i < oldFloatParameters.count; ++i)
        {
            auto name     = oldFloatParameters.buffer[i]->key;
            auto oldValue = oldFloatParameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetFloatParameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentFloatParameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetFloat(name);
            }
        }

        // Float2
        for (size_t i = 0; i < oldFloat2Parameters.count; ++i)
        {
            auto name     = oldFloat2Parameters.buffer[i]->key;
            auto oldValue = oldFloat2Parameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetFloat2Parameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentFloat2Parameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetFloat2(name);
            }
        }

        // Float3
        for (size_t i = 0; i < oldFloat3Parameters.count; ++i)
        {
            auto name     = oldFloat3Parameters.buffer[i]->key;
            auto oldValue = oldFloat3Parameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetFloat3Parameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentFloat3Parameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetFloat3(name);
            }
        }

        // Float4
        for (size_t i = 0; i < oldFloat4Parameters.count; ++i)
        {
            auto name     = oldFloat4Parameters.buffer[i]->key;
            auto oldValue = oldFloat4Parameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetFloat4Parameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentFloat4Parameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetFloat4(name);
            }
        }

        // Float2x2
        for (size_t i = 0; i < oldFloat2x2Parameters.count; ++i)
        {
            auto name     = oldFloat2x2Parameters.buffer[i]->key;
            auto oldValue = oldFloat2x2Parameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetFloat2x2Parameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentFloat2x2Parameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetFloat2x2(name);
            }
        }

        // Float3x3
        for (size_t i = 0; i < oldFloat3x3Parameters.count; ++i)
        {
            auto name     = oldFloat3x3Parameters.buffer[i]->key;
            auto oldValue = oldFloat3x3Parameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetFloat3x3Parameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentFloat3x3Parameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetFloat3x3(name);
            }
        }

        // Float4x4
        for (size_t i = 0; i < oldFloat4x4Parameters.count; ++i)
        {
            auto name     = oldFloat4x4Parameters.buffer[i]->key;
            auto oldValue = oldFloat4x4Parameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetFloat4x4Parameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentFloat4x4Parameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetFloat4x4(name);
            }
        }


        // Texture2D
        for (size_t i = 0; i < oldTexture2DParameters.count; ++i)
        {
            auto name     = oldTexture2DParameters.buffer[i]->key;
            auto oldValue = oldTexture2DParameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetTexture2DParameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentTexture2DParameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetTexture2D(name);
            }
        }

        // TextureCube
        for (size_t i = 0; i < oldTextureCubeParameters.count; ++i)
        {
            auto name     = oldTextureCubeParameters.buffer[i]->key;
            auto oldValue = oldTextureCubeParameters.buffer[i]->value.value;

            auto newDefaultParameter = this->definition.defaultParams.GetTextureCubeParameter(name);
            if (newDefaultParameter != nullptr)
            {
                auto iCurrentParameter = currentTextureCubeParameters.Find(name);
                if (iCurrentParameter != nullptr)
                {
                    if (iCurrentParameter->value.value == oldValue)
                    {
                        this->parameters.Set(name, newDefaultParameter->value);
                    }
                }
                else
                {
                    this->parameters.Set(name, newDefaultParameter->value);
                }
            }
            else
            {
                this->parameters.UnsetTextureCube(name);
            }
        }
    }


#if 0
    void Material::ResetDefaultParameters(const ShaderParameterCache &oldDefaultParameters)
    {
        this->parameters.UnsetUnusedParameters(oldDefaultParameters);


        for (size_t iParameter = 0; iParameter < oldDefaultParameters.GetCount(); ++iParameter)
        {
            auto parameterName = oldDefaultParameters.GetNameByIndex(iParameter);
            assert(parameterName != nullptr);
            {
                auto oldParameter = oldDefaultParameters.GetByIndex(iParameter);
                auto newParameter = definition->defaultParams.Get(parameterName);

                if (newParameter == nullptr)
                {
                    // The parameter doesn't exist anymore. It needs to be removed from the material.
                    material->UnsetParameter(parameterName);
                }
                else
                {
                    // The parameter still exists, but we need to check if it needs replacing.
                    auto iMaterialParameter = material->GetParameters().Find(parameterName);
                    if (iMaterialParameter != nullptr)
                    {
                        if (CompareShaderParameters(oldParameter, iMaterialParameter->value))
                        {
                            // We'll replace the parameter.
                            material->SetParameter(parameterName, newParameter);
                        }
                    }
                }
            }
        }
    }
#endif

    void Material::Serialize(Serializer &serializer) const
    {
        // Our one and only chunk for now is the parameters chunk.
        BasicSerializer parametersSerializer;
        this->parameters.Serialize(parametersSerializer);


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_Material_Parameters;
        header.version     = 1;
        header.sizeInBytes = parametersSerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(parametersSerializer.GetBuffer(), header.sizeInBytes);
    }

    void Material::Deserialize(Deserializer &deserializer)
    {
        // The only chunk for now is the parameters chunk.
        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_Material_Parameters);
        {
            this->parameters.Deserialize(deserializer);
        }
    }
}
