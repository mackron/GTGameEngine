// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Audio/SoundWorld.hpp>
#include <GTEngine/Assets/SoundAsset.hpp>
#include <GTEngine/EngineContext.hpp>

#include "../Audio/Streamers/SoundStreamer_WAV.hpp"
#include "../Audio/Streamers/SoundStreamer_Vorbis.hpp"

namespace GT
{
    namespace Engine
    {
        struct EA_SoundData
        {
            // A pointer to the engine context.
            GT::Engine::EngineContext* pEngineContext;

            // The asset that was used to create the streamer.
            GT::Asset* pAsset;

            // A pointer to the streamer.
            GTEngine::SoundStreamer* pStreamer;
        };

        static void EA_OnSoundDelete(easyaudio_sound* pSound)
        {
            // The streamer needs to be closed.
            EA_SoundData* pSoundData = reinterpret_cast<EA_SoundData*>(easyaudio_get_sound_extra_data(pSound));
            assert(pSoundData != NULL);

            GTEngine::SoundStreamer::Delete(pSoundData->pStreamer);
            pSoundData->pStreamer = NULL;

            pSoundData->pEngineContext->GetAssetLibrary().Unload(pSoundData->pAsset);
            pSoundData->pAsset = NULL;
        }

        static easyaudio_bool EA_OnSoundRead(easyaudio_sound* pSound, void* pDataOut, unsigned int bytesToRead, unsigned int* bytesReadOut)
        {
            EA_SoundData* pSoundData = reinterpret_cast<EA_SoundData*>(easyaudio_get_sound_extra_data(pSound));
            assert(pSoundData != NULL);

            if (pSoundData->pStreamer != NULL) {
                return pSoundData->pStreamer->Read(pDataOut, bytesToRead, bytesReadOut);
            }

            return false;
        }

        static easyaudio_bool EA_OnSoundSeek(easyaudio_sound* pSound, unsigned int offsetInBytesFromStart)
        {
            EA_SoundData* pSoundData = reinterpret_cast<EA_SoundData*>(easyaudio_get_sound_extra_data(pSound));
            assert(pSoundData != NULL);

            if (pSoundData->pStreamer != NULL) {
                return pSoundData->pStreamer->Seek(offsetInBytesFromStart);
            }

            return false;
        }



        SoundWorld::SoundWorld(GT::Engine::EngineContext &engineContext)
            : m_engineContext(engineContext),
              m_pWorld(nullptr)
        {
            
        }

        SoundWorld::~SoundWorld()
        {
        }


        bool SoundWorld::Startup()
        {
            m_pWorld = easyaudio_create_world(m_engineContext.GetAudioPlaybackDevice());
            if (m_pWorld != nullptr)
            {
                return true;
            }

            return false;
        }

        void SoundWorld::Shutdown()
        {
            easyaudio_delete_world(m_pWorld);
            m_pWorld = nullptr;
        }


        bool SoundWorld::PlaySound(const char* filePath, const glm::vec3 &position, bool relative)
        {
            GT::Asset* pAsset = m_engineContext.GetAssetLibrary().Load(filePath);
            if (pAsset != nullptr)
            {
                // TODO: Don't use streaming for tiny sounds.

                GTEngine::SoundStreamer* pStreamer = GTEngine::SoundStreamer::CreateFromAsset(pAsset);
                if (pStreamer != nullptr && pStreamer->Initialize())
                {
                    EA_SoundData extraData;
                    extraData.pEngineContext = &m_engineContext;
                    extraData.pAsset         = pAsset;
                    extraData.pStreamer      = pStreamer;

                    easyaudio_sound_desc desc;
                    desc.flags         = 0;
                    desc.format        = pStreamer->GetFormat();
                    desc.channels      = pStreamer->GetNumChannels();
                    desc.sampleRate    = pStreamer->GetSampleRate();
                    desc.bitsPerSample = pStreamer->GetBitsPerSample();
                    desc.sizeInBytes   = 0;
                    desc.pData         = nullptr;
                    desc.onDelete      = EA_OnSoundDelete;
                    desc.onRead        = EA_OnSoundRead;
                    desc.onSeek        = EA_OnSoundSeek;
                    desc.extraDataSize = sizeof(EA_SoundData);
                    desc.pExtraData    = &extraData;
                    if (desc.channels == 1) {
                        desc.flags = EASYAUDIO_ENABLE_3D;

                        if (relative) {
                            desc.flags = EASYAUDIO_RELATIVE_3D;
                        }
                    }

                    easyaudio_play_inline_sound_3f(m_pWorld, desc, position.x, position.y, position.z);

                    return true;
                }


                // If we get here it means we an error occured and we need to fall through and unload the asset and return false.
                m_engineContext.GetAssetLibrary().Unload(pAsset);
            }

            return false;
        }

        void SoundWorld::StopAllSounds()
        {
            easyaudio_stop_all_sounds(m_pWorld);
        }
    }
}