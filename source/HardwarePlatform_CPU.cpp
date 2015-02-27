// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/HardwarePlatform_CPU.hpp>

namespace GT
{
    HardwarePlatform_CPU::HardwarePlatform_CPU()
    {
    }

    HardwarePlatform_CPU::~HardwarePlatform_CPU()
    {
    }


    ResultCode HardwarePlatform_CPU::Startup()
    {
        return 0;   // No error.
    }

    void HardwarePlatform_CPU::Shutdown()
    {
    }
}
