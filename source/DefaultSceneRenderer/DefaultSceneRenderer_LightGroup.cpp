// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_LightGroup.hpp>

namespace GTEngine
{
    DefaultSceneRenderer_LightGroup::DefaultSceneRenderer_LightGroup()
        : id(), lightIDs()
    {
    }


    void DefaultSceneRenderer_LightGroup::AddAmbientLight(uint32_t lightID)
    {
        this->lightIDs.InsertAt(lightID, this->GetAmbientLightStartIndex() + this->GetAmbientLightCount());
        this->id.AddAmbientLight();
    }

    void DefaultSceneRenderer_LightGroup::AddDirectionalLight(uint32_t lightID)
    {
        this->lightIDs.InsertAt(lightID, this->GetDirectionalLightStartIndex() + this->GetDirectionalLightCount());
        this->id.AddDirectionalLight();
    }

    void DefaultSceneRenderer_LightGroup::AddPointLight(uint32_t lightID)
    {
        this->lightIDs.InsertAt(lightID, this->GetPointLightStartIndex() + this->GetPointLightCount());
        this->id.AddPointLight();
    }

    void DefaultSceneRenderer_LightGroup::AddSpotLight(uint32_t lightID)
    {
        this->lightIDs.InsertAt(lightID, this->GetSpotLightStartIndex() + this->GetSpotLightCount());
        this->id.AddSpotLight();
    }

    void DefaultSceneRenderer_LightGroup::AddShadowDirectionalLight(uint32_t lightID)
    {
        this->lightIDs.InsertAt(lightID, this->GetShadowDirectionalLightStartIndex() + this->GetShadowDirectionalLightCount());
        this->id.AddShadowDirectionalLight();
    }

    void DefaultSceneRenderer_LightGroup::AddShadowPointLight(uint32_t lightID)
    {
        this->lightIDs.InsertAt(lightID, this->GetShadowPointLightStartIndex() + this->GetShadowPointLightCount());
        this->id.AddShadowPointLight();
    }

    void DefaultSceneRenderer_LightGroup::AddShadowSpotLight(uint32_t lightID)
    {
        this->lightIDs.InsertAt(lightID, this->GetShadowSpotLightStartIndex() + this->GetShadowSpotLightCount());
        this->id.AddShadowSpotLight();
    }



    size_t DefaultSceneRenderer_LightGroup::GetAmbientLightStartIndex() const
    {
        return 0;
    }

    size_t DefaultSceneRenderer_LightGroup::GetDirectionalLightStartIndex() const
    {
        return this->GetAmbientLightStartIndex() + this->GetAmbientLightCount();
    }

    size_t DefaultSceneRenderer_LightGroup::GetPointLightStartIndex() const
    {
        return this->GetDirectionalLightStartIndex() + this->GetDirectionalLightCount();
    }

    size_t DefaultSceneRenderer_LightGroup::GetSpotLightStartIndex() const
    {
        return this->GetPointLightStartIndex() + this->GetPointLightCount();
    }

    size_t DefaultSceneRenderer_LightGroup::GetShadowDirectionalLightStartIndex() const
    {
        return this->GetSpotLightStartIndex() + this->GetSpotLightCount();
    }

    size_t DefaultSceneRenderer_LightGroup::GetShadowPointLightStartIndex() const
    {
        return this->GetShadowDirectionalLightStartIndex() + this->GetShadowDirectionalLightCount();
    }

    size_t DefaultSceneRenderer_LightGroup::GetShadowSpotLightStartIndex() const
    {
        return this->GetShadowPointLightStartIndex() + this->GetShadowPointLightCount();
    }



    uint16_t DefaultSceneRenderer_LightGroup::GetAmbientLightCount() const
    {
        return this->id.GetAmbientLightCount();
    }

    uint16_t DefaultSceneRenderer_LightGroup::GetDirectionalLightCount() const
    {
        return this->id.GetDirectionalLightCount();
    }

    uint16_t DefaultSceneRenderer_LightGroup::GetPointLightCount() const
    {
        return this->id.GetPointLightCount();
    }

    uint16_t DefaultSceneRenderer_LightGroup::GetSpotLightCount() const
    {
        return this->id.GetSpotLightCount();
    }

    uint16_t DefaultSceneRenderer_LightGroup::GetShadowDirectionalLightCount() const
    {
        return this->id.GetShadowDirectionalLightCount();
    }

    uint16_t DefaultSceneRenderer_LightGroup::GetShadowPointLightCount() const
    {
        return this->id.GetShadowPointLightCount();
    }

    uint16_t DefaultSceneRenderer_LightGroup::GetShadowSpotLightCount() const
    {
        return this->id.GetShadowSpotLightCount();
    }


    void DefaultSceneRenderer_LightGroup::Clear()
    {
        this->id.Reset();
        this->lightIDs.Clear();
    }
}