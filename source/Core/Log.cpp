// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/Log.hpp>
#include <GTGE/Core/DateTime.hpp>
#include <GTGE/Core/Strings/Replacer.hpp>
#include <GTGE/Core/Strings/List.hpp>
#include <GTGE/Core/stdlib.hpp>
#include <easy_path/easy_path.h>

#include <clocale>
#include <cstdarg>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4482)   // non-standard extension.
#endif

namespace GT
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

namespace GT
{
    /// The instantiation of the global log object.
    LogFile GlobalLog;


    namespace Detail
    {
        /// The temporary buffer used by all logs. Results from building strings are stored in this
        /// buffer by all logs. It is protected with a mutex.
        //GT::BasicBuffer LogTempBuffer;

        /// The replacer object for replacing unicode text to HTML.
        //Strings::Replacer LogReplacer;

        /// The mutex protecting access to the global objects.
        //easyutil_mutex LogMutex = easyutil_create_mutex();
    }

    #if defined(_WIN32)
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

    LogFile::LogFile()
        : file(nullptr), format(LogFormat_Text), isOpen(false), currentHTMLPosition(0), eventHandlers()
    {
        LogMutex = easyutil_create_mutex();
    }

    LogFile::LogFile(easyvfs_context* pVFS, const char *fileName, const char *title)
        : file(nullptr), format(LogFormat_Text), isOpen(false), currentHTMLPosition(0), eventHandlers()
    {
        LogMutex = easyutil_create_mutex();
        this->Open(pVFS, fileName, title);
    }

    LogFile::~LogFile()
    {
        easyutil_delete_mutex(LogMutex);
    }

    bool LogFile::Open(easyvfs_context* pVFS, const char *fileName, const char *title)
    {
        if (fileName)
        {
            this->file = easyvfs_open(pVFS, fileName, EASYVFS_READ | EASYVFS_WRITE | EASYVFS_CREATE_DIRS, 0);
            if (this->file == nullptr) {
                return false;
            }

            assert(this->file != nullptr);


            // We're good. Now we need to set our format based on the extension. If it is .html, we will output to a HTML file.
            if (easypath_extension_equal(fileName, "html"))
            {
                // HTML.
                this->format = LogFormat_HTML;

                easyutil_lock_mutex(LogMutex);
                {
                    // We need to setup a few things if we're doing HTML. We first output the header, then we grab the position, then we
                    // write the tail. We need the position just after the header because that is where new entries will be inserted. We
                    // can't insert new log entries at the end of the file, because that would result in invalid HTML.
                    easyvfs_write_string(this->file, this->BuildHTMLHead(title));

                    // We need to grab the current position of the file so that we can set that as the position to start inserting rows.
                    this->currentHTMLPosition = (int64_t)easyvfs_tell(this->file);

                    easyvfs_write_string(this->file, this->BuildHTMLTail());
                }
                easyutil_unlock_mutex(LogMutex);

                easyvfs_flush(this->file);
            }
            else
            {
                // Text.
                this->format = LogFormat_Text;

                easyvfs_write_string(this->file, title);
                easyvfs_flush(this->file);
            }

            this->isOpen = true;
            this->OnOpen(fileName, title);

            return true;
        }

        return false;
    }

    void LogFile::Close()
    {
        this->OnClose();

        if (this->file != nullptr)
        {
            easyvfs_close(this->file);
        }

        this->isOpen = false;
    }

    LogFile & LogFile::WriteString(const char* value)
    {
        if (this->file != nullptr)
        {
            if (this->format == LogFormat_HTML)
            {
                // On a HTML log, we just move to the insertion position, write the new entry, and then write the tail again. We
                // always need to make sure the tail is attached after each entry so that we have valid HTML.
                easyutil_lock_mutex(LogMutex);
                {
					//IO::Seek(this->file, this->currentHTMLPosition, SeekOrigin::Start);
                    //IO::WriteString(this->file, this->BuildHTMLEntry(value));
                    //this->currentHTMLPosition = IO::Tell(this->file);

                    //IO::WriteString(this->file, this->BuildHTMLTail());

                    easyvfs_seek(this->file, this->currentHTMLPosition, easyvfs_start);
                    easyvfs_write_string(this->file, this->BuildHTMLEntry(value));
                    this->currentHTMLPosition = (int64_t)easyvfs_tell(this->file);

                    easyvfs_write_string(this->file, this->BuildHTMLTail());
                }
                easyutil_unlock_mutex(LogMutex);

                easyvfs_flush(this->file);
            }
            else
            {
                // All we need to do is build the text entry, throw it into the file and then flush.
                easyutil_lock_mutex(LogMutex);
                {
                    easyvfs_write_string(this->file, this->BuildTextEntry(value));
                }
                easyutil_unlock_mutex(LogMutex);

                easyvfs_flush(this->file);
            }

            this->OnWrite(value);
        }

        return *this;
    }

