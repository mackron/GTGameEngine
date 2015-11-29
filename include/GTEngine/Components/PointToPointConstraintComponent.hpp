// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_PointToPointConstraintComponent
#define GT_PointToPointConstraintComponent

#include "../Component.hpp"
#include "../Physics.hpp"

namespace GT
{
    /// Component class for a cone twist phsics constraint.
    class PointToPointConstraintComponent : public Component
    {
    public:

        /// Constructor.
        PointToPointConstraintComponent(SceneNode &node);

        /// Destructor.
        ~PointToPointConstraintComponent();


        /// Sets the attachments.
        ///
        /// @param sceneNodeA [in] A reference to the first scene node to attach to the constraint.
        /// @param sceneNodeB [in] A reference to the second scene node to attach to the constraint.
        /// @param pivotInA   [in] The pivot location for objectA.
        /// @param pivotInB   [in] The pivot location for objectB.
        void SetAttachments(SceneNode &sceneNodeA, SceneNode &sceneNodeB, const glm::vec3 &pivotInA, const glm::vec3 &pivotInB);
        void SetAttachments(SceneNode &sceneNodeB, const glm::vec3 &pivotInB);


        /// Retrieves a pointer to the internal constraint object.
        ///
        /// @remarks
        ///     This can return null. It will return null if SetAttachments() has not yet been called.
        PointToPointConstraint* GetConstraint() { return this->constraint; }



    private:

        /// A pointer to the constraint. Due to the way Bullet handles construction of constraint objects, we need to make this a point. We are going to need
        /// to dynamically delete and re-create the constraint when the attachments are changed.
        PointToPointConstraint* constraint;

      

        GTENGINE_DECL_COMPONENT_ATTRIBS(PointToPointConstraintComponent)
    };
}

#endif