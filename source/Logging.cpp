
#include <GTEngine/Logging.hpp>
#include <GTCore/Strings/Create.hpp>
#include <GTCore/Log.hpp>

namespace GTEngine
{
    namespace Logging
    {
        /// A pointer to the global log object.
        GTCore::Log* EngineLog = nullptr;

        void Startup(const char* filename)
        {
            EngineLog = new GTCore::Log(filename, "GTEngine");
        }

        void Shutdown()
        {
            delete EngineLog;
            EngineLog = nullptr;
        }
    }


    void LogString(const char* value)
    {
        // Obviously, we write the value to the log object. We don't use an assert here because it's possible this may be called during
        // initialisation, before the logging sub-system has been initialised.
        if (Logging::EngineLog != nullptr)
        {
            Logging::EngineLog->WriteString(value);
        }

        // We'll also write the message to stdout.
        printf("%s\n", value);
    }

    void Log(const char* format, ...)
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

        
        // Obviously, we write the value to the log object. We don't use an assert here because it's possible this may be called during
        // initialisation, before the logging sub-system has been initialised.
        if (Logging::EngineLog != nullptr)
        {
            Logging::EngineLog->Write(formattedStr);
        }

        // We'll also write the message to stdout.
        printf("%s\n", formattedStr);


        // We're finished with the formatted string, so it needs to be deleted.
        GTCore::Strings::Delete(formattedStr);
    }
}
