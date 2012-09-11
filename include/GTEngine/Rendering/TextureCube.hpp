
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
        TextureCube();

        /// Destructor.
        ~TextureCube();



        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Face textures. Unfortunately these must be pointers because it's important that the cube map is created before the faces. These will bever be null.

        Texture2D* PositiveX;
        Texture2D* NegativeX;
        Texture2D* PositiveY;
        Texture2D* NegativeY;
        Texture2D* PositiveZ;
        Texture2D* NegativeZ;



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


    private:

        /// The renderer will need to store it's own properties about the texture. This pointer can be used by the renderer
        /// to hold a pointer to some renderer-specific data.
        void* rendererData;

        /// The list of shaders that this texture is attached to.
        GTCore::List<Shader*> shaders;
    };
}

#endif
