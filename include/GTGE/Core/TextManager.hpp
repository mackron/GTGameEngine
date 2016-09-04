// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTType_TextManager
#define __GTType_TextManager

namespace GT
{
    enum Alignment
    {
        Alignment_Left,
        Alignment_Right,
        Alignment_Top,
        Alignment_Bottom,
        Alignment_Center,
    };

    /// An event handler for the text manager.
    class TextManagerEventHandler
    {
    public:

        /// Destructor.
        virtual ~TextManagerEventHandler() {};


    public:

        /// Called when the text changes.
        virtual void OnTextChanged(const char*) {}
    };

    struct TextManagerRenderingOptions
    {
        TextManagerRenderingOptions()
            : yAtTop(true), alpha(1.0f)
        {
        }

        /// Whether or not the origin of the y axis should be at the top. True by default.
        bool yAtTop;

        /// The alpha colour.
        float alpha;
    };


    /// Structure containing information that can be used for drawing text.
    struct TextManagerRenderingInfo
    {
        TextManagerRenderingInfo()
            : meshes()
        {
        }

        ~TextManagerRenderingInfo()
        {
            this->Clear();
        }

        void Clear()
        {
            while (this->meshes.root != nullptr)
            {
                delete this->meshes.root->value;
                this->meshes.RemoveRoot();
            }
        }


        /// The list of meshes making up the visible text.
        List<TextMesh*> meshes;
    };



    /// Enumerator for the different command types.
    enum TextCommandType
    {
        TextCommandType_Insert,
        TextCommandType_Delete,
    };


    /// Structure representing a command made by the text manager. We use this for implementing undo/redo.
    struct TextCommand
    {
        TextCommand()
            : type(), text(), lineStart(), lineStartCharacter(), lineEnd(), lineEndCharacter(), placeCursorAtStart()
        {
        }


        /// The command type.
        TextCommandType type;

        /// The text.
        String text;

        /// The location of the first character in the text at the time of deleting.
        int lineStart;
        int lineStartCharacter;

        int lineEnd;
        int lineEndCharacter;


        /// Controls whether or not the cursor should be placed at the beginning of the text when it is being re-inserted from an undo or redo.
        bool placeCursorAtStart;
    };
}

namespace GT
{
    /// Class representing a line of text in the text manager.
    ///
    /// A line is simply made up of individual sections. Each section has it's own font. Using a section mechanism is how
    /// we can support lines with different fonts.
    class TextManagerLine
    {
    public:

        /// Constructor.
        TextManagerLine(const Font* defaultFont, unsigned int tabSizeInPixels, const char* text, ptrdiff_t textSizeInTs = -1);

        /// Destructor.
        ~TextManagerLine();


        /// Sets the default font.
        void SetDefaultFont(const Font* defaultFont);

        /// Sets the tab size, in pixels.
        void SetTabSizeInPixels(unsigned int newTabSize);


        /// Retrieves the width in pixels of this line.
        unsigned int GetWidth() const { return this->width; }

        /// Retrieves the height in pixels of this line.
        unsigned int GetHeight() const { return this->height; }


        /// Retrieves the ascent in pixels of this line.
        int CalculateAscent() const;

        /// Retrieves the descent in pixels of this line.
        int CalculateDescent() const;


        /// Sets the text of the line.
        void SetText(const char* text, ptrdiff_t textSizeInTs = -1);

        /// Retrieves the text.
        const char* GetText() const;


        /// Retrieves the number of characters in the line.
        size_t GetCharacterCount() const;

        /// Determines whether or not the line is empty.
        ///
        /// @remarks
        ///     This is equivalent, but more efficient, to GetCharacterCount() == 0.
        bool IsEmpty() const;

        /// Determines whether or not the line is not empty.
        ///
        /// @remarks
        ///     This is the opposite of IsEmpty().
        bool IsNotEmpty() const;



