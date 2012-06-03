
#include <GTEngine/GameObjectGC.hpp>

namespace GTEngine
{
    GameObjectGC::GameObjectGC()
        : objects()
    {
    }

    GameObjectGC::~GameObjectGC()
    {
    }


    void GameObjectGC::MarkForCollection(GameObject &object)
    {
        // We only mark the object if it's not already marked.
        if (this->objects.Find(&object) == nullptr)
        {
            this->objects.Append(&object);
        }
    }

    void GameObjectGC::Collect()
    {
        for (auto i = this->objects.root; i != nullptr; )
        {
            auto next = i->next;

            auto object = i->value;
            assert(object != nullptr);

            if (object->GetGCCounter() == 0)
            {
                delete object;
                this->objects.Remove(i);
            }

            i = next;
        }
    }
}