// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Log_hpp_
#define __GTLib_Log_hpp_

#include "Vector.hpp"
#include <easy_fs/easy_vfs.h>

namespace GT
{
    /**
    *   \brief  The base class for log event handlers.
    */
    class LogEventHandler
    {
    public:
        virtual ~LogEventHandler() {}

    public:

        virtual void OnOpen(const char *filename);
        virtual void OnClose();
        virtual void OnWrite(const char *msg);

        virtual void OnOpen(const char *filename, const char *title);
    };
}


namespace GT
{
    /**
    *   \brief  A class representing a log file.
    *
    *   A log can be written as a HTML file, or as a plain text file. If the extension of the file name is .html, it will
    *   be output as HTML. Otherwise, it will just be a plain text file.
    */
    class LogFile
    {
    public:

        /**
        *   \brief  Default constructor.
        *
        *   \remarks
        *       When using this constructor, the log must first be opened with open().
        */
        LogFile();

        /**
        *   \brief                Constructor.
        *   \param  fileName [in] The file name of the log. See remarks.
        *
        *   \remarks
        *       If <code>fileName<code>'s extension is .html, the log will be outputted to a HTML file, in HTML format. Anything
        *       will be a plain text log.
        *       \par
        *       This constructor will attempt to open the log. Use IsOpen() to determine if the log was opened successfully.
        */
        LogFile(easyvfs_context* pVFS, const char *fileName, const char *title);

        /**
        *   \brief  Opens a log for writing.
        *   \return True if the log is opened successfully; false otherwise.
        *
        *   \remarks
        *       A log must be opened before it can be used.
        */
        bool Open(easyvfs_context* pVFS, const char *fileName, const char *title);

        /**
        *   \brief  Closes the log.
        */
        void Close();

        /**
        *   \brief  Determines whether or not the log is open.
        *   \return True if the log is open and useable; false otherwise.
        */
        bool IsOpen() const { return this->isOpen; }

        /**
        *   \brief             Writes a string to the log.
        *   \param  value [in] The value to be written to the log.
        *
        *   \remarks
        *       Each call to this function will write the value on a separate line.
        */
        LogFile & WriteString(const char* value);
        LogFile & Write(const char *value, ...);

        /**
        *   \brief  Operator for writing a value to the log. Same as write().
        */
        LogFile & operator << (const char *value) { return this->Write(value); }


        /**
        *   \brief  Attaches an event handler to the log.
        *
        *   \remarks
        *       Remember that the log object will post an OnClose() event when it is destroyed. Therefore, ensure the
        *       event handlers are still valid when the log is closed. Alternatively, detach the event handlers before
        *       the log is closed.
        */
        void AttachEventHandler(LogEventHandler *eventHandler);

        /**
        *   \brief  Detaches an event handler from the log.
        */
        void DetachEventHandler(LogEventHandler *eventHandler);


    private:

        // The methods below are NOT thread-safe!
        const char * BuildHTMLHead(const char *title) const;
        const char * BuildHTMLTail() const;
        const char * BuildHTMLEntry(const char *value) const;
        const char * BuildTextEntry(const char *value) const;

    // Methods below are helpers for fireing events.
    private:

        void OnOpen(const char *filename, const char *title);
        void OnClose();
        void OnWrite(const char *msg);


    private:

        // The file the log will be written to.
        easyvfs_file* file;

        // The format of the log.
        unsigned int format;

        // Keeps track of whether or not the log is open.
        bool isOpen;

        /// The position in the HTML log where the next entry will be written.
        int64_t currentHTMLPosition;

        /// The list of event handlers attached to this log.
        Vector<LogEventHandler *> eventHandlers;


    private:    // No copying.
        LogFile(const LogFile &);
        LogFile & operator=(const LogFile &);

    };

    // The global Log object for use by applications. Lick libraries will write to this log. An application can attach
    // event handlers to the log for intercepting log messages.
    extern LogFile GlobalLog;
}

#endif
