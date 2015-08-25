// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/TextManager.hpp>
#include <GTLib/FontServer.hpp>
#include <GTLib/Strings/LineIterator.hpp>
#include <GTLib/Strings/Iterator.hpp>
#include <GTLib/Windowing/Clipboard.hpp>
#include <GTLib/Math.hpp>
#include <cassert>

namespace GTLib
{
    inline const char* GetStringAtCharPos(const char* text, size_t charIndex)
    {
        size_t iChar = 0;
        while (iChar < charIndex && GTLib::Strings::NextChar(text) != '\0')
        {
            ++iChar;
        }

        return text;
    }



    TextManagerLine::TextManagerLine(GT::GUIFontManager* pFontManager, const GT::HGUIFont defaultFontIn, unsigned int tabSizeInPixelsIn, const char* textIn, ptrdiff_t textSizeInTs)
        : m_pFontManager(pFontManager), defaultFont(defaultFontIn), text(nullptr), width(0), height(0), tabSizeInPixels(tabSizeInPixelsIn)
    {
        this->SetText(textIn, textSizeInTs);
    }

    TextManagerLine::~TextManagerLine()
    {
    }


    void TextManagerLine::SetDefaultFont(const GT::HGUIFont defaultFontIn)
    {
        this->defaultFont = defaultFontIn;
        this->RecalculateBounds();
    }

    void TextManagerLine::SetTabSizeInPixels(unsigned int newTabSize)
    {
        this->tabSizeInPixels = newTabSize;
        this->RecalculateBounds();
    }


    int TextManagerLine::CalculateAscent() const
    {
        if (m_pFontManager != nullptr)
        {
            return m_pFontManager->GetAscent(this->defaultFont);
        }
        else
        {
            return 0;
        }
    }

    int TextManagerLine::CalculateDescent() const
    {
        if (m_pFontManager != nullptr)
        {
            return m_pFontManager->GetDescent(this->defaultFont);
        }
        else
        {
            return 0;
        }
    }


    void TextManagerLine::SetText(const char* textIn, ptrdiff_t textSizeInTs)
    {
        this->text.Assign(textIn, textSizeInTs);
        this->RecalculateBounds();
    }

    const char* TextManagerLine::GetText() const
    {
        return this->text.c_str();
    }


    size_t TextManagerLine::GetCharacterCount() const
    {
        return this->text.GetCharacterCount();
    }

    bool TextManagerLine::IsEmpty() const
    {
        return this->text.IsEmpty();
    }

    bool TextManagerLine::IsNotEmpty() const
    {
        return this->text.GetLengthInTs() != 0;
    }


    unsigned int TextManagerLine::Measure(size_t startCharIndex, size_t endCharIndex, int &startPosition, int &endPosition) const
    {
        assert(endCharIndex >= startCharIndex);

        if (this->defaultFont != 0 && m_pFontManager != nullptr)
        {
#if 0
            struct Callback : public FontEngine::MeasureStringCallback
            {
                Callback(int tabSize, size_t startIndex, size_t endIndex, int &startPosition, int &endPosition)
                    : m_tabSize(tabSize),
                      m_startIndex(startIndex), m_endIndex(endIndex), m_currentIndex(0),
                      m_startPosition(startPosition), m_endPosition(endPosition)
                {
                    m_startPosition = 0;
                    m_endPosition   = 0;
                }


                /// FontEngine::MeasureStringCallback::GetTabSize()
                int GetTabSize() const
                {
                    return m_tabSize;
                }

                /// FontEngine::MeasureStringCallback::HandleCharacter()
                bool HandleCharacter(const FontEngine &fontEngine, char32_t character, GlyphHandle glyph, const GTLib::Rect<int> &rect, GlyphMetrics &metrics, int &penPositionX, int &penPositionY)
                {
                    (void)fontEngine;
                    (void)glyph;
                    (void)character;
                    (void)rect;
                    (void)penPositionY;


                    // If we have just hit the start, set the start position.
                    if (m_currentIndex == m_startIndex)
                    {
                        m_startPosition = penPositionX;
                    }

                    m_currentIndex += 1;


                    // If we're at the end we need to set the end position and kill the iteration.
                    if (m_currentIndex == m_endIndex)
                    {
                        m_endPosition = penPositionX + metrics.advance;
                        return false;       // <-- Kill the iteration.
                    }

                    return true;
                }


                int m_tabSize;
                size_t m_startIndex;
                size_t m_endIndex;
                size_t m_currentIndex;
                int &m_startPosition;
                int &m_endPosition;

            }callback(this->tabSizeInPixels, startCharIndex, endCharIndex, startPosition, endPosition);

            // Now we measure.
            this->defaultFont->GetServer().GetFontEngine().MeasureString(this->defaultFont->GetFontHandle(), this->text.c_str(), callback);

            // At this point the callback should have the information we need.
            return callback.m_endPosition - callback.m_startPosition;
#endif

            // TODO: What about kerning between the two segments.

            // Need to measure twice - once for the first part of the string, and again for the second part.
            int textWidth1  = 0;
            int textHeight1 = 0;
            m_pFontManager->MeasureString(this->defaultFont, this->text.c_str(), startCharIndex, textWidth1, textHeight1);

            int textWidth2  = 0;
            int textHeight2 = 0;
            m_pFontManager->MeasureString(this->defaultFont, GetStringAtCharPos(this->text.c_str(), startCharIndex), endCharIndex - startCharIndex, textWidth2, textHeight2);

            startPosition = textWidth1;
            endPosition   = textWidth1 + textWidth2;
            return textWidth2;
        }

        return 0;
    }

    unsigned int TextManagerLine::Measure(size_t startCharIndex, size_t endCharIndex) const
    {
        int startPosition;
        int endPosition;
        return this->Measure(startCharIndex, endCharIndex, startPosition, endPosition);
    }

    unsigned int TextManagerLine::Measure() const
    {
        return this->Measure(0, this->GetCharacterCount());
    }


    void TextManagerLine::DeleteCharacters(size_t startCharIndex, size_t endCharIndex)
    {
        assert(endCharIndex >= startCharIndex);


        // We'll need to iterate over the string and find pointers to the start and end of the range in order to be unicode-correct.
        const char* rangeStart;
        const char* rangeEnd;
        this->GetTextInRange(startCharIndex, endCharIndex, rangeStart, rangeEnd);


        GTLib::Strings::List<char> newText;
        newText.Append(this->text.c_str(), rangeStart - this->text.c_str());
        newText.Append(rangeEnd);

        this->SetText(newText.c_str());
    }


    GTLib::String TextManagerLine::GetTextInRange(size_t startCharIndex, size_t endCharIndex)
    {
        assert(endCharIndex >= startCharIndex);


        // We'll need to iterate over the string and find pointers to the start and end of the range in order to be unicode-correct.
        const char* rangeStart;
        const char* rangeEnd;
        this->GetTextInRange(startCharIndex, endCharIndex, rangeStart, rangeEnd);

        return GTLib::String(rangeStart, rangeEnd - rangeStart);
    }


    void TextManagerLine::GetTextInRange(size_t startCharIndex, size_t endCharIndex, const char* &rangeStart, const char* &rangeEnd)
    {
        size_t thisCharCount = this->GetCharacterCount();

        if (startCharIndex > thisCharCount)
        {
            startCharIndex = thisCharCount;
        }

        if (endCharIndex > thisCharCount)
        {
            endCharIndex = thisCharCount;
        }



        assert(endCharIndex >= startCharIndex);



        // We'll need to iterate over the string and find pointers to the start and end of the range in order to be unicode-correct.
        size_t counter = 0;
        GTLib::Strings::Iterator<char> iCharacter(this->text.c_str());
        while (iCharacter)
        {
            if (counter == startCharIndex)
            {
                rangeStart = iCharacter.str;
            }

            if (counter == endCharIndex)
            {
                rangeEnd = iCharacter.str;
                break;
            }

            ++iCharacter;
            ++counter;
        }


        if (startCharIndex == thisCharCount)
        {
            rangeStart = this->text.c_str() + this->text.GetLengthInTs();
        }

        if (endCharIndex == thisCharCount)
        {
            rangeEnd = this->text.c_str() + this->text.GetLengthInTs();
        }
    }


    void TextManagerLine::RecalculateBounds()
    {
        if (this->defaultFont != 0 && m_pFontManager != nullptr)
        {
            int startPosition;
            int endPosition;

            this->width  = this->Measure(0, this->GetCharacterCount(), startPosition, endPosition);
            this->height = m_pFontManager->GetLineHeight(this->defaultFont);
            //this->height = this->defaultFont->GetLineHeight();
        }
    }
}


