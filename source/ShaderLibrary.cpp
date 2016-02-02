// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/ShaderLibrary.hpp>
#include <GTGE/Rendering/Renderer.hpp>
#include <GTGE/GTEngine.hpp>           // For g_Context. Remove this when the global context is removed.
#include <GTGE/Core/Dictionary.hpp>
#include <GTGE/Core/Vector.hpp>
#include <GTGE/Core/String.hpp>
#include <dr_libs/dr_path.h>

using namespace rapidxml;

/*
The shader library.

Shaders are build from XML. Shaders are defined in <shader> tags and are represented by the ShaderLibraryShader class.

All shaders have ID's associated with them. This serves two purposes. The first is to support including via XML. The
other is to allow the shader string to be retrieved from the ShaderLibrary class.

When a shader uses an include, it uses the 'url' argument to link it to a shader. If the shader hasn't yet been
loaded, it will create an empty shader whose ID will be the URL.
*/

namespace GT
{
    ShaderLibrary::ShaderLibrary(Context &context)
        : m_context(context),
          m_GUIShader(nullptr), m_GUIShaderA8(nullptr), m_Textured2DQuadShader(nullptr),
          m_shaders()
    {
    }

    ShaderLibrary::~ShaderLibrary()
    {
    }


    void ShaderLibrary::Shutdown()
    {
        Renderer::DeleteShader(m_GUIShader);
        Renderer::DeleteShader(m_GUIShaderA8);
        Renderer::DeleteShader(m_Textured2DQuadShader);

        m_GUIShader            = nullptr;
        m_GUIShaderA8          = nullptr;
        m_Textured2DQuadShader = nullptr;

        m_shaders.Clear();

        // All shader objects need to be deleted.
        for (size_t i = 0; i < AcquiredShaders.count; ++i)
        {
            Renderer::DeleteShader(AcquiredShaders.buffer[i]->shader);
            delete AcquiredShaders.buffer[i];
        }
        AcquiredShaders.Clear();
    }


    bool ShaderLibrary::LoadFromDirectory(const char* directory, bool recursive)
    {
        drvfs_context* pVFS = g_Context->GetVFS();
        assert(pVFS != nullptr);

        if (drpath_is_absolute(directory))
        {
            // Absolute.

            drvfs_iterator iFile;
            if (drvfs_begin(pVFS, directory, &iFile))
            {
                do
                {
                    if ((iFile.info.attributes & DRVFS_FILE_ATTRIBUTE_DIRECTORY) != 0)
                    {
                        if (recursive)
                        {
                            ShaderLibrary::LoadFromDirectory(iFile.info.absolutePath);
                        }
                    }
                    else
                    {
                        ShaderLibrary::LoadFromFile(iFile.info.absolutePath);
                    }

                } while (drvfs_next(pVFS, &iFile));
            }
        }
        else
        {
            // Relative. We just call this recursively with the absolute path.

            unsigned int baseDirectoryCount = drvfs_get_base_directory_count(pVFS);
            for (unsigned int iBaseDirectory = 0; iBaseDirectory < baseDirectoryCount; ++iBaseDirectory)
            {
                const char* baseDirectory = drvfs_get_base_directory_by_index(pVFS, iBaseDirectory);
                if (baseDirectory != nullptr)
                {
                    char searchDir[DRVFS_MAX_PATH];
                    drpath_copy_and_append(searchDir, sizeof(searchDir), baseDirectory, directory);

                    ShaderLibrary::LoadFromDirectory(searchDir, recursive);
                }
            }
        }

        return true;
    }

    bool ShaderLibrary::LoadFromFile(const char* fileName)
    {
        drvfs_file* pFile = drvfs_open(g_Context->GetVFS(), fileName, DRVFS_READ, 0);
        if (pFile != nullptr)
        {
            // We need to read the content of the file and then load it as XML. We cast the size to a size_t to
            // play nicely with 32-bit compilations. We can pretty safely assume the XML file will not exceed that.
            size_t fileSize = static_cast<size_t>(drvfs_size(pFile));
            
            auto fileData = static_cast<char*>(malloc(fileSize + 1));
            drvfs_read(pFile, fileData, fileSize, nullptr);
            fileData[fileSize] = '\0';
            
            drvfs_close(pFile);
            
            bool result = ShaderLibrary::LoadFromXML(fileData);
            
            free(fileData);
            return result;
        }

        return false;
    }

