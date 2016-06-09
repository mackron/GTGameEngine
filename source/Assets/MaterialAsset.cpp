// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGE/Assets/MaterialAsset.hpp>
#include <dr_libs/dr_mtl.h>
#include <dr_libs/dr.h>

namespace GT
{
    MaterialAsset::MaterialAsset(const char* absolutePathOrIdentifier, AssetType assetType)
        : Asset(absolutePathOrIdentifier, assetType)
    {
    }

    MaterialAsset::~MaterialAsset()
    {
    }


    AssetClass MaterialAsset::GetClass() const
    {
        return AssetClass_Material;
    }


    unsigned int MaterialAsset::GetInputCount() const
    {
        drmtl_material material;
        if (drmtl_initfromexisting_nocopy(&material, this->GetData(), this->GetDataSizeInBytes()))
        {
            return drmtl_getinputcount(&material);
        }

        return 0;
    }

    bool MaterialAsset::GetInputByIndex(unsigned int index, MaterialVariable &variableOut)
    {
        drmtl_material material;
        if (drmtl_initfromexisting_nocopy(&material, this->GetData(), this->GetDataSizeInBytes()))
        {
            drmtl_input* pInput = drmtl_getinputbyindex(&material, index);
            if (pInput != nullptr)
            {
                drmtl_identifier* pIdentifier = drmtl_getidentifier(&material, pInput->identifierIndex);
                if (pIdentifier != nullptr)
                {
                    strcpy_s(variableOut.name, 28, pIdentifier->name);
                    variableOut.type = GraphicsMaterialVariableType(pIdentifier->type);
                    memcpy(variableOut.path.value, pInput->path.value, 224);

                    return true;
                }
            }
        }

        return false;
    }


    unsigned int MaterialAsset::GetPropertyCount() const
    {
        drmtl_material material;
        if (drmtl_initfromexisting_nocopy(&material, this->GetData(), this->GetDataSizeInBytes()))
        {
            return drmtl_getpropertycount(&material);
        }

        return 0;
    }

    bool MaterialAsset::GetPropertyByIndex(unsigned int index, MaterialVariable &variableOut)
    {
        drmtl_material material;
        if (drmtl_initfromexisting_nocopy(&material, this->GetData(), this->GetDataSizeInBytes()))
        {
            drmtl_property* pProperty = drmtl_getpropertybyindex(&material, index);
            if (pProperty != nullptr)
            {
                strcpy_s(variableOut.name, 28, pProperty->name);
                variableOut.type = GraphicsMaterialVariableType(pProperty->type);
                memcpy(variableOut.path.value, pProperty->path.value, 224);

                return true;
            }
        }

        return false;
    }
}
