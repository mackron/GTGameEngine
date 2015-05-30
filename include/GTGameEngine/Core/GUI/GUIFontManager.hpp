// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_FontManager_hpp_
#define __GT_GUI_FontManager_hpp_

#include "GUIGlyphMapManager.hpp"
#include <GTLib/FontServer.hpp>
#include <GTLib/Map.hpp>
#include <cstdint>

namespace GT
{
    typedef uint32_t HGUIFont;

    /// Structure describing a font. This is used for font selection.
    struct GUIFontInfo
    {
        GUIFontInfo()
            : family(),
              styleFlags(0),
              sizeInPoints(10.0f), sizeInPixels(0.0f),
              dpiX(96), dpiY(96)
        {
        }
        
        GUIFontInfo(const GUIFontInfo &info)
            : family(info.family),
              styleFlags(info.styleFlags),
              sizeInPoints(info.sizeInPoints), sizeInPixels(info.sizeInPixels),
              dpiX(info.dpiX), dpiY(info.dpiY)
        {
        }
        
        virtual ~GUIFontInfo()
        {
        }
        
        GUIFontInfo & operator=(const GUIFontInfo &other)
        {
            if (this != &other)
            {
                this->family       = other.family;
                this->styleFlags   = other.styleFlags;
                this->sizeInPoints = other.sizeInPoints;
                this->sizeInPixels = other.sizeInPixels;
                this->dpiX         = other.dpiX;
                this->dpiY         = other.dpiY;
            }
            
            return *this;
        }

        bool IsEqual(const GUIFontInfo &other)
        {
            return this->family          == other.family        &&
                   this->styleFlags      == other.styleFlags    &&
                   this->sizeInPoints    == other.sizeInPoints  &&
                   this->sizeInPixels    == other.sizeInPixels  &&
                   this->dpiX            == other.dpiX          &&
                   this->dpiY            == other.dpiY;
        }
        
        
        /// The font family (Arial, Times, Courier, etc)
        GTLib::String family;
        
        /// The style flags of the font.
        uint32_t styleFlags;
        
        
        /// The size of the font, in points. We use a float here so we can have values like 8.5.
        float sizeInPoints;
        float sizeInPixels;
        
        /// The X DPI to use for the font. Default is 96.
        unsigned int dpiX;
        
        /// The Y DPI to use for the font. Default is 96.
        unsigned int dpiY;
    };



    /// Base class for managing fonts.
    class GUIFontManager
    {
    public:

        /// Constructor.
        GUIFontManager();

        /// Destructor.
        virtual ~GUIFontManager();


        /// Acquires the closest matching font based on the given font information.
        ///
        /// @param fontInfo [in] A FontInfo structure containing information about the font.
        ///
        /// @return A handle to the font if one is found; false otherwise.
        ///
        /// @remarks
        ///     So long as there is at least 1 font installed, this should never really return null.
        ///     @par
        ///     The font should be unacquired with UnacquireFont().
        HGUIFont AcquireFont(const GUIFontInfo &fontInfo);
        
        /// Unacquires a font that was previously acquired with AcquireFont().
        ///
        /// @param hFont [in] A pointer to the font to unacquire.
        void UnacquireFont(const HGUIFont hFont);


        /// Encodes a font-family string.
        ///
        /// @param family [in] The font family to encode.
        ///
        /// @return A 32-bit token that can be passed to DecodeFontFamily() to retrieve the decoded font family string.
        uint32_t EncodeFontFamily(const char* family);

        /// Decodes a font-family string from the given token.
        ///
        /// @param family [in] The font family token that was previously returned by EncodeFontFamily().
        ///
        /// @return The decoded font family string, or null if the string could not be decoded.
        const char* DecodeFontFamily(uint32_t family);



        /////////////////////////////////////////////
        // Virtual Methods

        /// Creates a font from the given font info.
        ///
        /// @param fontInfo [in] A reference to the object containing information about the font to create.
        virtual HGUIFont CreateFont(const GUIFontInfo &fontInfo) = 0;

        /// Deletes a font that was created with CreateFont().
        ///
        /// @param hFont [in] A handle to the font to delete.
        virtual void DeleteFont(HGUIFont hFont) = 0;

        /// Measures the given string using the given font.
        ///
        /// @param hFont             [in]  A handle to the font to use when measuring the string.
        /// @param text              [in]  The text to measure.
        /// @param textLengthInChars [in]  The number of characters in the string.
        /// @param widthOut          [out] A reference to the variable that will receive the width of the string.
        /// @param heightOut         [out] A reference to the variable that will receive the height of the string.
        ///
        /// @return True if the string was measured successfully; false otherwise.
        ///
        /// @remarks
        ///     If this returns false, \c widthOut and \c heightOut will be left unset.
        virtual bool MeasureString(HGUIFont hFont, const char* text, size_t textLengthInChars, unsigned int &widthOut, unsigned int &heightOut) = 0;



