// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "../Component.hpp"

namespace GTEngine
{
    class ParticleSystemComponent : public Component
    {
    public:

        /// Constructor.
        ParticleSystemComponent(SceneNode &sceneNode);

        /// Destructor.
        ~ParticleSystemComponent();



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);



    private:





        GTENGINE_DECL_COMPONENT_ATTRIBS(ParticleSystemComponent)
    };
}