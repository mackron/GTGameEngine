// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/GlyphMapLayout.hpp>

namespace GTLib
{
    GlyphMapLayout::GlyphMapLayout(unsigned int glyphMapWidth, unsigned int glyphMapHeight)
        : m_glyphMapWidth(glyphMapWidth), m_glyphMapHeight(glyphMapHeight), m_rows()
    {
        // Add an initial row.
        m_rows.PushBack(GlyphMapLayoutRow(0));
    }
    
    GlyphMapLayout::~GlyphMapLayout()
    {
    }
    
    
    bool GlyphMapLayout::FindAndInsert(unsigned int glyphWidth, unsigned int glyphHeight, unsigned int &xPosOut, unsigned int &yPosOut)
    {
        // We need to find an available row, starting from the bottom. We treat the bottom most row as a special case because we're going to be
        // testing against the bottom boundary.
        assert(m_rows.count > 0);
        {
            // Check the bottom row.
            auto &bottomRow = m_rows[m_rows.count - 1];
            if (CanFitInX(bottomRow, glyphWidth))
            {
                // We can fit the glyph into the bottom row on the horizontal axis, but we need to make sure it can fit on the
                // vertical axis.
                if ((m_glyphMapHeight - bottomRow.m_yPos) >= glyphHeight)
                {
                    // The slot can fit in the bottom row.
                    xPosOut = bottomRow.width;
                    yPosOut = bottomRow.m_yPos;
                    
                    // Expand the row.
                    bottomRow.width += glyphWidth;
                    
                    if (bottomRow.height < glyphHeight)
                    {
                        bottomRow.height = glyphHeight;
                    }
                    
                    
                    return true;
                }
            }
            

            // If we get here, it means we couldn't fit the glyph into the bottom row. We now want to check the other rows. It doesn't
            // really matter which direction we look, but we'll just go bottom up since we started at the bottom row earlier.
            for (size_t iRow = m_rows.count - 1; iRow > 0; --iRow)
            {
                auto &row = m_rows[iRow - 1];
                if (CanFitInX(row, glyphWidth) && CanFitInY(row, glyphHeight))
                {
                    // The slot can fit in this row.
                    xPosOut = row.width;
                    yPosOut = row.m_yPos;
                    
                    // Exand the row.
                    row.width += glyphWidth;
                    
                    return true;
                }
            }
        }
        
        return false;
    }
    
    
    
    
    /////////////////////////////////////////////
    // Statics Functions
    
    bool GlyphMapLayout::CanFitInX(const GlyphMapLayoutRow &row, unsigned int glyphWidth)
    {
        return row.width + glyphWidth <= m_glyphMapWidth;
    }
    
    bool GlyphMapLayout::CanFitInY(const GlyphMapLayoutRow &row, unsigned int glyphHeight)
    {
        return glyphHeight <= row.height;
    }
}
