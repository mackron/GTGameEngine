// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_TextureCube_hpp_
#define __GTEngine_TextureCube_hpp_

#include "Texture2D.hpp"

namespace GTEngine
{
    /// Class representing a cube map.
    class TextureCube
    {
    public:

        /// Constructor.
        TextureCube(Texture2D* positiveX, Texture2D* negativeX, Texture2D* positiveY, Texture2D* negativeY, Texture2D* positiveZ, Texture2D* negativeZ);

        /// Destructor.
        ~TextureCube();



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

        /// Retrieves a pointer to the internal renderer data.
        ///
        /// @remarks
        ///     This should only be used by the renderer.
              void* GetRendererData()       { return this->rendererData; }
        const void* GetRendererData() const { return this->rendererData; }


        /// Sets the pointer to the internal renderer data.
        ///
        /// @remarks
        ///     This does not deallocate the previous renderer data. That is the responsibility of the renderer itself.
        void SetRendererData(void* rendererData) { this->rendererData = rendererData; }


        /// Called when the texture is attached to a shader.
        void OnAttachToShader(Shader &shader);

        /// Called when the texture is detached from a shader.
        void OnDetachFromShader(Shader &shader);



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

        /// The renderer will need to store it's own properties about the texture. This pointer can be used by the renderer
        /// to hold a pointer to some renderer-specific data.
        void* rendererData;

        /// The list of shaders that this texture is attached to.
        GTCore::List<Shader*> shaders;
        
        
        
    private:    // No copying.
        TextureCube(const TextureCube &);
        TextureCube & operator=(const TextureCube &);
    };
}

#endif
