// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_MaterialAsset_MTL_hpp_
#define __GT_MaterialAsset_MTL_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_MTL)
#include <GTGameEngine/Assets/MaterialAsset.hpp>

namespace GT
{
    /// Material asset for OBJ MTL material files.
    class MaterialAsset_MTL : public MaterialAsset
    {
    public:

        /// Constructor.
        MaterialAsset_MTL(AssetType assetType);

        /// Destructor.
        ~MaterialAsset_MTL();


        /// @copydoc ModelAsset::Load()
        bool Load(const char* absolutePath, FileSystem &fileSystem);



    private:

        /// The ambient colour.
        float m_ambientColor[3];

        /// The diffuse colour.
        float m_diffuseColor[3];

        /// The specular colour.
        float m_specularColor[3];

        /// The specular exponent.
        float m_specularExponent;

        /// The transparency. 1 = opaque, 0 = transparent.
        float m_opacity;


        /// The file path of the ambient texture map. This is relative to the material file, and can be an empty string if it's not used.
        GTLib::String m_ambientMap;

        /// The file path of the diffuse texture map. This is relative to the material file, and can be an empty string if it's not used.
        GTLib::String m_diffuseMap;

        /// The file path of the specular texture map. This is relative to the material file, and can be an empty string if it's not used.
        GTLib::String m_specularMap;

        /// The file path of the specular exponent texture map. This is relative to the material file, and can be an empty string if it's not used.
        GTLib::String m_specularExponentMap;
    };
}


#endif

#endif
