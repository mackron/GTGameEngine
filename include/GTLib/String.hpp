// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_String
#define GT_String

#include "Strings/Create.hpp"
#include "Strings/Equal.hpp"
#include "Strings/List.hpp"
#include "Strings/Size.hpp"
#include "Strings/Trim.hpp"
#include <cassert>
#include <locale>

namespace GTLib
{
    /// Class representing a basic UTF string.
    ///
    /// We've decided to favour simplicity and ease-of-implementation over speed with this class. We simply reuse low-level, c-style functions
    /// that are already implemented by GTLib::Strings. The result is a much simpler and easier-to-maintain implementation. The main
    /// consequence of this is that the internal buffer will be reallocated whenever the string is modified. Complex concatinations will be
    /// slow with this class. Consider GTLib::Strings::List for doing complex concatinations.
    ///
    /// Unlike std::string, it is perfectly OK to specify a null pointer as the input string for an assignment. In this case, it will simply
    /// use an empty string as the value.
    ///
    /// This class also supports inter-encoding operations. For example, a UTF-8 string (String8) can be compared against a UTF-16 string
    /// (String16) completely seamlessly such as: 'someUTF8String == someUTF16String'.
    template <typename T>
    class BasicStringUTF
    {
    public:

        /// Constructor.
        BasicStringUTF()
            : data(nullptr), lengthInTs(0)
        {
        }

        BasicStringUTF(const BasicStringUTF<T> &other)
            : data(Strings::Create(other.c_str(), static_cast<ptrdiff_t>(other.GetLengthInTs()))), lengthInTs(other.GetLengthInTs())
        {
        }

        BasicStringUTF(const T* strIn, ptrdiff_t lengthInTsIn = -1)
            : data(Strings::Create(strIn, lengthInTsIn)), lengthInTs(static_cast<size_t>(Strings::SizeInTs(data)))
        {
        }

        template <typename U>
        BasicStringUTF(const U* strIn, ptrdiff_t lengthInUsIn = -1)
            : data(Strings::Create<T>(strIn, lengthInUsIn)), lengthInTs(static_cast<size_t>(Strings::SizeInTs(data)))
        {
        }

        /// Destructor.
        ~BasicStringUTF()
        {
            Strings::Delete(this->data);
        }


        /// Retrieves the length of the string in T's. Note this function simply returns the number of T's making up the string, and
        /// not neccessarilly the number of characters.
        size_t GetLengthInTs() const
        {
            return this->lengthInTs;
        }

        /// The same as GetLengthInTs(), for those who don't like the "InTs" syntax.
        size_t GetLength() const { return this->GetLengthInTs(); }


        /// Retrieves the number of characters in the string.
        ///
        /// This runs in linear time.
        size_t GetCharacterCount() const
        {
            return Strings::GetCharacterCount(this->data, static_cast<ptrdiff_t>(this->lengthInTs));
        }


        /// Sets the value of the string.
        BasicStringUTF<T> & Assign(const BasicStringUTF<T> &other)
        {
            if (this != &other)
            {
                if (this->data != nullptr && this->lengthInTs >= other.GetLengthInTs())
                {
                    Strings::Copy(this->data, other.c_str(), static_cast<ptrdiff_t>(other.GetLengthInTs()));
                }
                else
                {
                    Strings::Delete(this->data);
                    this->data = Strings::Create(other.c_str(), static_cast<ptrdiff_t>(other.GetLengthInTs()));
                }

                this->lengthInTs = other.GetLengthInTs();
            }

            return *this;
        }
        BasicStringUTF<T> & Assign(const T* other, ptrdiff_t otherSizeInTs = -1)
        {
            auto oldData = this->data;

            this->data = Strings::Create(other, otherSizeInTs, &this->lengthInTs);

            Strings::Delete(oldData);
            return *this;
        }

        template <typename U>
        BasicStringUTF<T> & Assign(const BasicStringUTF<U> &other)
        {
            auto oldData = this->data;

            this->data = Strings::Create<T>(other.c_str(), other.GetLengthInTs(), &this->lengthInTs);

            Strings::Delete(oldData);
            return *this;
        }
        template <typename U>
        BasicStringUTF<T> & Assign(const U* other, ptrdiff_t otherSizeInUs = -1)
        {
            auto oldData = this->data;

            this->data = Strings::Create<T>(other, otherSizeInUs, &this->lengthInTs);

            Strings::Delete(oldData);
            return *this;
        }

        template <typename U>
        BasicStringUTF<T> & AssignFormatted(const U* other, ...)
        {
            auto oldData = this->data;

            va_list argList1;
            va_list argList2;
            va_start(argList1, other);
            va_start(argList2, other);
            {
                this->data = Strings::CreateFormatted(other, argList1, argList2, &this->lengthInTs);
            }
            va_end(argList1);
            va_end(argList2);

            Strings::Delete(oldData);
            return *this;
        }


