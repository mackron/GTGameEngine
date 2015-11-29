// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/ApplicationConfig.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/GTEngine.hpp>           // For g_EngineContext. Remove this when the global context is removed.
#include <GTLib/Dictionary.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/String.hpp>
#include <easy_path/easy_path.h>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <GTLib/rapidxml.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

using namespace rapidxml;

/*
The shader library.

Shaders are build from XML. Shaders are defined in <shader> tags and are represented by the ShaderLibraryShader class.

All shaders have ID's associated with them. This serves two purposes. The first is to support including via XML. The
other is to allow the shader string to be retrieved from the ShaderLibrary class.

When a shader uses an include, it uses the 'url' argument to link it to a shader. If the shader hasn't yet been
loaded, it will create an empty shader whose ID will be the URL.
*/

namespace GTEngine
{
    /**
    *   \brief  Class representing a shader in the shader library.
    */
    class ShaderLibraryShader
    {
    public:

        /// Constructor.
        ShaderLibraryShader(const char* id)
            : id(id), includes(), anonIncludes(), content(nullptr)
        {
        }

        /// Destructor.
        ~ShaderLibraryShader()
        {
            this->DeleteAnonymousIncludes();
        }

        /**
        *   \brief  Retrieves the ID of the shader.
        */
        const char* GetID() const { return this->id.c_str(); }

        /**
        *   \brief  Retrieves the content of the shader.
        *
        *   \remarks
        *       The first time this is called, the content will be constructed from it's includes. If any include has not yet
        *       had it's content set, an empty string will be returned.
        */
        const char* GetContent()
        {
            if (this->content == "")
            {
                this->ConstructContent();
            }

            return this->content.c_str();
        }

        /**
        *   \brief  Sets the content fo the shader directly.
        */
        void SetContent(const char* contentIn)
        {
            this->content = contentIn;
        }


        /**
        *   \brief  Appends a named include to the shader.
        */
        void AppendInclude(ShaderLibraryShader *include)
        {
            this->includes.Append(include);
        }

        /**
        *   \brief  Appends an anonymous include to the shader.
        */
        void AppendAnonymousInclude(const char *contentIn)
        {
            auto newInclude = new ShaderLibraryShader(nullptr);
            newInclude->SetContent(contentIn);

            this->includes.Append(newInclude);
            this->anonIncludes.Append(newInclude);
        }

        /**
        *   \brief  Resets the shader, leaving it as an empty shell. Does not reset it's ID.
        */
        void Reset()
        {
            this->includes.Clear();
            this->DeleteAnonymousIncludes();

            this->content = "";
        }


    private:

        /**
        *   \brief  Constructs the content of the shader based on it's includes.
        *
        *   \remarks
        *       If any include has not had it's content loaded, the content string is left unmodified.
        */
        void ConstructContent()
        {
            // We will use a string list for constructing the content string.
            GTLib::Strings::List<char> contentList;
            for (auto i = this->includes.root; i != nullptr; i = i->next)
            {
                const char *iContent = i->value->GetContent();
                if (iContent != nullptr)
                {
                    contentList.Append(iContent);
                }
                else
                {
                    // Early termination if one of our includes haven't yet been loaded.
                    return;
                }
            }

            // If we've made it here it means the shader content was built successfully and we can set it.
            this->content = contentList.c_str();
        }

        /**
        *   \brief  Helper function for deleting the anonymous includes.
        */
        void DeleteAnonymousIncludes()
        {
            for (auto i = this->anonIncludes.root; i != nullptr; i = i->next)
            {
                delete i->value;
            }
            this->anonIncludes.Clear();
        }


    private:

        /// The ID of the shader. All shaders need an ID for loading/retrieval purposes.
        GTLib::String id;

        /// The list of includes making up this shader. This includes both named and anonymous includes.
        GTLib::List<ShaderLibraryShader*> includes;

        /// The list containing pointers to the anonymous includes. These includes are tied to this shader, and
        /// only this shader.
        GTLib::List<ShaderLibraryShader*> anonIncludes;

        /// The content of the shader. This will start out as empty, but will be given content when the
        /// shader content is constructed with ConstructContent(), or SetContent() is called.
        GTLib::String content;


    private:    // No copying.
        ShaderLibraryShader(const ShaderLibraryShader &);
        ShaderLibraryShader & operator=(const ShaderLibraryShader &);
    };
    
