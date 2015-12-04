// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_State_OpenGL21
#define GT_Rendering_State_OpenGL21

#include "../CapabilityBits.hpp"
#include "TextureState_OpenGL21.hpp"
#include "ShaderState_OpenGL21.hpp"
#include "FramebufferState_OpenGL21.hpp"

#include <gtgl/gtgl.h>
#include <cstdint>

#include <GTGE/Core/Vector.hpp>

namespace GT
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


        /// The list of currently instantiated textures. This includes all texture types.
        Vector<TextureState_OpenGL21*> instantiatedTextureObjects;

        /// The list of currently instantiated shader programs.
        Vector<ShaderState_OpenGL21*> instantiatedProgramObjects;

        /// The list of currently instantiated buffer objects.
        Vector<GLuint*> instantiatedBufferObjects;

        /// The list of currently instantiated framebuffer objects.
        Vector<FramebufferState_OpenGL21*> instantiatedFramebufferObjects;
        

        /// The list texture objects that need to be deleted.
        Vector<TextureState_OpenGL21*> deletedTextureObjects[2];

        /// The list of shader programs that need to be deleted.
        Vector<ShaderState_OpenGL21*> deletedProgramObjects[2];

        /// The list of buffer objects that need to be deleted.
        Vector<GLuint*> deletedBufferObjects[2];

        /// The list of framebuffer objects that need to be deleted.
        Vector<FramebufferState_OpenGL21*> deletedFramebufferObjects[2];


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