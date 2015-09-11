// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUI_FontManager
#define GT_GUI_FontManager

#include <GTLib/FontServer.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/Colour.hpp>
#include <cstdint>

#define GT_MAX_TEXT_RUN_SIZE_IN_BYTES   128

namespace GT
{
    typedef uint32_t HGUIFont;

    static const uint32_t FontOption_NoClearType  = (1 << 0);


    /// Structure describing a font. This is used for font selection.
    struct GUIFontInfo
    {
        GUIFontInfo()
            : family(),
              styleFlags(0),
              size(14),
              dpiX(96), dpiY(96)
        {
        }

        GUIFontInfo(const GUIFontInfo &info)
            : family(info.family),
              styleFlags(info.styleFlags),
              size(info.size),
              dpiX(info.dpiX), dpiY(info.dpiY)
        {
        }

        GUIFontInfo & operator=(const GUIFontInfo &other)
        {
            if (this != &other)
            {
                this->family     = other.family;
                this->styleFlags = other.styleFlags;
                this->size       = other.size;
                this->dpiX       = other.dpiX;
                this->dpiY       = other.dpiY;
            }

            return *this;
        }

        bool IsEqual(const GUIFontInfo &other)
        {
            return this->family     == other.family     &&
                   this->styleFlags == other.styleFlags &&
                   this->size       == other.size       &&
                   this->dpiX       == other.dpiX       &&
                   this->dpiY       == other.dpiY;
        }


        /// The font family (Arial, Times, Courier, etc)
        GTLib::String family;

        /// The style flags of the font.
        uint32_t styleFlags;

        /// The size of the font, in points. We use a float here so we can have values like 8.5.
        float size;

        /// The X DPI to use for the font. Default is 96.
        unsigned int dpiX;

        /// The Y DPI to use for the font. Default is 96.
        unsigned int dpiY;
    };

    /// The structure containing the metrics of a glyph.
    struct GUIGlyphMetrics
    {
        GUIGlyphMetrics()
            : width(0), height(0), advance(0), bearingX(0), bearingY(0)
        {
        }

        /// The width of the glyph.
        int width;

        /// The height of the glyph.
        int height;

        /// The advance for the glyph. This is the distance to advance the pen position when rendering the glyph.
        int advance;

        /// The horizontal bearing. This is the distance between the current pen position and the leftmost border of the glyph image.
        int bearingX;

        /// The vertical bearing. This is the distance between the base line and the topmost border of the glyph image.
        int bearingY;
    };



    /// Base class for managing fonts.
    class GUIFontManager
    {
    public:

        /// Constructor.
        GUIFontManager(uint32_t options = 0);

        /// Destructor.
        virtual ~GUIFontManager();


        /// Retrieves the option flags that were passed to the constructor.
        uint32_t GetOptions() const;


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


        /// Helper function for deleting every font.
        void DeleteAllFonts();


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
        virtual bool MeasureString(HGUIFont hFont, const char* text, size_t textLengthInChars, int &widthOut, int &heightOut) = 0;

        /// Retrieves the ascent of the given font.
        ///
        /// @param hFont [in] The font.
        ///
        /// @return The ascent.
        virtual int GetAscent(HGUIFont hFont) const = 0;

        /// Retrieves the descent of the given font.
        ///
        /// @param hFont [in] The font.
        ///
        /// @return The descent.
        virtual int GetDescent(HGUIFont hFont) const = 0;

        /// Retrieves the line height of the given font.
        ///
        /// @param hFont [in] The font.
        ///
        /// @return The line height for the given font.
        virtual int GetLineHeight(HGUIFont hFont) const = 0;

        /// Retrievse the metrics of the glyph of the given character and font.
        ///
        /// @param hFont      [in]  A handle to the font.
        /// @param character  [in]  The character whose glyph metrics are being retrieved.
        /// @param metricsOut [out] A reference to the object that will receive the reference.
        virtual bool GetGlyphMetrics(HGUIFont hFont, char32_t character, GUIGlyphMetrics &metricsOut) const = 0;


        /// Calculates the position to place a text cursor based on the given input position.
        virtual bool GetTextCursorPositionFromPoint(HGUIFont hFont, const char* text, unsigned int textLengthInChars, unsigned int maxWidth, int inputPosX, int &textCursorPosXOut, unsigned int &characterIndexOut) const = 0;

        /// Calculates the position to places a text cursor based on the given character index.
        virtual bool GetTextCursorPositionFromCharacter(HGUIFont hFont, const char* text, unsigned int characterIndex, int &textCursorPosXOut) const = 0;


        /// Draws the given text to a raw buffer.
        ///
        /// @remarks
        ///     The output buffer should be 32-bits per pixel, and tightly packed based on the return value of MeasureString().
        virtual bool DrawTextToBuffer(HGUIFont hFont, const char* text, size_t textLengthInChars, GTLib::Colour color, void* bufferOut, size_t bufferOutSize) = 0;


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
        /// @param hFont    [in]  A pointer to the GTLib::Font whose corresponding LoadedFont object is being retrieved.
        /// @param indexOut [out] A reference to the variable that will receive the index of the font info.
        ///
        /// @return A pointer to the LoadedFont object that corresponds to the given font.
        LoadedFont* FindLoadedFont(HGUIFont hFont, size_t &indexOut);


    private:

        /// The option flags specified in the constructor.
        uint32_t m_options;

        /// The list of loaded fonts.
        GTLib::Vector<LoadedFont> m_loadedFonts;

        /// A map for mapping a font-family hash to it's actual string value.
        GTLib::Map<uint32_t, GTLib::String> m_fontFamilies;
    };
}

#endif
