// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GPURenderingDevice_Gen3.hpp>

namespace GT
{
    namespace GE
    {
        GPURenderingDevice_Gen3::GPURenderingDevice_Gen3(const GPURenderingDeviceInfo &info)
            : GPURenderingDevice(info)
        {
        }

        GPURenderingDevice_Gen3::~GPURenderingDevice_Gen3()
        {
        }


        unsigned int GPURenderingDevice_Gen3::GetGeneration() const
        {
            return 3;
        }
    }
}
