// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetTextureState.hpp"
#include "../TypeConversion.hpp"
#include <gtgl/gtgl.h>
#include <assert.h>

namespace GTEngine
{
    #define SET_DATA_1D_BIT         (1 << 1)
    #define SET_DATA_2D_BIT         (1 << 2)
    #define SET_DATA_3D_BIT         (1 << 3)
    #define SET_FILTERS_BIT         (1 << 4)
    #define SET_ANISOTROPY_BIT      (1 << 5)
    #define SET_WRAP_MODE_1D_BIT    (1 << 6)
    #define SET_WRAP_MODE_2D_BIT    (1 << 7)
    #define SET_WRAP_MODE_3D_BIT    (1 << 8)
    #define SET_MIPMAP_LEVELS_BIT   (1 << 9)
    #define GENERATE_MIPMAPS_BIT    (1 << 10)


    RCSetTextureState::RCSetTextureState()
        : operationBitfield(0),
          textureObject(nullptr), target(),
          mipmapDatas(), filters(), anisotropy(), wrapMode()
    {
    }


    void RCSetTextureState::SetTexture1DData(GLuint* textureObjectIn, GLenum targetIn, int mipmap, GTImage::ImageFormat format, unsigned int width, const void* data, size_t dataSizeInBytes)
    {
        assert(this->textureObject == nullptr || (this->textureObject != textureObjectIn && this->target != targetIn));
        {
            TextureData mipmapData;
            mipmapData.internalFormat = ToOpenGLInternalFormat(format);
            mipmapData.format         = ToOpenGLFormat(format);
            mipmapData.type           = ToOpenGLType(format);
            mipmapData.width          = width;
            mipmapData.height         = 0;
            mipmapData.depth          = 0;

            mipmapData.data = static_cast<GLvoid*>(malloc(dataSizeInBytes));
            memcpy(mipmapData.data, data, dataSizeInBytes);


            // The old mipmap data will need to be deleted, if any.
            auto iMipmapData = this->mipmapDatas.Find(static_cast<GLint>(mipmap));
            if (iMipmapData != nullptr)
            {
                free(iMipmapData->value.data);
                iMipmapData->value = mipmapData;
            }
            else
            {
                this->mipmapDatas.Add(static_cast<GLint>(mipmap), mipmapData);
            }


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_DATA_1D_BIT;
        }
    }

    void RCSetTextureState::SetTexture2DData(GLuint* textureObjectIn, GLenum targetIn, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, const void* data, size_t dataSizeInBytes)
    {
        assert(this->textureObject == nullptr || (this->textureObject != textureObjectIn && this->target != targetIn));
        {
            TextureData mipmapData;
            mipmapData.internalFormat = ToOpenGLInternalFormat(format);
            mipmapData.format         = ToOpenGLFormat(format);
            mipmapData.type           = ToOpenGLType(format);
            mipmapData.width          = width;
            mipmapData.height         = height;
            mipmapData.depth          = 0;

            mipmapData.data = static_cast<GLvoid*>(malloc(dataSizeInBytes));
            memcpy(mipmapData.data, data, dataSizeInBytes);


            // The old mipmap data will need to be deleted, if any.
            auto iMipmapData = this->mipmapDatas.Find(static_cast<GLint>(mipmap));
            if (iMipmapData != nullptr)
            {
                free(iMipmapData->value.data);
                iMipmapData->value = mipmapData;
            }
            else
            {
                this->mipmapDatas.Add(static_cast<GLint>(mipmap), mipmapData);
            }


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_DATA_1D_BIT;
        }
    }

    void RCSetTextureState::SetTexture3DData(GLuint* textureObjectIn, GLenum targetIn, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* data, size_t dataSizeInBytes)
    {
        assert(this->textureObject == nullptr || (this->textureObject != textureObjectIn && this->target != targetIn));
        {
            TextureData mipmapData;
            mipmapData.internalFormat = ToOpenGLInternalFormat(format);
            mipmapData.format         = ToOpenGLFormat(format);
            mipmapData.type           = ToOpenGLType(format);
            mipmapData.width          = width;
            mipmapData.height         = height;
            mipmapData.depth          = depth;

            mipmapData.data = static_cast<GLvoid*>(malloc(dataSizeInBytes));
            memcpy(mipmapData.data, data, dataSizeInBytes);


            // The old mipmap data will need to be deleted, if any.
            auto iMipmapData = this->mipmapDatas.Find(static_cast<GLint>(mipmap));
            if (iMipmapData != nullptr)
            {
                free(iMipmapData->value.data);
                iMipmapData->value = mipmapData;
            }
            else
            {
                this->mipmapDatas.Add(static_cast<GLint>(mipmap), mipmapData);
            }


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_DATA_1D_BIT;
        }
    }


