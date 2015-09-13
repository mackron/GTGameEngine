// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUISimpleTextLayout.hpp>
#include <GTLib/Keyboard.hpp>

#ifndef OUT
#define OUT
#endif

#ifndef DO_NOTHING
#define DO_NOTHING
#endif

#ifndef UNUSED
#define UNUSED
#endif

#ifndef DEFAULT_SPACE_WIDTH
#define DEFAULT_SPACE_WIDTH     4
#endif


namespace GT
{
    GUISimpleTextLayout::GUISimpleTextLayout(GUIFontManager &fontManager)
        : GUITextLayout(fontManager),
          m_text(),
          m_containerBoundsWidth(0), m_containerBoundsHeight(0),
          m_containerInnerOffsetX(0), m_containerInnerOffsetY(0),
          m_tabSizeInSpaces(4),
          m_horizontalAlignment(GUITextLayoutHorizontalAlignment::Left), m_verticalAlignment(GUITextLayoutVerticalAlignment::Top),
          m_hFont(0),
          m_color(),
          m_runs(),
          m_textBoundsWidth(0), m_textBoundsHeight(0),
          m_cursor(),
          m_selectionAnchor(), m_isInSelectionMode(false), m_isAnythingSelected(false)
    {
    }

    GUISimpleTextLayout::~GUISimpleTextLayout()
    {
    }


    void GUISimpleTextLayout::SetText(const char* text)
    {
        m_text = text;

        this->RefreshLayout();
    }

    const char* GUISimpleTextLayout::GetText() const
    {
        return m_text.c_str();
    }


    void GUISimpleTextLayout::SetContainerBounds(unsigned int width, unsigned int height)
    {
        m_containerBoundsWidth  = width;
        m_containerBoundsHeight = height;

        this->RefreshLayout();
    }

    void GUISimpleTextLayout::GetContainerBounds(unsigned int &widthOut, unsigned int &heightOut) const
    {
        widthOut  = m_containerBoundsWidth;
        heightOut = m_containerBoundsHeight;
    }


    void GUISimpleTextLayout::SetContainerInnerOffset(int offsetX, int offsetY)
    {
        m_containerInnerOffsetX = offsetX;
        m_containerInnerOffsetY = offsetY;
    }

    void GUISimpleTextLayout::GetContainerInnerOffset(int &offsetXOut, int &offsetYOut) const
    {
        offsetXOut = m_containerInnerOffsetX;
        offsetYOut = m_containerInnerOffsetY;
    }


    int GUISimpleTextLayout::GetTextWidth() const
    {
        return m_textBoundsWidth;
    }

    int GUISimpleTextLayout::GetTextHeight() const
    {
        return m_textBoundsHeight;
    }


    void GUISimpleTextLayout::SetHorizontalAlignment(GUITextLayoutHorizontalAlignment alignment)
    {
        if (m_horizontalAlignment != alignment)
        {
            m_horizontalAlignment = alignment;

            this->RefreshAlignment();
        }
    }

    GUITextLayoutHorizontalAlignment GUISimpleTextLayout::GetHorizontalAlignment() const
    {
        return m_horizontalAlignment;
    }

    void GUISimpleTextLayout::SetVerticalAlignment(GUITextLayoutVerticalAlignment alignment)
    {
        if (m_verticalAlignment != alignment)
        {
            m_verticalAlignment = alignment;

            this->RefreshAlignment();
        }
    }

    GUITextLayoutVerticalAlignment GUISimpleTextLayout::GetVerticalAlignment() const
    {
        return m_verticalAlignment;
    }

    void GUISimpleTextLayout::SetAlignment(GUITextLayoutHorizontalAlignment horizontalAlignment, GUITextLayoutVerticalAlignment verticalAlignment)
    {
        if (m_horizontalAlignment != horizontalAlignment || m_verticalAlignment != verticalAlignment)
        {
            m_verticalAlignment   = verticalAlignment;
            m_horizontalAlignment = horizontalAlignment;

            this->RefreshAlignment();
        }
    }


    void GUISimpleTextLayout::SetDefaultFont(HGUIFont hFont)
    {
        m_hFont = hFont;

        this->RefreshLayout();
    }

    HGUIFont GUISimpleTextLayout::GetDefaultFont() const
    {
        return m_hFont;
    }


    void GUISimpleTextLayout::SetDefaultTextColor(const GTLib::Colour &color)
    {
        m_color = color;
    }

    GTLib::Colour GUISimpleTextLayout::GetDefaultTextColor() const
    {
        return m_color;
    }


    void GUISimpleTextLayout::GetTextRectRelativeToBounds(GTLib::Rect<int> &rectOut) const
    {
        rectOut.left   = 0;
        rectOut.top    = 0;

        switch (m_horizontalAlignment)
        {
        case GUITextLayoutHorizontalAlignment::Right:
            {
                rectOut.left = m_containerBoundsWidth - m_textBoundsWidth;
                break;
            }

        case GUITextLayoutHorizontalAlignment::Center:
            {
                rectOut.left = (m_containerBoundsWidth - m_textBoundsWidth) / 2;
                break;
            }

        case GUITextLayoutHorizontalAlignment::Left:
        default:
            {
                break;
            }
        }


        switch (m_verticalAlignment)
        {
        case GUITextLayoutVerticalAlignment::Bottom:
            {
                rectOut.top = m_containerBoundsHeight - m_textBoundsHeight;
                break;
            }

        case GUITextLayoutVerticalAlignment::Center:
            {
                rectOut.top = (m_containerBoundsHeight - m_textBoundsHeight) / 2;
                break;
            }

        case GUITextLayoutVerticalAlignment::Top:
        default:
            {
                break;
            }
        }

        rectOut.left  += m_containerInnerOffsetX;
        rectOut.right += m_containerInnerOffsetY;

        rectOut.right  = rectOut.left + m_textBoundsWidth;
        rectOut.bottom = rectOut.top  + m_textBoundsHeight;
    }


