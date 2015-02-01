// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GPURenderingDevice_Gen1.hpp>

namespace GT
{
    namespace GE
    {
        GPURenderingDevice_Gen1::GPURenderingDevice_Gen1(const GPURenderingDeviceInfo &info)
            : GPURenderingDevice(info)
        {
        }

        GPURenderingDevice_Gen1::~GPURenderingDevice_Gen1()
        {
        }


        unsigned int GPURenderingDevice_Gen1::GetGeneration() const
        {
            return 1;
        }
    }
}
