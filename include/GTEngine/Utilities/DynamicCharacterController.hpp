
#ifndef __GTEngine_Utilities_DynamicCharacterController_hpp_
#define __GTEngine_Utilities_DynamicCharacterController_hpp_

#include "../SceneNode.hpp"

namespace GTEngine
{
    /// Class representing a dynamic character controller. This is not a kinematic body, but instead a rigid body with a set mass.
    class DynamicCharacterController
    {
    public:

        /// Constructor.
        DynamicCharacterController(float height = 2.0f, float radius = 0.5f);

        /// Destructor.
        ~DynamicCharacterController();


        /// Retrieves the internal scene node.
              GTEngine::SceneNode & GetSceneNode()       { return this->sceneNode; }
        const GTEngine::SceneNode & GetSceneNode() const { return this->sceneNode; }


        /// Sets the mass of the character.
        void SetMass(float newMass);


        /// Sets the linear velocity of the character.
        ///
        /// @param velocity [in] The new velocity.
        void SetLinearVelocity(const glm::vec3 &velocity);
        void SetLinearVelocity(float x, float y, float z) { this->SetLinearVelocity(x, y, z); }

        /// Retrieves the linear velocity of the character.
        glm::vec3 GetLinearVelocity() const;



    private:

        /// The internal scene node.
        GTEngine::SceneNode sceneNode;
    };
}

#endif