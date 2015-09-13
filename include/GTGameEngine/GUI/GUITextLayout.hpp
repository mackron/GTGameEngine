// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUITextLayout
#define GT_GUITextLayout

#include "GUIFontManager.hpp"
#include "../../GTLib/Colour.hpp"

namespace GT
{
    enum class GUITextLayoutHorizontalAlignment
    {
        Left,
        Center,
        Right
    };

    enum class GUITextLayoutVerticalAlignment
    {
        Top,
        Center,
        Bottom
    };


    /// Structure containing information about a text run in a text layout.
    struct GUITextRunDesc
    {
        /// The text.
        char text[GT_MAX_TEXT_RUN_SIZE_IN_BYTES];

        /// A handle to the font the text should be drawn with.
        HGUIFont hFont;

        /// The position of the text run on the x axis.
        int xPos;

        /// The position of the text run on the y axis.
        int yPos;

        /// The rotation of the text run in tenths of degrees.
        int rotationInDegrees;

        /// The color of the text.
        GTLib::Colour color;
    };

    /// Base class for laying out text.
    ///
    /// This class is used to layout the text of an element. It has been built in such a way that it doesn't actually depend
    /// on a GUI context, elements or surfaces. Thus, it can be used in places outside of the normal GUI stuff.
    class GUITextLayout
    {
    protected:

        /// Constructor.
        GUITextLayout(GUIFontManager &fontManager);


    public:

        /// Destructor.
        virtual ~GUITextLayout();


        /// Retrieves a reference to the font manager.
        ///
        /// @return A reference to the font manager.
              GUIFontManager & GetFontManager();
        const GUIFontManager & GetFontManager() const;


        /// Sets the text of the text manager.
        ///
        /// @param text [in] The text.
        virtual void SetText(const char* text) = 0;

        /// Retrieves a pointer to the text.
        ///
        /// @return A null-terminated string containing the text of the text layout.
        virtual const char* GetText() const = 0;


        /// Sets the size of the bounding rectangle of the container that contains text.
        ///
        /// @param width  [in] The width of the bounding rectangle.
        /// @param height [in] The height of the bounding rectangle.
        virtual void SetContainerBounds(unsigned int width, unsigned int height) = 0;

        /// Retrieves the bounding rectangle of the container that contains the text.
        ///
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        virtual void GetContainerBounds(unsigned int &widthOut, unsigned int &heightOut) const = 0;


        /// Sets the offset of the text.
        ///
        /// @param offsetX [in] The offset on the x axis.
        /// @param offsetY [in] The offset on the y axis.
        virtual void SetContainerInnerOffset(int offsetX, int offsetY) = 0;

        /// Retrieves the offset of the text.
        ///
        /// @param offsetXOut [out] A reference to the variable that will receive the offset on the x axis.
        /// @param offsetYOut [out] A reference to the variable that will receive the offset on the y axis.
        virtual void GetContainerInnerOffset(int &offsetXOut, int &offsetYOut) const = 0;


        /// Iterates over every text run contained withing the current bounds.
        ///
        /// @param func [in] The function to call for each text run.
        virtual void IterateVisibleTextRuns(std::function<void (const GUITextRunDesc &textRunDesc)> func) const = 0;


        /// Sets the size of a tab in spaces.
        ///
        /// @param tabSizeInSpaces [in] The size of a tab in spaces.
        virtual void SetTabSizeInSpaces(int tabSizeInSpaces) = 0;

        /// Retrieves the size of a tab in spaces.
        ///
        /// @return The size of a tab character in spaces.
        virtual int GetTabSizeInSpaces() const = 0;


        /// Retrieves the width of the text.
        ///
        /// @return The width of the text.
        ///
        /// @remarks
        ///     This is not the same as the main boundary that's retrieved with GetBounds(), but rather the width of the text itself.
        virtual int GetTextWidth() const = 0;

        /// Retrieves the height of the text.
        ///
        /// @return The height of the text.
        ///
        /// @remarks
        ///     This is not the same as the main boundary that's retrieved with GetBounds(), but rather the height of the text itself.
        virtual int GetTextHeight() const = 0;


