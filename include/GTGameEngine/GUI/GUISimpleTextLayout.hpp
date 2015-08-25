// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUISimpleTextLayout
#define GT_GUISimpleTextLayout

#include "GUITextLayout.hpp"

namespace GT
{
    /// Implementation of the text layout class for simple layouts with a single font and color for the whole text.
    class GUISimpleTextLayout : public GUITextLayout
    {
    public:

        /// Constructor.
        GUISimpleTextLayout(GUIFontManager &fontManager);

        /// Destructor.
        ~GUISimpleTextLayout();


        //////////////////////////////////////////
        // Virtual Implementations

        /// @copydoc GUITextManager::SetText()
        void SetText(const char* text);

        /// @copydoc GUITextManager::GetText()
        const char* GetText() const;


        /// @copydoc GUITextLayout::SetBounds()
        void SetBounds(unsigned int width, unsigned int height);

        /// @copydoc GUITextLayout::GetBounds()
        void GetBounds(unsigned int &widthOut, unsigned int &heightOut) const;


        /// @copydoc GUITextLayout::SetOffset()
        void SetOffset(int offsetX, int offsetY);

        /// @copydoc GUITextLayout::GetOffset()
        void GetOffset(int &offsetXOut, int &offsetYOut) const;


        /// @copydoc GUITextLayout::IterateVisibleTextRuns()
        void IterateVisibleTextRuns(std::function<void (const GUITextRunDesc &textRunDesc)> func) const;


        /// @copydoc GUITextLayout::SetTabSizeInSpaces()
        void SetTabSizeInSpaces(int tabSizeInSpaces);

        /// @copydoc GUITextLayout::GetTabSizeInSpaces()
        int GetTabSizeInSpaces() const;


        /// @copydoc GUITextLayout::GetTextWidth()
        int GetTextWidth() const;

        /// @copydoc GUITextLayout::GetTextHeight()
        int GetTextHeight() const;


        /// @copydoc GUITextLayout::SetHorizontalAlignment()
        void SetHorizontalAlignment(GUITextLayoutHorizontalAlignment alignment);

        /// @copydoc GUITextLayout::GetVerticalAlignment()
        GUITextLayoutHorizontalAlignment GetHorizontalAlignment() const;

        /// @copydoc GUITextLayout::SetVerticalAlignment()
        void SetVerticalAlignment(GUITextLayoutVerticalAlignment alignment);

        /// @copydoc GUITextLayout::GetVerticalAlignment()
        GUITextLayoutVerticalAlignment GetVerticalAlignment() const;

        /// @copydoc GUITextLayout::SetAlignment()
        void SetAlignment(GUITextLayoutHorizontalAlignment horizontalAlignment, GUITextLayoutVerticalAlignment verticalAlignment);


        /// @copydoc GUITextLayout::SetDefaultFont()
        void SetDefaultFont(HGUIFont hFont);

        /// @copydoc GUITextLayout::GetDefaultFont()
        HGUIFont GetDefaultFont() const;


        /// @copydoc GUITextLayout::SetDefaultTextColor()
        void SetDefaultTextColor(const GTLib::Colour &color);

        /// @copydoc GUITextLayout::GetDefaultTextColor()
        GTLib::Colour GetDefaultTextColor() const;



    private:

        /// Structure containing information about a text run.
        struct TextRun
        {
            /// A pointer to the start of the text.
            const char* textStart;

            /// A pointer to the end of the text.
            const char* textEnd;

            /// The number of characters making up the text run.
            unsigned int characterCount;

            /// The position of the run on the x axis, relative to the left side of the line.
            int xPos;

            /// The position of the run on the y axis, relative to the top side of the line.
            int yPos;

            /// The width of the text run.
            unsigned int width;
        };

        /// Structure containing information about a text line.
        struct TextLine
        {
            /// The list of text runs making up the line.
            GTLib::Vector<TextRun> runs;

            /// The width of the line.
            int width;

            /// The height of the line.
            int height;

            /// The offset on the x axis to apply to the line to account for horizontal alignment.
            int alignmentOffsetX;

            /// The offset on the y axis to apply to the line to account for vertical alignment.
            int alignmentOffsetY;
        };


    private:

        /// Refreshes the layout.
        void RefreshLayout();

        /// Refreshes the alignment of the text.
        void RefreshAlignment();



    private:

        /// The text.
        GTLib::String m_text;


        /// The width of the text boundary.
        unsigned int m_boundsWidth;

        /// The height of the text boundary.
        unsigned int m_boundsHeight;


        /// The offset of the text on the x axis.
        int m_offsetX;

        /// The offset of the text on the y axis.
        int m_offsetY;


        /// The size of a tab character in spaces.
        int m_tabSizeInSpaces;


        /// The horizontal alignment.
        GUITextLayoutHorizontalAlignment m_horizontalAlignment;

        /// The vertical alignment.
        GUITextLayoutVerticalAlignment m_verticalAlignment;



        /// The font.
        HGUIFont m_hFont;

        /// The colour of the text.
        GTLib::Colour m_color;


        /// The list of text runs making up the layout.
        GTLib::Vector<TextLine> m_lines;


        /// The width of the text.
        int m_textBoundsWidth;

        /// The height of the text.
        int m_textBoundsHeight;
    };
}

#endif // !__GT_GUISimpleTextLayout_hpp_
