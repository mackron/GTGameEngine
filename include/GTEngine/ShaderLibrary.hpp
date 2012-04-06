
#ifndef __GTEngine_ShaderLibrary_hpp_
#define __GTEngine_ShaderLibrary_hpp_

#include "Rendering/Shader.hpp"

namespace GTEngine
{
    /// Static class for loading and retrieving shader objects.
    ///
    /// The main function of this class is to load shader strings from .shader files. It can also be used to retrieve pointers to
    /// shader objects.
    ///
    /// Because there is potentially a very large combination of shaders, the shader library uses lazy initialization when retrieving
    /// shader objects.
    class ShaderLibrary
    {
    // Startup/Shutdown.
    public:

        /// Shuts down the shader library. This will delete any shader objects.
        static void Shutdown();


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
        static bool LoadFromDirectory(const char *directory, bool recursive = true);

        /**
        *   \brief                Loads a shader or pipeline file.
        *   \param  fileName [in] The name of the file to load.
        *
        *   \remarks
        *       This function is not thread-safe.
        */
        static bool LoadFromFile(const char *fileName);

        /**
        *   \brief           Loads a shader or pipeline from an XML string.
        *   \param  xml [in] The XML string to load from.
        */
        static bool LoadFromXML(const char *xml);
        static bool LoadFromXML(char *xml);
        
        /**
        *   \brief                 Manually adds a shader string.
        *   \param  id        [in] The ID of the shader string.
        *   \param  shaderStr [in] The shader string.
        */
        static void AddShaderString(const char* id, const char* shaderStr);
        
        /**
        *   \brief          Retrieves a shader string for the given ID.
        *   \param  id [in] The ID of the shader to retrieve, not including the leading hash ('#').
        */
        static const char* GetShaderString(const char *id);


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
        static Shader* Acquire(const char* vertexShaderID, const char* fragmentShaderID);

        /// Acquires an already acquired shader.
        /// @param shader [in] The shader that is being acquired.
        /// @return <shader>. Done for consistency with AcquireShader(const char*, const char*).
        static Shader* Acquire(Shader* shader);


        /// Unacquires a shader object. See remarks.
        /// @param shader [in] A pointer to the shader object to unacquire.
        ///
        /// @remarks
        ///     An internal counter is used to determine whether or not the shader itself should be deleted. UnacquireShader() will
        ///     decrement the counter. If the counter reaches 0, the shader object will be deleted completely.
        static void Unacquire(Shader* shader);


    public:

        /// Searches for an already-acquired shader object based on the shader IDs.
        /// @param vertexShaderID [in] The ID of the vertex shader.
        /// @param fragmentShaderID [in] The ID of the fragment shader.
        /// @return A pointer to the shader object, or null if it could not be found.
        ///
        /// @remarks
        ///     Use this function to determine whether or not a shader object of the given IDs has been acquired. This will only search
        ///     acquired shaders.
        static Shader* FindShader(const char* vertexShaderID, const char* fragmentShaderID);



    // Retrieval functions for GTEngine shaders.
    public:

        /**
        *   \brief  Retrieves the shader that is used to draw a textured quad on a GUI.
        */
        static Shader* GetGUIQuadShader();

        /**
        *   \brief  Retrieves the shader that is used to draw text on a GUI.
        */
        static Shader* GetGUITextShader();

        /**
        *   \brief  Retrieves the shader that is used to draw shadows on a GUI.
        */
        static Shader* GetGUIShadowShader();

        /**
        *   \brief  Retrieves the shader that is used to draw a simple textured fullscreen quad.
        */
        static Shader* GetFullscreenQuadShader();

        /**
        *   \brief  Retrieves the shader that is used to draw wireframes.
        */
        static Shader* GetLineShader();

    };    
}


#endif
