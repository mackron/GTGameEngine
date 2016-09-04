// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GlyphMapManager
#define GT_GlyphMapManager

namespace GT
{
    typedef size_t GlyphMapHandle;
    
    /// Base class for managing glyph maps.
    ///
    /// Every FontServer will be assigned an instance of one of these managers. It's used so an application can
    /// choose how it wants to create, store and reference glyph maps.
    ///
    /// This type of system allows great flexibility for the host application. It means they are free to use
    /// whatever imaging library they want, at the expense of a little bit of added complexity.
    class GlyphMapManager
    {
    public:
    
        /// Constructor.
        GlyphMapManager();
        
        /// Destructor.
        virtual ~GlyphMapManager();
        
        
        /// Retrieves the maximum width of a glyph map.
        ///
        /// @remarks
        ///     The default value is 256.
        virtual unsigned int GetMaxWidth() const;
        
        /// Retrieves the maximum height of a glyph map.
        ///
        /// @remarks
        ///     The default value is 256.
        virtual unsigned int GetMaxHeight() const;
        
        
        //////////////////////////////////////////////////////////
        // Methods below must be implemented by child classes.
        
        /// Creates a new, empty image.
        ///
        /// @param width  [in] The width to make the new glyph map.
        /// @param height [in] The height to make the new glyph map.
        ///
        /// @remarks
        ///     The image must be in RGBA format.
        ///     @par
        ///     The initial data does not need to be defined.
        ///     @par
        ///     If an error occurs, 0 should be returned.
        virtual GlyphMapHandle CreateGlyphMap(unsigned int width, unsigned int height) = 0;
        
        /// Deletes a glyph created by CreateGlyphMap().
        ///
        /// @param glyphMap [in] The handle to the glyph map to delete.
        virtual void DeleteGlyphMap(GlyphMapHandle glyphMap) = 0;
        
        /// Sets the data of a rectangle in the glyph map.
        ///
        /// @param glyphMap [in] The handle to the glyph map whose data is being set.
        /// @param x        [in] The x position of the rectangle whose data is being set.
        /// @param y        [in] The y position of the rectangle whose data is being set.
        /// @param width    [in] The width of the rectangle whose data is being set.
        /// @param height   [in] The height of the rectangle whose data is being set.
        /// @param data     [in] The new image data for the specified rectangle.
        ///
        /// @remarks
        ///     Implementations of this method are free to mutate the provided data. This is mainly to allow implementations to
        ///     flip the image data (such as for use with OpenGL) without needing to do a full memory allocation.
        virtual void SetGlyphMapData(GlyphMapHandle glyphMap, int x, int y, unsigned int width, unsigned int height, void* data) = 0;
        
        /// Maps the glyph map image data.
        ///
        /// @param glyphMap [in] The handle to the glyph map whose data is being mapped.
        ///
        /// @return A pointer to the image data of the glyph map.
        ///
        /// @remarks
        ///     The image data should be unmapped with UnmapGlyphMapData().
        virtual void* MapGlyphMapData(GlyphMapHandle glyphMapHandle) = 0;

        /// Unmaps the glyph map image data.
        ///
        /// @param glyphMap [in] The handle to the glyph map whose data is being mapped.
        virtual void UnmapGlyphMapData(GlyphMapHandle glyphMapHandle) = 0;

        /// Retrieves the dimensions of the glyph map.
        ///
        /// @param glyphMap  [in]  The handle to the glyph map whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        virtual void GetGlyphMapDimensions(GlyphMapHandle glyphMap, unsigned int &widthOut, unsigned int &heightOut) const = 0;
    };
}

#endif // GT_GlyphMapManager





















