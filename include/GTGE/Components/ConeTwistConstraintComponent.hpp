// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ConeTwistConstraintComponent
#define GT_ConeTwistConstraintComponent

#include "../Component.hpp"
#include "../Physics.hpp"

namespace GT
{
    /// Component class for a cone twist phsics constraint.
    class ConeTwistConstraintComponent : public Component
    {
    public:

        /// Constructor.
        ConeTwistConstraintComponent(SceneNode &node);

        /// Destructor.
        ~ConeTwistConstraintComponent();


        /// Sets the bodies that are attached to the constraint.
        ///
        /// @param sceneNodeA [in] A reference to the first scene node to attach to the constraint.
        /// @param sceneNodeB [in] A reference to the second scene node to attach to the constraint.
        /// @param frameA     [in] The transformation frame for objectA.
        /// @param frameB     [in] The transformation frame for objectB.
        void SetAttachments(SceneNode &sceneNodeA, SceneNode &sceneNodeB, const glm::mat4 &frameA, const glm::mat4 &frameB);
        void SetAttachments(SceneNode &sceneNodeB, const glm::mat4 &frameB);


        /// Sets the limits.
        void SetLimits(float swingSpan1, float swingSpan2, float twistSpan);


        /// Retrieves a pointer to the internal constraint object.
        ///
        /// @remarks
        ///     This can return null. It will return null if SetAttachments() has not yet been called.
        ConeTwistConstraint* GetConstraint() { return this->constraint; }



    private:

        /// A pointer to the constraint. Due to the way Bullet handles construction of constraint objects, we need to make this a pointer. We are going to need
        /// to dynamically delete and re-create the constraint when the attachments are changed.
        ConeTwistConstraint* constraint;

     

        GTENGINE_DECL_COMPONENT_ATTRIBS(ConeTwistConstraintComponent)
    };
}

#endif