        /// Measures a part of this string, returning positional information and the width.
        ///
        /// @param startCharIndex [in]  The index of the character to start at.
        /// @param endCharIndex   [in]  The index of the character to end at. See remarks.
        /// @param startPosition  [out] A reference to the variable that will receive the pixel position of the start.
        /// @param endPosition    [out] A reference to the variable taht will receive the pixel position of the end.
        ///
        /// @return The width.
        ///
        /// @remarks
        ///     endCharIndex should be 1 past the last character to include. As an example, setting this to this->GetCharacterCount() will include
        ///     the very last character in the line.
        ///
        ///     Setting startCharIndex and endCharIndex to the same value will return 0.
        unsigned int Measure(size_t startCharIndex, size_t endCharIndex, int &startPosition, int &endPosition) const;
        unsigned int Measure(size_t startCharIndex, size_t endCharIndex) const;
        unsigned int Measure() const;


        /// Deletes the characters in the given range.
        ///
        /// @param startCharIndex [in]  The index of the character to start at.
        /// @param endCharIndex   [in]  The index of the character to end at. See remarks.
        ///
        /// @remarks
        ///     endCharIndex should be 1 past the last character to include. As an example, setting this to this->GetCharacterCount() will include
        ///     the very last character in the line.
        void DeleteCharacters(size_t startCharIndex, size_t endCharIndex);


        /// Retrieves the characters in the given range as a string.
        ///
        /// @param startCharIndex [in]  The index of the character to start at.
        /// @param endCharIndex   [in]  The index of the character to end at. See remarks.
        ///
        /// @remarks
        ///     endCharIndex should be 1 past the last character to include. As an example, setting this to this->GetCharacterCount() will include
        ///     the very last character in the line.
        String GetTextInRange(size_t startCharIndex, size_t endCharIndex);

        /// Retrieves the characters in the given range as a string.
        ///
        /// @param startCharIndex [in]  The index of the character to start at.
        /// @param endCharIndex   [in]  The index of the character to end at. See remarks.
        /// @param rangeStart     [out] A reference to the variable that will receive a pointer to the beginning of the sub string.
        /// @param rangeEnd       [out] A reference to the variable that will receive a pointer to the end of the sub string.
        ///
        /// @remarks
        ///     endCharIndex should be 1 past the last character to include. As an example, setting this to this->GetCharacterCount() will include
        ///     the very last character in the line.
        void GetTextInRange(size_t startCharIndex, size_t endCharIndex, const char* &rangeStart, const char* &rangeOut);



    private:

        /// Recalculates the bounds of the line.
        void RecalculateBounds();



    private:

        /// The default font. This will be removed when we add support for multiple fonts in the same line.
        const Font* defaultFont;


        /// The line's text.
        String text;

        /// The width of the line, in pixels. This is recalculated whenever the text has changed.
        int width;

        /// The height of the line, in pixels. This is recalculated whenever the text has changed.
        int height;

        /// The size of a tab in pixels.
        unsigned int tabSizeInPixels;



    private:    // No copying.
        TextManagerLine(const TextManagerLine &);
        TextManagerLine & operator=(const TextManagerLine &);


    friend class TextManager;
    };
}

namespace GT
{
    /// Manages a group of text, making it easier for an application to work on text.
    ///
    /// This class supports functionality required for text editors: text cursor (caret), selection, drawing information, etc.
    ///
    /// The text manager always works on individual lines. You can consider text to be made up of n lines.
    class TextManager
    {
    public:

        /// A structure containing information about a position in the text manager.
        struct Marker
        {
            Marker()
                : posX(0), posY(0), line(nullptr), lineIndex(0), characterIndex(0)
            {
            }

            Marker(const Marker &other)
                : posX(other.posX), posY(other.posY), line(other.line), lineIndex(other.lineIndex), characterIndex(other.characterIndex)
            {
            }


            Marker & operator=(const Marker &other)
            {
                this->posX           = other.posX;
                this->posY           = other.posY;
                this->line           = other.line;
                this->lineIndex      = other.lineIndex;
                this->characterIndex = other.characterIndex;

                return *this;
            }