    bool ShaderLibrary::LoadFromXML(const char *xml)
    {
        // We need to create a non-const copy because LoadFromXML() is destructive.
        auto input = Strings::Create(xml);
        
        bool result = ShaderLibrary::LoadFromXML(input);
        
        Strings::Delete(input);
        return result;
    }

    bool ShaderLibrary::LoadShaderFromXML(xml_node<> *node)
    {
        if (node != nullptr)
        {
            // We first need to load the shader into the shader list. All <shader> tags must have an 'id' attribute.
            xml_attribute<> *attr;
            if ((attr = node->first_attribute("id")) != nullptr)
            {
                // We have an ID, so we can load the shader.
                ShaderLibraryShader *shader = m_shaders.LoadShader(attr->value());  // attr->value() will be the shader's ID.
                if (shader != nullptr)
                {
                    // Now that the shader is in the library, it needs to be filled with content. To do this we simply
                    // iterate over the children. If it has no children, it means the shader is made up of only a single
                    // anonymous include.
                    xml_node<> *childNode = node->first_node();
                    if (childNode != nullptr)
                    {
                        // If the child is cdata, we just load directly like normal. Otherwise it may be an <include>.
                        if (childNode->type() != node_cdata)
                        {
                            if (strcmp(childNode->name(), "") != 0)
                            {
                                // We have children.
                                while (childNode != nullptr)
                                {
                                    if (Strings::Equal(childNode->name(), "include"))
                                    {
                                        // If the include node has a 'url' attribute, it means it's using an external, named include. Otherwise, it's
                                        // assumed to be an anonymous include.
                                        xml_attribute<> *includeAttr = childNode->first_attribute("url");
                                        if (includeAttr != nullptr)
                                        {
                                            // The URL will begin with a '#'. We need to move to the character after that before loading the include.
                                            const char *url = includeAttr->value() + 1;     // <-- The '+ 1' will move past the '#' character.
                                    
                                            ShaderLibraryShader *include = m_shaders.FindByID(url);
                                            if (include == nullptr)
                                            {
                                                include = m_shaders.LoadShader(url);
                                            }

                                            shader->AppendInclude(include);
                                        }
                                        else
                                        {
                                            shader->AppendAnonymousInclude(childNode->value());
                                        }
                                    }
                    
                                    childNode = childNode->next_sibling();
                                }
                            }
                            else
                            {
                                shader->SetContent(node->value());
                            }
                        }
                        else
                        {
                            // We'll get here if the child was cdata.
                            shader->SetContent(childNode->value());
                        }
                    }
                    else
                    {
                        // We don't have children so we just set the content directly.
                        shader->SetContent(node->value());
                    }
                }
                else
                {
                    g_Context->LogError("ShaderLibrary - There was an unknown error loading a <shader> tag. This shader will be ignored.");
                }
            }
            else
            {
                g_Context->LogError("ShaderLibrary - <shader> tags must have an 'id' attribute.");
            }
        }

        return false;
    }

    bool ShaderLibrary_LoadSectionFromXML(xml_node<>* node)
    {
        if (node != nullptr)
        {
            return true;
        }

        return false;
    }

    bool ShaderLibrary::LoadFromXML(char* xml)
    {
        if (xml)
        {
            xml_document<> document;

            try
            {
                document.parse<0>(xml);
            }
            catch (rapidxml::parse_error &e)
            {
                g_Context->LogErrorf("ShaderLibrary: %s", e.what());
                return false;
            }
            
            // We just iterate over every node. All we care about is <shader> nodes.
            auto shaderNode = document.first_node();
            while (shaderNode != nullptr)
            {
                if (strcmp(shaderNode->name(), "shader") == 0)
                {
                    ShaderLibrary::LoadShaderFromXML(shaderNode);
                }

                shaderNode = shaderNode->next_sibling();
            }

            return true;
        }
        
        return false;
    }

    void ShaderLibrary::AddShaderString(const char* id, const char* shaderStr)
    {
        m_shaders.Add(id, shaderStr);
    }

    const char* ShaderLibrary::GetShaderString(const char* id)
    {
        // First we need to try and find the shader. If we can't find it, we return null.
        ShaderLibraryShader *shader = m_shaders.FindByID(id);
        if (shader)
        {
            // All we need to do is resolve the shader and then return it's content.
            return shader->GetContent();
        }
        
        return nullptr;
    }


