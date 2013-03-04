// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCSetTextureState_hpp_
#define __GTEngine_Rendering_OpenGL33_RCSetTextureState_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <GTImage/ImageFormats.hpp>
#include <GTCore/Map.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

#include "ShaderState_OpenGL33.hpp"

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCSetTextureState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetTextureState();


        /// Renderer::PushTexture1DData().
        void SetTexture1DData(GLuint* textureObject, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, const void* data, size_t dataSizeInBytes);

        /// Renderer::PushTexture2DData().
        void SetTexture2DData(GLuint* textureObject, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, const void* data, size_t dataSizeInBytes);

        /// Renderer::PushTexture3DData().
        void SetTexture3DData(GLuint* textureObject, GLenum target, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* data, size_t dataSizeInBytes);


        /// Renderer::SetTexture*Filter().
        void SetTextureFilter(GLuint* textureObject, GLenum target, GLint minification, GLint magnification);

        /// Renderer::SetTexture*Anisotropy().
        void SetTextureAnisotropy(GLuint* textureObject, GLenum target, GLint anisotropy);

        /// Renderer::SetTexture*WrapMode().
        void SetTextureWrapMode(GLuint* textureObject, GLenum target, GLint wrapMode);

        /// Renderer::SetTexture*MipmapLevels().
        void SetTextureMipmapLevels(GLuint* textureObject, GLenum target, GLint baseLevel, GLint maxLevel);

        /// Renderer::SetTexture*GenerateMipmaps().
        void GenerateTextureMipmaps(GLuint* textureObject, GLenum target);



        /// Retrieves the texture object whose state is being set.
        GLuint* GetTextureObject();

        /// Retrieves the target (texture type) of the state being set.
        GLenum GetTarget();


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// The texture object whose state is being modified.
        GLuint* textureObject;

        /// The texture target.
        GLenum target;


        /// The vertex data to set.
        struct TextureData
        {
            GLint   internalFormat;
            GLenum  format;
            GLenum  type;
            GLsizei width;
            GLsizei height;
            GLsizei depth;
            GLvoid* data;
        };

        /// The map of TextureData objects for each mipmap whose data is getting set. The key is the mipmap level.
        GTCore::Map<GLint, TextureData> mipmapDatas;



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