            bool operator==(const Marker &other) const
            {
                return this->lineIndex == other.lineIndex && this->characterIndex == other.characterIndex;
            }

            bool operator!=(const Marker &other) const
            {
                return !(*this == other);
            }


            /// The x position of the marker, relative to the left of the text area.
            unsigned int posX;

            /// The y position of the marker, relative to the top of the text area.
            unsigned int posY;


            /// The line the marker is positioned on.
            TextManagerLine* line;

            /// The index of the line.
            size_t lineIndex;

            /// The index of the character in 'section' that the marker is sitting to the left of. If this is 0, it is at the very start of the section.
            size_t characterIndex;
        };



    public:

        /// Constructor.
        TextManager(Font* defaultFont);

        /// Destructor.
        ~TextManager();

        /// Sets the text that should be manager.
        ///
        /// @remarks
        ///     This will clear the undo stack!
        ///     @par
        ///     This will delete the previous text, including rendering information.
        void SetText(const char* text, bool blockEvent = false);

        /// Retrieves a null-terminated pointer to the text this manager is managing.
        ///
        /// This is not a simple accessor. You may need to
        const char* GetText() const;

        /// Determines whether or not the text manager has content.
        bool HasText() const;


        /// Sets the default font to use with the manager.
        void SetDefaultFont(Font* defaultFont);

        /// Retrieves the default font.
        Font* GetDefaultFont();


        /// Sets the size of the area the contains the text.
        ///
        /// @param  width  [in] The width of the container.
        /// @param  height [in] The height of the container.
        ///
        /// If either 'width' or 'height' is 0, it will be as if the container stretches with the text for that particular dimension.
        void SetContainerSize(unsigned int width, unsigned int height);

        /// Retrieves the size of the container.
        ///
        /// @param  width  [out] The width of the container.
        /// @param  height [out] The height of the container.
        void GetContainerSize(unsigned int &width, unsigned int &height) const;


        /// Sets the offset of the area containing the text.
        ///
        /// @param  offsetX [in] The x offset of the container.
        /// @param  offsetY [in] The y offset of the container.
        void SetContainerOffset(int offsetX, int offsetY);


        /// Sets the horizontal alignment. Must be Alignment_Left, Alignment_Center or Alignment_Right.
        void SetHorizontalAlign(Alignment align);

        /// Sets the vertical alignment. Must be Alignment_Top, Alignment_Center or Alignment_Bottom.
        void SetVerticalAlign(Alignment align);


        /// Retrieves the rectangle area containing the text, relative to the container area.
        void GetTextRect(Rect<int> &rect) const;

        /// Retrieves the total width of the text.
        unsigned int GetTextWidth() const;

        /// Retrieves the total height of the text.
        unsigned int GetTextHeight() const;


        /// Calculates the rectangle of the given line relative to the container.
        ///
        /// @param line [in] The line whose rectangle is being retrieved.
        /// @param rect [in] A reference to the Rect object that will receive the rectangle.
        void GetLineRect(const TextManagerLine *line, Rect<int> &rect) const;


        /// Retrieves the line that comes before the given line.
        TextManagerLine* GetPreviousLine(TextManagerLine* line) const;

        /// Retrieves the line that comes after the given line.
        TextManagerLine* GetNextLine(TextManagerLine* line) const;


        /// Sets the event handler.
        ///
        /// @param eventHandler [in] The event handler to use with the text manager.
        void SetEventHandler(TextManagerEventHandler* eventHandler) { m_eventHandler = eventHandler; }
        void SetEventHandler(TextManagerEventHandler &eventHandler) { this->SetEventHandler(&eventHandler); }


        /// Sets the default colour for text.
        void SetDefaultTextColour(float r, float g, float b);

        /// Sets the selection background colour.
        void SetSelectionBackgroundColour(float r, float g, float b);


        /// Retrieves the text in the given range.
        String GetTextInRange(const Marker &markerA, const Marker &markerB) const;


        /// Retrieves the total number of lines.
        unsigned int GetLineCount() const;

