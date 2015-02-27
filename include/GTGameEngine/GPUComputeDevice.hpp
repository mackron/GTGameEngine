// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUComputeDevice_hpp_
#define __GT_GE_GPUComputeDevice_hpp_

namespace GT
{
    /// Class representing a GPU compute device.
    class GPUComputeDevice
    {
    public:

        /// Constructor.
        GPUComputeDevice();

        /// Destructor.
        ~GPUComputeDevice();






    private:    // No copying.
        GPUComputeDevice(const GPUComputeDevice &);
        GPUComputeDevice & operator=(const GPUComputeDevice &);
    };
}

#endif