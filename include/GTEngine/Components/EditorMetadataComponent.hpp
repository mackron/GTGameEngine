
#ifndef __GTEngine_EditorMetadataComponent_hpp_
#define __GTEngine_EditorMetadataComponent_hpp_

#include "../Component.hpp"

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



    private:

        /// Keeps track of whether or not the scene node is selected in the editor.
        bool isSelected;


    GTENGINE_DECL_COMPONENT_END()
}

#endif