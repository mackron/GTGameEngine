// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Utilities_CharacterControllerEventHandler
#define GT_Utilities_CharacterControllerEventHandler

namespace GT
{
    /// Event handler for character controllers.
    class CharacterControllerEventHandler
    {
    public:
        
        /// Warning silencing destructor.
        virtual ~CharacterControllerEventHandler() {}

        /// Called when the character is updated by the scene. This is called after the controller applies gravity and movement.
        virtual void OnUpdate(double deltaTimeInSeconds) { (void) deltaTimeInSeconds; }

        /// Called when the character lands on the ground.
        virtual void OnLand() {}

        /// Called when the character is raised off the ground, either from jumping or the ground being removed.
        virtual void OnRaise() {}

        /// Called when the character hits it's head.
        virtual void OnHitHead() {}
    };
}

#endif
