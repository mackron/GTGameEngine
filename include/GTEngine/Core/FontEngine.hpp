// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_FontEngine_hpp_
#define __GTLib_FontEngine_hpp_

#include "GlyphMapManager.hpp"
#include "FontInfo.hpp"
#include "FontMetrics.hpp"
#include "GlyphMetrics.hpp"
#include "KerningVector.hpp"
#include <GTEngine/Core/Rect.hpp>
#include <functional>

namespace GTLib
{
    /// A handle to a font for use by the font engine. The meaning of the value of a font handle is dependant on the implementation
    /// of the FontEngine class.
    typedef size_t FontHandle;
    
    /// A handle to a glyph for use by the font engine. The meaning of the value of the glyph handle is dependant on the implementation
    /// of the FontEngine class.
    typedef size_t GlyphHandle;
    
    /// Class used for font discovery and glyph rendering.
    ///
    /// This class will have a different implementation depending on the environment. For Windows builds, the Win32 API will
    /// be used directly. For Linux builds, FontConfig and FreeType will be used.
    ///
    /// There are two main parts to this class - the platform dependant part and the patform independant part. The platform
    /// dependant part is implemented in FontEngine_FCFT.cpp and FontEngine_Win32.cpp. The platform independant part is
    /// implemented in FontEngine.cpp.
    ///
    /// The platform dependant part has only two main things - font/glyph creation, and string measuring. String measurement
    /// is platform dependant mainly because of Win32's not-that-good kerning API.
    class FontEngine
    {
    public:

        typedef std::function<bool (const FontEngine &, char32_t, GlyphHandle, const GTLib::Rect<int> &, GlyphMetrics &, int &, int &)> MeasureString_OnHandleCharacter;
        
        /// A callback for use when measuring a string.
        class MeasureStringCallback
        {
        public:
        
            /// Constructor.
            MeasureStringCallback() {}
            
            /// Destructor.
            virtual ~MeasureStringCallback() {}
            
            
            
            /// Called once at the start of the iteration and determines the size of a tab, in pixels.
            ///
            /// @return The size in pixels of a tab.
            ///
            /// @remarks
            ///     Currently, tabs will always be aligned to the tab grid, with each grid square being the size of a tab.
            virtual int GetTabSize() const = 0;
            
            /// Retrieves the start position the string, in pixels.
            ///
            /// @remarks
            ///     This is needed in order to align tabs correctly.
            virtual int GetXStartPosition() const { return 0; }
            virtual int GetYStartPosition() const { return 0; }
            
            
            
            /// Called when a character is handled.
            ///
            /// @param fontEngine   [in] A reference to the font engine.
            /// @param character    [in] The character to handle.
            /// @param glyph        [in] A handle to the glyph representation.
            /// @param rect         [in] The glyph rectangle.
            /// @param metrics      [in] The metrics of the character's glyph.
            /// @param penPositionX [in] The position of the pen on the horizontal axis.
            /// @param penPositionY [in] The position of the pen on the vertical axis.
            ///
            /// @return True if the measurement should continue; false otherwise.
            ///
            /// @remarks
            ///     This is called before the position is advanced. It is possible to modify the advance by changing
            ///     the value in 'metrics'.
            ///     @par
            ///     The pen positions can be changed at will.
            virtual bool HandleCharacter(const FontEngine &fontEngine, char32_t character, GlyphHandle glyph, const GTLib::Rect<int> &rect, GlyphMetrics &metrics, int &penPositionX, int &penPositionY) = 0;
        };
        
        
    public:
        
        /// Finds and creates the best font based on the given font information.
        ///
        /// @param fontInfo [in] A reference to the FontInfo structure containing information about the font.
        ///
        /// @return A handle to the font.
        ///
        /// @remarks
        ///     The return value is a handle that will be used as a key by the main font server. The meaning of the handle depends
        ///     on the implementation of this class.
        ///     @par
        ///     The returned handle must be deleted with DeleteFont.
        virtual FontHandle CreateFont(const FontInfo &fontInfo) = 0;
        
