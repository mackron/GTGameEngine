// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetTextureState.hpp"
#include "ServerState_OpenGL33.hpp"
#include "../TypeConversion.hpp"
#include <gtgl/gtgl.h>
#include <assert.h>

namespace GTEngine
{
    #define SET_DATA_1D_BIT         (1 << 1)
    #define SET_DATA_2D_BIT         (1 << 2)
    #define SET_DATA_3D_BIT         (1 << 3)
    #define SET_DATA_CUBE_BIT       (1 << 4)
    #define SET_FILTERS_BIT         (1 << 5)
    #define SET_ANISOTROPY_BIT      (1 << 6)
    #define SET_WRAP_MODE_1D_BIT    (1 << 7)
    #define SET_WRAP_MODE_2D_BIT    (1 << 8)
    #define SET_WRAP_MODE_3D_BIT    (1 << 9)
    #define SET_MIPMAP_LEVELS_BIT   (1 << 10)
    #define GENERATE_MIPMAPS_BIT    (1 << 11)


    RCSetTextureState::RCSetTextureState()
        : operationBitfield(0),
          textureState(nullptr), target(),
          mipmapDatas(), cubeMapData(), filters(), anisotropy(), wrapMode(),
          mipmapLevels()
    {
        // These are important initialisations!
        this->cubeMapData.positiveXData = nullptr;
        this->cubeMapData.negativeXData = nullptr;
        this->cubeMapData.positiveYData = nullptr;
        this->cubeMapData.negativeYData = nullptr;
        this->cubeMapData.positiveZData = nullptr;
        this->cubeMapData.negativeZData = nullptr;
    }


