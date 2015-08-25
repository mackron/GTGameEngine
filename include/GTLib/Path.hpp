// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GTLib_Path
#define GTLib_Path

#include "Strings/List.hpp"
#include <cstddef>

namespace GTLib
{
    /**
    *   \brief  Class for working with a path.
    *
    *   This class only works with forward slashes. This is due to conflicts with the regex back-slash escape character.
    *   To get this class working correctly with back-slashes, you must first convert those slashes to forward slashes
    *   with the static GTLib::Path::FormatSlashes() function.
    *   \par
    *   When the path string is required, use c_str().
    */
    class Path
    {
    public:

        /**
        *   \brief            Constructor.
        *   \param  path [in] The path string.
        */
        Path(const char* path);

        /**
        *   \brief  Destructor.
        */
        ~Path();

        /**
        *   \brief  Retrieves the size in bytes of the path string. Required for Copy().
        *   \return The size in bytes of the path string, which can be used to allocate a buffer for Copy().
        */
        size_t Size() const;

        /**
        *   \brief  Copies the path string into the given buffer.
        *   \param  dest [in] The destination buffer.
        *
        *   \remarks
        *       The destination buffer must be large enough to store the whole string, plus a null terminator. Use Size() for this.
        */
        void Copy(char* dest) const;

        /**
        *   \brief            Makes the path absolute.
        *   \param  base [in] The base part of the absolute path. This must be absolute.
        */
        void MakeAbsolute(const char* base);

        /**
        *   \brief  The same as MakeAbsolute(const char *), except uses the current directory for the base.
        */
        void MakeAbsolute();


        /// Makes the path relative to the given absolute path.
        ///
        /// @param relativeTo [in] The path to go relative to.
        void MakeRelative(const char* relativeTo);

        /// Makes the path relative to the current directory.
        void MakeRelative();


        /**
        *   \brief             Appends another path to this one.
        *   \param  other [in] The other path to append.
        *   \return            *this
        *
        *   \remarks
        *       This method will handle any leading ".." directories in \c other.
        */
        Path & Append(const char* other);

        /// Prepends a path to this path.
        ///
        /// @param other [in] The other path to prepend.
        ///
        /// @return *this.
        Path & Prepend(const char* other);


        /// Removes the first item in the path.
        ///
        /// @return *this.
        Path & RemoveFirst();

        /// Removes the last item from the path.
        ///
        /// @return *this.
        Path & RemoveLast();


        /// Cleans the path by resolving the ".." parts.
        void Clean();


        /**
        *   \brief  Determines if the path is absolute.
        */
        bool IsAbsolute() const;

		/**
		*	\brief	Converts the path to a c-style string and returns a pointer to that string. See remarks.
		*
		*	\remarks
		*		The returned string will be made invalid the next time c_str() is called or when the path is destructred.
		*		\par
		*		The string will be re-constructed for each call. Don't call it more frequently than needed.
		*/
		const char* c_str();

        /**
        *   \brief            Sets the path to another string.
        *   \param  path [in] The value to set for the path. Must stay in scope.
        */
        Path & operator= (const char* path);


    private:

        /// The list of parts making up the path.
        Strings::List<char> parts;

        /// The number of parts making up the path. We need this so that we can calculate the length of the path correctly - so we know
        /// how many slashes to include when calculating the length.
        size_t partCount;

		/// The temporary string we keep a hold of for the c_str() method.
		char* str;

		/// Keeps track of whether or not to include a leading slash. This is because the tokenizer will remove it, but it is needed
        /// when constructing the path string.
        bool leadingSlash;


	private:
		Path(const Path &);
		Path & operator=(const Path &);


    public: // Statics.

        /**
        *   \brief            Determines if a path is relative.
        *   \param  path [in] The path that is being checked.
        *   \return           True if the path is relative; false otherwise.
        */
        static bool IsRelative(const char* path);

        /**
        *   \brief            Checks whether or not the path is absolute.
        *   \param  path [in] The path to check.
        *   \return           True if the path is absolute; false otherwise.
        */
        static bool IsAbsolute(const char* path);

        /**
        *   \brief            Retrieves the extension of the given path.
        *   \param  path [in] The path whose extension is being retrieved.
        *   \return           The extension of the path, or an empty string if the path does not have an extension.
        */
        static const char* Extension(const char* path);

        /**
        *   \brief            Checks if the extension of the given path is the same as the given extension.
        *   \param  path [in] The path whose extension is being compared.
        *   \param  ext  [in] The extension used in the comparison.
        *
        *   \remarks
        *       The comparison is case-insensitive.
        */
        static bool ExtensionEqual(const char* path, const char* ext);

        /**
        *   \brief                             Splits a path into it's individual sections and appends them to the given list.
        *   \param  path                 [in]  The path being split.
        *   \param  list                 [out] A reference to the list that will have the parts appended to.
        *   \param  resolveRelativeParts [in]  Controls whether or not the '..' parts should be resolved.
        *   \return                            The number of parts in the path.
        *
        *   \remarks
        *       As usual, copies are not made of the input string.
        *       \par
        *       The root directory is not included in the list, or the return value.
        */
        static size_t SplitPath(const char* path, Strings::List<char> &list, bool resolveRelativeParts = false);

        /**
        *   \brief                Formats all of the slashes in the path to the specified slash character.
        *   \param  path     [in] The path whose slashes are being formatted.
        *   \param  newSlash [in] The new slash to use. Should be either '\\' or '/'.
        *   \return               \c path.
        *
        *   \remarks
        *       The input path is a UTF-8 string. The slash character will always be the same number of bytes, so the function
        *       will simple do an in-place conversion.
        */
        static char* FormatSlashes(char* path, char32_t newSlash);
    };
}

#endif
