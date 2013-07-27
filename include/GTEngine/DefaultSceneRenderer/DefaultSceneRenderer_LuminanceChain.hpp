// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_LuminanceChain_hpp_
#define __GTEngine_DefaultSceneRenderer_LuminanceChain_hpp_

#include <GTCore/Vector.hpp>

namespace GTEngine
{
    class Framebuffer;
    class Texture2D;
    class Shader;

    /// Class representing a chain of buffers that are used for calculating the luminance of a scene.
    ///
    /// The luminance calculation works by downsampling the original image. Each buffer in this chain is 1/4
    /// the size of the previous and is in R32F format texture (containing only the luminance value).
    ///
    /// The chain has a framebuffer object associated with it which is used for attaching the buffers to it. When
    /// Initialize() is called, it will attach as many render buffers as it can which will have the same index
    /// as it's index in the internal list.
    ///
    /// The chain can be re-sized with the Initialize() method.
    class DefaultSceneRenderer_LuminanceChain
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_LuminanceChain();

        /// Destructor.
        ~DefaultSceneRenderer_LuminanceChain();

        /// Performs the luminance computation from the given base image.
        ///
        /// @param baseImage [in] The base image whose luminance value is being computed.
        /// @param exposure  [in] The exposure to use.
        ///
        /// @remarks
        ///     The last buffer in the chain will be a 1x1 buffer containing the luminance value.
        void ComputeLuminance(const Texture2D &sourceImage, float exposure);

        /// Brief retrieve the 1x1 buffer that contains the luminance value as it's only texel.
        const Texture2D* GetLuminanceBuffer() const;


    private:

        /// Initializes the chain.
        ///
        /// @param baseWidth  [in] The width of the base image whose luminance is being calculated.
        /// @param baseHeight [in] The height of the base image whose luminance is being calculated.
        bool Initialize(unsigned int baseWidth, unsigned int baseHeight);

        /// Creates a shader for use when rendering a link in the chain.
        ///
        /// @param inputTextureWidth  [in] The width of the input texture.
        /// @param inputTextureHeight [in] The height of the input texture.
        Shader* CreateShader(unsigned int inputTextureWidth, unsigned int inputTextureHeight, bool encodeLuminance, bool decodeLuminance);

        /// Deletes every shader in m_shaders and clears the container.
        void DeleteAllShaders();

        /// Deletes every buffer in m_buffers and clears the container.
        void DeleteAllBuffers();


    private:

        /// The framebuffer to use when rendering.
        Framebuffer* m_framebuffer;

        /// The list of buffers to use in the downsampling. This does not include the original image.
        GTCore::Vector<Texture2D*> m_buffers;

        /// The list of shaders to use in the downsampling. There is one of these for each buffer. Each item maps to the
        /// corresponding item in m_buffers;
        GTCore::Vector<Shader*> m_shaders;

        /// The base width that was used when the chain was last initialized.
        unsigned int m_baseWidth;

        /// The base height that was used when the chain was last initialized.
        unsigned int m_baseHeight;
    };
}

#endif