
#ifndef __GTEngine_ScriptComponent_hpp_
#define __GTEngine_ScriptComponent_hpp_

#include "../Component.hpp"

namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(ScriptComponent)
    public:

        /// Constructor.
        ScriptComponent(SceneNode &node);

        /// Destructor.
        ~ScriptComponent();



    GTENGINE_DECL_COMPONENT_END()
}

#endif