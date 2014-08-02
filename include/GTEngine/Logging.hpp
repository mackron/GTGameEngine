// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Logging_hpp_
#define __GTEngine_Logging_hpp_

namespace GTEngine
{
    /// Logs a message.
    void LogString(const char* value);
    void Log(const char* format, ...);
}

#endif