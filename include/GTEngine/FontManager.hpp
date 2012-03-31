
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
        static bool Initialise();

        /**
        *   \brief  Uninitialises the font manager.
        */
        static void Uninitialise();

        /**
        *   \brief  Retrieves the texture that is mapped to the given font.
        */
        static Texture2D* GetTexture(const GTType::Font &font);


        /// Called by GTEngine::FontEventHandler.
        static void OnCreateFont(GTType::FontServer &server, GTType::Font &font);

        /// Called by GTEngine::FontEventHandler.
        static void OnDeleteFont(GTType::FontServer &server, GTType::Font &font);
    };

    
}

namespace GTEngine
{
    class FontEventHandler : public GTType::FontEventHandler
    {
    public:

        /// Constructor.
        FontEventHandler();

        /// Destructor.
        ~FontEventHandler();

        /// OnCreateFont.
        void OnCreateFont(GTType::FontServer &server, GTType::Font &font);

        /// OnDeleteFont.
        void OnDeleteFont(GTType::FontServer &server, GTType::Font &font);
    };

    /// The global font manager object. This is declared in FontManager.cpp.
    extern FontEventHandler GlobalFontEventHandler;
}

#endif