    void GUISimpleTextLayout::MoveCursorToPoint(int inputPosX, int inputPosY)
    {
        this->InitMarkerByPointRelativeToContainer(inputPosX, inputPosY, m_cursor);

        if (m_isInSelectionMode) {
            m_isAnythingSelected = this->HasSpacingBetweenSelectionMarkers();
        }
    }

    void GUISimpleTextLayout::GetCursorPosition(int &posXOut, int &posYOut) const
    {
        this->GetMarkerPositionRelativeToContainer(m_cursor, posXOut, posYOut);
    }

    bool GUISimpleTextLayout::MoveCursorLeft()
    {
        if (this->MoveMarkerLeft(m_cursor))
        {
            if (m_isInSelectionMode) {
                m_isAnythingSelected = this->HasSpacingBetweenSelectionMarkers();
            }

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveCursorRight()
    {
        if (this->MoveMarkerRight(m_cursor))
        {
            if (m_isInSelectionMode) {
                m_isAnythingSelected = this->HasSpacingBetweenSelectionMarkers();
            }

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveCursorUp()
    {
        if (this->MoveMarkerUp(m_cursor))
        {
            if (m_isInSelectionMode) {
                m_isAnythingSelected = this->HasSpacingBetweenSelectionMarkers();
            }

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveCursorDown()
    {
        if (this->MoveMarkerDown(m_cursor))
        {
            if (m_isInSelectionMode) {
                m_isAnythingSelected = this->HasSpacingBetweenSelectionMarkers();
            }

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveCursorToEndOfLine()
    {
        if (this->MoveMarkerToEndOfLine(m_cursor))
        {
            if (m_isInSelectionMode) {
                m_isAnythingSelected = this->HasSpacingBetweenSelectionMarkers();
            }

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveCursorToStartOfLine()
    {
        if (this->MoveMarkerToStartOfLine(m_cursor))
        {
            if (m_isInSelectionMode) {
                m_isAnythingSelected = this->HasSpacingBetweenSelectionMarkers();
            }

            return true;
        }

        return false;
    }



    //////////////////////////////
    // Selection

    void GUISimpleTextLayout::EnterSelectionMode()
    {
        if (!m_isInSelectionMode)
        {
            // If nothing is currently selected, we want to set the selection anchor to the current cursor position.
            if (!m_isAnythingSelected)
            {
                m_selectionAnchor = m_cursor;
            }


            m_isInSelectionMode = true;
        }
    }

    void GUISimpleTextLayout::LeaveSelectionMode()
    {
        if (m_isInSelectionMode)
        {
            m_isInSelectionMode = false;
        }
    }

    bool GUISimpleTextLayout::IsInSelectionMode() const
    {
        return m_isInSelectionMode;
    }

    bool GUISimpleTextLayout::IsAnythingSelected() const
    {
        return m_isAnythingSelected;
    }

    void GUISimpleTextLayout::DeselectAll()
    {
        m_isAnythingSelected = false;
    }

    void GUISimpleTextLayout::IterateVisibleSelectionRects(std::function<void (const GUITextRectDesc &textRectDesc)> callback) const
    {
        TextMarker selectionStart;
        TextMarker selectionEnd;
        if (this->GetSelectionMarkers(selectionStart, selectionEnd))
        {
            GUITextRectDesc desc;
            desc.colour = GTLib::Colour(0.2f, 0.6f, 0.8f);

            if (selectionStart.iRun == selectionEnd.iRun)
            {
                // The selection region is on the same run.
                const TextRun &run = m_runs[selectionStart.iRun];

                desc.rect.left   = run.posX + selectionStart.relativePosX + m_containerInnerOffsetX;
                desc.rect.top    = run.posY + m_containerInnerOffsetY;
                desc.rect.right  = run.posX + selectionEnd.relativePosX + m_containerInnerOffsetX;
                desc.rect.bottom = run.posY + run.height + m_containerInnerOffsetY;
                callback(desc);
            }
            else
            {
                // First run.
                desc.rect.left   = m_runs[selectionStart.iRun].posX + selectionStart.relativePosX + m_containerInnerOffsetX;
                desc.rect.top    = m_runs[selectionStart.iRun].posY + m_containerInnerOffsetY;
                desc.rect.right  = m_runs[selectionStart.iRun].posX + m_runs[selectionStart.iRun].width;
                desc.rect.bottom = m_runs[selectionStart.iRun].posY + m_runs[selectionStart.iRun].height + m_containerInnerOffsetY;
                callback(desc);

                // Every run in between the two markers.
                for (unsigned int iRun = selectionStart.iRun + 1; iRun < selectionEnd.iRun; ++iRun)
                {
                    desc.rect.left   = m_runs[iRun].posX + m_containerInnerOffsetX;
                    desc.rect.top    = m_runs[iRun].posY + m_containerInnerOffsetY;
                    desc.rect.right  = m_runs[iRun].posX + m_runs[iRun].width;
                    desc.rect.bottom = m_runs[iRun].posY + m_runs[iRun].height + m_containerInnerOffsetY;
                    callback(desc);
                }

                // The last run.
                desc.rect.left   = m_runs[selectionEnd.iRun].posX + m_containerInnerOffsetX;
                desc.rect.top    = m_runs[selectionEnd.iRun].posY + m_containerInnerOffsetY;
                desc.rect.right  = m_runs[selectionEnd.iRun].posX + selectionEnd.relativePosX;
                desc.rect.bottom = m_runs[selectionEnd.iRun].posY + m_runs[selectionEnd.iRun].height + m_containerInnerOffsetY;
                callback(desc);
            }
        }
    }



    //////////////////////////////
    // Editing

    void GUISimpleTextLayout::InsertCharacterAtCursor(char32_t character)
    {
        // Transform '\r' to '\n'.
        if (character == '\r') {
            character = '\n';
        }


        TextRun &run = m_runs[m_cursor.iRun];
        unsigned int iAbsoluteMarkerChar = run.iChar + m_cursor.iChar;

        // Insert the character into the string.
        m_text.InsertCharacter(character, iAbsoluteMarkerChar);

        // The layout will have changed so it needs to be refreshed.
        this->RefreshLayout();


        // The marker needs to be updated based on the new layout and it's new position, which is one character ahead.
        this->MoveMarkerToCharacter(m_cursor, iAbsoluteMarkerChar + 1);


        // The cursor's sticky position needs to be updated whenever the text is edited.
        this->UpdateMarkerStickyPosition(m_cursor);
    }

    void GUISimpleTextLayout::DeleteCharacterToLeftOfCursor()
    {
        if (this->MoveCursorLeft())
        {
            this->DeleteCharacterToRightOfCursor();
        }
    }

    void GUISimpleTextLayout::DeleteCharacterToRightOfCursor()
    {
        this->DeleteCharacterToRightOfMarker(m_cursor);
    }

    void GUISimpleTextLayout::DeleteSelectedText()
    {
        // We need to get the selected text range and then delete that section from the internal string. Then, we want to position the cursor
        // at the first character index of that range.

        TextMarker selectionStart;
        TextMarker selectionEnd;
        if (this->GetSelectionMarkers(selectionStart, selectionEnd))
        {
            unsigned int iCharStart = m_runs[selectionStart.iRun].iChar + selectionStart.iChar;
            unsigned int iCharEnd   = m_runs[selectionEnd.iRun].iChar   + selectionEnd.iChar;


            // Remove the character from the string.
            m_text.EraseRange(iCharStart, iCharEnd);

            // The layout will have changed.
            this->RefreshLayout();


            // The marker needs to be updated based on the new layout.
            this->MoveMarkerToCharacter(m_cursor, iCharStart);


            // The cursor's sticky position needs to be updated whenever the text is edited.
            this->UpdateMarkerStickyPosition(m_cursor);


            // Reset the selection marker.
            m_selectionAnchor = m_cursor;
            m_isAnythingSelected = false;
        }
    }




    //////////////////////////////////////////
    // Private

    void GUISimpleTextLayout::IterateVisibleTextRuns(std::function<void (const GUITextRunDesc &textRunDesc)> func) const
    {
        // This is a naive implementation. Can be improved a bit.
        for (size_t iRun = 0; iRun < m_runs.GetCount(); ++iRun)
        {
            const TextRun &run = m_runs[iRun];

            if (!this->IsRunWhitespace(run))
            {
                int runTop    = run.posY + m_containerInnerOffsetY;
                int runBottom = runTop   + run.height;

                if (runBottom > 0 && runTop < int(m_containerBoundsHeight))
                {
                    int runLeft  = run.posX + m_containerInnerOffsetX;
                    int runRight = runLeft  + run.width;

                    if (runRight > 0 && runLeft < int(m_containerBoundsWidth))
                    {
                        // The run is visible.
                        GUITextRunDesc runDesc;
                        GTLib::Strings::Copy(runDesc.text, GT_MAX_TEXT_RUN_SIZE_IN_BYTES, m_text.c_str() + run.iChar, run.iCharEnd - run.iChar);
                        runDesc.hFont             = m_hFont;
                        runDesc.xPos              = runLeft;
                        runDesc.yPos              = runTop;
                        runDesc.rotationInDegrees = 0;
                        runDesc.color             = m_color;
                        func(runDesc);
                    }
                }
            }
        }
    }


    void GUISimpleTextLayout::SetTabSizeInSpaces(int tabSizeInSpaces)
    {
        m_tabSizeInSpaces = tabSizeInSpaces;
    }
        
    int GUISimpleTextLayout::GetTabSizeInSpaces() const
    {
        return m_tabSizeInSpaces;
    }



    //////////////////////////////////////////
    // Private

    bool GUISimpleTextLayout::NextRunString(const char* runStart, const char* textEndPastNullTerminator, const char* &runEnd)
    {
        assert(runStart <= textEndPastNullTerminator);

        if (runStart == nullptr || runStart == textEndPastNullTerminator)
        {
            // String is empty.
            return false;
        }


        char firstChar = runStart[0];
        if (firstChar == '\t')
        {
            // We loop until we hit anything that is not a tab character (tabs will be grouped together into a single run).
            do
            {
                runStart += 1;
                runEnd = runStart;
            } while (runStart[0] != '\0' && runStart[0] == '\t');
        }
        else if (firstChar == '\n')
        {
            runStart += 1;

            if (runStart[0] == '\r') {
                runStart += 1;
            }

            runEnd = runStart;
        }
        else if (firstChar == '\0')
        {
            assert(runStart + 1 == textEndPastNullTerminator);
            runEnd = textEndPastNullTerminator;
        }
        else
        {
            do
            {
                runStart += 1;
                runEnd = runStart;
            } while (runStart[0] != '\0' && runStart[0] != '\t' && runStart[0] != '\n');
        }

        return true;
    }

    void GUISimpleTextLayout::RefreshLayout()
    {
        const int lineHeight = this->GetLineHeight();
        const int spaceWidth = this->GetSpaceWidth();
        const int tabWidth   = this->GetTabWidth();


        // We split the runs based on tabs and new-lines. We want to create runs for tabs and new-line characters as well because we want
        // to have the entire string covered by runs for the sake of simplicity when it comes to editing.
        //
        // The first pass positions the runs based on a top-to-bottom, left-to-right alignment. The second pass then repositions the runs
        // based on alignment.
        m_textBoundsWidth  = 0;
        m_textBoundsHeight = 0;

        // The previous runs need to be removed.
        m_runs.Clear();

        unsigned int iCurrentLine = 0;

        const char* nextRunStart = m_text.c_str();
        const char* nextRunEnd;
        while (this->NextRunString(nextRunStart, m_text.c_str() + m_text.GetLengthInTs() + 1, OUT nextRunEnd))
        {
            TextRun run;
            run.iLine          = iCurrentLine;
            run.iChar          = nextRunStart - m_text.c_str();
            run.iCharEnd       = nextRunEnd   - m_text.c_str();
            run.characterCount = GTLib::Strings::GetCharacterCount(nextRunStart, nextRunEnd - nextRunStart);
            run.width          = 0;
            run.height         = lineHeight;
            run.posX           = 0;
            run.posY           = run.iLine * lineHeight;

            // X position
            //
            // The x position depends on the previous run that's on the same line.
            if (m_runs.GetCount() > 0)
            {
                TextRun &prevRun = m_runs.GetBack();
                if (prevRun.iLine == iCurrentLine)
                {
                    run.posX = prevRun.posX + prevRun.width;
                }
                else
                {
                    // It's the first run on the line.
                    run.posX = 0;
                }
            }


            // Width.
            assert(nextRunEnd > nextRunStart);
            if (nextRunStart[0] == '\t')
            {
                // Tab.
                const unsigned int tabCount = run.iCharEnd - run.iChar;
                run.width = ((tabCount*tabWidth) - (run.posX % tabWidth));
            }
            else if (nextRunStart[0] == '\n')
            {
                // New line.
                iCurrentLine += 1;
                run.width = spaceWidth;     // Set the width of this to a space so that blank lines are visualized when selected.
            }
            else if (nextRunStart[0] == '\0')
            {
                // Null terminator.
                run.width = 0;
            }
            else
            {
                // Normal run.
                int runWidth  = 0;
                int runHeight = 0;
                if (this->GetFontManager().MeasureString(m_hFont, nextRunStart, run.characterCount, runWidth, runHeight))
                {
                    run.width = runWidth;
                }
            }



            // Update the text bounds.
            if (m_textBoundsWidth < run.posX + run.width)
            {
                m_textBoundsWidth = run.posX + run.width;
            }

            m_textBoundsHeight = (iCurrentLine + 1) * lineHeight;



            // Add the run to the internal list.
            m_runs.PushBack(run);

            // Go to the next run string.
            nextRunStart = nextRunEnd;
        }


        // If we were to return now the text would be alignment top/left. If the alignment is not top/left we need to refresh the layout.
        if (m_horizontalAlignment != GUITextLayoutHorizontalAlignment::Left || m_verticalAlignment != GUITextLayoutVerticalAlignment::Top)
        {
            this->RefreshAlignment();
        }
    }


    void GUISimpleTextLayout::RefreshAlignment()
    {
        const int lineHeight = this->GetFontManager().GetLineHeight(m_hFont);

        unsigned int iCurrentLine = 0;
        for (size_t iRun = 0; iRun < m_runs.GetCount(); DO_NOTHING)     // iRun is incremented from within the loop.
        {
            int lineWidth = 0;

            // This loop does a few things. First, it defines the end point for the loop after this one (jRun). Second, it calculates
            // the line width which is needed for center and right alignment. Third it resets the position of each run to their
            // unaligned equivalents which will be offsetted in the second loop.
            size_t jRun;
            for (jRun = iRun; jRun < m_runs.GetCount() && m_runs[jRun].iLine == iCurrentLine; ++jRun)
            {
                TextRun &run = m_runs[jRun];
                run.posX = lineWidth;
                run.posY = iCurrentLine * lineHeight;

                lineWidth += m_runs[jRun].width;
            }


            // The actual alignment is done here.
            int lineOffsetX;
            int lineOffsetY;
            this->CalculateLineAlignmentOffset(lineWidth, OUT lineOffsetX, OUT lineOffsetY);

            for (DO_NOTHING; iRun < jRun; ++iRun)
            {
                TextRun &run = m_runs[iRun];
                run.posX += lineOffsetX;
                run.posY += lineOffsetY;
            }


            // Go to the next line.
            iCurrentLine += 1;
        }
    }

    void GUISimpleTextLayout::CalculateLineAlignmentOffset(int lineWidth, int &offsetXOut, int &offsetYOut) const
    {
        switch (m_horizontalAlignment)
        {
        case GUITextLayoutHorizontalAlignment::Right:
            {
                offsetXOut = int(m_containerBoundsWidth) - lineWidth;
                break;
            }

        case GUITextLayoutHorizontalAlignment::Center:
            {
                offsetXOut = (int(m_containerBoundsWidth) - lineWidth) / 2;
                break;
            }

        case GUITextLayoutHorizontalAlignment::Left:
        default:
            {
                offsetXOut = 0;
                break;
            }
        }


        switch (m_verticalAlignment)
        {
        case GUITextLayoutVerticalAlignment::Bottom:
            {
                offsetYOut = int(m_containerBoundsHeight) - m_textBoundsHeight;
                break;
            }

        case GUITextLayoutVerticalAlignment::Center:
            {
                offsetYOut = (int(m_containerBoundsHeight) - m_textBoundsHeight) / 2;
                break;
            }

        case GUITextLayoutVerticalAlignment::Top:
        default:
            {
                offsetYOut = 0;
                break;
            }
        }
    }


    bool GUISimpleTextLayout::IsRunWhitespace(const TextRun &run) const
    {
        if (run.characterCount > 0)
        {
            if (m_text.c_str()[run.iChar] != '\t' && m_text.c_str()[run.iChar] != '\n')
            {
                return false;
            }
        }

        return true;
    }


    bool GUISimpleTextLayout::InitMarkerByPointRelativeToContainer(int inputPosX, int inputPosY, TextMarker &markerOut) const
    {
        markerOut.iRun         = 0;
        markerOut.iChar        = 0;
        markerOut.relativePosX = 0;

        GTLib::Rect<int> textRect;
        this->GetTextRectRelativeToBounds(textRect);

        int inputPosXRelativeToText = inputPosX - textRect.left;
        int inputPosYRelativeToText = inputPosY - textRect.top;
        if (this->MoveMarkerToPoint(markerOut, inputPosXRelativeToText, inputPosYRelativeToText))
        {
            this->UpdateMarkerStickyPosition(markerOut);
            return true;
        }

        return false;
    }

    void GUISimpleTextLayout::GetMarkerPositionRelativeToContainer(const TextMarker &marker, int &posXOut, int &posYOut) const
    {
        posXOut = 0;
        posYOut = 0;

        if (marker.iRun < m_runs.GetCount())
        {
            posXOut = m_runs[marker.iRun].posX + marker.relativePosX + m_containerInnerOffsetX;
            posYOut = m_runs[marker.iRun].posY                       + m_containerInnerOffsetY;
        }
    }


    bool GUISimpleTextLayout::FindClosestRunToPoint(int inputPosXRelativeToText, int inputPosYRelativeToText, unsigned int &iRunOut) const
    {
        unsigned int iFirstRunOnLine;
        unsigned int iLastRunOnLinePlus1;
        if (this->FindClosestLineToPoint(inputPosYRelativeToText, OUT iFirstRunOnLine, OUT iLastRunOnLinePlus1))
        {
            const TextRun &firstRunOnLine = m_runs[iFirstRunOnLine];
            const TextRun &lastRunOnLine  = m_runs[iLastRunOnLinePlus1 - 1];

            if (inputPosXRelativeToText < firstRunOnLine.posX)
            {
                // It's to the left of the first run.
                iRunOut = iFirstRunOnLine;
            }
            else if (inputPosXRelativeToText > lastRunOnLine.posX + lastRunOnLine.width)
            {
                // It's to the right of the last run.
                iRunOut = iLastRunOnLinePlus1 - 1;
            }
            else
            {
                // It's in the middle of the line. We just iterate over each run on the line and return the first one that contains the point.
                for (unsigned int iRun = iFirstRunOnLine; iRun < iLastRunOnLinePlus1; ++iRun)
                {
                    const TextRun &run = m_runs[iRun];

                    if (inputPosXRelativeToText >= run.posX && inputPosXRelativeToText <= run.posX + run.width)
                    {
                        iRunOut = iRun;
                        break;
                    }
                }
            }

            return true;
        }
        else
        {
            // There was an error finding the closest line.
            return false;
        }
    }

    bool GUISimpleTextLayout::FindClosestLineToPoint(int inputPosYRelativeToText, unsigned int &iFirstRunOnLineOut, unsigned int &iLastRunOnLinePlus1Out) const
    {
        if (m_runs.GetCount() > 0)
        {
            unsigned int iFirstRunOnLine     = 0;
            unsigned int iLastRunOnLinePlus1 = 0;

            int runningLineTop = 0;

            int lineHeight;
            while (this->FindLineInfo(iFirstRunOnLine, OUT iLastRunOnLinePlus1, OUT lineHeight))
            {
                const int lineTop    = runningLineTop;
                const int lineBottom = lineTop + lineHeight;

                iFirstRunOnLineOut     = iFirstRunOnLine;
                iLastRunOnLinePlus1Out = iLastRunOnLinePlus1;

                if (inputPosYRelativeToText < lineBottom)
                {
                    // It's on this line.
                    break;
                }
                else
                {
                    // It's not on this line - go to the next one.
                    iFirstRunOnLine = iLastRunOnLinePlus1;
                    runningLineTop  = lineBottom;
                }
            }

            return true;
        }

        return false;
    }

    unsigned int GUISimpleTextLayout::FindRunAtCharacterIndex(unsigned int iChar)
    {
        if (iChar < m_text.GetLengthInTs())
        {
            for (unsigned int iRun = 0; iRun < static_cast<unsigned int>(m_runs.GetCount()); ++iRun)
            {
                TextRun &run = m_runs[iRun];

                if (iChar < run.iCharEnd)
                {
                    return iRun;
                }
            }

            // Should never get here.
            assert(false);
            return 0;
        }
        else
        {
            // The character index is too high. Return the last run.
            return static_cast<unsigned int>(m_runs.GetCount()) - 1;
        }
    }

    bool GUISimpleTextLayout::FindLineInfo(unsigned int iFirstRunOnLine, unsigned int &iLastRunOnLinePlus1Out, int &lineHeightOut) const
    {
        if (iFirstRunOnLine < m_runs.GetCount())
        {
            const unsigned int iLine = m_runs[iFirstRunOnLine].iLine;
            int lineHeight = 0;

            unsigned int iRun;
            for (iRun = iFirstRunOnLine; iRun < m_runs.GetCount() && m_runs[iRun].iLine == iLine; ++iRun)
            {
                if (lineHeight < m_runs[iRun].height) {
                    lineHeight = m_runs[iRun].height;
                }
            }

            assert(iRun > iFirstRunOnLine);
            iLastRunOnLinePlus1Out = iRun;
            lineHeightOut          = lineHeight;

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::FindLineInfoByIndex(unsigned int iLine, GTLib::Rect<int> &rectOut, unsigned int &iFirstRunOut, unsigned int &iLastRunPlus1Out) const
    {
        unsigned int iFirstRunOnLine     = 0;
        unsigned int iLastRunOnLinePlus1 = 0;

        int lineTop    = 0;
        int lineHeight = 0;

        for (unsigned int iCurrentLine = 0; iCurrentLine <= iLine; ++iCurrentLine)
        {
            lineTop += lineHeight;

            if (!this->FindLineInfo(iFirstRunOnLine, iLastRunOnLinePlus1, lineHeight))
            {
                // There was an error retrieving information about the line.
                return false;
            }
        }


        // At this point we have the first and last runs that make up the line and we can generate our output.
        if (iLastRunOnLinePlus1 > iFirstRunOnLine)
        {
            iFirstRunOut     = iFirstRunOnLine;
            iLastRunPlus1Out = iLastRunOnLinePlus1;

            rectOut.left   = m_runs[iFirstRunOnLine].posX;
            rectOut.right  = m_runs[iLastRunOnLinePlus1 - 1].posX + m_runs[iLastRunOnLinePlus1 - 1].width;
            rectOut.top    = lineTop;
            rectOut.bottom = rectOut.top + lineHeight;

            return true;
        }
        else
        {
            // We couldn't find any runs.
            return false;
        }
    }


    bool GUISimpleTextLayout::FindLastRunOnLineStartingFromRun(unsigned int iRun, unsigned int &iLastRunOnLineOut) const
    {
        unsigned int iLine = m_runs[iRun].iLine;
        for (DO_NOTHING; iRun < m_runs.GetCount() && m_runs[iRun].iLine == iLine; ++iRun)
        {
            iLastRunOnLineOut = iRun;
        }


        return true;
    }

    bool GUISimpleTextLayout::FindFirstRunOnLineStartingFromRun(unsigned int iRun, unsigned int &iFirstRunOnLineOut) const
    {
        iFirstRunOnLineOut = iRun;

        unsigned int iLine = m_runs[iRun].iLine;
        for (DO_NOTHING; iRun > 0 && m_runs[iRun - 1].iLine == iLine; --iRun)
        {
            iFirstRunOnLineOut = iRun - 1;
        }

        return true;
    }



    bool GUISimpleTextLayout::MoveMarkerToPoint(TextMarker &marker, int inputPosXRelativeToText, int inputPosYRelativeToText) const
    {
        unsigned int iClosestRunToPoint;
        if (this->FindClosestRunToPoint(inputPosXRelativeToText, inputPosYRelativeToText, OUT iClosestRunToPoint))
        {
            const TextRun &run = m_runs[iClosestRunToPoint];

            marker.iRun = iClosestRunToPoint;

            if (inputPosXRelativeToText < run.posX)
            {
                // It's to the left of the run.
                marker.iChar        = 0;
                marker.relativePosX = 0;
            }
            else if (inputPosXRelativeToText > run.posX + run.width)
            {
                // It's to the right of the run. It may be a new-line run. If so, we need to move the marker to the front of it, not the back.
                marker.iChar        = run.characterCount;
                marker.relativePosX = run.width;

                if (m_text.c_str()[run.iChar] == '\n') {
                    assert(marker.iChar == 1);
                    marker.iChar       -= 1;
                    marker.relativePosX = 0;
                }
            }
            else
            {
                // It's somewhere in the middle of the run. We need to handle this a little different for tab runs since they are aligned differently.
                if (m_text.c_str()[run.iChar] == '\n')
                {
                    // It's a new line character. It needs to be placed at the beginning of it.
                    marker.iChar        = 0;
                    marker.relativePosX = 0;
                }
                else if (m_text.c_str()[run.iChar] == '\t')
                {
                    // It's a tab run.
                    marker.iChar        = 0;
                    marker.relativePosX = 0;

                    int tabWidth = this->GetTabWidth();

                    int tabLeft = run.posX + marker.relativePosX;
                    for (; marker.iChar < run.characterCount; ++marker.iChar)
                    {
                        const int tabRight = tabWidth * ((run.posX + (tabWidth*(marker.iChar + 1))) / tabWidth);
                        if (inputPosXRelativeToText >= tabLeft && inputPosXRelativeToText <= tabRight)
                        {
                            // The input position is somewhere on top of this character. If it's positioned on the left side of the character, set the output
                            // value to the character at iChar. Otherwise it should be set to the character at iChar + 1.
                            int charBoundsRightHalf = tabLeft + int(ceil(((tabRight - tabLeft) / 2.0f)));
                            if (inputPosXRelativeToText <= charBoundsRightHalf) {
                                marker.relativePosX = tabLeft - run.posX;
                            } else {
                                marker.relativePosX = tabRight - run.posX;
                                marker.iChar += 1;
                            }

                            break;
                        }

                        tabLeft = tabRight;
                    }

                    // If we're past the last character in the tab run, we want to move to the start of the next run.
                    if (marker.iChar == run.characterCount) {
                        this->MoveMarkerToFirstCharacterOfNextRun(marker);
                    }
                }
                else
                {
                    // It's a standard run.
                    int inputPosXRelativeToRun = inputPosXRelativeToText - run.posX;
                    if (this->GetFontManager().GetTextCursorPositionFromPoint(m_hFont, m_text.c_str() + run.iChar, run.characterCount, run.width, inputPosXRelativeToRun, OUT marker.relativePosX, OUT marker.iChar))
                    {
                        // If the marker is past the last character of the run it needs to be moved to the start of the next one.
                        if (marker.iChar == run.characterCount) {
                            this->MoveMarkerToFirstCharacterOfNextRun(marker);
                        }
                    }
                    else
                    {
                        // An error occured somehow.
                        return false;
                    }
                }
            }

            return true;
        }
        else
        {
            // Couldn't find a run.
            return false;
        }
    }

    bool GUISimpleTextLayout::MoveMarkerLeft(TextMarker &marker) const
    {
        if (m_runs.GetCount() > 0)
        {
            if (marker.iChar > 0)
            {
                marker.iChar -= 1;

                const TextRun &run = m_runs[marker.iRun];
                if (m_text.c_str()[run.iChar] == '\t')
                {
                    const int tabWidth = this->GetTabWidth();

                    if (marker.iChar == 0)
                    {
                        // Simple case - it's the first tab character which means the relative position is just 0.
                        marker.relativePosX = 0;
                    }
                    else
                    {
                        marker.relativePosX  = tabWidth * ((run.posX + (tabWidth*(marker.iChar + 0))) / tabWidth);
                        marker.relativePosX -= run.posX;
                    }
                }
                else
                {
                    if (!this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_text.c_str() + m_runs[marker.iRun].iChar, marker.iChar, marker.relativePosX))
                    {
                        return false;
                    }
                }
            }
            else
            {
                // We're at the beginning of the run which means we need to transfer the cursor to the end of the previous run.
                if (!this->MoveMarkerToLastCharacterOfPreviousRun(marker))
                {
                    return false;
                }
            }

            this->UpdateMarkerStickyPosition(marker);
            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerRight(TextMarker &marker) const
    {
        if (m_runs.GetCount() > 0)
        {
            if (marker.iChar + 1 < m_runs[marker.iRun].characterCount)
            {
                marker.iChar += 1;

                const TextRun &run = m_runs[marker.iRun];
                if (m_text.c_str()[run.iChar] == '\t')
                {
                    const int tabWidth = this->GetTabWidth();

                    marker.relativePosX  = tabWidth * ((run.posX + (tabWidth*(marker.iChar + 0))) / tabWidth);
                    marker.relativePosX -= run.posX;
                }
                else
                {
                    if (!this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_text.c_str() + m_runs[marker.iRun].iChar, marker.iChar, marker.relativePosX))
                    {
                        return false;
                    }
                }
            }
            else
            {
                // We're at the end of the run which means we need to transfer the cursor to the beginning of the next run.
                if (!this->MoveMarkerToFirstCharacterOfNextRun(marker))
                {
                    return false;
                }
            }

            this->UpdateMarkerStickyPosition(marker);
            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerUp(TextMarker &marker) const
    {
        const TextRun &oldRun = m_runs[marker.iRun];
        if (oldRun.iLine > 0)
        {
            GTLib::Rect<int> lineRect;
            unsigned int iFirstRunOnLine;
            unsigned int iLastRunOnLinePlus1;
            if (this->FindLineInfoByIndex(oldRun.iLine - 1, OUT lineRect, OUT iFirstRunOnLine, OUT iLastRunOnLinePlus1))
            {
                int newMarkerPosX = marker.absoluteSickyPosX; //oldRun.posX + marker.relativePosX;
                int newMarkerPosY = lineRect.top;
                this->MoveMarkerToPoint(marker, newMarkerPosX, newMarkerPosY);

                return true;
            }
            else
            {
                // An error occured while finding information about the line above.
                return false;
            }
        }
        else
        {
            // The cursor is already on the top line.
            return false;
        }
    }

    bool GUISimpleTextLayout::MoveMarkerDown(TextMarker &marker) const
    {
        const TextRun &oldRun = m_runs[marker.iRun];

        GTLib::Rect<int> lineRect;
        unsigned int iFirstRunOnLine;
        unsigned int iLastRunOnLinePlus1;
        if (this->FindLineInfoByIndex(oldRun.iLine + 1, OUT lineRect, OUT iFirstRunOnLine, OUT iLastRunOnLinePlus1))
        {
            int newMarkerPosX = marker.absoluteSickyPosX; //oldRun.posX + marker.relativePosX;
            int newMarkerPosY = lineRect.top;
            this->MoveMarkerToPoint(marker, newMarkerPosX, newMarkerPosY);

            return true;
        }
        else
        {
            // An error occured while finding information about the line above.
            return false;
        }
    }

    bool GUISimpleTextLayout::MoveMarkerToEndOfLine(TextMarker &marker) const
    {
        unsigned int iLastRunOnLine;
        if (this->FindLastRunOnLineStartingFromRun(marker.iRun, iLastRunOnLine))
        {
            return this->MoveMarkerToLastCharacterOfRun(marker, iLastRunOnLine);
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToStartOfLine(TextMarker &marker) const
    {
        unsigned int iFirstRunOnLine;
        if (this->FindFirstRunOnLineStartingFromRun(marker.iRun, iFirstRunOnLine))
        {
            return this->MoveMarkerToFirstCharacterOfRun(marker, iFirstRunOnLine);
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToLastCharacterOfRun(TextMarker &marker, unsigned int iRun) const
    {
        if (iRun < m_runs.GetCount())
        {
            marker.iRun         = iRun;
            marker.iChar        = m_runs[marker.iRun].characterCount;
            marker.relativePosX = m_runs[marker.iRun].width;

            if (marker.iChar > 0)
            {
                // At this point we are located one character past the last character - we need to move it left.
                return this->MoveMarkerLeft(marker);
            }

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToFirstCharacterOfRun(TextMarker &marker, unsigned int iRun) const
    {
        if (iRun < m_runs.GetCount())
        {
            marker.iRun         = iRun;
            marker.iChar        = 0;
            marker.relativePosX = 0;

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToLastCharacterOfPreviousRun(TextMarker &marker) const
    {
        if (marker.iRun > 0)
        {
            return this->MoveMarkerToLastCharacterOfRun(marker, marker.iRun - 1);
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToFirstCharacterOfNextRun(TextMarker &marker) const
    {
        assert(m_runs.GetCount() > 0);

        if (marker.iRun < m_runs.GetCount() - 1)
        {
            return this->MoveMarkerToFirstCharacterOfRun(marker, marker.iRun + 1);
        }
        
        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToCharacter(TextMarker &marker, unsigned int iAbsoluteChar)
    {
        assert(m_runs.GetCount() > 0);

        marker.iRun = this->FindRunAtCharacterIndex(iAbsoluteChar);
        
        assert(marker.iRun < m_runs.GetCount());
        marker.iChar = iAbsoluteChar - m_runs[marker.iRun].iChar;


        // The relative position depends on whether or not the run is a tab character.
        return this->UpdateMarkerRelativePosition(marker);
    }

    bool GUISimpleTextLayout::UpdateMarkerRelativePosition(TextMarker &marker) const
    {
        assert(m_runs.GetCount() > 0);

        const TextRun &run = m_runs[marker.iRun];
        if (m_text.c_str()[run.iChar] == '\t')
        {
            const int tabWidth = this->GetTabWidth();

            if (marker.iChar == 0)
            {
                // Simple case - it's the first tab character which means the relative position is just 0.
                marker.relativePosX = 0;
            }
            else
            {
                marker.relativePosX  = tabWidth * ((run.posX + (tabWidth*(marker.iChar + 0))) / tabWidth);
                marker.relativePosX -= run.posX;
            }

            return true;
        }
        else
        {
            return this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_text.c_str() + m_runs[marker.iRun].iChar, marker.iChar, marker.relativePosX);
        }
    }

    void GUISimpleTextLayout::UpdateMarkerStickyPosition(TextMarker &marker) const
    {
        marker.absoluteSickyPosX = m_runs[marker.iRun].posX + marker.relativePosX;
    }



    bool GUISimpleTextLayout::DeleteCharacterToRightOfMarker(TextMarker &marker)
    {
        TextRun &run = m_runs[marker.iRun];

        unsigned int iAbsoluteMarkerChar = run.iChar + marker.iChar;
        if (iAbsoluteMarkerChar < m_text.GetLengthInTs())
        {
            if (marker.iChar < run.characterCount)
            {
                // Remove the character from the string.
                m_text.EraseCharacterByIndex(run.iChar + marker.iChar);

                // The layout will have changed.
                this->RefreshLayout();


                // The marker needs to be updated based on the new layout.
                this->MoveMarkerToCharacter(marker, iAbsoluteMarkerChar);


                return true;
            }
            else
            {
                // If we get here it means the marker is sitting past the last character in it's run. This is an erroneous case.
                assert(false);
            }
        }

        return false;
    }


    bool GUISimpleTextLayout::GetSelectionMarkers(TextMarker &startOut, TextMarker &endOut) const
    {
        if (this->IsAnythingSelected())
        {
            if (m_selectionAnchor.iRun > m_cursor.iRun || (m_selectionAnchor.iRun == m_cursor.iRun && m_selectionAnchor.iChar > m_cursor.iChar))
            {
                startOut = m_cursor;
                endOut   = m_selectionAnchor;
            }
            else
            {
                startOut = m_selectionAnchor;
                endOut   = m_cursor;
            }

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::HasSpacingBetweenSelectionMarkers() const
    {
        return (m_cursor.iRun != m_selectionAnchor.iRun || m_cursor.iChar != m_selectionAnchor.iChar);
    }


    int GUISimpleTextLayout::GetLineHeight() const
    {
        return this->GetFontManager().GetLineHeight(this->GetDefaultFont());
    }

    int GUISimpleTextLayout::GetSpaceWidth() const
    {
        GUIGlyphMetrics spaceMetrics;
        if (this->GetFontManager().GetGlyphMetrics(m_hFont, ' ', spaceMetrics))
        {
            return spaceMetrics.advance;
        }

        return DEFAULT_SPACE_WIDTH;
    }

    int GUISimpleTextLayout::GetTabWidth() const
    {
        return this->GetTabSizeInSpaces() * this->GetSpaceWidth();
    }
}
