// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    namespace GE
    {
        EngineContext::EngineContext()
        {
        }

        EngineContext::~EngineContext()
        {
        }


        ResultCode EngineContext::Startup(int argc, char** argv)
        {
            (void)argc;
            (void)argv;

            return 0;
        }

        void EngineContext::Shutdown()
        {
        }
    }
}