namespace GTLib
{
    TextManager::TextManager(GT::GUIFontManager* pFontManager, GT::HGUIFont defaultFontIn)
        : text(nullptr), isTextValid(false),
          m_pFontManager(pFontManager), defaultFont(defaultFontIn),
          containerWidth(0), containerHeight(0),
          containerOffsetX(0), containerOffsetY(0),
          lines(),
          textRect(),
          horizontalAlign(Alignment_Left), verticalAlign(Alignment_Top),
          eventHandler(nullptr),
          cursorMarker(),
          selectionStartMarker(), selectionEndMarker(),
          defaultTextColour(), selectionBackgroundColour(),
          cursorCharacterIndexBeforeVerticalMovement(0),
          commandStack(), commandIndex(0),
          tabSize(4)
    {
        this->AppendNewLine();
        this->MoveCursorToEnd();
    }

    TextManager::~TextManager()
    {
        this->Reset();
    }

    void TextManager::SetText(const char *textIn, bool blockEvent)
    {
        this->Reset();

        // We need lines...
        GTLib::Strings::LineIterator line(textIn);
        while (line)
        {
            auto lineStart = line.start;
            auto lineEnd   = line.end;

            // We want to ignore the carriage-return if it has one.
            if (lineEnd - 1 > lineStart && *(lineEnd - 1) == '\r')
            {
                --lineEnd;
            }

            this->lines.PushBack(new TextManagerLine(m_pFontManager, this->defaultFont, this->GetTabSizeInPixels(), lineStart, lineEnd - lineStart));
            ++line;
        }

        // If we still don't have a line, we'll create one.
        if (this->lines.IsEmpty())
        {
            this->AppendNewLine(true);      // <-- Always block the OnTextChanged() event from here - will be called later on if needed.
        }

        this->RefreshTextRect();
        this->MoveCursorToEnd();

        if (!blockEvent)
        {
            this->OnTextChanged();
        }


        // We will clear the undo stack here.
        this->commandIndex = 0;
        this->commandStack.Clear();
    }

    const char* TextManager::GetText() const
    {
        if (!this->isTextValid)
        {
            GTLib::Strings::Delete(this->text);

            GTLib::Strings::List<char> newTextList;
            for (size_t i = 0; i < this->lines.count; ++i)
            {
                newTextList.Append(this->lines[i]->GetText());

                if (i + 1 != this->lines.count)
                {
                    newTextList.Append("\n");
                }
            }

            this->text        = GTLib::Strings::Create(newTextList);
            this->isTextValid = true;
        }

        return this->text;
    }

    bool TextManager::HasText() const
    {
        // If the first line is not equal to the last line, it means we have more than one line, thus we have text...
        if (this->lines.count > 1)
        {
            return true;
        }

        // If we've made it here, it means we only have a single line.
        return this->lines[0]->IsNotEmpty();
    }

    void TextManager::SetDefaultFont(GT::HGUIFont newDefaultFont)
    {
        if (this->defaultFont != newDefaultFont)
        {
            this->defaultFont = newDefaultFont;


            // Every line needs to have the font changed. This should be temp until we add support for multiple fonts.
            for (size_t i = 0; i < this->lines.count; ++i)
            {
                this->lines[i]->SetDefaultFont(this->defaultFont);
            }


            this->RefreshTextRect();
            this->UpdateMarkerXPosition(this->cursorMarker);
        }
    }

    GT::HGUIFont TextManager::GetDefaultFont()
    {
        return this->defaultFont;
    }


    void TextManager::SetContainerSize(unsigned int width, unsigned int height)
    {
        this->containerWidth  = width;
        this->containerHeight = height;

        this->UpdateMarkerXPosition(this->cursorMarker);
    }

    void TextManager::GetContainerSize(unsigned int &width, unsigned int &height) const
    {
        width  = this->containerWidth;
        height = this->containerHeight;
    }

    void TextManager::SetContainerOffset(int offsetX, int offsetY)
    {
        // We don't need to do a full recalculation of the text rectangle when only the offset has changed. So instead of doing this->RefreshTextRect(), we will just
        // manually modify the attributes.
        int oldOffsetX = this->containerOffsetX;
        int oldOffsetY = this->containerOffsetY;

        this->textRect.left   -= oldOffsetX;
        this->textRect.right  -= oldOffsetX;
        this->textRect.top    -= oldOffsetY;
        this->textRect.bottom -= oldOffsetY;

        this->textRect.left   += offsetX;
        this->textRect.right  += offsetX;
        this->textRect.top    += offsetY;
        this->textRect.bottom += offsetY;


        this->containerOffsetX = offsetX;
        this->containerOffsetY = offsetY;

        this->UpdateMarkerYPosition(this->cursorMarker);
        this->UpdateMarkerXPosition(this->cursorMarker);
    }

    void TextManager::SetHorizontalAlign(Alignment align)
    {
        this->horizontalAlign = align;

        this->RefreshTextRect();

        this->UpdateMarkerYPosition(this->cursorMarker);
        this->UpdateMarkerXPosition(this->cursorMarker);
    }

    void TextManager::SetVerticalAlign(Alignment align)
    {
        this->verticalAlign = align;

        this->RefreshTextRect();

        this->UpdateMarkerYPosition(this->cursorMarker);
        this->UpdateMarkerXPosition(this->cursorMarker);
    }


    void TextManager::GetTextRect(GTLib::Rect<int> &rect) const
    {
        rect = this->textRect;
    }

    unsigned int TextManager::GetTextWidth() const
    {
        return this->textRect.right - this->textRect.left;
    }

    unsigned int TextManager::GetTextHeight() const
    {
        return this->textRect.bottom - this->textRect.top;
    }


    void TextManager::GetLineRect(const TextManagerLine *line, GTLib::Rect<int> &rect) const
    {
        assert(line != nullptr);

        // This depends on the alignment.
        unsigned int lineWidth  = line->GetWidth();
        unsigned int lineHeight = line->GetHeight();
        unsigned int textHeight = this->GetTextHeight();

        if (this->horizontalAlign == Alignment_Right)
        {
            rect.left = this->containerWidth - lineWidth;
        }
        else if (this->horizontalAlign == Alignment_Center)
        {
            rect.left = (this->containerWidth / 2) - (lineWidth / 2);
        }
        else
        {
            rect.left = 0;
        }


        if (this->verticalAlign == Alignment_Bottom)
        {
            rect.top = this->containerHeight - textHeight;
        }
        else if (this->verticalAlign == Alignment_Center)
        {
            rect.top = (this->containerHeight / 2) - (textHeight / 2);
        }
        else
        {
            rect.top = 0;
        }


        // Now we need to loop over each line and move the top downwards until we hit the input line.
        for (size_t i = 0; i < this->lines.count && this->lines[i] != line; ++i)
        {
            rect.top += this->lines[i]->GetHeight();
        }



        rect.left += this->containerOffsetX;
        rect.top  += this->containerOffsetY;

        rect.right  = rect.left + lineWidth;
        rect.bottom = rect.top  + lineHeight;
    }


    TextManagerLine* TextManager::GetPreviousLine(TextManagerLine* line) const
    {
        size_t lineIndex;
        if (this->lines.FindFirstIndexOf(line, lineIndex))
        {
            if (lineIndex > 0)
            {
                return this->lines[lineIndex - 1];
            }
        }

        return nullptr;
    }

    TextManagerLine* TextManager::GetNextLine(TextManagerLine* line) const
    {
        size_t lineIndex;
        if (this->lines.FindFirstIndexOf(line, lineIndex))
        {
            if (lineIndex + 1 < this->lines.count)
            {
                return this->lines[lineIndex + 1];
            }
        }

        return nullptr;
    }


    void TextManager::SetDefaultTextColour(float r, float g, float b)
    {
        this->defaultTextColour.r = r;
        this->defaultTextColour.g = g;
        this->defaultTextColour.b = b;
        this->defaultTextColour.a = 1.0f;
    }

    void TextManager::SetSelectionBackgroundColour(float r, float g, float b)
    {
        this->selectionBackgroundColour.r = r;
        this->selectionBackgroundColour.g = g;
        this->selectionBackgroundColour.b = b;
        this->selectionBackgroundColour.a = 1.0f;
    }


    GTLib::String TextManager::GetTextInRange(const Marker &markerA, const Marker &markerB) const
    {
        // We need to go line-by-line.
        GTLib::String result;

        if (markerA != markerB)
        {
            auto start = &markerA;
            auto end   = &markerB;

            if (this->IsMarkerBefore(markerB, markerA))
            {
                start = &markerB;
                end   = &markerA;
            }

            assert(start != nullptr);
            assert(end   != nullptr);


            // We go line-by-line.
            for (size_t i = start->lineIndex; i < this->lines.count; ++i)
            {
                auto line = this->lines[i];
                assert(line != nullptr);

                size_t startCharIndex = start->characterIndex;
                size_t endCharIndex   = end->characterIndex;

                if (line != start->line)
                {
                    startCharIndex = 0;
                }

                if (line != end->line)
                {
                    endCharIndex = line->GetCharacterCount();
                }


                result += line->GetTextInRange(startCharIndex, endCharIndex);


                // If the line is the same as end.line, we break now.
                if (line == end->line)
                {
                    break;
                }
                else
                {
                    result += "\n";
                }
            }
        }

        return result;
    }

