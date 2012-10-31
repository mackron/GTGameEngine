
#include <GTEngine/GarbageCollector.hpp>
#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/Framebuffer.hpp>
#include <GTEngine/Rendering/VertexArray.hpp>
#include <GTEngine/Bone.hpp>
#include <GTCore/List.hpp>

// GC globals.
namespace GTEngine
{
    /// Structure representing an item that is marked for collection.
    template <typename T>
    struct GCItem
    {
        GCItem(T* object, int counter)
            : object(object), counter(counter)
        {
        }

        T*  object;
        int counter;
    };

    static GTCore::List<GCItem<Texture2D>>   GarbageTexture2Ds;
    static GTCore::List<GCItem<Shader>>      GarbageShaders;
    static GTCore::List<GCItem<Framebuffer>> GarbageFramebuffers;
    static GTCore::List<GCItem<VertexArray>> GarbageVertexArrays;
    static GTCore::List<GCItem<Bone>>        GarbageBones;
}

namespace GTEngine
{
    bool GarbageCollector::Startup()
    {
        return true;
    }

    void GarbageCollector::Shutdown()
    {
        while (GarbageTexture2Ds.root != nullptr)
        {
            delete GarbageTexture2Ds.root->value.object;
            GarbageTexture2Ds.RemoveRoot();
        }

        while (GarbageShaders.root != nullptr)
        {
            delete GarbageShaders.root->value.object;
            GarbageShaders.RemoveRoot();
        }

        while (GarbageFramebuffers.root != nullptr)
        {
            delete GarbageFramebuffers.root->value.object;
            GarbageFramebuffers.RemoveRoot();
        }

        while (GarbageVertexArrays.root != nullptr)
        {
            delete GarbageVertexArrays.root->value.object;
            GarbageVertexArrays.RemoveRoot();
        }

        while (GarbageBones.root != nullptr)
        {
            delete GarbageBones.root->value.object;
            GarbageBones.RemoveRoot();
        }
    }


    void GarbageCollector::MarkForCollection(Texture2D &texture, int counter)
    {
        GarbageTexture2Ds.Append(GCItem<Texture2D>(&texture, counter));
    }

    void GarbageCollector::MarkForCollection(Shader &shader, int counter)
    {
        GarbageShaders.Append(GCItem<Shader>(&shader, counter));
    }

    void GarbageCollector::MarkForCollection(Framebuffer &framebuffer, int counter)
    {
        GarbageFramebuffers.Append(GCItem<Framebuffer>(&framebuffer, counter));
    }

    void GarbageCollector::MarkForCollection(VertexArray &va, int counter)
    {
        GarbageVertexArrays.Append(GCItem<VertexArray>(&va, counter));
    }

    void GarbageCollector::MarkForCollection(Bone &bone, int counter)
    {
        GarbageBones.Append(GCItem<Bone>(&bone, counter));
    }


    void GarbageCollector::CollectTexture2Ds()
    {
        for (auto i = GarbageTexture2Ds.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0)
            {
                delete gcitem.object;

                auto next = i->next;
                GarbageTexture2Ds.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }
    }

    void GarbageCollector::CollectShaders()
    {
        for (auto i = GarbageShaders.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0)
            {
                delete gcitem.object;

                auto next = i->next;
                GarbageShaders.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }
    }

    void GarbageCollector::CollectFramebuffers()
    {
        for (auto i = GarbageFramebuffers.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0)
            {
                delete gcitem.object;

                auto next = i->next;
                GarbageFramebuffers.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }
    }

    void GarbageCollector::CollectVertexArrays()
    {
        for (auto i = GarbageVertexArrays.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0)
            {
                delete gcitem.object;

                auto next = i->next;
                GarbageVertexArrays.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }
    }

    void GarbageCollector::CollectBones()
    {
        for (auto i = GarbageBones.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0)
            {
                delete gcitem.object;

                auto next = i->next;
                GarbageBones.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }
    }
}