        /// Retrieves the number of lines that fit on a page.
        ///
        /// @remarks
        ///     This is based on the size of the default font.
        unsigned int GetPageLineCount() const;


        /// Retrieves the start and end indices of the visible lines.
        ///
        /// @param firstIndex [out] A reference to the variable that will receive the index of the first visible line.
        /// @param lastIndex  [out] A reference to the variable that will receive the index of the last visible line.
        void GetVisibleLineRange(size_t &firstIndex, size_t &lastIndex) const;


        /// Performs an undo operation.
        void Undo();

        /// Performs a redo operation.
        void Redo();


        /// Retrieves the tab size, in spaces.
        unsigned int GetTabSizeInSpaces() const;

        /// Sets the size of a tab, in spaces.
        void SetTabSizeInSpaces(unsigned int newTabSize);

        /// Retrieves the size of a tab in pixels.
        unsigned int GetTabSizeInPixels() const;





        //////////////////////////////////////////////////////////////
        // Marker

        /// Moves the given marker based on the input position.
        ///
        /// @param x      [in]  The x position to use as the basis for the marker's actual position.
        /// @param y      [in]  The y position to use as the basis for the marker's actual position.
        /// @param marker [out] The marker that will receive the positioning information.
        ///
        /// This method will round the input position to the closest character. This is super useful for positioning the caret and selection boxes.
        void MoveMarker(int x, int y, Marker &marker) const;

        /// Moves the given marker to the start of the text.
        void MoveMarkerToStart(Marker &marker) const;

        /// Moves the given marker to the end of the text.
        void MoveMarkerToEnd(Marker &marker) const;

        /// Moves the given marker one character to the left.
        void MoveMarkerLeft(Marker &marker) const;

        /// Moves the given marker one character ot the right.
        void MoveMarkerRight(Marker &marker) const;

        /// Moves the given marker one line up.
        void MoveMarkerUp(Marker &marker) const;

        /// Moves the given marker one line down.
        void MoveMarkerDown(Marker &marker) const;

        /// Determines whether or not a marker comes before another.
        ///
        /// @param markerA [in] A reference to the first marker.
        /// @param markerB [in] A reference to the second marker.
        ///
        /// @return True if markerA comes before markerB.
        bool IsMarkerBefore(const Marker &markerA, const Marker &markerB) const;




        //////////////////////////////////////////////////////////////
        // Cursor

        /// Moves the text cursor to the closest character based on a position relative to the top-left of the container area.
        /// @param  x [in] The x position to use as the basis for the new cursor position.
        /// @param  y [in] The y position to use as the basis for the new cursor position.
        ///
        /// @remarks
        ///     The input coordinates 'x' and 'y' are used to find the closest character to position the cursor at. Use GetCursorPosition() to
        ///     retrieve the new position of the cursor for drawing or whatnot.
        ///     \par
        ///     The input coordinates should be relative to the top-left of the container area.
        void MoveCursor(int x, int y);
        void MoveCursorToLine(int lineIndex, int characterIndex);

        /// Moves the cursor to the start of the text.
        void MoveCursorToStart();

        /// Moves the cursor to the end of the text.
        void MoveCursorToEnd();

        /// Moves the cursor one character to the left.
        void MoveCursorLeft();

        /// Moves the cursor one character to the right.
        void MoveCursorRight();

        /// Moves the cursor one line up.
        void MoveCursorUp();

        /// Moves the cursor one line down.
        void MoveCursorDown();

        /// Moves the cursor to the selection marker that comes first in the string.
        void MoveCursorToFirstSelectionMarker();

        /// Moves the cursor to the selection marker that comes afterwards in the string.
        void MoveCursorToLastSelectionMarker();

        /// Moves the cursor to the start of the line it's currently sitting on.
        void MoveCursorToStartOfLine();

        /// Moves the cursor to the end of the line it's currently sitting on.
        void MoveCursorToEndOfLine();


