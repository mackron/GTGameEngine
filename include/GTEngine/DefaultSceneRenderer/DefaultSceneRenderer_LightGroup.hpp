// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_LightGroup_hpp_
#define __GTEngine_DefaultSceneRenderer_LightGroup_hpp_

#include <GTCore/Vector.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior: elements of array will be default initialized
#endif

namespace GTEngine
{
    /// Structure representing a value for identifying a combination of light types.
    ///
    /// Each type of light is allocated 16 bits, which means the maximum count of each light is 65,535.
    struct DefaultSceneRenderer_LightGroupID
    {
        /// The counts of each type of light. Each light is allocated 16-bits.
        ///
        /// 0 = Ambient lights.
        /// 1 = Directional lights.
        /// 2 = Point lights.
        /// 3 = Spot lights.
        /// 4 = Shadow-casting directional lights.
        /// 5 = Shadow-casting point lights.
        /// 6 = Shadow-casting spot lights.
        uint16_t value[8];


        DefaultSceneRenderer_LightGroupID()
            : value()
        {
            value[0] = 0;
            value[1] = 0;
            value[2] = 0;
            value[3] = 0;
            value[4] = 0;
            value[5] = 0;
            value[6] = 0;
            value[7] = 0;
        }


        void SetAmbientLightCount(uint16_t count)
        {
            value[0] = count;
        }

        void SetDirectionalLightCount(uint16_t count)
        {
            value[1] = count;
        }

        void SetPointLightCount(uint16_t count)
        {
            value[2] = count;
        }

        void SetSpotLightCount(uint16_t count)
        {
            value[3] = count;
        }

        void SetShadowDirectionalLightCount(uint16_t count)
        {
            value[4] = count;
        }

        void SetShadowPointLightCount(uint16_t count)
        {
            value[5] = count;
        }

        void SetShadowSpotLightCount(uint16_t count)
        {
            value[6] = count;
        }



        uint16_t GetAmbientLightCount() const
        {
            return value[0];
        }

        uint16_t GetDirectionalLightCount() const
        {
            return value[1];
        }

        uint16_t GetPointLightCount() const
        {
            return value[2];
        }

        uint16_t GetSpotLightCount() const
        {
            return value[3];
        }

        uint16_t GetShadowDirectionalLightCount() const
        {
            return value[4];
        }

        uint16_t GetShadowPointLightCount() const
        {
            return value[5];
        }

        uint16_t GetShadowSpotLightCount() const
        {
            return value[6];
        }




        void AddAmbientLight()
        {
            this->SetAmbientLightCount(this->GetAmbientLightCount() + 1);
        }

        void AddDirectionalLight()
        {
            this->SetDirectionalLightCount(this->GetDirectionalLightCount() + 1);
        }

        void AddPointLight()
        {
            this->SetPointLightCount(this->GetPointLightCount() + 1);
        }

        void AddSpotLight()
        {
            this->SetSpotLightCount(this->GetSpotLightCount() + 1);
        }

        void AddShadowDirectionalLight()
        {
            this->SetShadowDirectionalLightCount(this->GetShadowDirectionalLightCount() + 1);
        }

        void AddShadowPointLight()
        {
            this->SetShadowPointLightCount(this->GetShadowPointLightCount() + 1);
        }

        void AddShadowSpotLight()
        {
            this->SetShadowSpotLightCount(this->GetShadowSpotLightCount() + 1);
        }



        void RemoveAmbientLight()
        {
            auto count = this->GetAmbientLightCount();
            if (count > 0)
            {
                this->SetAmbientLightCount(count - 1);
            }
        }

        void RemoveDirectionalLight()
        {
            auto count = this->GetDirectionalLightCount();
            if (count > 0)
            {
                this->SetDirectionalLightCount(count - 1);
            }
        }

        void RemovePointLight()
        {
            auto count = this->GetPointLightCount();
            if (count > 0)
            {
                this->SetPointLightCount(count - 1);
            }
        }

        void RemoveSpotLight()
        {
            auto count = this->GetSpotLightCount();
            if (count > 0)
            {
                this->SetSpotLightCount(count - 1);
            }
        }

        void RemoveShadowDirectionalLight()
        {
            auto count = this->GetShadowDirectionalLightCount();
            if (count > 0)
            {
                this->SetShadowDirectionalLightCount(count - 1);
            }
        }

        void RemoveShadowPointLight()
        {
            auto count = this->GetShadowPointLightCount();
            if (count > 0)
            {
                this->SetShadowPointLightCount(count - 1);
            }
        }

        void RemoveShadowSpotLight()
        {
            auto count = this->GetShadowSpotLightCount();
            if (count > 0)
            {
                this->SetShadowSpotLightCount(count - 1);
            }
        }
    };


    /// Structure representing a group of lights.
    ///
    /// Each light is representing as a simple 32-bit value.
    struct DefaultSceneRenderer_LightGroup
    {
        /// The ID of the light group.
        DefaultSceneRenderer_LightGroupID id;

        /// The list of light IDs.
        GTCore::Vector<uint32_t> lightIDs;



        /// Constructor.
        DefaultSceneRenderer_LightGroup();


        /// Adds an ambient light.
        ///
        /// @param lightID [in] The ID of the light.
        void AddAmbientLight(uint32_t lightID);

        /// Adds a directional light.
        ///
        /// @param lightID [in] The ID of the light.
        void AddDirectionalLight(uint32_t lightID);

        /// Adds a point light.
        ///
        /// @param lightID [in] The ID of the light.
        void AddPointLight(uint32_t lightID);

        /// Adds a spot light.
        ///
        /// @param lightID [in] The ID of the light.
        void AddSpotLight(uint32_t lightID);

        /// Adds a shadow-casting directional light.
        ///
        /// @param lightID [in] The ID of the light.
        void AddShadowDirectionalLight(uint32_t lightID);

        /// Adds a shadow-casting point light.
        ///
        /// @param lightID [in] The ID of the light.
        void AddShadowPointLight(uint32_t lightID);

        /// Adds a shadow-casting spot light.
        ///
        /// @param lightID [in] The ID of the light.
        void AddShadowSpotLight(uint32_t lightID);


        /// Retrieves the index of the first ambient light.
        size_t GetAmbientLightStartIndex() const;

        /// Retrieves the index of the first directional light.
        size_t GetDirectionalLightStartIndex() const;

        /// Retrieves the index of the first point light.
        size_t GetPointLightStartIndex() const;

        /// Retrieves the index of the first spot light.
        size_t GetSpotLightStartIndex() const;

        /// Retrieves the index of the first shadow-casting directional light.
        size_t GetShadowDirectionalLightStartIndex() const;

        /// Retrieves the index of the first shadow-casting point light.
        size_t GetShadowPointLightStartIndex() const;

        /// Retrieves the index of the first shadow-casting spot light.
        size_t GetShadowSpotLightStartIndex() const;


        /// Retrieves the number of ambient lights in the group.
        uint16_t GetAmbientLightCount() const;

        /// Retrieves the number of directional lights in the group.
        uint16_t GetDirectionalLightCount() const;

        /// Retrieves the number of point lights in the group.
        uint16_t GetPointLightCount() const;

        /// Retrieves the number of spot lights in the group.
        uint16_t GetSpotLightCount() const;

        /// Retrieves the number of shadow-casting directionl lights in the group.
        uint16_t GetShadowDirectionalLightCount() const;

        /// Retrieves the number of shadow-casting point lights in the group.
        uint16_t GetShadowPointLightCount() const;

        /// Retrieves the number of shadow-casting spot lights in the group.
        uint16_t GetShadowSpotLightCount() const;
    };
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif