// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Framebuffer_hpp_
#define __GT_GE_Framebuffer_hpp_

namespace GT
{
    /// Class representing a framebuffer for the rendering system.
    class Framebuffer
    {
    public:

        /// Constructor.
        Framebuffer();

        /// Destructor.
        virtual ~Framebuffer();






    private:    // No copying.
        Framebuffer(const Framebuffer &);
        Framebuffer & operator=(const Framebuffer&);
    };
}

#endif // !__GT_GE_Framebuffer_hpp_
