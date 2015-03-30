// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Strings_Replacer_hpp_
#define __GTLib_Strings_Replacer_hpp_

#include "../BasicBuffer.hpp"
#include "Size.hpp"
#include "List.hpp"
#include "Find.hpp"
#include "Copy.hpp"

namespace GTLib
{
    namespace Strings
    {
        /**
        *   \breif  Class used for replacing content in a string.
        *
        *   This class takes a string, called the base string, and allows you to replace parts of that string
        *   with other strings. The base string is set via the constructor or the SetBaseString() method. All
        *   calls to Replace() will work on the base string. The base string can be changed at any time.
        *
        *   The best way to use this class is to have a sort of global instantiation that is used across multiple
        *   parts of the application. This will allow you to more easily re-use the internal buffers to your
        *   advantage. This class is not thread-safe, so keep that in mind.
        *
        *   The internal buffers store the results of the last calls to Replace(). There are two buffers, which
        *   are swapped after each call to Replace(). Replace() will return a direct pointer to the respective
        *   buffer. You can also use GetBase() to get a pointer to the base string.
        */
        template <typename T>
        class ReplacerUTF
        {
        public:

            /**
            *   \brief  Default constructor.
            */
            ReplacerUTF()
                : m_base(nullptr), m_baseSizeInTs(0), m_buffers(), m_bufferIndex(0)
            {
            }
            
            /**
            *   \brief            Constructor.
            *   \param  base [in] The string that is having content replaced.
            *
            *   \remarks
            *       The base string is not copied straight away. A copy is only made after the first call to Replace().
            */
            ReplacerUTF(const T* base, ptrdiff_t baseSizeInTs = -1)
                : m_base(base), m_baseSizeInTs(baseSizeInTs), m_buffers(), m_bufferIndex(0)
            {
            }
                
            /**
            *   \brief            Sets the base string.
            *   \param  base [in] The new base string.
            *
            *   \remarks
            *       The base string is not copied straight away. A copy is only made after the first call to Replace().
            */
            void SetBase(const T* base, ptrdiff_t baseSizeInTs = -1)
            {
				m_base         = base;
				m_baseSizeInTs = baseSizeInTs;
            }
                
