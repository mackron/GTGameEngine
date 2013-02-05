// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_FontManager_hpp_
#define __GTEngine_FontManager_hpp_

#include <GTType/FontEventHandler.hpp>

namespace GTEngine
{
    class Texture2D;

    /**
    *   \brief  Class for managing fonts.
    */
    class FontManager
    {
    public:

        /**
        *   \brief  Initialises the font manager.
        */
        static bool Startup();

        /**
        *   \brief  Uninitialises the font manager.
        */
        static void Shutdown();

        /**
        *   \brief  Retrieves the texture that is mapped to the given font.
        */
        static Texture2D* GetTexture(const GTType::Font &font);


        /// Called when a font needs to be loaded.
        static void OnCreateFont(GTType::Font &font);

        /// Called when a font needs to be unloaded.
        static void OnDeleteFont(GTType::Font &font);
    };

    
}

#endif