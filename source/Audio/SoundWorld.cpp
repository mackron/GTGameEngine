// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Audio/SoundWorld.hpp>
#include <GTEngine/EngineContext.hpp>

namespace GT
{
    namespace Engine
    {
        static void EA_OnStreamingBufferStop(easyaudio_buffer* pBuffer, unsigned int eventID, void *pUserData)
        {
            Sound* pSound = reinterpret_cast<Sound*>(pUserData);
            assert(pSound != NULL);

            SoundWorld* pWorld = pSound->GetWorld();
            assert(pWorld != NULL);

            // When an inline sound stops we want to delete it.
            pWorld->_UntrackAndDeleteInlineSound(*pSound);
        }



        SoundWorld::SoundWorld(GT::Engine::EngineContext &engineContext)
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

            assert(m_inlineSounds.GetCount() == 0);
        }


        void SoundWorld::AddSound(Sound &sound)
        {
            (void)sound;
        }

        void SoundWorld::RemoveSound(Sound &sound)
        {
            (void)sound;
        }


        bool SoundWorld::PlaySound(const char* filePath, const glm::vec3 &position, bool relative)
        {
            auto sound = new Sound(m_engineContext);
            if (sound->LoadFromFile(filePath))
            {
                sound->SetWorld(this);
                m_engineContext.GetAudioSystem().OnBufferStop(sound->GetAudioBuffer(), EA_OnStreamingBufferStop, sound);

                _TrackInlineSound(*sound);

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

        void SoundWorld::_TrackInlineSound(Sound &sound)
        {
            m_inlineSoundsMutex.Lock();
            {
                m_inlineSounds.PushBack(&sound);
            }
            m_inlineSoundsMutex.Unlock();
        }

        void SoundWorld::_UntrackAndDeleteInlineSound(Sound &sound)
        {
            m_inlineSoundsMutex.Lock();
            {
                m_inlineSounds.RemoveFirstOccuranceOf(&sound);
                delete &sound;
            }
            m_inlineSoundsMutex.Unlock();
        }
    }
}