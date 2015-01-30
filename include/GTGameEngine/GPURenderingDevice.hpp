// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_hpp_
#define __GT_GE_GPURenderingDevice_hpp_

namespace GT
{
    namespace GE
    {
        /// Class representing a rendering GPU device.
        class GPURenderingDevice
        {
        public:

            /// Constructor.
            GPURenderingDevice();

            /// Destructor.
            virtual ~GPURenderingDevice();



            /// Retrieves the generation number for this renderer.
            ///
            /// @return An integer representing the generation of rendering technologies this rendering device supports. See remarks.
            ///
            /// @remarks
            ///     This is used in determining whether or not this device can be cased to GPURenderingDevice_Gen1 or GPURenderingDevice_Gen2. 
            ///     @par
            ///     If this returns 1, the rendering device supports Direct3D 9 / OpenGL 2.1 class of functionality.
            ///     @par
            ///     If this returns 2, the rendering device supports Direct3D 11 / OpenGL 4.5 class of functionality.
            virtual unsigned int GetGeneration() const = 0;



        private:    // No copying.
            GPURenderingDevice(const GPURenderingDevice &);
            GPURenderingDevice & operator=(const GPURenderingDevice &);
        };
    }
}

#endif