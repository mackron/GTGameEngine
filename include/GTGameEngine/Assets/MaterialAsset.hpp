// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_MaterialAsset
#define GT_MaterialAsset

#include "Asset.hpp"
#include "../Graphics/GraphicsWorld.hpp"    // For material variable types.

namespace GT
{
    struct MaterialVariable
    {
        /// The name of the variable. Cannot be more than 27 characters + null terminator.
        char name[28];

        /// The variable type.
        GraphicsMaterialVariableType type;

        /// The value.
        union
        {
            struct
            {
                float x;
            } f1;
            struct
            {
                float x;
                float y;
            } f2;
            struct
            {
                float x;
                float y;
                float z;
            } f3;
            struct
            {
                float x;
                float y;
                float z;
                float w;
            } f4;

            struct
            {
                int x;
            } i1;
            struct
            {
                int x;
                int y;
            } i2;
            struct
            {
                int x;
                int y;
                int z;
            } i3;
            struct
            {
                int x;
                int y;
                int z;
                int w;
            } i4;

            struct
            {
                char value[224];
            } path;
        };
    };


    /// Base class for material assets.
    class MaterialAsset : public Asset
    {
    public:

        /// Constructor.
        MaterialAsset(const char* absolutePathOrIdentifier, AssetType assetType);

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


        /// Retrieves the number of input variables.
        virtual unsigned int GetInputCount() const;

        /// Retrieves an input variable by it's count.
        virtual bool GetInputByIndex(unsigned int index, MaterialVariable &variableOut);


        /// Retrieves the number of properties.
        virtual unsigned int GetPropertyCount() const;

        /// Retrieves a property by it's count.
        virtual bool GetPropertyByIndex(unsigned int index, MaterialVariable &variableOut);
    };
}

#endif
