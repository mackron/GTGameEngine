// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_Logging_hpp_
#define __GT_GUI_Logging_hpp_

#include <GTLib/String.hpp>

namespace GT
{
    /// Class for logging the layout calculations.
    class GUILayoutLogger
    {
    public:

        /// Constructor.
        GUILayoutLogger()
            : m_indentationLevel(0)
        {
        }

        /// Destructor.
        ~GUILayoutLogger()
        {
        }



        /// Marks the beginning of a logging line.
        void BeginLine()
        {
            m_indentationLevel += 2;
        }

        /// Marks the end of a logging line.
        void EndLine()
        {
            if (m_indentationLevel > 2)
            {
                m_indentationLevel -= 2;
            }
            else
            {
                m_indentationLevel = 0;
            }
        }



        void Print(const char* text)
        {
            GTLib::String indentation;
            for (unsigned int i = 0; i < m_indentationLevel; ++i)
            {
                indentation.Append(" ");
            }


            printf("%s%s", indentation.c_str(), text);
        }



    private:
        
        /// The current indentation level.
        unsigned int m_indentationLevel;
    };
}

#endif