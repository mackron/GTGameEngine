
#ifndef __GTEngine_CollisionGroups_hpp_
#define __GTEngine_CollisionGroups_hpp_

namespace GTEngine
{
    /// The various engine level collision groups for collision detection.
    namespace CollisionGroups
    {
        static const short All  = (short)-1;            ///< Everything
        static const short None = 0;                    ///< Nothing

        static const short Picking    = (1 << 0);       ///< Allows for system level picking against models.
        static const short Occluder   = (1 << 1);       ///< Used for occlusion

        static const short Model      = (1 << 2);       ///< The collision object represents a model.
        static const short PointLight = (1 << 3);       ///< The collision object represents a point light.
        static const short SpotLight  = (1 << 4);       ///< The collision object represents a spot light.
    }
}


#endif