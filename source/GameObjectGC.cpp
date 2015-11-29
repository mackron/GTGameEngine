// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/GameObjectGC.hpp>

namespace GT
{
    GameObjectGC::GameObjectGC()
        : objects()
    {
    }

    GameObjectGC::~GameObjectGC()
    {
        this->Collect(true);    // 'true' means to force deletion regardless of counters.
    }


    void GameObjectGC::MarkForCollection(GameObject &object)
    {
        // We only mark the object if it's not already marked.
        if (this->objects.Find(&object) == nullptr)
        {
            this->objects.Append(&object);
        }
    }

    void GameObjectGC::Collect(bool force)
    {
        for (auto i = this->objects.root; i != nullptr; )
        {
            auto next = i->next;

            auto object = i->value;
            assert(object != nullptr);

            if (force || object->GetGCCounter() == 0)
            {
                delete object;
                this->objects.Remove(i);
            }

            i = next;
        }
    }
}