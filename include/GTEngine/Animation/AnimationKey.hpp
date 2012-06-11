
#ifndef __GTEngine_AnimationKey_hpp_
#define __GTEngine_AnimationKey_hpp_

#include "../Math.hpp"

namespace GTEngine
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


    private:

        /// The position.
        glm::vec3 position;

        /// The rotation.
        glm::quat rotation;

        /// The scale.
        glm::vec3 scale;
    };
}

#endif