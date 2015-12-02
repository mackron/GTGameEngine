// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_FontServer
#define GT_FontServer

#include "FontEngine.hpp"
#include "FontEventHandler.hpp"
#include "GlyphMapManager.hpp"
#include "Font.hpp"
#include <GTGE/Core/List.hpp>

namespace GT
{
    /// The main interface to GTType. An application must create a font server to use GTType.
    class FontServer
    {
    public:

        /// Constructor.
        FontServer(GlyphMapManager &glyphMapManager);

        /// Destructor.
        ~FontServer();

        
        /// Retrieves a reference to the font engine.
        ///
        /// @remarks
        ///     This should only need to be used internally.
              FontEngine & GetFontEngine()       { assert(m_fontEngine != nullptr); return *m_fontEngine; }
        const FontEngine & GetFontEngine() const { assert(m_fontEngine != nullptr); return *m_fontEngine; }
        
        
        /// Creates a font that most closely matches the given font info.
        ///
        /// @param fontInfo [in] A reference to the FontInfo structure that contains the font information.
        ///
        /// @return A pointer to the closest matching font, or null if a font can't be found.
        ///
        /// @remarks
        ///     The returned font must be deleted with DeleteFont().
        ///     @par
        ///     This does not do caching - if this is called twice with the same input, separate, independant fonts will
        ///     be returned, each of which must be deleted with DeleteFont().
        Font* CreateFont(const FontInfo &fontInfo);
        
        /// Deletes a font that was created with CreateFont().
        ///
        /// @param font [in] A pointer to the font to delete.
        void DeleteFont(Font* font);


        /// Attaches an event handler to the server.
        void AttachEventHandler(FontEventHandler* eventHandler);

        /// Detaches an event handler from the server.
        void DetachEventHandler(FontEventHandler* eventHandler);

        
        /// Retrieves a pointer to the current glyph manager.
              GlyphMapManager* GetGlyphMapManager()       { return m_glyphMapManager; }
        const GlyphMapManager* GetGlyphMapManager() const { return m_glyphMapManager; }
        

    // Event handler helpers.
    private:

        /// OnCreateFont.
        void OnCreateFont(FontServer &server, Font &font)
        {
            for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
            {
                i->value->OnCreateFont(server, font);
            }
        }

        /// OnDeleteFont.
        void OnDeleteFont(FontServer &server, Font &font)
        {
            for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
            {
                i->value->OnDeleteFont(server, font);
            }
        }


    private:
        
        /// A pointer to the font engine. This will be allocated in the constructor and should never be null.
        FontEngine* m_fontEngine;
        
        /// A pointer to the glyph map manager. This can be null, in which case the server will not be able to build glyph maps.
        GlyphMapManager* m_glyphMapManager;

        /// The list of event handlers.
        List<FontEventHandler*> eventHandlers;
        
        
    private:    // No copying.
        FontServer(const FontServer &);
        FontServer & operator=(const FontServer &);
    };
}

#endif
