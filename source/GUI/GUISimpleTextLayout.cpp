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
          m_lines(),
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
#if 0
        int lineHeight = this->GetFontManager().GetLineHeight(m_hFont);
        int topOffset  = 0; //this->GetFontManager().GetAscent(m_hFont); // - lineHeight;

        for (size_t iLine = 0; iLine < m_lines.GetCount(); ++iLine)
        {
            auto &line = m_lines[iLine];

            int lineTop    = line.height * static_cast<int>(iLine) + m_containerInnerOffsetY + topOffset;
            int lineBottom = lineTop + lineHeight;
            
            if (lineBottom > 0 && lineTop < static_cast<int>(m_containerBoundsHeight))
            {
                int lineLeft  = line.alignmentOffsetX + m_containerInnerOffsetX;
                int lineRight = lineLeft + line.width;

                if (lineRight > 0 && lineLeft < static_cast<int>(m_containerBoundsWidth))
                {
                    // The line is visible. Now iterate over each of it's runs.
                    for (size_t iRun = 0; iRun < line.runs.GetCount(); ++iRun)
                    {
                        auto &run = line.runs[iRun];

                        int runLeft  = lineLeft + run.xPos;
                        int runRight = runLeft + run.width;

                        if (runRight > 0 && runLeft < static_cast<int>(m_containerBoundsWidth))
                        {
                            // The run is visible.
                            int runTop = run.yPos + line.alignmentOffsetY + topOffset;

                            GUITextRunDesc runDesc;
                            GTLib::Strings::Copy(runDesc.text, GT_MAX_TEXT_RUN_SIZE_IN_BYTES, run.textStart, run.textEnd - run.textStart);
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
#endif

        // This is a naive implementation. Can be improved a bit.
        for (size_t iRun = 0; iRun < m_runs.GetCount(); ++iRun)
        {
            const TextRun2 &run = m_runs[iRun];

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



        /////////////////////////////////////////////////////
        // OLD Implementation

        // The previous runs need to be removed.
        m_lines.Clear();

        int textBoundsWidth  = 0;
        int textBoundsHeight = 0;

        // We split by lines and then again by tabs.
        int lineCount = 0;
        GTLib::Strings::LineIterator iLine(m_text.c_str());
        while (iLine)
        {
            TextLine newLine;
            newLine.width  = 0;
            newLine.height = lineHeight;
            newLine.alignmentOffsetX = 0;
            newLine.alignmentOffsetY = 0;

            TextRun currentRun;
            currentRun.textStart      = iLine.start;
            currentRun.textEnd        = nullptr;
            currentRun.characterCount = 0;
            currentRun.width          = 0;
            currentRun.xPos           = 0;
            currentRun.yPos           = 0;

            // We need to split the string based on tabs.
            GTLib::Strings::Iterator<char> c(iLine.start, iLine.end - iLine.start);
            while (c)
            {
                bool endRunEarly = ((c.str - currentRun.textStart) == (GT_MAX_TEXT_RUN_SIZE_IN_BYTES - 1));

                if (c.character == '\t' || endRunEarly)
                {
                    // If we are in the middle of a run it needs to be ended.
                    if (currentRun.textStart != nullptr)
                    {
                        // End the run and add it to the list of runs.
                        {
                            currentRun.textEnd = c.str;
                            currentRun.xPos    = newLine.width;
                            currentRun.yPos    = newLine.height * lineCount;

                            int runWidth  = 0;
                            int runHeight = 0;
                            if (fontManager.MeasureString(m_hFont, currentRun.textStart, currentRun.characterCount, runWidth, runHeight))
                            {
                                currentRun.width = runWidth;
                                newLine.width += runWidth;
                            }


                            newLine.runs.PushBack(currentRun);
                        }
                        

                        // Reset the run.
                        {
                            currentRun.textStart      = nullptr;
                            currentRun.textEnd        = nullptr;
                            currentRun.characterCount = 0;
                            currentRun.width          = 0;
                            currentRun.xPos           = 0;
                            currentRun.yPos           = 0;
                        }
                    }

                    
                    // Increment the tab size.
                    if (c.character == '\t')
                    {
                        newLine.width += (tabWidth - (newLine.width % tabWidth));
                    }
                }
                else
                {
                    // It's a normal character. If we are in the middle of a run we just continue iterating. Otherwise we begin a new one. The run cannot be longer than GT_MAX_TEXT_RUN_SIZE_IN_BYTES.
                    if (currentRun.textStart == nullptr)
                    {
                        currentRun.textStart = c.str;
                    }

                    currentRun.characterCount += 1;
                }


                // If we ended the run early, we don't want to go to the next character. We instead want to stay where we are so that the beginning
                // of the next run starts at the end of the last one, rather than the character after.
                if (!endRunEarly)
                {
                    ++c;
                }
            }

            // There might be a run that needs to be added.
            if (currentRun.textStart != nullptr)
            {
                currentRun.textEnd = c.str;
                currentRun.xPos    = newLine.width;
                currentRun.yPos    = newLine.height * lineCount;

                int runWidth  = 0;
                int runHeight = 0;
                if (fontManager.MeasureString(m_hFont, currentRun.textStart, currentRun.characterCount, runWidth, runHeight))
                {
                    currentRun.width = runWidth;
                    newLine.width += runWidth;
                }


                newLine.runs.PushBack(currentRun);
            }



            if (textBoundsWidth < newLine.width)
            {
                textBoundsWidth = newLine.width;
            }

            textBoundsHeight += newLine.height;



            m_lines.PushBack(newLine);

            lineCount += 1;
            ++iLine;
        }


        m_textBoundsWidth  = textBoundsWidth;
        m_textBoundsHeight = textBoundsHeight;



        /////////////////////////////////////////////////////
        // New Implementation

        // We split the runs based on tabs and new-lines. We want to create runs for tabs and new-line characters as well because we want
        // to have the entire string covered by runs for the sake of simplicity when it comes to editing.
        //
        // The first pass positions the runs based on a top-to-bottom, left-to-right alignment. The second pass then repositions the runs
        // based on alignment.
        //m_textBoundsWidth  = 0;
        //m_textBoundsHeight = 0;

        // The previous runs need to be removed.
        m_runs.Clear();

        unsigned int iCurrentLine = 0;

        const char* nextRunStart = m_text.c_str();
        const char* nextRunEnd;
        while (this->NextRunString(nextRunStart, OUT nextRunEnd))
        {
            TextRun2 run;
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
                TextRun2 &prevRun = m_runs.GetBack();
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




        // If we were to return now the text would be alignment top/left. If the alignment is not top/left we need to refresh the layout.
        if (m_horizontalAlignment != GUITextLayoutHorizontalAlignment::Left || m_verticalAlignment != GUITextLayoutVerticalAlignment::Top)
        {
            this->RefreshAlignment();
        }
    }


    void GUISimpleTextLayout::RefreshAlignment()
    {
        /////////////////////////////////////////////////////
        // OLD Implementation

        for (size_t iLine = 0; iLine < m_lines.GetCount(); ++iLine)
        {
            auto &line = m_lines[iLine];

            switch (m_horizontalAlignment)
            {
            case GUITextLayoutHorizontalAlignment::Right:
                {
                    line.alignmentOffsetX = int(m_containerBoundsWidth) - line.width;
                    break;
                }

            case GUITextLayoutHorizontalAlignment::Center:
                {
                    line.alignmentOffsetX = (int(m_containerBoundsWidth) - line.width) / 2;
                    break;
                }

            case GUITextLayoutHorizontalAlignment::Left:
            default:
                {
                    line.alignmentOffsetX = 0;
                    break;
                }
            }


            switch (m_verticalAlignment)
            {
            case GUITextLayoutVerticalAlignment::Bottom:
                {
                    line.alignmentOffsetY = int(m_containerBoundsHeight) - m_textBoundsHeight;
                    break;
                }

            case GUITextLayoutVerticalAlignment::Center:
                {
                    line.alignmentOffsetY = (int(m_containerBoundsHeight) - m_textBoundsHeight) / 2;
                    break;
                }

            case GUITextLayoutVerticalAlignment::Top:
            default:
                {
                    line.alignmentOffsetY = 0;
                    break;
                }
            }
        }




        /////////////////////////////////////////////////////
        // New Implementation

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
                TextRun2 &run = m_runs[jRun];
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
                TextRun2 &run = m_runs[iRun];
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



    void GUISimpleTextLayout::MakeRelativeToContainer(int inputPosX, int inputPosY, int &posXOut, int &posYOut) const
    {
        posXOut = inputPosX;
        posYOut = inputPosY;

        switch (m_horizontalAlignment)
        {
        case GUITextLayoutHorizontalAlignment::Right:
            {
                posXOut += m_containerBoundsWidth - m_textBoundsWidth;
                break;
            }

        case GUITextLayoutHorizontalAlignment::Center:
            {
                posXOut += (m_containerBoundsWidth - m_textBoundsWidth) / 2;
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
                posYOut += m_containerBoundsHeight - m_textBoundsHeight;
                break;
            }

        case GUITextLayoutVerticalAlignment::Center:
            {
                posYOut += (m_containerBoundsHeight - m_textBoundsHeight) / 2;
                break;
            }

        case GUITextLayoutVerticalAlignment::Top:
        default:
            {
                break;
            }
        }

        posXOut += m_containerInnerOffsetX;
        posYOut += m_containerInnerOffsetY;
    }

    void GUISimpleTextLayout::MakeRelativeToTextBounds(int inputPosX, int inputPosY, int &posXOut, int &posYOut) const
    {
        // TODO: Implement me.
        posXOut = inputPosX;
        posYOut = inputPosY;
    }


    bool GUISimpleTextLayout::InitMarkerByPointRelativeToContainer(int inputPosX, int inputPosY, TextMarker &markerOut) const
    {
        markerOut.iLine        = 0;
        markerOut.iRun         = 0;
        markerOut.iChar        = 0;
        markerOut.relativePosX = 0;

        if (m_lines.GetCount() > 0)
        {
            // Y axis.
            const TextLine* pLine = nullptr;

            int lineHeight = this->GetFontManager().GetLineHeight(m_hFont);
            for (unsigned int iLine = 0; iLine < static_cast<unsigned int>(m_lines.GetCount()); ++iLine)
            {
                pLine = &m_lines[iLine];
                markerOut.iLine = iLine;

                int lineTop    = pLine->height * int(markerOut.iLine) + m_containerInnerOffsetY + pLine->alignmentOffsetY;
                int lineBottom = lineTop + lineHeight;

                if (inputPosY >= lineTop)
                {
                    if (inputPosY <= lineBottom)
                    {
                        break;
                    }
                }
                else
                {
                    assert(markerOut.iLine == 0);
                    break;
                }
            }


            // X axis.
            assert(pLine != nullptr);

            int lineLeft  = pLine->alignmentOffsetX + m_containerInnerOffsetX;
            int lineRight = lineLeft + pLine->width;

            if (inputPosX < lineLeft)
            {
                markerOut.iRun         = 0;
                markerOut.iChar        = 0;
                markerOut.relativePosX = 0;
            }
            else if (inputPosX > lineRight)
            {
                markerOut.iRun         = pLine->runs.GetCount() - 1;
                markerOut.iChar        = pLine->runs.GetBack().characterCount;
                markerOut.relativePosX = pLine->runs.GetBack().width;
            }
            else
            {
                for (unsigned int iRun = 0; iRun < static_cast<unsigned int>(pLine->runs.GetCount()); ++iRun)
                {
                    const TextRun &run = pLine->runs[iRun];

                    int runLeft  = run.xPos;
                    int runRight = run.xPos + run.width;

                    if (iRun > 0)
                    {
                        const TextRun &prevRun = pLine->runs[iRun - 1];

                        const int spaceBetweenRuns = run.xPos - (prevRun.xPos + prevRun.width);
                        runLeft -= int(GTLib::Round(spaceBetweenRuns / 2.0f));
                    }

                    if (iRun < pLine->runs.GetCount() - 1)
                    {
                        const TextRun &nextRun = pLine->runs[iRun + 1];
                        
                        const int spaceBetweenRuns = nextRun.xPos - (run.xPos + run.width);
                        runRight += int(GTLib::Round(spaceBetweenRuns / 2.0f));
                    }



                    if (inputPosX >= runLeft && inputPosX <= runRight)
                    {
                        // It is somewhere on this run.
                        markerOut.iRun = iRun;

                        int inputPosXRelativeToRun = inputPosX - run.xPos;
                        if (inputPosXRelativeToRun <= 0)
                        {
                            // It's in the spacing between this run and the previous one, but closer to this one. Set it to the position of the first character in the run.
                            markerOut.iChar        = 0;
                            markerOut.relativePosX = 0;
                        }
                        else if (inputPosXRelativeToRun >= run.xPos + int(run.width))
                        {
                            // It's in the spaceing between this run and the next one, but closer to this one. Set it to the position of the last character in this run.
                            markerOut.iChar        = pLine->runs[iRun].characterCount;
                            markerOut.relativePosX = pLine->runs[iRun].width;
                        }
                        else
                        {
                            if (!this->GetFontManager().GetTextCursorPositionFromPoint(m_hFont, run.textStart, run.characterCount, run.width, inputPosXRelativeToRun, markerOut.relativePosX, markerOut.iChar))
                            {
                                // An error occured somehow.
                                return false;
                            }
                        }

                        break;
                    }
                }
            }
        }
        
        return true;
    }

    void GUISimpleTextLayout::GetMarkerPositionRelativeToContainer(const TextMarker &marker, int &posXOut, int &posYOut) const
    {
        posXOut = 0;
        posYOut = 0;

        if (marker.iLine < m_lines.GetCount())
        {
            posYOut = m_lines[marker.iLine].height * int(marker.iLine) + m_containerInnerOffsetY + m_lines[marker.iLine].alignmentOffsetY;

            if (marker.iRun < m_lines[marker.iLine].runs.GetCount())
            {
                posXOut = m_lines[marker.iLine].runs[marker.iRun].xPos + m_containerInnerOffsetX + m_lines[marker.iLine].alignmentOffsetX + marker.relativePosX;
            }
        }
    }


    bool GUISimpleTextLayout::MoveMarkerLeft(TextMarker &marker)
    {
        if (m_lines.GetCount() > 0)
        {
            if (marker.iChar > 0)
            {
                marker.iChar -= 1;
                return this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_lines[marker.iLine].runs[marker.iRun].textStart, marker.iChar, marker.relativePosX);
            }
            else
            {
                // We're at the beginning of the run which means we need to transfer the cursor to the end of the previous run.
                return this->MoveMarkerToEndOfPreviousRun(marker);
            }
        }
        
        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerRight(TextMarker &marker)
    {
        if (m_lines.GetCount() > 0)
        {
            if (marker.iChar < m_lines[marker.iLine].runs[marker.iRun].characterCount)
            {
                marker.iChar += 1;
                return this->GetFontManager().GetTextCursorPositionFromCharacter(this->GetDefaultFont(), m_lines[marker.iLine].runs[marker.iRun].textStart, marker.iChar, marker.relativePosX);
            }
            else
            {
                // We're at the end of the run which means we need to transfer the cursor to the beginning of the next run.
                return this->MoveMarkerToStartOfNextRun(marker);
            }
        }

        return false;
    }

    bool GUISimpleTextLayout::MoveMarkerToEndOfPreviousRun(TextMarker &marker)
    {
        if (marker.iRun > 0)
        {
            // The previous run is on the same line.
            marker.iRun        -= 1;
            marker.iChar        = m_lines[marker.iLine].runs[marker.iRun].characterCount;
            marker.relativePosX = m_lines[marker.iLine].runs[marker.iRun].width;

            return true;
        }
        else
        {
            // The previous run is on the previous line.
            return this->MoveMarkerToEndOfPreviousLine(marker);
        }
    }

    bool GUISimpleTextLayout::MoveMarkerToStartOfNextRun(TextMarker &marker)
    {
        assert(m_lines[marker.iLine].runs.GetCount() > 0);

        if (marker.iRun < m_lines[marker.iLine].runs.GetCount() - 1)
        {
            // The next run is on the same line.
            marker.iRun        += 1;
            marker.iChar        = 0;
            marker.relativePosX = 0;

            return true;
        }
        else
        {
            // The next run is on the next line.
            return this->MoveMarkerToStartOfNextLine(marker);
        }
    }

    bool GUISimpleTextLayout::MoveMarkerToEndOfPreviousLine(TextMarker &marker)
    {
        if (marker.iLine > 0)
        {
            marker.iLine       -= 1;
            marker.iRun         = m_lines[marker.iLine].runs.GetCount() - 1;
            marker.iChar        = m_lines[marker.iLine].runs[marker.iRun].characterCount;
            marker.relativePosX = m_lines[marker.iLine].runs[marker.iRun].width;

            return true;
        }
        else
        {
            // There is no previous line.
            return false;
        }
    }

    bool GUISimpleTextLayout::MoveMarkerToStartOfNextLine(TextMarker &marker)
    {
        if (m_lines.GetCount() > 0)
        {
            if (marker.iLine < m_lines.GetCount() - 1)
            {
                marker.iLine       += 1;
                marker.iRun         = 0;
                marker.iChar        = 0;
                marker.relativePosX = 0;

                return true;
            }
            else
            {
                // There is no next line.
                return false;
            }
        }
        else
        {
            // There are no lones at all.
            return false;
        }
    }

    bool GUISimpleTextLayout::DeleteCharacterToRightOfMarker(TextMarker &marker)
    {
        if (m_lines.GetCount() > 0)
        {
            if (marker.iChar < m_lines[marker.iLine].runs[marker.iRun].characterCount)
            {
                // It's not at the end of the run.

                
            }
            else
            {
                // It's at the end of the run.
                assert(m_lines[marker.iLine].runs.GetCount() > 0);

                if (marker.iRun < m_lines[marker.iLine].runs.GetCount() - 1)
                {
                    // It's not the last run on the line. If the character to the right is a tab character, it needs to be removed. If there is another tab character to the
                    // right, then the runs should be left unmerged. Otherwise, they need to be merged.

                    
                }
                else
                {
                    // It's 
                }

                if (marker.iLine < m_lines.GetCount() - 1)
                {
                    // It's at the very end of 
                }
            }
        }

        return false;
    }
}
