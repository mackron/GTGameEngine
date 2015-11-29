// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_StyleScriptError_hpp_
#define __GTLib_GUI_StyleScriptError_hpp_

#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/Strings/Tokenizer.hpp>
#include <GTEngine/Core/Strings/LineIterator.hpp>

namespace GTGUI
{
    struct StyleScriptError
    {
        StyleScriptError()
            : message(), lineString(), lineNumber(0), info(0)
        {
        }

        StyleScriptError(const char* messageIn, const char* lineStringIn, unsigned int lineNumberIn, const char* infoIn)
            : message(messageIn), lineString(lineStringIn), lineNumber(lineNumberIn), info(infoIn)
        {
        }

        StyleScriptError(const StyleScriptError &other)
            : message(other.message), lineString(other.lineString), lineNumber(other.lineNumber), info(other.info)
        {
        }

        StyleScriptError & operator=(const StyleScriptError &other)
        {
            this->message    = other.message;
            this->lineString = other.lineString;
            this->lineNumber = other.lineNumber;
            this->info       = other.info;

            return *this;
        }


        /// Builds and returns a formatted string for output.
        GTLib::String GetFormatted(int level = 1)
        {
            GTLib::String format;
            format += "Styling Error: %s";

            if (level >= 1)
            {
                format += "\n    Line: %d: %s";

                if (level >= 2 && this->info.GetLength() > 0)
                {
                    format += "\n\n";
                    format += "%s";
                }
            }
            

            GTLib::String result;

            if (level >= 2 && this->info.GetLength() > 0)
            {
                // We need to format the info here so it looks nice for output. We want each line to be no longer than 80 characters. We also need to
                // consider that the info string will be indented by 4 spaces.
                //
                // We iterate over each word and check if there is enough room to fit that word into the line. If so, we just insert. Otherwise, we
                // place it onto the next line.
                GTLib::String formattedInfo;

                GTLib::Strings::LineIterator line(this->info.c_str());
                while (line)
                {
                    const char* prevTokenEnd = line.start;
                    GTLib::String currentLine = "    ";

                    GTLib::Strings::Tokenizer token(prevTokenEnd, line.end - prevTokenEnd);
                    while (token)
                    {
                        if (currentLine.GetLengthInTs() + static_cast<size_t>(token.end - prevTokenEnd) <= 76U)
                        {
                            currentLine.Append(prevTokenEnd, (token.end - prevTokenEnd));
                        }
                        else
                        {
                            formattedInfo += currentLine;
                            formattedInfo += "\n";

                            currentLine = "    ";
                            currentLine.Append(prevTokenEnd, token.end - prevTokenEnd);
                        }

                        prevTokenEnd = token.end;
                        ++token;
                    }

                    // The last line needs to be appended here.
                    formattedInfo += currentLine;
                    formattedInfo += "\n";

                    ++line;
                }

                result.AssignFormatted(format.c_str(), this->message.c_str(), this->lineNumber, this->lineString.c_str(), formattedInfo.c_str());
            }
            else if (level >= 1)
            {
                result.AssignFormatted(format.c_str(), this->message.c_str(), this->lineNumber, this->lineString.c_str());
            }
            else
            {
                result.AssignFormatted(format.c_str(), this->message.c_str());
            }

            return result;
        }




        /// The error message.
        GTLib::String message;

        /// The string of the line containing the error.
        GTLib::String lineString;

        /// The number of the line containing the error.
        unsigned int lineNumber;

        /// Additional info contained with this message - usually information about the correct syntax.
        GTLib::String info;
    };
}

#endif
