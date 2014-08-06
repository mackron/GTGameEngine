// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_TextureCube_hpp_
#define __GTEngine_TextureCube_hpp_

#include "Texture2D.hpp"

namespace GTEngine
{
    /// Class representing a cube map.
    class TextureCube
    {
    protected:

        /// Constructor.
        TextureCube(Texture2D* positiveX, Texture2D* negativeX, Texture2D* positiveY, Texture2D* negativeY, Texture2D* positiveZ, Texture2D* negativeZ);


    public:

        /// Destructor.
        virtual ~TextureCube();



        /// Sets the minification filter.
        ///
        /// @param newFilter [in] The new minification filter.
        void SetMinificationFilter(TextureFilter newFilter);

        /// Sets the magnification filter.
        ///
        /// @param newFilter [in] The new magnification filter.
        void SetMagnificationFilter(TextureFilter newFilter);

        /// Retrieves the minification filter.
        TextureFilter GetMinificationFilter() const;

        /// Retrieves the magnification filter.
        TextureFilter GetMagnificationFilter() const;


        /// Helper method for setting both the minification and magnification filter.
        ///
        /// @param newMinFilter [in] The new minification filter.
        /// @param newMagFilter [in] The new magnification filter.
        void SetFilter(TextureFilter newMinFilter, TextureFilter newMagFilter)
        {
            this->SetMinificationFilter(newMinFilter);
            this->SetMagnificationFilter(newMagFilter);
        }



        /// Sets the level of anisotropic filtering to use with this texture.
        /// @param newAnisotropy [in] The new level of anisotropic filtering to use. This is clamped against the renderers maxiumum.
        void SetAnisotropy(unsigned int newAnisotropy);

        /// Retrieves the level of anisotropic filtering being used with this texture.
        unsigned int GetAnisotropy() const;



    public:

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Face textures. Unfortunately these must be pointers because it's important that the cube map is created before the faces. These will bever be null.

        Texture2D* PositiveX;
        Texture2D* NegativeX;
        Texture2D* PositiveY;
        Texture2D* NegativeY;
        Texture2D* PositiveZ;
        Texture2D* NegativeZ;



    private:

        /// The current minification filter.
        TextureFilter minFilter;

        /// The current magnification filter.
        TextureFilter magFilter;

        /// The level of aniostropic filtering to use with the image. Defaults to 1.
        unsigned int anisotropy;

        /// The list of shaders that this texture is attached to.
        GTLib::List<Shader*> shaders;
        
        
        
    private:    // No copying.
        TextureCube(const TextureCube &);
        TextureCube & operator=(const TextureCube &);
    };
}

#endif
