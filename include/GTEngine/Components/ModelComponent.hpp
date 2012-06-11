
#ifndef __GTEngine_ModelComponent_hpp_
#define __GTEngine_ModelComponent_hpp_

#include "../Component.hpp"
#include "../Model.hpp"

namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(ModelComponent)
    public:

        ModelComponent(SceneNode &node);
        ~ModelComponent();
        void SetModel(Model *model, bool takeOwnership = false);
        Model * GetModel();


        /// Sets the back/front face culling mode. By default, back faces are culled.
        ///
        /// @param cullFront [in] Specifies whether or not front faces should be drawn.
        /// @param cullBack  [in] Specifies whether or not back faces should be drawn.
        void SetFaceCulling(bool cullFront, bool cullBack);

        /// Determines whether or not front faces should be culled.
        bool CullFrontFaces() const { return this->cullFrontFaces; }

        /// Determines whether or not back faces should be culled.
        bool CullBackFaces() const { return this->cullBackFaces; }

        


        /**
        *   \brief  Marks this component as the owner of the model.
        *
        *   \remarks
        *       This will cause the destructor of this component to delete the model. Only use this if this component
        *       is the only thing using the model.
        */
        void MakeOwner();

        /**
        *   \brief  Removes ownership of the model.
        */
        void RemoveOwnership();

        
    private:

        /// A pointer to the applicable model.
        Model *model;

        /// Keeps track of whether or not to draw front faces.
        bool cullFrontFaces;

        /// Keeps track of whether or not back faces should be drawn.
        bool cullBackFaces;
        

        /// Keeps track of whether or not the component owns the model. If this is 'true', the component will destruct
        /// the model when it is destructed.
        bool isOwner;

    GTENGINE_DECL_COMPONENT_END()   
}

#endif
