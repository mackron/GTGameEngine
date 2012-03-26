
#include <GTEngine/Physics/KinematicMotionState.hpp>

namespace GTEngine
{
    KinematicMotionState::KinematicMotionState()
        : transform(btMatrix3x3::getIdentity(), btVector3(0.0f, 0.0f, 0.0f))
    {
    }

    KinematicMotionState::KinematicMotionState(const glm::mat4 &initialTransform)
        : transform()
    {
        transform.setFromOpenGLMatrix(&initialTransform[0][0]);
    }

    KinematicMotionState::~KinematicMotionState()
    {
    }

    void KinematicMotionState::setWorldTransform(const glm::mat4 &worldTransform)
    {
        this->transform.setFromOpenGLMatrix(&worldTransform[0][0]);
    }
}