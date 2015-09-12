// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUISimpleTextLayout.hpp>
#include <GTLib/Strings/LineIterator.hpp>
#include <GTLib/Strings/Tokenizer.hpp>

#ifndef OUT
#define OUT
#endif

#ifndef DO_NOTHING
#define DO_NOTHING
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
          m_cursor()
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
    }

    void GUISimpleTextLayout::GetCursorPosition(int &posXOut, int &posYOut) const
    {
        this->GetMarkerPositionRelativeToContainer(m_cursor, posXOut, posYOut);
    }

    bool GUISimpleTextLayout::MoveCursorLeft()
    {
        return this->MoveMarkerLeft(m_cursor);
    }

    bool GUISimpleTextLayout::MoveCursorRight()
    {
        return this->MoveMarkerRight(m_cursor);
    }


    //////////////////////////////
    // Editing

    void GUISimpleTextLayout::InsertCharacterAtCursor(char32_t character)
    {
        switch (character)
        {
        case '\b':
            {
                this->DeleteCharacterToLeftOfCursor();
                break;
            }

        case '\n':
        case '\r':
            {
                this->InsertNewLineAtCursor();
                break;
            }

        case '\t':
            {
                this->InsertTabAtCursor();
                break;
            }

        default:
            {
                // Assume a regular character.
                break;
            }
        }
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

    void GUISimpleTextLayout::InsertNewLineAtCursor()
    {
        // TODO: Implement.
    }

    void GUISimpleTextLayout::InsertTabAtCursor()
    {
        // TODO: Implement.
    }


    //////////////////////////////////////////
    // Private

    void GUISimpleTextLayout::IterateVisibleTextRuns(std::function<void (const GUITextRunDesc &textRunDesc)> func) const
    {
        // This is a naive implementation. Can be improved a bit.
        for (size_t iRun = 0; iRun < m_runs.GetCount(); ++iRun)
        {
            const TextRun &run = m_runs[iRun];

            if (run.characterCount > 0)
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

    bool GUISimpleTextLayout::NextRunString(const char* runStart, const char* &runEnd)
    {
        if (runStart == nullptr || runStart[0] == '\0')
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

            return true;
        }
        else if (firstChar == '\n')
        {
            runStart += 1;

            if (runStart[0] == '\r') {
                runStart += 1;
            }

            runEnd = runStart;
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
        GUIFontManager &fontManager = this->GetFontManager();

        int lineHeight = fontManager.GetLineHeight(m_hFont);
        int tabWidth = 0;

        GUIGlyphMetrics spaceMetrics;
        if (fontManager.GetGlyphMetrics(m_hFont, ' ', spaceMetrics))
        {
            tabWidth = spaceMetrics.advance * this->GetTabSizeInSpaces();
        }


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
        while (this->NextRunString(nextRunStart, OUT nextRunEnd))
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
                run.width = 0;
            }
            else
            {
                // Normal run.
                int runWidth  = 0;
                int runHeight = 0;
                if (fontManager.MeasureString(m_hFont, nextRunStart, run.characterCount, runWidth, runHeight))
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

        
        // We now need to add a run to act as the null terminator.
        assert(nextRunStart[0] == '\0');
        TextRun terminatorRun;
        terminatorRun.iLine          = iCurrentLine;
        terminatorRun.iChar          = nextRunStart - m_text.c_str();
        terminatorRun.iCharEnd       = terminatorRun.iChar;
        terminatorRun.characterCount = 0;
        terminatorRun.width          = 0;
        terminatorRun.height         = lineHeight;
        terminatorRun.posX           = 0;
        terminatorRun.posY           = terminatorRun.iLine * lineHeight;

        if (m_runs.GetCount() > 0)
        {
            TextRun &prevRun = m_runs.GetBack();
            if (prevRun.iLine == iCurrentLine)
            {
                terminatorRun.posX = prevRun.posX + prevRun.width;
            }
        }

        m_runs.PushBack(terminatorRun);




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


    bool GUISimpleTextLayout::InitMarkerByPointRelativeToContainer(int inputPosX, int inputPosY, TextMarker &markerOut) const
    {
        markerOut.iRun         = 0;
        markerOut.iChar        = 0;
        markerOut.relativePosX = 0;

        GTLib::Rect<int> textRect;
        this->GetTextRectRelativeToBounds(textRect);

        int inputPosXRelativeToText = inputPosX - textRect.left;
        int inputPosYRelativeToText = inputPosY - textRect.top;

        unsigned int iClosestRunToPoint;
        if (this->FindClosestRunToPoint(inputPosXRelativeToText, inputPosYRelativeToText, OUT iClosestRunToPoint))
        {
            const TextRun &run = m_runs[iClosestRunToPoint];

            markerOut.iRun = iClosestRunToPoint;

            if (inputPosXRelativeToText < run.posX)
            {
                markerOut.iChar        = 0;
                markerOut.relativePosX = 0;
            }
            else if (inputPosXRelativeToText > run.posX + run.width)
            {
                // It may be a new line run. If so, we need to move the marker to the front of it, not the back.
                markerOut.iChar        = run.characterCount;
                markerOut.relativePosX = run.width;

                if (m_text.c_str()[run.iChar] == '\n') {
                    markerOut.iChar -= 1;
                    return this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_text.c_str() + m_runs[markerOut.iRun].iChar, markerOut.iChar, markerOut.relativePosX);
                }
            }
            else
            {
                int inputPosXRelativeToRun = inputPosX - run.posX;
                if (!this->GetFontManager().GetTextCursorPositionFromPoint(m_hFont, m_text.c_str() + run.iChar, run.characterCount, run.width, inputPosXRelativeToRun, markerOut.relativePosX, markerOut.iChar))
                {
                    // An error occured somehow.
                    return false;
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

                if (inputPosYRelativeToText <= lineBottom)
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


    bool GUISimpleTextLayout::MoveMarkerLeft(TextMarker &marker)
    {
        if (m_runs.GetCount() > 0)
        {
            if (marker.iChar > 0)
            {
                marker.iChar -= 1;
                return this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_text.c_str() + m_runs[marker.iRun].iChar, marker.iChar, marker.relativePosX);
            }
            else
            {
                // We're at the beginning of the run which means we need to transfer the cursor to the end of the previous run.
                return this->MoveMarkerToLastCharacterOfPreviousRun(marker);
            }
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerRight(TextMarker &marker)
    {
        if (m_runs.GetCount() > 0)
        {
            if (marker.iChar + 1 < m_runs[marker.iRun].characterCount)
            {
                marker.iChar += 1;
                return this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_text.c_str() + m_runs[marker.iRun].iChar, marker.iChar, marker.relativePosX);
            }
            else
            {
                // We're at the end of the run which means we need to transfer the cursor to the beginning of the next run.
                return this->MoveMarkerToFirstCharacterOfNextRun(marker);
            }
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToLastCharacterOfPreviousRun(TextMarker &marker)
    {
        if (marker.iRun > 0)
        {
            // The previous run is on the same line.
            marker.iRun        -= 1;
            marker.iChar        = m_runs[marker.iRun].characterCount;
            marker.relativePosX = m_runs[marker.iRun].width;

            if (marker.iChar > 0) {
                marker.iChar -= 1;
                return this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_text.c_str() + m_runs[marker.iRun].iChar, marker.iChar, marker.relativePosX);
            }

            return true;
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToFirstCharacterOfNextRun(TextMarker &marker)
    {
        assert(m_runs.GetCount() > 0);

        if (marker.iRun < m_runs.GetCount() - 1)
        {
            marker.iRun        += 1;
            marker.iChar        = 0;
            marker.relativePosX = 0;

            return true;
        }
        
        return false;
    }

    bool GUISimpleTextLayout::DeleteCharacterToRightOfMarker(TextMarker &marker)
    {
        return false;
    }
}
