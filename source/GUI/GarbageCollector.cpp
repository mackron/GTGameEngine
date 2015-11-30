// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/GarbageCollector.hpp>
#include <GTEngine/GUI/Element.hpp>
#include <GTEngine/GUI/Server.hpp>
#include <GTEngine/GUI/Rendering/GUIMesh.hpp>

namespace GTGUI
{
    GarbageCollector::GarbageCollector()
        : garbageElements(), garbageMeshes()
    {
    }
    
    GarbageCollector::~GarbageCollector()
    {
        while (this->garbageElements.root != nullptr)
        {
            delete this->garbageElements.root->value.object;
            this->garbageElements.RemoveRoot();
        }

        while (this->garbageMeshes.root != nullptr)
        {
            delete this->garbageMeshes.root->value.object;
            this->garbageMeshes.RemoveRoot();
        }
    }

    void GarbageCollector::Collect(bool force)
    {
        for (auto i = this->garbageElements.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0 || force)
            {
                delete gcitem.object;

                auto next = i->next;
                this->garbageElements.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }

        for (auto i = this->garbageMeshes.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0 || force)
            {
                delete gcitem.object;

                auto next = i->next;
                this->garbageMeshes.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }
    }

    void GarbageCollector::MarkForCollection(Element &element, int counter)
    {
        // The primary style class should be deleted immediately so that it is never referenced while the element is in the garbage.
        element.DeletePrimaryStyleClass();
        this->garbageElements.Append(GCItem<Element>(element, counter));
    }

    void GarbageCollector::MarkForCollection(GUIMesh &mesh, int counter)
    {
        this->garbageMeshes.Append(GCItem<GUIMesh>(mesh, counter));
    }
}
