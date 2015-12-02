// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_TextureCube_OpenGL21
#define GT_Rendering_TextureCube_OpenGL21

#include <GTGE/Rendering/TextureCube.hpp>
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL21.hpp"


namespace GT
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
