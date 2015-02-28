// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_Gen2_hpp_
#define __GT_GE_GPURenderingDevice_Gen2_hpp_

#include "GPURenderingDevice.hpp"

namespace GT
{
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
        ///
        /// @return A handle to the new vertex shader object, or 0 if an error occurs.
        virtual HVertexShader CreateVertexShader(const void* shaderData, size_t shaderDataSize) = 0;

        /// Decrements the reference counter of the given shader and deletes the internal object if it reaches 0.
        ///
        /// @param shader [in] A handle to the shader object to release.
        ///
        /// @remarks
        ///     This is thread-safe.
        ///     @par
        ///     It is possible that the internal API-specific data structure may not be deleted until the next flush or buffer swap in the interest of
        ///     synchronization.
        virtual void ReleaseVertexShader(HVertexShader hShader) = 0;

        /// Increments the reference counter of the given shader.
        ///
        /// @param hShader [in] A handle to the shader whose reference counter is being incremented.
        ///
        /// @remarks
        ///     This is thread-safe.
        virtual void HoldVertexShader(HVertexShader hShader) = 0;


        /// Creates a vertex shader.
        ///
        /// @param shaderData     [in] A pointer to the buffer containing the vertex shader data.
        /// @param shaderDataSize [in] The size in bytes of the vertex shader data.
        ///
        /// @return A handle to the new vertex shader object, or 0 if an error occurs.
        virtual HFragmentShader CreateFragmentShader(const void* shaderData, size_t shaderDataSize) = 0;

        /// Decrements the reference counter of the given shader and deletes the internal object if it reaches 0.
        ///
        /// @param shader [in] A handle to the shader object to release.
        ///
        /// @remarks
        ///     This is thread-safe.
        ///     @par
        ///     It is possible that the internal API-specific data structure may not be deleted until the next flush or buffer swap in the interest of
        ///     synchronization.
        virtual void ReleaseFragmentShader(HFragmentShader hShader) = 0;

        /// Increments the reference counter of the given shader.
        ///
        /// @param hShader [in] A handle to the shader whose reference counter is being incremented.
        ///
        /// @remarks
        ///     This is thread-safe.
        virtual void HoldFragmentShader(HFragmentShader hShader) = 0;



    private:    // No copying.
        GPURenderingDevice_Gen2(const GPURenderingDevice_Gen2 &);
        GPURenderingDevice_Gen2 & operator=(const GPURenderingDevice_Gen2 &);
    };
}

#endif