        /// Deletes a font created with CreateFont().
        ///
        /// @param font [in] A handle to the font to delete.
        virtual void DeleteFont(FontHandle font) = 0;
    
    
        /// Retrieves the metrics of the font.
        ///
        /// @param font       [in]  A handle to the font whose metrics are being retrieved.
        /// @param metricsOut [out] A reference to the object that will receive the font metrics.
        virtual void GetFontMetrics(FontHandle font, FontMetrics &metricsOut) const = 0;

        
        /// Retrieves a glyph from the given font and character, and creates it if it doesn't already exist.
        ///
        /// @param font      [in] The font to create the glyph from.
        /// @param character [in] The character whose glyph is being created.
        ///
        /// @return A handle to the new glyph.
        ///
        /// @remarks
        ///     This function is the main glyph creation function. The idea is that glyph creation should be a completely transparent
        ///     operation without any need for manual creation.
        ///     @par
        ///     This will render the glyph's bitmap and place it into a glyph map using the glyph map manager.
        ///     @par
        ///     If the glyph has already been created, this will return a handle to the existing glyph.
        virtual GlyphHandle GetGlyph(FontHandle font, char32_t character) const = 0;
        
        /// Retrieves the glyph map the given glyph's bitmap is stored in, along with UV coordinates.
        ///
        /// @param glyph       [in]  The glyph whose owner glyph map is being retrieved.
        /// @param uvCoordsOut [out] The Rect structure that will receive the UV coordinates.
        ///
        /// @return A handle to the glyph map that is housing the glyph's bitmap data.
        virtual GlyphMapHandle GetGlyphMap(GlyphHandle glyph, GTLib::Rect<float> &uvCoordsOut) const = 0;

        /// Retrieves the metrics of the given glyph.
        ///
        /// @param glyph      [in]  The glyph whose metrics are being retrieved.
        /// @param metricsOut [out] A reference to the Metrics object that will receive the metrics.
        virtual void GetGlyphMetrics(GlyphHandle glyph, GlyphMetrics &metricsOut) const = 0;
        
        
        /// Measures a string.
        ///
        /// @param font        [in] The font to use when measuring.
        /// @param inputString [in] The beginning of the string to measure.
        /// @param callback    [in] The callback.
        ///
        /// @remarks
        ///     This will only measure along the X axis. It does not handle multiple lines. The string is assumed to be only a single
        ///     line.
        ///     @par
        ///     The callback is used to allow the callee to handle each character in the string individually. It also allows the callee
        ///     to control when the measurement of the string should stop. If 'inputString' is not null terminated, the callback will
        ///     need to be the one to stop iterating.
        ///     @par
        ///     The callback is also responsible for specifying options such as the tab size.
        ///     @par
        ///     The start position is controlled with callback.GetStartPosition().
        ///     @par
        ///     The size of a tab grid segment is controlled with callback.GetTabSize().
        virtual void MeasureString(FontHandle font, const char* inputString, MeasureStringCallback &callback) const = 0;
        
        
        virtual void MeasureString(FontHandle font, const char* inputString, unsigned int tabSize, MeasureString_OnHandleCharacter onHandleCharacter) const = 0;

        
        //////////////////////////////////////////////////////
        // Cross Platform
        
    public:
        
        /// Destructor.
        virtual ~FontEngine();


        /// Maps the image data of the given glyph map.
        ///
        /// @param glyphMap [in] The glyph map whose image data is being mapped.
        ///
        /// @return A pointer to the mapped image data.
        ///
        /// @remarks
        ///     The image data should be unmapped with UnmapGlyphMapData().
        void* MapGlyphMapData(GlyphMapHandle hGlyphMap) const;

        /// Unmaps the image data of the given glyph map.
        ///
        /// @param glyphMap [in] the glyph map whose image data is being unmapped.
        void UnmapGlyphMapData(GlyphMapHandle hGlyphMap) const;

        /// Retrieves the size of the given glyph map.
        ///
        /// @param hGlyphMap [in]  A handle to the glyph map whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        void GetGlyphMapSize(GlyphMapHandle hGlyphMap, unsigned int &widthOut, unsigned int &heightOut) const;

        
    protected:
        
        /// Constructor.
        FontEngine(GlyphMapManager* glyphMapManager);
        
        
        /// Retrieves a pointer to the glyph map manager.
        GlyphMapManager* GetGlyphMapManager() { return m_glyphMapManager; }
        
        
    private:
        
        /// A pointer to the glyph map manager.
        GlyphMapManager* m_glyphMapManager;
        
        
        
    private:    // No copying.
        FontEngine(const FontEngine &);
        FontEngine & operator=(const FontEngine &);
    };
}

#endif // __GTLib_FontEngine_hpp_
