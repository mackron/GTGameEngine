// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_LineIterator_hpp_
#define __GTLib_LineIterator_hpp_

#include "Find.hpp"
#include "Tokenizer.hpp"
#include <cstring>

namespace GT
{
    namespace Strings
    {
        /// Class for iterating over the lines of a string.
        template <typename T>
        class LineIteratorUTF
        {
        public:

            /// Constructor.
            ///
            /// @param str [in] The string whose lines are being iterated.
            LineIteratorUTF(const T* str)
                : start(str), end(str), nullTerminated(true), remainingTs(-1)
            {
                this->ctor();
            }

            /// Constructor.
            ///
            /// @param str         [in] The string whose lines are being iterator.
            /// @param strSizeInTs [in] The size of 'str' in T's.
            ///
            /// @remarks
            ///     Set 'strSizeInTs' to -1 if the string is null terminated.
            LineIteratorUTF(const T* str, ptrdiff_t strSizeInTs)
                : start(str), end(str), nullTerminated(strSizeInTs == -1), remainingTs(strSizeInTs)
            {
                this->ctor();
            }

            /// Copy constructor.
            LineIteratorUTF(const LineIteratorUTF &other)
                : start(other.start), end(other.end), nullTerminated(other.nullTerminated), remainingTs(other.remainingTs)
            {
            }

            /// Destructor.
            ~LineIteratorUTF()
            {
            }



            ////////////////////////////////////////////////
            // Operators.

            LineIteratorUTF<T> & operator++()
            {
                if (*this)
                {
                    // Step 1: Move the start pointer to the start of the next line.
                    this->start = this->FindStartOfNextLine(this->end, static_cast<size_t>(this->remainingTs));
                    if (this->start != nullptr)
                    {
                        this->remainingTs -= this->start - this->end;

                        // Step 2: Move the end pointer to the end of the current line.
                        this->end = FindFirst(this->start, this->remainingTs, '\n');
                        if (this->end != nullptr)
                        {
                            if (this->end > this->start)
                            {
                                if (*(this->end - 1) == '\r')
                                {
                                    this->end -= 1;
                                }
                            }

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
                        // No more lines.
                        this->end = nullptr;
                    }
                }

                return *this;
            }

            LineIteratorUTF<T> operator++(int)
            {
                LineIteratorUTF<T> temp(*this);
                ++(*this);
                return temp;
            }

            LineIteratorUTF<T> & operator=(const LineIteratorUTF<T> &other)
            {
                if (this != &other)
                {
                    this->start          = other.start;
                    this->end            = other.end;
                    this->nullTerminated = other.nullTerminated;
                    this->remainingTs    = other.remainingTs;
                }

                return *this;
            }

            /// Boolean cast operator.
            operator bool() const
            {
                // If both the start and end are nullptr, the iterator is finished.
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

            /// Finds the start of the next line.
            const T* FindStartOfNextLine(const char* source, size_t sourceSizeInTs)
            {
                if (source != nullptr && sourceSizeInTs > 0 && source[0] != '\0')
                {
                    if (source[0] == '\n')
                    {
                        ++source;
                        --sourceSizeInTs;
                    }
                    else if (source[0] == '\r')
                    {
                        ++source;
                        --sourceSizeInTs;

                        if (sourceSizeInTs > 0 && source[0] == '\n')
                        {
                            ++source;
                            --sourceSizeInTs;
                        }
                    }

                    return source;
                }

                return nullptr;
            }


        public:

            /// A pointer to the start of the current line.
            const T* start;

            /// A pointer to one character past the last character of the current token.
            const T* end;

            /// Keeps track of whether not the string is null terminated.
            bool nullTerminated;

            /// The number of Ts remaining in the string. This is decreased as the iterator is moved forward. When it reaches 0, the iterator
            /// will have reached the end and will not move any more. This is required for non-null terminated strings.
            ptrdiff_t remainingTs;
        };


        typedef LineIteratorUTF<char>     LineIterator;
        typedef LineIteratorUTF<char>     LineIterator8;
        typedef LineIteratorUTF<char16_t> LineIterator16;
        typedef LineIteratorUTF<char32_t> LineIterator32;
    }
}

#endif