    /**
    *   \brief  Static class for managing the shaders that have been loaded into the library.
    */
    class ShaderLibraryShaders
    {
    public:

        /**
        *   \brief  Loads an returns a shader.
        *
        *   \remarks
        *       If a shader with the same ID already exists, that existing shader is reset before returning. Use FindByID() to retrieve
        *       a pointer to an existing shader.
        */
        static ShaderLibraryShader* LoadShader(const char *id)
        {
            ShaderLibraryShader* shader = nullptr;

            auto item = Shaders.Find(id);
            if (item == nullptr)
            {
                shader = new ShaderLibraryShader(id);
                Shaders.Add(shader->GetID(), shader);           // <-- DON'T use 'id' in place of 'shader->GetID()'.
            }
            else
            {
                shader = item->value;
                shader->Reset();
            }

            return shader;
        }

        /**
        *   \brief  Finds a shader by it's ID.
        */
        static ShaderLibraryShader* FindByID(const char *id)
        {
            auto item = Shaders.Find(id);
            if (item != nullptr)
            {
                return item->value;
            }

            return nullptr;
        }


        /**
        *   \brief               Adds a shader.
        *   \param  id      [in] The ID of the new shader.
        *   \param  content [in] The content of the shader.
        */
        static ShaderLibraryShader* Add(const char* id, const char* content)
        {
            auto newShader = LoadShader(id);
            newShader->SetContent(content);

            return newShader;
        }


        /**
        *   \brief  Clears the list of shaders.
        */
        static void Clear()
        {
            for (size_t i = 0; i < Shaders.count; ++i)
            {
                delete Shaders.buffer[i]->value;
            }

            Shaders.Clear();
        }


    private:

        /// The map containing pointers to each of the loaded shaders, indexed by their ID.
        static GTLib::Dictionary<ShaderLibraryShader*> Shaders;
    };
    GTLib::Dictionary<ShaderLibraryShader*> ShaderLibraryShaders::Shaders;
}

namespace GTEngine
{
    bool ShaderLibrary::LoadFromDirectory(const char* directory, bool recursive)
    {
        easyvfs_context* pVFS = g_EngineContext->GetVFS();
        assert(pVFS != nullptr);

        if (easypath_isabsolute(directory))
        {
            // Absolute.

            easyvfs_iterator iFile;
            if (easyvfs_begin_iteration(pVFS, directory, &iFile))
            {
                easyvfs_file_info fi;
                while (easyvfs_next_iteration(pVFS, &iFile, &fi))
                {
                    if ((fi.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) != 0)
                    {
                        if (recursive)
                        {
                            ShaderLibrary::LoadFromDirectory(fi.absolutePath);
                        }
                    }
                    else
                    {
                        ShaderLibrary::LoadFromFile(fi.absolutePath);
                    }
                }
            }
        }
        else
        {
            // Relative. We just call this recursively with the absolute path.

            unsigned int baseDirectoryCount = easyvfs_get_base_directory_count(pVFS);
            for (unsigned int iBaseDirectory = 0; iBaseDirectory < baseDirectoryCount; ++iBaseDirectory)
            {
                const char* baseDirectory = easyvfs_get_base_directory_by_index(pVFS, iBaseDirectory);
                if (baseDirectory != nullptr)
                {
                    char searchDir[EASYVFS_MAX_PATH];
                    easypath_copyandappend(searchDir, sizeof(searchDir), baseDirectory, directory);

                    ShaderLibrary::LoadFromDirectory(searchDir, recursive);
                }
            }
        }


#if 0
        // We need to do this for every data directory, including the current directory.
        GTLib::Vector<GTLib::String> baseDirectories;
        baseDirectories.PushBack(GTLib::IO::GetCurrentDirectory());

        if (!GTLib::Path::IsAbsolute(directory))
        {
            g_EngineContext->GetApplicationConfig().GetDataDirectories(baseDirectories);
        }


        for (size_t iBaseDirectory = 0; iBaseDirectory < baseDirectories.count; ++iBaseDirectory)
        {
            // Here we save and then set the current directory.
            GTLib::IO::PushCurrentDirectory();
            GTLib::IO::SetCurrentDirectory(baseDirectories[iBaseDirectory].c_str());

            // First we need the files in the directory.
            GTLib::Path searchQuery(directory);
            searchQuery.Append(".*");                   // <-- This means to iterate over every file in the directory.

            GTLib::IO::FileIterator i(searchQuery.c_str());
            while (i)
            {
                if (i.isDirectory)
                {
                    if (recursive)
                    {
                        ShaderLibrary::LoadFromDirectory(i.name, true);
                    }
                }
                else
                {
                    ShaderLibrary::LoadFromFile(i.name);
                }

                ++i;
            }

            // Can't forget to restore the previous directory.
            GTLib::IO::PopCurrentDirectory();
        }
#endif
        
        return true;
    }

