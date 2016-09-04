// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Audio/SoundWorld.hpp>
#include <GTGE/Audio/SoundStreamer.hpp>
#include <GTGE/Assets/SoundAsset.hpp>
#include <GTGE/Context.hpp>

#undef PlaySound

namespace GT
{
    struct EA_SoundData
    {
        // A pointer to the engine context.
        GT::Context* pContext;

        // The asset that was used to create the streamer.
        GT::Asset* pAsset;

        // A pointer to the streamer.
        SoundStreamer* pStreamer;
    };

    static void EA_OnSoundDelete(dra_sound* pSound)
    {
        // The streamer needs to be closed.
        EA_SoundData* pSoundData = reinterpret_cast<EA_SoundData*>(pSound->pUserData);
        assert(pSoundData != NULL);

        SoundStreamer::Delete(pSoundData->pStreamer);
        pSoundData->pStreamer = NULL;

        pSoundData->pContext->GetAssetLibrary().Unload(pSoundData->pAsset);
        pSoundData->pAsset = NULL;

        delete pSoundData;
    }

    static uint64_t EA_OnSoundRead(dra_sound* pSound, uint64_t samplesToRead, void* pSamplesOut)
    {
        EA_SoundData* pSoundData = reinterpret_cast<EA_SoundData*>(pSound->pUserData);
        assert(pSoundData != NULL);

        if (pSoundData->pStreamer != NULL) {
            return pSoundData->pStreamer->Read(samplesToRead, pSamplesOut);
        }

        return 0;
    }

    static bool EA_OnSoundSeek(dra_sound* pSound, uint64_t sample)
    {
        EA_SoundData* pSoundData = reinterpret_cast<EA_SoundData*>(pSound->pUserData);
        assert(pSoundData != NULL);

        if (pSoundData->pStreamer != NULL) {
            return pSoundData->pStreamer->Seek(sample);
        }

        return false;
    }



    SoundWorld::SoundWorld(GT::Context &engineContext)
        : m_engineContext(engineContext),
          m_pWorld(nullptr)
    {
            
    }

    SoundWorld::~SoundWorld()
    {
    }


    bool SoundWorld::Startup()
    {
        m_pWorld = dra_sound_world_create(m_engineContext.GetAudioPlaybackDevice());
        if (m_pWorld != nullptr)
        {
            return true;
        }

        return false;
    }

    void SoundWorld::Shutdown()
    {
        dra_sound_world_delete(m_pWorld);
        m_pWorld = nullptr;
    }


    bool SoundWorld::PlaySound(const char* filePath, const glm::vec3 &position, bool relative)
    {
        GT::Asset* pAsset = m_engineContext.GetAssetLibrary().Load(filePath);
        if (pAsset != nullptr)
        {
            // TODO: Don't use streaming for tiny sounds.

            SoundStreamer* pStreamer = SoundStreamer::CreateFromAsset(pAsset);
            if (pStreamer != nullptr && pStreamer->Initialize())
            {
                EA_SoundData* pSoundData = new EA_SoundData;
                pSoundData->pContext  = &m_engineContext;
                pSoundData->pAsset    = pAsset;
                pSoundData->pStreamer = pStreamer;

                dra_sound_desc desc;
                desc.format     = pStreamer->GetFormat();
                desc.channels   = pStreamer->GetNumChannels();
                desc.sampleRate = pStreamer->GetSampleRate();
                desc.dataSize   = 0;
                desc.pData      = nullptr;
                desc.onDelete   = EA_OnSoundDelete;
                desc.onRead     = EA_OnSoundRead;
                desc.onSeek     = EA_OnSoundSeek;
                desc.pUserData  = pSoundData;

                (void)relative;
                dra_sound_world_play_inline_3f(m_pWorld, &desc, NULL, position.x, position.y, position.z);

                return true;
            }


            // If we get here it means we an error occured and we need to fall through and unload the asset and return false.
            m_engineContext.GetAssetLibrary().Unload(pAsset);
        }

        return false;
    }

    void SoundWorld::StopAllSounds()
    {
        dra_sound_world_stop_all_sounds(m_pWorld);
    }


    void SoundWorld::SetListenerPosition(float xPos, float yPos, float zPos)
    {
        dra_sound_world_set_listener_position(m_pWorld, xPos, yPos, zPos);
    }

    void SoundWorld::SetListenerOrientation(float xForward, float yForward, float zForward, float xUp, float yUp, float zUp)
    {
        dra_sound_world_set_listener_orientation(m_pWorld, xForward, yForward, zForward, xUp, yUp, zUp);
    }
}