        /// Retrieves the actual position of the cursor in pixels, relative to the top left of the container area.
        ///
        /// @param  x [out] A reference to the variable that will receive the x position of the cursor.
        /// @param  y [out] A reference to the variable that will receive the y position of the cursor.
        ///
        /// The y position will be positioned at the top of the applicable line.
        void GetCursorPosition(int &x, int &y) const;

        /// Retrieves a pointer to the line the cursor is currently sitting on.
        const TextManagerLine* GetCursorLine() const;

        /// Retrieves the index of the line that the text cursor is sitting on.
        size_t GetCursorLineIndex() const;


        //////////////////////////////////////////////////////////////
        // Editting

        /// Appends a new line to the very end of the text.
        void AppendNewLine(bool blockEvent = false);

        /// Inserts a character at the position of the cursor.
        ///
        /// This will move the cursor forward.
        void InsertCharacterAtCursor(char32_t c);

        /// Inserts a string at the position of the cursor, and the moves the cursor to the end of the newly inserted string.
        ///
        /// @param appendNewCommand [in] Used internally. Controls whether or not an undo/redo command should be generated for this.
        void InsertTextAtCursor(const char* text, ptrdiff_t textSizeInTs = -1, bool appendNewCommand = true);

        /// Inserts the contents of the clipboard at the cursor position.
        void InsertClipboardTextAtCursor();

        /// Inserts a new line at the cursor position, moving everything after the cursor on the current line down to it.
        void InsertNewLineAtCursor(bool appendNewCommand = true);

        /// Inserts a tab at the cursor position.
        ///
        /// @param useSpaces [in] Whether or not to use space instead of a normal tab.
        void InsertTabAtCursor(bool useSpaces);


        /// Deletes the character to the left of the cursor.
        ///
        /// This will move the cursor backwards.
        void DeleteCharacterAtLeftOfCursor();

        /// Deletes the character to the left of the cursor.
        void DeleteCharacterAtRightOfCursor();

        /// Deletes the characters in the given range.
        ///
        /// @param appendNewCommand [in] Used internally. Controls whether or not an undo/redo command should be generated for this.
        void DeleteCharactersInRange(const Marker &markerA, const Marker &markerB, bool appendNewCommand = true, bool postChangeEvent = true);
        void DeleteCharactersInRange(int lineStart, int lineStartCharacter, int lineEnd, int lineEndCharacter, bool appendNewCommand = true, bool postChangeEvent = true);

        /// Deletes the selected characters.
        ///
        /// @param postChangeEvent [in] Specifies whether or not the OnTextChanged event should be fired.
        ///
        /// @remarks
        ///     This will move the cursor position to the start of the selection and then remove the selection.
        void DeleteSelectedCharacters(bool postChangeEvent = true);


        /// Copies the characters in the given range to the clipboard.
        void CopyRangeToClipboard(const Marker &markerA, const Marker &markerB) const;

        /// Copies the current selection to the clipboard.
        void CopySelectionToClipboard() const;



        //////////////////////////////////////////////////////////////
        // Selection

        /// Selects a range of text.
        ///
        /// @param startX [in] The x position of the start of the selection.
        /// @param startY [in] The y position of the start of the selection.
        /// @param endX   [in] The x position of the end of the selection.
        /// @param endY   [in] The y position of the end of the selection.
        ///
        /// The selection is rounded to the closest characters. Typically, you'll want to select text based on the cursor position.
        ///
        /// Use SelectAll() to select all of the text.
        void Select(int startX, int startY, int endX, int endY);

        /// Selects all of the text.
        void SelectAll();

        /// Removes the selection.
        ///
        /// @remarks
        ///     This will move both selection markers to the cursor.
        void DeselectAll();

        /// Moves the selection start point.
        void MoveSelectionStart(int x, int y);

        /// Moves the selection end point.
        void MoveSelectionEnd(int x, int y);

        /// Moves the selection start point to the cursor.
        void MoveSelectionStartToCursor();

        /// Moves the selection end point to the curosr.
        void MoveSelectionEndToCursor();

        /// Determines whether or not anything is selected.
        bool IsAnythingSelected() const;



