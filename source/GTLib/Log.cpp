// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Config.hpp>
#include <GTLib/Log.hpp>
#include <GTLib/DateTime.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Strings/Replacer.hpp>
#include <GTLib/Strings/List.hpp>
#include <GTLib/Threading.hpp>
#include <GTLib/BasicBuffer.hpp>
#include <GTLib/IO.hpp>
#include <GTLib/stdlib.hpp>

#include <clocale>
#include <cstdarg>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4482)   // non-standard extension.
#endif

namespace GTLib
{
    void LogEventHandler::OnOpen(const char *)
    {
    }
    void LogEventHandler::OnOpen(const char *filename, const char *)
    {
        this->OnOpen(filename);
    }

    void LogEventHandler::OnClose()
    {
    }

    void LogEventHandler::OnWrite(const char *)
    {
    }
}

namespace GTLib
{
    /// The instantiation of the global log object.
    Log GlobalLog;


    namespace Detail
    {
        /// The temporary buffer used by all logs. Results from building strings are stored in this
        /// buffer by all logs. It is protected with a mutex.
        GT::BasicBuffer LogTempBuffer;

        /// The replacer object for replacing unicode text to HTML.
        Strings::Replacer LogReplacer;

        /// The mutex protecting access to the global objects.
        Threading::Mutex LogMutex;
    }

    #if defined(GT_PLATFORM_WINDOWS)
    static const char *newline = "\r\n";
    #else
    static const char *newline = "\n";
    #endif

    /**
    *   \brief  Enumerator for the different types of logs.
    */
    enum LogFormat
    {
        LogFormat_Text,
        LogFormat_HTML
    };

    Log::Log()
        : file(nullptr), format(LogFormat_Text), isOpen(false), currentHTMLPosition(0), eventHandlers()
    {
    }

    Log::Log(const char *fileName, const char *title)
        : file(nullptr), format(LogFormat_Text), isOpen(false), currentHTMLPosition(0), eventHandlers()
    {
        this->Open(fileName, title);
    }

    bool Log::Open(const char *fileName, const char *title)
    {
        if (fileName)
        {
            this->file = IO::Open(fileName, IO::OpenMode::Read | IO::OpenMode::Write | IO::OpenMode::CreateDirs);
            if (this->file != nullptr)
            {
                // We're good. Now we need to set our format based on the extension. If it is .html, we will output to a HTML file.
                if (Path::ExtensionEqual(fileName, "html"))
                {
                    // HTML.
                    this->format = LogFormat_HTML;

                    Detail::LogMutex.Lock();
                    {
                        // We need to setup a few things if we're doing HTML. We first output the header, then we grab the position, then we
                        // write the tail. We need the position just after the header because that is where new entries will be inserted. We
                        // can't insert new log entries at the end of the file, because that would result in invalid HTML.
                        IO::WriteString(this->file, this->BuildHTMLHead(title));

                        // We need to grab the current position of the file so that we can set that as the position to start inserting rows.
						this->currentHTMLPosition = IO::Tell(this->file);

                        IO::WriteString(this->file, this->BuildHTMLTail());
                    }
                    Detail::LogMutex.Unlock();

                    IO::Flush(this->file);
                }
                else
                {
                    // Text.
                    this->format = LogFormat_Text;

                    IO::WriteString(this->file, title);
                    IO::Flush(this->file);
                }

                this->isOpen = true;
                this->OnOpen(fileName, title);

                return true;
            }
            else
            {
                return false;
            }
        }

        return false;
    }

    void Log::Close()
    {
        this->OnClose();

        if (this->file != nullptr)
        {
            IO::Close(this->file);
        }

        this->isOpen = false;
    }

    Log & Log::WriteString(const char* value)
    {
        if (this->file != nullptr)
        {
            if (this->format == LogFormat_HTML)
            {
                // On a HTML log, we just move to the insertion position, write the new entry, and then write the tail again. We
                // always need to make sure the tail is attached after each entry so that we have valid HTML.
                Detail::LogMutex.Lock();
                {
					IO::Seek(this->file, this->currentHTMLPosition, SeekOrigin::Start);
                    IO::WriteString(this->file, this->BuildHTMLEntry(value));
                    this->currentHTMLPosition = IO::Tell(this->file);

                    IO::WriteString(this->file, this->BuildHTMLTail());
                }
                Detail::LogMutex.Unlock();

                IO::Flush(this->file);
            }
            else
            {
                // All we need to do is build the text entry, throw it into the file and then flush.
                Detail::LogMutex.Lock();
                {
                    IO::WriteString(this->file, this->BuildTextEntry(value));
                }
                Detail::LogMutex.Unlock();

                IO::Flush(this->file);
            }

            this->OnWrite(value);
        }

        return *this;
    }

    Log & Log::Write(const char *value, ...)
    {
        // The text log.
        if (this->file != nullptr)
        {
            char*  transformedValue = nullptr;

            // We need to use printf to transform the value.
            va_list argList1;
            va_list argList2;
            va_start(argList1, value);
            va_start(argList2, value);
            {
                transformedValue = GTLib::Strings::CreateFormatted(value, argList1, argList2);
            }
            va_end(argList1);
            va_end(argList2);

            this->WriteString(transformedValue);

            GTLib::Strings::Delete(transformedValue);
        }

        return *this;
    }


