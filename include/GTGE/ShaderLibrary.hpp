// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ShaderLibrary
#define GT_ShaderLibrary

#include "Rendering/Shader.hpp"

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <GTGE/Core/rapidxml.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

namespace GT
{
    class Context;


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
            Strings::List<char> contentList;
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
        String id;

        /// The list of includes making up this shader. This includes both named and anonymous includes.
        List<ShaderLibraryShader*> includes;

        /// The list containing pointers to the anonymous includes. These includes are tied to this shader, and
        /// only this shader.
        List<ShaderLibraryShader*> anonIncludes;

        /// The content of the shader. This will start out as empty, but will be given content when the
        /// shader content is constructed with ConstructContent(), or SetContent() is called.
        String content;


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
        ShaderLibraryShader* LoadShader(const char *id)
        {
            ShaderLibraryShader* shader = nullptr;

            auto item = m_shaders.Find(id);
            if (item == nullptr)
            {
                shader = new ShaderLibraryShader(id);
                m_shaders.Add(shader->GetID(), shader);           // <-- DON'T use 'id' in place of 'shader->GetID()'.
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
        ShaderLibraryShader* FindByID(const char *id)
        {
            auto item = m_shaders.Find(id);
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
        ShaderLibraryShader* Add(const char* id, const char* content)
        {
            auto newShader = LoadShader(id);
            newShader->SetContent(content);

            return newShader;
        }


        /**
        *   \brief  Clears the list of shaders.
        */
        void Clear()
        {
            for (size_t i = 0; i < m_shaders.count; ++i)
            {
                delete m_shaders.buffer[i]->value;
            }

            m_shaders.Clear();
        }


    private:

        Dictionary<ShaderLibraryShader*> m_shaders;
    };


    /// Static class for loading and retrieving shader objects.
    ///
    /// The main function of this class is to load shader strings from .shader files. It can also be used to retrieve pointers to
    /// shader objects.
    class ShaderLibrary
    {
    public:

        /// Constructor.
        ShaderLibrary(Context &context);

        /// Destructor.
        ~ShaderLibrary();



        /// Shuts down the shader library. This will delete any shader objects.
        void Shutdown();


    public:

        /**
        *   \brief                 Loads a set of shader and pipeline files from the given directory.
        *   \param  directory [in] The directory to load the shader files from.
        *   \param  recursive [in] Determines whether or not to call the function recursively for child directories. Set to true by default.
        *
        *   \remarks
        *       Before a shader string can be retrieved, they must first be loaded. If a shader references an element that has not yet been
        *       loaded, it will fail to resolve it's final string.
        *       \par
        *       This function is not thread-safe.
        */
        bool LoadFromDirectory(const char *directory, bool recursive = true);

        /**
        *   \brief                Loads a shader or pipeline file.
        *   \param  fileName [in] The name of the file to load.
        *
        *   \remarks
        *       This function is not thread-safe.
        */
        bool LoadFromFile(const char *fileName);

        /**
        *   \brief           Loads a shader or pipeline from an XML string.
        *   \param  xml [in] The XML string to load from.
        */
        bool LoadFromXML(const char *xml);
        bool LoadFromXML(char *xml);
        bool LoadShaderFromXML(rapidxml::xml_node<> *node);
        
        /**
        *   \brief                 Manually adds a shader string.
        *   \param  id        [in] The ID of the shader string.
        *   \param  shaderStr [in] The shader string.
        */
        void AddShaderString(const char* id, const char* shaderStr);
        
        /**
        *   \brief          Retrieves a shader string for the given ID.
        *   \param  id [in] The ID of the shader to retrieve, not including the leading hash ('#').
        */
        const char* GetShaderString(const char *id);


    // Acquire/Unacquire.
    public:

        /// Retrieves a shader object from IDs. See remarks.
        /// @param vertexShaderID   [in] The ID of the vertex shader to use with the shader object.
        /// @param fragmentShaderID [in] The ID of the fragment shader to use with the shader object.
        /// @return A pointer to the Shader object, or nullptr if an error occurs. See remarks.
        ///
        /// @remarks
        ///     If a shader has already been acquired using the same combination of shaders, this function will return a pointer
        ///     to that already-existing shader. An internal counter will be incremeneted which will be used by UnacquireShader().
        ///     UnacquireShader() will decrement that counter, and when it reaches 0, will delete the shader object properly. Thus,
        ///     Always ensure each call to AcquireShader() is matched up with a call to UnacquireShader(). Applications need to be
        ///     aware of this detail when they need to change properties of the shader.
        ///     @par
        ///     In some cases it is desireable to give an object access to an already acquired shader. Use AcquireShader(Shader*)
        ///     to increment the reference count of an already acquired shader without having to use the shader IDs.
        Shader* Acquire(const char* vertexShaderID, const char* fragmentShaderID);

        /// Acquires an already acquired shader.
        /// @param shader [in] The shader that is being acquired.
        /// @return <shader>. Done for consistency with AcquireShader(const char*, const char*).
        Shader* Acquire(Shader* shader);


        /// Unacquires a shader object. See remarks.
        /// @param shader [in] A pointer to the shader object to unacquire.
        ///
        /// @remarks
        ///     An internal counter is used to determine whether or not the shader itself should be deleted. UnacquireShader() will
        ///     decrement the counter. If the counter reaches 0, the shader object will be deleted completely.
        void Unacquire(Shader* shader);


    public:

        /// Searches for an already-acquired shader object based on the shader IDs.
        /// @param vertexShaderID [in] The ID of the vertex shader.
        /// @param fragmentShaderID [in] The ID of the fragment shader.
        /// @return A pointer to the shader object, or null if it could not be found.
        ///
        /// @remarks
        ///     Use this function to determine whether or not a shader object of the given IDs has been acquired. This will only search
        ///     acquired shaders.
        Shader* FindShader(const char* vertexShaderID, const char* fragmentShaderID);



    // Retrieval functions for GTEngine shaders.
    public:

        /// Retrieves the shader that is used for drawing GUI elements using RGB8 or RGBA8 texture formats.
        Shader* GetGUIShader();
        
        /// Retrieves the shader that is used for drawing GUI elements that use an A8 texture format.
        Shader* GetGUIShaderA8();


        /// Retrieves the shader that is used to draw a simple textured fullscreen quad.
        Shader* GetFullscreenQuadShader();

        /// Retrieves the shader that is used to draw a textured 2D quad.
        Shader* GetTextured2DQuadShader();

        /// Retrieves the shader that is used to draw a solid colour background quad.
        Shader* GetColouredBGQuadShader();

        /// Retrieves the shader that is used to draw wireframes.
        Shader* GetLineShader();


        /// Retrieves the depth clearing shader.
        Shader* GetDepthClearShader();


    private:

        /// A reference to the context that owns this library.
        Context &m_context;


        Shader* m_GUIShader;
        Shader* m_GUIShaderA8;
        Shader* m_Textured2DQuadShader;


        ShaderLibraryShaders m_shaders;


        struct ShaderInfo
        {
            ShaderInfo()
                : shader(nullptr),
                  vertexShaderID(), fragmentShaderID(),
                  refCount(0)
            {
            }
        
            Shader*        shader;
            String vertexShaderID;
            String fragmentShaderID;
            int            refCount;
        
        
        private:    // No copying.
            ShaderInfo(const ShaderInfo &);
            ShaderInfo & operator=(const ShaderInfo &);
        };
        Vector<ShaderInfo*> AcquiredShaders;


        ShaderInfo* FindShaderInfo(const char* vertexShaderID, const char* fragmentShaderID);
        ShaderInfo* FindShaderInfo(Shader* shader);
    };
}


#endif
