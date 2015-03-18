// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Regex_hpp_
#define __GTLib_Regex_hpp_

namespace GTLib
{
    /**
    *   \brief  Class representing a regular expression pattern.
    *
    *   \remarks
    *       This class uses SLRE v1.0 internally, which implements a subset of Perl regular expressions. See http://slre.sourceforge.net/.
    */
    class Regex
    {
    public:

        /**
        *   \brief  Constructor.
        */
        Regex(const char *pattern);

        /**
        *   \brief  Destructor.
        */
        ~Regex();

        /**
        *   \brief  Checks if the given string matches this expression.
        */
        bool Match(const char *subject);


    private:

        // Used internally.
        void *internalData;


    private:    // No copying.
        Regex(const Regex &);
        Regex & operator=(const Regex &);
    };
}

#endif
