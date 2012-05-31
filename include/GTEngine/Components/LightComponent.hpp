
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
        
        /**
        *   \brief  Retrievs the colour of the light.
        */
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /**
        *   \brief  Sets the colour of the light.
        */
        void SetColour(float r, float g, float b) { this->SetColour(glm::vec3(r, g, b)); }
        void SetColour(const glm::vec3 &colour) { this->colour = colour; }
        
        /**
        *   \brief  Retrieves the constant attenuation.
        */
        float GetConstantAttenuation() const { return this->constantAttenuation; }
        
        /**
        *   \brief  Retrieves the linear attenuation.
        */
        float GetLinearAttenuation() const { return this->linearAttenuation; }
        
        /**
        *   \brief  Retrieves the quadratic attenuation.
        */
        float GetQuadraticAttenuation() const { return this->quadraticAttenuation; }
        
        /**
        *   \brief  Sets the attenuation of the light.
        */
        void SetAttenuation(float constantAttenuation, float linearAttenuation, float quadraticAttenuation);
        
        /**
        *   \brief  Retrieves an approximate radius of the light.
        */
        float GetApproximateRadius() const;

        
    public:

        /// The colour of the light. No alpha.
        glm::vec3 colour;
        
        /// The constant attenuation.
        float constantAttenuation;
        
        /// The linear attenuation.
        float linearAttenuation;
        
        /// The quadratic attenuation.
        float quadraticAttenuation;

    GTENGINE_DECL_COMPONENT_END()
}

/// SpotLight.
namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(SpotLightComponent)
    public:
        
        /// Constructor.
        SpotLightComponent(SceneNode &node);
        
        /**
        *   \brief  Retrievs the colour of the light.
        */
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /**
        *   \brief  Sets the colour of the light.
        */
        void SetColour(const glm::vec3 &colour) { this->colour = colour; }
        
        /**
        *   \brief  Retrieves the constant attenuation.
        */
        float GetConstantAttenuation() const { return this->constantAttenuation; }
        
        /**
        *   \brief  Retrieves the linear attenuation.
        */
        float GetLinearAttenuation() const { return this->linearAttenuation; }
        
        /**
        *   \brief  Retrieves the quadratic attenuation.
        */
        float GetQuadraticAttenuation() const { return this->quadraticAttenuation; }
        
        /**
        *   \brief  Sets the attenuation of the light.
        */
        void SetAttenuation(float constantAttenuation, float linearAttenuation, float quadraticAttenuation);
        
        /// Retrieves the approximate length of the light based on it's attenuation.
        float GetApproximateLength() const;


        /// Sets the inner and outer angles of the spot light.
        void SetAngles(float newInnerAngle, float newOuterAngle)
        {
            if (newInnerAngle > newOuterAngle)
            {
                newInnerAngle = newOuterAngle;
            }

            this->innerAngle = newInnerAngle;
            this->outerAngle = newOuterAngle;
        }

        /// Retrieves the inner angle of the light.
        float GetInnerAngle() const { return this->innerAngle; }

        /// Retrieves the outer angle of the light.
        float GetOuterAngle() const { return this->outerAngle; }

        

        
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

    GTENGINE_DECL_COMPONENT_END()
}


// DirectionalLight
namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(DirectionalLightComponent)
    public:
        
        /// Constructor.
        DirectionalLightComponent(SceneNode &node);
        
        /**
        *   \brief  Initialises the light.
        */
        void Initialise(const glm::vec3 &colour);
        
        /**
        *   \brief  Retrievs the colour of the light.
        */
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /**
        *   \brief  Sets the colour of the light.
        */
        void SetColour(const glm::vec3 &colour) { this->colour = colour; }

        
    public:

        /// The colour of the light. No alpha.
        glm::vec3 colour;

    GTENGINE_DECL_COMPONENT_END()
}


// DirectionalLight
namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(AmbientLightComponent)
    public:
        
        /// Constructor.
        AmbientLightComponent(SceneNode &node);
        
        /**
        *   \brief  Initialises the light.
        */
        void Initialise(const glm::vec3 &colour);
        
        /**
        *   \brief  Retrievs the colour of the light.
        */
        const glm::vec3 & GetColour() const { return this->colour; }
        
        /**
        *   \brief  Sets the colour of the light.
        */
        void SetColour(const glm::vec3 &colour) { this->colour = colour; }

        
    public:

        /// The colour of the light. No alpha.
        glm::vec3 colour;

    GTENGINE_DECL_COMPONENT_END()
}



#endif
