// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GTLib_Clipboard
#define GTLib_Clipboard

#include <GTLib/String.hpp>

namespace GTLib
{
    /// Static class representing the clipboard for doing copy-and-paste.
    class Clipboard
    {
    public:

        /// Sets text data to the clipboard.
        ///
        /// @param text     [in] The text to place into the clipboard.
        /// @param sizeInTs [in] The size of 'text' in Ts. Can be -1, in which case it is assumed 'text' is null terminated.
        static void SetText(const char* text, ptrdiff_t sizeInTs = -1);

        /// Retrieves the text data currently on the clipboard.
        static GTLib::String GetText();


    private:


    };
}

#endif
