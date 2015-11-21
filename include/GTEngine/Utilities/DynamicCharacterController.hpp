// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Utilities_DynamicCharacterController_hpp_
#define __GTEngine_Utilities_DynamicCharacterController_hpp_

#include "../SceneNode.hpp"
#include "CharacterControllerEventHandler.hpp"

namespace GTEngine
{
    /// Class representing a dynamic character controller. This is not a kinematic body, but instead a rigid body with a set mass.
    ///
    /// This class assumes a -y direction for gravity at the moment.
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
        void SetLinearVelocity(float x, float y, float z) { this->SetLinearVelocity(glm::vec3(x, y, z)); }

        /// Retrieves the linear velocity of the character.
        glm::vec3 GetLinearVelocity() const;


        /// Determines whether or not the controller is on the ground.
        bool IsOnGround() const;


        /// Determines whether or not the controller is able to jump.
        ///
        /// @remarks
        ///     This will be determined based on the current velocity and whether or not the character is standing on anything.
        bool CanJump() const;



        /// Sets the event handler.
        ///
        /// @param eventHandler [in] A reference to the new event handler.
        ///
        /// @remarks
        ///     Use remove event handler to remove it.
        void SetEventHandler(CharacterControllerEventHandler &eventHandler);

        /// Removes the current event handler.
        void RemoveEventHandler();



        ///////////////////////////////////////////////////////
        // Events

        /// Called when the scene node of the character controller is updated.
        void OnUpdate(double deltaTimeInSeconds);

        /// Called when the character has landed onto the ground.
        void OnLand();

        /// Called when the character is raised off the ground.
        void OnRaise();

        /// Called when the character hits it's head.
        void OnHitHead();



    private:

        /// The internal scene node.
        GTEngine::SceneNode sceneNode;

        /// The event handler for the scene node. We need this so we can handle OnUpdate() and check whether or not the player is on the ground.
        class SceneNodeEventHandler : public GTEngine::SceneNodeEventHandler
        {
        public:

            SceneNodeEventHandler(DynamicCharacterController &controllerIn)
                : controller(controllerIn)
            {
            }

            void OnUpdate(GTEngine::SceneNode &, double deltaTimeInSeconds)
            {
                this->controller.OnUpdate(deltaTimeInSeconds);
            }



        private:

            DynamicCharacterController &controller;


        private:    // No copying
            SceneNodeEventHandler(const SceneNodeEventHandler &);
            SceneNodeEventHandler & operator=(const SceneNodeEventHandler &);

        }sceneNodeEventHandler;


        /// The height of the player.
        float height;

        /// The radius of the player.
        float radius;


        /// Keeps track of whether or not we are on the ground.
        bool onGround;

        /// Keeps track of whether or not the players head is hitting something.
        bool isHeadTouchingSomething;


        /// A pointer to the event handler.
        CharacterControllerEventHandler* m_eventHandler;


    private:    // No copying.
        DynamicCharacterController(const DynamicCharacterController &);
        DynamicCharacterController & operator=(const DynamicCharacterController &);
    };
}

#endif
