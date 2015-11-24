// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_IO_FileIterator_hpp_
#define __GTLib_IO_FileIterator_hpp_

#if 0
#include <GTLib/List.hpp>
#include <cstdint>
#include <ctime>
#include <easy_fs/easy_vfs.h>

namespace GTLib
{
    namespace IO
    {
        /**
        *   \brief  Iterator for retrieving files in a directory.
		*
		*	\remarks
		*		The file iterator is a great class for searching for files in a directory. Upon construction, you specify
		*		the directory that should be iterated. The iterator uses regular expressions allowing you can filter out various
		*		files. To search an entire directory, you just use the '*' wildcard symbol. E.g: "C:\\Windows\\*" will search for
		*		every file in the the "C:\\Windows" directory. Note that in this example the trailing star is required. If a path
		*		ends with just a slash, the iterator will not find anything.
		*		\par
		*		You can also do something like the following: "MyProject/source/.*\.cpp" which will iterate over the files with
		*		a .cpp extension residing in the "MyProject/source" directory.
		*		\par
		*		Take this query: "MyProjects/source/main\.cpp".
		*		In this case, only one file will be iteratated - main.cpp. If main.cpp happens to not exist, nothing will be
		*		iterated. Note how the dot needs to be escaped due to the use of regular expressions.
		*		\par
		*		Take this query: "MyProjects/source".
		*		This query will find only a single item - the "MyProjects/source" directory. It will \c NOT find the files
		*		contained in "MyProjects/source". To do that, use "MyProjects/source/.*".
		*		\par
		*		The default constructor will iterate over all files and directories in the current directory.
		*		\par
		*		This is a forward-only iterator - you can not iterate backwards with operator--.
		*		\par
		*		Use a loop like the following to iterate over the files.
		*		\code
		*		GTLib::IO::FileIterator i("source/.*");
		*		while (i)
		*		{
		*			// Do some stuff with 'i'...
		*			++i;
		*		}
		*		\endcode
		*		\par
		*		Access the 'name' attribute to get the name of the current file. This name will be in the same format
		*		as the search query used in the constructor. As in the example above, you would get a file name like
		*		"source/main\.cpp". In the case of the default constructor, it'll be like "main.cpp".
		*		\par
		*		You can use both forward- and back-slashes in paths.
		*       \par
		*       Due to the way directory/file iteration works on POSIX systems, the iterator will find every matching file
		*       at once during the constructor. This has some minor side effects:
		*           - The iterated files will be the same as those that existing at the time the constructor was called. Any
		*             modifications made to the directory during iteration will not be reflected by the iterator.
		*           - You'll get a fairly slow constructor.
		*       The reason we go with this behaviour is because with POSIX we would need to change the working directory for
		*       every iteration, which could lead to problems with multithreaded environments (working-directory routines are
        *       not synchronized).
        *       \par
        *       The iterator needs to call GTLib::IO::GetCurrentDirectory() in it's constructor in order to translate
        *       relative paths to absolute paths. Be aware of this when doing multi-threading. GetCurrentDirectory() is only
        *       called once.
		*       \par
		*       The order of the returned files is undefined. It won't neccessarilly be in alphabetical order.
        *       \par
        *       This class uses PCRE internally. You will need to link to a pcre library if you use this class.
        */
        class FileIterator
        {
        public:

			FileIterator();
            FileIterator(const char* fileQuery);
			FileIterator(const FileIterator &i);

            ~FileIterator();

            /**
            *   \brief  Moves to the next file in the directory.
            */
            FileIterator & Next();

            /**
            *   \brief  Determines whether or not iterator is past the last file.
            */
            bool AtEnd() const;


            FileIterator & operator++ ()    { return this->Next(); };
            FileIterator   operator++ (int) { FileIterator temp(*this); ++(*this); return temp; }

            operator bool() const { return !this->AtEnd(); }

            FileIterator & operator= (const FileIterator &i);


        public:

            /// The name of the file or directory.
            const char* name;

            /// The absolute path of the file or directory.
            const char* absolutePath;

            /// The size of the file in bytes. This is 0 if 'directory' is true.
            int64_t size;

            /// The time the file was last modified.
            time_t lastModified;

            /// Keeps track of whether or not the file is a directory.
            bool isDirectory;


		private:

            /// A list containing information about all of the iterated files. This is filled during construction
            /// and emptied as the iterator moves forward. The root item in this list is the file the iterator is
            /// currently pointing to.
            ///
            /// This needs to be a pointer so that we don't have to make a deep copy of the list when copying the
            /// iterator.
            GTLib::List<easyvfs_file_info*>* fileInfoList;

            /// The number of instantiations of this iterator we have floating around. We use this to determine when
            /// to deallocate the internal data (like \c fileInfoList).
            int count;


		private:

			/**
			*	\brief	Generalised constructor.
			*/
			void ctor(const char* fileQuery);
        };
    }
}


#endif

#endif
