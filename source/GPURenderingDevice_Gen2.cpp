// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GPURenderingDevice_Gen2.hpp>

namespace GT
{
    GPURenderingDevice_Gen2::GPURenderingDevice_Gen2(const GPURenderingDeviceInfo &info)
        : GPURenderingDevice(info)
    {
    }

    GPURenderingDevice_Gen2::~GPURenderingDevice_Gen2()
    {
    }


    unsigned int GPURenderingDevice_Gen2::GetGeneration() const
    {
        return 2;
    }
}
