// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Engine_SoundPlaybackEventHandler_hpp_
#define __GT_Engine_SoundPlaybackEventHandler_hpp_

namespace GT
{
    namespace Engine
    {
        /// Base class for allowing things to handle playback events of sounds.
        ///
        /// The idea for this class is to allow external classes to know when the state of a sound's playback has changed so it
        /// can handle things correctly. For example, when a sound is stopped, some part of the engine may want to trigger an
        /// event or whatnot.
        class SoundPlaybackEventHandler
        {
        public:

            /// Constructor.
            SoundPlaybackEventHandler();

            /// Destructor.
            virtual ~SoundPlaybackEventHandler();



            /// Called when the sound begins playing.
            virtual void OnStart();

            /// Called when the sound stops playing.
            virtual void OnStop();

            /// Called when the sound is resumed from a paused state.
            virtual void OnResume();
            
            /// Called when the sound is paused from a playing state.
            virtual void OnPause();
        };
    }
}

#endif // !__GT_Engine_SoundPlaybackEventHandler_hpp_
