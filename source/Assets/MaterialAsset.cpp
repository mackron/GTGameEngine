// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTEngine/Assets/MaterialAsset.hpp>
#include <easy_draw/easy_mtl.h>
#include <easy_util/easy_util.h>

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
        easymtl_material material;
        if (easymtl_initfromexisting_nocopy(&material, this->GetData(), this->GetDataSizeInBytes()))
        {
            return easymtl_getinputcount(&material);
        }

        return 0;
    }

    bool MaterialAsset::GetInputByIndex(unsigned int index, MaterialVariable &variableOut)
    {
        easymtl_material material;
        if (easymtl_initfromexisting_nocopy(&material, this->GetData(), this->GetDataSizeInBytes()))
        {
            easymtl_input* pInput = easymtl_getinputbyindex(&material, index);
            if (pInput != nullptr)
            {
                easymtl_identifier* pIdentifier = easymtl_getidentifier(&material, pInput->identifierIndex);
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
        easymtl_material material;
        if (easymtl_initfromexisting_nocopy(&material, this->GetData(), this->GetDataSizeInBytes()))
        {
            return easymtl_getpropertycount(&material);
        }

        return 0;
    }

    bool MaterialAsset::GetPropertyByIndex(unsigned int index, MaterialVariable &variableOut)
    {
        easymtl_material material;
        if (easymtl_initfromexisting_nocopy(&material, this->GetData(), this->GetDataSizeInBytes()))
        {
            easymtl_property* pProperty = easymtl_getpropertybyindex(&material, index);
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
