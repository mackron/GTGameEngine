
#include <GTEngine/DataFilesWatcher.hpp>

namespace GTEngine
{
    /// This function will be called by the watcher's thread.
    void DataFilesWatcher_CheckForChanges_Async(void* self)
    {
        if (self != nullptr)
        {
            static_cast<DataFilesWatcher*>(self)->__CheckForChangesOnCallingThread();
        }
    }


    DataFilesWatcher::DataFilesWatcher()
        : root(), thread(),
          eventHandlers(), events()
    {
    }

    DataFilesWatcher::~DataFilesWatcher()
    {
    }

    void DataFilesWatcher::AddRootDirectory(const char* directory)
    {
        this->root.InsertChild(GTCore::IO::FileInfo(directory));
    }

    void DataFilesWatcher::RemoveRootDirectory(const char* directory)
    {
        this->root.RemoveChild(directory);
    }


    void DataFilesWatcher::AddEventHandler(EventHandler &eventHandler)
    {
        if (!this->eventHandlers.Exists(&eventHandler))
        {
            this->eventHandlers.Append(&eventHandler);
        }
    }

    void DataFilesWatcher::RemoveEventHandler(EventHandler &eventHandler)
    {
        this->eventHandlers.Remove(this->eventHandlers.Find(&eventHandler));
    }


    bool DataFilesWatcher::CheckForChanges(bool asynchronous)
    {
        if (!this->EventsReady())
        {
            return false;
        }

        if (asynchronous)
        {
            this->thread.Start(DataFilesWatcher_CheckForChanges_Async, this, true);
        }
        else
        {
            this->__CheckForChangesOnCallingThread();
        }

        return true;
    }

    bool DataFilesWatcher::DispatchEvents(bool wait)
    {
        if (wait)
        {
            this->WaitForEvents();
        }
        else
        {
            if (this->thread.Busy())
            {
                return false;
            }
        }


        while (this->events.root != nullptr)
        {
            auto e = this->events.root->value;
            assert(e.item != nullptr);

            for (auto iEventHandler = this->eventHandlers.root; iEventHandler != nullptr; iEventHandler = iEventHandler->next)
            {
                auto handler = iEventHandler->value;
                assert(handler != nullptr);

                switch (e.type)
                {
                case 0: handler->OnInsert(*e.item); break;
                case 1: handler->OnRemove(*e.item); break;
                case 2: handler->OnUpdate(*e.item); break;

                default: break;
                }
            }

            this->events.RemoveRoot();
        }

        return true;
    }

    bool DataFilesWatcher::EventsReady()
    {
        return !this->thread.Busy();
    }

    void DataFilesWatcher::WaitForEvents()
    {
        this->thread.Wait();
    }


    void DataFilesWatcher::__CheckForChangesOnCallingThread()
    {
        // We need to recursively check directories.
        this->__CheckForChangesOnCallingThread(this->root);
    }

    void DataFilesWatcher::__CheckForChangesOnCallingThread(Item &root)
    {
        // We first check ourselves for changes. 'root' will be the old info.
        GTCore::IO::FileInfo newInfo;
        GTCore::IO::GetFileInfo(root.info.absolutePath.c_str(), newInfo);

        if (newInfo.lastModifiedTime != root.info.lastModifiedTime)
        {
            Event e;
            e.type = 2;
            e.item = &root;

            this->events.Append(e);
        }


        
        // Now we need to check the children for modifications. We're going to build a new map of children and then
        // compare that to the old one.

        /*
        GTCore::List<GTCore::String> newChildren;

        for (size_t i = 0; i < this
        */
    }
}