    unsigned int TextManager::GetLineCount() const
    {
        return static_cast<unsigned int>(this->lines.count);
    }

    unsigned int TextManager::GetPageLineCount() const
    {
        if (this->defaultFont != 0 && m_pFontManager != nullptr)
        {
            return this->containerHeight / m_pFontManager->GetLineHeight(this->defaultFont);
        }

        return 0;
    }

    void TextManager::GetVisibleLineRange(size_t &firstIndexOut, size_t &lastIndexOut) const
    {
        // TODO: This needs to be changed when we get support for multiple fonts.
        if (this->defaultFont != 0)
        {
            if (this->lines.count > 0)
            {
                size_t lineHeight = static_cast<size_t>(this->lines[0]->GetHeight());

                firstIndexOut = -this->containerOffsetY / lineHeight;
                lastIndexOut  = GTLib::Min(this->lines.count - 1, firstIndexOut + (this->containerHeight / lineHeight));
            }
            else
            {
                firstIndexOut = 0;
                lastIndexOut  = 0;
            }
        }
    }


    void TextManager::Undo()
    {
        if (this->commandIndex > 0)
        {
            // If the operation was an insert, that range needs to be deleted. If it's a delete operation, we need to add it back in.
            --this->commandIndex;
            auto &command = this->commandStack[this->commandIndex];

            if (command.type == TextCommandType_Insert)
            {
                // The text needs to be deleted.
                this->DeleteCharactersInRange(command.lineStart, command.lineStartCharacter, command.lineEnd, command.lineEndCharacter, false);     // <-- Important that we pass 'false' as the last argument here!
                this->MoveCursorToLine(command.lineStart, command.lineStartCharacter);
            }
            else
            {
                // The text needs to be inserted.
                this->MoveCursorToLine(command.lineStart, command.lineStartCharacter);
                this->InsertTextAtCursor(command.text.c_str(), command.text.GetLengthInTs(), false);                                                // <-- Important that we pass 'false' as the last argument here!

                if (command.placeCursorAtStart)
                {
                    this->MoveCursorToLine(command.lineStart, command.lineStartCharacter);
                }
            }
        }
    }

    void TextManager::Redo()
    {
        if (this->commandIndex < this->commandStack.count)
        {
            auto &command = this->commandStack[this->commandIndex];

            if (command.type == TextCommandType_Insert)
            {
                // The text needs to be inserted.
                this->MoveCursorToLine(command.lineStart, command.lineStartCharacter);
                this->InsertTextAtCursor(command.text.c_str(), command.text.GetLengthInTs(), false);                                                // <-- Important that we pass 'false' as the last argument here!

                if (command.placeCursorAtStart)
                {
                    this->MoveCursorToLine(command.lineStart, command.lineStartCharacter);
                }
            }
            else
            {
                // The text needs to be deleted.
                this->DeleteCharactersInRange(command.lineStart, command.lineStartCharacter, command.lineEnd, command.lineEndCharacter, false);     // <-- Important that we pass 'false' as the last argument here!
                this->MoveCursorToLine(command.lineStart, command.lineStartCharacter);
            }

            ++this->commandIndex;
        }
    }



    unsigned int TextManager::GetTabSizeInSpaces() const
    {
        return this->tabSize;
    }

    void TextManager::SetTabSizeInSpaces(unsigned int newTabSize)
    {
        this->tabSize = newTabSize;

        auto tabSizeInPixels = this->GetTabSizeInPixels();

        // Every line needs to be updated and the text refreshed.
        for (size_t i = 0; i < this->lines.count; ++i)
        {
            this->lines[i]->SetTabSizeInPixels(tabSizeInPixels);
        }

        this->RefreshTextRect();
        this->UpdateMarkerXPosition(this->cursorMarker);
    }

    unsigned int TextManager::GetTabSizeInPixels() const
    {
        // TODO: Profile this function. I want to see how many times this is called.
        if (this->defaultFont != 0 && m_pFontManager != nullptr)
        {
            GT::GUIGlyphMetrics spaceMetrics;
            if (m_pFontManager->GetGlyphMetrics(this->defaultFont, ' ', spaceMetrics))
            {
                return this->tabSize * spaceMetrics.advance;
            }
        }

        return 0;
    }






    void TextManager::MoveCursor(int x, int y)
    {
        this->MoveMarker(x, y, this->cursorMarker);
        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
    }

    void TextManager::MoveCursorToLine(int lineIndex, int characterIndex)
    {
        this->cursorMarker.line           = this->lines[lineIndex];
        this->cursorMarker.lineIndex      = lineIndex;
        this->cursorMarker.characterIndex = characterIndex;

        this->UpdateMarkerYPosition(this->cursorMarker);
        this->UpdateMarkerXPosition(this->cursorMarker);

        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
    }

    void TextManager::MoveCursorToStart()
    {
        this->MoveMarkerToStart(this->cursorMarker);
        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
    }

    void TextManager::MoveCursorToEnd()
    {
        this->MoveMarkerToEnd(this->cursorMarker);
        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
    }

    void TextManager::MoveCursorLeft()
    {
        this->MoveMarkerLeft(this->cursorMarker);
        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
    }

    void TextManager::MoveCursorRight()
    {
        this->MoveMarkerRight(this->cursorMarker);
        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
    }

    void TextManager::MoveCursorUp()
    {
        this->MoveMarkerUp(this->cursorMarker);
    }

    void TextManager::MoveCursorDown()
    {
        this->MoveMarkerDown(this->cursorMarker);
    }

    void TextManager::MoveCursorToFirstSelectionMarker()
    {
        if (this->IsMarkerBefore(this->selectionStartMarker, this->selectionEndMarker))
        {
            this->cursorMarker = this->selectionStartMarker;
        }
        else
        {
            this->cursorMarker = this->selectionEndMarker;
        }

        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
    }

    void TextManager::MoveCursorToLastSelectionMarker()
    {
        if (this->IsMarkerBefore(this->selectionStartMarker, this->selectionEndMarker))
        {
            this->cursorMarker = this->selectionEndMarker;
        }
        else
        {
            this->cursorMarker = this->selectionStartMarker;
        }

        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
    }

    void TextManager::MoveCursorToStartOfLine()
    {
        if (this->cursorMarker.line != nullptr)
        {
            this->cursorMarker.characterIndex = 0;
            this->UpdateMarkerXPosition(this->cursorMarker);

            this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
        }
    }

    void TextManager::MoveCursorToEndOfLine()
    {
        if (this->cursorMarker.line != nullptr)
        {
            this->cursorMarker.characterIndex = this->cursorMarker.line->GetCharacterCount();
            this->UpdateMarkerXPosition(this->cursorMarker);

            this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;
        }
    }


    void TextManager::GetCursorPosition(int &x, int &y) const
    {
        x = this->cursorMarker.posX;
        y = this->cursorMarker.posY;
    }

    const TextManagerLine* TextManager::GetCursorLine() const
    {
        return this->cursorMarker.line;
    }

    size_t TextManager::GetCursorLineIndex() const
    {
        return this->cursorMarker.lineIndex;
    }



    void TextManager::AppendNewLine(bool blockEvent)
    {
        this->lines.PushBack(new TextManagerLine(m_pFontManager, this->defaultFont, this->GetTabSizeInPixels(), ""));

        this->isTextValid = false;

        if (!blockEvent)
        {
            this->OnTextChanged();
        }
    }

