// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_Gen2_hpp_
#define __GT_GE_GPURenderingDevice_Gen2_hpp_

#include "GPURenderingDevice.hpp"

namespace GT
{
    class GPUVertexShader;
    class GPUFragmentShader;


    /// Class representing a rendering D3D9-generation GPU device.
    class GPURenderingDevice_Gen2 : public GPURenderingDevice
    {
    public:

        /// Constructor.
        GPURenderingDevice_Gen2(const GPURenderingDeviceInfo &info);

        /// Destructor.
        ~GPURenderingDevice_Gen2();


        /// GPURenderingDevice::GetGeneration().
        unsigned int GetGeneration() const;



        /////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////
        //
        // Generation 2 Specific
        //
        /////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////


        /////////////////////////////////////////////////////////////////////////////
        //
        // Object Creation and Deletion
        //
        /////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////
        // Shaders

        /// Creates a vertex shader.
        ///
        /// @param shaderData     [in] A pointer to the buffer containing the vertex shader data.
        /// @param shaderDataSize [in] The size in bytes of the vertex shader data.
        /// @param shaderOut      [out] A reference to the variable that will receive a pointer to the new shader object.
        ///
        /// @return A result code describing whether or not the shader was created successfully.
        virtual ResultCode CreateVertexShader(const void* shaderData, size_t shaderDataSize, GPUVertexShader* &shaderOut) = 0;

        /// Deletes a vertex shader.
        ///
        /// @param shader [in] A pointer to the shader object to delete.
        virtual void DeleteVertexShader(GPUVertexShader* shader) = 0;


        /// Creates a fragment shader.
        ///
        /// @param shaderData     [in] A pointer to the buffer containing the fragment shader data.
        /// @param shaderDataSize [in] The size in bytes of the fragment shader data.
        /// @param shaderOut      [out] A reference to the variable that will receive a pointer to the new shader object.
        ///
        /// @return A result code describing whether or not the shader was created successfully.
        virtual ResultCode CreateFragmentShader(const void* shaderData, size_t shaderDataSize, GPUFragmentShader* &shaderOut) = 0;

        /// Deletes a fragment shader.
        ///
        /// @param shader [in] A pointer to the shader object to delete.
        virtual void DeleteFragmentShader(GPUFragmentShader* shader) = 0;



    private:    // No copying.
        GPURenderingDevice_Gen2(const GPURenderingDevice_Gen2 &);
        GPURenderingDevice_Gen2 & operator=(const GPURenderingDevice_Gen2 &);
    };
}

#endif