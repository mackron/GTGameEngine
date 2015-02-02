// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Framebuffer_D3D11_hpp_
#define __GT_GE_Framebuffer_D3D11_hpp_

#include "Config.hpp"
#include <GTGameEngine/Rendering/Framebuffer.hpp>

namespace GT
{
    namespace GE
    {
        /// D3D11 implementation of the Framebuffer class.
        class Framebuffer_D3D11 : public Framebuffer
        {
        public:

            /// Constructor.
            Framebuffer_D3D11();

            /// Destructor.
            ~Framebuffer_D3D11();



        private:    // No copying.
            Framebuffer_D3D11(const Framebuffer_D3D11 &);
            Framebuffer_D3D11 & operator=(const Framebuffer_D3D11 &);
        };
    }
}

#endif
