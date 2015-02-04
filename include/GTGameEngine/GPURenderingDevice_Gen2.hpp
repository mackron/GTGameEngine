// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_Gen2_hpp_
#define __GT_GE_GPURenderingDevice_Gen2_hpp_

#include "GPURenderingDevice.hpp"

namespace GT
{
    namespace GE
    {
        /// Class representing a rendering D3D9-generation GPU device.
        class GPURenderingDevice_Gen2 : public GPURenderingDevice
        {
        public:

            /// Constructor.
            GPURenderingDevice_Gen2(const GPURenderingDeviceInfo &info);

            /// Destructor.
            ~GPURenderingDevice_Gen2();


            /// GPURenderingDevice::GetGeneration().
            unsigned int GetGeneration() const;



        private:    // No copying.
            GPURenderingDevice_Gen2(const GPURenderingDevice_Gen2 &);
            GPURenderingDevice_Gen2 & operator=(const GPURenderingDevice_Gen2 &);
        };
    }
}

#endif