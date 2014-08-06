// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_DefaultSceneRenderer_MaterialUniformNameCache_hpp_
#define __GTEngine_DefaultSceneRenderer_MaterialUniformNameCache_hpp_

#include <GTLib/Vector.hpp>
#include <GTLib/String.hpp>

namespace GTEngine
{
    /// Enumerator representing the different material uniform names to cache.
    enum MaterialUniformNameID
    {
        // Ambient lights.
        MaterialUniform_AmbientLightFS_Colour = 0,
            
        // Directional lights.
        MaterialUniform_DirectionalLightFS_Colour,
        MaterialUniform_DirectionalLightFS_Direction,

        // Point lights.
        MaterialUniform_PointLightVS_PositionVS,
        MaterialUniform_PointLightFS_Colour,
        MaterialUniform_PointLightFS_Radius,
        MaterialUniform_PointLightFS_Falloff,

        // Spot lights.
        MaterialUniform_SpotLightFS_Position,
        MaterialUniform_SpotLightFS_Colour,
        MaterialUniform_SpotLightFS_Direction,
        MaterialUniform_SpotLightFS_Length,
        MaterialUniform_SpotLightFS_Falloff,
        MaterialUniform_SpotLightFS_CosAngleInner,
        MaterialUniform_SpotLightFS_CosAngleOuter,

        // Shadow directional lights.
        MaterialUniform_ShadowDirectionalLightVS_ProjectionView,
        MaterialUniform_ShaderDirectionalLightFS_Colour,
        MaterialUniform_ShaderDirectionalLightFS_Direction,
        MaterialUniform_ShaderDirectionalLightFS_ShadowMap,

        // Shadow point lights.
        MaterialUniform_ShadowPointLightFS_PositionVS,
        MaterialUniform_ShadowPointLightFS_PositionWS,
        MaterialUniform_ShadowPointLightFS_Colour,
        MaterialUniform_ShadowPointLightFS_Radius,
        MaterialUniform_ShadowPointLightFS_Falloff,
        MaterialUniform_ShadowPointLightFS_ShadowMap,

        // Shadow spot lights.
        MaterialUniforms_ShadowSpotLightVS_ProjectionView,
        MaterialUniforms_ShadowSpotLightFS_Position,
        MaterialUniforms_ShadowSpotLightFS_Colour,
        MaterialUniforms_ShadowSpotLightFS_Direction,
        MaterialUniforms_ShadowSpotLightFS_Length,
        MaterialUniforms_ShadowSpotLightFS_Falloff,
        MaterialUniforms_ShadowSpotLightFS_CosAngleInner,
        MaterialUniforms_ShadowSpotLightFS_CosAngleOuter,
        MaterialUniforms_ShadowSpotLightFS_ShadowMap,


        MaterialUniform_LightNamesCount,
        MaterialUniform_LightNamesStart = MaterialUniform_AmbientLightFS_Colour,
    };


    /// Class for caching string representing the names of uniform variables used by materials.
    ///
    /// We need this class because it is too slow to keep constructing the strings with printf() or similar. What this allows instead
    /// is to construct the strings once, and then quickly retrieve them later on.
    class DefaultSceneRenderer_MaterialUniformNameCache
    {
    public:


