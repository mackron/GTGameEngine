// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/FontServer.hpp>
#include "FontEngine_FCFT.hpp"

#include "FontEngine_FCFT.hpp"
#include "FontEngine_Win32.hpp"

#include <GTGE/Core/System.hpp>
#include <cassert>

namespace GT
{
    FontServer::FontServer(GlyphMapManager &glyphMapManager)
        : m_fontEngine(nullptr), m_glyphMapManager(&glyphMapManager), eventHandlers()
    {
    #if defined(__linux__)
        m_fontEngine = new FontEngine_FCFT(m_glyphMapManager);
    #elif defined(_WIN32)
        m_fontEngine = new FontEngine_Win32(m_glyphMapManager);
    #endif
    }

    FontServer::~FontServer()
    {
        delete m_fontEngine;
    }
    
    
    Font* FontServer::CreateFont(const FontInfo &fontInfo)
    {
        assert(m_fontEngine != nullptr);
        {
            auto handle = m_fontEngine->CreateFont(fontInfo);
            if (handle != 0)
            {
                return new Font(*this, handle);
            }
        }
        
        return nullptr;
    }
    
    void FontServer::DeleteFont(Font* font)
    {
        assert(m_fontEngine != nullptr);
        {
            if (font != nullptr)
            {
                m_fontEngine->DeleteFont(font->GetFontHandle());
                delete font;
            }
        }
    }
    

    void FontServer::AttachEventHandler(FontEventHandler* eventHandler)
    {
        // Only attach if it isn't already attached...
        if (this->eventHandlers.Find(eventHandler) == nullptr)
        {
            this->eventHandlers.Append(eventHandler);
        }
    }

    void FontServer::DetachEventHandler(FontEventHandler* eventHandler)
    {
        this->eventHandlers.Remove(this->eventHandlers.Find(eventHandler));
    }
}
