// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_D3D11_hpp_
#define __GT_GE_GPURenderingDevice_D3D11_hpp_

#include "GPURenderingDevice_Gen2.hpp"

namespace GT
{
    namespace GE
    {
        /// Class representing a rendering D3D9-generation GPU device.
        class GPURenderingDevice_D3D11 : public GPURenderingDevice_Gen2
        {
        public:

            /// Constructor.
            GPURenderingDevice_D3D11();

            /// Destructor.
            virtual ~GPURenderingDevice_D3D11();






        private:    // No copying.
            GPURenderingDevice_D3D11(const GPURenderingDevice_D3D11 &);
            GPURenderingDevice_D3D11 & operator=(const GPURenderingDevice_D3D11 &);
        };
    }
}

#endif