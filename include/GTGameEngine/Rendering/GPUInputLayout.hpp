// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUInputLayout_hpp_
#define __GT_GE_GPUInputLayout_hpp_

namespace GT
{
    /// Base class representing the layout of a vertex shader's input.
    class GPUInputLayout
    {
    public:

        /// Constructor.
        GPUInputLayout();

        /// Destructor
        virtual ~GPUInputLayout();



    private:    // No copying.
        GPUInputLayout(const GPUInputLayout &);
        GPUInputLayout & operator=(const GPUInputLayout &);
    };
}

#endif