        /// Sets the horizontal alignment.
        ///
        /// @param alignment [in] The new horizontal alignment.
        virtual void SetHorizontalAlignment(GUITextLayoutHorizontalAlignment alignment) = 0;

        /// Retrieves the horizontal alignment.
        ///
        /// @return The horizontal alignment.
        virtual GUITextLayoutHorizontalAlignment GetHorizontalAlignment() const = 0;

        /// Sets the vertical alignment.
        ///
        /// @param alignment [in] The new vertical alignment.
        virtual void SetVerticalAlignment(GUITextLayoutVerticalAlignment alignment) = 0;

        /// Retrieves the vertical alignment.
        ///
        /// @return The vertical alignment.
        virtual GUITextLayoutVerticalAlignment GetVerticalAlignment() const = 0;

        /// Optimized method for setting both vertical and horizontal alignment at once.
        ///
        /// @param horizontalAlignment [in] The horizontal alignment.
        /// @param verticalAlignment   [in] The vertical alignment.
        virtual void SetAlignment(GUITextLayoutHorizontalAlignment horizontalAlignment, GUITextLayoutVerticalAlignment verticalAlignment) = 0;


        /// Sets the default font to use with the text layout.
        ///
        /// @param hFont [in] A handle to the default font.
        virtual void SetDefaultFont(HGUIFont hFont) = 0;

        /// Retrieves a handle to the default font.
        ///
        /// @return A handle to the default font.
        virtual HGUIFont GetDefaultFont() const = 0;


        /// Sets the default colour for the text.
        ///
        /// @param color [in] The default color of the text.
        virtual void SetDefaultTextColor(const GTLib::Colour &color) = 0;

        /// Retrieves the default colour of the text.
        ///
        /// @return The default colour of the text.
        virtual GTLib::Colour GetDefaultTextColor() const = 0;


        /// Retrieves the text rectangle relative to the bounds.
        virtual void GetTextRectRelativeToBounds(GTLib::Rect<int> &rectOut) const = 0;


        /// Positions the cursor at the closest character based on the given input coordinates, relative to the local bounds.
        virtual void MoveCursorToPoint(int inputPosX, int inputPosY) = 0;

        /// Retrieves the position of the cursor relative to the container bounds.
        virtual void GetCursorPosition(int &posXOut, int &posYOut) const = 0;

        /// Moves the cursor one character to the left.
        virtual bool MoveCursorLeft() = 0;

        /// Moves the cursor one character to the right.
        virtual bool MoveCursorRight() = 0;

        /// Moves the cursor up one line.
        virtual bool MoveCursorUp() = 0;

        /// Moves the cursor down one line.
        virtual bool MoveCursorDown() = 0;

        /// Moves the cursor to the end of the line.
        virtual bool MoveCursorToEndOfLine() = 0;

        /// Moves the cursor to the start of the line.
        virtual bool MoveCursorToStartOfLine() = 0;


        //////////////////////////////
        // Editing
        //
        // The editing methods below will move the cursor where appropriate.

        /// Inserts a character at the position of the cursor.
        ///
        /// @param character [in] The character to insert.
        ///
        /// @remarks
        ///     This should be used for backspace, delete and enter characters.
        virtual void InsertCharacterAtCursor(char32_t character) = 0;

        /// Deletes the character to the left of the cursor, and moves the cursor in response.
        ///
        /// @remarks
        ///     You would typically call this for backspace key presses.
        virtual void DeleteCharacterToLeftOfCursor() = 0;

        /// Deletes the character to the right of the cursor.
        ///
        /// @remarks
        ///     This will not move the cursor.
        virtual void DeleteCharacterToRightOfCursor() = 0;


    private:

        /// A reference to the font manager for working with the font.
        GUIFontManager &m_fontManager;



    private:    // No copying.
        GUITextLayout(const GUITextLayout &);
        GUITextLayout & operator=(const GUITextLayout &);
    };
}

#endif // !__GT_GUITextLayout_hpp_
