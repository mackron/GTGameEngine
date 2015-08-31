// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/Audio/SoundWorld.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    class InlineSoundPlaybackEventHandler : public SoundPlaybackEventHandler
    {
    public:

        /// Constructor.
        InlineSoundPlaybackEventHandler(SoundWorld &world, Sound &sound)
            : m_world(world),
                m_sound(sound)
        {
        }


        /// SoundPlaybackEventHandler::OnStop().
        void OnStop() override
        {
            m_world._OnInlineSoundStop(m_sound);
        }



    private:

        /// A reference to the world that contains the sound.
        SoundWorld &m_world;

        /// A reference to the applicable sound.
        Sound &m_sound;


    private:    // No copying.
        InlineSoundPlaybackEventHandler(const InlineSoundPlaybackEventHandler &);
        InlineSoundPlaybackEventHandler & operator=(const InlineSoundPlaybackEventHandler &);
    };




    SoundWorld::SoundWorld(EngineContext &engineContext)
        : m_engineContext(engineContext),
          m_sounds(),
          m_inlineSounds()
    {
    }

    SoundWorld::~SoundWorld()
    {
        this->StopAllSounds();


        // All sounds need to be deleted and removed from the scene. The sound will be stopped during destructor.
        for (size_t iSound = 0; iSound < m_sounds.GetCount(); ++iSound)
        {
            delete m_sounds[iSound];
        }

        for (size_t iSound = 0; iSound < m_inlineSounds.GetCount(); ++iSound)
        {
            // The event handler needs to be deleted. This is the only class that will know about this particular Sound instance so we
            // can safely assert that there is only a single event handler attached, and that ours is at index 0.
            auto sound = m_inlineSounds[iSound];
            assert(sound != nullptr);
            {
                GT::SoundPlaybackEventHandler* playbackEventHandler = nullptr;

                assert(sound->GetPlaybackEventHandlerCount() == 1);
                {
                    playbackEventHandler = &sound->GetPlaybackEventHandlerAtIndex(0);
                    assert(playbackEventHandler != nullptr);
                    {
                        sound->RemovePlaybackEventHandlerAtIndex(0);
                    }
                }

                delete m_inlineSounds[iSound];
                delete playbackEventHandler;
            }
        }
    }


    void SoundWorld::AddSound(Sound &sound)
    {
        (void)sound;
    }

    void SoundWorld::RemoveSound(Sound &sound)
    {
        (void)sound;
    }


    bool SoundWorld::PlaySound(const char* filePath, const vec3 &position, bool relative)
    {
        auto sound = new Sound(m_engineContext);
        if (sound->LoadFromFile(filePath))
        {
            // We need an event handler here so we know that the sound needs to be deleted.
            auto eventHandler = new InlineSoundPlaybackEventHandler(*this, *sound);
            sound->AddPlaybackEventHandler(*eventHandler);

            m_inlineSounds.PushBack(sound);

            // Now we actually want to start playing the sound.
            sound->SetPosition(position);
            sound->SetIsPositionRelative(relative);
            sound->Play();

            return true;
        }
        else
        {
            // Failed to load the sound.
            delete sound;
                
            return false;
        }
    }



    void SoundWorld::StopAllSounds()
    {
        for (size_t iSound = 0; iSound < m_sounds.GetCount(); ++iSound)
        {
            auto sound = m_sounds[iSound];
            assert(sound != nullptr);
            {
                sound->Stop();
            }
        }


        // Inline sounds. For these, when the sound is stopped, it will be removed from the list, and so we need to do a different loop to
        // the one we use for normal sounds.
        while (m_inlineSounds.GetCount() > 0)
        {
            auto sound = m_inlineSounds.GetBack();
            assert(sound != nullptr);
            {
                sound->Stop();
            }
        }
    }





    ////////////////////////////////////////////////////////
    // Internal-Use-Only Methods
        
    void SoundWorld::_OnInlineSoundStop(Sound &sound)
    {
        m_inlineSounds.RemoveFirstOccuranceOf(&sound);

        // It looks bad, but we actually want to delete the input sound here.
        GT::SoundPlaybackEventHandler* playbackEventHandler = &sound.GetPlaybackEventHandlerAtIndex(0);

        delete &sound;
        delete playbackEventHandler;
    }
}