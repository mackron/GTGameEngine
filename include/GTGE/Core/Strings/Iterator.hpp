// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Strings_Iterator
#define GT_Strings_Iterator

namespace GT
{
    namespace Strings
    {
        /**
        *   \brief  Class for iterating over a string.
        *
        *   The string does not need to be null terminated. In that case, you must specify the size of the stirng
        *   in bytes. If the size is specified, the iterator will never look beyond those bytes. Also, even if
        *   the size is given, the iterator will never move beyond a null terminator.
        */
        template <typename T>
        class Iterator
        {
        public:
        
            /**
            *   \brief  Constructs the iterator from a string.
            */
            Iterator(const T *str, ptrdiff_t strSizeInTs = -1)
                : character('\0'), str(str), next(str), sizeInTs(strSizeInTs)
            {
                // We just move to the first character by incrementing.
                ++(*this);
            }
            
            
            Iterator & operator++()
            {
                if (*this)
                {
                    this->str       = this->next;
                    this->character = NextChar(this->next, this->sizeInTs);
                    if (this->character == '\0')
                    {
                        this->sizeInTs = 0;
                    }
                }
                
                return *this;
            }
        
            operator bool() const
            {
                return !(this->sizeInTs == 0 && this->character == '\0');
            }
        
        
        public:
        
            /// The current character.
            char32_t character;

            /// The string being iterated.
            const T* str;

            /// The position of the next character.
            const T* next;
            
            /// the number of bytes remaining.
            ptrdiff_t sizeInTs;
        };
    }
}

#endif
