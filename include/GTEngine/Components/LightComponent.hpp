// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_LightComponent_hpp_
#define __GTEngine_LightComponent_hpp_

#include "../Component.hpp"
#include "../Math.hpp"

/// PointLight.
namespace GTEngine
{
    class PointLightComponent : public Component
    {
    public:
        
        /// Constructor.
        PointLightComponent(SceneNode &node);
        
        
        /// Retrieves the colour of the light.
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /// Sets the colour of the light.
        void SetColour(const glm::vec3 &colour);
        void SetColour(float x, float y, float z) { this->SetColour(glm::vec3(x, y, z)); }



        /// Sets the radius of the light.
        void SetRadius(float radius);

        /// Retrieves the radius of the light.
        float GetRadius() const;


        /// Sets the falloff rate factor.
        void SetFalloff(float falloff);

        /// Retrieves the falloff rate factor.
        float GetFalloff() const;


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
        

        /// The radius of the light.
        float radius;

        /// The exponential falloff factor.
        float falloff;


        /// Keeps track of whether or not the light should cast shadows.
        bool castShadows;



        GTENGINE_DECL_COMPONENT_ATTRIBS(PointLightComponent)
    };
}





/// SpotLight.
namespace GTEngine
{
    class SpotLightComponent : public Component
    {
    public:
        
        /// Constructor.
        SpotLightComponent(SceneNode &node);
        
        
        /// Retrieves the colour of the light.
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /// Sets the colour of the light.
        void SetColour(const glm::vec3 &colour);
        void SetColour(float x, float y, float z) { this->SetColour(glm::vec3(x, y, z)); }

        

        /// Sets the radius of the light.
        void SetLength(float radius);

        /// Retrieves the radius of the light.
        float GetLength() const;


        /// Sets the falloff rate factor.
        void SetFalloff(float falloff);

        /// Retrieves the falloff rate factor.
        float GetFalloff() const;



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


        /// The length of the spot light.
        float length;

        /// The exponential falloff factor.
        float falloff;


        /// Keeps track of whether or not the light casts shadows.
        bool castShadows;



        GTENGINE_DECL_COMPONENT_ATTRIBS(SpotLightComponent)
    };
}



// DirectionalLight
namespace GTEngine
{
    class DirectionalLightComponent : public Component
    {
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




        GTENGINE_DECL_COMPONENT_ATTRIBS(DirectionalLightComponent)
    };
}



// DirectionalLight
namespace GTEngine
{
    class AmbientLightComponent : public Component
    {
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



        GTENGINE_DECL_COMPONENT_ATTRIBS(AmbientLightComponent)
    };
}



#endif