    void RCSetTextureState::SetTexture2DFilter(GLuint* textureObjectIn, GLenum targetIn, GLint minification, GLint magnification)
    {
        assert(this->textureObject == nullptr || (this->textureObject != textureObjectIn && this->target != targetIn));
        {
            this->filters.minification  = minification;
            this->filters.magnification = magnification;


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_FILTERS_BIT;
        }
    }

    void RCSetTextureState::SetTexture2DAnisotropy(GLuint* textureObjectIn, GLenum targetIn, GLint anisotropyIn)
    {
        assert(this->textureObject == nullptr || (this->textureObject != textureObjectIn && this->target != targetIn));
        {
            this->anisotropy = anisotropyIn;


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_ANISOTROPY_BIT;
        }
    }

    void RCSetTextureState::SetTexture2DWrapMode(GLuint* textureObjectIn, GLenum targetIn, GLint wrapModeIn)
    {
        assert(this->textureObject == nullptr || (this->textureObject != textureObjectIn && this->target != targetIn));
        {
            this->wrapMode = wrapModeIn;


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_WRAP_MODE_2D_BIT;
        }
    }

    void RCSetTextureState::SetTexture2DMipmapLevels(GLuint* textureObjectIn, GLenum targetIn, GLint baseLevel, GLint maxLevel)
    {
        assert(this->textureObject == nullptr || (this->textureObject != textureObjectIn && this->target != targetIn));
        {
            this->mipmapLevels.baseLevel = baseLevel;
            this->mipmapLevels.maxLevel  = maxLevel;


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_MIPMAP_LEVELS_BIT;
        }
    }

    void RCSetTextureState::GenerateTexture2DMipmaps(GLuint* textureObjectIn, GLenum targetIn)
    {
        assert(this->textureObject == nullptr || (this->textureObject != textureObjectIn && this->target != targetIn));
        {
            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= GENERATE_MIPMAPS_BIT;
        }
    }
    

    void RCSetTextureState::Execute()
    {
        assert(this->textureObject != nullptr);
        {
            glBindTexture(*this->textureObject, this->target);

            if ((this->operationBitfield & SET_DATA_1D_BIT))
            {
                for (size_t i = 0; i < this->mipmapDatas.count; ++i)
                {
                    auto  mipmapLevel = this->mipmapDatas.buffer[i]->key;
                    auto &mipmapData  = this->mipmapDatas.buffer[i]->value;

                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    {
                        glTexImage1D(this->target, mipmapLevel, mipmapData.internalFormat, mipmapData.width, 0, mipmapData.format, mipmapData.type, mipmapData.data);
                    }
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

                    
                    free(mipmapData.data);
                }
            }

            if ((this->operationBitfield & SET_DATA_2D_BIT))
            {
                for (size_t i = 0; i < this->mipmapDatas.count; ++i)
                {
                    auto  mipmapLevel = this->mipmapDatas.buffer[i]->key;
                    auto &mipmapData  = this->mipmapDatas.buffer[i]->value;

                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    {
                        glTexImage2D(this->target, mipmapLevel, mipmapData.internalFormat, mipmapData.width, mipmapData.height, 0, mipmapData.format, mipmapData.type, mipmapData.data);
                    }
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

                    
                    free(mipmapData.data);
                }
            }

            if ((this->operationBitfield & SET_DATA_3D_BIT))
            {
                for (size_t i = 0; i < this->mipmapDatas.count; ++i)
                {
                    auto  mipmapLevel = this->mipmapDatas.buffer[i]->key;
                    auto &mipmapData  = this->mipmapDatas.buffer[i]->value;

                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    {
                        glTexImage3D(this->target, mipmapLevel, mipmapData.internalFormat, mipmapData.width, mipmapData.height, mipmapData.depth, 0, mipmapData.format, mipmapData.type, mipmapData.data);
                    }
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

                    
                    free(mipmapData.data);
                }
            }


            if ((this->operationBitfield & SET_FILTERS_BIT))
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filters.minification);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filters.magnification);
            }

            if ((this->operationBitfield & SET_ANISOTROPY_BIT))
            {
                if (GTGL_EXT_texture_filter_anisotropic)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);
                }
            }

            if ((this->operationBitfield & SET_WRAP_MODE_1D_BIT))
            {
                glTexParameteri(this->target, GL_TEXTURE_WRAP_S, this->wrapMode);
            }
            if ((this->operationBitfield & SET_WRAP_MODE_2D_BIT))
            {
                glTexParameteri(this->target, GL_TEXTURE_WRAP_S, this->wrapMode);
                glTexParameteri(this->target, GL_TEXTURE_WRAP_T, this->wrapMode);
            }
            if ((this->operationBitfield & SET_WRAP_MODE_2D_BIT))
            {
                glTexParameteri(this->target, GL_TEXTURE_WRAP_S, this->wrapMode);
                glTexParameteri(this->target, GL_TEXTURE_WRAP_T, this->wrapMode);
            }

            if ((this->operationBitfield & GENERATE_MIPMAPS_BIT))
            {
                glGenerateMipmap(this->target);
            }
        }
    }
}
