// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Strings_Tokenizer_hpp_
#define __GTLib_Strings_Tokenizer_hpp_

#include "Find.hpp"
#include "Size.hpp"
#include "Equal.hpp"
#include <cstring>

namespace GT
{
    namespace Strings
    {
        /**
        *   \brief  Class for iterating over tokens in a string. This is an iterator.
        *
        *   Think of this class in a similar way you would the strtok() function.
        *
        *   This class never makes copies of the string internally. Instead is just stores the start and end pointers of each token. Then,
        *   when required, a null terminated copy can be made. Ignoring any calls to Copy(), the tokenizer will run in roughly O(n) time
        *   from start to finish.
        */
        template <typename T>
        class TokenizerUTF
        {
        public:

            /**
            *   \brief                  Constructor.
            *   \param  str        [in] The string being tokenized.
            *   \param  delimiters [in] A string containing the delimiters to use when tokenizing.
            *
            *   \remarks
            *       A copy is NOT made of either \c str or \c delimiters. Ensure both strings stay valid through the life of the iterator.
            */
            TokenizerUTF(const T* str, const T *delimiters = nullptr)
                : start(str), end(str), nullTerminated(true), remainingTs(-1), delimiters(delimiters)
            {
                this->ctor();
            }

            TokenizerUTF(const T* str, ptrdiff_t strSizeInTs, const char *delimiters = nullptr)
                : start(str), end(str), nullTerminated(strSizeInTs == -1), remainingTs(strSizeInTs), delimiters(delimiters)
            {
                this->ctor();
            }

            TokenizerUTF(const TokenizerUTF<T> &other)
                : start(other.start), end(other.end), nullTerminated(other.nullTerminated), remainingTs(other.remainingTs), delimiters(other.delimiters)
            {
            }
            
            /**
            *   \brief  Destructor.
            */
            ~TokenizerUTF()
            {
            }
            
            /**
            *   \brief  Calculates the size of the token in Ts, not including the null terminator.
            */
            ptrdiff_t GetSizeInTs() const
            {
                // NOTE: Should this check be an assert? Probably.
                if (this->end > this->start)
                {
                    return this->end - this->start;
                }

                return 0;
            }

            
            /**
            *   \brief            Copies a null terminated copy of the current token into the given buffer.
            *   \param  dest [in] A pointer to the buffer that will receive the copy.
            *   \return           The number of bytes copied into \c dest, including the null terminator.
            *
            *   \remarks
            *       The required size of the buffer can be determined with GetSizeInTs(). Note you will need to add extra space for the null terminator.
            */
            ptrdiff_t Copy(T* dest) const
            {
                if (dest != nullptr)
                {
                    ptrdiff_t sizeInTs = this->GetSizeInTs();
                    if (this->start != nullptr && sizeInTs > 0)
                    {
                        std::memcpy(dest, this->start, static_cast<size_t>(sizeInTs) * sizeof(T));
                    }

                    dest[sizeInTs] = '\0';

                    return sizeInTs + 1;
                }

                return 0;
            }
            
            /**
            *   \brief  Checks if the token is equal to another.
            */
            bool Equals(const T* other, ptrdiff_t otherSizeInTs = -1)
            {
                return Strings::Equal(this->start, this->GetSizeInTs(), other, otherSizeInTs);
            }
            
            /**
            *   \brief  Operator for moving to the next token.
            *   \return *this;
            */
            TokenizerUTF<T> & operator++()
            {
                if (*this)
                {
                    // Step 1: Move to the start of the next token.
                    this->start = this->FindFirstNotDelimiter(this->end, this->remainingTs);
                    if (this->start != nullptr)
                    {
                        this->remainingTs -= this->start - this->end;

                        // Step 2: Move the end pointer to the next delimiter (the end of the next token).
                        this->end = this->FindFirstDelimiter(this->start, this->remainingTs);
                        if (this->end != nullptr)
                        {
                            this->remainingTs -= this->end - this->start;
                        }
                        else
                        {
                            // We have found the end. In this situation, we need to either move to the null terminator, or
                            // move forward by the remaining number of bytes.
                            if (this->nullTerminated)
                            {
                                this->end = this->start + Strings::SizeInTs(this->start);
                            }
                            else
                            {
                                this->end = this->start + this->remainingTs;
                            }
                        
                            this->remainingTs -= this->end - this->start;
                        }
                    }
                    else
                    {
                        // No more tokens.
                        this->end = nullptr;
                    }
                }

                return *this;
            }

