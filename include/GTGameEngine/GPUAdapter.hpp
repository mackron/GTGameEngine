// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUAdapter_hpp_
#define __GT_GE_GPUAdapter_hpp_

namespace GT
{
    namespace GE
    {
        /// Class representing a physical GPU adapter.
        class GPUAdapter
        {
        public:

            /// Constructor.
            GPUAdapter();

            /// Destructor.
            ~GPUAdapter();



        private:    // No copying.
            GPUAdapter(const GPUAdapter &);
            GPUAdapter & operator=(const GPUAdapter &);
        };
    }
}

#endif