    void TextManager::InsertCharacterAtCursor(char32_t c)
    {
        // If we're not positioned anywhere, we're going to move to the very end of the text.
        if (this->cursorMarker.line == nullptr)
        {
            this->MoveCursorToEnd();
        }

        auto sectionText = this->cursorMarker.line->GetText();
        char cANSI = char(c);

        // We need to construct a new string for the current section.
        GTLib::Strings::List<char> newText;
        newText.Append(sectionText, this->cursorMarker.characterIndex);
        newText.Append(&cANSI, 1);
        newText.Append(sectionText + this->cursorMarker.characterIndex);

        this->cursorMarker.line->SetText(newText.c_str());

        // Move the cursor forward.
        ++this->cursorMarker.characterIndex;
        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;


        // We need to make sure the size of the text rectangle is updated. Doing a full refresh with this->RefreshTextRect() is overkill here. All we need
        // to do is check if the new width of the line we just modified is larger than the existing rectangle. If so, we just update the rectangle accordingly.
        if (static_cast<int>(this->cursorMarker.line->GetWidth()) > this->textRect.right - this->textRect.left)
        {
            this->textRect.right = this->textRect.left + this->cursorMarker.line->GetWidth();
        }

        this->UpdateMarkerXPosition(this->cursorMarker);

        this->isTextValid = false;
        this->OnTextChanged();


        // We don't want to do undo/redo character-by-character. Instead we will group them. If the current stack index is at the end, and the last command
        // is an insert that is not a new-line character, we will just append to that one. Otherwise, we'll setup a new command.
        bool appendToPreviousUndoCommand = false;
        if (this->commandStack.count > 0 && this->commandIndex == this->commandStack.count)                                 // <-- Is there a command, and are we sitting on the most recent one?
        {
            auto &previousCommand = this->commandStack.GetBack();
            if (previousCommand.type == TextCommandType_Insert && previousCommand.text != "\n")                             // <-- Was the last command an insert that's not a new line.
            {
                if (previousCommand.lineStart == static_cast<int>(this->cursorMarker.lineIndex))                            // <-- Was the last command on the same line.
                {
                    if (previousCommand.lineEndCharacter == static_cast<int>(this->cursorMarker.characterIndex) - 1)        // <-- Was the character inserted right after the last command.
                    {
                        appendToPreviousUndoCommand = true;
                    }
                }
            }
        }

        //if (this->commandStack.count > 0 && this->commandIndex == this->commandStack.count && this->commandStack.GetBack().type == TextCommandType_Insert && this->commandStack.GetBack().text != "\n")
        if (appendToPreviousUndoCommand)
        {
            this->commandStack.GetBack().text.Append(&c, 1);
            this->commandStack.GetBack().lineEndCharacter += 1;
        }
        else
        {
            this->AppendCommand(TextCommandType_Insert, &cANSI, 1, static_cast<int>(this->cursorMarker.lineIndex), static_cast<int>(this->cursorMarker.characterIndex - 1), static_cast<int>(this->cursorMarker.lineIndex), static_cast<int>(this->cursorMarker.characterIndex), false);
        }
    }

    void TextManager::InsertTextAtCursor(const char* textIn, ptrdiff_t textSizeInTs, bool appendNewCommand)
    {
        // We need to grab the start so we can create an undo/redo command.
        size_t lineStart          = this->cursorMarker.lineIndex;
        size_t lineStartCharacter = this->cursorMarker.characterIndex;



        // If we're not positioned anywhere, we're going to move to the very end of the text.
        if (this->cursorMarker.line == nullptr)
        {
            this->MoveCursorToEnd();
        }


        // We need to insert line-by-line.
        GTLib::Strings::LineIterator lineText(textIn, textSizeInTs);
        while (lineText)
        {
            // TODO: Do an additional check to see if the text manager is in single-line mode. Only care about the first line in that case.
            if (lineText.start != textIn)
            {
                this->InsertNewLineAtCursor(false);
            }


            auto currentLineText = this->cursorMarker.line->GetText();

            // TODO: Make this unicode-compatible.
            GTLib::Strings::List<char> newText;
            newText.Append(currentLineText, this->cursorMarker.characterIndex);
            newText.Append(lineText.start, lineText.end - lineText.start);
            newText.Append(currentLineText + this->cursorMarker.characterIndex);

            this->cursorMarker.line->SetText(newText.c_str());


            this->cursorMarker.characterIndex += (lineText.end - lineText.start);
            this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;


            ++lineText;
        }



        this->RefreshTextRect();

        this->UpdateMarkerXPosition(this->cursorMarker);
        this->UpdateMarkerYPosition(this->cursorMarker);

        this->isTextValid = false;
        this->OnTextChanged();


        // Now we need to append a new undo/redo command.
        if (appendNewCommand)
        {
            size_t lineEnd          = this->cursorMarker.lineIndex;
            size_t lineEndCharacter = this->cursorMarker.characterIndex;

            this->AppendCommand(TextCommandType_Insert, textIn, textSizeInTs, int(lineStart), int(lineStartCharacter), int(lineEnd), int(lineEndCharacter), false);
        }
    }

    void TextManager::InsertClipboardTextAtCursor()
    {
        this->InsertTextAtCursor(GTLib::Clipboard::GetText().c_str());
    }

    void TextManager::InsertNewLineAtCursor(bool appendNewCommand)
    {
        auto   currentLine               = this->cursorMarker.line;
        size_t currentLineIndex          = this->cursorMarker.lineIndex;
        size_t currentLineCharacterIndex = this->cursorMarker.characterIndex;


        // The new line needs text.
        GTLib::Strings::List<char> newLineText;
        newLineText.Append(currentLine->GetText() + currentLineCharacterIndex);

        auto newLine = new TextManagerLine(m_pFontManager, this->defaultFont, this->GetTabSizeInPixels(), newLineText.c_str());
        this->lines.InsertAt(newLine, currentLineIndex + 1);


        // The current line needs new text.
        GTLib::Strings::List<char> currentLineText;
        currentLineText.Append(currentLine->GetText(), currentLineCharacterIndex);

        currentLine->SetText(currentLineText.c_str());


        // Now we finish up by moving the cursor down.
        this->cursorMarker.line           = newLine;
        this->cursorMarker.lineIndex     += 1;
        this->cursorMarker.characterIndex = 0;

        this->UpdateMarkerXPosition(this->cursorMarker);
        this->UpdateMarkerYPosition(this->cursorMarker);

        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;


        // Here we need to update the text rectangle. Doing this->RefreshTextRect() is just overkill. All we need to do is extend the height
        // of the box by the height of the new line.
        this->textRect.bottom += newLine->GetHeight();


        // The text has changed.
        this->isTextValid = false;
        this->OnTextChanged();


        // We need a new undo/redo command for this one.
        if (appendNewCommand)
        {
            this->AppendCommand(TextCommandType_Insert, "\n", 1, int(currentLineIndex), int(currentLineCharacterIndex), int(this->cursorMarker.lineIndex), int(this->cursorMarker.characterIndex), false);
        }
    }

    void TextManager::InsertTabAtCursor(bool useSpaces)
    {
        // If we have a selection, we're going to insert the tab at the start of each selected line.
        if (this->IsAnythingSelected() && this->selectionEndMarker.lineIndex != this->selectionStartMarker.lineIndex)
        {
            // We need to keep track of the cursor position so it can be restored later.
            auto prevCursorMarker = this->cursorMarker;

            size_t iFirstLine = GTLib::Min(this->selectionStartMarker.lineIndex, this->selectionEndMarker.lineIndex);
            size_t iLastLine  = GTLib::Max(this->selectionStartMarker.lineIndex, this->selectionEndMarker.lineIndex);

            for (size_t iLine = iFirstLine; iLine <= iLastLine; ++iLine)
            {
                this->cursorMarker.lineIndex      = iLine;
                this->cursorMarker.characterIndex = 0;
                this->cursorMarker.line           = this->lines[iLine];

                // Insert the tabs at the start of the line.
                if (useSpaces)
                {
                    auto spaces = malloc(this->tabSize);
                    memset(spaces, ' ', this->tabSize);

                    this->InsertTextAtCursor(static_cast<const char*>(spaces), this->tabSize - (this->cursorMarker.characterIndex % this->tabSize));

                    free(spaces);
                }
                else
                {
                    // We just insert the tab as text.
                    this->InsertTextAtCursor("\t");
                }
            }

            this->cursorMarker = prevCursorMarker;
            this->cursorMarker.characterIndex += this->tabSize;

            // The selection markers need to be adjusted slightly.
            this->selectionStartMarker.characterIndex += 4;
            this->selectionEndMarker.characterIndex   += 4;
        }
        else
        {
            // Insert the tabs in place.
            if (useSpaces)
            {
                auto spaces = malloc(this->tabSize);
                memset(spaces, ' ', this->tabSize);

                this->InsertTextAtCursor(static_cast<const char*>(spaces), this->tabSize - (this->cursorMarker.characterIndex % this->tabSize));

                free(spaces);
            }
            else
            {
                // We just insert the tab as text.
                this->InsertTextAtCursor("\t");
            }
        }
    }



