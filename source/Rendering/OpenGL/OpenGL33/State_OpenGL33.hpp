// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_State_OpenGL33_hpp_
#define __GTEngine_Rendering_State_OpenGL33_hpp_

#include "RCClear.hpp"
#include "RCSetGlobalState.hpp"
#include "RCCreateVertexArray.hpp"
#include "RCDeleteVertexArray.hpp"

#include "../CapabilityBits.hpp"

#include <gtgl/gtgl.h>
#include <cstdint>

#include <GTCore/Vector.hpp>

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



        /// The list of currently instantiated textures. This includes all texture types.
        GTCore::Vector<GLuint*> currentTextureObjects;

        /// The list of currently instantiated shader programs.
        GTCore::Vector<GLuint*> currentProgramObjects;

        /// The list of currently instantiated VAO's.
        GTCore::Vector<GLuint*> currentVertexArrayObjects;

        /// The list of currently instantiated buffer objects.
        GTCore::Vector<GLuint*> currentBufferObjects;

        /// The list of currently instantiated framebuffer objects.
        GTCore::Vector<GLuint*> currentFramebufferObjects;
        

        /// The list texture objects that need to be deleted.
        GTCore::Vector<GLuint*> deletedTextureObjects;

        /// The list of shader programs that need to be deleted.
        GTCore::Vector<GLuint*> deletedProgramObjects;

        /// The list of VAO's that need to be deleted.
        GTCore::Vector<GLuint*> deletedVertexArrayObjects;

        /// The list of buffer objects that need to be deleted.
        GTCore::Vector<GLuint*> deletedBufferObjects;

        /// The list of framebuffer objects that need to be deleted.
        GTCore::Vector<GLuint*> deletedFramebufferObjects;



        /// Constructor.
        State_OpenGL33();

        /// Destructor.
        ~State_OpenGL33();

        /// Clears (deletes) OpenGL objects that have been marked for deletion.
        void ClearDeletedOpenGLObjects();


    private:    // No copying.
        State_OpenGL33(const State_OpenGL33 &);
        State_OpenGL33 & operator=(const State_OpenGL33 &);
    };
}

#endif