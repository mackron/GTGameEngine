// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/HardwarePlatform_GPU.hpp>

namespace GT
{
    namespace GE
    {
        HardwarePlatform_GPU::HardwarePlatform_GPU()
        {
        }

        HardwarePlatform_GPU::~HardwarePlatform_GPU()
        {
        }


        ResultCode HardwarePlatform_GPU::Startup()
        {
            return 0;   // No error.
        }

        void HardwarePlatform_GPU::Shutdown()
        {
        }
    }
}