    void TextManager::DeleteCharacterAtLeftOfCursor()
    {
        size_t lineEnd          = this->cursorMarker.lineIndex;
        size_t lineEndCharacter = this->cursorMarker.characterIndex;

        char deletedCharacter = '\0';

        bool textChanged = true;

        // For ease of use...
        auto sectionText = this->cursorMarker.line->GetText();

        // If we're at the start of the line, we will need to combine the line with the above line.
        if (this->cursorMarker.characterIndex == 0)
        {
            size_t currentLineIndex = this->cursorMarker.lineIndex;
            if (currentLineIndex > 0)
            {
                auto previousLine = this->lines[currentLineIndex - 1];
                auto currentLine  = this->lines[currentLineIndex];

                this->cursorMarker.line           = previousLine;
                this->cursorMarker.lineIndex     -= 1;
                this->cursorMarker.characterIndex = previousLine->GetCharacterCount();

                GTLib::Strings::List<char> newText;
                newText.Append(previousLine->GetText());
                newText.Append(currentLine->GetText());

                this->cursorMarker.line->SetText(newText.c_str());

                delete currentLine;
                this->lines.Remove(currentLineIndex);

                deletedCharacter = '\n';
            }
            else
            {
                textChanged = false;
            }
        }
        else
        {
            deletedCharacter = sectionText[this->cursorMarker.characterIndex - 1];

            // Now we can modify the section's text.
            GTLib::Strings::List<char> newText;
            newText.Append(sectionText, this->cursorMarker.characterIndex - 1);
            newText.Append(sectionText + this->cursorMarker.characterIndex);

            // Move the cursor backwards.
            --this->cursorMarker.characterIndex;

            // Setting the text must be done after moving the cursor.
            this->cursorMarker.line->SetText(newText.c_str());
        }

        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;


        if (textChanged)
        {
            this->RefreshTextRect();

            this->UpdateMarkerXPosition(this->cursorMarker);
            this->UpdateMarkerYPosition(this->cursorMarker);

            this->isTextValid = false;
            this->OnTextChanged();


            // Now we need to append an undo/redo command.
            size_t lineStart          = this->cursorMarker.lineIndex;
            size_t lineStartCharacter = this->cursorMarker.characterIndex;
            this->AppendCommand(TextCommandType_Delete, &deletedCharacter, 1, int(lineStart), int(lineStartCharacter), int(lineEnd), int(lineEndCharacter), false);
        }
    }

    void TextManager::DeleteCharacterAtRightOfCursor()
    {
        char deletedCharacter = '\0';

        bool textChanged = true;

        auto currentLine = this->cursorMarker.line;
        if (currentLine != nullptr)
        {
            // For ease of use...
            auto currentLineText = currentLine->GetText();

            if (this->cursorMarker.characterIndex == currentLine->GetCharacterCount())
            {
                size_t currentLineIndex = this->cursorMarker.lineIndex;
                if (currentLineIndex + 1 < this->lines.count)
                {
                    auto nextLine = this->lines[currentLineIndex + 1];

                    // All we need to do is append the text of the next line. The cursor does not move.
                    GTLib::Strings::List<char> newText;
                    newText.Append(currentLineText, this->cursorMarker.characterIndex);
                    newText.Append(nextLine->GetText());

                    this->cursorMarker.line->SetText(newText.c_str());

                    delete nextLine;
                    this->lines.Remove(currentLineIndex + 1);

                    deletedCharacter = '\n';
                }
                else
                {
                    textChanged = false;
                }
            }
            else
            {
                deletedCharacter = currentLineText[this->cursorMarker.characterIndex];

                // All we need to do is replace the text. The cursor does not move.
                GTLib::Strings::List<char> newText;
                newText.Append(currentLineText, this->cursorMarker.characterIndex);
                newText.Append(currentLineText + this->cursorMarker.characterIndex + 1);

                this->cursorMarker.line->SetText(newText.c_str());
            }
        }

        this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;


        if (textChanged)
        {
            this->RefreshTextRect();

            this->UpdateMarkerXPosition(this->cursorMarker);
            this->UpdateMarkerYPosition(this->cursorMarker);

            this->isTextValid = false;
            this->OnTextChanged();


            // Now we need to append an undo/redo command.
            size_t lineEnd          = (deletedCharacter != '\n') ? this->cursorMarker.lineIndex          : this->cursorMarker.lineIndex + 1;
            size_t lineEndCharacter = (deletedCharacter != '\n') ? this->cursorMarker.characterIndex + 1 : 0;
            this->AppendCommand(TextCommandType_Delete, &deletedCharacter, 1, int(this->cursorMarker.lineIndex), int(this->cursorMarker.characterIndex), int(lineEnd), int(lineEndCharacter), true);
        }
    }

    void TextManager::DeleteCharactersInRange(const Marker &markerA, const Marker &markerB, bool appendNewCommand, bool postChangeEvent)
    {
        if (markerA != markerB)
        {
            auto start = &markerA;
            auto end   = &markerB;

            if (this->IsMarkerBefore(markerB, markerA))
            {
                start = &markerB;
                end   = &markerA;
            }

            assert(start != nullptr);
            assert(end   != nullptr);


            // We need to grab the text for the undo command.
            GTLib::String textForUndo = this->GetTextInRange(*start, *end);

            auto startLine          = this->lines[start->lineIndex];
            size_t startLineIndex     = start->lineIndex;
            size_t startLineCharacter = start->characterIndex;
            auto endLine            = this->lines[end->lineIndex];
            size_t endLineIndex       = end->lineIndex;
            size_t endLineCharacter   = end->characterIndex;


            bool combineFirstAndLastLines = startLine != endLine;

            for (size_t i = startLineIndex; i <= endLineIndex; )
            {
                auto line = this->lines[i];
                assert(line != nullptr);

                // If the entire line needs to be removed, we'll do that. We never remove the first line in the range, though.
                if (line != startLine && line != endLine)
                {
                    this->lines.Remove(i);
                    delete line;
                }
                else
                {
                    size_t startCharIndex = startLineCharacter;
                    size_t endCharIndex   = endLineCharacter;

                    if (line != startLine)
                    {
                        startCharIndex = 0;
                    }

                    if (line != endLine)
                    {
                        endCharIndex = line->GetCharacterCount();
                    }

                    line->DeleteCharacters(startCharIndex, endCharIndex);

                    if (line == end->line)
                    {
                        if (combineFirstAndLastLines)
                        {
                            GTLib::Strings::List<char> newText;
                            newText.Append(startLine->GetText());
                            newText.Append(endLine->GetText());

                            startLine->SetText(newText.c_str());

                            this->lines.Remove(i);
                            delete line;
                        }

                        break;
                    }
                    else
                    {
                        ++i;
                    }
                }
            }


            this->cursorCharacterIndexBeforeVerticalMovement = this->cursorMarker.characterIndex;


            this->RefreshTextRect();

            if (this->cursorMarker.lineIndex >= this->lines.count)
            {
                this->cursorMarker.line           = this->lines[this->lines.count - 1];
                this->cursorMarker.lineIndex      = this->lines.count - 1;
                this->cursorMarker.characterIndex = this->lines[this->lines.count - 1]->GetCharacterCount();
            }
            else if (this->cursorMarker.characterIndex > this->lines[this->cursorMarker.lineIndex]->GetCharacterCount())
            {
                this->cursorMarker.characterIndex = this->lines[this->cursorMarker.lineIndex]->GetCharacterCount();
            }

            this->cursorMarker.line = this->lines[this->cursorMarker.lineIndex];

            this->UpdateMarkerXPosition(this->cursorMarker);
            this->UpdateMarkerYPosition(this->cursorMarker);

            this->isTextValid = false;

            if (postChangeEvent)
            {
                this->OnTextChanged();
            }


            if (appendNewCommand)
            {
                this->AppendCommand(TextCommandType_Delete, textForUndo.c_str(), textForUndo.GetLengthInTs(), int(startLineIndex), int(startLineCharacter), int(endLineIndex), int(endLineCharacter), false);
            }
        }
    }

    void TextManager::DeleteCharactersInRange(int lineStart, int lineStartCharacter, int lineEnd, int lineEndCharacter, bool appendNewCommand, bool postChangeEvent)
    {
        Marker markerA;
        markerA.line           = this->lines[lineStart];
        markerA.lineIndex      = lineStart;
        markerA.characterIndex = lineStartCharacter;

        Marker markerB;
        markerB.line           = this->lines[lineEnd];
        markerB.lineIndex      = lineEnd;
        markerB.characterIndex = lineEndCharacter;

        this->DeleteCharactersInRange(markerA, markerB, appendNewCommand, postChangeEvent);
    }


    void TextManager::DeleteSelectedCharacters(bool postChangeEvent)
    {
        this->MoveCursorToFirstSelectionMarker();

        this->DeleteCharactersInRange(this->selectionStartMarker, this->selectionEndMarker, true, postChangeEvent);
        this->DeselectAll();

        this->UpdateMarkerXPosition(this->cursorMarker);
        this->UpdateMarkerYPosition(this->cursorMarker);
    }

    void TextManager::CopyRangeToClipboard(const Marker &markerA, const Marker &markerB) const
    {
        GTLib::Clipboard::SetText(this->GetTextInRange(markerA, markerB).c_str());
    }

    void TextManager::CopySelectionToClipboard() const
    {
        this->CopyRangeToClipboard(this->selectionStartMarker, this->selectionEndMarker);
    }



    void TextManager::Select(int startX, int startY, int endX, int endY)
    {
        this->MoveMarker(startX, startY, this->selectionStartMarker);
        this->MoveMarker(endX,   endY,   this->selectionEndMarker);
    }

    void TextManager::SelectAll()
    {
        this->MoveMarkerToStart(this->selectionStartMarker);
        this->MoveMarkerToEnd(this->selectionEndMarker);
    }

