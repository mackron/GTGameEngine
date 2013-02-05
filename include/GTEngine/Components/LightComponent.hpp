// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_LightComponent_hpp_
#define __GTEngine_LightComponent_hpp_

#include "../Component.hpp"
#include "../Math.hpp"

/// PointLight.
namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(PointLightComponent)
    public:
        
        /// Constructor.
        PointLightComponent(SceneNode &node);
        
        
        /// Retrieves the colour of the light.
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /// Sets the colour of the light.
        void SetColour(const glm::vec3 &colour);
        void SetColour(float x, float y, float z) { this->SetColour(glm::vec3(x, y, z)); }


        /// Retrieves the constant attenuation.
        float GetConstantAttenuation() const { return this->constantAttenuation; }
        
        /// Retrieves the linear attenuation.
        float GetLinearAttenuation() const { return this->linearAttenuation; }
        
        /// Retrieves the quadratic attenuation.
        float GetQuadraticAttenuation() const { return this->quadraticAttenuation; }
        
        /// Sets the attenuation of the light.
        void SetAttenuation(float constantAttenuation, float linearAttenuation, float quadraticAttenuation);
        
        /// Retrieves an approximate radius of the light.
        float GetApproximateRadius() const;


        /// Enables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void EnableShadowCasting();

        /// Disables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void DisableShadowCasting();

        /// Determines whether or not this model casts shadows.
        bool IsShadowCastingEnabled() const { return this->castShadows; }



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);

        
    public:

        /// The colour of the light. No alpha.
        glm::vec3 colour;
        
        /// The constant attenuation.
        float constantAttenuation;
        
        /// The linear attenuation.
        float linearAttenuation;
        
        /// The quadratic attenuation.
        float quadraticAttenuation;

        /// Keeps track of whether or not the light should cast shadows.
        bool castShadows;

    GTENGINE_DECL_COMPONENT_END()
}





/// SpotLight.
namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(SpotLightComponent)
    public:
        
        /// Constructor.
        SpotLightComponent(SceneNode &node);
        
        
        /// Retrieves the colour of the light.
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /// Sets the colour of the light.
        void SetColour(const glm::vec3 &colour);
        void SetColour(float x, float y, float z) { this->SetColour(glm::vec3(x, y, z)); }

        
        /// Retrieves the constant attenuation.
        float GetConstantAttenuation() const { return this->constantAttenuation; }
        
        /// Retrieves the linear attenuation.
        float GetLinearAttenuation() const { return this->linearAttenuation; }
        
        /// Retrieves the quadratic attenuation.
        float GetQuadraticAttenuation() const { return this->quadraticAttenuation; }
        
        /// Sets the attenuation of the light.
        void SetAttenuation(float constantAttenuation, float linearAttenuation, float quadraticAttenuation);

        
        /// Retrieves the approximate length of the light based on it's attenuation.
        float GetApproximateLength() const;


        /// Sets the inner and outer angles of the spot light.
        void SetAngles(float newInnerAngle, float newOuterAngle);

        /// Retrieves the inner angle of the light.
        float GetInnerAngle() const { return this->innerAngle; }

        /// Retrieves the outer angle of the light.
        float GetOuterAngle() const { return this->outerAngle; }

        
        /// Enables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void EnableShadowCasting();

        /// Disables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void DisableShadowCasting();

        /// Determines whether or not this model casts shadows.
        bool IsShadowCastingEnabled() const { return this->castShadows; }



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);


        
    public:

        /// The inner angle of the spot light.
        float innerAngle;

        /// The outer angle of the spot light.
        float outerAngle;

        /// The colour of the light. No alpha.
        glm::vec3 colour;
        
        /// The constant attenuation.
        float constantAttenuation;
        
        /// The linear attenuation.
        float linearAttenuation;
        
        /// The quadratic attenuation.
        float quadraticAttenuation;

        /// Keeps track of whether or not the light casts shadows.
        bool castShadows;

    GTENGINE_DECL_COMPONENT_END()
}



// DirectionalLight
namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(DirectionalLightComponent)
    public:
        
        /// Constructor.
        DirectionalLightComponent(SceneNode &node);

        
        /// Retrieves the colour of the light.
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /// Sets the colour of the light.
        void SetColour(const glm::vec3 &colour);
        void SetColour(float x, float y, float z) { this->SetColour(glm::vec3(x, y, z)); }


        /// Enables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void EnableShadowCasting();

        /// Disables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void DisableShadowCasting();

        /// Determines whether or not this model casts shadows.
        bool IsShadowCastingEnabled() const { return this->castShadows; }



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);

        
    public:

        /// The colour of the light. No alpha.
        glm::vec3 colour;

        /// Keeps track of whether or not the light casts shadows.
        bool castShadows;


    GTENGINE_DECL_COMPONENT_END()
}



// DirectionalLight
namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(AmbientLightComponent)
    public:
        
        /// Constructor.
        AmbientLightComponent(SceneNode &node);
        

        /// Retrieves the colour of the light.
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /// Sets the colour of the light.
        void SetColour(const glm::vec3 &colour);
        void SetColour(float x, float y, float z) { this->SetColour(glm::vec3(x, y, z)); }



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);

        
    public:

        /// The colour of the light. No alpha.
        glm::vec3 colour;

    GTENGINE_DECL_COMPONENT_END()
}



#endif
