// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL21_RCSetTextureState_hpp_
#define __GTEngine_Rendering_OpenGL21_RCSetTextureState_hpp_

#include <GTImage/ImageFormats.hpp>
#include <GTLib/Map.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

#include "../../RenderCommand.hpp"
#include "TextureState_OpenGL21.hpp"
#include "ShaderState_OpenGL21.hpp"

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCSetTextureState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetTextureState();

        /// Destructor.
        ~RCSetTextureState();


        /// Renderer::PushTexture1DData().
        void SetTexture1DData(TextureState_OpenGL21* textureState, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, const void* data, size_t dataSizeInBytes);


        /// Renderer::PushTexture2DData().
        void SetTexture2DData(TextureState_OpenGL21* textureState, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, const void* data, size_t dataSizeInBytes, bool flip);
        
        /// Renderer::PushTexture2DSubData().
        void SetTexture2DSubData(TextureState_OpenGL21* textureState, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height, const void* data, size_t dataSizeInBytes, bool flip);


        /// Renderer::PushTexture3DData().
        void SetTexture3DData(TextureState_OpenGL21* textureState, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* data, size_t dataSizeInBytes);


        /// Renderer::PushTextureCubeData().
        void SetTextureCubeData(TextureState_OpenGL21* textureState, GTImage::ImageFormat format, unsigned int width, unsigned int height, size_t dataSizeInBytes,
            const void* positiveXData, const void* negativeXData,
            const void* positiveYData, const void* negativeYData,
            const void* positiveZData, const void* negativeZData);


        /// Renderer::SetTexture*Filter().
        void SetTextureFilter(TextureState_OpenGL21* textureState, GLenum target, GLint minification, GLint magnification);

        /// Renderer::SetTexture*Anisotropy().
        void SetTextureAnisotropy(TextureState_OpenGL21* textureState, GLenum target, GLint anisotropy);

        /// Renderer::SetTexture*WrapMode().
        void SetTextureWrapMode(TextureState_OpenGL21* textureState, GLenum target, GLint wrapMode);

        /// Renderer::SetTexture*MipmapLevels().
        void SetTextureMipmapLevels(TextureState_OpenGL21* textureState, GLenum target, GLint baseLevel, GLint maxLevel);

        /// Renderer::SetTexture*GenerateMipmaps().
        void GenerateTextureMipmaps(TextureState_OpenGL21* textureState, GLenum target);



        /// Retrieves the texture object whose state is being set.
        TextureState_OpenGL21* GetTextureState();

        /// Retrieves the target (texture type) of the state being set.
        GLenum GetTarget();


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// The texture object whose state is being modified.
        TextureState_OpenGL21* textureState;

        /// The texture target.
        GLenum target;


        /// The texture data to set. This does not do cube maps.
        struct TextureData
        {
            GLint   mipmapIndex;
            GLint   internalFormat;
            GLenum  format;
            GLenum  type;
            GLint   xOffset;    // <-- Only used with sub-region sets.
            GLint   yOffset;    // <-- Only used with sub-region sets.
            GLint   zOffset;    // <-- Only used with sub-region sets.
            GLsizei width;
            GLsizei height;
            GLsizei depth;
            GLvoid* data;
            
        }textureData;


        /// The cube map texture data.
        struct CubeMapTextureData
        {
            GLint   internalFormat;
            GLenum  format;
            GLenum  type;
            GLsizei width;
            GLsizei height;
            GLvoid* positiveXData;
            GLvoid* negativeXData;
            GLvoid* positiveYData;
            GLvoid* negativeYData;
            GLvoid* positiveZData;
            GLvoid* negativeZData;

        }cubeMapData;




        /// The new filters.
        struct
        {
            GLint minification;
            GLint magnification;

        }filters;

        /// The new anisotropy.
        GLint anisotropy;

        /// The new wrapping mode.
        GLint wrapMode;

        /// The new mipmap levels.
        struct
        {
            GLint baseLevel;
            GLint maxLevel;

        }mipmapLevels;



    private:    // No copying.
        RCSetTextureState(const RCSetTextureState &);
        RCSetTextureState & operator=(const RCSetTextureState &);
    };
}

#endif
