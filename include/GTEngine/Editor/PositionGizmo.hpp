
#ifndef __GTEngine_PositionGizmo_hpp_
#define __GTEngine_PositionGizmo_hpp_

#include "../SceneNode.hpp"

namespace GTEngine
{
    /// Class representing the positioning gizmo in an editor.
    class PositionGizmo
    {
    public:

        /// Constructor.
        PositionGizmo();

        /// Destructor.
        ~PositionGizmo();


        /// Initialises the gizmo.
        void Initialise();



        /// Sets the position of the gizmo.
        void SetPosition(const glm::vec3 &position);

        /// Retrieves the position of the gizmo.
        const glm::vec3 & GetPosition() const;


        /// Retrieves a reference the main scene node.
              SceneNode & GetSceneNode()       { return this->sceneNode; }
        const SceneNode & GetSceneNode() const { return this->sceneNode; }

        /// Retrieves a reference to the x axis arrow scene node.
              SceneNode & GetXArrowSceneNode()       { return this->xArrowSceneNode; }
        const SceneNode & GetXArrowSceneNode() const { return this->xArrowSceneNode; }

        /// Retrieves a reference to the x axis arrow scene node.
              SceneNode & GetYArrowSceneNode()       { return this->yArrowSceneNode; }
        const SceneNode & GetYArrowSceneNode() const { return this->yArrowSceneNode; }

        /// Retrieves a reference to the x axis arrow scene node.
              SceneNode & GetZArrowSceneNode()       { return this->zArrowSceneNode; }
        const SceneNode & GetZArrowSceneNode() const { return this->zArrowSceneNode; }



    private:

        /// The main scene node.
        SceneNode sceneNode;

        /// The scene node for the x axis arrow.
        SceneNode xArrowSceneNode;

        /// The scene node for the y axis arrow.
        SceneNode yArrowSceneNode;

        /// The scene node for the z axis arrow.
        SceneNode zArrowSceneNode;
    };
}

#endif