            TokenizerUTF<T> operator++(int)
            {
                TokenizerUTF<T> temp(*this);
                ++(*this);
                return temp;
            }
            
            TokenizerUTF<T> & operator=(const TokenizerUTF<T> &other)
            {
                if (this != &other)
                {
                    this->start          = other.start;
                    this->end            = other.end;
                    this->nullTerminated = other.nullTerminated;
                    this->remainingTs    = other.remainingTs;
                    this->delimiters     = other.delimiters;
                }
            
                return *this;
            }

            /**
            *   \brief  Cast operator for checking if there is any other tokens waiting.
            */
            operator bool() const
            {
                // If both the start and end are nullptr, the tokenizer is finished.
                return !(this->start == nullptr && this->end == nullptr);
            }
            
            
    
        private:
        
            /// Constructor implementation.
            void ctor()
            {
                if (this->start != nullptr)
                {
                    // We just move to the "next" token, which will just be the first one.
                    ++(*this);
                }
            }

            /// Finds the first occurance of the delimiter.
            const T* FindFirstDelimiter(const T* source, ptrdiff_t sourceSizeInTs)
            {
                if (this->delimiters != nullptr)
                {
                    return FindFirstOf(source, sourceSizeInTs, this->delimiters);
                }
                else
                {
                    return FindFirstWhitespace(source, sourceSizeInTs);
                }
            }

            /// Finds the first occurances of a non-delimiter.
            const T* FindFirstNotDelimiter(const T* source, ptrdiff_t sourceSizeInTs)
            {
                if (this->delimiters != nullptr)
                {
                    return FindFirstNotOf(source, sourceSizeInTs, this->delimiters);
                }
                else
                {
                    return FindFirstNotWhitespace(source, sourceSizeInTs);
                }
            }


        public:

            /// A pointer to the start of the current token.
            const T* start;
            
            /// A pointer to one character past the last character of the current token.
            const T* end;
            
            /// Keeps track of whether not the string is null terminated.
            bool nullTerminated;
            
            /// The number of Ts remaining in the string being tokenized. This is decreased as the iterator is moved forward.
            /// When it reaches 0, the iterator will have reached the end and will not move any more. This is required for non-
            /// null terminated strings.
            ptrdiff_t remainingTs;
            
            /// The delimiters. If this is null, we'll use whitespaces.
            const T* delimiters;
        };


        typedef TokenizerUTF<char>     Tokenizer;
        typedef TokenizerUTF<char>     Tokenizer8;
        typedef TokenizerUTF<char16_t> Tokenizer16;
        typedef TokenizerUTF<char32_t> Tokenizer32;
        
        
        
        
        /// An optimized tokenizer for splitting a UTF-8 string by whitespace.
        ///
        /// @remarks
        ///     This class assumes the string is well formed and valid.
        class WhitespaceTokenizerUTF8
        {
        public:
            
            /// Constructor.
            WhitespaceTokenizerUTF8(const char* str)
                : start(str), end(str), inputStrEnd(nullptr)
            {
                ++(*this);
            }
            
            /// Constructor.
            WhitespaceTokenizerUTF8(const char* str, ptrdiff_t strSizeInBytes)
                : start(str), end(str), inputStrEnd((strSizeInBytes != -1) ? str + strSizeInBytes : nullptr)
            {
                ++(*this);
            }
            
