// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include "StyleTokenizer.hpp"
#include <GTEngine/Core/Strings/Equal.hpp>
#include <GTEngine/Core/Strings/Find.hpp>
#include <cstring>

namespace GTGUI
{
    StyleTokenizer::StyleTokenizer(const char *script)
        : start(script), end(script), lineStart(script), lineNumber(1),
          isInLineComment(false), isInBlockComment(false)
    {
        // We need to move the first token, which can be done by just incrementing.
        ++(*this);
    }
    
    size_t StyleTokenizer::Size() const
    {
        if (this->end > this->start)
        {
            return static_cast<size_t>(this->end - this->start) + 1; // +1 for null terminator.
        }
        
        return 1;   // Enough room for an empty string with only a null terminator.
    }
    
    size_t StyleTokenizer::Copy(char *dest) const
    {
        if (dest)
        {
            size_t length = static_cast<size_t>(this->end - this->start);
            if (length >= 1)
            {
                if (this->start)
                {
                    std::memcpy(dest, this->start, length);
                }
                
                dest[length] = '\0';
            }
            
            return length + 1;
        }
        
        return 0;
    }
    
    bool StyleTokenizer::Equals(const char *value) const
    {
        if (*this)
        {
            return GT::Strings::Equal(this->start, this->end - this->start, value, -1);
        }

        return false;
    }
    
    bool StyleTokenizer::Equals(char value) const
    {
        if (*this)
        {
            if (this->end == this->start + 1)
            {
                return *start == value;
            }
        }
        
        return false;
    }
    
    void StyleTokenizer::SkipNewLines()
    {
        while (*this)
        {
            if (!this->Equals('\n'))
            {
                break;
            }
            
            ++(*this);
        }
    }


    GT::String StyleTokenizer::GetString() const
    {
        return GT::String(this->start, this->end - this->start);
    }

    GT::String StyleTokenizer::GetCurrentLineString() const
    {
        // We just find the first occurance of the \n or \0 character.
        if (this->lineStart != nullptr)
        {
            auto lineEnd = GT::Strings::FindFirst(this->lineStart, '\n');
            if (lineEnd != nullptr)
            {
                return GT::String(this->lineStart, lineEnd - this->lineStart).Trim();
            }
            else
            {
                return GT::String(this->lineStart).Trim();
            }
        }

        return "";
    }
    
    StyleTokenizer & StyleTokenizer::operator++()
    {
        if (*this)
        {
            // Step 1: Move to the start of the next token. We need to skip over any comments.

            this->start = this->end;
            while (this->start[0] != '\0')
            {
                // Check for a new line, but don't skip over the \n character - we need that character to determine whether or not we've ended a line comment.
                if (this->start[0] == '\n')
                {
                    this->lineNumber += 1;
                    
                    if (this->isInLineComment)
                    {
                        this->isInLineComment = false;
                        this->start += 1;
                    }
                }
                else if (this->isInBlockComment && (this->start[0] == '*' && this->start[1] == '/'))
                {
                    this->isInBlockComment = false;
                    this->start += 2;
                }


                if (!this->IsInComment())
                {
                    // If the next token is a special symbol, we will do an early termination.
                    if (this->IsSpecialSymbol(this->start[0]))
                    {
                        this->end = this->start + 1;
                        return *this;
                    }

                    // If we get here, it means we are looking at a normal word that is not a symbol or the start of a comment block.
                    if (!GT::Strings::IsWhitespace(static_cast<char32_t>(this->start[0])))
                    {
                        // Might be a comment block.
                        if (this->start[0] == '/')     
                        {
                            if (this->start[1] == '/')
                            {
                                this->isInLineComment = true;
                            }
                            else if (this->start[1] == '*')
                            {
                                this->isInBlockComment = true;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }


                this->start += 1;
            }



            // Step 2: Move to the end of the token. The end of the token is the position of the first occurance of a whitespace character, a specialized symbol,
            //         or the beginning of a comment block.

            if (this->start[0] != '\0')
            {
                this->end = this->start + 1;
            
                while (this->end[0] != '\0')
                {
                    if (this->IsSpecialSymbol(this->end[0]))
                    {
                        break;
                    }
                    else if (GT::Strings::IsWhitespace(static_cast<char32_t>(this->end[0])))    // Whitespace
                    {
                        break;
                    }
                    else if (this->end[0] == '/' && (this->end[1] == '/' || this->end[1] == '*'))   // Comment block
                    {
                        break;
                    }

                    this->end += 1;
                }
            }
            else
            {
                this->start = nullptr;
                this->end   = nullptr;
            }
        }

        return *this;
    }
    
    StyleTokenizer::operator bool() const
    {
        // If both the start and end are null, the tokenizer is finished.
        return !(this->start == nullptr && this->end == nullptr);
    }
}
