// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_GenericConstraintComponent
#define GT_GenericConstraintComponent

#include "../Component.hpp"
#include "../Physics.hpp"

namespace GT
{
    /// Component class for a generic 6 DOF phsics constraint.
    class GenericConstraintComponent : public Component
    {
    public:

        /// Constructor.
        GenericConstraintComponent(SceneNode &node);

        /// Destructor.
        ~GenericConstraintComponent();


        /// Sets the bodies that are attached to the constraint.
        ///
        /// @param sceneNodeA [in] A reference to the first scene node to attach to the constraint.
        /// @param sceneNodeB [in] A reference to the second scene node to attach to the constraint.
        /// @param frameA     [in] The transformation frame for objectA.
        /// @param frameB     [in] The transformation frame for objectB.
        void SetAttachments(SceneNode &sceneNodeA, SceneNode &sceneNodeB, const glm::mat4 &frameA, const glm::mat4 &frameB);
        void SetAttachments(SceneNode &sceneNodeB, const glm::mat4 &frameB);


        /// Sets the lower linear limit.
        void SetLinearLowerLimit(const glm::vec3 &limit);

        /// Sets the upper linear limit.
        void SetLinearUpperLimit(const glm::vec3 &limit);

        /// Sets the lower angular limit.
        void SetAngularLowerLimit(const glm::vec3 &limit);

        /// Sets the upper angular limit.
        void SetAngularUpperLimit(const glm::vec3 &limit);


        /// Retrieves a pointer to the internal constraint object.
        ///
        /// @remarks
        ///     This can return null. It will return null if SetAttachments() has not yet been called.
        GenericConstraint* GetConstraint() { return this->constraint; }



    private:

        /// A pointer to the constraint. Due to the way Bullet handles construction of constraint objects, we need to make this a point. We are going to need
        /// to dynamically delete and re-create the constraint when the attachments are changed.
        GenericConstraint* constraint;

      

        GTENGINE_DECL_COMPONENT_ATTRIBS(GenericConstraintComponent)
    };
}

#endif
