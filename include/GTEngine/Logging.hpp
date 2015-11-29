// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Logging_hpp_
#define __GTEngine_Logging_hpp_

namespace GT
{
    /// Logs a message.
    void LogString(const char* value);
    void Log(const char* format, ...);
}

#endif