    void Log::AttachEventHandler(LogEventHandler *eventHandler)
    {
        if (eventHandler != nullptr)
        {
            if (!this->eventHandlers.Exists(eventHandler))
            {
                this->eventHandlers.PushBack(eventHandler);
            }
        }        
    }

    void Log::DetachEventHandler(LogEventHandler *eventHandler)
    {
        this->eventHandlers.RemoveFirstOccuranceOf(eventHandler);
    }


    const char * Log::BuildHTMLHead(const char *title) const
    {
        Strings::List<char> head;
        head.Append("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
            "    <head>\n"
            "        <style type=\"text/css\">\n"
            "            * { border-collapse:collapse; margin:0; padding:0; }\n"
            "            .title { color:#FFBB00; font:18pt Arial; margin:8px; border-bottom:solid 1px #fff }\n"
            "            .content { padding:4px; font:10pt Courier New; color:#fff }\n"
            "            .content.tr {  }\n"
            "            .content.tr:hover { background-color:#B3B3B3; }\n"
            "            .row-left { width:180px; padding-right:16px; padding-left:8px; padding-top:2px; text-align:right; vertical-align:top; }\n"
            "            .row-right{ padding-left:16px; padding-bottom:2px; padding-top:2px; border-left:solid 1px #fff; }\n"
            "        </style>\n"
            "        <title>"); head.Append(title); head.Append("</title>\n");
        head.Append(
            "    </head>\n"
            "    <body style=\"background-color:#AAAAAA;\">\n"
            "        <div class=\"title\">"); head.Append(title); head.Append("</div>\n");
        head.Append(
            "        <table class=\"content\">\n");

        Detail::LogTempBuffer.Allocate(head.BuildString(nullptr));
        head.BuildString((char *)Detail::LogTempBuffer.GetDataPointer());

        return reinterpret_cast<const char*>(Detail::LogTempBuffer.GetDataPointer());
    }

    const char * Log::BuildHTMLTail() const
    {
        return
            "        </table>\n"
            "    </body>\n"
            "</html>";
    }

    // This is NOT THREAD-SAFE!. Needs to be locked at a higher level.
    const char * Log_ToHTML(const char *value)
    {
        Detail::LogReplacer.SetBase(value);
        Detail::LogReplacer.Replace("<",    "&lt;");
        Detail::LogReplacer.Replace(">",    "&gt;");
        Detail::LogReplacer.Replace(" ",    "&nbsp;");
        Detail::LogReplacer.Replace("\n\r", "<br />");
        Detail::LogReplacer.Replace("\n",   "<br />");
        Detail::LogReplacer.Replace("\t",   "&nbsp&nbsp&nbsp&nbsp");

        return Detail::LogReplacer.GetBase();
    }

    const char * Log::BuildHTMLEntry(const char *value) const
    {
        // Set the locale for the date.
        DateTime now(DateTime::Now());
        setlocale(LC_ALL, "");
        GTLib::String date = now.ToShortString();
        setlocale(LC_ALL, "C");

        Strings::List<char> output;

        output.Append(
            "                <tr >\n"
            "                    <td class=\"row-left\">"); output.Append(date.c_str()); output.Append("</td>\n");
        output.Append(
            "                    <td class=\"row-right\">"); output.Append(Log_ToHTML(value)); output.Append("</td>\n");
        output.Append(
            "                </tr>\n");

        Detail::LogTempBuffer.Allocate(output.BuildString(nullptr));
        output.BuildString((char *)Detail::LogTempBuffer.GetDataPointer());

        return reinterpret_cast<const char*>(Detail::LogTempBuffer.GetDataPointer());
    }

    const char * Log::BuildTextEntry(const char *value) const
    {
        DateTime now(DateTime::Now());
        setlocale(LC_ALL, "");
        GTLib::String date = now.ToShortString();
        setlocale(LC_ALL, "C");

        Strings::List<char> output;
        output.Append(date.c_str());
        output.Append(value);
        output.Append(GTLib::newline);

        Detail::LogTempBuffer.Allocate(output.BuildStringSize());
        output.BuildString((char *)Detail::LogTempBuffer.GetDataPointer());

        return (const char *)Detail::LogTempBuffer.GetDataPointer();
    }


    void Log::OnOpen(const char *filename, const char *title)
    {
        for (size_t iEventHandler = 0; iEventHandler < this->eventHandlers.count; ++iEventHandler)
        {
            auto eventHandler = this->eventHandlers[iEventHandler];
            assert(eventHandler != nullptr);
            {
                eventHandler->OnOpen(filename, title);
            }
        }
    }

    void Log::OnClose()
    {
        for (size_t iEventHandler = 0; iEventHandler < this->eventHandlers.count; ++iEventHandler)
        {
            auto eventHandler = this->eventHandlers[iEventHandler];
            assert(eventHandler != nullptr);
            {
                eventHandler->OnClose();
            }
        }
    }

    void Log::OnWrite(const char *msg)
    {
        for (size_t iEventHandler = 0; iEventHandler < this->eventHandlers.count; ++iEventHandler)
        {
            auto eventHandler = this->eventHandlers[iEventHandler];
            assert(eventHandler != nullptr);
            {
                eventHandler->OnWrite(msg);
            }
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