    bool ShaderLibrary::LoadFromFile(const char* fileName)
    {
#if 0
        auto file = GTLib::IO::Open(fileName, GTLib::IO::OpenMode::Read);
        if (file)
        {
            // We need to read the content of the file and then load it as XML. We cast the size to a size_t to
            // play nicely with 32-bit compilations. We can pretty safely assume the XML file will not exceed that.
            size_t fileSize = static_cast<size_t>(GTLib::IO::Size(file));
            
            auto fileData = static_cast<char*>(malloc(fileSize + 1));
            GTLib::IO::Read(file, fileData, fileSize);
            fileData[fileSize] = '\0';
            
            GTLib::IO::Close(file);
            
            bool result = ShaderLibrary::LoadFromXML(fileData);
            
            free(fileData);
            return result;
        }
        
        return false;
#endif

        easyvfs_file* pFile = easyvfs_open(g_EngineContext->GetVFS(), fileName, EASYVFS_READ, 0);
        if (pFile != nullptr)
        {
            // We need to read the content of the file and then load it as XML. We cast the size to a size_t to
            // play nicely with 32-bit compilations. We can pretty safely assume the XML file will not exceed that.
            size_t fileSize = static_cast<size_t>(easyvfs_file_size(pFile));
            
            auto fileData = static_cast<char*>(malloc(fileSize + 1));
            easyvfs_read(pFile, fileData, fileSize, nullptr);
            fileData[fileSize] = '\0';
            
            easyvfs_close(pFile);
            
            bool result = ShaderLibrary::LoadFromXML(fileData);
            
            free(fileData);
            return result;
        }

        return false;
    }

    bool ShaderLibrary::LoadFromXML(const char *xml)
    {
        // We need to create a non-const copy because LoadFromXML() is destructive.
        auto input = GTLib::Strings::Create(xml);
        
        bool result = ShaderLibrary::LoadFromXML(input);
        
        GTLib::Strings::Delete(input);
        return result;
    }

