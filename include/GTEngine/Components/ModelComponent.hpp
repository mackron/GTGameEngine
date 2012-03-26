
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

        /// Keeps track of whether or not the component owns the model. If this is 'true', the component will destruct
        /// the model when it is destructed.
        bool isOwner;

    GTENGINE_DECL_COMPONENT_END()   
}

#endif
