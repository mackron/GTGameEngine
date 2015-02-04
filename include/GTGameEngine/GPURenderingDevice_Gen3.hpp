// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_Gen3_hpp_
#define __GT_GE_GPURenderingDevice_Gen3_hpp_

#include "GPURenderingDevice.hpp"

namespace GT
{
    namespace GE
    {
        /// Class representing a rendering D3D9-generation GPU device.
        class GPURenderingDevice_Gen3 : public GPURenderingDevice
        {
        public:

            /// Constructor.
            GPURenderingDevice_Gen3(const GPURenderingDeviceInfo &info);

            /// Destructor.
            virtual ~GPURenderingDevice_Gen3();


            /// GPURenderingDevice::GetGeneration().
            unsigned int GetGeneration() const;



        private:    // No copying.
            GPURenderingDevice_Gen3(const GPURenderingDevice_Gen3 &);
            GPURenderingDevice_Gen3 & operator=(const GPURenderingDevice_Gen3 &);
        };
    }
}

#endif