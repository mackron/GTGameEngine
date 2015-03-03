// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_RenderingTypes_OpenGL4_hpp_
#define __GT_RenderingTypes_OpenGL4_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL4)
#include <GTGameEngine/Rendering/RenderingTypes.hpp>
#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>
#include <GTLib/ReferenceCountedObject.hpp>

namespace GT
{
    /// OpenGL 2.1 rasterizer state.
    struct RasterizerState_OpenGL4 : public ReferenceCountedObject, public GPURasterizerStateDesc
    {
        RasterizerState_OpenGL4(const GPURasterizerStateDesc &desc)
            : ReferenceCountedObject(), GPURasterizerStateDesc(desc)
        {
        }
    };

    /// OpenGL 2.1 depth/stencil state.
    struct DepthStencilState_OpenGL4 : public ReferenceCountedObject, public GPUDepthStencilStateDesc
    {
        DepthStencilState_OpenGL4(const GPUDepthStencilStateDesc &desc)
            : ReferenceCountedObject(), GPUDepthStencilStateDesc(desc)
        {
        }
    };



    /// Base class representing an OpenGL object.
    class OpenGLObject
    {
    public:

        /// Constructor.
        OpenGLObject(GLuint objectGL)
            : m_objectGL(objectGL)
        {
        }

        /// Retrieves the OpenGL object.
        GLuint GetOpenGLObject() const { return m_objectGL; }


    private:

        /// The OpenGL object.
        GLuint m_objectGL;
    };




    //////////////////////////////////////////////////
    // Shader_OpenGL4

    /// OpenGL 4 shader program.
    class Shader_OpenGL4 : public ReferenceCountedObject, public OpenGLObject
    {
    public:

        /// Constructor.
        Shader_OpenGL4(GLuint objectGL, GLenum typeGL)
            : ReferenceCountedObject(), OpenGLObject(objectGL),
              m_typeGL(typeGL)
        {
        }


        /// Retrieves the shader type.
        GLenum GetOpenGLShaderType() const { return m_typeGL; }


    private:

        /// The shader type as an OpenGL type.
        GLenum m_typeGL;
    };



    //////////////////////////////////////////////////
    // Buffer_OpenGL4

    /// Class representing an OpenGL buffer object.
    class Buffer_OpenGL4 : public ReferenceCountedObject, public OpenGLObject
    {
    public:

        /// Constructor.
        Buffer_OpenGL4(GLuint objectGL, GLbitfield flagsGL, GLsizeiptr sizeInBytes)
            : ReferenceCountedObject(), OpenGLObject(objectGL), m_flagsGL(flagsGL), m_sizeInBytes(sizeInBytes)
        {
        }


        /// Retrieves the flags that were used to create the buffer.
        GLenum GetOpenGLFlags() const { return m_flagsGL; }

        /// Retrieves the size of the buffer in bytes.
        GLsizeiptr GetSizeInBytes() const { return m_sizeInBytes; }



    private:

        /// The flags that were used to create the buffer.
        GLenum m_flagsGL;

        /// The size of the buffer in bytes. This is needed in order to know how many bytes to specify when mapping the entire buffer.
        GLsizeiptr m_sizeInBytes;
    };




    //////////////////////////////////////////////////
    // InputLayout_OpenGL4

    /// OpenGL input layout object.
    class InputLayout_OpenGL4 : public ReferenceCountedObject
    {
    public:

        /// Structure describing an attribute.
        struct AttributeDesc
        {
            unsigned int           slotIndex;
            GLint                  attribLocation;
            GLint                  attribComponentCount;
            GLenum                 attribComponentType;
            GLsizei                attribOffset;
            GPUInputClassification attributeClass;            // Per-Vertex or Per-Instance
            unsigned int           instanceStepRate;
        };



    public:

        /// Constructor.
        InputLayout_OpenGL4(AttributeDesc* const attributes, size_t attributeCount, unsigned int slotAttributeCounts[GT_GE_MAX_VERTEX_BUFFER_SLOTS]);

        /// Destructor.
        ~InputLayout_OpenGL4();


        /// Retrieves the number of vertex attribute count.
        ///
        /// @return The number of attributes making up this vertex input layout.
        size_t GetAttributeCount() const;

        /// Retrieves a reference to the attribute descriptor for the attribute at the given index.
        ///
        /// @param attributeIndex [in] The index of the attribute descriptor to retrieve.
        ///
        /// @return A reference to the attribute descriptor at the given index.
        const AttributeDesc & GetAttribute(size_t attributeIndex) const;

        
        /// Retrieves the range of indices that make up attributes that are bound to the buffer on the given slot.
        ///
        /// @param slotIndex     [in]  The index of the slot whose attribute range is being retrieved.
        /// @param rangeStartOut [out] The index of the first element in the range.
        /// @param rangeEndOut   [out] The index of the last attribute in the range, plus 1.
        ///
        /// @remarks
        ///     If rangeStart and rangeEnd are the same value, it means there are no attributes bound to the given slot.
        void GetSlotAttributeRange(unsigned int slotIndex, size_t &rangeStartOut, size_t &rangeEndOut) const;


    private:

        /// The list of attribute descriptors. This is immutable.
        AttributeDesc* const m_attributes;

        /// The number of attributes containing within this layout object. This is immutable.
        size_t m_attributeCount;

        /// m_attributes will contain a list of vertex attributes, sorted by slot. Each item in this list corresponds to a slot, and contains the index of the
        /// first attribute for the respective slot, and the index + 1 of the last attribute for the slot.
        uint32_t m_slotAttributeRanges[GT_GE_MAX_VERTEX_BUFFER_SLOTS];


    private:    // No copying.
        InputLayout_OpenGL4(const InputLayout_OpenGL4 &);
        InputLayout_OpenGL4 & operator=(const InputLayout_OpenGL4 &);
    };
}

#endif

#endif