        struct LightIndexUniformNames
        {
            LightIndexUniformNames(int index)
                : names(MaterialUniform_LightNamesCount)
            {
                names.Resize(MaterialUniform_LightNamesCount);

                // Ambient.
                names[MaterialUniform_AmbientLightFS_Colour]                   = GTLib::String::CreateFormatted("AmbientLightFS%d.Colour",                   index);

                // Directional.
                names[MaterialUniform_DirectionalLightFS_Colour]               = GTLib::String::CreateFormatted("DirectionalLightFS%d.Colour",               index);
                names[MaterialUniform_DirectionalLightFS_Direction]            = GTLib::String::CreateFormatted("DirectionalLightFS%d.Direction",            index);

                // Point.
                names[MaterialUniform_PointLightVS_PositionVS]                 = GTLib::String::CreateFormatted("PointLightVS%d.PositionVS",                 index);
                names[MaterialUniform_PointLightFS_Colour]                     = GTLib::String::CreateFormatted("PointLightFS%d.Colour",                     index);
                names[MaterialUniform_PointLightFS_Radius]                     = GTLib::String::CreateFormatted("PointLightFS%d.Radius",                     index);
                names[MaterialUniform_PointLightFS_Falloff]                    = GTLib::String::CreateFormatted("PointLightFS%d.Falloff",                    index);

                // Spot.
                names[MaterialUniform_SpotLightFS_Position]                    = GTLib::String::CreateFormatted("SpotLightFS%d.Position",                    index);
                names[MaterialUniform_SpotLightFS_Colour]                      = GTLib::String::CreateFormatted("SpotLightFS%d.Colour",                      index);
                names[MaterialUniform_SpotLightFS_Direction]                   = GTLib::String::CreateFormatted("SpotLightFS%d.Direction",                   index);
                names[MaterialUniform_SpotLightFS_Length]                      = GTLib::String::CreateFormatted("SpotLightFS%d.Length",                      index);
                names[MaterialUniform_SpotLightFS_Falloff]                     = GTLib::String::CreateFormatted("SpotLightFS%d.Falloff",                     index);
                names[MaterialUniform_SpotLightFS_CosAngleInner]               = GTLib::String::CreateFormatted("SpotLightFS%d.CosAngleInner",               index);
                names[MaterialUniform_SpotLightFS_CosAngleOuter]               = GTLib::String::CreateFormatted("SpotLightFS%d.CosAngleOuter",               index);

                // Shadow Directional.
                names[MaterialUniform_ShadowDirectionalLightVS_ProjectionView] = GTLib::String::CreateFormatted("ShadowDirectionalLightVS%d.ProjectionView", index);
                names[MaterialUniform_ShaderDirectionalLightFS_Colour]         = GTLib::String::CreateFormatted("ShadowDirectionalLightFS%d.Colour",         index);
                names[MaterialUniform_ShaderDirectionalLightFS_Direction]      = GTLib::String::CreateFormatted("ShadowDirectionalLightFS%d.Direction",      index);
                names[MaterialUniform_ShaderDirectionalLightFS_ShadowMap]      = GTLib::String::CreateFormatted("ShadowDirectionalLightFS%d_ShadowMap",      index);

                // Shadow Point.
                names[MaterialUniform_ShadowPointLightFS_PositionVS]           = GTLib::String::CreateFormatted("ShadowPointLightVS%d.PositionVS",           index);
                names[MaterialUniform_ShadowPointLightFS_PositionWS]           = GTLib::String::CreateFormatted("ShadowPointLightVS%d.PositionWS",           index);
                names[MaterialUniform_ShadowPointLightFS_Colour]               = GTLib::String::CreateFormatted("ShadowPointLightFS%d.Colour",               index);
                names[MaterialUniform_ShadowPointLightFS_Radius]               = GTLib::String::CreateFormatted("ShadowPointLightFS%d.Radius",               index);
                names[MaterialUniform_ShadowPointLightFS_Falloff]              = GTLib::String::CreateFormatted("ShadowPointLightFS%d.Falloff",              index);
                names[MaterialUniform_ShadowPointLightFS_ShadowMap]            = GTLib::String::CreateFormatted("ShadowPointLightFS%d_ShadowMap",            index);

                // Shadow Spot.
                names[MaterialUniforms_ShadowSpotLightVS_ProjectionView]       = GTLib::String::CreateFormatted("ShadowSpotLightVS%d.ProjectionView",        index);
                names[MaterialUniforms_ShadowSpotLightFS_Position]             = GTLib::String::CreateFormatted("ShadowSpotLightFS%d.Position",              index);
                names[MaterialUniforms_ShadowSpotLightFS_Colour]               = GTLib::String::CreateFormatted("ShadowSpotLightFS%d.Colour",                index);
                names[MaterialUniforms_ShadowSpotLightFS_Direction]            = GTLib::String::CreateFormatted("ShadowSpotLightFS%d.Direction",             index);
                names[MaterialUniforms_ShadowSpotLightFS_Length]               = GTLib::String::CreateFormatted("ShadowSpotLightFS%d.Length",                index);
                names[MaterialUniforms_ShadowSpotLightFS_Falloff]              = GTLib::String::CreateFormatted("ShadowSpotLightFS%d.Falloff",               index);
                names[MaterialUniforms_ShadowSpotLightFS_CosAngleInner]        = GTLib::String::CreateFormatted("ShadowSpotLightFS%d.CosAngleInner",         index);
                names[MaterialUniforms_ShadowSpotLightFS_CosAngleOuter]        = GTLib::String::CreateFormatted("ShadowSpotLightFS%d.CosAngleOuter",         index);
                names[MaterialUniforms_ShadowSpotLightFS_ShadowMap]            = GTLib::String::CreateFormatted("ShadowSpotLightFS%d_ShadowMap",             index);
            }

            /// The list of names for the light index.
            GTLib::Vector<GTLib::String> names;
        };



    public:
        
        /// Constructor.
        DefaultSceneRenderer_MaterialUniformNameCache();


        /// Retrieves the name of a light uniform.
        ///
        /// @param nameClass [in] The identifier of the name to retrieve.
        /// @param index     [in] The index of the light whose uniform is being retrieved.
        const char* GetLightUniformName(MaterialUniformNameID nameID, int index);


    private:

        /// The caches for light uniforms.
        GTLib::Vector<LightIndexUniformNames> lightUniforms;
    };
}

#endif