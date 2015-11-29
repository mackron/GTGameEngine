// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __LICKGUI_STYLETOKENIZER_HPP_
#define __LICKGUI_STYLETOKENIZER_HPP_

#include <cstddef>
#include <GTEngine/Core/String.hpp>

#define haszero(v) (((v) - 0x01010101UL) & ~(v) & 0x80808080UL)
#define hasvalue(x,n) (haszero((x) ^ (~0UL/255 * (n))))

#define haszero64(v) (((v) - 0x0101010101010101ULL) & ~(v) & 0x8080808080808080ULL)
#define hasvalue64(x,n) (haszero64((x) ^ (~0ULL/255 * (n))))

namespace GTGUI
{
    /// Class for iterating over each token in a style script.
    ///
    /// The style tokenizer does not make copies of string. Instead is simple returns pointers to the start and end
    /// of the current token. This way we don't waste time doing memory allocations and copying.
    class StyleTokenizer
    {
    public:
    
        /// Constructor.
        ///
        /// @param  script [in] The text being tokenized. Must be null terminated.
        ///
        /// @remarks
        ///     The null terminator is used as the indicator for the end of the script.
        StyleTokenizer(const char *script);
        
        /// Retrieves the size of the token, including the null terminator.
        ///
        /// @remarks
        ///     Use the return value as the minimum size of the buffer used by Copy().
        size_t Size() const;
        
        /// Copies a null terminated copy of the current token into the given buffer.
        ///
        /// @param  dest [in] A pointer to the buffer that will receive the copy.
        ///
        /// @return The number of bytes copied into \c dest, including the null terminator.
        ///
        /// @remarks
        ///     The required size of the buffer can be determined with Size().
        size_t Copy(char *dest) const;
        
        /// Determines if the token is equal to the given string. Case sensitive.
        bool Equals(const char *value) const;
        
        /// Determines if the token is equal to the given character. Case sensitive.
        bool Equals(char value) const;
        
        /// Skips over new-line tokens.
        void SkipNewLines();


        /// Retrieves a string object containing the token.
        GT::String GetString() const;

        /// Builds and returns a string containing the current line.
        ///
        /// @return A string containing the line the tokenizer is currently sitting on.
        GT::String GetCurrentLineString() const;

        /// Retrieves the number of the current line.
        unsigned int GetCurrentLineNumber() const { return this->lineNumber; }
        
        
        /// Operator for moving to the next token.
        ///
        /// return *this;
        StyleTokenizer & operator++();
        StyleTokenizer   operator++(int) { StyleTokenizer temp(*this); ++(*this); return temp; }

        /// Cast operator for checking if there is any other tokens waiting.
        operator bool() const;
        

    private:

        /// Determines whether or not we are in any kind of comment (line or block).
        bool IsInComment() const
        {
            return this->isInLineComment || this->isInBlockComment;
        }

        /// Determines whether or not the next characters are a special symbol
        bool IsSpecialSymbol(char c)
        {
            return c == '{' || c == '}' || c == ':' || c == ';' || c == '(' || c == ')' || c == '\n';
        }
        /*
        bool IsSpecialSymbol(const char* symbolStart, int &lengthOfSymbolInTs)
        {
            assert(symbolStart != nullptr);

            auto c = symbolStart[0];
            if (c == '{' || c == '}' || c == ':' || c == ';' || c == '(' || c == ')' || c == '\n')
            {
                lengthOfSymbolInTs = 1;
                return true;
            }

            return false;
        }

        bool IsSpecialSymbol(const char* symbolStart)
        {
            int devnull;
            return this->IsSpecialSymbol(symbolStart, devnull);
        }
        */
    
    public:
    
        /// A pointer to the start of the current token.
        const char *start;
        
        /// A pointer to the memory address just after the last character of the current token.
        const char *end;

        /// A pointer to the start of the current line.
        const char* lineStart;

        /// The current line number.
        unsigned int lineNumber;


        /// Keeps track of whether or not we are in a line comment.
        bool isInLineComment;

        /// Keeps track of whether or not we are in a block comment.
        bool isInBlockComment;
    };
}

#endif
