// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_Texture2D_OpenGL33_hpp_
#define __GTEngine_Rendering_Texture2D_OpenGL33_hpp_

#include <GTEngine/Rendering/Texture2D.hpp>
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL33.hpp"


namespace GTEngine
{
    class Texture2D_OpenGL33 : public Texture2D
    {
    public:

        /// Constructor.
        Texture2D_OpenGL33(TextureState_OpenGL33* stateGLIn, GLenum targetIn);

        /// Destructor.
        ~Texture2D_OpenGL33();


        /// Retrieves a pointer to the OpenGL state object.
        TextureState_OpenGL33* GetOpenGLState() const;

        /// Retrieves the OpenGL texture target for this texture.
        GLenum GetTarget() const;



    private:

        /// A pointer to the structure containing the OpenGL state.
        TextureState_OpenGL33* stateGL;

        /// The texture target.
        GLenum target;
        
        
    private:    // No copying.
        Texture2D_OpenGL33(const Texture2D_OpenGL33 &);
        Texture2D_OpenGL33 & operator=(const Texture2D_OpenGL33 &);
    };
}

#endif
