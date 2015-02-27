// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUVertexShader_hpp_
#define __GT_GE_GPUVertexShader_hpp_

namespace GT
{
    /// Class representing a vertex shader.
    class GPUVertexShader
    {
    public:

        /// Constructor.
        GPUVertexShader();

        /// Destructor.
        virtual ~GPUVertexShader();





    private:    // No copying.
        GPUVertexShader(const GPUVertexShader &);
        GPUVertexShader & operator=(const GPUVertexShader &);
    };
}

#endif
