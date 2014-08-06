// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_State_OpenGL21_hpp_
#define __GTEngine_Rendering_State_OpenGL21_hpp_

#include "RCSetGlobalState_OpenGL21.hpp"
#include "RCClear_OpenGL21.hpp"
#include "RCDraw_OpenGL21.hpp"
#include "RCCreateVertexArray_OpenGL21.hpp"
#include "RCDeleteVertexArray_OpenGL21.hpp"
#include "RCSetVertexArrayState_OpenGL21.hpp"
#include "RCCreateTexture_OpenGL21.hpp"
#include "RCDeleteTexture_OpenGL21.hpp"
#include "RCSetTextureState_OpenGL21.hpp"
#include "RCCreateShader_OpenGL21.hpp"
#include "RCDeleteShader_OpenGL21.hpp"
#include "RCSetShaderState_OpenGL21.hpp"
#include "RCCreateFramebuffer_OpenGL21.hpp"
#include "RCDeleteFramebuffer_OpenGL21.hpp"
#include "RCSetFramebufferState_OpenGL21.hpp"

#include "../CapabilityBits.hpp"
#include "TextureState_OpenGL21.hpp"
#include "ShaderState_OpenGL21.hpp"
#include "FramebufferState_OpenGL21.hpp"

#include <gtgl/gtgl.h>
#include <cstdint>

#include <GTLib/Vector.hpp>

namespace GTEngine
{
    /// The current state of the OpenGL21 renderer.
    ///
    /// We use basic OpenGL types for the state.
    class State_OpenGL21
    {
    public:

        /// The state of the current shader program.
        ShaderState_OpenGL21* currentProgramState;

        /// A pointer to the OpenGL object of the current framebuffer.
        FramebufferState_OpenGL21* currentFramebufferState;


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
        GTLib::Vector<TextureState_OpenGL21*> instantiatedTextureObjects;

        /// The list of currently instantiated shader programs.
        GTLib::Vector<ShaderState_OpenGL21*> instantiatedProgramObjects;

        /// The list of currently instantiated buffer objects.
        GTLib::Vector<GLuint*> instantiatedBufferObjects;

        /// The list of currently instantiated framebuffer objects.
        GTLib::Vector<FramebufferState_OpenGL21*> instantiatedFramebufferObjects;
        

        /// The list texture objects that need to be deleted.
        GTLib::Vector<TextureState_OpenGL21*> deletedTextureObjects[2];

        /// The list of shader programs that need to be deleted.
        GTLib::Vector<ShaderState_OpenGL21*> deletedProgramObjects[2];

        /// The list of buffer objects that need to be deleted.
        GTLib::Vector<GLuint*> deletedBufferObjects[2];

        /// The list of framebuffer objects that need to be deleted.
        GTLib::Vector<FramebufferState_OpenGL21*> deletedFramebufferObjects[2];


        /// The current swap interval.
        int swapInterval;

        /// Keeps track of whether or not the swap interval needs to be updated.
        bool swapIntervalNeedsUpdate;


        /// The index of the back index. This is needed in determining which list to delete resource from.
        int backIndex;



        /// Constructor.
        State_OpenGL21();

        /// Destructor.
        ~State_OpenGL21();

        
        /// Marks a texture as deleted.
        void MarkTextureObjectAsDeleted(TextureState_OpenGL21* textureObject);

        /// Marks a shader program as deleted.
        void MarkProgramObjectAsDeleted(ShaderState_OpenGL21* programObject);

        /// Marks a buffer object as deleted.
        void MarkBufferObjectAsDeleted(GLuint* bufferObject);

        /// Marks a framebuffer object as deleted.
        void MarkFramebufferObjectAsDeleted(FramebufferState_OpenGL21* framebufferObject);


        /// Clears (deletes) OpenGL objects that have been marked for deletion.
        void ClearDeletedOpenGLObjects();


        /// Shuts down the state by clearing all objects waiting for deletion and whatnot.
        void Shutdown();


    private:    // No copying.
        State_OpenGL21(const State_OpenGL21 &);
        State_OpenGL21 & operator=(const State_OpenGL21 &);
    };
}

#endif