    private:

        /// Structure representing a font that's already loaded in the cache.
        struct LoadedFont
        {
            /// Constructor.
            LoadedFont(HGUIFont hFontIn, GUIFontInfo fontInfoIn)
                : hFont(hFontIn), fontInfo(fontInfoIn), referenceCount(1)
            {
            }
            
            /// Copy constructor.
            LoadedFont(const LoadedFont &other)
                : hFont(other.hFont), fontInfo(other.fontInfo), referenceCount(other.referenceCount)
            {
            }
            
            /// Assignment operator.
            LoadedFont & operator=(const LoadedFont &other)
            {
                if (this != &other)
                {
                    this->hFont          = other.hFont;
                    this->fontInfo       = other.fontInfo;
                    this->referenceCount = other.referenceCount;
                }
                
                return *this;
            }
            
            
            /// A pointer to the main font object.
            HGUIFont hFont;
            
            /// The FontInfo structure that was used to create the font. This is used in determining identical fonts.
            GUIFontInfo fontInfo;
            
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
        HGUIFont AcquireLoadedFont(const GUIFontInfo &fontInfo);
        
        /// Retrieves a pointer to a LoadedFont structure that is mapped to the givn GTLib::Font.
        ///
        /// @param font     [in]  A pointer to the GTLib::Font whose corresponding LoadedFont object is being retrieved.
        /// @param indexOut [out] A reference to the variable that will receive the index of the font info.
        ///
        /// @return A pointer to the LoadedFont object that corresponds to the given font.
        LoadedFont* FindLoadedFont(HGUIFont hFont, size_t &indexOut);


    private:

        /// The list of loaded fonts.
        GTLib::Vector<LoadedFont> m_loadedFonts;

        /// A map for mapping a font-family hash to it's actual string value.
        GTLib::Map<uint32_t, GTLib::String> m_fontFamilies;
    };




    /// Class for managing loaded fonts.
    ///
    /// When the GUI server wants a font, it will "acquire" it from the font cache. When the font is no longer
    /// needed, it will be "unacquired".
    ///
    /// Each loaded font has a reference counter. When the last reference has been unacquired, the font will be
    /// deleted completely.
    class GUIFontManagerOld
    {
    public:

        /// Constructor.
        GUIFontManagerOld(/*GUIImageManager &imageManager*/);

        /// Destructor.
        ~GUIFontManagerOld();


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
        GTLib::Font* AcquireFont(const GTLib::FontInfo &fontInfo);
        
        /// Unacquires a font that was previously acquired with AcquireFont().
        ///
        /// @param font [in] A pointer to the font to unacquire.
        void UnacquireFont(const GTLib::Font* font);
        

        /// Encodes a font-family string.
        ///
        /// @param family [in] The font family to encode.
        ///
        /// @return A 32-bit token that can be passed to DecodeFontFamily() to retrieve the decoded font family string.
        uint32_t EncodeFontFamily(const char* family);

        /// Decodes a font-family string from the given token.
        ///
        /// @param family [in] The font family token that was previously returned by EncodeFontFamily().
        ///
        /// @return The decoded font family string, or null if the string could not be decoded.
        const char* DecodeFontFamily(uint32_t family);

        
    private:
        
        /// Structure representing a font that's already loaded in the cache.
        struct LoadedFont
        {
            /// Constructor.
            LoadedFont(GTLib::Font* fontIn, GTLib::FontInfo fontInfoIn)
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
            GTLib::Font* font;
            
            /// The FontInfo structure that was used to create the font. This is used in determining identical fonts.
            GTLib::FontInfo fontInfo;
            
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
        GTLib::Font* AcquireLoadedFont(const GTLib::FontInfo &fontInfo);
        
        /// Retrieves a pointer to a LoadedFont structure that is mapped to the givn GTLib::Font.
        ///
        /// @param font     [in]  A pointer to the GTLib::Font whose corresponding LoadedFont object is being retrieved.
        /// @param indexOut [out] A reference to the variable that will receive the index of the font info.
        ///
        /// @return A pointer to the LoadedFont object that corresponds to the given font.
        LoadedFont* FindLoadedFont(const GTLib::Font* font, size_t &indexOut);
    
    


    private:

        /// The object for managing glyph maps.
        GUIGlyphMapManager m_glyphMapManager;

        /// The font server. The font server depends on the glyph map manager, so it must be declared and initialized afterwards.
        GTLib::FontServer m_fontServer;

        /// The list of loaded fonts.
        GTLib::Vector<LoadedFont> m_loadedFonts;

        /// A map for mapping a font-family hash to it's actual string value.
        GTLib::Map<uint32_t, GTLib::String> m_fontFamilies;
    };
}

#endif
