// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

#include <GTLib/Strings/Create.hpp>
#include <cstdarg>

namespace GTEngine
{
    void PostError(const char* format, ...)
    {
        char* formattedStr = nullptr;

        // We need to format the string first.
        va_list argList1;
        va_list argList2;
        va_start(argList1, format);
        va_start(argList2, format);
        {
            formattedStr = GTLib::Strings::CreateFormatted(format, argList1, argList2);
        }
        va_end(argList1);
        va_end(argList2);

        // Errors are always logged...
        GTEngine::LogString(formattedStr);


        // We're finished with the formatted string, so it needs to be deleted.
        GTLib::Strings::Delete(formattedStr);
    }
}
