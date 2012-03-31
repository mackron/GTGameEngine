
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
        *   \brief  Initialises the light.
        */
        void Initialise(const glm::vec3 &colour, float constantAttenuation, float linearAttenuation, float quadraticAttenuation);
        
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
        
        /**
        *   \brief  Retrieves an approximate radius of the light.
        */
        float GetApproximateRadius();

        
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