    LogFile & LogFile::Write(const char *value, ...)
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
                transformedValue = Strings::CreateFormatted(value, argList1, argList2);
            }
            va_end(argList1);
            va_end(argList2);

            this->WriteString(transformedValue);

            Strings::Delete(transformedValue);
        }

        return *this;
    }


    void LogFile::AttachEventHandler(LogEventHandler *eventHandler)
    {
        if (eventHandler != nullptr)
        {
            if (!this->eventHandlers.Exists(eventHandler))
            {
                this->eventHandlers.PushBack(eventHandler);
            }
        }        
    }

    void LogFile::DetachEventHandler(LogEventHandler *eventHandler)
    {
        this->eventHandlers.RemoveFirstOccuranceOf(eventHandler);
    }


    const char * LogFile::BuildHTMLHead(const char *title) const
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

        LogTempBuffer.Allocate(head.BuildString(nullptr));
        head.BuildString((char *)LogTempBuffer.GetDataPointer());

        return reinterpret_cast<const char*>(LogTempBuffer.GetDataPointer());
    }

    const char * LogFile::BuildHTMLTail() const
    {
        return
            "        </table>\n"
            "    </body>\n"
            "</html>";
    }

    // This is NOT THREAD-SAFE!. Needs to be locked at a higher level.
    GT::String Log_ToHTML(const char *value)
    {
        Strings::Replacer LogReplacer;
        LogReplacer.SetBase(value);
        LogReplacer.Replace("<",    "&lt;");
        LogReplacer.Replace(">",    "&gt;");
        LogReplacer.Replace(" ",    "&nbsp;");
        LogReplacer.Replace("\n\r", "<br />");
        LogReplacer.Replace("\n",   "<br />");
        LogReplacer.Replace("\t",   "&nbsp&nbsp&nbsp&nbsp");

        return LogReplacer.GetBase();
    }

    const char * LogFile::BuildHTMLEntry(const char *value) const
    {
        // Set the locale for the date.
        DateTime now(DateTime::Now());
        setlocale(LC_ALL, "");
        String date = now.ToShortString();
        setlocale(LC_ALL, "C");

        Strings::List<char> output;

        output.Append(
            "                <tr >\n"
            "                    <td class=\"row-left\">"); output.Append(date.c_str()); output.Append("</td>\n");
        output.Append(
            "                    <td class=\"row-right\">"); output.Append(Log_ToHTML(value).c_str()); output.Append("</td>\n");
        output.Append(
            "                </tr>\n");

        LogTempBuffer.Allocate(output.BuildString(nullptr));
        output.BuildString((char *)LogTempBuffer.GetDataPointer());

        return reinterpret_cast<const char*>(LogTempBuffer.GetDataPointer());
    }

    const char * LogFile::BuildTextEntry(const char *value) const
    {
        DateTime now(DateTime::Now());
        setlocale(LC_ALL, "");
        String date = now.ToShortString();
        setlocale(LC_ALL, "C");

        Strings::List<char> output;
        output.Append(date.c_str());
        output.Append(value);
        output.Append(newline);

        LogTempBuffer.Allocate(output.BuildStringSize());
        output.BuildString((char *)LogTempBuffer.GetDataPointer());

        return (const char *)LogTempBuffer.GetDataPointer();
    }


    void LogFile::OnOpen(const char *filename, const char *title)
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

    void LogFile::OnClose()
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

    void LogFile::OnWrite(const char *msg)
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
