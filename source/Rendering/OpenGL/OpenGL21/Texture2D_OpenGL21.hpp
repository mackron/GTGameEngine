// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_Texture2D_OpenGL21_hpp_
#define __GTEngine_Rendering_Texture2D_OpenGL21_hpp_

#include <GTEngine/Rendering/Texture2D.hpp>
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL21.hpp"


namespace GTEngine
{
    class Texture2D_OpenGL21 : public Texture2D
    {
    public:

        /// Constructor.
        Texture2D_OpenGL21(TextureState_OpenGL21* stateGLIn, GLenum targetIn);

        /// Destructor.
        ~Texture2D_OpenGL21();


        /// Retrieves a pointer to the OpenGL state object.
        TextureState_OpenGL21* GetOpenGLState() const;

        /// Retrieves the OpenGL texture target for this texture.
        GLenum GetTarget() const;



    private:

        /// A pointer to the structure containing the OpenGL state.
        TextureState_OpenGL21* stateGL;

        /// The texture target.
        GLenum target;
        
        
    private:    // No copying.
        Texture2D_OpenGL21(const Texture2D_OpenGL21 &);
        Texture2D_OpenGL21 & operator=(const Texture2D_OpenGL21 &);
    };
}

#endif