        /// Appends another string to the end of this one.
        BasicStringUTF<T> & Append(const T* other, ptrdiff_t otherSizeInTs = -1)
        {
            if (this->data == nullptr || this->lengthInTs == 0)
            {
                return this->Assign(other, otherSizeInTs);
            }


            Strings::List<T> newValue;
            newValue.Append(this->data, static_cast<ptrdiff_t>(this->lengthInTs));
            newValue.Append(other, otherSizeInTs);

            return this->Assign(newValue.c_str());
        }
        BasicStringUTF<T> & Append(const BasicStringUTF<T> &other)
        {
            return this->Append(other.c_str(), static_cast<ptrdiff_t>(other.GetLengthInTs()));
        }

        template <typename U>
        BasicStringUTF<T> & Append(const U* other, ptrdiff_t otherSizeInUs = -1)
        {
            // We'll first need to conver the other string to the destination encoding.
            T* otherStr = Strings::Create<T>(other, otherSizeInUs);
            this->Append(otherStr);
            Strings::Delete(otherStr);

            return *this;
        }
        template <typename U>
        BasicStringUTF<T> & Append(const BasicStringUTF<U> &other)
        {
            return this->Append(other.c_str(), static_cast<ptrdiff_t>(other.GetLengthInTs()));
        }

        template <typename U>
        BasicStringUTF<T> & AppendFormatted(const U* other, ...)
        {
            va_list argList1;
            va_list argList2;
            va_start(argList1, other);
            va_start(argList2, other);
            {
                auto stringToAppend = Strings::CreateFormatted(other, argList1, argList2);
                assert(stringToAppend != nullptr);
                {
                    this->Append(stringToAppend);
                }

                Strings::Delete(stringToAppend);
            }
            va_end(argList1);
            va_end(argList2);

            return *this;
        }


        /// Inserts a character at the given index.
        void InsertCharacter(char32_t character, unsigned int index)
        {
            T charStr[8];
            size_t charLength = GTLib::Strings::WriteChar(charStr, character);

            Strings::List<T> newValue;
            newValue.Append(this->data,         static_cast<ptrdiff_t>(index));
            newValue.Append(charStr,            charLength);
            newValue.Append(this->data + index, static_cast<ptrdiff_t>(this->lengthInTs - index));

            this->Assign(newValue.c_str());
        }


        /// Erases the characters in the given range.
        void EraseRange(unsigned int iCharStart, unsigned int iCharEnd)
        {
            assert(iCharStart <  iCharEnd);
            assert(iCharStart <  this->lengthInTs);
            assert(iCharEnd   <= this->lengthInTs);

            Strings::List<T> newValue;
            newValue.Append(this->data,            iCharStart);
            newValue.Append(this->data + iCharEnd, ptrdiff_t(this->lengthInTs - iCharEnd));

            this->Assign(newValue.c_str());
        }

        /// Erases the character at the given index.
        void EraseCharacterByIndex(unsigned int iChar)
        {
            if (iChar < this->lengthInTs)
            {
                // We're not going to re-create the buffer. Instead, we just move everything down a spot.
                for (size_t i = iChar; i < this->lengthInTs; ++i)
                {
                    this->data[i] = this->data[i + 1];
                }

                this->lengthInTs -= 1;
            }
        }


        /// Performs a simple character replacement.
        ///
        /// @param source [in] The character to be replaced.
        /// @param dest   [in] The replacement character.
        void SimpleReplace(T source, T dest)
        {
            for (size_t i = 0; i < this->lengthInTs; ++i)
            {
                if (this->data[i] == source)
                {
                    this->data[i] = dest;
                }
            }
        }


        /// Trims the whitespace from both the start and end of the string.
        BasicStringUTF<T> & Trim()
        {
            ptrdiff_t newLengthInTs = static_cast<ptrdiff_t>(this->lengthInTs);

            Strings::Trim(this->data, newLengthInTs);
            this->lengthInTs = static_cast<size_t>(newLengthInTs);

            return *this;
        }


        /// Converts the string to lower case.
        ///
        /// @remarks
        ///     This is only reliable for ASCII strings due to C++'s horrific string handling.
        void ToLower_ASCII()
        {
            std::locale loc;
            BasicStringUTF<T> temp;

            Strings::Iterator<T> i(this->data);
            while (i)
            {
                char32_t c = std::tolower(static_cast<char>(i.character), loc);     // <-- Ideally, we should be able to remove the 'char' cast here. It's needed in order to prevent an exception.
                temp.Append(&c, 1);

                ++i;
            }

            *this = temp;
        }


