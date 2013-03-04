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
        assert(this->textureObject == nullptr || (this->textureObject == textureObjectIn && this->target == targetIn) && textureObjectIn != nullptr);
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
        assert(this->textureObject == nullptr || (this->textureObject == textureObjectIn && this->target == targetIn) && textureObjectIn != nullptr);
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

            this->operationBitfield |= SET_DATA_2D_BIT;
        }
    }

    void RCSetTextureState::SetTexture3DData(GLuint* textureObjectIn, GLenum targetIn, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* data, size_t dataSizeInBytes)
    {
        assert(this->textureObject == nullptr || (this->textureObject == textureObjectIn && this->target == targetIn) && textureObjectIn != nullptr);
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

            this->operationBitfield |= SET_DATA_3D_BIT;
        }
    }


    void RCSetTextureState::SetTextureFilter(GLuint* textureObjectIn, GLenum targetIn, GLint minification, GLint magnification)
    {
        assert(this->textureObject == nullptr || (this->textureObject == textureObjectIn && this->target == targetIn) && textureObjectIn != nullptr);
        {
            this->filters.minification  = minification;
            this->filters.magnification = magnification;


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_FILTERS_BIT;
        }
    }

    void RCSetTextureState::SetTextureAnisotropy(GLuint* textureObjectIn, GLenum targetIn, GLint anisotropyIn)
    {
        assert(this->textureObject == nullptr || (this->textureObject == textureObjectIn && this->target == targetIn) && textureObjectIn != nullptr);
        {
            this->anisotropy = anisotropyIn;


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_ANISOTROPY_BIT;
        }
    }

    void RCSetTextureState::SetTextureWrapMode(GLuint* textureObjectIn, GLenum targetIn, GLint wrapModeIn)
    {
        assert(this->textureObject == nullptr || (this->textureObject == textureObjectIn && this->target == targetIn) && textureObjectIn != nullptr);
        {
            this->wrapMode = wrapModeIn;


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_WRAP_MODE_2D_BIT;
        }
    }

    void RCSetTextureState::SetTextureMipmapLevels(GLuint* textureObjectIn, GLenum targetIn, GLint baseLevel, GLint maxLevel)
    {
        assert(this->textureObject == nullptr || (this->textureObject == textureObjectIn && this->target == targetIn) && textureObjectIn != nullptr);
        {
            this->mipmapLevels.baseLevel = baseLevel;
            this->mipmapLevels.maxLevel  = maxLevel;


            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_MIPMAP_LEVELS_BIT;
        }
    }

    void RCSetTextureState::GenerateTextureMipmaps(GLuint* textureObjectIn, GLenum targetIn)
    {
        assert(this->textureObject == nullptr || (this->textureObject == textureObjectIn && this->target == targetIn) && textureObjectIn != nullptr);
        {
            this->textureObject = textureObjectIn;
            this->target        = targetIn;

            this->operationBitfield |= GENERATE_MIPMAPS_BIT;
        }
    }


    GLuint* RCSetTextureState::GetTextureObject()
    {
        return this->textureObject;
    }

    GLenum RCSetTextureState::GetTarget()
    {
        return this->target;
    }

    

    void RCSetTextureState::Execute()
    {
        assert(this->textureObject != nullptr);
        {
            // We may need to restore the texture binding at the end of this. To keep things simple, we're going to just use glGetIntegerv().
            GLint previousTextureObject = 0;

            if (this->target == GL_TEXTURE_1D)
            {
                glGetIntegerv(GL_TEXTURE_BINDING_1D, &previousTextureObject);
            }
            else if (this->target == GL_TEXTURE_2D)
            {
                glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTextureObject);
            }
            else if (this->target == GL_TEXTURE_3D)
            {
                glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousTextureObject);
            }
            else if (this->target == GL_TEXTURE_CUBE_MAP || (this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z))
            {
                glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &previousTextureObject);
            }
            else if (this->target == GL_TEXTURE_RECTANGLE)
            {
                glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &previousTextureObject);
            }


            // If the target is cube map face, the binding target needs to be GL_TEXTURE_CUBE_MAP.
            if (this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, *this->textureObject);
            }
            else
            {
                glBindTexture(this->target, *this->textureObject);
            }


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
                assert(!(this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
                {
                    glTexParameteri(this->target, GL_TEXTURE_MIN_FILTER, this->filters.minification);
                    glTexParameteri(this->target, GL_TEXTURE_MAG_FILTER, this->filters.magnification);
                }
            }

            if ((this->operationBitfield & SET_ANISOTROPY_BIT))
            {
                assert(!(this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
                {
                    if (GTGL_EXT_texture_filter_anisotropic)
                    {
                        glTexParameteri(this->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);
                    }
                }
            }

            if ((this->operationBitfield & SET_WRAP_MODE_1D_BIT))
            {
                assert(!(this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
                {
                    glTexParameteri(this->target, GL_TEXTURE_WRAP_S, this->wrapMode);
                }
            }
            if ((this->operationBitfield & SET_WRAP_MODE_2D_BIT))
            {
                assert(!(this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
                {
                    glTexParameteri(this->target, GL_TEXTURE_WRAP_S, this->wrapMode);
                    glTexParameteri(this->target, GL_TEXTURE_WRAP_T, this->wrapMode);
                }
            }
            if ((this->operationBitfield & SET_WRAP_MODE_2D_BIT))
            {
                assert(!(this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
                {
                    glTexParameteri(this->target, GL_TEXTURE_WRAP_S, this->wrapMode);
                    glTexParameteri(this->target, GL_TEXTURE_WRAP_T, this->wrapMode);
                }
            }

            if ((this->operationBitfield & GENERATE_MIPMAPS_BIT))
            {
                assert(!(this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
                {
                    glGenerateMipmap(this->target);
                }
            }


            // We need to restore the texture binding. If the target is cube map face, the binding target needs to be GL_TEXTURE_CUBE_MAP.
            if (this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(previousTextureObject));
            }
            else
            {
                glBindTexture(this->target, static_cast<GLuint>(previousTextureObject));
            }
        }
    }
}
