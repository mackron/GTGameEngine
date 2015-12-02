// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTTYpe_GlyphMetrics
#define __GTTYpe_GlyphMetrics

namespace GT
{
    /// Structure containing the metrics of a glyph, not including the width and height.
    ///
    /// All metrics are defined in pixels.
    struct GlyphMetrics
    {
        GlyphMetrics()
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
}

#endif // __GTTYpe_GlyphMetrics
