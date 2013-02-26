// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_State_OpenGL33_hpp_
#define __GTEngine_Rendering_State_OpenGL33_hpp_

#include "RCClear.hpp"
#include "RCSetGlobalState.hpp"
#include "../CapabilityBits.hpp"
#include <gtgl/gtgl.h>
#include <cstdint>

namespace GTEngine
{
    /// The current state of the OpenGL33 renderer.
    ///
    /// We use basic OpenGL types for the state.
    class State_OpenGL33
    {
    public:

        ///////////////////////////////////////////////////////////
        // Current Bindings.
        //
        // For bindings we actually store pointers to the OpenGL objects. The reason for this is that we may need to call glGen*()
        // somewhere in the pipeline. glGen*() will need to reference a variable that was declared at a higher level.
        GLuint* currentTexture1D;
        GLuint* currentTexture2D;
        GLuint* currentTexture3D;
        GLuint* currentTextureCube;
        GLuint* currentProgram;
        GLuint* currentVAO;
        GLuint* currentFBO;

        /// Enabled states. If the bit is set, it's enabled.
        uint32_t enabledStates;

        /// The current stencil mask.
        int stencilMask;


        
        // Pointers to the current render command for a particular operation. When null, a new one needs to be created and set.
        RCClear*          currentRCClear;
        RCSetGlobalState* currentRCSetGlobalState;



        /// Constructor.
        State_OpenGL33();

        /// Copy constructor.
        State_OpenGL33(const State_OpenGL33 &other);

        /// Assignment operator.
        State_OpenGL33 & operator=(const State_OpenGL33 &other);
    };
}

#endif