// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIFontCache
#define GT_GUIFontCache

#include <GTGE/Core/FontServer.hpp>
#include <GTGE/Core/Vector.hpp>

namespace GT
{
    /// Class used for caching fonts so same fonts are reused instead of re-loaded.
    ///
    /// Each instantiation of a font cache will be passed a reference to a font server. The font cache itself will
    /// manage the creation and deletion of each font. When the GUI server wants a font, it will "acquire" it from
    /// the font cache. When the font is no longer needed, it will be "unacquired".
    ///
    /// Each loaded font has a reference counter. When the last reference has been unacquired, the font will be
    /// deleted completely.
    class GUIFontCache
    {
    public:
    
        /// Constructor.
        GUIFontCache(GT::FontServer &fontServer);
        
        /// Destructor.
        ~GUIFontCache();
        
        
        /// Acquires the closest matching font based on the given font information.
        ///
        /// @param fontInfo [in] A FontInfo structure containing information about the font.
        ///
        /// @return A pointer to the font if one is found; false otherwise.
        ///
        /// @remarks
        ///     So long as there is at least 1 font installed, this should never really return null.
        ///     @par
        ///     The font should be unacquired with UnacquireFont().
        GT::Font* AcquireFont(const GT::FontInfo &fontInfo);
        
        /// Unacquires a font that was previously acquired with AcquireFont().
        ///
        /// @param font [in] A pointer to the font to unacquire.
        void UnacquireFont(const GT::Font* font);
        
        
    private:
        
        /// Structure representing a font that's already loaded in the cache.
        struct LoadedFont
        {
            /// Constructor.
            LoadedFont(GT::Font* fontIn, GT::FontInfo fontInfoIn)
                : font(fontIn), fontInfo(fontInfoIn), referenceCount(1)
            {
            }
            
            /// Copy constructor.
            LoadedFont(const LoadedFont &other)
                : font(other.font), fontInfo(other.fontInfo), referenceCount(other.referenceCount)
            {
            }
            
            /// Assignment operator.
            LoadedFont & operator=(const LoadedFont &other)
            {
                if (this != &other)
                {
                    this->font           = other.font;
                    this->fontInfo       = other.fontInfo;
                    this->referenceCount = other.referenceCount;
                }
                
                return *this;
            }
            
            
            /// A pointer to the main font object.
            GT::Font* font;
            
            /// The FontInfo structure that was used to create the font. This is used in determining identical fonts.
            GT::FontInfo fontInfo;
            
            /// The reference count. When this hits zero, it marks the end of the font.
            int referenceCount;
        };
    
    
    private:
        
        /// Retrieves a pointer to the already-loaded font that has the same font info.
        ///
        /// @param fontInfo [in] The FontInfo structure of the font being retrieved.
        ///
        /// @return A pointer to the already-loaded font, or null if it isn't already loaded.
        ///
        /// @remarks
        ///     This does not create a new font - it simply looks at the already loaded fonts.
        ///     @par
        ///     This will increment the reference counter if non-null is returned.
        GT::Font* AcquireLoadedFont(const GT::FontInfo &fontInfo);
        
        /// Retrieves a pointer to a LoadedFont structure that is mapped to the givn Font.
        ///
        /// @param font     [in]  A pointer to the Font whose corresponding LoadedFont object is being retrieved.
        /// @param indexOut [out] A reference to the variable that will receive the index of the font info.
        ///
        /// @return A pointer to the LoadedFont object that corresponds to the given font.
        LoadedFont* FindLoadedFont(const GT::Font* font, size_t &indexOut);
    
    
    private:
        
        
        /// A reference to the font server.
        GT::FontServer &m_fontServer;
        
        /// The list of loaded fonts.
        GT::Vector<LoadedFont> m_loadedFonts;


	private:	// No copying
		GUIFontCache(const GUIFontCache &);
		GUIFontCache & operator=(const GUIFontCache &);
    };
}

#endif
