// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_TextureCube_OpenGL21_hpp_
#define __GTEngine_Rendering_TextureCube_OpenGL21_hpp_

#include <GTEngine/Rendering/TextureCube.hpp>
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL21.hpp"


namespace GTEngine
{
    class TextureCube_OpenGL21 : public TextureCube
    {
    public:

        /// Constructor.
        TextureCube_OpenGL21(TextureState_OpenGL21* stateGLIn, Texture2D* positiveX, Texture2D* negativeX, Texture2D* positiveY, Texture2D* negativeY, Texture2D* positiveZ, Texture2D* negativeZ);

        /// Destructor.
        ~TextureCube_OpenGL21();


        /// Retrieves a pointer to the OpenGL object.
        TextureState_OpenGL21* GetOpenGLState() const;


    private:

        /// A pointer to the OpenGL object.
        TextureState_OpenGL21* stateGL;
        
        
    private:    // No copying.
        TextureCube_OpenGL21(const TextureCube_OpenGL21 &);
        TextureCube_OpenGL21 & operator=(const TextureCube_OpenGL21 &);
    };
}

#endif