    void TextManager::DeselectAll()
    {
        this->selectionStartMarker = this->cursorMarker;
        this->selectionEndMarker   = this->cursorMarker;
    }

    void TextManager::MoveSelectionStart(int x, int y)
    {
        this->MoveMarker(x, y, this->selectionStartMarker);
    }

    void TextManager::MoveSelectionEnd(int x, int y)
    {
        this->MoveMarker(x, y, this->selectionEndMarker);
    }

    void TextManager::MoveSelectionStartToCursor()
    {
        this->selectionStartMarker = this->cursorMarker;
    }

    void TextManager::MoveSelectionEndToCursor()
    {
        this->selectionEndMarker = this->cursorMarker;
    }


    bool TextManager::IsAnythingSelected() const
    {
        return this->selectionStartMarker != this->selectionEndMarker;
    }



    void TextManager::MoveMarker(int x, int y, Marker &marker) const
    {
        if (!this->lines.IsEmpty())
        {
            auto firstLine = this->lines[0];
            auto lastLine  = this->lines[this->lines.count - 1];

            // First we need to find the line that the cursor should be positioned on. Thus, we're finding the y coordinate first.
            if (y <= this->textRect.top)
            {
                marker.posY      = this->textRect.top;
                marker.line      = firstLine;
                marker.lineIndex = 0;
            }
            else if (y >= textRect.bottom)
            {
                marker.posY      = this->textRect.bottom - lastLine->GetHeight();
                marker.line      = lastLine;
                marker.lineIndex = this->lines.count - 1;
            }
            else
            {
                marker.posY = this->textRect.top;

                for (size_t i = 0; i < this->lines.count; ++i)
                {
                    unsigned int lineHeight = this->lines[i]->GetHeight();

                    if (y >= static_cast<int>(marker.posY) && y < static_cast<int>(marker.posY + lineHeight))
                    {
                        marker.line      = this->lines[i];
                        marker.lineIndex = i;

                        break;
                    }
                    else
                    {
                        marker.posY += lineHeight;
                    }
                }
            }

            assert(marker.line != nullptr);

            // With the y position done, we can do the x position. We start the x position from the start of the line, not the start of the text rectangle.
            GTLib::Rect<int> lineRect;
            this->GetLineRect(marker.line, lineRect);

            if (x <= this->textRect.left)
            {
                marker.posX           = lineRect.left;
                marker.characterIndex = 0;
            }
            else if (x >= lineRect.right)
            {
                marker.posX           = lineRect.right;
                marker.characterIndex = marker.line->GetCharacterCount();
            }
            else
            {
                // We start on the left side...
                marker.posX           = lineRect.left;
                marker.characterIndex = 0;


#if 0
                // It's somewhere in the middle of the line, so now we need to correctly position it next to a character.
                struct Callback : public FontEngine::MeasureStringCallback
                {
                    Callback(int tabSize, int x, Marker &marker)
                        : m_tabSize(tabSize), m_x(x), m_marker(marker)
                    {
                    }


                    /// FontEngine::MeasureStringCallback::GetTabSize()
                    int GetTabSize() const
                    {
                        return m_tabSize;
                    }

                    /// FontEngine::GetXStartPosition()
                    int GetXStartPosition() const
                    {
                        return m_marker.posX;
                    }

                    /// FontEngine::MeasureStringCallback::HandleCharacter()
                    bool HandleCharacter(const FontEngine &fontEngine, char32_t character, GlyphHandle glyph, const GTLib::Rect<int> &rect, GlyphMetrics &metrics, int &penPositionX, int &penPositionY)
                    {
                        (void)fontEngine;
                        (void)glyph;
                        (void)character;
                        (void)rect;
                        (void)penPositionX;
                        (void)penPositionY;


                        int nextCursorPosX = m_marker.posX + metrics.advance;

                        if (m_x >= static_cast<int>(m_marker.posX) && m_x < nextCursorPosX)
                        {
                            // The cursor is on top of this character. We need to decide whether or not to move to the left or right of this character.
                            if (m_x >= static_cast<int>(m_marker.posX) + (metrics.advance / 2))
                            {
                                m_marker.posX            = nextCursorPosX;
                                m_marker.characterIndex += 1;
                            }

                            return false;
                        }


                        m_marker.posX            = nextCursorPosX;
                        m_marker.characterIndex += 1;

                        return true;
                    }


                    int m_tabSize;
                    int m_x;
                    Marker &m_marker;

                }callback(this->GetTabSizeInPixels(), x, marker);

                // All we need to do is call MeasureString() and let the callback handle the setting of the marker. After MeasureString() has returned,
                // the marker should be positioned correctly.
                this->defaultFont->GetServer().GetFontEngine().MeasureString(this->defaultFont->GetFontHandle(), marker.line->GetText(), callback);
#endif
            }
        }
        else
        {
            marker.posX           = 0;
            marker.posY           = 0;
            marker.line           = nullptr;
            marker.lineIndex      = 0;
            marker.characterIndex = 0;
        }
    }


    void TextManager::MoveMarkerToStart(Marker &marker) const
    {
        marker.posX           = 0;
        marker.posY           = 0;
        marker.line           = nullptr;
        marker.lineIndex      = 0;
        marker.characterIndex = 0;

        if (!this->lines.IsEmpty())
        {
            marker.line = this->lines[0];
            if (marker.line != nullptr)
            {
                GTLib::Rect<int> lineRect;
                this->GetLineRect(marker.line, lineRect);

                marker.posX = lineRect.left;
                marker.posY = lineRect.top;
            }
        }
    }


    void TextManager::MoveMarkerToEnd(Marker &marker) const
    {
        marker.posX           = 0;
        marker.posY           = 0;
        marker.line           = nullptr;
        marker.lineIndex      = 0;
        marker.characterIndex = 0;

        if (!this->lines.IsEmpty())
        {
            marker.line = this->lines[this->lines.count - 1];
            if (marker.line != nullptr)
            {
                marker.lineIndex      = this->lines.count - 1;
                marker.characterIndex = marker.line->GetCharacterCount();

                GTLib::Rect<int> lineRect;
                this->GetLineRect(marker.line, lineRect);

                marker.posX = lineRect.right;
                marker.posY = lineRect.top;
            }
        }
    }

    void TextManager::MoveMarkerLeft(Marker &marker) const
    {
        if (marker.characterIndex == 0)
        {
            if (marker.lineIndex > 0)
            {
                marker.line           = this->lines[marker.lineIndex - 1];
                marker.lineIndex     -= 1;
                marker.characterIndex = marker.line->GetCharacterCount();

                this->UpdateMarkerXPosition(marker);
                this->UpdateMarkerYPosition(marker);
            }
        }
        else if (marker.characterIndex > 0)
        {
            --marker.characterIndex;
            this->UpdateMarkerXPosition(marker);
        }
    }

    void TextManager::MoveMarkerRight(Marker &marker) const
    {
        if (marker.line != nullptr)
        {
            size_t charCount = marker.line->GetCharacterCount();
            if (charCount == marker.characterIndex)
            {
                if (marker.lineIndex + 1 < this->lines.count)
                {
                    marker.line           = this->lines[marker.lineIndex + 1];
                    marker.lineIndex     += 1;
                    marker.characterIndex = 0;

                    this->UpdateMarkerXPosition(marker);
                    this->UpdateMarkerYPosition(marker);
                }
            }
            else if (charCount > marker.characterIndex)
            {
                ++marker.characterIndex;
                this->UpdateMarkerXPosition(marker);
            }
        }
    }

    void TextManager::MoveMarkerUp(Marker &marker) const
    {
        if (marker.lineIndex > 0)
        {
            auto previousLine = this->lines[marker.lineIndex - 1];

            // Special case for the cursor.
            if (&marker == &this->cursorMarker)
            {
                marker.characterIndex = GTLib::Min(previousLine->GetCharacterCount(), this->cursorCharacterIndexBeforeVerticalMovement);
            }
            else
            {
                marker.characterIndex = GTLib::Min(marker.line->GetCharacterCount(), marker.characterIndex);
            }

            marker.line       = previousLine;
            marker.lineIndex -= 1;


            this->UpdateMarkerXPosition(marker);
            this->UpdateMarkerYPosition(marker);
        }
    }

    void TextManager::MoveMarkerDown(Marker &marker) const
    {
        if (marker.lineIndex + 1 < this->lines.count)
        {
            auto nextLine = this->lines[marker.lineIndex + 1];

            // Special case for the cursor.
            if (&marker == &this->cursorMarker)
            {
                marker.characterIndex = GTLib::Min(nextLine->GetCharacterCount(), this->cursorCharacterIndexBeforeVerticalMovement);
            }
            else
            {
                marker.characterIndex = GTLib::Min(marker.line->GetCharacterCount(), marker.characterIndex);
            }

            marker.line       = nextLine;
            marker.lineIndex += 1;


            this->UpdateMarkerXPosition(marker);
            this->UpdateMarkerYPosition(marker);
        }
    }

