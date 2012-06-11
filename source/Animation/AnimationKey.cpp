
#include <GTEngine/Animation/AnimationKey.hpp>

namespace GTEngine
{
    AnimationKey::AnimationKey()
    {
    }

    AnimationKey::~AnimationKey()
    {
    }
}


//////////////////////////////////////////////////////////////
// TransformAnimationKey

namespace GTEngine
{
    TransformAnimationKey::TransformAnimationKey(const glm::vec3 &positionIn, const glm::quat &rotationIn, const glm::vec3 &scaleIn)
        : position(positionIn), rotation(rotationIn), scale(scaleIn)
    {
    }

    TransformAnimationKey::~TransformAnimationKey()
    {
    }
}
