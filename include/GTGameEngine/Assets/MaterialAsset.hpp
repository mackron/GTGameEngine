// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_MaterialAsset_hpp_
#define __GT_MaterialAsset_hpp_

#include "Asset.hpp"
#include "../Math.hpp"

namespace GT
{
    enum class MaterialVariableType
    {
        Float,
        Float2,
        Float3,
        Float4,

        Integer,
        Integer2,
        Integer3,
        Integer4,

        Texture1D,
        Texture2D,
        Texture3D,
        TextureCube,
    };


    /// Base class for input variables and properties for material assets.
    class MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable(MaterialVariableType type, const char* name);


        /// Retrieves the type of the variable.
        MaterialVariableType GetType() const;

        /// Retrieves the name of the variable.
        const char* GetName() const;


    private:

        /// The type of the variable (float, float2, etc.)
        MaterialVariableType m_type;

        /// The name of the variable.
        char m_name[32];
    };

    class MaterialAssetVariable_Float : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Float(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Float, name),
              m_value(0.0f)
        {
        }


        /// Retrieves the value of the variable.
        float GetValue() const;

        /// Sets the value of the variable.
        void SetValue(float value);
        

    private:

        /// The value of the variable.
        float m_value;
    };

    class MaterialAssetVariable_Float2 : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Float2(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Float2, name),
              x(0.0f), y(0.0f)
        {
        }
        
    private:

        float x;
        float y;
    };

    class MaterialAssetVariable_Float3 : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Float3(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Float3, name),
              x(0.0f), y(0.0f), z(0.0f)
        {
        }
        
    private:

        float x;
        float y;
        float z;
    };

    class MaterialAssetVariable_Float4 : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Float4(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Float4, name),
              x(0.0f), y(0.0f), z(0.0f), w(0.0f)
        {
        }
        
    private:

        float x;
        float y;
        float z;
        float w;
    };

    class MaterialAssetVariable_Integer : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Integer(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Integer, name),
              value(0)
        {
        }
        
    private:

        int value;
    };

    class MaterialAssetVariable_Integer2 : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Integer2(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Integer2, name),
              x(0), y(0)
        {
        }
        
    private:

        int x;
        int y;
    };

    class MaterialAssetVariable_Integer3 : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Integer3(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Integer3, name),
              x(0), y(0), z(0)
        {
        }
        
    private:

        int x;
        int y;
        int z;
    };

    class MaterialAssetVariable_Integer4 : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Integer4(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Integer4, name),
              x(0), y(0), z(0), w(0)
        {
        }
        
    private:

        int x;
        int y;
        int z;
        int w;
    };


    class MaterialAssetVariable_Texture1D : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Texture1D(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Texture1D, name)
        {
        }
        
    private:

    };

    class MaterialAssetVariable_Texture2D : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Texture2D(const char* name, const char* imageAssetPath)
            : MaterialAssetVariable(MaterialVariableType::Texture2D, name),
              m_imageAssetPath(imageAssetPath)
        {
        }
        
    private:

        /// The relative path of the image. Leave blank if the texture should be set dynamically at run-time.
        GTLib::String m_imageAssetPath;
    };

    class MaterialAssetVariable_Texture3D : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_Texture3D(const char* name)
            : MaterialAssetVariable(MaterialVariableType::Texture3D, name)
        {
        }
        
    private:

    };

    class MaterialAssetVariable_TextureCube : public MaterialAssetVariable
    {
    public:

        /// Constructor.
        MaterialAssetVariable_TextureCube(const char* name)
            : MaterialAssetVariable(MaterialVariableType::TextureCube, name)
        {
        }
        
    private:

    };


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


        /// Retrieves a pointer to the raw material data.
        ///
        /// @remarks
        ///     This data must be in the format defined by easy_mtl.
        virtual const void* GetData() const = 0;

        /// Retrieves the size of the raw material data, in bytes.
        virtual unsigned int GetDataSizeInBytes() const = 0;



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


        /// Sets the byte-code data for the channel with the given name.
        ///
        /// @param name        [in] The name of the channel whose byte-code is being set.
        /// @param sizeInBytes [in] The size in bytes of the byte-code data.
        /// @param pData       [in] A pointer to the buffer containing the byte-code data.
        ///
        /// @return True if the data was set successfully; false otherwise.
        bool SetChannelByteCodeData(const char* name, size_t sizeInBytes, const void* pData);

        /// Retrieves the size of the byte-code of the channel with the given name.
        ///
        /// @param name [in] The name of the channel whose byte-code size is being retrieved.
        ///
        /// @return The size of the channel's byte-code data, or 0 if an error occurs.
        size_t GetChannelByteCodeSizeInBytes(const char* name) const;

        /// Retrieves the size of the byte-code data of the channel with the given name.
        ///
        /// @param name    [in ] The name of the channel whose byte-code data is being retrieved.
        /// @param dataOut [out] A pointer to the buffer that will receive the byte-code data.
        ///
        /// @remarks
        ///     Use GetChannelByteCodeSizeInBytes() to determine the required size of the buffer.
        void GetChannelByteCodeData(const char* name, void* pDataOut) const;



    private:

        /// Input variables.
        GTLib::Vector<MaterialAssetVariable*> m_inputVariables;

        /// The list of channels making up the material.
        GTLib::Vector<MaterialChannel*> m_channels;

        /// Properties.
        GTLib::Vector<MaterialAssetVariable*> m_properties;
    };
}

#endif
