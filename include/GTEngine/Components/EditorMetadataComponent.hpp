
#ifndef __GTEngine_EditorMetadataComponent_hpp_
#define __GTEngine_EditorMetadataComponent_hpp_

#include "../Component.hpp"
#include "../Math.hpp"
#include "../Physics.hpp"

namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(EditorMetadataComponent)
    public:

        /// Constructor.
        EditorMetadataComponent(SceneNode &node);

        /// Destructor.
        ~EditorMetadataComponent();


        /// Determines whether or not the scene node is marked as selected.
        bool IsSelected() const;

        /// Marks the scene node as selected.
        void Select();

        /// Marks the scene node as deselected.
        void Deselect();


        /// Retrieves the selection wireframe colour.
        const glm::vec3 & GetSelectionWireframeColour() const;

        /// Sets the selection wireframe colour.
        void SetSelectionWireframeColour(const glm::vec3 &newColour);


        /// Retrieves a reference to the collision object to use for picking.
              CollisionObject & GetPickingCollisionObject()       { return this->pickingCollisionObject; }
        const CollisionObject & GetPickingCollisionObject() const { return this->pickingCollisionObject; }

        /// Retrieves a pointer to the collision shape to use for picking. Returns null if there is no collision shape.
              btCollisionShape * GetPickingCollisionShape()       { return this->pickingCollisionShape; }
        const btCollisionShape * GetPickingCollisionShape() const { return this->pickingCollisionShape; }


        /// Sets the picking collision shape to that of the model geometry.
        void SetPickingCollisionShapeToModel();



    private:

        /// Keeps track of whether or not the scene node is selected in the editor.
        bool isSelected;

        /// The colour of the wireframe to use when an object is selected.
        glm::vec3 selectionWireframeColour;

        /// The collision object to use for picking.
        CollisionObject pickingCollisionObject;

        /// The collision shape to use for picking. Need to use a pointer here because we might not always use the same kind of shape for everything.
        btCollisionShape* pickingCollisionShape;


    GTENGINE_DECL_COMPONENT_END()
}

#endif