            /// Copy constructor.
            WhitespaceTokenizerUTF8(const WhitespaceTokenizerUTF8 &other)
                : start(other.start), end(other.end), inputStrEnd(other.inputStrEnd)
            {
            }
            
            
            /// Gets the size of the current token in bytes, not including the null terminator.
            size_t GetSizeInBytes() const
            {
                assert(this->end >= this->start);
                
                return this->end - this->start;
            }
            
            /// The same as GetSizeInBytes(). This is only used for compatibility with TokenizerUTF.
            size_t GetSizeInTs() const { return this->GetSizeInBytes(); }
            
            
            /// Copies a null-terminated copy of the current token to the given buffer.
            ///
            /// @param dest [in] A pointer to the destination buffer.
            ///
            /// @return The number of bytes copied over to the buffer, including the null terminator.
            ///
            /// @remarks
            ///     The buffer will be null terminated.
            ptrdiff_t Copy(char* dest) const
            {
                if (dest != nullptr)
                {
                    ptrdiff_t sizeInTs = this->GetSizeInBytes();
                    if (this->start != nullptr && sizeInTs > 0)
                    {
                        std::memcpy(dest, this->start, static_cast<size_t>(sizeInTs));
                    }

                    dest[sizeInTs] = '\0';

                    return sizeInTs + 1;
                }

                return 0;
            }
            
            
            /// Checks if the current token is equal to the given string.
            bool Equals(const char* other, ptrdiff_t otherSizeInTs = -1)
            {
                return Strings::Equal(this->start, this->GetSizeInBytes(), other, otherSizeInTs);
            }
            
            
            /// Moves to the next token.
            WhitespaceTokenizerUTF8 & MoveToNextToken()
            {
                if (*this)
                {
                    // Step 1: Move to the start of the next token.
                    this->start = this->FindFirstNotWhitespace(this->end);
                    if (this->start != nullptr)
                    {
                        // Step 2: Move the end pointer to the next delimiter (the end of the next token).
                        this->end = this->FindFirstWhitespace(this->start);
                    }
                    else
                    {
                        // No more tokens.
                        this->end = nullptr;
                    }
                }
                
                return *this;
            }
            
            
            
            /// Pre-increment overload.
            WhitespaceTokenizerUTF8 & operator++()
            {
                return this->MoveToNextToken();
            }
            
            /// Post-increment overload.
            WhitespaceTokenizerUTF8 operator++(int)
            {
                WhitespaceTokenizerUTF8 temp(*this);
                ++(*this);
                return temp;
            }
            
            
            /// bool casting operator.
            operator bool() const
            {
                return this->start != nullptr;
            }
            
            
        private:
            
            /// Finds the first occurance of a character that is not whitespace.
            ///
            /// @remarks
            ///     If the token is not found, null is returned.
            const char* FindFirstNotWhitespace(const char* source)
            {
                while (source[0] != '\0' && (this->inputStrEnd == nullptr || source < this->inputStrEnd))
                {
                    size_t increment = Strings::GetWhitespaceSize(source);
                    if (increment == 0)
                    {
                        return source;
                    }
                    else
                    {
                        source += increment;
                    }
                }
                
                return nullptr;
            }
            
            /// Finds the next occurances of a whitespace character.
            ///
            /// @remarks
            ///     If a whitespace character is not found, a pointer to the very end of the string is returned.
            const char* FindFirstWhitespace(const char* source)
            {
                while (source[0] != '\0' && (this->inputStrEnd == nullptr || source < this->inputStrEnd))
                {
                    size_t increment = Strings::GetWhitespaceSize(source);
                    if (increment == 0)
                    {
                        source += 1;
                    }
                    else
                    {
                        return source;
                    }
                }
                
                return source;
            }
            
            
            
            
        public:
            
            /// A pointer to the start of the current token.
            const char* start;
            
            /// A pointer to the end of the current token.
            const char* end;
            
            
        private:
            
            /// A pointer to the end of the main string being tokenized.
            const char* inputStrEnd;
        };
    }
}

#endif
