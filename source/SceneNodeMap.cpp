// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneNodeMap.hpp>

namespace GTEngine
{
    SceneNodeMap::SceneNodeMap()
        : buffer(nullptr),
          bufferSize(0),
          count(0)
    {
    }

    SceneNodeMap::SceneNodeMap(const SceneNodeMap &other)
        : buffer(static_cast<SceneNode**>(malloc(other.count * sizeof(SceneNode**)))),
          bufferSize(other.count),
          count(other.count)
    {
        memcpy(this->buffer, other.buffer, other.GetCount() * sizeof(SceneNode*));
    }

    SceneNodeMap::~SceneNodeMap()
    {
        this->Clear();
    }


    void SceneNodeMap::Insert(SceneNode &sceneNode)
    {
        size_t index;
        auto existingNode = this->FindByID(sceneNode.GetID(), index);
        if (existingNode == nullptr)
        {
            // Value needs to be inserted.

            // If we don't have enough room to fit the item, we need a new buffer. Otherwise, we move everything
            // after the insertion index down a position and just set the value directly.
            ++this->count;
            if (this->count > this->bufferSize)
            {
                // Need a new buffer...
                this->bufferSize = GTCore::Max(this->bufferSize * 2, static_cast<size_t>(1));
                auto newbuffer = static_cast<SceneNode**>(malloc(sizeof(SceneNode*) * this->bufferSize));

                // Copying over is done in three parts...
                // Part 1: The part before the insertion point.
                size_t i = 0;
                for ( ; i < index; ++i)
                {
                    newbuffer[i] = this->buffer[i];
                }

                // Part 2: The new item.
                newbuffer[i] = &sceneNode;
                ++i;

                // Part 3: The part after the insertion point.
                for ( ; i < this->count; ++i)
                {
                    newbuffer[i] = this->buffer[i - 1];
                }

                free(this->buffer);
                this->buffer = newbuffer;
            }
            else
            {
                // Don't need a new buffer...

                // Inserting is done in two parts...
                // Part 1: Move down a position starting at the insertion point.
                for (size_t i = this->count - 1; i > index; --i)
                {
                    this->buffer[i] = this->buffer[i - 1];
                }

                // Part 2: Apply the new item.
                this->buffer[index] = &sceneNode;
            }
        }
        else
        {
            // Value needs to be replaced.
            this->buffer[index] = &sceneNode;
        }
    }

    void SceneNodeMap::Remove(SceneNode &sceneNode)
    {
        this->Remove(sceneNode.GetID());
    }

    void SceneNodeMap::Remove(uint64_t sceneNodeID)
    {
        size_t index;
        auto sceneNode = this->FindByID(sceneNodeID, index);
        if (sceneNode != nullptr)
        {
            this->RemoveByIndex(index);
        }
    }

    void SceneNodeMap::RemoveByIndex(size_t index)
    {
        if (index < this->count)
        {
            // We just move everything after it down one spot.
            for (uint64_t i = index; i < this->count - 1; ++i)
            {
                this->buffer[i] = this->buffer[i + 1];
            }

            --this->count;
        }
    }


    SceneNode* SceneNodeMap::FindByID(uint64_t sceneNodeID, size_t &index) const
    {
        if (this->count > 0)
        {
            // It's just so much easier using signed values here...
            intptr_t low  = 0;
            intptr_t high = static_cast<intptr_t>(this->count - 1);   // <-- Safe cast as per the the check above.
            intptr_t mid  = 0;

            intptr_t offset = 0; // This will either be 0 or 1 and is added to the index if we don't find an exact value.
            while (low <= high)
            {
                mid = (low + high) / 2;    // This will be floored.

                auto miditem = this->buffer[mid];
                if (sceneNodeID < miditem->GetID())
                {
                    high   = mid - 1;
                    offset = 0;
                }
                else if (sceneNodeID > miditem->GetID())
                {
                    low    = mid + 1;
                    offset = 1;
                }
                else
                {
                    index = static_cast<size_t>(mid);
                    return this->buffer[mid];
                }
            }

            // If we have made it here, we weren't exact.
            index = static_cast<size_t>(mid + offset);
            return nullptr;
        }

        index = 0;
        return nullptr;
    }

    SceneNode* SceneNodeMap::FindByID(uint64_t sceneNodeID) const
    {
        size_t devnull;
        return this->FindByID(sceneNodeID, devnull);
    }


    bool SceneNodeMap::Exists(uint64_t sceneNodeID) const
    {
        return this->FindByID(sceneNodeID) != nullptr;
    }


    size_t SceneNodeMap::GetCount() const
    {
        return this->count;
    }

    SceneNode* SceneNodeMap::GetSceneNodeAtIndex(size_t index)
    {
        assert(index < this->count);
        {
            return this->buffer[index];
        }
    }

    const SceneNode* SceneNodeMap::GetSceneNodeAtIndex(size_t index) const
    {
        assert(index < this->count);
        {
            return this->buffer[index];
        }
    }


    void SceneNodeMap::Clear()
    {
        free(this->buffer);
        this->buffer     = nullptr;
        this->bufferSize = 0;
        this->count      = 0;
    }


    SceneNodeMap & SceneNodeMap::operator=(const SceneNodeMap &other)
    {
        if (this->bufferSize < other.count)
        {
            free(this->buffer);
            this->buffer     = static_cast<SceneNode**>(malloc(this->bufferSize * sizeof(SceneNode*)));
            this->bufferSize = other.count;
        }

        memcpy(this->buffer, other.buffer, other.count * sizeof(SceneNode*));
        this->count = other.count;

        return *this;
    }
}
