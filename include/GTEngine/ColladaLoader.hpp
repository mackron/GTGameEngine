
#ifndef __GTEngine_ColladaLoader_hpp_
#define __GTEngine_ColladaLoader_hpp_

namespace GTEngine
{
    class SceneNode;

    /**
    *   \brief  Static class for loading COLLADA files.
    *
    *   \remarks
    *       When a COLLADA file is loaded and/or parsed, it creates and returns a scene node containing everything
    *       that can be used by the engine. It will have the same parent/child structure as defined by the COLLADA
    *       file.
    *       \par
    *       Each node extracted from the file will have the appropriate components attached. For example, a camera
    *       will have a camera component attached; a node with geometric data will have a Model component attached, etc.
    */
    class ColladaLoader
    {
    public:

        /**
        *   \brief               Passes the content of a COLLADA file and returns a SceneNode representing that data.
        *   \param  content [in] The content of the COLLADA file. This is NOT the file name.
        */
        static SceneNode * Parse(const char *content);
        
        /**
        *   \brief                Loads and parses a COLLADA file.
        *   \param  fileName [in] The file name and path of the COLLADA file to load.
        */
        static SceneNode * Load(const char *fileName);
    };
}


#endif
