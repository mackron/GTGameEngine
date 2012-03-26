
#include <GTEngine/Material.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTCore/IO.hpp>
#include <GTCore/Parse.hpp>
#include <GTCore/Strings/Tokenizer.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <rapidxml/rapidxml.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

using namespace rapidxml;

namespace GTEngine
{
    static int AnonShaderIDCount = 0;

    /// Generates an ID for anonymous shaders.
    void GenerateAnonymousShaderID(GTCore::String &dest)
    {
        char buffer[64];
        GTCore::IO::snprintf(buffer, 64, "__AnonShaderID%d", AnonShaderIDCount);

        dest = buffer;
        
        ++AnonShaderIDCount;
    }


    MaterialDefinition::MaterialDefinition()
        : diffuseShaderID(), emissiveShaderID(), shininessShaderID(), defaultParams()
    {
    }

    MaterialDefinition::~MaterialDefinition()
    {
        // Default properties need to be deleted. Textures need to be unacquired.
        for (size_t i = 0; i < this->defaultParams.count; ++i)
        {
            auto iParam = this->defaultParams.buffer[i];
            assert(iParam        != nullptr);
            assert(iParam->value != nullptr);

            // If it's a texture property, it needs to be unacquired.
            if (iParam->value->type == ShaderParameterType_Texture2D)
            {
                Texture2DLibrary::Unacquire(ShaderParameter_Texture2D::Upcast(iParam->value)->value);
            }

            delete iParam->value;
        }

        this->defaultParams.Clear();
    }

    
    bool MaterialDefinition::LoadFromXML(char* xml)
    {
        xml_document<> document;

        try
        {
            document.parse<0>(xml);
        }
        catch (rapidxml::parse_error &e)
        {
            GTEngine::PostError("Material: %s", e.what());
            return false;
        }

        auto materialNode = document.first_node("material");
        if (materialNode != nullptr)
        {
            // <diffuse>
            auto diffuseNode = materialNode->first_node("diffuse");
            if (diffuseNode != nullptr)
            {
                auto idAttr = diffuseNode->first_attribute("id");
                if (idAttr != nullptr)
                {
                    this->diffuseShaderID = idAttr->value();
                }
                else
                {
                    GenerateAnonymousShaderID(this->diffuseShaderID);
                }

                if (diffuseNode->value_size() > 0)
                {
                    ShaderLibrary::AddShaderString(this->diffuseShaderID.c_str(), diffuseNode->value());
                }
            }
            else
            {
                this->diffuseShaderID = "Material_DefaultDiffuse";
            }

            // <emissive>
            auto emissiveNode = materialNode->first_node("emissive");
            if (emissiveNode != nullptr)
            {
                auto idAttr = emissiveNode->first_attribute("id");
                if (idAttr != nullptr)
                {
                    this->emissiveShaderID = idAttr->value();
                }
                else
                {
                    GenerateAnonymousShaderID(this->emissiveShaderID);
                }

                if (emissiveNode->value_size() > 0)
                {
                    ShaderLibrary::AddShaderString(this->emissiveShaderID.c_str(), emissiveNode->value());
                }
            }
            else
            {
                this->emissiveShaderID = "Material_DefaultEmissive";
            }

            // <shininess>
            auto shininessNode = materialNode->first_node("shininess");
            if (shininessNode != nullptr)
            {
                auto idAttr = shininessNode->first_attribute("id");
                if (idAttr != nullptr)
                {
                    this->shininessShaderID = idAttr->value();
                }
                else
                {
                    GenerateAnonymousShaderID(this->shininessShaderID);
                }

                if (shininessNode->value_size() > 0)
                {
                    ShaderLibrary::AddShaderString(this->shininessShaderID.c_str(), shininessNode->value());
                }
            }
            else
            {
                this->shininessShaderID = "Material_DefaultShininess";
            }


            // <defaultproperties>. Optional.
            auto defaultpropertiesNode = materialNode->first_node("defaultproperties");
            if (defaultpropertiesNode != nullptr)
            {
                auto child = defaultpropertiesNode->first_node();
                while (child != nullptr)
                {
                    if (GTCore::Strings::Equal("float", child->name()))
                    {
                        auto nameAttr = child->first_attribute("name");
                        if (nameAttr != nullptr)
                        {
                            auto valueStr = child->value();

                            float value[1];
                            MaterialDefinition::ParseFloatArray(valueStr, value, 1);

                            this->defaultParams.Add(nameAttr->value(), new ShaderParameter_Float(value[0]));
                        }
                    }
                    else if (GTCore::Strings::Equal("float2", child->name()))
                    {
                        auto nameAttr = child->first_attribute("name");
                        if (nameAttr != nullptr)
                        {
                            auto valueStr = child->value();

                            float value[2];
                            MaterialDefinition::ParseFloatArray(valueStr, value, 2);

                            this->defaultParams.Add(nameAttr->value(), new ShaderParameter_Float2(value[0], value[1]));
                        }
                    }
                    else if (GTCore::Strings::Equal("float3", child->name()))
                    {
                        auto nameAttr = child->first_attribute("name");
                        if (nameAttr != nullptr)
                        {
                            auto valueStr = child->value();

                            float value[3];
                            MaterialDefinition::ParseFloatArray(valueStr, value, 3);

                            this->defaultParams.Add(nameAttr->value(), new ShaderParameter_Float3(value[0], value[1], value[2]));
                        }
                    }
                    else if (GTCore::Strings::Equal("float4", child->name()))
                    {
                        auto nameAttr = child->first_attribute("name");
                        if (nameAttr != nullptr)
                        {
                            auto valueStr = child->value();

                            float value[4];
                            MaterialDefinition::ParseFloatArray(valueStr, value, 4);

                            this->defaultParams.Add(nameAttr->value(), new ShaderParameter_Float4(value[0], value[1], value[2], value[3]));
                        }
                    }
                    else if (GTCore::Strings::Equal("texture2D", child->name()))
                    {
                        auto nameAttr = child->first_attribute("name");
                        if (nameAttr != nullptr)
                        {
                            auto valueStr = child->value();
                            
                            this->defaultParams.Add(nameAttr->value(), new ShaderParameter_Texture2D(Texture2DLibrary::Acquire(valueStr)));
                        }
                    }

                    child = child->next_sibling();
                }
            }

            return true;
        }

        GTEngine::PostError("Material: Missing <material> node.");
        return false;
    }

