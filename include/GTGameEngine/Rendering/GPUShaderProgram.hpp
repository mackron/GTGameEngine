// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUShaderProgram_hpp_
#define __GT_GE_GPUShaderProgram_hpp_

namespace GT
{
    namespace GE
    {
        /// Class representing a shader program.
        class GPUShaderProgram
        {
        public:

            /// Constructor.
            GPUShaderProgram();

            /// Destructor.
            virtual ~GPUShaderProgram();






        private:    // No copying.
            GPUShaderProgram(const GPUShaderProgram &);
            GPUShaderProgram & operator=(const GPUShaderProgram &);
        };
    }
}

#endif