    bool ShaderLibrary_LoadShaderFromXML(xml_node<> *node)
    {
        if (node != nullptr)
        {
            // We first need to load the shader into the shader list. All <shader> tags must have an 'id' attribute.
            xml_attribute<> *attr;
            if ((attr = node->first_attribute("id")) != nullptr)
            {
                // We have an ID, so we can load the shader.
                ShaderLibraryShader *shader = ShaderLibraryShaders::LoadShader(attr->value());  // attr->value() will be the shader's ID.
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
                                    if (GTLib::Strings::Equal(childNode->name(), "include"))
                                    {
                                        // If the include node has a 'url' attribute, it means it's using an external, named include. Otherwise, it's
                                        // assumed to be an anonymous include.
                                        xml_attribute<> *includeAttr = childNode->first_attribute("url");
                                        if (includeAttr != nullptr)
                                        {
                                            // The URL will begin with a '#'. We need to move to the character after that before loading the include.
                                            const char *url = includeAttr->value() + 1;     // <-- The '+ 1' will move past the '#' character.
                                    
                                            ShaderLibraryShader *include = ShaderLibraryShaders::FindByID(url);
                                            if (include == nullptr)
                                            {
                                                include = ShaderLibraryShaders::LoadShader(url);
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
                    GTEngine::PostError("ShaderLibrary - There was an unknown error loading a <shader> tag. This shader will be ignored.");
                }
            }
            else
            {
                GTEngine::PostError("ShaderLibrary - <shader> tags must have an 'id' attribute.");
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
                GTEngine::PostError("ShaderLibrary: %s", e.what());
                return false;
            }
            
            // We just iterate over every node. All we care about is <shader> nodes.
            auto shaderNode = document.first_node();
            while (shaderNode != nullptr)
            {
                if (strcmp(shaderNode->name(), "shader") == 0)
                {
                    ShaderLibrary_LoadShaderFromXML(shaderNode);
                }

                shaderNode = shaderNode->next_sibling();
            }

            return true;
        }
        
        return false;
    }

    void ShaderLibrary::AddShaderString(const char* id, const char* shaderStr)
    {
        ShaderLibraryShaders::Add(id, shaderStr);
    }

    const char* ShaderLibrary::GetShaderString(const char* id)
    {
        // First we need to try and find the shader. If we can't find it, we return null.
        ShaderLibraryShader *shader = ShaderLibraryShaders::FindByID(id);
        if (shader)
        {
            // All we need to do is resolve the shader and then return it's content.
            return shader->GetContent();
        }
        
        return nullptr;
    }
}


// Creation/Deletion.
namespace GTEngine
{
    // FIXME: The shader detection code here can be improved a bit. For starters, we should use a sorted data structure
    //        and sort by vertexShaderID and then again by fragmentShaderID. This will allow faster searching.

    struct ShaderInfo
    {
        ShaderInfo()
            : shader(nullptr),
              vertexShaderID(), fragmentShaderID(),
              refCount(0)
        {
        }
        
        Shader*        shader;
        GTLib::String vertexShaderID;
        GTLib::String fragmentShaderID;
        int            refCount;
        
        
    private:    // No copying.
        ShaderInfo(const ShaderInfo &);
        ShaderInfo & operator=(const ShaderInfo &);
    };
    GTLib::Vector<ShaderInfo*> AcquiredShaders;

    ShaderInfo* ShaderLibrary_FindShaderInfo(const char* vertexShaderID, const char* fragmentShaderID)
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

    ShaderInfo* ShaderLibrary_FindShaderInfo(Shader* shader)
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



    Shader* ShaderLibrary::Acquire(const char* vertexShaderID, const char* fragmentShaderID)
    {
        auto shaderInfo = ShaderLibrary_FindShaderInfo(vertexShaderID, fragmentShaderID);
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
        auto shaderInfo = ShaderLibrary_FindShaderInfo(shader);
        if (shaderInfo != nullptr)
        {
            ++shaderInfo->refCount;
        }

        return shader;
    }

    void ShaderLibrary::Unacquire(Shader* shader)
    {
        auto shaderInfo = ShaderLibrary_FindShaderInfo(shader);
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
        auto shaderInfo = ShaderLibrary_FindShaderInfo(vertexShaderID, fragmentShaderID);
        if (shaderInfo != nullptr)
        {
            return shaderInfo->shader;
        }

        return nullptr;
    }
}


// Shader object retrieval.
namespace GTEngine
{
    static Shader* GUIShader            = nullptr;
    static Shader* GUIShaderA8          = nullptr;
    static Shader* Textured2DQuadShader = nullptr;

    Shader* ShaderLibrary::GetGUIShader()
    {
        if (GUIShader == nullptr)
        {
            GUIShader = Renderer::CreateShader(ShaderLibrary::GetShaderString("Engine_GUI_VS"), ShaderLibrary::GetShaderString("Engine_GUI_FS"), nullptr);
        }

        return GUIShader;
    }
    
    Shader* ShaderLibrary::GetGUIShaderA8()
    {
        if (GUIShaderA8 == nullptr)
        {
            GUIShaderA8 = Renderer::CreateShader(ShaderLibrary::GetShaderString("Engine_GUI_VS"), ShaderLibrary::GetShaderString("Engine_GUI_FS_A8"), nullptr);
        }

        return GUIShaderA8;
    }


    Shader* ShaderLibrary::GetTextured2DQuadShader()
    {
        if (Textured2DQuadShader == nullptr)
        {
            Textured2DQuadShader = Renderer::CreateShader(ShaderLibrary::GetShaderString("Engine_Textured2DQuad_VS"), ShaderLibrary::GetShaderString("Engine_Textured2DQuad_FS"));
        }

        return Textured2DQuadShader;
    }
}


// Shutdown.
namespace GTEngine
{
    void ShaderLibrary::Shutdown()
    {
        Renderer::DeleteShader(GUIShader);
        Renderer::DeleteShader(GUIShaderA8);
        Renderer::DeleteShader(Textured2DQuadShader);

        GUIShader            = nullptr;
        GUIShaderA8          = nullptr;
        Textured2DQuadShader = nullptr;

        ShaderLibraryShaders::Clear();

        // All shader objects need to be deleted.
        for (size_t i = 0; i < AcquiredShaders.count; ++i)
        {
            Renderer::DeleteShader(AcquiredShaders.buffer[i]->shader);
            delete AcquiredShaders.buffer[i];
        }
        AcquiredShaders.Clear();
    }
}