    bool TextManager::IsMarkerBefore(const Marker &markerA, const Marker &markerB) const
    {
        if (markerA != markerB)
        {
            if (markerA.line == markerB.line)
            {
                // On the same line.
                return markerA.characterIndex < markerB.characterIndex;
            }
            else
            {
                // On different line.
                return markerA.lineIndex < markerB.lineIndex;
            }
        }

        // Markers are in the same position.
        return false;
    }




    void TextManager::Render(const TextManagerRenderingOptions &options, TextManagerRenderingInfo &renderingInfo)
    {
        (void)options;

        // First thing we do is ensure the existing rendering info is cleared.
        renderingInfo.Clear();

        if (!this->lines.IsEmpty() && this->defaultFont != 0)
        {
#if 0
            // Background
            //
            // When doing the selection background we need to find the marker that comes prior to the other - it's possible that this->selectionEndMarker
            // actually comes before this->selectionStartMarker.
            Marker* selectionStart;
            Marker* selectionEnd;

            if (this->IsMarkerBefore(this->selectionStartMarker, this->selectionEndMarker))
            {
                selectionStart = &this->selectionStartMarker;
                selectionEnd   = &this->selectionEndMarker;
            }
            else
            {
                selectionStart = &this->selectionEndMarker;
                selectionEnd   = &this->selectionStartMarker;
            }

            // At this point we know the start and end points of the selection which we can now use to build the selection background meshes.
            this->BuildSelectionBackgroundMeshes(*selectionStart, *selectionEnd, renderingInfo);


            size_t firstLineIndex = 0;
            size_t lastLineIndex  = 0;
            this->GetVisibleLineRange(firstLineIndex, lastLineIndex);


            // Foreground
            //
            // The foreground can be made up of multiple meshes. We create a mesh for each glyph map that's used. We map meshes to the glyph map handle.
            struct ForegroundMesh
            {
                ForegroundMesh()
                    : vertices(), indices(), highIndex(0)
                {
                }

                GTLib::Vector<TextMeshVertex>   vertices;
                GTLib::Vector<unsigned int> indices;

                /// The largest index value currently in the list. This is used for properly calculating indices.
                unsigned int highIndex;
            };
            GTLib::Map<GlyphMapHandle, ForegroundMesh*> foregroundMeshes;



            // The y offset. This will increment as the line increments.
            int offsetY = this->textRect.top + (firstLineIndex * this->lines[0]->GetHeight());

            for (size_t i = firstLineIndex; i <= lastLineIndex; ++i)
            {
                auto line = this->lines[i];
                assert(line != nullptr);

                // We need the lines rectangle so we can do alignment correctly. Note that currently the line rectangle does
                // not have a correct vertical position.
                GTLib::Rect<int> lineRect;
                this->GetLineRect(line, lineRect);          // TODO: GetLineRect() is actually quite slow because it loops over every line before it. Should make this a bit more efficient.

                // The line's descent. We need this to position the text correctly for each line.
                //int lineAscent = line->CalculateAscent();

                // The x offset. This will increment as we move along the line.
                int offsetX = lineRect.left;


                // It's somewhere in the middle of the line, so now we need to correctly position it next to a character.
                struct Callback : public FontEngine::MeasureStringCallback
                {
                    Callback(int tabSize, int startPosX, int startPosY, Font* font, const GTLib::Colour &defaultTextColour, const TextManagerRenderingOptions &options, GTLib::Map<GlyphMapHandle, ForegroundMesh*> &foregroundMeshes)
                        : m_tabSize(tabSize),
                          m_startPosX(startPosX), m_startPosY(startPosY),
                          m_font(font),
                          m_options(options),
                          m_foregroundMeshes(foregroundMeshes),
                          m_vertex()
                    {
                        m_vertex.colourR = defaultTextColour.r;
                        m_vertex.colourG = defaultTextColour.g;
                        m_vertex.colourB = defaultTextColour.b;
                        m_vertex.colourA = m_options.alpha;
                    }


                    /// FontEngine::MeasureStringCallback::GetTabSize()
                    int GetTabSize() const
                    {
                        return m_tabSize;
                    }

                    /// FontEngine::GetXStartPosition()
                    int GetXStartPosition() const
                    {
                        return m_startPosX;
                    }

                    /// FontEngine::GetYStartPosition()
                    int GetYStartPosition() const
                    {
                        return m_startPosY;
                    }

                    /// FontEngine::MeasureStringCallback::HandleCharacter()
                    bool HandleCharacter(const FontEngine &fontEngine, char32_t character, GlyphHandle glyph, const GTLib::Rect<int> &rect, GlyphMetrics &metrics, int &penPositionX, int &penPositionY)
                    {
                        (void)metrics;
                        (void)penPositionX;
                        (void)penPositionY;

                        if (!GTLib::Strings::IsWhitespace(character))
                        {
                            GTLib::Rect<float> uvCoords;
                            GlyphMapHandle glyphMap = fontEngine.GetGlyphMap(glyph, uvCoords);
                            if (glyphMap != 0)
                            {
                                ForegroundMesh* foregroundMesh = nullptr;

                                auto iForegroundMesh = m_foregroundMeshes.Find(glyphMap);
                                if (iForegroundMesh != nullptr)
                                {
                                    foregroundMesh = iForegroundMesh->value;
                                }
                                else
                                {
                                    foregroundMesh = new ForegroundMesh;
                                    m_foregroundMeshes.Add(glyphMap, foregroundMesh);
                                }


                                float posLeft   = static_cast<float>(rect.left);
                                float posTop    = static_cast<float>(rect.top);
                                float posRight  = static_cast<float>(rect.right);
                                float posBottom = static_cast<float>(rect.bottom);

                                // If the Y origin is at the bottom, we simply swap the top and bottom positions.
                                if (!m_options.yAtTop)
                                {
                                    float temp = posBottom;
                                    posBottom  = posTop;
                                    posTop     = temp;
                                }


                                auto &vertices = foregroundMesh->vertices;
                                auto &indices  = foregroundMesh->indices;

                                // First the position, next the UVs. We need to do this 4 times. We go counter-clockwise starting from the bottom left vertex.
                                m_vertex.positionX = posLeft;
                                m_vertex.positionY = posBottom;
                                m_vertex.texCoordX = uvCoords.left;
                                m_vertex.texCoordY = uvCoords.bottom;
                                vertices.PushBack(m_vertex);

                                m_vertex.positionX = posRight;
                                m_vertex.positionY = posBottom;
                                m_vertex.texCoordX = uvCoords.right;
                                m_vertex.texCoordY = uvCoords.bottom;
                                vertices.PushBack(m_vertex);

                                m_vertex.positionX = posRight;
                                m_vertex.positionY = posTop;
                                m_vertex.texCoordX = uvCoords.right;
                                m_vertex.texCoordY = uvCoords.top;
                                vertices.PushBack(m_vertex);

                                m_vertex.positionX = posLeft;
                                m_vertex.positionY = posTop;
                                m_vertex.texCoordX = uvCoords.left;
                                m_vertex.texCoordY = uvCoords.top;
                                vertices.PushBack(m_vertex);


                                indices.PushBack(foregroundMesh->highIndex + 0);
                                indices.PushBack(foregroundMesh->highIndex + 1);
                                indices.PushBack(foregroundMesh->highIndex + 2);
                                indices.PushBack(foregroundMesh->highIndex + 2);
                                indices.PushBack(foregroundMesh->highIndex + 3);
                                indices.PushBack(foregroundMesh->highIndex + 0);
                                foregroundMesh->highIndex += 4;
                            }
                        }

                        return true;
                    }


                    int m_tabSize;
                    int m_startPosX;
                    int m_startPosY;
                    Font* m_font;
                    const TextManagerRenderingOptions &m_options;
                    GTLib::Map<GlyphMapHandle, ForegroundMesh*> &m_foregroundMeshes;

                    /// A reusable vertex for ease of use.
                    TextMeshVertex m_vertex;

                private:    // No copying.
                    Callback(const Callback &);
                    Callback & operator=(const Callback &);

                }callback(this->GetTabSizeInPixels(), offsetX, offsetY, this->defaultFont, this->defaultTextColour, options, foregroundMeshes);

                this->defaultFont->GetServer().GetFontEngine().MeasureString(this->defaultFont->GetFontHandle(), line->GetText(), callback);

                // We move down by the line height.
                offsetY += (lineRect.bottom - lineRect.top);
            }


            // Now each foreground mesh needs to be added (and then deleted).
            for (size_t iForegroundMesh = 0; iForegroundMesh < foregroundMeshes.count; ++iForegroundMesh)
            {
                auto glyphMapHandle = foregroundMeshes.buffer[iForegroundMesh]->key;
                auto foregroundMesh = foregroundMeshes.buffer[iForegroundMesh]->value;

                assert(glyphMapHandle != 0);
                assert(foregroundMesh != nullptr);
                {
                    if (foregroundMesh->vertices.count > 0 && foregroundMesh->indices.count > 0)
                    {
                        auto newMesh = new TextMesh(&foregroundMesh->vertices[0], foregroundMesh->vertices.count, &foregroundMesh->indices[0], foregroundMesh->indices.count, glyphMapHandle);
                        renderingInfo.meshes.Append(newMesh);
                    }

                    // The foreground mesh needs to be deleted.
                    delete foregroundMesh;
                }
            }
#endif
        }
    }


