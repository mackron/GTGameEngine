// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    namespace GE
    {
        EngineContext::EngineContext()
            : m_hardwarePlatform()
        {
        }

        EngineContext::~EngineContext()
        {
        }


        ResultCode EngineContext::Startup(int argc, char** argv)
        {
            (void)argc;
            (void)argv;


            ResultCode result = 0;

            
            // Hardware platform.
            result = m_hardwarePlatform.Startup();
            if (Failed(result))
            {
                return result;
            }



            return result;
        }

        void EngineContext::Shutdown()
        {
            m_hardwarePlatform.Shutdown();
        }
    }
}