        //////////////////////////////////////////////////////////////
        // Rendering

        /// Retrieves information useful for rendering.
        ///
        /// @param  options       [in] Rendering options.
        /// @param  renderingInfo [in] A reference to the structure that will receive the rendering information.
        void Render(const TextManagerRenderingOptions &options, TextManagerRenderingInfo &renderingInfo);






    private:


        ///////////////////////////////////////////////////////////
        // Private Methods

        /// Resets everything. This is called when SetText() and the destructor is called.
        void Reset();

        /// Refreshes the text width and height.
        void RefreshTextRect();


        /// Helper for updating the position of the given marker on the X axis based on it's current character index, section and line.
        ///
        /// @param marker [in, out] the marker whose x position is being updated.
        void UpdateMarkerXPosition(Marker &marker) const;

        /// Helper for updating the position of the given marker on the Y axis based on it's current line.
        void UpdateMarkerYPosition(Marker &marker) const;


        /// Helper method for building the selection meshes for the given range.
        void BuildSelectionBackgroundMeshes(const Marker &start, const Marker &end, TextManagerRenderingInfo &renderingInfo) const;


        /// Inserts a new text command. This will force eveything after it to be deleted.
        void AppendCommand(TextCommandType type, const char* text, size_t textSizeInTs, int lineStart, int lineStartCharacter, int lineEnd, int lineEndCharacter, bool placeCursorAtStart);


        /// Helper method for posting OnTextChanged events.
        void OnTextChanged()
        {
            if (m_eventHandler != nullptr)
            {
                m_eventHandler->OnTextChanged(m_text);
            }
        }




        //////////////////////////////////////////////////////////////
        // Private Attributes

        /// A pointer to the text managed by the text manager. This is created by GetText(). It becomes invalid whenever text
        /// is modified, in which case it will be re-created by GetText(). 'isTextValid' is used to keep track of whether or
        /// not the text needs to be updated.
        mutable char* m_text;

        /// Keeps track of whether or not 'text' needs to be updated in the next call to GetText().
        mutable bool isTextValid;

        /// The default font. Needs to be a pointer internally since it can be changed. Will never be null.
        Font* defaultFont;

        unsigned int containerWidth;    ///< The width of the container.
        unsigned int containerHeight;   ///< The height of the container.

        int containerOffsetX;           ///< The x offset of the container.
        int containerOffsetY;           ///< The y offset of the container.

        /// The list of lines.
        Vector<TextManagerLine*> lines;

        /// The rectangle encompassing the area taken up by actual text.
        Rect<int> textRect;

        Alignment horizontalAlign;      ///< The horizontal alignment (left, center or right).
        Alignment verticalAlign;        ///< The vertical alingment (top, center or bottom).


        /// The event handler. This needs to be a pointer because it can be changed with SetEventHandler(). Defaults to null.
        TextManagerEventHandler* m_eventHandler;


        /// The marker for the cursor.
        Marker cursorMarker;

        /// The marker for the start of the current selection.
        Marker selectionStartMarker;

        /// The marker for the end of the current selection.
        Marker selectionEndMarker;


        /// The default text colour.
        ColorF defaultTextColour;

        /// The selection background colour.
        ColorF selectionBackgroundColour;


        /// Keeps track of the cursors last horizontal character index before doing vertical movements. Needed to place the cursor relative to
        /// the position it was starting vertical movement.
        size_t cursorCharacterIndexBeforeVerticalMovement;


        /// The command stack for undo/redo.
        Vector<TextCommand> commandStack;

        /// The index of the command that we're currently sitting on. We move this as we undo or redo. This always sits at 1 above the command that
        /// will be the next to be undone. New commands will be placed at this index.
        size_t commandIndex;


        /// The size of a tab, in spaces. Defaults to 4, which means a single tab is the size of 4 space characters.
        unsigned int tabSize;


    private:    // No copying.
        TextManager(const TextManager &);
        TextManager & operator=(const TextManager &);
    };
}

#endif
