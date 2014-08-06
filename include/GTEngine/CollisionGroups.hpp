// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_CollisionGroups_hpp_
#define __GTEngine_CollisionGroups_hpp_

namespace GTEngine
{
    /// The various engine level collision groups for collision detection. These should not be used for any gameplay related collision detection.
    namespace CollisionGroups
    {
        static const short All            = -1;             ///< Everything
        static const short Nothing        = 0;              ///< Nothing

        static const short Picking        = (1 << 0);       ///< Allows for system level picking against models.
        static const short Occluder       = (1 << 1);       ///< Used for occlusion

        static const short Model          = (1 << 2);       ///< The collision object represents a model.
        static const short PointLight     = (1 << 3);       ///< The collision object represents a point light.
        static const short SpotLight      = (1 << 4);       ///< The collision object represents a spot light.
        static const short ParticleSystem = (1 << 5);       ///< The collision object represents a particle system.


        static const short EditorInsertionPointPlane = (1 << 11);
        static const short EditorGizmo               = (1 << 12);
        static const short EditorSelectionVolume     = (1 << 13);
        static const short EditorSelectionRay        = (1 << 14);
    }
}


#endif