            /**
            *   \brief  Retrieves the base string.
            *   \return A pointer to the base string.
            *
            *   \remarks
            *       This is not always the same pointer as specified in SetBase() or the constructor. The Replace() method
            *       will make a copy of the base and place it in the buffer, making that the new base string.
            */
            const T* GetBase() const
            {
                if (m_base != nullptr)
                {
                    return m_base;
                }
                else
                {
                    return static_cast<const T*>(m_buffers[m_bufferIndex].GetDataPointer());
                }
            }
                
                
            /**
            *   \brief                               Replaces every occurance of a string with another.
            *   \param  original                [in] The string being replaced.
            *   \param  originalSizeInBytes     [in] The size in bytes of the original string. Use this when the original string can not be null terminated.
            *   \param  replacement             [in] The replacement string.
            *   \param  replacementSizeInBytes  [in] The size in bytes of the replacement string. Use this when the replacement string can not be null terminated.
            *   \return                              A null-terminated string containing the new string.
            *
            *   \remarks
            *       A copy of the base string will be made during the first call to Replace(), after which time it is safe to destroy the
            *       original base string.
            *       The *SizeInTs arguments are used when a null terminator can not easily be placed into the respective string. However,
            *       even if the string is null terminated, always specify the size if it is already known. 
            */
            const T* Replace(const T* original, ptrdiff_t originalSizeInTs, const T* replacement, ptrdiff_t replacementSizeInTs)
            {
                if (original != nullptr)
                {
                    const T* base;
                    if (m_base != nullptr)
                    {
                        base = m_base;
                        if (m_baseSizeInTs == -1)
                        {
							m_baseSizeInTs = SizeInTs(base);
                        }
                    }
                    else
                    {
                        // We don't need to set the size here because m_baseSizeInBytes will have been set when the buffer was written.
                        base = reinterpret_cast<const T*>(m_buffers[m_bufferIndex].GetDataPointer());
                    }

                    if (base != nullptr)
                    {
                        // We need sizes for the input strings.
                        if (originalSizeInTs == -1)
                        {
                            originalSizeInTs = SizeInTs(original);
                        }
                        if (replacementSizeInTs == -1)
                        {
                            replacementSizeInTs = SizeInTs(replacement);
                        }
                        
                        // The difference in size between the replacement and original strings are used to determine the size of the
                        // destination buffer.
                        ptrdiff_t replaceSizeDiffInTs = replacementSizeInTs - originalSizeInTs;
                        
                        // Keeps track of the difference between the new size and the old size of the base string. This is just the
                        // accumulation of replaceSizeDiff for each occurance of original.
                        ptrdiff_t baseSizeDiffInTs = 0;
                        
                        // We're going to create a list of tokens for each 'original' match. From that list, we can go ahead and build the
                        // final string. We do it like this because: 1) The destination buffer will only need to be resized once, at most
                        // and 2) We don't need to move the data that comes after each 'original' - we just do it once.
                        Strings::List<char> originalList;
                        
                        // Now we just loop through each occurance of 'original' and add any to the list.
						const T* nextStart = FindFirst(base, -1, original, originalSizeInTs);
						while (nextStart)
						{
							originalList.Append(nextStart, originalSizeInTs);
							baseSizeDiffInTs += replaceSizeDiffInTs;

							nextStart = FindFirst(nextStart + originalSizeInTs, -1, original, originalSizeInTs);
						}
                        
                        // Now that the positions of each occurance of 'original', we can construct the result. The destination buffer is
                        // always the back buffer.
                        GT::BasicBuffer &destBuffer = m_buffers[!m_bufferIndex];
                        ptrdiff_t destSizeInTs      = m_baseSizeInTs + baseSizeDiffInTs;
                        
                        destBuffer.Allocate(static_cast<size_t>(destSizeInTs + 1) * sizeof(T));    // +1 for the null terminator.
                        T* result = static_cast<T*>(destBuffer.GetDataPointer());
                        T* dest   = result;
                        
                        // The pointer to the end of the last occurance of 'original'. This is used when we need to attach the last section.
                        const T* lastOriginalEnd = base;
                        
                        while (originalList.root)
                        {
                            // For each oldStr, we need to copy over two section. 1) The section before the occurance and 2) newStr.
                            std::memcpy(dest, lastOriginalEnd, static_cast<size_t>(originalList.root->start - lastOriginalEnd) * sizeof(T));
                            dest += originalList.root->start - lastOriginalEnd;
                            
                            std::memcpy(dest, replacement, static_cast<size_t>(replacementSizeInTs) * sizeof(T));
                            dest += replacementSizeInTs;

                            lastOriginalEnd = originalList.root->end;
                            
                            // Remove the root to move to the next item.
                            originalList.Remove(originalList.root);
                        }
                        
                        // There will be a section left over between the last occurance of oldStr and the end of the input string.
                        Copy(dest, lastOriginalEnd);
                        
                        
                        
                        // Now we need to finish up. We no longer want to use m_base, because that is now stored in one of our buffers.
                        // The new base size will be the size of the destination buffer, since that buffer is now the new base.
                        m_base         = nullptr;
                        m_baseSizeInTs = destSizeInTs;
                        m_bufferIndex  = !m_bufferIndex;
                        
                        return result;
                    }
                }

                return nullptr;
            }

            const T* Replace(const T* original, const T* replacement)
            {
                return this->Replace(original, -1, replacement, -1);
            }
                
            /**
            *   \brief  Replaces a character with another character.
            */
            const T* Replace(T original, T replacement)
            {
                // Replacing characters isn't as simple as it sounds. The problem is that characters can be variable in length.
                T originalStr[8];
                T replacementStr[8];

                WriteChar(originalStr, original);
                WriteChar(replacementStr, replacement);

                return this->Replace(originalStr, 1, replacementStr, 1);
            }

            /**
            */
            const T* c_str() const
            {
                if (m_base != nullptr)
                {
                    return m_base;
                }

                return static_cast<T*>(m_buffers[m_bufferIndex].ptr);
            }


        private:

            /// The base string. This is a direct reference, not a copy. Once this value has been copied into a buffer, it will be
            /// set to nullptr. It is not copied over straight away; instead it will be copied during the next call to Replace().
            const T* m_base;
                
            /// The size in bytes of the base string. Can be -1.
            ptrdiff_t m_baseSizeInTs;
            
            /// The two internal buffers.
            GT::BasicBuffer m_buffers[2];
                
            /// The index of the current buffer. Toggles between 0 and 1.
            size_t m_bufferIndex;
                
                
        private:    // No copying.
            ReplacerUTF(const ReplacerUTF<T> &);
            ReplacerUTF<T> & operator=(const ReplacerUTF<T> &);
        };


        typedef ReplacerUTF<char>     Replacer;
        typedef ReplacerUTF<char>     Replacer8;
        typedef ReplacerUTF<char16_t> Replacer16;
        typedef ReplacerUTF<char32_t> Replacer32;
    }
}

#endif