    // Creation/Deletion.

    Shader* ShaderLibrary::Acquire(const char* vertexShaderID, const char* fragmentShaderID)
    {
        auto shaderInfo = this->FindShaderInfo(vertexShaderID, fragmentShaderID);
        if (shaderInfo == nullptr)
        {
            // We'll get here if the shader has not already been created.
            shaderInfo = new ShaderInfo;
            shaderInfo->shader           = Renderer::CreateShader(ShaderLibrary::GetShaderString(vertexShaderID), ShaderLibrary::GetShaderString(fragmentShaderID));
            shaderInfo->vertexShaderID   = vertexShaderID;
            shaderInfo->fragmentShaderID = fragmentShaderID;
            shaderInfo->refCount         = 1;

            AcquiredShaders.PushBack(shaderInfo);
        }
        else
        {
            ++shaderInfo->refCount;
        }

        return shaderInfo->shader;
    }

    Shader* ShaderLibrary::Acquire(Shader* shader)
    {
        auto shaderInfo = this->FindShaderInfo(shader);
        if (shaderInfo != nullptr)
        {
            ++shaderInfo->refCount;
        }

        return shader;
    }

    void ShaderLibrary::Unacquire(Shader* shader)
    {
        auto shaderInfo = this->FindShaderInfo(shader);
        if (shaderInfo != nullptr)
        {
            // Here we do a simple check to see if we should delete the objects, or just decrement the reference count.
            if (shaderInfo->refCount <= 1)
            {
                AcquiredShaders.RemoveFirstOccuranceOf(shaderInfo);

                Renderer::DeleteShader(shaderInfo->shader);
                delete shaderInfo;
            }
            else
            {
                --shaderInfo->refCount;
            }
        }
    }


    Shader* ShaderLibrary::FindShader(const char* vertexShaderID, const char* fragmentShaderID)
    {
        auto shaderInfo = this->FindShaderInfo(vertexShaderID, fragmentShaderID);
        if (shaderInfo != nullptr)
        {
            return shaderInfo->shader;
        }

        return nullptr;
    }



    // Shader object retrieval.

    Shader* ShaderLibrary::GetGUIShader()
    {
        if (m_GUIShader == nullptr)
        {
            m_GUIShader = Renderer::CreateShader(ShaderLibrary::GetShaderString("Engine_GUI_VS"), ShaderLibrary::GetShaderString("Engine_GUI_FS"), nullptr);
        }

        return m_GUIShader;
    }
    
    Shader* ShaderLibrary::GetGUIShaderA8()
    {
        if (m_GUIShaderA8 == nullptr)
        {
            m_GUIShaderA8 = Renderer::CreateShader(ShaderLibrary::GetShaderString("Engine_GUI_VS"), ShaderLibrary::GetShaderString("Engine_GUI_FS_A8"), nullptr);
        }

        return m_GUIShaderA8;
    }


    Shader* ShaderLibrary::GetTextured2DQuadShader()
    {
        if (m_Textured2DQuadShader == nullptr)
        {
            m_Textured2DQuadShader = Renderer::CreateShader(ShaderLibrary::GetShaderString("Engine_Textured2DQuad_VS"), ShaderLibrary::GetShaderString("Engine_Textured2DQuad_FS"));
        }

        return m_Textured2DQuadShader;
    }





    ///////////////////////////////////////
    // Private

    ShaderLibrary::ShaderInfo* ShaderLibrary::FindShaderInfo(const char* vertexShaderID, const char* fragmentShaderID)
    {
        for (size_t i = 0; i < AcquiredShaders.count; ++i)
        {
            if (AcquiredShaders[i]->vertexShaderID   == vertexShaderID &&
                AcquiredShaders[i]->fragmentShaderID == fragmentShaderID)
            {
                return AcquiredShaders[i];
            }
        }

        return nullptr;
    }

    ShaderLibrary::ShaderInfo* ShaderLibrary::FindShaderInfo(Shader* shader)
    {
        for (size_t i = 0; i < AcquiredShaders.count; ++i)
        {
            if (AcquiredShaders[i]->shader == shader)
            {
                return AcquiredShaders[i];
            }
        }

        return nullptr;
    }
}
