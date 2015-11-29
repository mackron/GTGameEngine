// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Logging
#define GT_Logging

namespace GT
{
    /// Logs a message.
    void LogString(const char* value);
    void Log(const char* format, ...);
}

#endif