    void RCSetTextureState::SetTexture1DData(TextureState_OpenGL33* textureStateIn, GLenum targetIn, int mipmap, GTImage::ImageFormat format, unsigned int width, const void* data, size_t dataSizeInBytes)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == targetIn)) && textureStateIn != nullptr);
        {
            TextureData mipmapData;
            mipmapData.internalFormat = ToOpenGLInternalFormat(format);
            mipmapData.format         = ToOpenGLFormat(format);
            mipmapData.type           = ToOpenGLType(format);
            mipmapData.width          = width;
            mipmapData.height         = 0;
            mipmapData.depth          = 0;

            if (data != nullptr && dataSizeInBytes > 0)
            {
                mipmapData.data = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(mipmapData.data, data, dataSizeInBytes);
            }
            else
            {
                mipmapData.data = nullptr;
            }


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


            this->textureState = textureStateIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_DATA_1D_BIT;
        }
    }

    void RCSetTextureState::SetTexture2DData(TextureState_OpenGL33* textureStateIn, GLenum targetIn, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, const void* data, size_t dataSizeInBytes)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == targetIn)) && textureStateIn != nullptr);
        {
            TextureData mipmapData;
            mipmapData.internalFormat = ToOpenGLInternalFormat(format);
            mipmapData.format         = ToOpenGLFormat(format);
            mipmapData.type           = ToOpenGLType(format);
            mipmapData.width          = width;
            mipmapData.height         = height;
            mipmapData.depth          = 0;

            if (data != nullptr && dataSizeInBytes > 0)
            {
                mipmapData.data = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(mipmapData.data, data, dataSizeInBytes);
            }
            else
            {
                mipmapData.data = nullptr;
            }


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


            this->textureState = textureStateIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_DATA_2D_BIT;
        }
    }

    void RCSetTextureState::SetTexture3DData(TextureState_OpenGL33* textureStateIn, GLenum targetIn, int mipmap, GTImage::ImageFormat format, unsigned int width, unsigned int height, unsigned int depth, const void* data, size_t dataSizeInBytes)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == targetIn)) && textureStateIn != nullptr);
        {
            TextureData mipmapData;
            mipmapData.internalFormat = ToOpenGLInternalFormat(format);
            mipmapData.format         = ToOpenGLFormat(format);
            mipmapData.type           = ToOpenGLType(format);
            mipmapData.width          = width;
            mipmapData.height         = height;
            mipmapData.depth          = depth;

            if (data != nullptr && dataSizeInBytes > 0)
            {
                mipmapData.data = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(mipmapData.data, data, dataSizeInBytes);
            }
            else
            {
                mipmapData.data = nullptr;
            }


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


            this->textureState = textureStateIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_DATA_3D_BIT;
        }
    }

    void RCSetTextureState::SetTextureCubeData(TextureState_OpenGL33* textureStateIn, GTImage::ImageFormat format, unsigned int width, unsigned int height, size_t dataSizeInBytes,
        const void* positiveXData, const void* negativeXData,
        const void* positiveYData, const void* negativeYData,
        const void* positiveZData, const void* negativeZData)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == GL_TEXTURE_CUBE_MAP)) && textureStateIn != nullptr);
        {
            // Previous data needs to be freed.
            free(this->cubeMapData.positiveXData);
            free(this->cubeMapData.negativeXData);
            free(this->cubeMapData.positiveYData);
            free(this->cubeMapData.negativeYData);
            free(this->cubeMapData.positiveZData);
            free(this->cubeMapData.negativeZData);


            this->cubeMapData.internalFormat = ToOpenGLInternalFormat(format);
            this->cubeMapData.format         = ToOpenGLFormat(format);
            this->cubeMapData.type           = ToOpenGLType(format);
            this->cubeMapData.width          = width;
            this->cubeMapData.height         = height;

            // X Data
            if (positiveXData != nullptr)
            {
                this->cubeMapData.positiveXData = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(this->cubeMapData.positiveXData, positiveXData, dataSizeInBytes);
            }
            else
            {
                this->cubeMapData.positiveXData = nullptr;
            }

            if (negativeXData != nullptr)
            {
                this->cubeMapData.positiveXData = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(this->cubeMapData.negativeXData, negativeXData, dataSizeInBytes);
            }
            else
            {
                this->cubeMapData.negativeXData = nullptr;
            }


            // Y Data
            if (positiveYData != nullptr)
            {
                this->cubeMapData.positiveYData = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(this->cubeMapData.positiveYData, positiveYData, dataSizeInBytes);
            }
            else
            {
                this->cubeMapData.positiveYData = nullptr;
            }

            if (negativeYData != nullptr)
            {
                this->cubeMapData.negativeYData = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(this->cubeMapData.negativeYData, negativeYData, dataSizeInBytes);
            }
            else
            {
                this->cubeMapData.negativeYData = nullptr;
            }


            // Z Data
            if (positiveZData != nullptr)
            {
                this->cubeMapData.positiveZData = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(this->cubeMapData.positiveZData, positiveZData, dataSizeInBytes);
            }
            else
            {
                this->cubeMapData.positiveZData = nullptr;
            }

            if (negativeZData != nullptr)
            {
                this->cubeMapData.negativeZData = static_cast<GLvoid*>(malloc(dataSizeInBytes));
                memcpy(this->cubeMapData.negativeZData, negativeZData, dataSizeInBytes);
            }
            else
            {
                this->cubeMapData.negativeZData = nullptr;
            }



            this->textureState = textureStateIn;
            this->target        = GL_TEXTURE_CUBE_MAP;

            this->operationBitfield |= SET_DATA_CUBE_BIT;
        }
    }


    void RCSetTextureState::SetTextureFilter(TextureState_OpenGL33* textureStateIn, GLenum targetIn, GLint minification, GLint magnification)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == targetIn)) && textureStateIn != nullptr);
        {
            this->filters.minification  = minification;
            this->filters.magnification = magnification;


            this->textureState = textureStateIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_FILTERS_BIT;
        }
    }

    void RCSetTextureState::SetTextureAnisotropy(TextureState_OpenGL33* textureStateIn, GLenum targetIn, GLint anisotropyIn)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == targetIn)) && textureStateIn != nullptr);
        {
            this->anisotropy = anisotropyIn;


            this->textureState = textureStateIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_ANISOTROPY_BIT;
        }
    }

    void RCSetTextureState::SetTextureWrapMode(TextureState_OpenGL33* textureStateIn, GLenum targetIn, GLint wrapModeIn)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == targetIn)) && textureStateIn != nullptr);
        {
            this->wrapMode = wrapModeIn;


            this->textureState = textureStateIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_WRAP_MODE_2D_BIT;
        }
    }

    void RCSetTextureState::SetTextureMipmapLevels(TextureState_OpenGL33* textureStateIn, GLenum targetIn, GLint baseLevel, GLint maxLevel)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == targetIn)) && textureStateIn != nullptr);
        {
            this->mipmapLevels.baseLevel = baseLevel;
            this->mipmapLevels.maxLevel  = maxLevel;


            this->textureState = textureStateIn;
            this->target        = targetIn;

            this->operationBitfield |= SET_MIPMAP_LEVELS_BIT;
        }
    }

    void RCSetTextureState::GenerateTextureMipmaps(TextureState_OpenGL33* textureStateIn, GLenum targetIn)
    {
        assert((this->textureState == nullptr || (this->textureState == textureStateIn && this->target == targetIn)) && textureStateIn != nullptr);
        {
            this->textureState = textureStateIn;
            this->target        = targetIn;

            this->operationBitfield |= GENERATE_MIPMAPS_BIT;
        }
    }


    TextureState_OpenGL33* RCSetTextureState::GetTextureState()
    {
        return this->textureState;
    }

    GLenum RCSetTextureState::GetTarget()
    {
        return this->target;
    }

    

    void RCSetTextureState::Execute()
    {
        assert(this->textureState != nullptr);
        {
            // We may need to restore the texture binding at the end of this, so we'll grab the current binding here.
            GLuint previousTextureObject = 0;

            if (this->target == GL_TEXTURE_1D)
            {
                previousTextureObject = ServerState_GL_TEXTURE_BINDING_1D;
            }
            else if (this->target == GL_TEXTURE_2D)
            {
                previousTextureObject = ServerState_GL_TEXTURE_BINDING_2D;
            }
            else if (this->target == GL_TEXTURE_3D)
            {
                previousTextureObject = ServerState_GL_TEXTURE_BINDING_3D;
            }
            else if (this->target == GL_TEXTURE_CUBE_MAP || (this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z))
            {
                previousTextureObject = ServerState_GL_TEXTURE_BINDING_CUBE;
            }



            // If the target is cube map face, the binding target needs to be GL_TEXTURE_CUBE_MAP.
            if (this->target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && this->target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureState->objectGL);
            }
            else
            {
                glBindTexture(this->target, this->textureState->objectGL);
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

            
            if ((this->operationBitfield & SET_DATA_CUBE_BIT))
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, this->cubeMapData.internalFormat, this->cubeMapData.width, this->cubeMapData.height, 0, this->cubeMapData.format, this->cubeMapData.type, this->cubeMapData.positiveXData);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, this->cubeMapData.internalFormat, this->cubeMapData.width, this->cubeMapData.height, 0, this->cubeMapData.format, this->cubeMapData.type, this->cubeMapData.negativeXData);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, this->cubeMapData.internalFormat, this->cubeMapData.width, this->cubeMapData.height, 0, this->cubeMapData.format, this->cubeMapData.type, this->cubeMapData.positiveYData);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, this->cubeMapData.internalFormat, this->cubeMapData.width, this->cubeMapData.height, 0, this->cubeMapData.format, this->cubeMapData.type, this->cubeMapData.negativeYData);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, this->cubeMapData.internalFormat, this->cubeMapData.width, this->cubeMapData.height, 0, this->cubeMapData.format, this->cubeMapData.type, this->cubeMapData.positiveZData);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, this->cubeMapData.internalFormat, this->cubeMapData.width, this->cubeMapData.height, 0, this->cubeMapData.format, this->cubeMapData.type, this->cubeMapData.negativeZData);

                    free(this->cubeMapData.positiveXData);
                    free(this->cubeMapData.negativeXData);
                    free(this->cubeMapData.positiveYData);
                    free(this->cubeMapData.negativeYData);
                    free(this->cubeMapData.positiveZData);
                    free(this->cubeMapData.negativeZData);
                }
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
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
            if ((this->operationBitfield & SET_WRAP_MODE_3D_BIT))
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
                glBindTexture(GL_TEXTURE_CUBE_MAP, previousTextureObject);
            }
            else
            {
                glBindTexture(this->target, previousTextureObject);
            }
        }
    }
}
