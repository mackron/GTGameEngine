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


        /// @copydoc GUITextLayout::SetContainerBounds()
        void SetContainerBounds(unsigned int width, unsigned int height);

        /// @copydoc GUITextLayout::GetContainerBounds()
        void GetContainerBounds(unsigned int &widthOut, unsigned int &heightOut) const;


        /// @copydoc GUITextLayout::SetContainerInnerOffset()
        void SetContainerInnerOffset(int offsetX, int offsetY);

        /// @copydoc GUITextLayout::GetContainerInnerOffset()
        void GetContainerInnerOffset(int &offsetXOut, int &offsetYOut) const;


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


        /// @copydoc GUITextLayout::GetTextRectRelativeToBounds()
        void GetTextRectRelativeToBounds(GTLib::Rect<int> &rectOut) const;


        /// @copydoc GUITextLayout::MoveCursorToPoint()
        void MoveCursorToPoint(int inputPosX, int inputPosY);

        /// @copydoc GUITextLayout::GetCursorPositionRelativeToContainer()
        void GetCursorPosition(int &posXOut, int &posYOut) const;

        /// @copydoc GUITextLayout::MoveCursorLeft()
        bool MoveCursorLeft();

        /// @copydoc GUITextLayout::MoveCursorRight()
        bool MoveCursorRight();


        //////////////////////////////
        // Editing

        /// @copydoc GUITextLayout::InsertCharacterAtCursor()
        void InsertCharacterAtCursor(char32_t character);

        /// @copydoc GUITextLayout::DeleteCharacterToLeftOfCursor()
        void DeleteCharacterToLeftOfCursor();

        /// @copydoc GUITextLayout::DeleteCharacterToRightOfCursor()
        void DeleteCharacterToRightOfCursor();

        /// @copydoc GUITextLayout::InsertNewLineAtCursor()
        void InsertNewLineAtCursor();

        /// @copydoc GUITextLayout::InsertTabAtCursor()
        void InsertTabAtCursor();


    private:

        /// Structure representing a text run.
        ///
        /// A text run is just a group of 
        struct TextRun2
        {
            /// Index of the line the run is placed on. For runs that are new line characters, this will represent the number of lines that came before it. For
            /// example, if this run represents the new-line character for the first line, this will be 0 and so on.
            unsigned int iLine;

            /// Index in the main text string of the first character of the run.
            unsigned int iChar;

            /// Index in the main text string of the character just past the last character in the run.
            unsigned int iCharEnd;

            /// The number of characters making up the run. Note that this is not the number of bytes. It is possible for runs to have this set to zero, which
            /// will be the case for runs that contain only tabs and new-line characters. This is used to indicate that these runs are dividers.
            unsigned int characterCount;


            /// Position of the run on the x axis relative to the left side of the text boundary.
            int posX;

            /// Position of the run on the y axis relative to the top side of the text boundary.
            int posY;

            /// The width of the run. This will be 0 for new-line runs.
            int width;

            /// The height of the run.
            int height;
        };


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

        /// Structure representing a position within the text.
        struct TextMarker
        {
            TextMarker()
                : iLine(0), iRun(0), iChar(0), relativePosX(0)
            {
            }

            /// The index of the line the marker is positioned on.
            unsigned int iLine;

            /// The index of the run within the line the marker is positioned on.
            unsigned int iRun;

            /// The index of the character within the run the marker is positioned to the left of.
            unsigned int iChar;

            /// The position on the x axis, relative to the x position of the run.
            int relativePosX;
        };


    private:

        /// Finds the next run string.
        static bool NextRunString(const char* runStart, const char* &runEnd);


        /// Refreshes the layout.
        void RefreshLayout();

        /// Refreshes the alignment of the text.
        void RefreshAlignment();

        /// Calculates the offsets to apply to a line based on it's width and the current text bounds.
        void CalculateLineAlignmentOffset(int lineWidth, int &offsetXOut, int &offsetYOut) const;


        /// Converts a point relative to the text bounds to the container bounds.
        void MakeRelativeToContainer(int inputPosX, int inputPosY, int &posXOut, int &posYOut) const;

        /// Converts a point relative to the container bounds to the text bounds.
        void MakeRelativeToTextBounds(int inputPosX, int inputPosY, int &posXOut, int &posYOut) const;


        /// Creates a marker from the given input position, relative to the container bounds.
        bool InitMarkerByPointRelativeToContainer(int inputPosX, int inputPosY, TextMarker &markerOut) const;

        /// Retrieves the position of the given marker relative to the text boundary.
        void GetMarkerPositionRelativeToContainer(const TextMarker &marker, int &posXOut, int &posYOut) const;


        /// Moves the given marker to the left by one character.
        bool MoveMarkerLeft(TextMarker &marker);

        /// Moves the given marker to the right by one character.
        bool MoveMarkerRight(TextMarker &marker);

        /// Moves the given marker to the end of the previous run.
        bool MoveMarkerToEndOfPreviousRun(TextMarker &marker);

        /// Moves the given marker to the start of the next run.
        bool MoveMarkerToStartOfNextRun(TextMarker &marker);

        /// Moves the given marker to the end of the previous line.
        bool MoveMarkerToEndOfPreviousLine(TextMarker &marker);

        /// Moves the given marker to the start of the next line.
        bool MoveMarkerToStartOfNextLine(TextMarker &marker);


        /// Deletes the character to the right of the given marker.
        ///
        /// @remarks
        ///     This does not move the marker.
        bool DeleteCharacterToRightOfMarker(TextMarker &marker);



    private:

        /// The text.
        GTLib::String m_text;


        /// The width of the text boundary.
        unsigned int m_containerBoundsWidth;

        /// The height of the text boundary.
        unsigned int m_containerBoundsHeight;


        /// The offset of the text on the x axis.
        int m_containerInnerOffsetX;

        /// The offset of the text on the y axis.
        int m_containerInnerOffsetY;


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


        /// The list of runs making up the layout.
        GTLib::Vector<TextRun2> m_runs;

        /// The list of text runs making up the layout.
        GTLib::Vector<TextLine> m_lines;


        /// The width of the text.
        int m_textBoundsWidth;

        /// The height of the text.
        int m_textBoundsHeight;


        /// The marker representing the text cursor.
        TextMarker m_cursor;
    };
}

#endif // !__GT_GUISimpleTextLayout_hpp_
