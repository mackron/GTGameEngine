// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);


    GTENGINE_DECL_COMPONENT_END()
}

#endif