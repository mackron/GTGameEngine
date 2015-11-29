// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Animation/AnimationKey.hpp>

namespace GT
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

namespace GT
{
    TransformAnimationKey::TransformAnimationKey(const glm::vec3 &positionIn, const glm::quat &rotationIn, const glm::vec3 &scaleIn)
        : position(positionIn), rotation(rotationIn), scale(scaleIn)
    {
    }

    TransformAnimationKey::~TransformAnimationKey()
    {
    }
}
