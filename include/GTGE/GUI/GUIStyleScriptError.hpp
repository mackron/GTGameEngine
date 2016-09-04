// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIStyleScriptError
#define GT_GUIStyleScriptError

namespace GT
{
    struct GUIStyleScriptError
    {
        GUIStyleScriptError()
            : message(), lineString(), lineNumber(0), info(0)
        {
        }

        GUIStyleScriptError(const char* messageIn, const char* lineStringIn, unsigned int lineNumberIn, const char* infoIn)
            : message(messageIn), lineString(lineStringIn), lineNumber(lineNumberIn), info(infoIn)
        {
        }

        GUIStyleScriptError(const GUIStyleScriptError &other)
            : message(other.message), lineString(other.lineString), lineNumber(other.lineNumber), info(other.info)
        {
        }

        GUIStyleScriptError & operator=(const GUIStyleScriptError &other)
        {
            this->message    = other.message;
            this->lineString = other.lineString;
            this->lineNumber = other.lineNumber;
            this->info       = other.info;

            return *this;
        }


        /// Builds and returns a formatted string for output.
        GT::String GetFormatted(int level = 1)
        {
            GT::String format;
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
            

            GT::String result;

            if (level >= 2 && this->info.GetLength() > 0)
            {
                // We need to format the info here so it looks nice for output. We want each line to be no longer than 80 characters. We also need to
                // consider that the info string will be indented by 4 spaces.
                //
                // We iterate over each word and check if there is enough room to fit that word into the line. If so, we just insert. Otherwise, we
                // place it onto the next line.
                GT::String formattedInfo;

                GT::Strings::LineIterator line(this->info.c_str());
                while (line)
                {
                    const char* prevTokenEnd = line.start;
                    GT::String currentLine = "    ";

                    GT::Strings::Tokenizer token(prevTokenEnd, line.end - prevTokenEnd);
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
        GT::String message;

        /// The string of the line containing the error.
        GT::String lineString;

        /// The number of the line containing the error.
        unsigned int lineNumber;

        /// Additional info contained with this message - usually information about the correct syntax.
        GT::String info;
    };
}

#endif