    void TextManager::Reset()
    {
        GTLib::Strings::Delete(this->text);
        this->text        = nullptr;
        this->isTextValid = false;


        for (size_t i = 0; i < this->lines.count; ++i)
        {
            delete this->lines[i];
        }
        this->lines.Clear();


        this->textRect.left   = 0;
        this->textRect.right  = 0;
        this->textRect.top    = 0;
        this->textRect.bottom = 0;

        this->cursorMarker.posX           = 0;
        this->cursorMarker.posY           = 0;
        this->cursorMarker.line           = nullptr;
        this->cursorMarker.characterIndex = 0;

        this->DeselectAll();
    }

    void TextManager::RefreshTextRect()
    {
        this->textRect.left   = 0;
        this->textRect.right  = 0;
        this->textRect.top    = 0;
        this->textRect.bottom = 0;

        unsigned int textWidth  = 0;
        unsigned int textHeight = 0;

        // First we need the dimensions.
        for (size_t i = 0; i < this->lines.count; ++i)
        {
            textWidth   = GTLib::Max(textWidth, this->lines[i]->GetWidth());
            textHeight += this->lines[i]->GetHeight();
        }

        // x position.
        if (this->containerWidth > 0)
        {
            if (this->horizontalAlign == Alignment_Right)
            {
                this->textRect.left = this->containerWidth - textWidth;
            }
            else if (this->horizontalAlign == Alignment_Center)
            {
                this->textRect.left = (this->containerWidth / 2) - (textWidth / 2);
            }
            else
            {
                this->textRect.left = 0;
            }
        }
        else
        {
            // Auto-sized.
            this->textRect.left = 0;
        }

        // y position.
        if (this->containerHeight > 0)
        {
            if (this->verticalAlign == Alignment_Bottom)
            {
                this->textRect.top = this->containerHeight - textHeight;
            }
            else if (this->verticalAlign == Alignment_Center)
            {
                this->textRect.top = (this->containerHeight / 2) - (textHeight / 2);
            }
            else
            {
                this->textRect.top = 0;
            }
        }
        else
        {
            // Auto-sized.
            this->textRect.top = 0;
        }

        this->textRect.left  += this->containerOffsetX;
        this->textRect.top   += this->containerOffsetY;
        this->textRect.right  = this->textRect.left + textWidth;
        this->textRect.bottom = this->textRect.top  + textHeight;
    }





    void TextManager::UpdateMarkerXPosition(Marker &marker) const
    {
        if (this->defaultFont != 0 && m_pFontManager != nullptr)
        {
            marker.posX = this->textRect.left;

            if (marker.line != nullptr)
            {
                // We just need to measure the string up to the character position of the marker.
                int widthOut  = 0;
                int heightOut = 0;
                m_pFontManager->MeasureString(this->defaultFont, marker.line->GetText(), marker.characterIndex, widthOut, heightOut);

                marker.posX = static_cast<unsigned int>(widthOut);
            }
        }
    }

    void TextManager::UpdateMarkerYPosition(Marker &marker) const
    {
        marker.posY = this->textRect.top;

        if (marker.line != nullptr)
        {
            GTLib::Rect<int> lineRect;
            this->GetLineRect(marker.line, lineRect);

            marker.posY = lineRect.top;
        }
    }



    void TextManager::BuildSelectionBackgroundMeshes(const Marker &start, const Marker &end, TextManagerRenderingInfo &renderingInfo) const
    {
        // We will do this line-by-line and make the height the same across all sections, regardless of the size of the font being used in those sections.
        if (start != end && !this->lines.IsEmpty())
        {
            // We need to find the indices of the visible lines. What we do is just clamp selection range to the visible range.
            size_t visibleStartLineIndex;
            size_t visibleLastLineIndex;
            this->GetVisibleLineRange(visibleStartLineIndex, visibleLastLineIndex);

            if (start.lineIndex <= visibleLastLineIndex && end.lineIndex >= visibleStartLineIndex)
            {
                size_t startLineIndex = GTLib::Max(start.lineIndex, visibleStartLineIndex);
                size_t lastLineIndex  = GTLib::Min(end.lineIndex, visibleLastLineIndex);

                for (size_t i = startLineIndex; i <= lastLineIndex; ++i)
                {
                    auto line = this->lines[i];
                    assert(line != nullptr);

                    size_t startCharIndex = start.characterIndex;
                    size_t endCharIndex   = end.characterIndex;

                    if (line != start.line)
                    {
                        startCharIndex = 0;
                    }

                    if (line != end.line)
                    {
                        endCharIndex = line->GetCharacterCount();
                    }



                    // Now we can measure the rectangle for this line, create a mesh, and then add it to the rendering info.
                    GTLib::Rect<int> lineRect;
                    this->GetLineRect(line, lineRect);              // <-- This will loop over every line before it. This needs improving.

                    int selectionLeft;
                    int selectionRight;
                    line->Measure(startCharIndex, endCharIndex, selectionLeft, selectionRight);

                    float left   = static_cast<float>(lineRect.left + selectionLeft);
                    float top    = static_cast<float>(lineRect.top);
                    float right  = static_cast<float>(lineRect.left + selectionRight);
                    float bottom = static_cast<float>(lineRect.bottom);

                    auto &colour = this->selectionBackgroundColour;


                    // If the line is empty, or we have a selection spanning from the end of one line to the beginning of the next, we won't actually see
                    // anything selected. We're going to add an extra little section to the end of every line except the last one.
                    if (line != end.line)
                    {
                        // Should probably consider making this the size of a space character or something, but a fixed amount will do for now.
                        right += 6.0f;
                    }


                    // Now we create a standard quad.
                    TextMeshVertex vertices[4];

                    // Left/Bottom
                    vertices[0].positionX = left;     vertices[0].positionY = bottom;
                    vertices[0].texCoordX = 0.0f;     vertices[0].texCoordY = 0.0f;
                    vertices[0].colourR   = colour.r; vertices[0].colourG   = colour.g; vertices[0].colourB = colour.b; vertices[0].colourA = colour.a;

                    // Right/Bottom
                    vertices[1].positionX = right;    vertices[1].positionY = bottom;
                    vertices[1].texCoordX = 1.0f;     vertices[1].texCoordY = 0.0f;
                    vertices[1].colourR   = colour.r; vertices[1].colourG   = colour.g; vertices[1].colourB = colour.b; vertices[1].colourA = colour.a;

                    // Right/Top
                    vertices[2].positionX = right;    vertices[2].positionY = top;
                    vertices[2].texCoordX = 1.0f;     vertices[2].texCoordY = 1.0f;
                    vertices[2].colourR   = colour.r; vertices[2].colourG   = colour.g; vertices[2].colourB = colour.b; vertices[2].colourA = colour.a;

                    // Left/Top
                    vertices[3].positionX = left;     vertices[3].positionY = top;
                    vertices[3].texCoordX = 0.0f;     vertices[3].texCoordY = 1.0f;
                    vertices[3].colourR   = colour.r; vertices[3].colourG   = colour.g; vertices[3].colourB = colour.b; vertices[3].colourA = colour.a;


                    unsigned int indices[6] =
                    {
                        0, 1, 2,
                        2, 3, 0
                    };

                    renderingInfo.meshes.Append(new TextMesh(vertices, 4, indices, 6));
                }
            }
        }
    }



    void TextManager::AppendCommand(TextCommandType type, const char* textIn, size_t textSizeInTs, int lineStart, int lineStartCharacter, int lineEnd, int lineEndCharacter, bool placeCursorAtStart)
    {
        TextCommand command;
        command.type               = type;
        command.lineStart          = lineStart;
        command.lineStartCharacter = lineStartCharacter;
        command.lineEnd            = lineEnd;
        command.lineEndCharacter   = lineEndCharacter;
        command.placeCursorAtStart = placeCursorAtStart;
        command.text.Assign(textIn, static_cast<ptrdiff_t>(textSizeInTs));

        // Everything after the current index needs to be removed.
        while (this->commandStack.count > 0 && this->commandStack.count > this->commandIndex)
        {
            this->commandStack.PopBack();
        }

        this->commandStack.PushBack(command);
        ++this->commandIndex;
    }
}
