// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_Gen1_hpp_
#define __GT_GE_GPURenderingDevice_Gen1_hpp_

#include "GPURenderingDevice.hpp"

namespace GT
{
    namespace GE
    {
        /// Class representing a rendering D3D9-generation GPU device.
        class GPURenderingDevice_Gen1 : public GPURenderingDevice
        {
        public:

            /// Constructor.
            GPURenderingDevice_Gen1(const GPURenderingDeviceInfo &info);

            /// Destructor.
            ~GPURenderingDevice_Gen1();


            /// GPURenderingDevice::GetGeneration().
            unsigned int GetGeneration() const;



        private:    // No copying.
            GPURenderingDevice_Gen1(const GPURenderingDevice_Gen1 &);
            GPURenderingDevice_Gen1 & operator=(const GPURenderingDevice_Gen1 &);
        };
    }
}

#endif