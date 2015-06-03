// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUISimpleTextLayout.hpp>
#include <GTLib/Strings/LineIterator.hpp>
#include <GTLib/Strings/Tokenizer.hpp>

namespace GT
{
    GUISimpleTextLayout::GUISimpleTextLayout(GUIFontManager &fontManager)
        : GUITextLayout(fontManager),
          m_text(),
          m_boundsWidth(0), m_boundsHeight(0),
          m_offsetX(0), m_offsetY(0),
          m_tabSizeInSpaces(4),
          m_hFont(0),
          m_color(),
          m_lines(),
          m_textBoundsWidth(0), m_textBoundsHeight(0)
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


    void GUISimpleTextLayout::SetBounds(unsigned int width, unsigned int height)
    {
        m_boundsWidth  = width;
        m_boundsHeight = height;
    }

    void GUISimpleTextLayout::GetBounds(unsigned int &widthOut, unsigned int &heightOut) const
    {
        widthOut  = m_boundsWidth;
        heightOut = m_boundsHeight;
    }


    void GUISimpleTextLayout::SetOffset(int offsetX, int offsetY)
    {
        m_offsetX = offsetX;
        m_offsetY = offsetY;
    }

    void GUISimpleTextLayout::GetOffset(int &offsetXOut, int &offsetYOut) const
    {
        offsetXOut = m_offsetX;
        offsetYOut = m_offsetY;
    }


    int GUISimpleTextLayout::GetTextWidth() const
    {
        return m_textBoundsWidth;
    }

    int GUISimpleTextLayout::GetTextHeight() const
    {
        return m_textBoundsHeight;
    }



    //////////////////////////////////////////
    // Font Management

    void GUISimpleTextLayout::SetFont(HGUIFont hFont)
    {
        m_hFont = hFont;

        this->RefreshLayout();
    }

    HGUIFont GUISimpleTextLayout::GetFont() const
    {
        return m_hFont;
    }


    void GUISimpleTextLayout::SetColor(const GTLib::Colour &color)
    {
        m_color = color;
    }

    const GTLib::Colour & GUISimpleTextLayout::GetColor() const
    {
        return m_color;
    }


    void GUISimpleTextLayout::SetTextAndFont(const char* text, HGUIFont hFont)
    {
        m_text  = text;
        m_hFont = hFont;

        this->RefreshLayout();
    }



    //////////////////////////////////////////
    // Private

    void GUISimpleTextLayout::IterateVisibleTextRuns(std::function<void (const GUITextRunDesc &textRunDesc)> func) const
    {
        int lineHeight = this->GetFontManager().GetLineHeight(m_hFont);

        for (size_t iLine = 0; iLine < m_lines.GetCount(); ++iLine)
        {
            auto &line = m_lines[iLine];

            int lineTop    = line.height * static_cast<int>(iLine);
            int lineBottom = lineTop + lineHeight;
            
            if (lineBottom > 0 && lineTop < static_cast<int>(m_boundsHeight))
            {
                int lineLeft  = line.xPos;
                int lineRight = lineLeft + line.width;

                if (lineRight > 0 && lineLeft < static_cast<int>(m_boundsWidth))
                {
                    // The line is visible. Now iterate over each of it's runs.
                    for (size_t iRun = 0; iRun < line.runs.GetCount(); ++iRun)
                    {
                        auto &run = line.runs[iRun];

                        int runLeft  = run.xPos;
                        int runRight = runLeft + run.width;

                        if (runRight > 0 && runLeft < static_cast<int>(m_boundsWidth))
                        {
                            // The run is visible.
                            GUITextRunDesc runDesc;
                            runDesc.text              = GTLib::String(run.textStart, run.textEnd - run.textStart);
                            runDesc.hFont             = m_hFont;
                            runDesc.xPos              = run.xPos;
                            runDesc.yPos              = run.yPos;
                            runDesc.rotationInDegrees = 0;
                            runDesc.color             = m_color;
                            func(runDesc);
                        }
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

    void GUISimpleTextLayout::RefreshLayout()
    {
        // The previous runs need to be removed.
        m_lines.Clear();


        GUIFontManager &fontManager = this->GetFontManager();

        int lineHeight = fontManager.GetLineHeight(m_hFont);
        int tabWidth = 0;

        GUIGlyphMetrics spaceMetrics;
        if (fontManager.GetGlyphMetrics(m_hFont, ' ', spaceMetrics))
        {
            tabWidth = spaceMetrics.advance * this->GetTabSizeInSpaces();
        }

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
            newLine.xPos   = 0;

            GTLib::Strings::Tokenizer8 token(iLine.start, iLine.end - iLine.start, "\t");
            while (token)
            {
                TextRun run;
                run.textStart      = iLine.start;
                run.textEnd        = iLine.end;
                run.characterCount = static_cast<unsigned int>(GTLib::Strings::GetCharacterCount(iLine.start, iLine.end - iLine.start));
                run.xPos           = newLine.width;
                run.yPos           = newLine.height * lineCount;

                int tokenWidth  = 0;
                int tokenHeight = 0;
                fontManager.MeasureString(m_hFont, iLine.start, GTLib::Strings::GetCharacterCount(iLine.start, iLine.end - iLine.start), tokenWidth, tokenHeight);

                run.width = tokenWidth;
                
                
                // Update the width of the line.
                newLine.width += run.width;
                if (token++)
                {
                    // Handle the tab character.
                    newLine.width += (tabWidth - (newLine.width % tabWidth));
                }
                

                newLine.runs.PushBack(run);
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
    }
}