    bool MaterialDefinition::LoadFromXML(const char* xml)
    {
        char* xmlCopy = GTCore::Strings::Create(xml);

        bool result = this->LoadFromXML(xmlCopy);

        GTCore::Strings::Delete(xmlCopy);

        return result;
    }

    bool MaterialDefinition::LoadFromFile(const char* fileName)
    {
        FILE* file = GTCore::IO::Open(fileName, GTCore::IO::OpenMode::Binary | GTCore::IO::OpenMode::Read);
        if (file != nullptr)
        {
            size_t fileSize = static_cast<size_t>(GTCore::IO::Size(file));

            auto data = static_cast<char*>(malloc(fileSize + 1));
            GTCore::IO::Read(file, data, fileSize);

            // String must be null-terminated.
            data[fileSize] = '\0';

            // Now we load the XML...
            bool result = this->LoadFromXML(data);

            // Finally, we clean up and return the result.
            free(data);
            GTCore::IO::Close(file);

            return result;
        }

        return false;
    }


    void MaterialDefinition::ParseFloatArray(const char* str, float* dest, size_t count)
    {
        assert(str != nullptr);
        assert(dest != nullptr);

        char tempStr[16];

        GTCore::Strings::Tokenizer tokens(str);
        while (tokens && count > 0)
        {
            GTCore::Strings::Copy(tempStr, tokens.start, tokens.GetSizeInTs());

            *dest++ = GTCore::Parse<float>(tempStr);

            ++tokens;
            --count;
        }
    }
}

namespace GTEngine
{
    Material::Material(const MaterialDefinition &definition)
        : definition(definition), parameters()
    {
        for (size_t i = 0; i < definition.defaultParams.count; ++i)
        {
            auto iParam = definition.defaultParams.buffer[i];
            assert(iParam        != nullptr);
            assert(iParam->value != nullptr);

            this->SetParameter(iParam->key, CopyShaderParameter(iParam->value));
        }
    }

    Material::~Material()
    {
        this->ClearPendingParameters();
    }


    void Material::SetParameter(const char* name, float value)
    {
        this->SetParameter<ShaderParameter_Float>(name, value);
    }

    void Material::SetParameter(const char* name, const glm::vec2 &value)
    {
        this->SetParameter<ShaderParameter_Float2>(name, value);
    }
    void Material::SetParameter(const char* name, const glm::vec3 &value)
    {
        this->SetParameter<ShaderParameter_Float3>(name, value);
    }
    void Material::SetParameter(const char* name, const glm::vec4 &value)
    {
        this->SetParameter<ShaderParameter_Float4>(name, value);
    }

    void Material::SetParameter(const char* name, const glm::mat2 &value)
    {
        this->SetParameter<ShaderParameter_Float2x2>(name, value);
    }
    void Material::SetParameter(const char* name, const glm::mat3 &value)
    {
        this->SetParameter<ShaderParameter_Float3x3>(name, value);
    }
    void Material::SetParameter(const char* name, const glm::mat4 &value)
    {
        this->SetParameter<ShaderParameter_Float4x4>(name, value);
    }

    void Material::SetParameter(const char* name, Texture2D* value)
    {
        this->SetParameter<ShaderParameter_Texture2D>(name, value);
    }


    void Material::ClearPendingParameters()
    {
        // Pending properties need to be cleared. Textures need to be unacquired.
        for (size_t i = 0; i < this->parameters.count; ++i)
        {
            auto iProp = this->parameters.buffer[i];
            assert(iProp != nullptr);
            assert(iProp->value != nullptr);

            // Now we can delete the parameter.
            delete iProp->value;
        }

        this->parameters.Clear();
    }

    
    // **** Private ****
    void Material::SetParameter(const char* name, ShaderParameter* prop)
    {
        // If a property of the same name already exists, it means it is being overwritten.
        auto iParam = this->parameters.Find(name);
        if (iParam != nullptr)
        {
            delete iParam->value;
        }
        else
        {
            this->parameters.Add(name, prop);
        }
    }
}
