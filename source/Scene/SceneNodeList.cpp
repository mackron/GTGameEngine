// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeList.hpp>
#include <GTLib/Math.hpp>

namespace GT
{
    ///////////////////////////////////
    // SceneNodeListChunk

    SceneNodeListChunk::SceneNodeListChunk()
        : m_sceneNodes(),
          m_bitfield0(0)
    {
    }

    
    SceneNode* SceneNodeListChunk::CreateAndInsertSceneNode(uint64_t sceneNodeID, uint32_t &indexOut)
    {
        if (m_bitfield0 != 0xFFFFFFFFFFFFFFFF)
        {
            int newIndex = GTLib::BitScanForward(~m_bitfield0);
            assert(newIndex < 64);
            {
                // Set the bitfield value to 1 so that we know this is a valid scene node. This will ensure the slot is not reused.
                m_bitfield0 |= (uint64_t(1) << uint64_t(newIndex));

                indexOut = static_cast<uint32_t>(newIndex);
                return new (m_sceneNodes + newIndex) SceneNode(sceneNodeID);
            }
        }
        else
        {
            // No room.
            return nullptr;
        }
    }

    void SceneNodeListChunk::DeleteSceneNodeByIndex(uint32_t index)
    {
        assert(index < 64);
        {
            // Call the destructor, but do not deallocate any memory.
            (m_sceneNodes + index)->~SceneNode();

            // Set the bitfield value to 0 so we know that this is no longer a valid scene node. This will allow this slot to be reused.
            m_bitfield0 &= ~(1 << index);
        }
    }


    SceneNode* SceneNodeListChunk::GetSceneNodeByIndex(uint32_t index)
    {
        assert(index < 64);
        {
            if ((m_bitfield0 & (uint64_t(1) << uint64_t(index))) != 0)
            {
                return &m_sceneNodes[index];
            }
            else
            {
                // Not a valid scene node. Either deleted or not yet allocated.
                return nullptr;
            }
        }
    }




    ///////////////////////////////////
    // SceneNodeList

    SceneNodeList::SceneNodeList()
        : m_chunks(),
          m_count(0)
    {
    }

    SceneNodeList::~SceneNodeList()
    {
        for (size_t iChunk = 0; iChunk < m_chunks.GetCount(); ++iChunk)
        {
            delete m_chunks[iChunk];
        }
    }


    SceneNode* SceneNodeList::CreateAndInsertSceneNode(uint64_t sceneNodeID, uint64_t &indexOut)
    {
        // Keep trying to insert into a chunk until we find one. If we don't find one, we'll need to create a new chunk and insert into that. It is
        // more likely that chunks at the end will have an available space, so we will iterate backwards.
        if (m_count < m_chunks.GetCount()*64)
        {
            for (size_t iChunk = m_chunks.GetCount(); iChunk > 0; --iChunk)
            {
                auto pSceneNode = this->CreateAndInsertSceneNodeInChunk(iChunk - 1, sceneNodeID, indexOut);
                if (pSceneNode != nullptr)
                {
                    return pSceneNode;
                }
            }
        }
        


        // If we get here it means there were not chunks with any available space, so we need to create a new one.
        m_chunks.PushBack(new SceneNodeListChunk());
        assert(m_chunks.GetCount() > 0);
        {
            return this->CreateAndInsertSceneNodeInChunk(m_chunks.GetCount() - 1, sceneNodeID, indexOut);
        }
    }

    void SceneNodeList::DeleteSceneNodeByIndex(uint64_t index)
    {
        uint32_t chunkIndex = uint32_t(index >> 32);
        uint32_t localIndex = uint32_t(index & 0x00000000FFFFFFFF);

        if (chunkIndex < m_chunks.GetCount())
        {
            m_chunks[chunkIndex]->DeleteSceneNodeByIndex(localIndex);
            m_count -= 1;
        }
    }

    SceneNode* SceneNodeList::GetSceneNodeByIndex(uint64_t index)
    {
        uint32_t chunkIndex = uint32_t(index >> 32);
        uint32_t localIndex = uint32_t(index & 0x00000000FFFFFFFF);

        if (chunkIndex < m_chunks.GetCount())
        {
            return m_chunks[chunkIndex]->GetSceneNodeByIndex(localIndex);
        }
        else
        {
            return nullptr;
        }
    }



    ////////////////////////////////////////
    // Private

    SceneNode* SceneNodeList::CreateAndInsertSceneNodeInChunk(size_t chunkIndex, uint64_t sceneNodeID, uint64_t &indexOut)
    {
        assert(chunkIndex < m_chunks.GetCount());
        {
            auto pChunk = m_chunks[chunkIndex];
            assert(pChunk != nullptr);
            {
                uint32_t localIndex;
                auto pSceneNode = pChunk->CreateAndInsertSceneNode(sceneNodeID, localIndex);
                if (pSceneNode != nullptr)
                {
                    m_count += 1;

                    indexOut = (uint64_t(chunkIndex) << 32) | uint64_t(localIndex);
                    return pSceneNode;
                }
                else
                {
                    return nullptr;
                }
            }
        }
    }
}
