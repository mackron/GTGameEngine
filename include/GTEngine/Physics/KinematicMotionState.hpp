
#ifndef __GTEngine_Physics_KinematicMotionState_hpp_
#define __GTEngine_Physics_KinematicMotionState_hpp_

#include "Bullet.hpp"
#include "../Math.hpp"

namespace GTEngine
{
    class KinematicMotionState : public btMotionState
    {
    public:

        /// Constructor.
        KinematicMotionState();

        /// Constructor.
        KinematicMotionState(const glm::mat4 &initialTransform);

        /// Destructor.
        virtual ~KinematicMotionState();

        /**
        *   \brief  Sets the world transform from a GLM matrix.
        */
        void setWorldTransform(const glm::mat4 &worldTransform);


    // btMotionState implementations.
    public:

        /// getWorldTransform()
        virtual void getWorldTransform(btTransform &worldTransform) const { worldTransform = this->transform; }

        /// setWorldTransform()
        virtual void setWorldTransform(const btTransform &worldTransform) { this->transform = worldTransform; };


    private:

        /// The current transform. We use a btTransform here so we do fast retrievals with getWorldTransform().
        btTransform transform;
    };
}

#endif