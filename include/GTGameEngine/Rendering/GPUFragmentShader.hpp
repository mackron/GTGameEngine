// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUFragmentShader_hpp_
#define __GT_GE_GPUFragmentShader_hpp_

namespace GT
{
    /// Class representing a fragment shader.
    class GPUFragmentShader
    {
    public:

        /// Constructor.
        GPUFragmentShader();

        /// Destructor.
        virtual ~GPUFragmentShader();





    private:    // No copying.
        GPUFragmentShader(const GPUFragmentShader &);
        GPUFragmentShader & operator=(const GPUFragmentShader &);
    };
}

#endif
