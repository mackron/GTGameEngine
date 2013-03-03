// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_State_OpenGL33_hpp_
#define __GTEngine_Rendering_State_OpenGL33_hpp_

#include "RCSetGlobalState.hpp"
#include "RCClear.hpp"
#include "RCDraw.hpp"
#include "RCCreateVertexArray.hpp"
#include "RCDeleteVertexArray.hpp"
#include "RCSetVertexArrayState.hpp"
#include "RCCreateTexture.hpp"
#include "RCDeleteTexture.hpp"
#include "RCSetTextureState.hpp"
#include "RCCreateShader.hpp"
#include "RCDeleteShader.hpp"
#include "RCSetShaderState.hpp"
#include "RCCreateFramebuffer.hpp"
#include "RCDeleteFramebuffer.hpp"
#include "RCSetFramebufferState.hpp"

#include "../CapabilityBits.hpp"
#include "ShaderState_OpenGL33.hpp"
#include "FramebufferState_OpenGL33.hpp"

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

        /// The state of the current shader program.
        ShaderState_OpenGL33* currentProgramState;

        /// A pointer to the OpenGL object of the current framebuffer.
        FramebufferState_OpenGL33* currentFramebufferState;


        /// Enabled states. If the bit is set, it's enabled.
        uint32_t enabledStates;

        /// The current stencil mask.
        int stencilMask;


        
        // Pointers to the current render command for a particular operation. When null, a new one needs to be created and set.
        RCSetGlobalState*      currentRCSetGlobalState;
        RCSetVertexArrayState* currentRCSetVertexArrayState;
        RCSetTextureState*     currentRCSetTextureState;
        RCSetShaderState*      currentRCSetShaderState;
        RCSetFramebufferState* currentRCSetFramebufferState;
        RCClear*               currentRCClear;
        RCDraw*                currentRCDraw;



        /// The list of currently instantiated textures. This includes all texture types.
        GTCore::Vector<GLuint*> instantiatedTextureObjects;

        /// The list of currently instantiated shader programs.
        GTCore::Vector<ShaderState_OpenGL33*> instantiatedProgramObjects;

        /// The list of currently instantiated VAO's.
        GTCore::Vector<GLuint*> instantiatedVertexArrayObjects;

        /// The list of currently instantiated buffer objects.
        GTCore::Vector<GLuint*> instantiatedBufferObjects;

        /// The list of currently instantiated framebuffer objects.
        GTCore::Vector<FramebufferState_OpenGL33*> instantiatedFramebufferObjects;
        

        /// The list texture objects that need to be deleted.
        GTCore::Vector<GLuint*> deletedTextureObjects;

        /// The list of shader programs that need to be deleted.
        GTCore::Vector<ShaderState_OpenGL33*> deletedProgramObjects;

        /// The list of VAO's that need to be deleted.
        GTCore::Vector<GLuint*> deletedVertexArrayObjects;

        /// The list of buffer objects that need to be deleted.
        GTCore::Vector<GLuint*> deletedBufferObjects;

        /// The list of framebuffer objects that need to be deleted.
        GTCore::Vector<FramebufferState_OpenGL33*> deletedFramebufferObjects;



        /// Constructor.
        State_OpenGL33();

        /// Destructor.
        ~State_OpenGL33();

        
        /// Marks a texture as deleted.
        void MarkTextureObjectAsDeleted(GLuint* textureObject);

        /// Marks a shader program as deleted.
        void MarkProgramObjectAsDeleted(ShaderState_OpenGL33* programObject);

        /// Marks a vertex array as deleted.
        void MarkVertexArrayObjectAsDeleted(GLuint* vertexArrayObject);

        /// Marks a buffer object as deleted.
        void MarkBufferObjectAsDeleted(GLuint* bufferObject);

        /// Marks a framebuffer object as deleted.
        void MarkFramebufferObjectAsDeleted(FramebufferState_OpenGL33* framebufferObject);


        /// Clears (deletes) OpenGL objects that have been marked for deletion.
        void ClearDeletedOpenGLObjects();


    private:    // No copying.
        State_OpenGL33(const State_OpenGL33 &);
        State_OpenGL33 & operator=(const State_OpenGL33 &);
    };
}

#endif