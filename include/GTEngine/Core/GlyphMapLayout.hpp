// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GlyphMapLayout
#define GT_GlyphMapLayout

#include <GTEngine/Core/Vector.hpp>

namespace GT
{
    /// Structure representing a row in the glyph map layout.
    struct GlyphMapLayoutRow
    {
        /// Constructor.
        GlyphMapLayoutRow(unsigned int yPos)
            : m_yPos(yPos), width(0), height(0)
        {
        }
        
        
        /// The y position of the row.
        unsigned int m_yPos;
        
        /// The current width of the row.
        unsigned int width;
        
        /// The current height of the row.
        unsigned int height;
    };
    
    
    /// Class for helping layout glyphs in a glyph map.
    ///
    /// This is an order dependant layout, meaning the actual layout will be determined based on the order glyphs
    /// are added to it. This will never shuffle existing glyph slots when a new slot is inserted.
    ///
    /// The layout is a simple row based system. When a glyph is added, it checks if it can fit in a row, starting
    /// from the bottom. When it finds one, it inserts it.
    class GlyphMapLayout
    {
    public:
        
        /// Constructor.
        GlyphMapLayout(unsigned int glyphMapWidth, unsigned int glyphMapHeight);
        
        /// Destructor.
        ~GlyphMapLayout();
        
        
        /// Finds an available slot for a glyph of the given dimensions, and inserts it if possible.
        ///
        /// @param glyphWidth  [in] The width of the slot needing to be inserted.
        /// @param glyphHeight [in] The height of the slot needing to be inserted.
        /// @param xPosOut     [out] A reference to the variable that will receive the x position of the inserted slot.
        /// @param yPosOut     [out] A reference to the variable that will receive the y position of the inserted slot.
        ///
        /// @return True if a place was found to place the slot, false otherwise.
        ///
        /// @remarks
        ///     When 'false' is returned, the contents of 'xPosOut' and 'yPosOut' are undefined.
        bool FindAndInsert(unsigned int glyphWidth, unsigned int glyphHeight, unsigned int &xPosOut, unsigned int &yPosOut);
    
    
    
    private:
        
        /////////////////////////////////////////////
        // Statics Functions
        
        /// Determines whether or not the given row can fit a slot on the horizontal axis.
        ///
        /// @param row        [in] The row.
        /// @param glyphWidth [in] The width of the slot.
        bool CanFitInX(const GlyphMapLayoutRow &row, unsigned int glyphWidth);
        
        /// Determines whether or not the given row can fit a slot on the vertical axis.
        ///
        /// @param row         [in] The row.
        /// @param glyphHeight [in] The height of the slot.
        bool CanFitInY(const GlyphMapLayoutRow &row, unsigned int glyphHeight);
    
    
    private:
        
        /// The total width of the glyph map.
        unsigned int m_glyphMapWidth;
        
        /// The total height of the glyph map.
        unsigned int m_glyphMapHeight;
        
        /// The list of rows in the glyph map layout.
        Vector<GlyphMapLayoutRow> m_rows;
    };
}

#endif // GT_GlyphMapLayout
