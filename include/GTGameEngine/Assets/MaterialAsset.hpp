// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_MaterialAsset_hpp_
#define __GT_MaterialAsset_hpp_

#include "Asset.hpp"

namespace GT
{
    /// Class representing a channel in a material.
    ///
    /// A channel is basically just a function that defines a particular property of a material. For example, a texture might be implemented using
    /// the a channel called "diffuse".
    class MaterialChannel
    {
    public:

        /// Constructor.
        MaterialChannel(const char* name, const char* shaderCode);

        
        /// Sets the name of the channel.
        ///
        /// @param name [in] The name of the channel. This cannot be mnore than 31 characters in length, and must be null terminated.
        void SetName(const char* name);

        /// Retrieves the name of the channel.
        ///
        /// @return The name of the channel.
        const char* GetName() const;


        /// Sets the shader code of the channel.
        ///
        /// @param shaderCode [in] The shader code.
        void SetShaderCode(const char* shaderCode);

        /// Retrieves the shader code of the channel.
        ///
        /// @return The shader code.
        const char* GetShaderCode() const;


    private:

        /// The name of the channel.
        char m_name[32];

        /// The shader string containing the function that implements the channel. This is actually temporary, and will be changed to raw SPIR-V code.
        GTLib::String m_shaderCode;
    };



    /// Base class for material assets.
    class MaterialAsset : public Asset
    {
    public:

        /// Constructor.
        MaterialAsset(AssetType assetType);

        /// Destructor.
        virtual ~MaterialAsset();


        /// @copydoc Asset::GetClass()
        AssetClass GetClass() const;



        /// Adds a channel to the material.
        ///
        /// @param name       [in] The name of the channel. This cannot be more than 31 characters in length.
        /// @param shaderCode [in] The shader code.
        ///
        /// @remarks
        ///     If a channel of the same name already exists it will be removed.
        void AddChannel(const char* name, const char* shaderCode);

        /// Removes a channel by the channel name.
        ///
        /// @param name [in] The name of the channel to remove.
        void RemoveChannel(const char* name);

        /// Sets the shader code for the channel with the given name.
        ///
        /// @param name       [in] The name of the channel whose shader code is being set.
        /// @param shaderCode [in] The shader code.
        void SetChannelShaderCode(const char* name, const char* shaderCode);

        /// Retrieves the shader code for the channel with the given name.
        ///
        /// @param name [in] The name of the channel whose shader code is being retrieved.
        const char* GetChannelShaderCode(const char* name) const;



    private:

        /// The list of channels making up the material.
        GTLib::Vector<MaterialChannel*> m_channels;
    };
}

#endif
