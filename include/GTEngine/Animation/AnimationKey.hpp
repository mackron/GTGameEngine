// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_AnimationKey
#define GT_AnimationKey

#include "../Math.hpp"

namespace GT
{
    /// Base class for animation keys.
    class AnimationKey
    {
    public:

        /// Constructor.
        AnimationKey();

        /// Destructor.
        virtual ~AnimationKey();
    };




    /// Animation key for transformations.
    class TransformAnimationKey : public AnimationKey
    {
    public:

        /// Constructor.
        TransformAnimationKey(const glm::vec3 &positionIn, const glm::quat &rotationIn, const glm::vec3 &scaleIn);

        /// Destructor.
        ~TransformAnimationKey();


    public:

        /// The position.
        glm::vec3 position;

        /// The rotation.
        glm::quat rotation;

        /// The scale.
        glm::vec3 scale;
    };
}

#endif