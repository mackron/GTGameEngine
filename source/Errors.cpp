
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

#include <GTCore/Strings/Create.hpp>

#include <cstdlib>
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
            formattedStr = GTCore::Strings::CreateFormatted(format, argList1, argList2);
        }
        va_end(argList1);
        va_end(argList2);

        // Errors are always logged...
        GTEngine::Log(formattedStr);


        // We're finished with the formatted string, so it needs to be deleted.
        GTCore::Strings::Delete(formattedStr);
    }
}