        /// Determines whether or not the string is equal to another.
        template <typename U>
        bool IsEqual(const BasicStringUTF<U> &other) const
        {
            if (this->data == other.data)
            {
                return true;
            }

            if (this->data == nullptr && other.data != nullptr)
            {
                return false;
            }

            return Strings::Equal(this->data, static_cast<ptrdiff_t>(this->lengthInTs), other.c_str(), static_cast<ptrdiff_t>(other.GetLengthInTs()));
        }
        template <typename U>
        bool IsEqual(const U* other) const
        {
            if (this->data == other)
            {
                return true;
            }

            if (this->data == nullptr && other != nullptr)
            {
                return false;
            }

            return Strings::Equal(this->data, other);
        }


        /// Determines whether or not the string is empty.
        bool IsEmpty() const
        {
            return this->lengthInTs == 0;
        }

        /// Clears the string to make it an empty one.
        void Clear()
        {
            Strings::Delete(this->data);

            this->data       = nullptr;
            this->lengthInTs = 0;
        }


        /// Retrieves a pointer to the c-style string data.
        const T* c_str() const
        {
            if (this->data == nullptr)
            {
                this->data = Strings::CreateEmpty<T>();
            }

            return this->data;
        }


    // Operator overloads.
    public:

        /// Assignment operator.
        BasicStringUTF<T> & operator=(const BasicStringUTF<T> &other)           // <-- Not having this produces -Weffc++.
        {
            return this->Assign(other);
        }

        template <typename U>
        BasicStringUTF<T> & operator=(const BasicStringUTF<U> &other)
        {
            return this->Assign(other);
        }
        template <typename U>
        BasicStringUTF<T> & operator=(const U* other)
        {
            return this->Assign(other);
        }


        /// Concatination operator.
        template <typename U>
        BasicStringUTF<T> & operator+=(const BasicStringUTF<U> &other)
        {
            return this->Append(other);
        }
        template <typename U>
        BasicStringUTF<T> & operator+=(const U* other)
        {
            return this->Append(other);
        }

        template <typename U>
        BasicStringUTF<T> operator+(const BasicStringUTF<U> &other) const
        {
            BasicStringUTF result(*this);
            result += other;

            return result;
        }
        template <typename U>
        BasicStringUTF<T> operator+(const U* other) const
        {
            BasicStringUTF result(*this);
            result += other;

            return result;
        }


        /// Comparison operator.
        template <typename U>
        bool operator==(const BasicStringUTF<U> &other) const
        {
            return this->IsEqual(other);
        }
        template <typename U>
        bool operator==(const U* other) const
        {
            return this->IsEqual(other);
        }

        template <typename U>
        bool operator!=(const BasicStringUTF<U> &other) const
        {
            return !this->IsEqual(other);
        }
        template <typename U>
        bool operator!=(const U* other) const
        {
            return !this->IsEqual(other);
        }


    private:

        /// A pointer to the string data. This is always null terminated. This can be null, which is equivalent to an empty string. If this
        /// is null when c_str() is called, it will be initialised to an empty string. Thus, it must be mutable.
        mutable T* data;

        /// The length of the string in T's. Does not include the null-terminator. This is NOT the number of characters,
        /// only the number of Ts in the buffer.
        size_t lengthInTs;





    ////////////////////////////////////////////////
    // Static Functions.
    public:

        /// Creates a formatted string object.
        static BasicStringUTF<T> CreateFormatted(const char* input, ...)
        {
            BasicStringUTF<T> result;


            va_list argList1;
            va_list argList2;
            va_start(argList1, input);
            va_start(argList2, input);
            {
                auto formattedString = Strings::CreateFormatted(input, argList1, argList2);

                result.Assign(formattedString);

                Strings::Delete(formattedString);
            }
            va_end(argList1);
            va_end(argList2);


            return result;
        }
    };


    typedef BasicStringUTF<char>     String;
    typedef BasicStringUTF<char>     String8;
    typedef BasicStringUTF<char16_t> String16;
    typedef BasicStringUTF<char32_t> String32;



    /// Simple structure representing a string segment.
    template <typename T>
    struct StringSegmentUTF
    {
        StringSegmentUTF()
            : strBegin(nullptr), strEnd(nullptr)
        {
        }

        StringSegmentUTF(T* strBeginIn, T* strEndIn)
            : strBegin(strBeginIn), strEnd(strEndIn)
        {
        }


        /// The beginning of the string.
        T* strBegin;

        /// The end of the string. This should be one character after the last character. This allows the length to be computed as "strEnd - startBegin".
        T* strEnd;
    };

    typedef StringSegmentUTF<char>     StringSegment;
    typedef StringSegmentUTF<char>     StringSegment8;
    typedef StringSegmentUTF<char16_t> StringSegment16;
    typedef StringSegmentUTF<char32_t> StringSegment32;
}

#endif
